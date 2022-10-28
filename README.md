# ublox-M8-I2C-standalone
Simple program for communicating with u-blox M8 series GNSS modules over I2C

# Installation
## Build `i2c_stream`
Run this code on the target:
```bash
gcc i2c_stream.c -o i2c_stream
```

## Setup software I2C
Many Raspberry Pi's, including the Raspberry Pi zero W, don't support clock stretching on their hardware I2C bus.
You can add the following to `/boot/config.txt` activate a bit-banged I2C bus on GPIO pins 23 and 24 at `/dev/i2c-3`
```ini
[all]
dtoverlay=i2c-gpio,bus=3,i2c_gpio_sda=23,i2c_gpio_scl=24
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
$ ./i2c_stream
Usage: ./i2c_stream <bus> <addr> <poll_sleep_ms> <poll_error_sleep_ms>

$ ./i2c_stream /dev/i2c-3 42 100 1000
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
