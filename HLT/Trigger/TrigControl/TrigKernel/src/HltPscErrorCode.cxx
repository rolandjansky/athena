/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigKernel/HltPscErrorCode.h"

//
// helper function to print HLT PSC error code in human readable form 
// 
std::string hltonl::PrintPscErrorCode(hltonl::PSCErrorCode hlt_psc_error) {
  hltonl::MapPscErrorCode mapPscError;
  return mapPscError.codeToName(hlt_psc_error) ;
}
