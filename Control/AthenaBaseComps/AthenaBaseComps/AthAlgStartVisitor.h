// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
// $Id$
/**
 * @file AthenaBaseComps/src/AthAlgStartVisitor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief Call start() on all ReadCondHandle's of an algorithm.
 */


#ifndef ATHENABASECOMPS_ATHALGSTARTVISITOR_H
#define ATHENABASECOMPS_ATHALGSTARTVISITOR_H


#include "GaudiKernel/IDataHandleHolder.h"
#include <unordered_set>


/**
 * @brief Call start() on all ReadCondHandle's of an algorithm.
 *
 * For efficient conditions handling, a CondHandleKey needs to cache a pointer
 * to the conditions container.  initialize() is too early for this as conditions
 * containers made by CondInputLoader are not created until start().  So during
 * the start() of an algorithm, we visit all read conditions handle keys
 * and call start() on them, to give them the opportunity to look up the
 * conditions object.
 *
 * We use @c IDataHandleVisitor to do this recursively for all owned tools as well.
 *
 * FIXME: this should probably be moved to GaudiKernel, and have all AlgTools
 *        started by their parent Algorithms
 */
class AthAlgStartVisitor
  : public IDataHandleVisitor
{
public:
  /**
   * @brief Visit a component and start() any read conditions handles.
   * @param holder The component to visit.
   */
  virtual void visit (const IDataHandleHolder* holder) override;

  AthAlgStartVisitor() = default;
  AthAlgStartVisitor(INamedInterface*);  /// figure out where we're called from

private:
  /// Keep track of components we've already processed.
  std::unordered_set<const IDataHandleHolder*> m_seen;
  bool m_recursive {true};            /// process recursively (for Algs)
  bool m_ignore {false};              /// don't do anything  (if Sequence)
};


#endif // not ATHENABASECOMPS_ATHALGSTARTVISITOR_H
