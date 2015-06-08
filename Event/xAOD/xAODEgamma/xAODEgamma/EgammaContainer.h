// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaContainer.h 672987 2015-06-06 16:15:13Z christos $
#ifndef XAODEGAMMA_EGAMMACONTAINER_H
#define XAODEGAMMA_EGAMMACONTAINER_H

// Local include(s):
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/versions/EgammaContainer_v1.h"

namespace xAOD {
   /// Definition of the current "egamma container version"
   typedef EgammaContainer_v1 EgammaContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EgammaContainer , 1087548811 , 1 )

#endif // XAODEGAMMA_EGAMMACONTAINER_H
