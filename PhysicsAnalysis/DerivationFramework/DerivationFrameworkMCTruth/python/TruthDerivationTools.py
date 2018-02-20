# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
DFCommonTruthClassifier = MCTruthClassifier(name = "DFCommonTruthClassifier",
                                    ParticleCaloExtensionTool = "") 
ToolSvc += DFCommonTruthClassifier

#==============================================================================
# Schedule the tools for adding new truth collection
# Note that taus are handled separately (see MCTruthCommon.py)
# Note also that navigation info is dropped here and added separately
#==============================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker

DFCommonTruthMuonTool = DerivationFramework__TruthCollectionMaker(name           = "DFCommonTruthMuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         KeepNavigationInfo      = False,
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthMuonTool
DFCommonTruthElectronTool = DerivationFramework__TruthCollectionMaker(name       = "DFCommonTruthElectronTool",
                                                         NewCollectionName       = "TruthElectrons",
                                                         KeepNavigationInfo      = False,
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthElectronTool
DFCommonTruthPhotonTool = DerivationFramework__TruthCollectionMaker(name         = "DFCommonTruthPhotonTool",
                                                         NewCollectionName       = "TruthPhotons",
                                                         KeepNavigationInfo      = False,
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthPhotonTool
# this tool is needed for making TruthPhotons from sim samples, where extra cuts are needed
DFCommonTruthPhotonToolSim = DerivationFramework__TruthCollectionMaker(name      = "DFCommonTruthPhotonToolSim",
                                                         NewCollectionName       = "TruthPhotons",
                                                         KeepNavigationInfo      = False,
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && ((TruthParticles.classifierParticleOrigin != 42) || (TruthParticles.pt > 20.0*GeV)) && ( TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset)+")")
ToolSvc += DFCommonTruthPhotonToolSim

neutrinoexpression = "(TruthParticles.isNeutrino && TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset)
DFCommonTruthNeutrinoTool = DerivationFramework__TruthCollectionMaker(name                 = "DFCommonTruthNeutrinoTool",
                                                                   NewCollectionName       = "TruthNeutrinos",
                                                                   KeepNavigationInfo      = False,
                                                                   ParticleSelectionString = neutrinoexpression)
ToolSvc += DFCommonTruthNeutrinoTool

DFCommonTruthTopTool = DerivationFramework__TruthCollectionMaker(name                   = "DFCommonTruthTopTool",
                                                                NewCollectionName       = "TruthTop",
                                                                KeepNavigationInfo      = False,
                                                                ParticleSelectionString = "(abs(TruthParticles.pdgId) == 6)",
                                                                Do_Compress             = True,
                                                          )
ToolSvc += DFCommonTruthTopTool

DFCommonTruthBosonTool = DerivationFramework__TruthCollectionMaker(name                   = "DFCommonTruthBosonTool",
                                                                  NewCollectionName       = "TruthBoson",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 23 || abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25)",
                                                                  Do_Compress             = True,
                                                                  Do_Sherpa               = True)
ToolSvc += DFCommonTruthBosonTool

DFCommonTruthBSMTool = DerivationFramework__TruthCollectionMaker(name                   = "DFCommonTruthBSMTool",
                                                                NewCollectionName       = "TruthBSM",
                                                                KeepNavigationInfo      = False,
                                                                ParticleSelectionString = "(TruthParticles.isBSM)",
                                                                Do_Compress             = True)
ToolSvc += DFCommonTruthBSMTool

#==============================================================================
# Decoration tools
#==============================================================================

#add the 'decoration' tool to dress the main truth collection with the classification
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
DFCommonTruthClassificationTool = DerivationFramework__TruthClassificationDecorator(name = "DFCommonTruthClassificationTool",
                                                                             ParticlesKey = "TruthParticles",
                                                                             MCTruthClassifier = DFCommonTruthClassifier)  
ToolSvc += DFCommonTruthClassificationTool

#add the 'decoration' tools for dressing and isolation
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool
DFCommonTruthElectronDressingTool = DerivationFramework__TruthDressingTool(name = "DFCommonTruthElectronDressingTool",
                                                                  dressParticlesKey = "TruthElectrons",
                                                                  usePhotonsFromHadrons = False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [11]
                                                                  )
ToolSvc += DFCommonTruthElectronDressingTool
DFCommonTruthMuonDressingTool = DerivationFramework__TruthDressingTool(name = "DFCommonTruthMuonDressingTool",
                                                                  dressParticlesKey = "TruthMuons",
                                                                  usePhotonsFromHadrons = False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [13]
                                                                  )
ToolSvc += DFCommonTruthMuonDressingTool
DFCommonTruthTauDressingTool = DerivationFramework__TruthDressingTool(name = "DFCommonTruthTauDressingTool",
                                                                  dressParticlesKey = "TruthTaus",
                                                                  usePhotonsFromHadrons = False,
                                                                  dressingConeSize = 0.2, # Tau special
                                                                  particleIDsToDress = [15]
                                                                  )
ToolSvc += DFCommonTruthTauDressingTool

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
DFCommonTruthElectronIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthElectronIsolationTool1",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthElectronIsolationTool1
DFCommonTruthElectronIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthElectronIsolationTool2",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthElectronIsolationTool2
DFCommonTruthMuonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthMuonIsolationTool1",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthMuonIsolationTool1
DFCommonTruthMuonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthMuonIsolationTool2",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthMuonIsolationTool2
DFCommonTruthPhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthPhotonIsolationTool1",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthPhotonIsolationTool1
DFCommonTruthPhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthPhotonIsolationTool2",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthPhotonIsolationTool2
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthQGDecorationTool
DFCommonTruthQGLabelTool = DerivationFramework__TruthQGDecorationTool(name="DFCommonTruthQGLabelTool",
                                                                  JetCollection = "AntiKt4TruthWZJets")
ToolSvc += DFCommonTruthQGLabelTool
