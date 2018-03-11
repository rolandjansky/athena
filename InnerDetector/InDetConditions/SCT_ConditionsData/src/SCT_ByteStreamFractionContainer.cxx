/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for a data object class for SCT_ByteStreamErrorSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_ByteStreamFractionContainer.h"

//----------------------------------------------------------------------
// Constructor
SCT_ByteStreamFractionContainer::SCT_ByteStreamFractionContainer()
{
  clear();
}

//----------------------------------------------------------------------
// a new rodId with a bool value
void SCT_ByteStreamFractionContainer::insert(const Type type, const uint32_t rodId, const bool value)
{
  if (type>=FirstIndex and type<NumberOfTypes) {
    m_fraction[type].insert(rodId, value);
  }
}

//----------------------------------------------------------------------
// Clear m_trueSets and m_falseSets
void SCT_ByteStreamFractionContainer::clear()
{
  for (unsigned int type{static_cast<unsigned int>(FirstIndex)}; type<NumberOfTypes; type++) {
    m_fraction[type].clear();
  }
}

//----------------------------------------------------------------------
// Get majority
bool SCT_ByteStreamFractionContainer::majority(const Type type) const
{
  if (type>=FirstIndex and type<NumberOfTypes) {
    return m_fraction[type].majority();
  }
  return false;
}
