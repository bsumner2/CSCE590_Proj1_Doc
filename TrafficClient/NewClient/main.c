#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#include <curl/easy.h>

#define perrf(fmt, ...) fprintf(stderr, "\x1b[1;31m[Error]:\x1b[0m "fmt, __VA_ARGS__)

#define perr(s) fputs("\x1b[1;31m[Error]:\x1b[0m "s, stderr)

static size_t write_callback(void*, size_t, size_t, void*);

#define ENDPOINT_URL_BASE "https://csce590-sp1-dockerdep.azurewebsites.net/TrafficTesting/Sieve"

#ifdef LOCAL_TEST

#undef ENDPOINT_URL_BASE

#define ENDPOINT_URL_BASE "http://localhost:5024/TrafficTesting/Sieve"
#endif

typedef struct rsp {
  void *userdata;
  size_t len;
} Rsp_t;


size_t write_callback(void *data, size_t unit_len, size_t unit_ct, void *userdata) {
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


static volatile bool run = false;

void *thread_entrypt(void *userdata) {
  Rsp_t rsp = {0};
  CURL *curl_handle;
  char *url;
  CURLcode outcome;
  int len, num, client_id;
  client_id = (int)userdata;
  while (!run)
    continue;  // Wait for all threads to be created.
  while (run) {
    len = snprintf(NULL, 0, ENDPOINT_URL_BASE"?limit=%d", (num=(2+(0x3FFFF&rand()))));
    if (len <= 0)
      return NULL;
    url = (char*)calloc(len+1, sizeof(char));
    sprintf(url, ENDPOINT_URL_BASE"?limit=%d", num);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&rsp);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    outcome = curl_easy_perform(curl_handle);
    
    free((void*)url);
    
    url = NULL;
    
    if (outcome != CURLE_OK) {
      perrf("\x1b[1;34m[[Client No. %d]]:\x1b[31m "
          "Request failed.\x1b[0m Details below:\n%s\n",
          client_id, curl_easy_strerror(outcome));
    } else {
      if (rsp.len <= 1000)
        printf("\x1b[1;34m[[Client No. %d]]:\x1b[33m "
            "Request Succeeded:\x1b[0m %s\n", client_id, (char*)rsp.userdata);
      else
        printf("\x1b[1;34m[[Client No. %d]]:\x1b[33m "
            "Request Succeeded:\x1b[0m %s\n", client_id,
            ((char*)(rsp.userdata) + rsp.len - 825));
    }
    
    free((void*)(rsp.userdata));
    curl_easy_cleanup(curl_handle);
    rsp.userdata = NULL;
    rsp.len = 0;
    sleep(rand()&7);
  }

  return (void*)1;
}


void interrupt_handler(int signal_no) {
  if (!run)
    exit(0);
  run = false;
  printf("Waiting for clients to finish.\n");
}


int main(int argc, char *argv[]) {
  if (argc!=2) {
    perrf("Invalid args. Usage: \x1b[1m%s <client thread ct>\x1b[0m", argv[0]);
    return 1;
  }
  int client_ct = atoi(argv[1]);
  if (client_ct <= 0) {
    perrf("Invalid args. Usage: \x1b[1m%s <client thread ct>\x1b[0m", argv[0]);
    return 1;
  }
  signal(SIGINT, interrupt_handler);
  
  pthread_attr_t threat_attrs;

  pthread_attr_init(&threat_attrs);

  pthread_t clients[client_ct];

  for (int i = 0; i < client_ct; ++i) {

    pthread_create(&clients[i], &threat_attrs, thread_entrypt, (void*)i);
  }

  run = true;

  while (run)
    continue;
  void *ret;
  for (int i = 0; i < client_ct; ++i) {
    pthread_join(clients[i], &ret);
    if (!ret)
      perrf("\x1b[1;34m[[Client No. %d]]\x1b[0m: Thread exited with error value.\n", i);
  }
  return 0;









}






























