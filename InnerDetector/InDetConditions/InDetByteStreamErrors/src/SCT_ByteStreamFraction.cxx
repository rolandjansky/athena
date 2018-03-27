/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for a data object class for SCT_ByteStreamErrorSvc
//----------------------------------------------------------------------

#include "InDetByteStreamErrors/SCT_ByteStreamFraction.h"

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
  if (value) m_trueSet.insert(rodId);
  else m_falseSet.insert(rodId);
}

//----------------------------------------------------------------------
// Clear m_trueSet and m_falseSet
void SCT_ByteStreamFraction::clear()
{
  m_trueSet.clear();
  m_falseSet.clear();
}

//----------------------------------------------------------------------
// Get majority
bool SCT_ByteStreamFraction::majority() const
{
  return (m_trueSet.size() >= m_falseSet.size());
}

//----------------------------------------------------------------------
// Get entries
unsigned int SCT_ByteStreamFraction::entries() const
{
  return (m_trueSet.size() + m_falseSet.size());
}

//----------------------------------------------------------------------
// Get true set
std::set<uint32_t> SCT_ByteStreamFraction::getTrueSet() const
{
  return m_trueSet;
}

//----------------------------------------------------------------------
// Get false set
std::set<uint32_t> SCT_ByteStreamFraction::getFalseSet() const
{
  return m_falseSet;
}

//----------------------------------------------------------------------
// Set true set
void SCT_ByteStreamFraction::setTrueSet(const std::set<uint32_t>& trueSet)
{
  m_trueSet = trueSet;
}

//----------------------------------------------------------------------
// Set false set
void SCT_ByteStreamFraction::setFalseSet(const std::set<uint32_t>& falseSet)
{
  m_falseSet = falseSet;
}
