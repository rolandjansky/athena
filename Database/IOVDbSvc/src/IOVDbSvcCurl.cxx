// Basic Curl wrapper
// @author Shaun Roe
// @date 21 March 2018

#include "IOVDbSvcCurl.h"

namespace IOVDbNamespace{

  IOVDbSvcCurl::IOVDbSvcCurl(const std::string & url) {
    m_handle = curl_easy_init();
    if ( m_handle == nullptr ) throw CurlException("Unable to initialize curl handler");
    if ( url.empty() ) throw CurlException("URL can't be of zero length");
    m_url = url;
  }

  std::string 
  IOVDbSvcCurl::get(){
    setOptions();
    sendGetRequest();
    return m_data;
  }

  IOVDbSvcCurl::~IOVDbSvcCurl() {
    curl_easy_cleanup(m_handle);
    curl_global_cleanup();
  }
   
  void 
  IOVDbSvcCurl::setOptions(){
    //set the url
    check( curl_easy_setopt(m_handle, CURLOPT_URL, m_url.c_str()) );
    //no progress bar
    check( curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, 1L));
    //set the callback function
    check( curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION,IOVDbSvcCurl::writeDataCallback));
    //set pointer in call back function
    check( curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this));
  }

  void 
  IOVDbSvcCurl::check(CURLcode res){
    if ( res != CURLE_OK) throw CurlException(res);
  }

  void 
  IOVDbSvcCurl::sendGetRequest(){
    check( curl_easy_perform(m_handle));
  }

  size_t
  IOVDbSvcCurl::writeDataCallback(void *ptr, size_t size,size_t nmemb, void* pInstance){
      return (static_cast<IOVDbSvcCurl*>(pInstance))->writeData(ptr, size, nmemb);
  }

  size_t 
  IOVDbSvcCurl::writeData(void* ptr, size_t size, size_t nmemb){
    size_t numOfBytes = size * nmemb;
    char *iter = (char*)ptr;
    char *iterEnd = iter + numOfBytes;
    m_data += std::string(iter, iterEnd);
    return numOfBytes;
  }
   
}

