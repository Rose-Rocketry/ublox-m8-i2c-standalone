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
| u-blox pin | Raspberry Pi pin                                 |
|------------|--------------------------------------------------|
| 3v3        | [3v3](https://pinout.xyz/pinout/3v3_power)       |
| GND        | [GND](https://pinout.xyz/pinout/ground)          |
| SDA        | [GPIO23](https://pinout.xyz/pinout/pin16_gpio23) |
| SCL        | [GPIO24](https://pinout.xyz/pinout/pin18_gpio24) |

![Image of Sparkfun u-blox SAM-M8Q connected to a Raspberry Pi zero W](https://user-images.githubusercontent.com/12852902/194787496-3f6a5b18-6893-4dd7-84e0-b92f5a14d139.jpg)

## Testing
You can test your connection to the module with the following command:
```sh
$ python -m ublox_m8_i2c
Compiling i2c_stream...
..$GNRMC,010129.00,V,,,,,,,101022,,,N*68
$GNVTG,,,,,,,,,N*2E
$GNGGA,010129.00,,,,,0,00,99.99,,,,,,*73
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*2E
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*2E
$GPGSV,1,1,01,08,,,19*78
```


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
