/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     BasicCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/BasicCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

// #include "CaloGeoHelpers/CaloSampling.h"
#include <sstream>

BasicCleaner::BasicCleaner(float n90Threshold,
                           float presamplerThreshold,
                           float negativeEThreshold):
  m_n90Threshold(n90Threshold), 
  m_presamplerThreshold(presamplerThreshold),
  m_negativeEThreshold(negativeEThreshold){
  }
  
bool BasicCleaner::operator()(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float n90jet;
  if (jet -> getAttribute("N90Constituents", n90jet)){
    //std::cout << "TrigHLTJetHypoHelpers: N90Constituents = " << n90jet << '\n';
    if(n90jet < m_n90Threshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve n90");
  }
      

  //example of code to compute presamplerFraction
  /*
  std::vector<float> vecvalue;
  if (jet->getAttribute(xAOD::JetAttribute::EnergyPerSampling, vecvalue)) {
     int vecsize = vecvalue.size();
     std::cout << "TrigHLTJetHypoHelpers: Got EnergyPerSampling vector, size: " << vecsize << '\n';
     if (vecsize != 0) {
        int j = 0;
        for(const auto & thisvalue : vecvalue){
           ++j;
           std::cout << "TrigHLTJetHypoHelpers: EnergyPerSampling #" << j << " = " << thisvalue << '\n';
        }
        std::cout << "TrigHLTJetHypoHelpers: size of attribute vector == number of displayed attributes: " << (vecsize == j) << '\n';
        if (vecsize != j) {
           std::cout << "TrigHLTJetHypoHelpers: Problem with displaying this attribute" << '\n';
        }
        else {
           //see Reconstruction/Jet/JetUtils/Root/JetCaloQualityUtils.cxx
           double presamplerE = vecvalue[CaloSampling::PreSamplerB] + vecvalue[CaloSampling::PreSamplerE];
           std::cout << "TrigHLTJetHypoHelpers: presamplerE = " << presamplerE << '\n';
           double jetE = jet->jetP4(xAOD::JetEMScaleMomentum).E();
           std::cout << "TrigHLTJetHypoHelpers: jetE = " << jetE << '\n';
           double presamplerF = 0;
           if(jetE!=0) presamplerF = presamplerE/jetE;
           else presamplerF = -999;
           std::cout << "TrigHLTJetHypoHelpers: presamplerF = " << presamplerF << '\n';
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
    throw UncleanableJet("Cleaner Cannot retrieve presamplerFraction");
  }
  */

  float negEjet;
  if(jet -> getAttribute("NegativeE", negEjet)){
    //std::cout << "TrigHLTJetHypoHelpers: NegativeE = " << negEjet << '\n';
    if(negEjet < m_negativeEThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve NegativeE");
  }
    
  return isClean;

}

std::string BasicCleaner::toString() const noexcept{
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
BasicCleaner::getName() const noexcept {return "BasicCleaner";}
