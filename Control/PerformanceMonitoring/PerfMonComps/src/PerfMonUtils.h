///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonUtils.h 
// Header file for a few utilities related to managing auditors
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONCOMPS_PERFMONUTILS_H 
#define PERFMONCOMPS_PERFMONUTILS_H 

// Framework includes
#include "GaudiKernel/StatusCode.h"

// forward declares
class IAuditorSvc;
class MsgStream;

namespace PerfMon {

/// simple function to factorize boring things such as asking the @c AuditorSvc
/// if an auditor is there (and scheduled) and make sure to create it if it is
/// not yet alived.
StatusCode makeAuditor( const std::string& audName, IAuditorSvc* audSvc,
                        MsgStream& msg );

} //> end namespace PerfMon

#endif // PERFMONCOMPS_PERFMONUTILS_H
