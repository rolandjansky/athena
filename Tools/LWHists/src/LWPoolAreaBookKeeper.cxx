/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWPoolAreaBookKeeper              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

//Fixme: Cache locality... use area right after class for m_fields?

#include "LWPoolAreaBookKeeper.h"

#include <cassert>
#include <new>//for placement new...

#define LW_FIELDS (reinterpret_cast<BitField*>(reinterpret_cast<char*>(this)+sizeof(LWPoolAreaBookKeeper)))

//____________________________________________________________________
unsigned LWPoolAreaBookKeeper::nBytesNeededForCreate(unsigned nEntries)
{
  return (nFields(nEntries)*sizeof(BitField)+sizeof(LWPoolAreaBookKeeper) + 7) & ~7;
}

//____________________________________________________________________
unsigned LWPoolAreaBookKeeper::nFields(unsigned nEntries)
{
  return nEntries/LW_ENTRIESPERGROUP+(nEntries%LW_ENTRIESPERGROUP?1:0);
}

//____________________________________________________________________
unsigned LWPoolAreaBookKeeper::nBytesCovered() const
{
  return nBytesNeededForCreate(m_nEntries);
}

//____________________________________________________________________
LWPoolAreaBookKeeper * LWPoolAreaBookKeeper::create(char *c, unsigned nEntries)
{
  return new(c) LWPoolAreaBookKeeper(nEntries);
}

//____________________________________________________________________
LWPoolAreaBookKeeper::LWPoolAreaBookKeeper(unsigned nEntries)
  : //m_fields(0),
    m_nEntries(nEntries),
    m_nFields(nFields(nEntries)),
    m_nonEmptyField(0),
    m_nUnusedEntries(nEntries)
{
  assert(CHAR_BIT==8);//Likely to have bugs if CHAR_BIT!=8
  memset(LW_FIELDS,0xFF,m_nFields*sizeof(LW_FIELDS[0]));//set all bits (0xFF is 8 bits)
  const unsigned nlast(nEntries%LW_ENTRIESPERGROUP);
  if (nlast)
    LW_FIELDS[m_nFields-1] >>= (LW_ENTRIESPERGROUP-nlast);//Only set the 'nlast' least significant bits
}

//____________________________________________________________________
void LWPoolAreaBookKeeper::findNewNonEmptyField()
{
  assert(!LW_FIELDS[m_nonEmptyField]);
  //For cache-locality reasons we first look in the following
  //fields, then start from 0:
  unsigned newNonEmptyField = UINT_MAX;
  for (unsigned iField = m_nonEmptyField+1; iField<m_nFields;++iField) {
    if (LW_FIELDS[iField]) {
      newNonEmptyField = iField;
      break;
    }
  }
  if (newNonEmptyField==UINT_MAX)
    for (unsigned iField = 0; iField<m_nonEmptyField;++iField) {
      if (LW_FIELDS[iField]) {
	newNonEmptyField = iField;
	break;
      }
    }
  m_nonEmptyField = newNonEmptyField;
  assert(m_nonEmptyField==UINT_MAX||LW_FIELDS[m_nonEmptyField]);
}







