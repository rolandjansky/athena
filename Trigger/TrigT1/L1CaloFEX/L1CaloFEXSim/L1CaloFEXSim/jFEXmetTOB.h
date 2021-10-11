/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXmetTOB - TOBs MET Algorithm in jFEX
//                              -------------------
//     begin                : 14 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************


#ifndef JFEX_MET_TOB_H
#define JFEX_MET_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXmetTOB
  {
 
  private:
    unsigned int m_MET_X;
    unsigned int m_MET_Y;
    unsigned int m_Sat;
    unsigned int m_Res;


  public:
    jFEXmetTOB();
    ~jFEXmetTOB() {};

    void setMetX(unsigned int);
    void setMetY(unsigned int);
    void setSat(unsigned int);
    void setRes(unsigned int);
    unsigned int GetMetX();
    unsigned int GetMetY();
    unsigned int GetSat();
    unsigned int GetRes();


    

  };
  

} //end of namespace

//CLASS_DEF( LVL1::jFEXmetTOB , 207986940 , 1 )
#endif 
