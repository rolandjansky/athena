/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for a data object class for SCT_ByteStreamErrorSvc
//----------------------------------------------------------------------

#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"

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
  // Check invalid type
  if (not (type>=FirstIndex and type<NumberOfTypes)) return false;

  // Default value without entries is different
  if (type==SimulatedData and m_fraction[type].entries()==0) return false;
  if (type==CondensedMode and m_fraction[type].entries()==0) return true;
  if (type==HVOn and m_fraction[type].entries()==0) return false;

  return m_fraction[type].majority();
}

//----------------------------------------------------------------------
// Set true set
void SCT_ByteStreamFractionContainer::setTrueSet(const Type type, const std::set<uint32_t> trueSet)
{
  if (type>=FirstIndex and type<NumberOfTypes) m_fraction[type].setTrueSet(trueSet);
}

//----------------------------------------------------------------------
// Set false set
void SCT_ByteStreamFractionContainer::setFalseSet(const Type type, const std::set<uint32_t> falseSet)
{
  if (type>=FirstIndex and type<NumberOfTypes) m_fraction[type].setFalseSet(falseSet);
}

//----------------------------------------------------------------------
// Get fraction
SCT_ByteStreamFraction SCT_ByteStreamFractionContainer::getFraction(const Type type) const
{
  if (type>=FirstIndex and type<NumberOfTypes) return m_fraction[type];
  return SCT_ByteStreamFraction{};
}
