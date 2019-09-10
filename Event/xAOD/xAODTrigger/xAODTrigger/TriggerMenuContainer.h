// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuContainer.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_TRIGGERMENUCONTAINER_H
#define XAODTRIGGER_TRIGGERMENUCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"
#include "xAODTrigger/TriggerMenu.h"

namespace xAOD {
   /// Define the latest version of the trigger menu container class
   typedef TriggerMenuContainer_v1 TriggerMenuContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuContainer, 1107011239, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::TriggerMenuContainer> , 11070039 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::TriggerMenuContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_TRIGGERMENUCONTAINER_H
