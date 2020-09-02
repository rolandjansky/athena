// this file is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFO.h 744541 2016-05-03 15:55:30Z krasznaa $
#ifndef XAODPFLOW_FLOWELEMENT_H
#define XAODPFLOW_FLOWELEMENT_H

// Local include(s):
#include "xAODPFlow/versions/FlowElement_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Definition of the current "pfo version"
  typedef FlowElement_v1 FlowElement;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FlowElement , 25811773 , 1 )

#endif // XAODPFLOW_FlowElement_H
