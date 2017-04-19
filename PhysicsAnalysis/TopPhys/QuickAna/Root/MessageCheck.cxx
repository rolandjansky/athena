/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/MessageCheck.h>

#include <PATInterfaces/CorrectionCode.h>
#include <PATInterfaces/SystematicCode.h>

//
// method implementations
//

namespace ana
{
  ANA_MSG_SOURCE (msgToolHandle, "QuickAna_AnaToolHandle")
  ANA_MSG_SOURCE (msgConfiguration, "QuickAna_Configuration")
  ANA_MSG_SOURCE (msgIsolationTools, "QuickAna_IsolationTools")
  ANA_MSG_SOURCE (msgStore, "QuickAna_Store")
  ANA_MSG_SOURCE (msgScheduler, "QuickAna_Scheduler")
  ANA_MSG_SOURCE (msgSelection, "QuickAna_Selection")
  ANA_MSG_SOURCE (msgObjectDefinition, "QuickAna_ObjectDefinition")
  ANA_MSG_SOURCE (msgKinSelect, "QuickAna_KinSelect")
  ANA_MSG_SOURCE (msgUserCode, "QuickAna_UserCode")

  void enableStatusCodeFailure ()
  {
#ifdef ROOTCORE
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();
#endif
    CP::SystematicCode::enableFailure();
    CP::CorrectionCode::enableFailure();
  }
}
