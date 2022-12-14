// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_XAODCUTFLOWHELPERS_H
#define XAODCUTFLOW_XAODCUTFLOWHELPERS_H

// STL includes
#include <string>

// Local include(s):
#include "xAODCutFlow/CutBookkeeperContainer.h"

// forward declarations
class TH1;


/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Namespace for all helpers
  namespace CutFlowHelpers {
    /// Make a histogram of the number of accepted events from a container.
    ::TH1* nAcceptedEvents( const xAOD::CutBookkeeperContainer* cbkCont,
                            const std::string& histName,
                            int minCycle=0, int maxCycle=-1 );

    /// Helper function to update a container with information from another one
    void updateContainer( xAOD::CutBookkeeperContainer *contToUpdate,
                          const xAOD::CutBookkeeperContainer *otherCont );
  } // End: namespace CutFlowHelpers

} // End: namespace xAOD

#endif // XAODCUTFLOW_XAODCUTFLOWHELPERS_H
