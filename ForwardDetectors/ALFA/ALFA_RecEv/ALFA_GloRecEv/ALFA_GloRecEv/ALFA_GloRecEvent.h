/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEvent_h
#define ALFA_GloRecEvent_h

#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"


class ALFA_GloRecEvent {
///////////////////////////////////////////////////////
//
//   GloRecEvent class contains information
//   about global tracks reconstructed from
//   LocRecCorr information
//
//   m_arm = arm of the track 
//           i.e. 1 - left  upper A7L1U && B7L1U
//                2 - left  lower A7L1L && B7L1L
//                3 - right upper A7R1U && B7R1U
//                4 - right lower A7R1L && B7R1L
//
//   m_x, m_y ... intercept coordinates
//                halfway between the pots

//   m_x_slope, m_y_slope ... slopes
//
//   m_near_LocRecCorr
//   m_far_LocRecCorr  ... are the pointers
//                         to the local hits
//                         the global track is
//                         calculated from
//                     ... the pointers exist in the
//                         transient version
//                     ... they may not exist in the
//                         persistent format
//
///////////////////////////////////////////////////////
  
 public:
  
  ALFA_GloRecEvent();
  ~ALFA_GloRecEvent();
  
  
  
  ALFA_GloRecEvent( int arm,
                    float x,
                    float y,
                    float x_slope,
                    float y_slope,
                    ALFA_LocRecCorrEvent * near_hit,
                    ALFA_LocRecCorrEvent * far_hit);

  inline int   getArmNum()    const { return m_arm;}
  inline float getXposition() const { return m_x;}
  inline float getYposition() const { return m_y;}
  inline float getXslope() const {return m_x_slope;}
  inline float getYslope() const {return m_y_slope;}
  inline ALFA_LocRecCorrEvent * getNearLocRecCorr() const {return m_near_LocRecCorr;}
  inline ALFA_LocRecCorrEvent * getFarLocRecCorr() const {return m_far_LocRecCorr;}
  
 private: 
  int   m_arm;
  float m_x;
  float m_y;
  float m_x_slope;
  float m_y_slope;
  ALFA_LocRecCorrEvent * m_near_LocRecCorr;
  ALFA_LocRecCorrEvent * m_far_LocRecCorr;

};

#endif // ALFA_GloRecEvent_h
