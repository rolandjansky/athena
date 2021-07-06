/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSYSTEMEXTENSIONCOLLECTION_H
#define MUON_MUONSYSTEMEXTENSIONCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODCore/CLASS_DEF.h"

typedef DataVector<Muon::MuonSystemExtension> MuonSystemExtensionCollection;
CLASS_DEF(MuonSystemExtensionCollection, 1302347472, 1)

#endif
