// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFO.h 744541 2016-05-03 15:55:30Z krasznaa $
#ifndef XAODPFLOW_PFO_H
#define XAODPFLOW_PFO_H

// Local include(s):
#include "xAODPFlow/versions/PFO_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Definition of the current "pfo version"
  typedef PFO_v1 PFO;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PFO, 104356027, 1 )

#endif // XAODPFLOW_PFO_H
