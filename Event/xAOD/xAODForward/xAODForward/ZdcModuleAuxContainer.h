// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_ZDCMODULEAUXCONTAINER_H
#define XAODFORWARD_ZDCMODULEAUXCONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/ZdcModuleAuxContainer_v1.h"

namespace xAOD {
   typedef ZdcModuleAuxContainer_v1 ZdcModuleAuxContainer;
}

// Set up a CLID for the class:
CLASS_DEF( xAOD::ZdcModuleAuxContainer , 1177096607 , 1 )

#endif // XAODFORWARD_ZDCMODULEAUXCONTAINER_H
