/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFEXTRAPOLATORNSWHYPO_H 
#define TRIG_TRIGMUONEFEXTRAPOLATORNSWHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFExtrapolatorNSWHypo: public HLT::HypoAlgo
{
   public:
      TrigMuonEFExtrapolatorNSWHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~TrigMuonEFExtrapolatorNSWHypo();

      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
      HLT::ErrorCode hltFinalize();

   private:

      // Properties
      BooleanProperty m_acceptAll;

      // ToolHandle for EDM helper tool
      ToolHandle<Muon::MuonEDMHelperTool>  m_edmhelperTool;

      // ToolHandle for IdHelperTool
      ToolHandle<Muon::MuonIdHelperTool>  m_idhelperTool;

      // Services
      StoreGateSvc* m_storeGate;

      // cuts
      bool dThetaCut(double eta, double dTheta);
      bool dLCut(double eta, double dLeta, double dLphi);

      // Monitoring variables
      int m_fex_n_seg_passed; 
      int m_fex_n_seg_sameside;
      float m_fex_failed_eta;
      float m_fex_failed_phi;
      std::vector<float> m_fex_dTheta;
      std::vector<float> m_fex_dLeta;
      std::vector<float> m_fex_dLphi;
};

#endif
