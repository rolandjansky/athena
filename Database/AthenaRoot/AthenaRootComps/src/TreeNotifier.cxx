///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TreeNotifier.cxx 
// Implementation file for class TreeNotifier
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "TreeNotifier.h"

// STL includes
#include <iostream>

// ROOT includes
#include "TTree.h"

namespace Athena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Default constructor: 
TreeNotifier::TreeNotifier() :
  TObject(),
  m_notify_chain(NULL)
{}


/// Constructor with parameters: 
TreeNotifier::TreeNotifier(TTree *ntuple) :
  TObject(),
  m_notify_chain( ntuple != NULL ? ntuple->GetNotify() : NULL)
{}

  /// Destructor: 
TreeNotifier::~TreeNotifier()
{
  //if (m_notify_chain)
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/**
 * @brief Standard Root notify routine.
 *
 * Called by ROOT when the chain switched trees or when a new file kicked in.
 * ie: called whenever the (branch) addresses need to be updated.
 *
 */
Bool_t 
TreeNotifier::Notify()
{
  std::cerr << "::TN:: --Notify--\n";
  // propagate the notification
  if (m_notify_chain) {
    return m_notify_chain->Notify();
  }
  return true;
}

} //> end namespace Athena
