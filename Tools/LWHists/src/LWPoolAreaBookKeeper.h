/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWPoolAreaBookKeeper                //
//                                                            //
//  Description: Helper class for keeping track of which      //
//               chunks in a pool area have been dished out.  //
//                                                            //
//               The internal book-keeping overhead is 1 bit  //
//               per entry (plus a contribution which is      //
//               negligible for high nEntries)                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWPOOLAREABOOKKEEPER_H
#define LWPOOLAREABOOKKEEPER_H

#include <stdint.h>
#include <cassert>
#include <climits>//for CHAR_BIT and UINT_MAX
#include <cstring>//for memset(..) and ffs(..)

class LWPoolAreaBookKeeper {
public:

  //Methods used for creating objects, this tricky procedure is to
  //ensure cache-locality and remove internal malloc's (to destroy,
  //simply release the memory - no need for the destructor)
  static unsigned nBytesNeededForCreate(unsigned nEntries);
  static LWPoolAreaBookKeeper * create(char *, unsigned nEntries);
  unsigned nBytesCovered() const;

  unsigned acquireEntry();//Returns UINT_MAX when none are available.
  void returnEntry(unsigned);
  bool isCompletelyFull() const;
  bool isCompletelyEmpty() const;
  unsigned numberOfAvailableEntries() const;
  unsigned numberOfEntriesHandedOut() const;

private:
  typedef unsigned BitField;
#ifndef NDEBUG
  friend class LWPoolArea;
#endif
  const unsigned m_nEntries;
  const unsigned m_nFields;
  unsigned m_nonEmptyField;//set to UINT_MAX when all are empty
  unsigned m_nUnusedEntries;

private:
  LWPoolAreaBookKeeper(unsigned nEntries);
  ~LWPoolAreaBookKeeper(){}
  LWPoolAreaBookKeeper( const LWPoolAreaBookKeeper & );
  LWPoolAreaBookKeeper & operator= ( const LWPoolAreaBookKeeper & );
  static unsigned nFields(unsigned nEntries);
  void findNewNonEmptyField();
#define LW_ENTRIESPERGROUP (CHAR_BIT*sizeof(BitField))
};

#include "LWPoolAreaBookKeeper.icc"

#endif
