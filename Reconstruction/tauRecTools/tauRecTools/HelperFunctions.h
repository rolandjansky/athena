#/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_HELPERFUNCTIONS_H
#define TAURECTOOLS_HELPERFUNCTIONS_H

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/Jet.h"
#include "xAODTau/TauJet.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

#include "AsgMessaging/MessageCheck.h"

#include "MVAUtils/BDT.h"
#include "TLorentzVector.h"
#include "TString.h"

#include <vector>
#include <map>


namespace tauRecTools
{
  ANA_MSG_HEADER(msgHelperFunction)

  /**
   * @brief Return the vertex of jet candidate
   * @warning In trigger, jet candidate does not have a candidate, and an ERROR
   *          message will be print out !
   */ 
  const xAOD::Vertex* getJetVertex(const xAOD::Jet& jet);
 
  /**
   * @brief Return the vertex of tau candidate
   *        If the vertex link of tau candidate is valid, then the vertex which the link point to will
   *        be returned. Otherwise, it will try to retrieve the vertex of the seed jet in offline reconstruction
   */  
  const xAOD::Vertex* getTauVertex(const xAOD::TauJet& tau, bool inTrigger = false);

  /**
   * @brief Return the four momentum of the tau axis
   *        The tau axis is widely used to select clusters and cells in tau reconstruction.
   *        If doVertexCorrection is true, then IntermediateAxis is returned. Otherwise, 
   *        DetectorAxis is returned.  
   */ 
  TLorentzVector getTauAxis(const xAOD::TauJet& tau, bool doVertexCorrection = true);

  TLorentzVector GetConstituentP4(const xAOD::JetConstituent& constituent);

  xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag);
  bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2);

  std::unique_ptr<MVAUtils::BDT> configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile);
  // initialise the BDT and return the list of input variable names
  std::unique_ptr<MVAUtils::BDT> configureMVABDT(std::vector<TString>& variableNames, const TString& weightFile);

  std::vector<TString> parseString(const TString& str, const TString& delim=",");
  std::vector<TString> parseStringMVAUtilsBDT(const TString& str, const TString& delim=",");
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
