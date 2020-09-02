/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARWAVEDERIVEDQUANTITIESP_H
#define LARRAWCONDITIONS_LARWAVEDERIVEDQUANTITIESP_H

// persistent c-struct
class LArWaveDerivedQuantitiesP
{

 public:

  LArWaveDerivedQuantitiesP() : 
    m_baseline(-9999999),
    m_maxAmp(-9999999), 
    m_tmaxAmp(-9999999), 
    m_width(-9999999), 
    m_rT0(-9999999), 
    m_posLobe(-9999999),
    m_jitter(-9999999),
    m_flag(0)
    {};
    
  LArWaveDerivedQuantitiesP(float b, float m, float t, float w, float r, float p, float j, unsigned f) :
    m_baseline(b),
    m_maxAmp(m), 
    m_tmaxAmp(t), 
    m_width(w), 
    m_rT0(r), 
    m_posLobe(p),
    m_jitter(j),
    m_flag(f)
    {} ;

  bool isEmpty() const 
    { return ( m_baseline<-9999 &&  m_maxAmp<-9999 &&  m_tmaxAmp<-9999 && 
	       m_width<-9999 && m_rT0<-9999 && m_posLobe<-9999 &&
	       m_jitter<-9999); }
  
  float m_baseline;
  float m_maxAmp;
  float m_tmaxAmp;
  float m_width;
  float m_rT0;
  float m_posLobe;
  float m_jitter;

  unsigned m_flag; // Wave flag

};
     
#endif
