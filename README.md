# ublox-M8-I2C
Asynchronous library for interfacing with u-blox M8 series GNSS modules over I2C


# Installation
## Install ubloc-m8-i2c
```bash
pip install "git+https://github.com/Rose-Rocketry/ublox-m8-i2c"
```

## Setup software I2C
Many Raspberry Pi's, including the Raspberry Pi zero W, don't support clock stretching on their hardware I2C bus.
You can add the following to `/boot/config.txt` activate a bit-banged I2C bus on GPIO pins 23 and 24 at `/dev/i2c-3`
```ini
# (...default values...)

# Uncomment some or all of these to enable the optional hardware interfaces
dtoverlay=i2c-gpio,bus=3,i2c_gpio_sda=23,i2c_gpio_scl=24
#dtparam=i2c_arm=on
#dtparam=i2s=on
#dtparam=spi=on

# (...default values...)
```

## Wiring
_TODO_


# Usage Example
See [\_\_main__.py](./ublox_m8_i2c/__main__.py) for a complete example

```py
from ublox_m8_i2c import open_async

reader, writer = await open_async(
    bus="/dev/i2c-3",
    address=0x42,
)

await writer.write(b"Hello World!\n")

line = await reader.readline()
print(line)
```
