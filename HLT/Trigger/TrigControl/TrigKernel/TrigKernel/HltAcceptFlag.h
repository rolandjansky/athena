/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltAcceptFlag_H
#define HltAcceptFlag_H
#include "TrigKernel/HltOnlHelper.h"
#include <string>
#include <stdint.h>

// Definition of different acceptance flags
namespace hltonl {
  enum AcceptFlag {
        PSC_FORCED_REJECT = -4,
        STEERING_REJECT,
        PSC_UNDEFINED,
        PSC_DEBUG,
        STEERING_DEBUG,
        STEERING_ACCEPT,
        PSC_FORCED_ACCEPT,
        NUM_ACCEPTANCE_FLAGS = 7
  };

  /// helper class to map accept codes to string names or a int indexes
  class MapAcceptFlag: virtual public hltonl::MapEnumeration<hltonl::AcceptFlag> {
    public:
      MapAcceptFlag();
      virtual ~MapAcceptFlag() {};
  };

  /// helper function to print HLT accept flag in human readable form
  std::string PrintHltAcceptFlag(hltonl::AcceptFlag) ;
}

inline hltonl::MapAcceptFlag::MapAcceptFlag() {
  // add error codes and description
  add(hltonl::PSC_FORCED_REJECT ,"PSC_FORCED_REJECT") ;
  add(hltonl::STEERING_REJECT   ,"STEERING_REJECT") ;
  add(hltonl::PSC_UNDEFINED     ,"PSC_UNDEFINED") ;
  add(hltonl::PSC_DEBUG         ,"PSC_DEBUG") ;
  add(hltonl::STEERING_DEBUG    ,"STEERING_DEBUG") ;
  add(hltonl::STEERING_ACCEPT   ,"STEERING_ACCEPT") ;
  add(hltonl::PSC_FORCED_ACCEPT ,"PSC_FORCED_ACCEPT") ;
  // return values in case of invalid code
  invalidCode("UNDEFINED",-10);
}
#endif
