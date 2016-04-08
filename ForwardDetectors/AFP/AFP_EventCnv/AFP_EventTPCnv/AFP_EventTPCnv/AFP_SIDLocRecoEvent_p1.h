/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
	int m_iAlgoNum;
	int m_nStationID;
	
	float m_x;
	float m_y;
	float m_z;
	float m_x_slope;
	float m_y_slope;
	float m_z_slope;
	
	float m_nHits;
	float m_nHoles;
	float m_fChi2;	

};

     
#endif
     
