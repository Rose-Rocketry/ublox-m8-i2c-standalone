import asyncio
from calendar import c
import hashlib
from pathlib import Path
from asyncio import subprocess
from importlib.resources import files

CACHE_DIR = Path.home() / ".cache" / "ubloc-m8-i2c"
BINARY_PREFIX = "i2c_stream."


async def generate_binary_path():
    CACHE_DIR.mkdir(parents=True, exist_ok=True)

    c_source = (files('ublox_m8_i2c') / "i2c_stream.c").read_bytes()
    c_source_hash = hashlib.sha256(b"v1:" + c_source).hexdigest()
    binary = CACHE_DIR / (BINARY_PREFIX + c_source_hash)

    if not binary.is_file():
        # Delete old binaries
        for file in CACHE_DIR.glob(BINARY_PREFIX + "*"):
            if file.is_file():
                file.unlink()

        print("Compiling i2c_stream...")
        # Compile with cc
        proc: subprocess.Process = await subprocess.create_subprocess_exec(
            # "echo",
            # "--",
            "cc",
            "-x",
            "c",  # Compile C code
            "-o",
            binary,  # Output to binary
            "-",  # Read from stdin
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        stdout, stderr = await proc.communicate(c_source)
        if len(stdout) > 0:
            print("[stdout]")
            print(stdout.decode())
        if len(stderr) > 0:
            print("[stderr]")
            print(stderr.decode())

        code = await proc.wait()
        if code != 0:
            binary.unlink(missing_ok=True)
            raise RuntimeError(f"Compiler exited with code {code}")

    if not binary.is_file():
        raise RuntimeError("Binary not generated")

    return binary
