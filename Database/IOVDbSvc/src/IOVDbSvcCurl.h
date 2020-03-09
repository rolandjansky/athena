/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file Curl.h
// Basic Curl wrapper
// @author Shaun Roe
// @date 21 March 2018
#ifndef IOVDbSvc_IOVDbSvcCurl_h
#define IOVDbSvc_IOVDbSvcCurl_h

#include <string>
#include <stdexcept>

extern "C"{
  #include "curl/curl.h"
  #include <stdlib.h>
}

namespace IOVDbNamespace{
  ///CurlException class to use with IOVDbSvcCurl
  class CurlException : public std::runtime_error{
  public:
   CurlException(std::string const& message): std::runtime_error(message) {}
   CurlException(CURLcode error): std::runtime_error(curl_easy_strerror(error)) {}
  };
  
  //@brief Minimal interface to Curl library to perform http get
  class IOVDbSvcCurl{
  public:
    ///Constructor takes URL as string
    IOVDbSvcCurl(const std::string & url);
    ///simple http GET to the URL, return data as a string
    std::string get();
    ///Destructor does curl cleanup
    ~IOVDbSvcCurl();
  private:
    ///Set curl options
    void setOptions();
    ///Check return codes and throw if not ok
    void check(CURLcode res);
    ///Perform request after options are set
    void sendGetRequest();
    ///Callback function with static linkage for curl to use in the GET
    static size_t writeDataCallback(void *ptr, size_t size,size_t nmemb, void* pInstance);
    ///Actual function used in the callback
    size_t writeData(void* ptr, size_t size, size_t nmemb);
    
    //data members
    CURL *m_handle;
    std::string m_url;
    std::string m_data;

  };

}//namespace

#endif

