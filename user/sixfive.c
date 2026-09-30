#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static void
emit(char *token, int len, int valid, int remainder)
{
  if (!valid || len == 0 || (remainder % 5 != 0 && remainder % 6 != 0))
    return;
  token[len] = '\0';
  // Print the numeric value, without redundant leading zeroes.
  while (len > 1 && *token == '0') {
    token++;
    len--;
  }
  printf("%s\n", token);
}

static int
sixfive(int fd)
{
  char input[512];
  int capacity = 64, len = 0, valid = 1, remainder = 0, n;
  char *token = malloc(capacity);
  if (token == 0) {
    fprintf(2, "sixfive: out of memory\n");
    return 1;
  }

  while ((n = read(fd, input, sizeof(input))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = input[i];
      // NUL is not a separator, even though strchr can find it at the end.
      if (c != '\0' && strchr(" -\r\t\n./,", c)) {
        emit(token, len, valid, remainder);
        len = remainder = 0;
        valid = 1;
      } else if (c < '0' || c > '9') {
        valid = 0;
      } else if (valid) {
        if (len + 1 == capacity) {
          char *larger = 0;
          if (capacity <= 2147483647 / 2)
            larger = malloc(capacity * 2);
          if (larger == 0) {
            fprintf(2, "sixfive: out of memory\n");
            free(token);
            return 1;
          }
          memmove(larger, token, len);
          free(token);
          token = larger;
          capacity *= 2;
        }
        token[len++] = c;
        // Modulo 30 preserves divisibility by 5 and 6 without overflow.
        remainder = (remainder * 10 + c - '0') % 30;
      }
    }
  }
  if (n < 0)
    fprintf(2, "sixfive: read error\n");
  else
    emit(token, len, valid, remainder); // EOF also terminates a token.
  free(token);
  return n < 0;
}

int
main(int argc, char *argv[])
{
  if (argc == 1)
    exit(sixfive(0));

  int failed = 0;
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      failed = 1;
      continue;
    }
    failed |= sixfive(fd);
    close(fd);
  }
  exit(failed);
}
