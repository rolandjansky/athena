/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFCOMBINERDIMUONMASSHYPO_H 
#define TRIG_TRIGMUONEFCOMBINERDIMUONMASSHYPO_H

#include <string>
#include <algorithm>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFCombinerDiMuonMassHypo: public HLT::HypoAlgo
{
   public:
      TrigMuonEFCombinerDiMuonMassHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~TrigMuonEFCombinerDiMuonMassHypo();
      
      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
      HLT::ErrorCode hltFinalize();
      HLT::ErrorCode hltBeginRun();

   private:

      // Properties
      BooleanProperty m_acceptAll;
      double          m_massThresLow;
      double          m_massThresHigh;
      int             m_signRequirement;
  
      // Monitoring variables
      std::vector<int>   m_mon_nMuons;
      std::vector<float> m_mon_chargeMult;
      std::vector<float> m_mon_invMass;
      std::vector<float> m_mon_muonPt;
      std::vector<float> m_mon_muonEta;

      // Functions
      double invMass(double m1, double pt1, double eta1, double phi1,
		     double m2, double pt2, double eta2, double phi2);

      // Other members
      StoreGateSvc* m_storeGate;
};

#endif
