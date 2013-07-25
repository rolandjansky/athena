/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecODEvent_h
#define ALFA_GloRecODEvent_h

class ALFA_GloRecODEvent {
  
 public:
  
  ALFA_GloRecODEvent();
  ~ALFA_GloRecODEvent();
  
  
  ALFA_GloRecODEvent(int pot_num, int side, float y, float y_slope);
  
  inline int getPotNum() const {return m_pot_num;}
  inline int getSide() const {return m_side;}
  inline float getYposition() const { return m_y; }
  inline float getYslope() const {return m_y_slope;}
  
 private: 
  
  int   m_pot_num;
  int   m_side;
  float m_y;
  float m_y_slope;
};

#endif
