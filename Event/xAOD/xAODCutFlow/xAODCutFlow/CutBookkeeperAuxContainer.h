// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_CUTBOOKKEEPERAUXCONTAINER_H
#define XAODCUTFLOW_CUTBOOKKEEPERAUXCONTAINER_H

// Local include(s):
#include "xAODCutFlow/versions/CutBookkeeperAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CutBookkeeperAuxContainer class
   typedef CutBookkeeperAuxContainer_v1 CutBookkeeperAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CutBookkeeperAuxContainer, 1147935274, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::CutBookkeeperAuxContainer> , 1422109 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::CutBookkeeperAuxContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODCUTFLOW_CUTBOOKKEEPERAUXCONTAINER_H
