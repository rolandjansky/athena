# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

#add the custom tools for new truth collections
from DerivationFrameworkMCTruth.TruthObjectTools import *

#add the 'decoration' tools for dressing and isolation
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool
TRUTH1ElectronDressingTool = DerivationFramework__TruthDressingTool(name = "TRUTH1ElectronDressingTool",
                                                                  dressParticlesKey = "TruthElectrons",
                                                                  usePhotonsFromHadrons	 	= False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [11]
                                                                  )
ToolSvc += TRUTH1ElectronDressingTool
TRUTH1MuonDressingTool = DerivationFramework__TruthDressingTool(name = "TRUTH1MuonDressingTool",
                                                                  dressParticlesKey = "TruthMuons",
                                                                  usePhotonsFromHadrons	 	= False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [13]
                                                                  )
ToolSvc += TRUTH1MuonDressingTool

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
TRUTH1ElectronIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "TRUTH1ElectronIsolationTool1",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += TRUTH1ElectronIsolationTool1
TRUTH1ElectronIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "TRUTH1ElectronIsolationTool2",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += TRUTH1ElectronIsolationTool2
TRUTH1MuonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "TRUTH1MuonIsolationTool1",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += TRUTH1MuonIsolationTool1
TRUTH1MuonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "TRUTH1MuonIsolationTool2",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += TRUTH1MuonIsolationTool2
TRUTH1PhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "TRUTH1PhotonIsolationTool1",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += TRUTH1PhotonIsolationTool1
TRUTH1PhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "TRUTH1PhotonIsolationTool2",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += TRUTH1PhotonIsolationTool2
