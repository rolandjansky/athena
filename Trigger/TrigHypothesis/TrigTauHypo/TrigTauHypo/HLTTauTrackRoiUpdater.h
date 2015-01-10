/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_HLTTAUTRACKROIUPDATER_H
#define TRIGTAUHYPO_HLTTAUTRACKROIUPDATER_H

#include "TrigInterfaces/FexAlgo.h"

class HLTTauTrackRoiUpdater : public HLT::FexAlgo 
{ 
    
 public:   

  HLTTauTrackRoiUpdater(const std::string &name, ISvcLocator *pSvcLocator); 
  ~HLTTauTrackRoiUpdater();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* outputTE);    
  HLT::ErrorCode hltFinalize();

 private:

  float m_deltaRLeadTrkRoI;
  float m_z0HalfWidth;
  float m_minTrackPt;
  std::string m_InputTrackColl;
  
};
#endif
