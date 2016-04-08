# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#==============================================================================
# Set up the MCTruthClassifier
#============================================================================== 

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
STDMClassifier = MCTruthClassifier( name                      = "STDMClassifier",
                                     ParticleCaloExtensionTool = "" ) 
ToolSvc += STDMClassifier

#add the 'decoration' tool to dress the main truth collection with the classification
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
STDMTruthClassificationTool = DerivationFramework__TruthClassificationDecorator(name = "STDMTruthClassificationTool",
                                                                             ParticlesKey = "TruthParticles",
                                                                             MCTruthClassifier = STDMClassifier)  
ToolSvc += STDMTruthClassificationTool

#==============================================================================
# Schedule the tool for adding new truth collection
#==============================================================================
STDMAugmentationTools=[STDMTruthClassificationTool]



from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker

truthmuonexpression = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status ==1) && (TruthParticles.barcode < 200000)"
STDMMuonTool = DerivationFramework__TruthCollectionMaker(name                   = "STDMMuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         ParticleSelectionString = truthmuonexpression,
                                                         )
ToolSvc += STDMMuonTool
STDMAugmentationTools.append(STDMMuonTool)

truthelecexpression = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status ==1) && (TruthParticles.barcode < 200000)"
STDMElectronTool = DerivationFramework__TruthCollectionMaker(name               = "STDMElectronTool",
                                                             NewCollectionName       = "TruthElectrons",
                                                             ParticleSelectionString = truthelecexpression,
#                                                             MCTruthClassifier       = STDMClassifier,
                                                             )

ToolSvc += STDMElectronTool
STDMAugmentationTools.append(STDMElectronTool)

truthphotexpression = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status ==1) && (TruthParticles.barcode < 200000)"
STDMPhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "STDMPhotonTool",
                                                            NewCollectionName       = "TruthPhotons",
                                                            ParticleSelectionString = truthphotexpression,
#                                                            MCTruthClassifier       = STDMClassifier,
                                                            )

ToolSvc += STDMPhotonTool
STDMAugmentationTools.append(STDMPhotonTool)

STDMTauTool = DerivationFramework__TruthCollectionMaker(name                    = "STDMTauTool",
                                                        NewCollectionName       = "TruthTaus",
                                                        ParticleSelectionString = "abs(TruthParticles.pdgId) == 15",
#                                                        MCTruthClassifier       = STDMClassifier,
                                                        )

ToolSvc += STDMTauTool
STDMAugmentationTools.append(STDMTauTool)

neutrinoexpression = "(abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.status == 1)"
STDMNeutrinoTool = DerivationFramework__TruthCollectionMaker(name                 = "STDMNeutrinoTool",
                                                             NewCollectionName       = "TruthNeutrinos",
                                                             ParticleSelectionString = neutrinoexpression,
#                                                             MCTruthClassifier       = STDMClassifier,
                                                             )
ToolSvc += STDMNeutrinoTool
STDMAugmentationTools.append(STDMNeutrinoTool)



#==============================================================================
# TRUTH DRESSING
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool

STDMElectronDressingTool = DerivationFramework__TruthDressingTool(name = "STDMElectronDressingTool",
                                                                    dressParticlesKey = "TruthElectrons",
                                                                    usePhotonsFromHadrons         = False,
                                                                    dressingConeSize = 0.1,
                                                                    particleIDsToDress = [11]
                                                                    )
ToolSvc += STDMElectronDressingTool
STDMAugmentationTools.append(STDMElectronDressingTool)


STDMMuonDressingTool = DerivationFramework__TruthDressingTool(name = "STDMMuonDressingTool",
                                                               dressParticlesKey = "TruthMuons",
                                                               usePhotonsFromHadrons         = False,
                                                               dressingConeSize = 0.1,
                                                               particleIDsToDress = [13]
                                                               )
ToolSvc += STDMMuonDressingTool
STDMAugmentationTools.append(STDMMuonDressingTool)

STDMElectronDressingTool2 = DerivationFramework__TruthDressingTool(name = "STDMElectronDressingTool2",
                                                                    dressParticlesKey = "TruthParticles",
                                                                    usePhotonsFromHadrons         = False,
                                                                    dressingConeSize = 0.1,
                                                                    particleIDsToDress = [11]
                                                                    )

ToolSvc += STDMElectronDressingTool2
STDMAugmentationTools.append(STDMElectronDressingTool2)

STDMMuonDressingTool2 = DerivationFramework__TruthDressingTool(name = "STDMMuonDressingTool2",
                                                                dressParticlesKey = "TruthParticles",
                                                                usePhotonsFromHadrons         = False,
                                                                dressingConeSize = 0.1,
                                                                particleIDsToDress = [13]
                                                                )

ToolSvc += STDMMuonDressingTool2
STDMAugmentationTools.append(STDMMuonDressingTool2)


#==============================================================================
# CREATE THE COMMON AUGMENTATION KERNEL ALGORITHM
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("STDMAugmentationKernel", AugmentationTools = STDMAugmentationTools)
