/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_HELPERFUNCTIONS_H
#define TAURECTOOLS_HELPERFUNCTIONS_H

#include "MVAUtils/BDT.h"

#include <vector>

// ROOT include(s)
#include "TLorentzVector.h"
#include "TString.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

#define TRT_CHECK_BOOL( dec, action )		\
  do {						\
    if (!dec) {					\
      ATH_MSG_ERROR("TauRecTools failed");	\
      return action;				\
    }						\
  } while(0)

namespace tauRecTools
{

  void createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s);
  
  xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag);
  bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2);

  MVAUtils::BDT* configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile);

  std::vector<TString> parseString(const TString& str, const TString& delim=",");
  std::vector<TString> parseStringMVAUtilsBDT(const TString& str, const TString& delim=",");
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
