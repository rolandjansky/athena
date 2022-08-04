# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
## GenParticle Filters
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticleFinalStateFilter",            "ISF_ParticleFinalStateFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticleSimWhiteList",                "ISF_ParticleSimWhiteList")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticleSimWhiteList_ExtraParticles", "ISF_ParticleSimWhiteList_ExtraParticles")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getGenParticleSimQuasiStableFilter",     "ISF_GenParticleSimQuasiStableFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticlePositionFilterID",            "ISF_ParticlePositionFilterID")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticlePositionFilterCalo",          "ISF_ParticlePositionFilterCalo")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticlePositionFilterMS",            "ISF_ParticlePositionFilterMS")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticlePositionFilterWorld",         "ISF_ParticlePositionFilterWorld")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getParticlePositionFilterDynamic",       "ISF_ParticlePositionFilterDynamic")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getGenParticleInteractingFilter",        "ISF_GenParticleInteractingFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getEtaPhiFilter",                        "ISF_EtaPhiFilter")
## Truth Strategies
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupID",                "ISF_MCTruthStrategyGroupID")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupID_MC15",           "ISF_MCTruthStrategyGroupID_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupIDHadInt",          "ISF_MCTruthStrategyGroupIDHadInt")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupIDHadInt_MC15",     "ISF_MCTruthStrategyGroupIDHadInt_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupCaloMuBrem",        "ISF_MCTruthStrategyGroupCaloMuBrem")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupCaloMuBrem_MC15",   "ISF_MCTruthStrategyGroupCaloMuBrem_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupCaloDecay",         "ISF_MCTruthStrategyGroupCaloDecay")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getTruthStrategyGroupCaloDecay_MC15",    "ISF_MCTruthStrategyGroupCaloDecay_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getValidationTruthStrategy",             "ISF_ValidationTruthStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getLLPTruthStrategy",                    "ISF_LLPTruthStrategy")
# Truth Strategy Modifiers
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getKeepLLPDecayChildrenStrategy",        "ISF_KeepLLPDecayChildrenStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getKeepLLPHadronicInteractionChildrenStrategy", "ISF_KeepLLPHadronicInteractionChildrenStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getKeepAllDecayChildrenStrategy",        "ISF_KeepAllDecayChildrenStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfigLegacy.getKeepHadronicInteractionChildrenStrategy", "ISF_KeepHadronicInteractionChildrenStrategy")
