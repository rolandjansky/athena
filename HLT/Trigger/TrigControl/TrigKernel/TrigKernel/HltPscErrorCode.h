/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PscErrorCode_H
#define PscErrorCode_H
#include "TrigKernel/HltOnlHelper.h"
#include <string>
#include <stdint.h>

// Definition of different error codes for the PSC
namespace hltonl {
  enum class PSCErrorCode : uint32_t {
    UNCLASSIFIED          = 0,
    BEFORE_NEXT_EVENT     = 1,
    CANNOT_RETRIEVE_EVENT = 2,
    NO_EVENT_INFO         = 3,
    SCHEDULING_FAILURE    = 4,
    CANNOT_ACCESS_SLOT    = 5,
    PROCESSING_FAILURE    = 6,
    NO_HLT_RESULT         = 7,
    OUTPUT_BUILD_FAILURE  = 8,
    OUTPUT_SEND_FAILURE   = 9,
    AFTER_RESULT_SENT     = 10,
    COOL_UPDATE           = 11
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
  add(hltonl::PSCErrorCode::UNCLASSIFIED,          "PSCErrorCode::UNCLASSIFIED");
  add(hltonl::PSCErrorCode::BEFORE_NEXT_EVENT,     "PSCErrorCode::BEFORE_NEXT_EVENT");
  add(hltonl::PSCErrorCode::CANNOT_RETRIEVE_EVENT, "PSCErrorCode::CANNOT_RETRIEVE_EVENT");
  add(hltonl::PSCErrorCode::NO_EVENT_INFO,         "PSCErrorCode::NO_EVENT_INFO");
  add(hltonl::PSCErrorCode::SCHEDULING_FAILURE,    "PSCErrorCode::SCHEDULING_FAILURE");
  add(hltonl::PSCErrorCode::CANNOT_ACCESS_SLOT,    "PSCErrorCode::CANNOT_ACCESS_SLOT");
  add(hltonl::PSCErrorCode::PROCESSING_FAILURE,    "PSCErrorCode::PROCESSING_FAILURE");
  add(hltonl::PSCErrorCode::NO_HLT_RESULT,         "PSCErrorCode::NO_HLT_RESULT");
  add(hltonl::PSCErrorCode::OUTPUT_BUILD_FAILURE,  "PSCErrorCode::OUTPUT_BUILD_FAILURE");
  add(hltonl::PSCErrorCode::OUTPUT_SEND_FAILURE,   "PSCErrorCode::OUTPUT_SEND_FAILURE");
  add(hltonl::PSCErrorCode::AFTER_RESULT_SENT,     "PSCErrorCode::AFTER_RESULT_SENT");
  add(hltonl::PSCErrorCode::COOL_UPDATE,           "PSCErrorCode::COOL_UPDATE");
  // return values in case of invalid code
  invalidCode("UNDEFINED",-1);
}
#endif
