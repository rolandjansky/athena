/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJwoJTOB - Forms the JwoJ TOBs for gFEX
//                              -------------------
//     begin                : 10 08 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#pragma once
#include "AthenaKernel/CLASS_DEF.h"

namespace LVL1 {
  class gFEXJwoJTOB
  {
    //gFEXJwoJTOB class description below:
    /** The gFEXJwoJTOB.h class stores the energy, the eta, phi coordinate,
     * the status and the type (gRho, gBlock, gJwoJ) of the gFEX JwoJ TOBs
     */
  private:
    uint32_t     m_word;
    unsigned int m_quantity1;
    unsigned int m_quantity2;
    unsigned int m_st1;
    unsigned int m_st2;
    unsigned int m_satur;
    unsigned int m_tobID;


  public:
    gFEXJwoJTOB();
    ~gFEXJwoJTOB() {};

    inline uint32_t      getWord() const {return m_word;}
    inline unsigned  int getQuantity1() const {return m_quantity1;}
    inline unsigned  int getQuantity2() const {return m_quantity2;}
    inline unsigned  int getStatus1() const {return m_st1;}
    inline unsigned  int getStatus2() const {return m_st2;}
    inline unsigned  int getSaturation() const {return m_satur;}
    inline unsigned  int getTobID() const {return m_tobID;}


    void setWord(uint32_t);
    void setQuantity1(unsigned int);
    void setQuantity2(unsigned int);
    void setStatus1(unsigned int);
    void setStatus2(unsigned int);
    void setSaturation(unsigned int);
    void setTobID(unsigned int);
  };

} // end of namespace

CLASS_DEF( LVL1::gFEXJwoJTOB, 2352924 , 1 )
