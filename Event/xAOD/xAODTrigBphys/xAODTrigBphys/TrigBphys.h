/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGBPHYS_TRIGBPHYS_H
#define XAODTRIGBPHYS_TRIGBPHYS_H

// Local include(s):
#include "xAODTrigBphys/versions/TrigBphys_v1.h"
/**
 Description: To be created by hypothesis algorithms 
              chains. To be used as a way to communicate EF decisions and
              to keep information available only online so far.

**/

namespace xAOD{
    typedef TrigBphys_v1 TrigBphys ;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigBphys, 52725123, 1 )

#endif
