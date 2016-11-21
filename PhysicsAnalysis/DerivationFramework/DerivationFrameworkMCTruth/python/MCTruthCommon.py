# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.TruthDerivationTools import *

dfInputIsEVNT = False # Flag to distinguish EVNT from AOD input
# Build truth collection if input is HepMC. Must be scheduled first to allow slimming.
# Input file is EVNT
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
	DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT"))
	dfInputIsEVNT = True
# Input file is HITS
elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
	DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="TruthEvent"))
	dfInputIsEVNT = True
# Input file must be xAOD
if (dfInputIsEVNT==False):
	# xAOD input so we need to schedule the special truth building tools and add them to a common augmentation
	augmentationToolsList = [  DFCommonTruthClassificationTool,
	                           DFCommonTruthMuonTool,DFCommonTruthElectronTool,
	                           DFCommonTruthPhotonToolSim,
	                           #DFCommonTruthTauTool,
                                   DFCommonTruthNeutrinoTool,
				   TRUTH3TopTool,
				   TRUTH3BosonTool,
				   TRUTH3BSMTool,
	                           DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
	                           DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
	                           DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
	                           DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2]
	from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
	DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonKernel",
										 AugmentationTools = augmentationToolsList
                                                                         	)
