#include <wiringPiI2C.h>
#include <wiringPi.h>

#include <unistd.h>

#include <iostream>

#include <math.h>

struct vec3
{
  double x;
  double y;
  double z;
};

int fd;

int read_word_2c(int addr)
{
  int val = wiringPiI2CReadReg8(fd, addr);
  val = val << 8;
  val += wiringPiI2CReadReg8(fd, addr + 1);

  if(val >= 0x8000)
  {
    val = -(65536 - val);
  }

  return val;
}

double dist(double a, double b)
{
  return sqrt(a * a + b * b);
}

vec3 get_orientation()
{
  double accelX = read_word_2c(0x3B);
  double accelY = read_word_2c(0x3D);
  double accelZ = read_word_2c(0x3F);

  accelX /= 16384.0f;
  accelY /= 16384.0f;
  accelZ /= 16384.0f;

  vec3 rtn = {0};

  rtn.x = atan2(accelY, dist(accelX, accelZ)) * (180.0f / M_PI);
  rtn.y = atan2(accelX, dist(accelY, accelZ)) * (180.0f / M_PI);
  rtn.z = atan2(accelZ, dist(accelX, accelY)) * (180.0f / M_PI);

  return rtn;
}

int main()
{
  fd = wiringPiI2CSetup(0x68);
  wiringPiI2CWriteReg8(fd, 0x6B, 0x00);

  std::cout << "Set 0x6B: " << wiringPiI2CReadReg8(fd, 0x6B) << std::endl;

  while(true)
  {
    vec3 orientation = get_orientation();

    std::cout << "Orientation: X: " << orientation.x << " Y: "
      << orientation.y << " Z: " << orientation.z << std::endl;

    sleep(1);
  }

  return 0;
}
