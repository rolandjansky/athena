#include "GaudiKernel/DeclareFactoryEntries.h"

// Top level tool
#include "METReconstruction/METRecoTool.h"
// Builders
#include "METReconstruction/METElectronTool.h"
#include "METReconstruction/METPhotonTool.h"
#include "METReconstruction/METJetTool.h"
#include "METReconstruction/METTauTool.h"
#include "METReconstruction/METMuonTool.h"
#include "METReconstruction/METSoftTermsTool.h"
// Truth tool
#include "METReconstruction/METTruthTool.h"
// CaloRegions
#include "METReconstruction/METCaloRegionsTool.h"
// Refiners
#include "METReconstruction/METJetFilterTool.h"
#include "METReconstruction/METTrackFilterTool.h"
#include "METReconstruction/METMuonElossTool.h"
#include "METReconstruction/METRegionsTool.h"
// Algs
#include "../src/METRecoAlg.h"
#include "../src/METReaderAlg.h"

using namespace met;

DECLARE_TOOL_FACTORY(METRecoTool)
DECLARE_TOOL_FACTORY(METElectronTool)
DECLARE_TOOL_FACTORY(METPhotonTool)
DECLARE_TOOL_FACTORY(METJetTool)
DECLARE_TOOL_FACTORY(METTauTool)
DECLARE_TOOL_FACTORY(METMuonTool)
DECLARE_TOOL_FACTORY(METSoftTermsTool)
//
DECLARE_TOOL_FACTORY(METTruthTool)
DECLARE_TOOL_FACTORY(METCaloRegionsTool)
//
DECLARE_TOOL_FACTORY(METJetFilterTool)
DECLARE_TOOL_FACTORY(METTrackFilterTool)
DECLARE_TOOL_FACTORY(METMuonElossTool)
DECLARE_TOOL_FACTORY(METRegionsTool)
//
DECLARE_ALGORITHM_FACTORY(METRecoAlg)
DECLARE_ALGORITHM_FACTORY(METReaderAlg)

DECLARE_FACTORY_ENTRIES(METReconstruction) {
  DECLARE_TOOL(METRecoTool)
  DECLARE_TOOL(METElectronTool)
  DECLARE_TOOL(METPhotonTool)
  DECLARE_TOOL(METJetTool)
  DECLARE_TOOL(METTauTool)
  DECLARE_TOOL(METMuonTool)
  DECLARE_TOOL(METSoftTermsTool)
    //
  DECLARE_TOOL(METTruthTool)
  DECLARE_TOOL(METCaloRegionsTool)
    //
  DECLARE_TOOL(METJetFilterTool)
  DECLARE_TOOL(METTrackFilterTool)
  DECLARE_TOOL(METMuonElossTool)
  DECLARE_TOOL(METRegionsTool)
    //
  DECLARE_ALGORITHM(METRecoAlg)
  DECLARE_ALGORITHM(METReaderAlg)
}
