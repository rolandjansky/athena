// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_ZDCMODULE_H
#define XAODFORWARD_ZDCMODULE_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/ZdcModule_v1.h"

namespace xAOD {
   typedef ZdcModule_v1 ZdcModule;
}

// Declare a CLID for the type:
CLASS_DEF( xAOD::ZdcModule, 144816136, 1 )

#endif // XAODFORWARD_ZDCMODULE_H
