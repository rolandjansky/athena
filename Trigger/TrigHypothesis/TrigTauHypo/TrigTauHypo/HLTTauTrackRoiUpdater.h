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

  float m_z0HalfWidth;
  std::string m_InputTrackColl;

  int m_nHitPix;      //<! at least n hits in pixels on lead track
  int m_nSiHoles;     //<! maximum number of Si holes on lead track

  bool m_updateEta;  //<! flag to determine whether Roi eta should be updated using the track direction (true by default)
  bool m_updatePhi;  //<! flag to determine whether Roi phi should be updated using the track direction (false by default)

};
#endif
