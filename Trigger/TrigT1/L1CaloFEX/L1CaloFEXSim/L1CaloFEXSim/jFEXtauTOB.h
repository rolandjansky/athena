/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXtauTOB - TOBs Tau Algorithm in jFEX
//                              -------------------
//     begin                : 18 02 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************


#ifndef JFEX_TAU_TOB_H
#define JFEX_TAU_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXtauTOB
  {
 
  private:
    unsigned int m_eta;
    unsigned int m_phi;
    unsigned int m_ET;
    unsigned int m_Iso;
    unsigned int m_Sat;


  public:
    jFEXtauTOB();
    ~jFEXtauTOB() {};

    void setET(unsigned int);
    void setPhi(unsigned int);
    void setEta(unsigned int);
    void setIso(unsigned int);
    void setSat(unsigned int);
    unsigned int GetEta();
    unsigned int GetPhi();
    unsigned int GetET();
    unsigned int GetIso();
    unsigned int GetSat();


    

  };
  

} //end of namespace

CLASS_DEF( LVL1::jFEXtauTOB , 207986940 , 1 )
#endif 
