/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXsumETTOB - TOBs Sum ET Algorithm in jFEX
//                              -------------------
//     begin                : 12 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************


#ifndef JFEX_SUMET_TOB_H
#define JFEX_SUMET_TOB_H
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class jFEXsumETTOB
  {
 
  private:
    unsigned int m_ETlow;
    unsigned int m_EThigh;
    unsigned int m_satlow;
    unsigned int m_sathigh;


  public:
    jFEXsumETTOB();
    ~jFEXsumETTOB() {};

    void setETlow(unsigned int);
    void setEThigh(unsigned int);
    void setSatlow(unsigned int);
    void setSathigh(unsigned int);
    unsigned int GetETlow();
    unsigned int GetEThigh();
    unsigned int GetSatlow();
    unsigned int GetSatHigh();


    

  };
  

} //end of namespace

//CLASS_DEF( LVL1::jFEXsumETTOB , 207986940 , 1 )
#endif 
