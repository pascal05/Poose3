from fastapi import WebSocket, WebSocketDisconnect, APIRouter

router = APIRouter(prefix="/ws")

class ConnectionManager:
    def __init__(self):
        self.active_connections: list[WebSocket] = []

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)

    def disconnect(self, websocket: WebSocket):
        self.active_connections.remove(websocket)

    async def send_personal_message(self, message: str, websocket: WebSocket):
        await websocket.send_text(message)

    async def broadcast(self, message):
        for connection in self.active_connections:
            await connection.send_json(message)

manager = ConnectionManager()


@router.get("/chat",
            summary="Documentation for the WebSocket Chat",
            description="""
This endpoint describes the WebSocket protocol for the chat application. 
It does not handle any logic itself but serves as a documentation entry point.

### Protocol

Communication is handled via JSON objects, each containing a `type` field that defines the message's purpose.

### Message Types

Below are the different types of messages that can be sent between the client and the server.

---

#### Client → Server Messages

*   **`start`**: Sent when a client joins the chat.
    ```json
    {
      "type": "start",
      "name": "string"
    }
    ```
*   **`message_send`**: Sent when a client wants to send a message.
    ```json
    {
      "type": "message_send",
      "message": "string"
    }
    ```

---

#### Server → Client Messages

*   **`message`**: A message sent by someone, that should be displayed. 
    ```json
    {
      "type": "message",
      "name": "string",
      "message": "string"
    }
    ```
""")
def websocket_endpoint_docs():
    """This endpoint is only for documentation purposes."""
    pass


@router.websocket("/chat")
async def websocket_chat(websocket: WebSocket):
    await manager.connect(websocket)
    try:
        while True:
            data = await websocket.receive_json()
            match data["type"]:
                case "start":
                    websocket.state.name = data["name"]
                    await manager.broadcast({"type": "message", "name": websocket.state.name, "text": "joined the chat"})
                case "message_send":
                    text = data["text"]
                    await manager.broadcast({"type": "message", "name": websocket.state.name, "text": text})
                case _:
                    await manager.send_personal_message({"type": "error", "message": "Invalid message type"})
    except WebSocketDisconnect:
        manager.disconnect(websocket)
        await manager.broadcast({"type": "message", "name": websocket.state.name, "text": "left the chat"})
