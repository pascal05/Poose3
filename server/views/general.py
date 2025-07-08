from fastapi import APIRouter, Request, HTTPException
from pydantic import BaseModel


class ErrorDetail(BaseModel):
    detail: str


router = APIRouter()


class ProductReturn(BaseModel):
    id: int
    name: str
    current_price: float
    # timestamp -> price
    price_history: dict[int, float]

    model_config = {
        "json_schema_extra": {
            "examples": [
                {
                    "id": 10,
                    "name": "A very nice Item",
                    "current_price": 35.4,
                    "price_history": {
                        1751657651759854: 35.4,
                        1751657651759850: 10
                    },
                }
            ]
        }
    }



@router.get("/products")
async def list_products(request: Request) -> list[ProductReturn]:
    """
    Handles the retrieval and listing of all available products for a given session.
    """
    server = request.app.state.server
    products = server.products_list(request.state.session_id)
    out = [
        ProductReturn(
            id=product.getproductid(),
            name=product.getproductname(),
            current_price=product.getcurrentprice(),
            price_history=product.getpricehistory(),
        )
        for product in products
    ]
    return out


class ProductPriceReturn(BaseModel):
    price: float


@router.get(
    "/product/{product_id}/price",
    responses={
        400: {
            "model": ErrorDetail,
            "description": "Invalid input or product not found",
        }
    },
)
async def get_product_price(product_id: int, request: Request) -> ProductPriceReturn:
    """
    Fetches the price of a specified product by ID and returns the price details.
    """
    server = request.app.state.server
    try:
        price = server.product_price(request.state.session_id, product_id)
    except RuntimeError as e:
        raise HTTPException(status_code=400, detail=str(e))
    return ProductPriceReturn(price=price)
