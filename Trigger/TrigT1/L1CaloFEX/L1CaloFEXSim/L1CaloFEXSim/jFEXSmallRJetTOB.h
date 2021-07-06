/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXSmallRJetTOB - TOBs small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************


#ifndef JFEX_SMALLRJET_TOB_H
#define JFEX_SMALLRJET_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXSmallRJetTOB
  {
 
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_ET;
    unsigned int m_Res;
    unsigned int m_Sat;


  public:
    jFEXSmallRJetTOB();
    ~jFEXSmallRJetTOB() {};
    
    unsigned int setEta(unsigned int);
    unsigned int setPhi(unsigned int);
    unsigned int setET(unsigned int);
    unsigned int setRes(unsigned int);
    unsigned int setSat(unsigned int);

  };
} //end of namespace

CLASS_DEF( LVL1::jFEXSmallRJetTOB , 262110794 , 1 )
#endif 
