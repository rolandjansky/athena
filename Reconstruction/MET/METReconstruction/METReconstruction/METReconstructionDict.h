/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METRECONSTRUCTIONDICT_H
#define METRECONSTRUCTIONDICT_H

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "METReconstruction/METBuilderTool.h"
#include "METReconstruction/METElectronTool.h"
#include "METReconstruction/METPhotonTool.h"
#include "METReconstruction/METTauTool.h"
#include "METReconstruction/METJetTool.h"
#include "METReconstruction/METMuonTool.h"
#include "METReconstruction/METSoftTermsTool.h"
#include "METReconstruction/METTruthTool.h"
#include "METReconstruction/METCaloRegionsTool.h"
#include "METReconstruction/METRegionsTool.h"
#include "METReconstruction/METTrackFilterTool.h"
#include "METReconstruction/METRecoTool.h"

#include "METReconstruction/METAssociator.h"
#include "METReconstruction/METJetAssocTool.h"
#include "METReconstruction/METEgammaAssociator.h"
#include "METReconstruction/METElectronAssociator.h"
#include "METReconstruction/METPhotonAssociator.h"
#include "METReconstruction/METTauAssociator.h"
#include "METReconstruction/METMuonAssociator.h"
#include "METReconstruction/METSoftAssociator.h"
#include "METReconstruction/METAssociationTool.h"

#endif //METRECONSTRUCTIONDICT_H
