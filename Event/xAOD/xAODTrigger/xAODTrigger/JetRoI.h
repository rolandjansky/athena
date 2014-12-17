// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoI.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_JETROI_H
#define XAODTRIGGER_JETROI_H

// Local include(s):
#include "xAODTrigger/versions/JetRoI_v1.h"
#include "xAODTrigger/versions/JetRoI_v2.h"

namespace xAOD{
   typedef JetRoI_v2 JetRoI;
}

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetRoI, 105032169, 1 )


#endif // XAODTRIGGER_JETROI_H
