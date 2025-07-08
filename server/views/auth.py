from fastapi import APIRouter, HTTPException, Request, Response
from pydantic import BaseModel
from server_module import Server, Product

router = APIRouter(
    prefix="/auth",
)


class AuthRequest(BaseModel):
    username: str
    password: str


class DetailReturn(BaseModel):
    detail: str


@router.post(
    "/signin",
    responses={
        401: {
            "model": DetailReturn,
            "description": "Invalid username or password.",
        }
    },
)
async def signin(
    request: Request, response: Response, auth_data: AuthRequest
) -> DetailReturn:
    """
    Handles user sign-in functionality. This endpoint validates user credentials
    and, upon successful authentication, generates a session ID, which is stored
    as an HTTP-only cookie for session management. If the provided
    credentials are invalid, the endpoint raises an HTTPException with a 401
    status code.
    """
    session_id = request.app.state.server.signin(auth_data.username, auth_data.password)

    # Check for errors
    if session_id == -1:
        raise HTTPException(status_code=401, detail="Invalid username or password")

    response.set_cookie(key="session_id", value=str(session_id), httponly=True)
    return DetailReturn(detail="Signed in successfully")


@router.post("/signout")
async def signout(request: Request, response: Response) -> DetailReturn:
    """
    Signs out a user by invalidating their session and removing the session cookie
    from the response.
    """
    session_id = request.cookies.get("session_id")
    if session_id:
        try:
            request.app.state.server.signout(int(session_id))
        except (RuntimeError, ValueError) as e:
            # Ignore errors if session is already invalid
            pass
    response.delete_cookie(key="session_id")
    return DetailReturn(detail="Signed out successfully")


@router.post(
    "/register",
    responses={
        401: {
            "model": DetailReturn,
            "description": "Invalid username or password.",
        }
    },
)
async def signup(request: Request, auth_data: AuthRequest) -> DetailReturn:
    """
    Handles user registration by processing the provided authentication data.
    """
    try:
        request.app.state.server.register_user(auth_data.username, auth_data.password)
        return DetailReturn(detail="Signed up successfully")
    except RuntimeError as e:
        raise HTTPException(status_code=400, detail=str(e))
