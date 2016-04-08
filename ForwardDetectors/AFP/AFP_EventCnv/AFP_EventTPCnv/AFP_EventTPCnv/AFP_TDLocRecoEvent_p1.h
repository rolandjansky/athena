/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEvent_p1_h
#define AFP_TDLocRecoEvent_p1_h

#include <vector>

class AFP_TDLocRecoEvent_p1
{
 public:
  // needed by athenaRoot 
  AFP_TDLocRecoEvent_p1(){};
  friend class AFP_TDLocRecoEventCnv_p1;
  
 private: 
  
	int m_iAlgoNum;
	
	int m_nStationID;
	int m_nDetectorID;
	int m_nTrainID;
	
	float m_fTrainTime;
	int m_nTrainSize;
	int m_nSaturation;

};

     
#endif
     
