#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void randname(char *buf)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  // Get the seed
  long r = ts.tv_nsec;

  for (int i = 0; i < 6; ++i)
  {
    // Get a random character using some magic algorithm shit
    buf[i] = 'A' + (r & 15) + (r & 16) * 2;
    // Shift out the bottom 5 bits to get a fresh new set of random bits
    r >>= 5;
  }
}

int create_shm_file(void)
{
  int retries = 100;
  do
  {
    char name[] = "/wl_shm-XXXXXX";
    // Not sure what is going on here???
    randname(name + sizeof(name) - 7);
    --retries;
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd >= 0)
    {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);

  // failed to get an open shared memory
  return -1;
}

int allocate_shm_file(size_t size)
{
  int fd = create_shm_file();
  if (fd < 0)
  {
    return -1;
  }

  /* Some stats to sanity check memory*/
  // struct stat *stat_buf = calloc(1, sizeof(*stat));
  // fstat(fd, stat_buf);
  // off_t fd_size = stat_buf->st_size;

  // printf("Required size: %lu\n", size);
  // printf("fd size: %ld\n", fd_size);

  int ret;
  do
  {
    // Extends memory buffer
    ret = ftruncate(fd, (off_t)size);
  } while (ret < 0 && errno == EINTR);

  if (ret < 0)
  {
    printf("ftruncate: %d\n", ret);
    close(fd);
    return -1;
  }

  return fd;
}