/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TChainStateTracker.cxx 619763 2014-10-03 08:36:08Z krasznaa $

// Local include(s):
#include "xAODRootAccess/tools/TChainStateTracker.h"

namespace xAOD {

   TChainStateTracker::TChainStateTracker()
      : ::TObject(),
        m_stateChanged( kFALSE ) {

   }

   ::Bool_t TChainStateTracker::Notify() {

      // Update our tracking variable:
      m_stateChanged = kTRUE;
      // Return gracefully:
      return kTRUE;
   }

   ::Bool_t TChainStateTracker::internalStateChanged() const {

      return m_stateChanged;
   }

   void TChainStateTracker::reset() {

      m_stateChanged = kFALSE;
      return;
   }

} // namespace xAOD
