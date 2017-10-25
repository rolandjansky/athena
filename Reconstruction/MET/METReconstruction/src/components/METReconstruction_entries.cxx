// Top level tool
#include "METReconstruction/METRecoTool.h"
#include "METReconstruction/METAssociationTool.h"
// Builders
#include "METReconstruction/METElectronTool.h"
#include "METReconstruction/METPhotonTool.h"
#include "METReconstruction/METJetTool.h"
#include "METReconstruction/METTauTool.h"
#include "METReconstruction/METMuonTool.h"
#include "METReconstruction/METSoftTermsTool.h"
#include "METReconstruction/METElectronAssociator.h"
#include "METReconstruction/METPhotonAssociator.h"
#include "METReconstruction/METJetAssocTool.h"
#include "METReconstruction/METTauAssociator.h"
#include "METReconstruction/METMuonAssociator.h"
#include "METReconstruction/METSoftAssociator.h"
// Truth tool
#include "METReconstruction/METTruthTool.h"
#include "METReconstruction/METTruthAssociator.h"
// CaloRegions
#include "METReconstruction/METCaloRegionsTool.h"
// Refiners
#include "METReconstruction/METJetFilterTool.h"
#include "METReconstruction/METTrackFilterTool.h"
#include "METReconstruction/METMuonElossTool.h"
#include "METReconstruction/METRegionsTool.h"
// Algs
#include "METRecoAlg.h"
#include "METReaderAlg.h"
#include "METAssocTestAlg.h"

using namespace met;

DECLARE_TOOL_FACTORY(METRecoTool)
DECLARE_TOOL_FACTORY(METAssociationTool)
DECLARE_TOOL_FACTORY(METElectronTool)
DECLARE_TOOL_FACTORY(METPhotonTool)
DECLARE_TOOL_FACTORY(METJetTool)
DECLARE_TOOL_FACTORY(METTauTool)
DECLARE_TOOL_FACTORY(METMuonTool)
DECLARE_TOOL_FACTORY(METSoftTermsTool)
DECLARE_TOOL_FACTORY(METElectronAssociator)
DECLARE_TOOL_FACTORY(METPhotonAssociator)
DECLARE_TOOL_FACTORY(METJetAssocTool)
DECLARE_TOOL_FACTORY(METTauAssociator)
DECLARE_TOOL_FACTORY(METMuonAssociator)
DECLARE_TOOL_FACTORY(METSoftAssociator)
//
DECLARE_TOOL_FACTORY(METTruthTool)
DECLARE_TOOL_FACTORY(METTruthAssociator)
DECLARE_TOOL_FACTORY(METCaloRegionsTool)
//
DECLARE_TOOL_FACTORY(METJetFilterTool)
DECLARE_TOOL_FACTORY(METTrackFilterTool)
DECLARE_TOOL_FACTORY(METMuonElossTool)
DECLARE_TOOL_FACTORY(METRegionsTool)
//
DECLARE_ALGORITHM_FACTORY(METRecoAlg)
DECLARE_ALGORITHM_FACTORY(METReaderAlg)
DECLARE_ALGORITHM_FACTORY(METAssocTestAlg)

