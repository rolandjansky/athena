// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRings.h 563319 2014-10-17 joaoVictor $
#ifndef XAODTRIGRINGER_TRIGRNNOUTPUTCONTAINER_H
#define XAODTRIGRINGER_TRIGRNNOUTPUTCONTAINER_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRNNOutputContainer class
   typedef TrigRNNOutputContainer_v1 TrigRNNOutputContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRNNOutputContainer, 1107329221 , 1 )

#endif // 
