import asyncio
from asyncio.subprocess import Process, PIPE
from ublox_m8_i2c.compiler import generate_binary_path


# from pathlib import Path
# _BINARY_PATH = Path(__file__).parent / "i2c_stream"
async def open_async(
    bus="/dev/i2c-3",
    address=0x42,
    poll_sleep=0.1,
    poll_error_sleep=2,
) -> "tuple[asyncio.StreamReader, asyncio.StreamWriter]":
    binary_path = await generate_binary_path()

    process: Process = await asyncio.create_subprocess_exec(
        binary_path,
        bus,
        str(address),
        str(int(poll_sleep * 1_000)),
        str(int(poll_error_sleep * 1_000)),
        stdin=PIPE,
        stdout=PIPE,
    )

    # TODO: Actual error handling
    return (process.stdout, process.stdin)
