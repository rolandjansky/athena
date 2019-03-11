// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigComposite.h 631318 2014-11-26 18:32:04Z krasznaa $
#ifndef XAODTRIGGER_TRIGCOMPOSITE_H
#define XAODTRIGGER_TRIGCOMPOSITE_H

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD{
   /// Declare the latest version of the class
   typedef TrigComposite_v1 TrigComposite;
}

namespace TrigCompositeUtils{ 
   typedef xAOD::TrigComposite Decision;
}

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigComposite, 178723140, 0 )

#endif // XAODTRIGGER_TRIGCOMPOSITE_H
