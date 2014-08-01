/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTNSWHYPO_H 
#define TRIG_MUFASTNSWHYPO_H 

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class MufastNSWHypo: public HLT::HypoAlgo
{
   public:
      MufastNSWHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~MufastNSWHypo();
    
      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
      HLT::ErrorCode hltFinalize();
    
   private:

      // cuts
      bool dThetaCut(double eta, double dTheta);
      bool dLCut(double eta, double dLeta, double dLphi);

      // Properties:
      BooleanProperty m_acceptAll;

      // Other members:   
      StoreGateSvc* m_storeGate;
    
      std::vector<float>::size_type m_bins;
      float m_fex_pt;
      float m_fex_eta;
      float m_fex_phi;
      float m_fex_tgcinn;
      float m_fex_inn;
      float m_fex_mid;
      float m_fex_out;
      float m_fex_dtheta;
      float m_fex_dleta;
      float m_fex_dlphi;
};

// --------------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------------
