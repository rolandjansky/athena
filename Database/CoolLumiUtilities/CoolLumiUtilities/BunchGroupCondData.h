// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/BunchGroupCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds data on filled bunches.
 */


#ifndef COOLLUMIUTILITIES_BUNCHGROUPCONDDATA_H
#define COOLLUMIUTILITIES_BUNCHGROUPCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <stdint.h>


/**
 * @brief Holds data on filled bunches.
 *
 * For each bunch group, hold a vector of filled BCIDs.
 */
class BunchGroupCondData
{
public:
  /// Number of bunch groups.
  constexpr static unsigned int NBUNCHGROUPS = 8;

  /// Maximum size of BCID vectors.
  constexpr static unsigned int NBCID = 3564;


  /** 
   * @brief Constructor.
   */
  BunchGroupCondData();


  /** 
   * @brief Return filled BCIDs for one bunch group.
   * @param group Bunch group to retrieve.
   *
   * Returns a vector of filled BCIDs for a group.
   */
  const std::vector<unsigned int>& bunchGroup (unsigned int group) const;


  /**
   * @brief Add informatin for one BCID.
   * @param bcid BCID to add.
   * @param mask Mask of groups in which this BCID is filled.
   */
  void addBCID (unsigned int bcid, uint8_t mask);


  /** 
   * @brief Shrink all vectors to size.
   */
  void shrink();


private:
  /// BCIDs of filled bunches, per group.
  std::vector<unsigned int> m_bunchGroups[NBUNCHGROUPS];
};


CLASS_DEF (BunchGroupCondData, 212775257, 0)
CONDCONT_DEF (BunchGroupCondData, 114859191);


#endif // not COOLLUMIUTILITIES_BUNCHGROUPCONDDATA_H
