// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPData.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_AFPDATA_H
#define XAODFORWARD_AFPDATA_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/AFPData_v1.h"

namespace xAOD
{
   typedef AFPData_v1 AFPData;
}

// Declare a CLID for the type:
CLASS_DEF( xAOD::AFPData, 121471926, 1 )

#endif // XAODFORWARD_AFPDATA_H
