/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_Digit_h
#define ALFA_Digit_h

class ALFA_Digit {
  
 public:
  
  ALFA_Digit();
  ALFA_Digit(const unsigned short station, const unsigned short plate, const unsigned short fiber);
  
  inline unsigned short getStation() const { return m_station; }  
  inline unsigned short getPlate()   const { return m_plate; }
  inline unsigned short getFiber()   const { return m_fiber; }
  
 private: 
  
  unsigned short m_station; 
  unsigned short m_plate;
  unsigned short m_fiber;
};

#endif
