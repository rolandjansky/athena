// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_XAODTRIGRINGERDICT_H
#define XAODTRIGRINGER_XAODTRIGRINGERDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"

namespace{
  struct GCCXML_DUMMY_INSTANTIATION_XAODRINGERRINGS {

    xAOD::TrigRingerRingsContainer_v1 c3;
    ElementLink< xAOD::TrigRingerRingsContainer_v1 > a1;
    std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v1 > > a3;
    std::vector< std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v1 > > > a4;
      
    xAOD::TrigRNNOutputContainer_v1 c4;
    ElementLink< xAOD::TrigRNNOutputContainer_v1 > b1;
    std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v1 > > b3;
    std::vector< std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v1 > > > b4;
  
  };
}

#endif // 
