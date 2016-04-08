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

// version 1
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"

// version 2
#include "xAODTrigRinger/versions/TrigRingerRings_v2.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v2.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"

// ElementLink<xAOD::*Container_v1> compiling in the dictionary. The CLID
/// is not needed in the "real" code, since users are never exposed to the _v1
/// classes in Athena anymore.
//#include "xAODCore/CLASS_DEF.h"
//CLASS_DEF( xAOD::TrigRingerRingsContainer_v1 , 1337532868 , 10 )
//CLASS_DEF( xAOD::TrigRNNOutputContainer_v1   , 1103699247 , 10 )


namespace{

  struct GCCXML_DUMMY_INSTANTIATION_XAODRINGERRINGS {

    // version 1 
    xAOD::TrigRingerRingsContainer_v1 c3;
    ElementLink< xAOD::TrigRingerRingsContainer_v1 > a1;
    std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v1 > > a3;
    std::vector< std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v1 > > > a4;
      
    xAOD::TrigRNNOutputContainer_v1 c4;
    ElementLink< xAOD::TrigRNNOutputContainer_v1 > b1;
    std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v1 > > b3;
    std::vector< std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v1 > > > b4;
  
    // version 2
    xAOD::TrigRingerRingsContainer_v2 v2_c3;
    ElementLink< xAOD::TrigRingerRingsContainer_v2 > v2_a1;
    std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v2 > > v2_a3;
    std::vector< std::vector< ElementLink< xAOD::TrigRingerRingsContainer_v2 > > > v2_a4;
     
    xAOD::TrigRNNOutputContainer_v2 v2_c4;
    ElementLink< xAOD::TrigRNNOutputContainer_v2 > v2_b1;
    std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v2 > > v2_b3;
    std::vector< std::vector< ElementLink< xAOD::TrigRNNOutputContainer_v2 > > > v2_b4;

    // Declarations needed in order to generate the dictionaries of the
    // auxiliary containers correctly:
    ElementLink< xAOD::TrigEMClusterContainer > dummy1;
    std::vector< ElementLink< xAOD::TrigEMClusterContainer > > dummy2;

  };
}

#endif // 
