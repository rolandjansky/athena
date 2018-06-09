/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RecRoi.cxx  -  description
                             -------------------
    begin                : Fri Oct 4 2002
    email                : moyse@zanzibar
 ***************************************************************************/


#include "TrigT1Interfaces/RecRoI.h"

LVL1::RecRoI::RecRoI():m_roiWord(0){
}
LVL1::RecRoI::~RecRoI(){
}

unsigned int LVL1::RecRoI::roiWord() const {
  return m_roiWord;
}
