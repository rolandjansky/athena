// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRangeAuxContainer.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_LUMIBLOCKRANGEAUXCONTAINER_H
#define XAODLUMINOSITY_LUMIBLOCKRANGEAUXCONTAINER_H

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRangeAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the auxiliary container
   typedef LumiBlockRangeAuxContainer_v1 LumiBlockRangeAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::LumiBlockRangeAuxContainer, 1251061086, 1 )

#endif // XAODLUMINOSITY_LUMIBLOCKRANGEAUXCONTAINER_H
