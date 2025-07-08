FROM debian:12-slim AS builder

RUN apt-get update
RUN apt-get install -y cmake clang make python3 python3-dev pybind11-dev libgtest-dev

COPY . /build

WORKDIR /build

RUN rm -rf build
RUN rm -f server/*.so
RUN cmake -S . -B build
RUN cmake --build build --target server_module
RUN cmake --install build

# The second stage will install the runtime dependencies only and copy
# the compiled executables

FROM debian:12-slim as runtime

WORKDIR /app

RUN apt-get update
RUN apt-get install -y python3 libstdc++6 libllvm14

COPY --from=ghcr.io/astral-sh/uv:latest /uv /uvx /bin/

COPY --from=builder /build/server /app
RUN ls /app
COPY --from=builder /build/pyproject.toml /app
COPY --from=builder /build/uv.lock /app

RUN uv venv --no-managed-python .venv
RUN uv sync --frozen
ENV PATH="/app/.venv/bin:$PATH"

EXPOSE 8000

ENTRYPOINT ["python", "main.py"]
