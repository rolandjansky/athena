// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/ThinningDecision.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Hold thinning decisions for one container.
 */


#ifndef ATHCONTAINERS_THINNINGDECISION_H
#define ATHCONTAINERS_THINNINGDECISION_H
#ifndef XAOD_STANDALONE


#include "AthContainers/AuxVectorBase.h"
#include "AthLinks/DataLink.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/ILockable.h"
#include <vector>
#include <string>


namespace SG {


/**
 * @brief Hold thinning decisions for one container.
 *
 * This essentially holds a bitmap of the elements to be thinned from the container,
 * as well as the mapping of indices before and after thinning (the latter available
 * only after the container has been locked in SG).  See the base class
 * @c ThinningDecisionBase for further details.
 */
class ThinningDecision
  : public ThinningDecisionBase, public ILockable
{
public:
  /**
   * @brief Constructor.
   * @param link Link to the container being thinned.
   *
   * Initialized with all elements kept.
   */
  ThinningDecision (const DataLink<SG::AuxVectorBase>& link);


  /**
   * @brief Constructor.
   * @param key SG key of the container being thinned.
   *
   * Initialized with all elements kept.
   */
  ThinningDecision (const std::string& key);


  /**
   * @brief Return link to object being thinned.
   */
  const DataLink<SG::AuxVectorBase>& link() const;


  /**
   * @brief Called when the object is locked in SG.
   *
   *  Build the index map.
   */
  virtual void lock() override;


private:
  /// Link to the object being thinned.
  DataLink<SG::AuxVectorBase> m_link;
};


} // namespace SG


CLASS_DEF( SG::ThinningDecision, 74079355, 1 )


#endif // XAOD_STANDALONE
#endif // not ATHCONTAINERS_THINNINGDECISION_H
