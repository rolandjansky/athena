/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXForwardElecTOB - TOBs ForwardElec Algorithm in jFEX
//                              -------------------
//     begin                : 28 11 2021
//     email                : ulla.blumenschein@cern.ch
//***************************************************************************

#ifndef JFEX_FELEC_TOB_H
#define JFEX_FELEC_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXForwardElecTOB
  {
 
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_ET;
    unsigned int m_Iso;
    unsigned int m_EMf1;
    unsigned int m_EMf2;
    unsigned int m_Sat;
    unsigned int m_Res;

  public:
    jFEXForwardElecTOB();
    ~jFEXForwardElecTOB() {};

    void setET(unsigned int);
    void setPhi(unsigned int);
    void setEta(unsigned int);
    void setIso(unsigned int);
    void setEMf1(unsigned int);
    void setEMf2(unsigned int);
    void setSat(unsigned int);
    void setRes(unsigned int);
    unsigned int GetEta();
    unsigned int GetPhi();
    unsigned int GetET();
    unsigned int GetIso();
    unsigned int GetEMf1();
    unsigned int GetEMf2();
    unsigned int GetSat();
    unsigned int GetRes();

    // Data locations within word
    // Sat: 0
    // Phi 1-4
    // Eta 5-9
    // ET: 10-20
    // Iso: 21-22
    // FracA: 23-24
    // FracB: 25-26
    // Res: 27-31 
  };
  

} //end of namespace

CLASS_DEF( LVL1::jFEXForwardElecTOB , 245965039 , 1 )
#endif 
