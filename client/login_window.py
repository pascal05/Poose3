import sys
import requests
from PyQt6 import uic
from PyQt6.QtWidgets import QApplication, QWidget
from PyQt6.QtCore import pyqtSlot
from marketplace_window import MarketplaceWindow  

SERVER_BASE_URL = "0.0.0.0:8000"

class LoginWindow(QWidget):
    """
    LoginWindow handles user login and registration using a simple PyQt6 form.

    Features:
    - Login via REST API to FastAPI backend
    - Register new users
    - On successful login, opens the MarketplaceWindow
    """

    def __init__(self):
        """
        Initialize the login window by loading the UI and connecting button signals.
        """
        super().__init__()
        uic.loadUi("login.ui", self)  # Load the .ui file created in Qt Designer

        # Connect buttons to handler methods
        self.loginButton.clicked.connect(self.login_clicked)
        self.registerButton.clicked.connect(self.register_clicked)

    @pyqtSlot()
    def login_clicked(self):
        """
        Handles the login button click.

        Sends a POST request to /auth/signin with username and password.
        If successful, opens the main marketplace window.
        """
        username = self.usernameLineEdit.text()
        password = self.passwordLineEdit.text()

        try:
            # Send login request to FastAPI backend
            response = requests.post(f"{SERVER_BASE_URL}/auth/signin", json={
                "username": username,
                "password": password
            })

            if response.status_code == 200:
                # Extract session ID from the cookie (set by FastAPI)
                session_id = response.cookies.get("session_id")
                self.infoLabel.setText("Login successful!")

                # Open the main marketplace window
                self.marketplace = MarketplaceWindow(session_id, username)
                self.marketplace.show()
                self.close()

            else:
                # Show error if login failed
                error = response.json().get("detail", "Login failed.")
                self.infoLabel.setText(f"Login failed: {error}")

        except Exception as e:
            self.infoLabel.setText(f"Error: {e}")

    @pyqtSlot()
    def register_clicked(self):
        """
        Handles the register button click.

        Sends a POST request to /auth/register with username and password.
        Displays success or error message based on response.
        """
        username = self.usernameLineEdit.text()
        password = self.passwordLineEdit.text()

        try:
            # Send registration request
            response = requests.post(f"{SERVER_BASE_URL}/auth/register", json={
                "username": username,
                "password": password
            })

            if response.status_code == 200:
                self.infoLabel.setText("Registration successful!")
            else:
                error = response.json().get("detail", "Register failed.")
                self.infoLabel.setText(f"Register failed: {error}")

        except Exception as e:
            self.infoLabel.setText(f"Error: {e}")


if __name__ == "__main__":
    # Start the PyQt6 application
    app = QApplication(sys.argv)
    window = LoginWindow()
    window.show()
    sys.exit(app.exec())
