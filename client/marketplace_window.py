# client/marketplace_window.py

import sys
import requests
from PyQt6 import uic
from PyQt6.QtWidgets import (
    QApplication, QWidget, QTableWidgetItem, QPushButton
)
from PyQt6.QtCore import Qt, QUrl, QTimer
from PyQt6.QtWebSockets import QWebSocket
from history_window import HistoryWindow
import json

SERVER_BASE_URL = "0.0.0.0:8000"
SERVER_WS_BASE_URL = "0.0.0.0:8000"

class MarketplaceWindow(QWidget):
    """
    Main GUI window for the Mercatoria marketplace game.

    Handles:
    - Real-time price updates and charting
    - Buying/selling products
    - Inventory and wallet display
    - Live multiplayer chat using WebSockets
    """

    def __init__(self, session_id, username):
        """
        Initialize the GUI and connect all logic.

        :param session_id: Authenticated session ID from login
        :param username: Player's username
        """
        super().__init__()
        uic.loadUi("marketplace.ui", self)

        # Mapping from product ID to product name
        self.product_name_by_id = {}

        # Store session and set up REST client
        self.session_id = session_id
        self.session = requests.Session()
        self.session.cookies.set("session_id", str(session_id))

        # Initialize local state
        self.products = []
        self.inventory = {}
        self.historyWindows = {}  # product_id → open HistoryWindow

        # Load initial data
        self.load_wallet()
        self.load_products()
        self.load_inventory()

        # Set up polling every 500ms
        self.timer = QTimer()
        self.timer.timeout.connect(self.refresh_data)
        self.timer.start(500)

        # WebSocket chat setup
        self.websocket = QWebSocket()
        self.websocket.errorOccurred.connect(self.websocket_error)
        self.websocket.connected.connect(self.websocket_connected)
        self.websocket.textMessageReceived.connect(self.receive_message)
        self.websocket.open(QUrl(f"{SERVER_WS_BASE_URL}/ws/chat"))

        # Connect send button
        self.sendButton.clicked.connect(self.send_chat_message)

        self.username = username

    def load_wallet(self):
        """Fetch and display the player's wallet balance."""
        try:
            res = self.session.get(f"{SERVER_BASE_URL}/my/wallet")
            if res.status_code == 200:
                coins = res.json()
                self.walletLabel.setText(f"Wallet: {coins:.2f} POOSE-Coins")
            else:
                self.walletLabel.setText("Wallet: Error")
        except Exception as e:
            self.walletLabel.setText(f"Wallet error: {e}")

    def load_products(self):
        """Fetch and display all products in the product table."""
        try:
            res = self.session.get(f"{SERVER_BASE_URL}/products")
            if res.status_code == 200:
                self.products = res.json()
                self.productTable.setRowCount(len(self.products))
                self.productTable.setColumnCount(4)
                self.productTable.setHorizontalHeaderLabels(["Name", "Price", "Buy", "Sell"])

                for row, product in enumerate(self.products):
                    name = product["name"]
                    price = product["current_price"]
                    pid = product["id"]
                    history = product.get("price_history", {})

                    # Column 0: Product name (clickable)
                    item = QTableWidgetItem(name)
                    item.setFlags(item.flags() ^ Qt.ItemFlag.ItemIsEditable)
                    item.setData(1000, history)  # Custom role for history
                    item.setData(1001, name)
                    self.productTable.setItem(row, 0, item)

                    # Column 1: Price
                    self.productTable.setItem(row, 1, QTableWidgetItem(f"{price:.2f}"))

                    # Column 2: Buy button
                    buy_button = QPushButton("Buy")
                    buy_button.clicked.connect(lambda _, pid=pid: self.buy_product(pid))
                    self.productTable.setCellWidget(row, 2, buy_button)

                    # Column 3: Sell button
                    sell_button = QPushButton("Sell")
                    sell_button.clicked.connect(lambda _, pid=pid: self.sell_product(pid))
                    self.productTable.setCellWidget(row, 3, sell_button)

                # Connect click handler for opening chart
                self.productTable.cellClicked.connect(self.open_history)

                # Save product name map
                self.product_name_by_id = {
                    product["id"]: product["name"]
                    for product in self.products
                }

            else:
                self.infoLabel.setText("Failed to load products")
        except Exception as e:
            self.infoLabel.setText(f"Error loading products: {e}")

    def load_inventory(self):
        """Fetch and display player's current inventory."""
        try:
            res = self.session.get(f"{SERVER_BASE_URL}/my/inventory")
            if res.status_code == 200:
                inventory = res.json()
                self.inventory = inventory
                self.inventoryTable.setRowCount(len(inventory))
                self.inventoryTable.setColumnCount(2)
                self.inventoryTable.setHorizontalHeaderLabels(["Product ID", "Amount"])

                for row, (pid, amount) in enumerate(inventory.items()):
                    pid_int = int(pid)
                    name = self.product_name_by_id.get(pid_int, f"Unknown ID {pid_int}")
                    self.inventoryTable.setItem(row, 0, QTableWidgetItem(name))
                    self.inventoryTable.setItem(row, 1, QTableWidgetItem(str(amount)))
            else:
                self.infoLabel.setText("Failed to load inventory")
        except Exception as e:
            self.infoLabel.setText(f"Inventory error: {e}")

    def buy_product(self, product_id):
        """Attempt to buy a product by ID."""
        try:
            res = self.session.post(f"{SERVER_BASE_URL}/my/product/{product_id}/buy")
            if res.status_code == 200:
                self.infoLabel.setText("Bought successfully!")
                self.load_wallet()
                self.load_inventory()
                self.load_products()
            else:
                msg = res.json().get("detail", "Unknown error")
                self.infoLabel.setText(f"Buy failed: {msg}")
        except Exception as e:
            self.infoLabel.setText(f"Buy error: {e}")

    def sell_product(self, product_id):
        """Attempt to sell a product by ID."""
        try:
            res = self.session.post(f"{SERVER_BASE_URL}/my/product/{product_id}/sell")
            if res.status_code == 200:
                self.infoLabel.setText("Sold successfully!")
                self.load_wallet()
                self.load_inventory()
                self.load_products()
            else:
                msg = res.json().get("detail", "Unknown error")
                self.infoLabel.setText(f"Sell failed: {msg}")
        except Exception as e:
            self.infoLabel.setText(f"Sell error: {e}")

    def open_history(self, row, column):
        """
        Open or update the price history chart for the clicked product.

        :param row: Row index in table
        :param column: Column index (only responds to Name column)
        """
        if column != 0:
            return

        item = self.productTable.item(row, 0)
        price_history = item.data(1000)
        name = item.data(1001)
        product_id = self.products[row]["id"]
        self.current_chart_product_id = product_id

        if product_id not in self.historyWindows or self.historyWindows[product_id] is None:
            window = HistoryWindow(price_history, name)
            self.historyWindows[product_id] = window
            window.show()
            window.destroyed.connect(lambda _, pid=product_id: self.historyWindows.pop(pid, None))
        else:
            self.historyWindows[product_id].update_chart(price_history)
            self.historyWindows[product_id].raise_()

    def refresh_data(self):
        """Poll for updated product data and update UI + charts."""
        try:
            res = self.session.get(f"{SERVER_BASE_URL}/products")
            if res.status_code == 200:
                new_products = res.json()

                for row, new_prod in enumerate(new_products):
                    current_item = self.productTable.item(row, 1)
                    new_price = new_prod["current_price"]
                    old_price = float(current_item.text())
                    if old_price != new_price:
                        current_item.setText(f"{new_price:.2f}")

                    # Update history for chart
                    name_item = self.productTable.item(row, 0)
                    name_item.setData(1000, new_prod["price_history"])

                self.products = new_products

                # Update all open charts
                for product in new_products:
                    pid = product["id"]
                    if pid in self.historyWindows and self.historyWindows[pid].isVisible():
                        self.historyWindows[pid].update_chart(product["price_history"])

        except Exception as e:
            print(f"[Polling Error] {e}")

    # === WebSocket Chat Methods ===

    def websocket_connected(self):
        """Send initial 'start' message when WebSocket connects."""
        start_msg = {
            "type": "start",
            "name": self.username
        }
        self.websocket.sendTextMessage(json.dumps(start_msg))

    def websocket_error(self, error):
        """Handle WebSocket connection errors."""
        print(f"WebSocket error: {error}")

    def send_chat_message(self):
        """Send a chat message from the input field."""
        text = self.chatInput.text().strip()
        if not text:
            return
        msg = {
            "type": "message_send",
            "text": text
        }
        self.websocket.sendTextMessage(json.dumps(msg))
        self.chatInput.clear()

    def receive_message(self, msg):
        """Handle incoming chat messages from WebSocket."""
        try:
            data = json.loads(msg)
            if data["type"] == "message":
                name = data["name"]
                text = data["text"]
                self.chatDisplay.append(f"<b>{name}:</b> {text}")
        except Exception as e:
            print("Error processing message:", e)
