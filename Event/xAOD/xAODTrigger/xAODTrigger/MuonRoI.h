// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoI.h 632693 2014-12-01 15:54:08Z stelzer $
#ifndef XAODTRIGGER_MUONROI_H
#define XAODTRIGGER_MUONROI_H

// Local include(s):
#include "xAODTrigger/versions/MuonRoI_v1.h"

namespace xAOD{
   typedef MuonRoI_v1 MuonRoI;
}

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonRoI, 84752656, 1 )

#endif // XAODTRIGGER_MUONROI_H
