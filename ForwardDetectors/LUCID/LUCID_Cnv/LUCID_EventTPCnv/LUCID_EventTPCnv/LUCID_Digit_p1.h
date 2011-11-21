/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGIT_P1_H
#define LUCID_DIGIT_P1_H

class LUCID_Digit_p1 {
 
 public:
  
  LUCID_Digit_p1() {

    m_tubeID = 0;
    m_npe    = 0;
    m_npeGas = 0;
    m_npePmt = 0;
    m_qdc    = 0;
    m_tdc    = 0;
  };
  
  friend class LUCID_DigitCnv_p1;
  
 private:
  
  unsigned short m_tubeID;
  unsigned short m_npe;
  unsigned short m_npeGas;
  unsigned short m_npePmt;
  unsigned short m_qdc;
  unsigned short m_tdc;
};

#endif
