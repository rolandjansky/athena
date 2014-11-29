// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRNNOutput.h 563319 2014-10-17 joaoVictor $
#ifndef XAODTRIGRINGER_TRIGRNNOUTPUT_H
#define XAODTRIGRINGER_TRIGRNNOUTPUT_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRings class
   typedef TrigRNNOutput_v1 TrigRNNOutput;
}


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRNNOutput,191776029, 1 )  // <- maybe this will change.

#endif // 
