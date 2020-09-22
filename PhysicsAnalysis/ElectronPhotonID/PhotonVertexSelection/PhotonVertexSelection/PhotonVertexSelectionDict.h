/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_PhotonVertexSelectionDict_H
#define PhotonVertexSelection_PhotonVertexSelectionDict_H

#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"
#include "PhotonVertexSelection/PhotonVertexSelectionTool.h"

#include "PhotonVertexSelection/IPhotonPointingTool.h"
#include "PhotonVertexSelection/PhotonPointingTool.h"

#include "PhotonVertexSelection/IElectronRemovalTool.h"
#include "PhotonVertexSelection/ElectronRemovalTool.h"

#include "PhotonVertexSelection/PhotonVertexHelpers.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_PhotonVertexSelection {
    std::pair<const xAOD::Vertex*, float> pair1;
    std::vector<std::pair<const xAOD::Vertex*, float> > vector1;
  };
}

#endif // PhotonVertexSelection_PhotonVertexSelectionDict_H
