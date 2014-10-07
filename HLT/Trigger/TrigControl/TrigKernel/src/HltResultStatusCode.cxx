/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigKernel/HltResultStatusCode.h"

//
// helper function to print HLT result status code in human readable form 
// 
std::string hltonl::PrintHltResultStatusCode(hltonl::ResultStatusCode hlt_status_code) {
  hltonl::MapResultStatusCode mapResultStatus;
  return mapResultStatus.codeToName(hlt_status_code) ;
}
