/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaBaseComps/src/AthAlgStartVisitor.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief 
 */


#include "AthenaBaseComps/AthAlgStartVisitor.h"
#include "StoreGate/VarHandleKey.h"
#include "GaudiKernel/IAlgorithm.h"

/**
 * @brief Figure out where we're called from. Only want to process holders
 *        recursively if caller is a real Algorithm. If it's an AlgTool, don't
 *        recurse. If it's a Sequence, don't do anything.
 * @param n back pointer to parent
 */
AthAlgStartVisitor::AthAlgStartVisitor(INamedInterface *n) {
  // only do recursive traversal if we are called from a real Algorithm
  IAlgorithm* ia = dynamic_cast<IAlgorithm*>(n);
  if ( ia == nullptr) {
    m_recursive = false;
  } else if ( ia->isSequence() ) {
    m_recursive = false;
    m_ignore = true;
  }
}


/**
 * @brief Visit a component and start() any read conditions handles.
 * @param holder The component to visit.
 */
void AthAlgStartVisitor::visit (const IDataHandleHolder* holder)
{
  if (m_ignore) return;

  // Make sure we process a component only once.
  if (m_seen.insert (holder).second) {
    // Call start() on all read conditions handle keys.
    for (Gaudi::DataHandle* h : holder->inputHandles()) {
      if (h->isCondition()) {
        if (SG::VarHandleKey* k = dynamic_cast<SG::VarHandleKey*> (h)) {
          k->start().ignore();
        }
      }
    }
    if (m_recursive) {
      holder->acceptDHVisitor (this);
    }
  }
}
