/***************************************************************************
                          Parity.cpp  -  description
                             -------------------
    begin                : Fri Jun 28 2002
    copyright            : (C) 2002 by Edward Moyse
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
