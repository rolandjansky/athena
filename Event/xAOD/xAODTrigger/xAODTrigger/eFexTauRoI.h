// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEFEXTAUROI_H
#define XAODEFEXTAUROI_H

// Local include(s):
#include "xAODTrigger/versions/eFexTauRoI_v1.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the eFexTauRoI class
  typedef eFexTauRoI_v1 eFexTauRoI;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexTauRoI , 27246462 , 1 )
#endif // XAODEFEXTAUROI_H

