/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltResultStatusCode_H
#define HltResultStatusCode_H
#include "TrigKernel/HltOnlHelper.h"
#include <string>
#include <stdint.h>

// Definition of different status codes for HLT result
namespace hltonl {
  enum ResultStatusCode {
    NORMAL_HLT_RESULT = 0x0,
    DUMMY_HLT_RESULT = 0x1,
    NORMAL_HLT_RESULT_TRUNCATED = 0x100,
    DUMMY_HLT_RESULT_TRUNCATED = 0x101,
    NUM_HLT_STATUS_CODES = 4
  };

  /// helper class to map HLT error code on a string name or a int index
  class MapResultStatusCode: virtual public hltonl::MapEnumeration<hltonl::ResultStatusCode> {
    public:
      MapResultStatusCode();
      virtual ~MapResultStatusCode() {};
  };

  /// helper function to print an HLT status code in human readable form
  std::string PrintHltResultStatusCode(hltonl::ResultStatusCode) ;
}

inline hltonl::MapResultStatusCode::MapResultStatusCode() {
  // add status codes and description
  add(hltonl::NORMAL_HLT_RESULT           ,"NORMAL_HLT_RESULT") ;
  add(hltonl::DUMMY_HLT_RESULT            ,"DUMMY_HLT_RESULT") ;
  add(hltonl::NORMAL_HLT_RESULT_TRUNCATED ,"NORMAL_HLT_RESULT_TRUNCATED") ;
  add(hltonl::DUMMY_HLT_RESULT_TRUNCATED  ,"DUMMY_HLT_RESULT_TRUNCATED") ;
  // return values in case of invalid code
  invalidCode("UNDEFINED",-1);
}
#endif
