# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

#==============================================================================
# Set up the MCTruthClassifier
#============================================================================== 

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
TRUTH1Classifier = MCTruthClassifier(name = "TRUTH1Classifier",
                                    ParticleCaloExtensionTool="") 
ToolSvc += TRUTH1Classifier

#==============================================================================
# Schedule the tool for adding new truth collection
#==============================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau

TRUTH1MuonTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH1MuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1)",
                                                         MCTruthClassifier       = TRUTH1Classifier,
                                                         RunClassifier           = True)
ToolSvc += TRUTH1MuonTool
TRUTH1ElectronTool = DerivationFramework__TruthCollectionMaker(name               = "TRUTH1ElectronTool",
                                                         NewCollectionName       = "TruthElectrons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1)",
                                                         MCTruthClassifier       = TRUTH1Classifier,
                                                         RunClassifier           = True)
ToolSvc += TRUTH1ElectronTool
TRUTH1PhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "TRUTH1PhotonTool",
                                                         NewCollectionName       = "TruthPhotons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1)",
                                                         MCTruthClassifier       = TRUTH1Classifier,
                                                         RunClassifier           = True)
ToolSvc += TRUTH1PhotonTool
TRUTH1TauTool = DerivationFramework__TruthCollectionMakerTau(name                    = "TRUTH1TauTool",
                                                             NewCollectionName       = "TruthTaus",
                                                             MCTruthClassifier       = TRUTH1Classifier,
                                                             RunClassifier           = True)
ToolSvc += TRUTH1TauTool

neutrinoexpression = "(abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.status == 1)"
TRUTH1NeutrinoTool = DerivationFramework__TruthCollectionMaker(name                 = "TRUTH1NeutrinoTool",
                                                             NewCollectionName       = "TruthNeutrinos",
                                                             ParticleSelectionString = neutrinoexpression,
                                                             MCTruthClassifier       = TRUTH1Classifier,
                                                             RunClassifier           = True)
ToolSvc += TRUTH1NeutrinoTool
