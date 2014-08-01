/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
   Hypo for multiple muons
   condition is formulated in bins of pT/Eta/multiplicities
*/

#ifndef TRIG_TRIGMUONEFTRACKBUILDERMULTIHYPO_H
#define TRIG_TRIGMUONEFTRACKBUILDERMULTIHYPO_H

#include <string>
#include <algorithm>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFTrackBuilderMultiHypo: public HLT::HypoAlgo {
      enum { MaxNumberTools = 20 };  
   public:
      TrigMuonEFTrackBuilderMultiHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~TrigMuonEFTrackBuilderMultiHypo();

      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
      HLT::ErrorCode hltFinalize();

   private:

      // Properties:
      std::vector<float> m_ptBins;
      std::vector<float> m_ptThresholds;
      std::vector<unsigned int> m_ptMultiplicity;  // muon multiplicity id, eg. {1,1,1,1,2,2,2,2,3,3,3,3}
      BooleanProperty m_acceptAll;
  
      // Other members:   
      StoreGateSvc* m_storeGate;
      std::vector<float>::size_type m_bins;
      std::vector<float> m_fex_pt;
      std::vector<float> m_fex_eta;
      std::vector<float> m_fex_phi;
      std::vector<unsigned int> m_fex_nmuons;
      unsigned int m_Nmult;                             // muon multiplicity, calculated in hltInitialize() method
      std::vector<unsigned int> m_masks;                // bit mask to verify hypo
      std::vector<float>::size_type m_mult;

      // Functions
      int bitsum(unsigned long il);
};

#endif
