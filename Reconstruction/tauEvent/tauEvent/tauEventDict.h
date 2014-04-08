/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// Definitions for dictionary generator
//-----------------------------------------------------------------------------
#ifndef TAU_EVENT_DICT_H
#define TAU_EVENT_DICT_H

#include "tauEvent/tauAnalysisHelperObject.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauJetParameters.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauPID.h"
#include "tauEvent/TauPi0Candidate.h"
#include "tauEvent/TauPi0Details.h"
#include "tauEvent/TauPi0Cluster.h"
#include "tauEvent/TauShot.h"
#include "TSystem.h"

namespace tauEventDict {

  struct temp {
    std::bitset<32> m_bitset;
    std::vector<
        std::vector<
        ElementLink<CaloCellContainer> > > v1;
    std::vector<
        ElementLink<CaloCellContainer> > v2;
   
    std::vector<Analysis::TauDetails*> v3;
    std::vector<Analysis::TauPi0Candidate*> v4;
    std::vector<Analysis::TauPi0Cluster*> v5;
    std::vector<Analysis::TauShot*> v6;
    
    // Get rid of warnings.
    temp(const temp&);
  };

// copied from egammaEvent/egammaEventDict.h
// Ensure that the dictionary for the enums is loaded when this one is.
struct EnumDictLoader
{
  EnumDictLoader();
};

EnumDictLoader::EnumDictLoader()
{
  // ??? W32 doesn't use lib...
  gSystem->Load ("libtauEnumsDict");
}

EnumDictLoader enumDictLoader;

} //> end namespace tauEventDict


#endif // TAU_EVENT_DICT_H
