// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoI.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_EMTAUROI_H
#define XAODTRIGGER_EMTAUROI_H

// Local include(s):
#include "xAODTrigger/versions/EmTauRoI_v1.h"
#include "xAODTrigger/versions/EmTauRoI_v2.h"

namespace xAOD{
   typedef EmTauRoI_v2 EmTauRoI;
}

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EmTauRoI, 113152536, 1 )

#endif // XAODTRIGGER_EMTAUROI_H
