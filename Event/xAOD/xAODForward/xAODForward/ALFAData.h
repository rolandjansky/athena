// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_ALFADATA_H
#define XAODFORWARD_ALFADATA_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/ALFAData_v1.h"

namespace xAOD {
   typedef ALFAData_v1 ALFAData;
}

// Declare a CLID for the type:
CLASS_DEF( xAOD::ALFAData, 197074916, 1 )

#endif // XAODFORWARD_ALFADATA_H
