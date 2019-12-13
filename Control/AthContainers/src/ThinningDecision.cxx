/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/src/ThinningDecision.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Hold thinning decisions for one container.
 */


#include "AthContainers/ThinningDecision.h"


namespace SG {


/**
 * @brief Constructor.
 * @param link Link to the container being thinned.
 *
 * Initialized with all elements kept.
 */
ThinningDecision::ThinningDecision (const DataLink<SG::AuxVectorBase>& link)
  : m_link (link)
{
  if (!m_link.isValid()) {
    throw SG::ExcInvalidThinningTarget (link.classID(), link.dataID());
  }
  resize (m_link->size_v());
}


/**
 * @brief Constructor.
 * @param key SG key of the container being thinned.
 *
 * Initialized with all elements kept.
 */
ThinningDecision::ThinningDecision (const std::string& key)
  : ThinningDecision (DataLink<SG::AuxVectorBase> (key))
{
}


/**
 * @brief Return link to object being thinned.
 */
const DataLink<SG::AuxVectorBase>& ThinningDecision::link() const
{
  return m_link;
}


/**
 * @brief Called when the object is locked in SG.
 *
 *  Build the index map.
 */
void ThinningDecision::lock()
{
  buildIndexMap();
}


} // namespace SG
