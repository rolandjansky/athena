/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDSPTHRESHOLDSP_H
#define LARDSPTHRESHOLDSP_H


class LArDSPThresholdsP {
  
 public:
  LArDSPThresholdsP() : m_tQThr(m_default), m_samplesThr(m_default), m_trigSumThr(m_default) {};
  
  LArDSPThresholdsP(const float tQThr, const float samplesThr, const float trigSumThr) : 
    m_tQThr(tQThr), m_samplesThr(samplesThr), m_trigSumThr(trigSumThr) {};

  void set (const float tQThr, const float samplesThr, const float trigSumThr) { 
    m_tQThr=tQThr; 
    m_samplesThr=samplesThr; 
    m_trigSumThr=trigSumThr;
  }

  float tQThr() const { return m_tQThr; }; 
  float samplesThr() const {return m_samplesThr; };
  float trigSumThr() const {return m_trigSumThr; }; 

  static float getDefault() {return m_default; };

 private:
  float m_tQThr;
  float m_samplesThr;
  float m_trigSumThr;
  
  const static float m_default;

};

#endif
