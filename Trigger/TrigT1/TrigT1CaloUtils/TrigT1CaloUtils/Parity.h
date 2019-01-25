/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Parity.h  -  description
                             -------------------
    begin                : Fri Jun 28 2002
    email                : moyse@zanzibar
 ***************************************************************************/


#ifndef PARITY_H
#define PARITY_H


/**calculates the parity of a passed word
  *@author Edward Moyse
  */

class Parity {
public: 
	Parity();
	~Parity();
  /** returns even parity of passed word i.e. 0 returns 0 and 111 (7) returns 1 */
  unsigned int even(unsigned int word) const;
  /** returns the odd parity of the passed word i.e. 0 returns 1 and 111 (7) returns 0 */
  unsigned int odd(unsigned int word) const;
};

#endif
