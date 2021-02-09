/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXLargeRJetTOB - TOBs large R jet Algorithm in jFEX
//                              -------------------
//     begin                : 21 01 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************


#ifndef JFEX_LARGERJET_TOB_H
#define JFEX_LARGERJET_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXLargeRJetTOB
  {
 
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_ET;
    unsigned int m_Sub;
    unsigned int m_Sat;


  public:
    jFEXLargeRJetTOB();
    ~jFEXLargeRJetTOB() {};
    
    unsigned int setEta(unsigned int);
    unsigned int setPhi(unsigned int);
    unsigned int setET(unsigned int);
    unsigned int setSub(unsigned int);
    unsigned int setSat(unsigned int);

  };
} //end of namespace
CLASS_DEF( LVL1::jFEXLargeRJetTOB , 264025730 , 1 )
#endif 

