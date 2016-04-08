// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_TRIGRNNOUTPUT_H
#define XAODTRIGRINGER_TRIGRNNOUTPUT_H

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the RingerRings class
   typedef TrigRNNOutput_v2 TrigRNNOutput;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigRNNOutput , 191776029 , 1 )

#endif // 
