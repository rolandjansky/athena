/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PscErrorCode_H
#define PscErrorCode_H
#include "TrigKernel/HltOnlHelper.h"
#include <string>
#include <stdint.h>

// Definition of different error codes for the PSC
namespace hltonl {
  enum PSCErrorCode {
    PSC_ERROR_UNCLASSIFIED = 0x0,
    PSC_ERROR_NO_L1_RESULT = 0x1,
    PSC_ERROR_SG_CLEAR_FAILED = 0x2,
    PSC_ERROR_NO_EVENTINFO = 0x3,
    PSC_ERROR_NO_VALID_EVENT = 0x4,
    PSC_ERROR_NO_HLTRESULT_FOUND = 0x10, 
    PSC_ERROR_NO_HLTRESULT_RETRIEVED = 0x11,
    PSC_ERROR_INVALID_CTP_RESULT = 0x101,
    PSC_ERROR_ROB_BUILD_FAILED = 0x102,
    NUM_PSC_ERROR_CODES = 9
  };

  /// helper class to map HLT PSC error code on a string name or a int index
  class MapPscErrorCode: virtual public hltonl::MapEnumeration<hltonl::PSCErrorCode> {
    public:
      MapPscErrorCode();
      virtual ~MapPscErrorCode() {};
  };

  /// helper function to print HLT PSC error code in human readable form 
  std::string PrintPscErrorCode(hltonl::PSCErrorCode) ;
}

inline hltonl::MapPscErrorCode::MapPscErrorCode() {
  // add error codes and description
  add(hltonl::PSC_ERROR_UNCLASSIFIED          ,"PSC_ERROR_UNCLASSIFIED") ;
  add(hltonl::PSC_ERROR_NO_L1_RESULT          ,"PSC_ERROR_NO_L1_RESULT") ;
  add(hltonl::PSC_ERROR_SG_CLEAR_FAILED       ,"PSC_ERROR_SG_CLEAR_FAILED") ;
  add(hltonl::PSC_ERROR_NO_EVENTINFO          ,"PSC_ERROR_NO_EVENTINFO") ;
  add(hltonl::PSC_ERROR_NO_VALID_EVENT        ,"PSC_ERROR_NO_VALID_EVENT") ;
  add(hltonl::PSC_ERROR_NO_HLTRESULT_FOUND    ,"PSC_ERROR_NO_HLTRESULT_FOUND") ;
  add(hltonl::PSC_ERROR_NO_HLTRESULT_RETRIEVED,"PSC_ERROR_NO_HLTRESULT_RETRIEVED") ;
  add(hltonl::PSC_ERROR_INVALID_CTP_RESULT    ,"PSC_ERROR_INVALID_CTP_RESULT") ;
  add(hltonl::PSC_ERROR_ROB_BUILD_FAILED      ,"PSC_ERROR_ROB_BUILD_FAILED") ;
  // return values in case of invalid code
  invalidCode("UNDEFINED",-1);
}
#endif
