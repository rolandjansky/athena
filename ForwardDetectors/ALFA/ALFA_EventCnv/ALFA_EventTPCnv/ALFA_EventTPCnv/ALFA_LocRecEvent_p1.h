/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEvent_p1_h
#define ALFA_LocRecEvent_p1_h

#include <vector>

class ALFA_LocRecEvent_p1
{
 public:
  // needed by athenaRoot 
  ALFA_LocRecEvent_p1()
  {
	m_iAlgoNum=-1;
	m_pot_num=-1;
	m_x=-9999.0;
	m_y=-9999.0;

	m_fOverU=-9999.0;
	m_fOverV=-9999.0;
	m_iNumU=-1;
	m_iNumV=-1;;
  };
  friend class ALFA_LocRecEventCnv_p1;
  
 private: 
  
	int m_iAlgoNum;
	int m_pot_num;
	float m_x;
	float m_y;

	float m_fOverU;
	float m_fOverV;
	int m_iNumU;
	int m_iNumV;
	std::vector<int> m_iFibSel;
};

     
#endif
     
