#/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_HELPERFUNCTIONS_H
#define TAURECTOOLS_HELPERFUNCTIONS_H

#include "AsgMessaging/MessageCheck.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

#include "MVAUtils/BDT.h"

#include "TLorentzVector.h"
#include "TString.h"

#include <vector>
#include <map>


namespace tauRecTools
{
  ANA_MSG_HEADER(msgHelperFunction)

  const StatusCode GetJetClusterList(const xAOD::Jet* jet, std::vector<const xAOD::CaloCluster*> &clusterList, bool useSubtractedCluster);

  xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag);
  bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2);

  std::unique_ptr<MVAUtils::BDT> configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile);
  // initialise the BDT and return the list of input variable names
  std::unique_ptr<MVAUtils::BDT> configureMVABDT(std::vector<TString>& variableNames, const TString& weightFile);

  std::vector<TString> parseString(const TString& str, const TString& delim=",");
  std::vector<TString> parseStringMVAUtilsBDT(const TString& str, const TString& delim=",");
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
