/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONIDTRACKMULTIHYPO_H 
#define TRIG_TRIGMUONIDTRACKMULTIHYPO_H

#include <string>
#include <algorithm>
#include "TrigInterfaces/AllTEAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonIDTrackMultiHypo: public HLT::AllTEAlgo
{
   public:
      TrigMuonIDTrackMultiHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~TrigMuonIDTrackMultiHypo();
      
      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
      HLT::ErrorCode hltFinalize();
      HLT::ErrorCode hltBeginRun();

   private:
      // Properties
      BooleanProperty m_acceptAll;
      BooleanProperty m_UseMuon;
      std::vector<float> m_ptBins1;
      std::vector<float> m_ptBins2;
      std::vector<float> m_ptBins3;
      std::vector<float> m_ptThresholds1;
      std::vector<float> m_ptThresholds2;
      std::vector<float> m_ptThresholds3;
      int m_Multiplicity1;
      int m_Multiplicity2;
      int m_Multiplicity3;
      float m_LowMassCut;
      float m_HighMassCut;
      std::string m_TrkAlgo;
      BooleanProperty m_UseMuRoiDrOnly;
      BooleanProperty m_UseMuRoiDr;
      float m_MuRoiDrMax;
      float m_MuRoiDrMin;

      // Monitoring variables
      std::vector<int>   m_mon_nTracks;
      std::vector<float> m_mon_TrkPt;
      std::vector<float> m_mon_MuRoiDr;
      /* std::vector<float> m_mon_invMass; */

      // Functions
      float deltaPhi(const float& v1, const float& v2);
      float deltaEta(const float& v1, const float& v2);
      float deltaR(const float& eta1, const float& phi1,const float& eta2, const float& phi2);
};

#endif
