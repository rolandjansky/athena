// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoI.h 791220 2017-01-01 04:52:41Z ssnyder $
#ifndef XAODTRIGL1CALO_JEMTOBROI_H
#define XAODTRIGL1CALO_JEMTOBROI_H

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMTobRoI_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the JEMTobRoI class
  typedef JEMTobRoI_v1 JEMTobRoI;

  typedef std::map<int, const JEMTobRoI*> JEMTobRoIMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMTobRoI , 257073972 , 1 )
#endif 

