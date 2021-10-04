/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJwoJTOB - Forms the Jets without Jets TOBs for gFEX
//                              -------------------
//     begin                : 11 08 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gFEXJwoJTOB.h"


LVL1::gFEXJwoJTOB::gFEXJwoJTOB():
  m_word{99999},
  m_quantity1{99999},
  m_quantity2{99999},
  m_st1{99999},
  m_st2{99999},
  m_satur{99999},
  m_tobID{99999}
{}

 void setWord(uint32_t);
    void setQuantity1(unsigned int);
    void setQuantity2(unsigned int);
    void setStatus1(unsigned int);
    void setStatus2(unsigned int);
    void setSaturation(unsigned int);
    void setTobID(unsigned int);


void LVL1::gFEXJwoJTOB::setWord(uint32_t word) {
  m_word = word;
}

void LVL1::gFEXJwoJTOB::setQuantity1(unsigned int quantity1) {
  m_quantity1 = quantity1;
}

void LVL1::gFEXJwoJTOB::setQuantity2(unsigned int quantity2) {
  m_quantity2 = quantity2;
}

void LVL1::gFEXJwoJTOB::setStatus1(unsigned int st1) {
  m_st1 = st1;
}

void LVL1::gFEXJwoJTOB::setStatus2(unsigned int st2) {
  m_st2 = st2;
}

void LVL1::gFEXJwoJTOB::setSaturation(unsigned int satur) {
  m_satur = satur;
}

void LVL1::gFEXJwoJTOB::setTobID(unsigned int tobID) {
  m_tobID = tobID;
}


