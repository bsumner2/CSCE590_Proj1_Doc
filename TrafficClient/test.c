#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <curl/curl.h>
#include <curl/easy.h>



#define perrf(fmt, ...) fprintf(stderr, "\x1b[1;31m[Error]:\x1b[0m "fmt, __VA_ARGS__)

#define perr(s) fputs("\x1b[1;31m[Error]:\x1b[0m "s, stderr)

typedef struct {
  void *userdata;
  size_t len;
} Rsp_t;

static size_t write_callback(void *data, size_t unit_len, size_t unit_ct, void *userdata) {
  size_t added_len = unit_ct*unit_len;
  Rsp_t *rsp = (Rsp_t*)userdata;

  uint8_t *dst = realloc(rsp->userdata, rsp->len + added_len + 1);
  
  if (!dst) {
    perr("Malloc failure while trying to fetch data.\n");
    return 0;
  }

  rsp->userdata = (void*)dst;
  memcpy((void*)(dst + (rsp->len)), data, added_len);
  rsp->len += added_len;
  dst[rsp->len] = 0;

  return added_len;
}



int main(void) {
  CURL *test;  
}
