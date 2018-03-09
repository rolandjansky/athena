/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for a data object class for SCT_ByteStreamErrorSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_ByteStreamFraction.h"

//----------------------------------------------------------------------
// Constructor
SCT_ByteStreamFraction::SCT_ByteStreamFraction()
{
  clear();
}

//----------------------------------------------------------------------
// a new rodId with a bool value
void SCT_ByteStreamFraction::insert(const uint32_t rodId, const bool value)
{
  if (value) m_trueSets.insert(rodId);
  else m_falseSets.insert(rodId);
}

//----------------------------------------------------------------------
// Clear m_trueSets and m_falseSets
void SCT_ByteStreamFraction::clear()
{
  m_trueSets.clear();
  m_falseSets.clear();
}

//----------------------------------------------------------------------
// Get majority
bool SCT_ByteStreamFraction::majority() const
{
  return (m_trueSets.size() >= m_falseSets.size());
}
