from fastapi import APIRouter, Request, HTTPException
from pydantic import BaseModel


class DetailReturn(BaseModel):
    detail: str


router = APIRouter(prefix="/my")


@router.get("/inventory")
async def get_inventory(request: Request) -> dict[int, int]:
    """
    Returns the user's inventory.

    The response is a dictionary where each key is the ID of a product
    and the corresponding value is the quantity of that product in the
    inventory.
    """
    server = request.app.state.server
    inventory = server.list_of_own_inventory(request.state.session_id)
    return inventory


@router.get("/wallet")
async def get_wallet(request: Request) -> float:
    """
    Returns the user's wallet.

    The response is a float with how many POOSE-Coins the user has.
    """
    server = request.app.state.server
    wallet = server.wallet(request.state.session_id)
    return wallet


@router.post(
    "/product/{product_id}/buy",
    responses={
        400: {
            "model": DetailReturn,
            "description": "Invalid input, product not found, or user has no money",
        }
    },
)
async def buy_product(product_id: int, request: Request) -> DetailReturn:
    """
    Buys the product (one item) specified by the product ID.
    """
    server = request.app.state.server
    try:
        server.buy(product_id, request.state.session_id)
    except RuntimeError as e:
        raise HTTPException(status_code=400, detail=str(e))
    return DetailReturn(detail="Product bought successfully!")


@router.post(
    "/product/{product_id}/sell",
    responses={
        400: {
            "model": DetailReturn,
            "description": "Invalid input, product not found, or user does not own the product",
        }
    },
)
async def sell_product(product_id: int, request: Request) -> DetailReturn:
    """
    Sell the product (one item) specified by the product ID.
    """
    server = request.app.state.server
    try:
        server.sell(product_id, request.state.session_id)
    except RuntimeError as e:
        raise HTTPException(status_code=400, detail=str(e))
    return DetailReturn(detail="Product sold successfully!")
