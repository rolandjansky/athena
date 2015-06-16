# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg

dfInputIsEVNT = False # Flag to distinguish EVNT from AOD input
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
	DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT")
	dfInputIsEVNT = True
elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
	DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="TruthEvent")
	dfInputIsEVNT = True
if (dfInputIsEVNT==False):
	# xAOD input so we need to schedule the special truth building tools and add them to a common augmentation
	from DerivationFrameworkMCTruth.TruthObjectTools import *
	from DerivationFrameworkMCTruth.TruthDecoratorTools import *

	#==============================================================================
	# Thinning the photon truth collection : no photons from pi0 (origin=42)
	#==============================================================================
	from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
	TRUTH1PhotonThinning = DerivationFramework__GenericTruthThinning(name            = "TRUTH1PhotonThinning",
                                                                 ThinningService         = "TRUTH1ThinningSvc",
                                                                 ParticlesKey            = "TruthPhotons",  
                                                                 ParticleSelectionString = "(TruthPhotons.particleOrigin != 42) || (TruthPhotons.pt > 20.0*GeV)")
	ToolSvc += TRUTH1PhotonThinning

	from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
	DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonKernel",
										 AugmentationTools = [	TRUTH1MuonTool,TRUTH1ElectronTool,TRUTH1PhotonTool,TRUTH1TauTool,
													TRUTH1ElectronDressingTool, TRUTH1MuonDressingTool,
													TRUTH1ElectronIsolationTool1, TRUTH1ElectronIsolationTool2,
													TRUTH1MuonIsolationTool1, TRUTH1MuonIsolationTool2,
													TRUTH1PhotonIsolationTool1, TRUTH1PhotonIsolationTool2]
                                                                         	)
		
