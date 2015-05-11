/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigKernel/HltAcceptFlag.h"

//
// helper function to print HLT accept flag in human readable form 
// 
std::string hltonl::PrintHltAcceptFlag(hltonl::AcceptFlag hlt_flag) {
  hltonl::MapAcceptFlag mapAccept;
  return mapAccept.codeToName(hlt_flag) ;
}
