// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTAU_VERSIONS_TAUJETCALIBMAPPER_V3_H
#define XAODTAU_VERSIONS_TAUJETCALIBMAPPER_V3_H

// System include(s):
#include <iostream>
#include "xAODTau/TauDefs.h"
namespace xAOD {
struct TauJetCalibTypeMapper_v3 {
  bool getValue(TauJetParameters::TauCalibType type, std::string& stringPt, std::string& stringEta, std::string& stringPhi, std::string& stringM){
    switch (type){
    case TauJetParameters::JetSeed:
      stringPt = "ptJetSeed"; 
      stringEta = "etaJetSeed";
      stringPhi = "phiJetSeed";
      stringM = "mJetSeed";
      return true;
    case TauJetParameters::DetectorAxis:
      stringPt = "ptDetectorAxis"; 
      stringEta = "etaDetectorAxis";
      stringPhi = "phiDetectorAxis";
      stringM = "mDetectorAxis";
      return true;
    case TauJetParameters::IntermediateAxis:
      stringPt = "ptIntermediateAxis"; 
      stringEta = "etaIntermediateAxis";
      stringPhi = "phiIntermediateAxis";
      stringM = "mIntermediateAxis";
      return true;
    case TauJetParameters::TauEnergyScale:
      stringPt = "ptTauEnergyScale"; //only energy is scaled
      stringEta = "";
      stringPhi = "";
      stringM = "";
      return true;
    case TauJetParameters::TauEtaCalib:
      stringPt = "ptTauEtaCalib";
      stringEta = "etaTauEtaCalib";//only eta changes, but pt must be be adjusted, so we store pt as well
      stringPhi = "";
      stringM = "";
      return true;
    case TauJetParameters::PanTauCellBasedProto:
      stringPt = "ptPanTauCellBasedProto"; 
      stringEta = "etaPanTauCellBasedProto";
      stringPhi = "phiPanTauCellBasedProto";
      stringM = "mPanTauCellBasedProto";
      return true;
    case TauJetParameters::PanTauCellBased:
      stringPt = "ptPanTauCellBased"; 
      stringEta = "etaPanTauCellBased";
      stringPhi = "phiPanTauCellBased";
      stringM = "mPanTauCellBased";
      return true;              
    case TauJetParameters::TrigCaloOnly:
      stringPt = "ptTrigCaloOnly"; 
      stringEta = "etaTrigCaloOnly";
      stringPhi = "phiTrigCaloOnly";
      stringM = "mTrigCaloOnly";
      return true;
    }//switch
    return false;
  }//getValue

};

}

#endif
