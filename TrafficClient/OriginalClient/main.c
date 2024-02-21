#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#define perrf(fmt, ...) fprintf(stderr, "\x1b[1;31m[Error]:\x1b[0m "fmt, __VA_ARGS__)

#define perr(s) fputs("\x1b[1;31m[Error]:\x1b[0m "s, stderr)

typedef struct rsp {
  void *userdata;
  size_t len;
} Rsp_t;

#define GUID_STRLEN 36

typedef struct post {
  char post_id[GUID_STRLEN+1], author_id[GUID_STRLEN+1],
      *title, *body;
  int title_len, body_len;
} Post_t;

typedef struct post_list_node {
  Post_t post;
  struct post_list_node *next;
} Post_List_Node_t;

typedef struct post_list {
  Post_List_Node_t *head;
  int len;
} Post_List_t;

static inline _Bool JSON_Separator_Valid(char *json_str) {
  return !strncmp(json_str, "\",\"", 3);
}


static void Post_Close(Post_t *post);

static Post_List_Node_t *new_node(const Post_t *post, Post_List_Node_t *next) {
  Post_List_Node_t *ret = (Post_List_Node_t*)malloc(sizeof(*ret));
  ret->post = *post;
  ret->next = next;
  return ret;
}

int AddPost(Post_List_t *list, char **json_str) {
  char *json_data = *json_str;

  if (strncmp(json_data, "\"postId\":\"", 10))
    goto UNEXPECTED_SYMBOL;
  
  json_data += 10;
  
  Post_t post = {0};
  strncpy(post.post_id, json_data, GUID_STRLEN);
  if (!JSON_Separator_Valid(json_data+=GUID_STRLEN))
    goto UNEXPECTED_SYMBOL;

  json_data += 3;

  if (strncmp(json_data, "author\":\"", 9))
    goto UNEXPECTED_SYMBOL;
  
  json_data+=9;

  strncpy(post.author_id, json_data, GUID_STRLEN);
  
  if (!JSON_Separator_Valid(json_data+=GUID_STRLEN))
    goto UNEXPECTED_SYMBOL;
  
  json_data+=3;
  if (strncmp(json_data, "title\":\"", 8))
    goto UNEXPECTED_SYMBOL;
  
  {
    char *cursor = json_data += 8, c;
    while ((c = *cursor++) && c!='\"')
      ++post.title_len;

    if (c != '\"')
      goto UNEXPECTED_SYMBOL;
  }
  post.title = (char*)malloc(sizeof(char)*(1+post.title_len));
  strncpy(post.title, json_data, post.title_len);
  post.title[post.title_len] = '\0';


  if (!JSON_Separator_Valid(json_data+=post.title_len)) {
    Post_Close(&post);
    goto UNEXPECTED_SYMBOL;
  }

  json_data+=3;

  {
    char *cur, *savept;
    cur = strtok_r(json_data, "\":,", &savept);
    while (cur!=NULL && strncmp(cur, "body", 4))
      cur = strtok_r(NULL, "\":,", &savept);
    if (cur == NULL) {
      Post_Close(&post);
      goto UNEXPECTED_SYMBOL;
    }

    json_data = cur + 7;
    cur = json_data;
    char c;
    while ((c=*cur++) && c!='\"')
      ++post.body_len;
    
    if (!c) {
      Post_Close(&post);
      goto UNEXPECTED_SYMBOL;
    }

  }

  post.body = (char*)malloc(sizeof(char)*(1+post.body_len));
  strncpy(post.body, json_data, post.body_len);
  post.body[post.body_len] = '\0';
  ++list->len;
  list->head = new_node(&post, list->head);
  json_data += post.body_len;
  if (*++json_data!='}')
    goto UNEXPECTED_SYMBOL;

  if (*++json_data==']')
    return 2;
  if (strncmp(json_data, ",{", 2))
    goto UNEXPECTED_SYMBOL;
  
  *json_str = json_data+2;
  
  return 1;

UNEXPECTED_SYMBOL:
  perrf("Unexpected field at start of JSON obj.\nJSON String from parser "
        "cursor: %s\n", json_data);
  return 0;

}


void Post_Close(Post_t *post) {
  if (post->title)
    free((void*)post->title);
  if (post->body)
    free((void*)post->body);
  memset(post, 0, sizeof(*post));

}


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

