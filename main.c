
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  CURL *curl;
  CURLcode res;
  char url[]= "http://169.254.150.67:8080/hello/world";
//  char postData[] = "{\"hi\":\"there\"}"}
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  printf("%d", res);
  return 0;
}
