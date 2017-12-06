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

DECLARE_COMPONENT( METRecoTool )
DECLARE_COMPONENT( METAssociationTool )
DECLARE_COMPONENT( METElectronTool )
DECLARE_COMPONENT( METPhotonTool )
DECLARE_COMPONENT( METJetTool )
DECLARE_COMPONENT( METTauTool )
DECLARE_COMPONENT( METMuonTool )
DECLARE_COMPONENT( METSoftTermsTool )
DECLARE_COMPONENT( METElectronAssociator )
DECLARE_COMPONENT( METPhotonAssociator )
DECLARE_COMPONENT( METJetAssocTool )
DECLARE_COMPONENT( METTauAssociator )
DECLARE_COMPONENT( METMuonAssociator )
DECLARE_COMPONENT( METSoftAssociator )
//
DECLARE_COMPONENT( METTruthTool )
DECLARE_COMPONENT( METTruthAssociator )
DECLARE_COMPONENT( METCaloRegionsTool )
//
DECLARE_COMPONENT( METJetFilterTool )
DECLARE_COMPONENT( METTrackFilterTool )
DECLARE_COMPONENT( METMuonElossTool )
DECLARE_COMPONENT( METRegionsTool )
//
DECLARE_COMPONENT( METRecoAlg )
DECLARE_COMPONENT( METReaderAlg )
DECLARE_COMPONENT( METAssocTestAlg )

