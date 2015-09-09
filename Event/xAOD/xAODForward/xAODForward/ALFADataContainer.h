// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataContainer.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_ALFADATACONTAINER_H
#define XAODFORWARD_ALFADATACONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/ALFAData.h"
#include "xAODForward/versions/ALFADataContainer_v1.h"

namespace xAOD {
   typedef ALFADataContainer_v1 ALFADataContainer;
}

// Set up a CLID for the container:
CLASS_DEF( xAOD::ALFADataContainer, 1278139236, 1 )

#endif // XAODFORWARD_ALFADATACONTAINER_H
