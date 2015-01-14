/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>

#include "testutils.h"
#include "TrigNavStructure/StringSerializer.h"

bool contentMany() {
  BEGIN_TEST;
  std::vector<std::string>  in{"Hello", "whoever", "listens"};
  std::vector<uint32_t> data;
  HLT::StringSerializer::serialize(in, data);  
  std::vector<std::string>  out;
  size_t sz = HLT::StringSerializer::deserialize(data.begin(), data.end(), out);
  if ( sz != data.size() )
    REPORT_AND_STOP("sizes reporting by deserialzie incorrect");
  if ( in != out )
    REPORT_AND_STOP("Content after deserialziation is different");
  
  return true;
}

bool contentOne() {
  BEGIN_TEST;
  std::string in("Another test");

  std::vector<uint32_t> data;
  HLT::StringSerializer::serialize(in, data);  
  std::string  out;
  size_t sz = HLT::StringSerializer::deserialize(data.begin(), data.end(), out);
  if ( sz != data.size() )
    REPORT_AND_STOP("sizes reporting by deserialzie incorrect");
  if ( in != out )
    REPORT_AND_STOP("Content after deserialziation is different");
  
  return true;
}

int main(){
  if ( contentMany() == false ) 
    ABORT("contentMany check failed");

  if ( contentOne() == false ) 
    ABORT("contentOne check failed");
  
  MSG("OK", "test passed");
}
