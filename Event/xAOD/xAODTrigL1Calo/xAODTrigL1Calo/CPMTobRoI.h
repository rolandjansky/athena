// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoI.h 791220 2017-01-01 04:52:41Z ssnyder $
#ifndef XAODTRIGL1CALO_CPMTOBROI_H
#define XAODTRIGL1CALO_CPMTOBROI_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTobRoI_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CPMTobRoI class
  typedef CPMTobRoI_v1 CPMTobRoI;

  typedef std::map<int, const CPMTobRoI*> CPMTobRoIMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMTobRoI , 73031610 , 1 )
#endif 

