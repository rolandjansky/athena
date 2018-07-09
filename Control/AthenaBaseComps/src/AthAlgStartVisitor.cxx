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


#include "AthAlgStartVisitor.h"
#include "StoreGate/VarHandleKey.h"


/**
 * @brief Visit a component and start() any read conditions handles.
 * @param holder The component to visit.
 */
void AthAlgStartVisitor::visit (const IDataHandleHolder* holder)
{
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
    holder->acceptDHVisitor (this);
  }
}
