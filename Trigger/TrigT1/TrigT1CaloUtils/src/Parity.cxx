/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Parity.cpp  -  description
                             -------------------
    begin                : Fri Jun 28 2002
    email                : moyse@zanzibar
 ***************************************************************************/


#include "TrigT1CaloUtils/Parity.h"

Parity::Parity(){
}
Parity::~Parity(){
}
/** returns even parity of passed word i.e. 0 returns 0 and 111 (7) returns 1 */
unsigned int Parity::even(unsigned int word) const {
  unsigned int numOfBits=0;
  for (unsigned int bit=0; bit<sizeof(word); bit++){
    if (word&(1<<bit)) numOfBits++;
  }
  return (numOfBits%2);
}
/** returns the odd parity of the passed word i.e. 0 returns 1 and 111 (7) returns 0 */
unsigned int Parity::odd(unsigned int word) const{
  return (1-even(word) );
}
