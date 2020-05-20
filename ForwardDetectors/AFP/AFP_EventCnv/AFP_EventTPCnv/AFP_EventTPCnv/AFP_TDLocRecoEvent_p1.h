/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  

        int m_iAlgoNum = 0;
	
	int m_nStationID = 0;
	int m_nDetectorID = 0;
	int m_nTrainID = 0;
	
	float m_fTrainTime = 0;
	int m_nTrainSize = 0;
	int m_nSaturation = 0;



};

     
#endif
     
