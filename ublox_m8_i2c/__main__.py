import asyncio
from ublox_m8_i2c import open_async


async def main():
    # Compiles native binary and starts polling module at 10Hz
    reader, writer = await open_async(
        bus="/dev/i2c-3",
        address=0x42,
        poll_sleep=0.1,
        poll_error_sleep=2,
    )

    while True:
        if (reader.at_eof()):
            raise EOFError()

        # Read messages from the module
        print(await reader.read(4096))


if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
    asyncio.get_event_loop().close()
