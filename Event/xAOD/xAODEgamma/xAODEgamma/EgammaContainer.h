// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaContainer.h 632191 2014-11-29 16:24:08Z christos $
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
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EgammaContainer , 1087548811 , 1 )
#endif // XAOD_STANDALONE

#endif // XAODEGAMMA_EGAMMACONTAINER_H
