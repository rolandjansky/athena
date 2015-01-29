// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTobContainer.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_CMXJETTOBCONTAINER_H
#define XAODTRIGL1CALO_CMXJETTOBCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMXJetTob.h"
#include "xAODTrigL1Calo/versions/CMXJetTobContainer_v2.h"

// Namespace holding all the xAOD EDM classes
namespace xAOD{
  /// Define the latest version of the CMMCPHits class
  typedef CMXJetTobContainer_v2 CMXJetTobContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMXJetTobContainer , 1232206365 , 1 )
#endif 

