/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigHisto2DCONTAINER_V1_H
#define xAODTrigMinBias_TrigHisto2DCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMinBias/versions/TrigHisto2D_v1.h"

namespace xAOD {
  typedef DataVector<xAOD::TrigHisto2D_v1> TrigHisto2DContainer_v1;
  
}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"

CLASS_DEF( xAOD::TrigHisto2DContainer_v1 , 1207147901 , 1 )

#endif

#endif
