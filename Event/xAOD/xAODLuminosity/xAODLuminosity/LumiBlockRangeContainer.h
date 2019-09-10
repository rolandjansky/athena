// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRangeContainer.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_LUMIBLOCKRANGECONTAINER_H
#define XAODLUMINOSITY_LUMIBLOCKRANGECONTAINER_H

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRangeContainer_v1.h"
#include "xAODLuminosity/LumiBlockRange.h"

namespace xAOD {
   /// Declare the latest version of the container
   typedef LumiBlockRangeContainer_v1 LumiBlockRangeContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::LumiBlockRangeContainer, 1115934851, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::LumiBlockRangeContainer> , 126923741 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::LumiBlockRangeContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODLUMINOSITY_LUMIBLOCKRANGECONTAINER_H
