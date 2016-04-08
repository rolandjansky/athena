// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_TRIGRNNOUTPUTCONTAINER_H
#define XAODTRIGRINGER_TRIGRNNOUTPUTCONTAINER_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TrigRNNOutputContainer class
   typedef TrigRNNOutputContainer_v2  TrigRNNOutputContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRNNOutputContainer , 1107329221 , 1 )

#endif // 
