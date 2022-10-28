#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

// #include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

// #define I2C_ADDR 0x42
#define I2C_BYTES_AVAIL 0xFD
#define I2C_DATA 0xFF

#define POLL_SLEEP 100000
#define POLL_ERROR_SLEEP 1000000

unsigned long gettime_ms()
{
  struct timespec now;

  clock_gettime(CLOCK_MONOTONIC_RAW, &now);

  return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

// I2C based on code from https://www.pololu.com/docs/0J73/15.8
bool i2c_write(int fd, uint8_t addr, uint8_t reg, uint16_t size, uint8_t *data)
{
  uint8_t len_buf[size + 1]; // [reg, ...data]
  len_buf[0] = reg;
  memcpy(len_buf + 1, data, size);

  struct i2c_msg messages[] = {
      {addr, 0, size, data}, // Write data to register "reg"
  };
  struct i2c_rdwr_ioctl_data ioctl_data = {messages, 1};

  return ioctl(fd, I2C_RDWR, &ioctl_data) == 1;
}

bool i2c_read(int fd, uint8_t addr, uint8_t reg, uint16_t size, uint8_t *data)
{
  uint8_t reg_buf[] = {reg};
  struct i2c_msg messages[] = {
      {addr, 0, sizeof(reg_buf), reg_buf}, // Send the register address
      {addr, I2C_M_RD, size, data},        // Read bytes from that address
  };
  struct i2c_rdwr_ioctl_data ioctl_data = {messages, 2};

  return ioctl(fd, I2C_RDWR, &ioctl_data) == 2;
}

int poll_read(int fd, uint8_t addr)
{
  uint8_t len_buf[2];

  if (!i2c_read(fd, addr, I2C_BYTES_AVAIL, 2, len_buf))
  {
    perror("Error reading length from I2C");
    return -1;
  }

  // Convert the read bytes to an int
  uint16_t len = ((uint16_t)len_buf[0] << 8) | ((uint16_t)len_buf[1]);

  if (len == 0)
  {
    return 0;
  }
  else
  {
    uint8_t data_buf[len];

    if (!i2c_read(fd, addr, I2C_DATA, len, data_buf))
    {
      perror("Error reading data from I2C");
      return -1;
    }

    // if (write(STDOUT_FILENO, data_buf, len) != len)
    // {
    //   perror("Error writing messages");
    //   return -1;
    // }

    uint8_t *data_ptr[] = {
        data_buf};

    if (fwrite(data_buf, len, 1, stdout) != 1)
    {
      perror("Error writing to STDOUT");
      exit(1);
    }

    for (int i = 0; i < len; i++)
    {
      putc(data_buf[i], stdout);
    }

    return len;
  }
}

void poll_write_timeout(int fd, uint8_t addr, unsigned long timeout)
{
  struct pollfd pfds[] = {
      {STDIN_FILENO, POLLIN}};

  unsigned char buffer[4096];
  unsigned long start = gettime_ms();

  unsigned long time_left = timeout - (gettime_ms() - start);

  // Wait for an event on stdin, or timeout
  if (time_left != 0)
  {
    poll(pfds, 1, time_left);
  };

  short revents = pfds[0].revents;
  if (revents != 0 || time_left == 0)
  {
    // Event occurred on stdin
    if (revents & POLLIN || time_left == 0)
    {
      ssize_t len = read(STDIN_FILENO, buffer, sizeof(buffer));
      if (!i2c_write(fd, addr, I2C_BYTES_AVAIL, len, buffer))
      {
        perror("Error writing to I2C");
      }
    }
    else
    {
      perror("Error reading from stdin");
      exit(1);
    }
  }
}

int main(int argc, const char **argv)
{
  if (argc != 5)
  {
    fprintf(stderr, "Usage: %s <bus> <addr> <poll_sleep_ms> <poll_error_sleep_ms>\n", argv[0]);
    return 1;
  }

  unsigned long addr = strtoul(argv[2], NULL, 16);
  if (addr == 0 || addr > 127)
  {
    fprintf(stderr, "Invalid argument \"%s\" for <addr>", argv[2]);
    return 1;
  }

  unsigned long poll_sleep_ms = strtoul(argv[3], NULL, 10);
  if (poll_sleep_ms == 0)
  {
    fprintf(stderr, "Invalid argument \"%s\" for <poll_sleep_ms>", argv[3]);
    return 1;
  }

  unsigned long poll_error_sleep_ms = strtoul(argv[4], NULL, 10);
  if (poll_error_sleep_ms == 0)
  {
    fprintf(stderr, "Invalid argument \"%s\" for <poll_error_sleep_ms>", argv[4]);
    return 1;
  }

  int fd = open(argv[1], O_RDWR);
  if (fd < 0)
  {
    perror("Error opening bus");
    return 1;
  }

  // TODO: read stdin

  while (true)
  {
    printf(".");
    int status = poll_read(fd, addr);

    if (status < 0)
    {
      poll_write_timeout(fd, addr, poll_error_sleep_ms);
      continue;
    }
    else if (status == 0)
    {
      poll_write_timeout(fd, addr, poll_sleep_ms);
      continue;
    }
    else
    {
      continue;
    }
  }
}
