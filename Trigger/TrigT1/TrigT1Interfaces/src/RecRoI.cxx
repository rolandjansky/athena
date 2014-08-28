/***************************************************************************
                          RecRoi.cxx  -  description
                             -------------------
    begin                : Fri Oct 4 2002
    copyright            : (C) 2002 by moyse
    email                : moyse@zanzibar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "TrigT1Interfaces/RecRoI.h"

LVL1::RecRoI::RecRoI():m_roiWord(0){
}
LVL1::RecRoI::~RecRoI(){
}

unsigned int LVL1::RecRoI::roiWord() const {
  return m_roiWord;
}
