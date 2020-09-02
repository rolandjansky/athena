// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_TRIGGERMENUJSONCONTAINER_H
#define XAODTRIGGER_TRIGGERMENUJSONCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuJsonContainer_v1.h"
#include "xAODTrigger/TriggerMenuJson.h"

namespace xAOD {
   typedef TriggerMenuJsonContainer_v1 TriggerMenuJsonContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuJsonContainer, 1221262614, 1 )

#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::TriggerMenuJsonContainer> , 1760135678 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::TriggerMenuJsonContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_TRIGGERMENUJSONCONTAINER_H
