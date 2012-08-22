/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODDigit_h
#define ALFA_ODDigit_h

class ALFA_ODDigit {
  
 public:
  
  ALFA_ODDigit();
  ALFA_ODDigit(const unsigned short station, const unsigned short side, const unsigned short plate, const unsigned short fiber);
  
  inline unsigned short getStation() const { return m_station; }
  inline unsigned short getSide()    const { return m_side; }    
  inline unsigned short getPlate()   const { return m_plate; }
  inline unsigned short getFiber()   const { return m_fiber; }
  
 private: 
  
  unsigned short m_station;
  unsigned short m_side;   
  unsigned short m_plate;
  unsigned short m_fiber;
};

#endif
