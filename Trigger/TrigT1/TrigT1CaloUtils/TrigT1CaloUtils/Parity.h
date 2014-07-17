/***************************************************************************
                          Parity.h  -  description
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
