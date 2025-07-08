from contextlib import asynccontextmanager

from fastapi import FastAPI, Request, HTTPException
from fastapi.responses import JSONResponse
import uvicorn

from views import auth, general, my, websocket
from server_module import Server


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.server = Server()
    yield
    del app.state.server


app = FastAPI(lifespan=lifespan)


@app.middleware("http")
async def auth_middleware(request: Request, call_next):
    # Don't require auth for some paths
    path_requires_auth = not (
        request.url.path.startswith("/auth/")
        or request.url.path in ["/docs", "/redoc", "/openapi.json", "/ws"]
    )
    if path_requires_auth:
        session_id = request.cookies.get("session_id")
        if not session_id:
            return JSONResponse(status_code=401, content={"detail": "Not authenticated"})

        try:
            session_id = int(session_id)
            if not request.app.state.server.checkifsessionidisvalid(session_id):
                return JSONResponse(status_code=401, content={"detail": "Invalid session ID"})
        except (ValueError, RuntimeError):
            return JSONResponse(status_code=401, content={"detail": "Invalid session ID"})

        request.state.session_id = session_id

    response = await call_next(request)
    return response


app.include_router(auth.router)
app.include_router(general.router)
app.include_router(my.router)
app.include_router(websocket.router)

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
