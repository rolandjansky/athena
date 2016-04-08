/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEvent_p1_h
#define ALFA_GloRecEvent_p1_h

class ALFA_GloRecEvent_p1
{
 public:
  // needed by athenaRoot 
  ALFA_GloRecEvent_p1()
  {
	m_arm=-1;
	m_x=-9999.0;
	m_y=-9999.0;
	m_x_slope=-9999.0;
	m_y_slope=-9999.0;
  };
  friend class ALFA_GloRecEventCnv_p1;
  
 private: 
  int   m_arm;    
  float m_x;
  float m_y;
  float m_x_slope;
  float m_y_slope;  
//  ALFA_LocRecCorrEvent * m_near_LocRecCorr;
//  ALFA_LocRecCorrEvent * m_far_LocRecCorr;
};
    
#endif // ALFA_GloRecEvent_p1_h
     
