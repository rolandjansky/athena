/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CosmicTrtHypo_h__
#define __CosmicTrtHypo_h__
/*
  CosmicTrtHypo.h
*/
#include "TH1.h"
#include "TrigInterfaces/HypoAlgo.h"

class CosmicTrtHypo : public HLT::HypoAlgo {
public:
  CosmicTrtHypo(const std::string& name, ISvcLocator* svcloc);
  ~CosmicTrtHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:
  //------------------------------------------------------------
  // Configurable properties
  //------------------------------------------------------------
  
  /** How many segments need to be found to pass Hypo */
  int mNTrtSegs;
  /** number of trt hits on segment to pass Hypo */
  int mNTrthitsCut;
  /** max value (abs) of d0 of segment to pass Hypo */
  double mTrtSegD0Cut;

  /** storegate pointer */
  StoreGateSvc* mStoreGate;
  
  //------------------------------------------------------------



  //tracking quantities to be monitored
   float m_ntracksHypo;
   
     
  // Monitoring histograms
   /** A vector of d0 of accepted segments for monitoring */
   std::vector<float> mAcceptedTrack_a0;
   /** A vector of pt of accepted segments for monitoring */
   std::vector<float> mAcceptedTrack_pt;
   /** A vector of phi0 of accepted segments for monitoring */
   std::vector<float> mAcceptedTrack_phi0;
   /** A vector of z0 of accepted segments for monitoring */
   std::vector<float> mAcceptedTrack_z0;
   /** A vector of eta of accepted segments for monitoring */
   std::vector<float> mAcceptedTrack_eta;
   /** A vector of nStraw_TRT of accepted segments for monitoring */
   std::vector<float> m_nStraw_TRT_hypo;
   /** A vector of nTR_TRT of accepted segments for monitoring */
   std::vector<float> m_nTR_TRT_hypo;

  

 
};

#endif // 
