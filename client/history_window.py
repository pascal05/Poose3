# client/history_window.py

import sys
from PyQt6.QtWidgets import QWidget, QVBoxLayout
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import datetime

class HistoryWindow(QWidget):
    """
    A QWidget that displays the price history of a product using a matplotlib chart.

    This window shows a line graph of price over time and supports live updates
    through the `update_chart` method. Used in the Mercatoria marketplace client.
    """

    def __init__(self, price_history: dict, product_name: str):
        """
        Initialize the chart window and plot the initial price history.

        :param price_history: Dictionary with timestamp (int) → price (float)
        :param product_name: Name of the product for display
        """
        super().__init__()
        self.setWindowTitle(f"Price History: {product_name}")

        # Create a vertical layout for the chart
        layout = QVBoxLayout()
        self.setLayout(layout)

        # Set up the matplotlib figure and canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        layout.addWidget(self.canvas)

        # Create a single subplot for the line chart
        self.ax = self.figure.add_subplot(111)
        self.ax.set_title(f"{product_name} - Price History")
        self.ax.set_xlabel("Time")
        self.ax.set_ylabel("Price")

        # Prepare and plot initial data
        sorted_data = sorted(price_history.items())
        times = [datetime.datetime.fromtimestamp(int(ts) / 1_000_000) for ts, _ in sorted_data]
        prices = [price for _, price in sorted_data]

        self.ax.plot(times, prices, marker='o', linestyle='-', color='blue')
        self.ax.grid(True)
        self.canvas.draw()

    def update_chart(self, price_history: dict):
        """
        Update the chart with a new set of price history data.

        Called from outside when prices change (e.g., after buy/sell or polling).

        :param price_history: Dictionary with timestamp (int) → price (float)
        """
        # Clear the current plot
        self.ax.clear()
        self.ax.set_title(self.windowTitle())
        self.ax.set_xlabel("Time")
        self.ax.set_ylabel("Price")

        # Re-parse and re-plot the updated history
        sorted_data = sorted(price_history.items())
        times = [datetime.datetime.fromtimestamp(int(ts) / 1_000_000) for ts, _ in sorted_data]
        prices = [price for _, price in sorted_data]

        self.ax.plot(times, prices, marker='o', linestyle='-', color='blue')
        self.ax.grid(True)
        self.canvas.draw()
