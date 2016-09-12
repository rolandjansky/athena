// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoMCWeightFixAlg.h 772715 2016-09-12 11:37:15Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTINFOMCWEIGHTFIXALG_H
#define XAODEVENTINFOCNV_EVENTINFOMCWEIGHTFIXALG_H

// System include(s):
#include <string>

// Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

/// Algorithm to fix the ATLASSIM-2989 issue with mc weights in EventInfo
///
/// It should be used in AODFix for 20.7 xAOD.
///
/// @author Will Buttinger <Will.Buttinger@cern.ch>
///
/// $Revision: 772715 $
/// $Date: 2016-09-12 13:37:15 +0200 (Mon, 12 Sep 2016) $
///
class EventInfoMCWeightFixAlg: public ::AthAlgorithm { 

public: 
   /// Regular algorithm constructor
   EventInfoMCWeightFixAlg( const std::string& name,
                            ISvcLocator* pSvcLocator );

   /// Function executing the algorithm
   virtual StatusCode execute();

private:
   /// Key of the truth event container
   std::string m_truthEventsKey;

}; // class EventInfoMCWeightFixAlg

#endif // XAODEVENTINFOCNV_EVENTINFOMCWEIGHTFIXALG_H