_Bool Dequeue_Post(Post_t *dst, Post_List_t *list) {
  if (!(list->len))
    return 0;
  *dst = list->head->post;
  Post_List_Node_t *oldhead = list->head;
  list->head = oldhead->next;
  --(list->len);
  free((void*)oldhead);

  return 1;
  


}





static _Bool GetAllPostsRaw(Rsp_t *rsp) {
  Rsp_t response = {0};

  CURLcode outcome;

  CURL *curl_handle = curl_easy_init();

  if (!curl_handle) {
    return 0;
  }

  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&response);

  curl_easy_setopt(curl_handle, CURLOPT_URL, "https://csce590-sp1-dockerdep.azurewebsites.net/Post");

  outcome = curl_easy_perform(curl_handle);

  if (outcome!=CURLE_OK) {
    if (response.userdata)
      free(response.userdata);
    curl_easy_cleanup(curl_handle);
    return 0;
  }
 
  *rsp = response;

  curl_easy_cleanup(curl_handle);

  return 1;
}

#define _GENERATE_ACCOUNT_CREDENTIALS_
#ifdef _GENERATE_ACCOUNT_CREDENTIALS_
typedef uint8_t PasswordHash_t[32];
typedef char PasswordHexStr_t[65];
#define ACCOUNT_CT 16
PasswordHexStr_t acct_pass_strs[ACCOUNT_CT] = {0};

static const char *account_names[ACCOUNT_CT] = {
  "LilyPad89",
  "MaverickSky",
  "RubySparrow",
  "PhoenixRider",
  "ShadowWhisperer",
  "LunaEcho",
  "ThunderPaws",
  "StarlightDreamer",
  "CrimsonTide",
  "AuroraBlaze",
  "MysticWanderer",
  "NightingaleSong",
  "OceanBreeze77",
  "EmberFlame",
  "SilverArrow",
  "WillowWisp"
};


void seed_account_hashes() {

  srand(time(NULL));
  PasswordHash_t password;
  uint64_t *dst = (uint64_t*)password;
  for (int i = 0; i < ACCOUNT_CT; ++i) {
    for (int j = 0; j < 4; ++j) {
      dst[j] = (((uint64_t)rand())<<32)|rand();
    }
    sprintf(acct_pass_strs[i],"%016lX%016lX%016lX%016lX",
        dst[0], dst[1], dst[2], dst[3]);
  }
}
#else
#include "accounts.h"
#endif




int main(void) {
#ifdef _GENERATE_ACCOUNT_CREDENTIALS_
  seed_account_hashes();
  FILE *fp = fopen("./accounts.h", "w");
  if (!fp) return 1;

  fputs("#ifndef _ACCOUNT_CREDENTIALS_H_\n#define _ACCOUNT_CREDENTIALS_H_\n", fp);

  fprintf(fp, "static const char *AccountNames[%d] = {\n", ACCOUNT_CT);
  for (int i = 0; i < ACCOUNT_CT-1; ++i) {
    fprintf(fp, "  %s,\n", account_names[i]);
  }
  fprintf(fp, "  %s\n};\n\n", account_names[ACCOUNT_CT-1]);

  fprintf(fp, "static const char *AccountPasswords[%d] = {\n", ACCOUNT_CT);
  for (int i = 0; i < ACCOUNT_CT-1; ++i) {
    fprintf(fp, "  %s,\n", acct_pass_strs[i]);
  }

  fprintf(fp, "  %s\n};\n\n", acct_pass_strs[ACCOUNT_CT-1]);

  fputs("#endif  /* _ACCOUNT_CREDENTIALS_H_ */\n\n", fp);
  
  fclose(fp);
  for (int i = 0; i < ACCOUNT_CT; ++i) {
    
  }

  
  return 0;
#else
  Rsp_t response;

  if (!GetAllPostsRaw(&response))
    return 1;
  
  Post_List_t posts = {0};
  char *json_cursor = 2+response.userdata;

  while (1&AddPost(&posts, &json_cursor))
    continue;
  free(response.userdata);
  Post_t post = {0};
  while (Dequeue_Post(&post, &posts)) {
    printf("Post:\n"
        "\tPost ID: %s\n"
        "\tAuthor ID: %s\n"
        "\tTitle: %s\n"
        "------------------------------------------\n%s\n"
        "------------------------------------------\n",
        post.post_id, post.author_id, post.title, post.body);
    Post_Close(&post);
  }

  assert(!posts.head && !posts.len);
    
  return 0;
#endif
}
