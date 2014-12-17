// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoIContainer.h 632693 2014-12-01 15:54:08Z stelzer $
#ifndef XAODTRIGGER_MUONROICONTAINER_H
#define XAODTRIGGER_MUONROICONTAINER_H

// Local include(s):
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/versions/MuonRoIContainer_v1.h"

namespace xAOD{
   typedef MuonRoIContainer_v1 MuonRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonRoIContainer, 1311809787, 1 )

#endif // XAODTRIGGER_MUONROICONTAINER_H
