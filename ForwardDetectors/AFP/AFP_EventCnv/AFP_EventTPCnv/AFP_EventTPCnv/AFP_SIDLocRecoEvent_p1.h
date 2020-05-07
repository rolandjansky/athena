/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEvent_p1_h
#define AFP_SIDLocRecoEvent_p1_h

#include <vector>

class AFP_SIDLocRecoEvent_p1
{
 public:
  // needed by athenaRoot 
  AFP_SIDLocRecoEvent_p1(){};
  friend class AFP_SIDLocRecoEventCnv_p1;
  
 private: 
  
	
        int m_iAlgoNum = 0;
	int m_nStationID = 0;
	
	float m_x = 0;
	float m_y = 0;
	float m_z = 0;
	float m_x_slope = 0;
	float m_y_slope = 0;
	float m_z_slope = 0;
	
	float m_nHits = 0;
	float m_nHoles = 0;
        float m_fChi2 = 0;

};

     
#endif
     
