/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecODEvent_p1_h
#define ALFA_LocRecODEvent_p1_h

#include <vector>

class ALFA_LocRecODEvent_p1
{
 public:
  // needed by athenaRoot 
  ALFA_LocRecODEvent_p1(){};
  friend class ALFA_LocRecODEventCnv_p1;
  
 private: 
  
  int m_iAlgoNum;
  int m_pot_num;
  int m_side;
  float m_y;
  
  float m_fOverY;
  int m_iNumY;

  std::vector<int> m_iFibSel;

};

     
#endif
     
