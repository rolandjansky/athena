# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "jobOptions to test EGammaAmbiguityTool"
__author__ = "Bruno Lenzi"

import AthenaPoolCnvSvc.ReadAthenaPool # EventSelector
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections = ['root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/valid3/24/c7/AOD.05062229._000129.pool.root.1']

# Redefine the function InputFileNames to make autoconfiguration work 
# outside RecExCommon
from RecExConfig import RecoFunctions
RecoFunctions.InputFileNames = lambda : svcMgr.EventSelector.InputCollections
from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys(['everything'])

###################

from egammaRec.Factories import ToolFactory, AlgFactory

# Add EGammaAmbiguityTool to ToolSvc
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import EGammaAmbiguityTool
ToolFactory(EGammaAmbiguityTool)()

# Add algorithm to AlgSequence
from ElectronPhotonSelectorTools.TestEGammaAmbiguityTool import TestEGammaAmbiguityAlg
AlgFactory(TestEGammaAmbiguityAlg)(doAdd=True)

