// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfAuxContainer.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_BUNCHCONFAUXCONTAINER_H
#define XAODTRIGGER_BUNCHCONFAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/BunchConfAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the bunch configuration auxiliary
   /// container class
   typedef BunchConfAuxContainer_v1 BunchConfAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BunchConfAuxContainer, 1159930079, 1 )

#endif // XAODTRIGGER_BUNCHCONFAUXCONTAINER_H
