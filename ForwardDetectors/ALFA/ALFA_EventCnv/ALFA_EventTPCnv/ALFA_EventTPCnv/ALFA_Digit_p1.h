/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_Digit_p1_h
#define ALFA_Digit_p1_h

//#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"


class ALFA_Digit_p1
{
 public:
  // needed by athenaRoot 
  ALFA_Digit_p1()
  {
	m_station=0;
	m_plate=0;
	m_fiber=0;
  };
  friend class ALFA_DigitCnv_p1;
  
 private: 
  
  unsigned short m_station;
  unsigned short m_plate;
  unsigned short m_fiber;
  
};

     
#endif
     
