#/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_HELPERFUNCTIONS_H
#define TAURECTOOLS_HELPERFUNCTIONS_H

#include "MVAUtils/BDT.h"
#include "AsgMessaging/MessageCheck.h"

#include <vector>
#include <map>

// ROOT include(s)
#include "TLorentzVector.h"
#include "TString.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

namespace tauRecTools
{
  ANA_MSG_HEADER(msgHelperFunction)

    const StatusCode GetJetClusterList(const xAOD::Jet* jet, std::vector<const xAOD::CaloCluster*> &clusterList, bool incShowerSubtracted, TLorentzVector dRVector = TLorentzVector(0.,0.,0.,0.), double dRCut = -1);

  xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag);
  bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2);

  std::unique_ptr<MVAUtils::BDT> configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile);

  std::vector<TString> parseString(const TString& str, const TString& delim=",");
  std::vector<TString> parseStringMVAUtilsBDT(const TString& str, const TString& delim=",");
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
