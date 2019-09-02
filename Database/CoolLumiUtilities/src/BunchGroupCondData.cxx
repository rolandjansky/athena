/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchGroupCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds data on filled bunches.
 */


#include "CoolLumiUtilities/BunchGroupCondData.h"
#include <stdexcept>


/** 
 * @brief Constructor.
 */
BunchGroupCondData::BunchGroupCondData()
{
  // Reserve all vectors to their full size.
  for (unsigned int i = 0; i < NBUNCHGROUPS; i++) {
    m_bunchGroups[i].reserve (NBCID);
  }
}


/** 
 * @brief Return filled BCIDs for one bunch group.
 * @param group Bunch group to retrieve.
 *
 * Returns a vector of filled BCIDs for a group.
 */
const std::vector<unsigned int>&
BunchGroupCondData::bunchGroup (unsigned int group) const
{
  if (group >= NBUNCHGROUPS) {
    throw std::out_of_range ("BunchGroupCondData::bunchGroup");
  }
  return m_bunchGroups[group];
}


/**
 * @brief Add informatin for one BCID.
 * @param bcid BCID to add.
 * @param mask Mask of groups in which this BCID is filled.
 */
void BunchGroupCondData::addBCID (unsigned int bcid, uint8_t mask)
{
  for (size_t i = 0; i < NBUNCHGROUPS; i++) {
    if ((mask & (1<<i)) != 0) {
      m_bunchGroups[i].push_back (bcid);
    }
  }
}


/** 
 * @brief Shrink all vectors to size.
 */
void BunchGroupCondData::shrink()
{
  for (unsigned int i = 0; i < NBUNCHGROUPS; i++) {
    m_bunchGroups[i].shrink_to_fit();
  }
}
