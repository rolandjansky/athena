/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     BasicCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/BasicCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

// #include "CaloGeoHelpers/CaloSampling.h"
#include <sstream>

BasicCleanerTool::BasicCleanerTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  base_class(type, name, parent){ 
}
  
bool BasicCleanerTool::select(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float n90jet;
  if (jet -> getAttribute("N90Constituents", n90jet)){
    if(n90jet < m_n90Threshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve n90");
  }
      
  
  //example of code to compute presamplerFraction
  /*
  std::vector<float> vecvalue;
  if (jet->getAttribute(xAOD::JetAttribute::EnergyPerSampling, vecvalue)) {
     int vecsize = vecvalue.size();
     if (vecsize != 0) {
        int j = 0;
        for(const auto & thisvalue : vecvalue){
           ++j;

        if (vecsize != j) {
        }
        else {
           //see Reconstruction/Jet/JetUtils/Root/JetCaloQualityUtils.cxx
           double presamplerE = vecvalue[CaloSampling::PreSamplerB] + vecvalue[CaloSampling::PreSamplerE];
           double jetE = jet->jetP4(xAOD::JetEMScaleMomentum).E();
           double presamplerF = 0;
           if(jetE!=0) presamplerF = presamplerE/jetE;
           else presamplerF = -999;
        }
     }
  }
  */

  //use of presamplerFraction needs further study, for now not used
  /*
  float presamplerfractionjet;
  if(jet -> getAttribute<float>("presamplerFraction", presamplerfractionjet)){
    if(presamplerfractionjet > m_presamplerThreshold) {isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve presamplerFraction");
  }
  */

  float negEjet;
  if(jet -> getAttribute("NegativeE", negEjet)){
    if(negEjet < m_negativeEThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve NegativeE");
  }
    
  return isClean;

}

std::string BasicCleanerTool::toString() const noexcept{
  std::stringstream ss;
  ss << getName() 
     <<  ": n90Threshold: "
     <<  m_n90Threshold
     <<  " m_presamplerThreshold: " 
     <<  m_presamplerThreshold
     <<  "m_negativeEThreshold: "
     <<  m_negativeEThreshold 
     << '\n';
  return ss.str();
}

std::string 
BasicCleanerTool::getName() const noexcept {return "BasicCleanerTool";}
