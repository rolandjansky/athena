// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_TRIGRNNOUTPUTAUXCONTAINER_H
#define XAODTRIGRINGER_TRIGRNNOUTPUTAUXCONTAINER_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRings class
   typedef TrigRNNOutputAuxContainer_v2 TrigRNNOutputAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRNNOutputAuxContainer , 1268129404 , 1 )

#endif // 
