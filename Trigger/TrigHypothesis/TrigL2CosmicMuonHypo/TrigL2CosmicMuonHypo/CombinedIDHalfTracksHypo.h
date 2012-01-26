/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDHalfTracksHypo_h__
#define __CombinedIDHalfTracksHypo_h__
/*
  CombinedIDHalfTracksHypo.h
*/
#include "TH1.h"
#include "TrigInterfaces/HypoAlgo.h"

class CombinedIDHalfTracksHypo : public HLT::HypoAlgo {
public:
  CombinedIDHalfTracksHypo(const std::string& name, ISvcLocator* svcloc);
  ~CombinedIDHalfTracksHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool&pass); 

private:
  //------------------------------------------------------------
  // Configurable properties
  //------------------------------------------------------------
  
  /** How many combined tracks need to be found to pass Hypo */
  int mCombIDNtracks;

  /** Delta Phi cut between the two tracks */
  float mCombID_DeltaPhi0Cut;

  /** Delta Eta cut between the two tracks */
  float mCombID_DeltaEtaCut;
   
  // HLT Services
  /** storegate pointer */
  StoreGateSvc* mStoreGate;
  
  //------------------------------------------------------------

  // Monitoring histograms
  /** histogram of deltaPhi between the 2 tracks after the Hypo cuts */
  std::vector<float> mHistIDdPhi_Cut;

  /** histogram of deltaEta between the 2 tracks after the Hypo cuts */
  std::vector<float> mHistIDdEta_Cut;
 

};

#endif // __CombinedIDHalfTracksHypo_h__


