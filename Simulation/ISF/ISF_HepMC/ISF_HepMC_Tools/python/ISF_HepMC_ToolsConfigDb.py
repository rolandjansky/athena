# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
## GenParticle Filters
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticleFinalStateFilter",            "ISF_ParticleFinalStateFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticleSimWhiteList",                "ISF_ParticleSimWhiteList")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticleSimWhiteList_ExtraParticles", "ISF_ParticleSimWhiteList_ExtraParticles")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilterID",            "ISF_ParticlePositionFilterID")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilterCalo",          "ISF_ParticlePositionFilterCalo")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilterMS",            "ISF_ParticlePositionFilterMS")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilterWorld",         "ISF_ParticlePositionFilterWorld")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilterDynamic",       "ISF_ParticlePositionFilterDynamic")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getGenParticleInteractingFilter",        "ISF_GenParticleInteractingFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getEtaPhiFilter",                        "ISF_EtaPhiFilter")
## Truth Strategies
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupID",                "ISF_MCTruthStrategyGroupID")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupID_MC15",           "ISF_MCTruthStrategyGroupID_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupIDHadInt",          "ISF_MCTruthStrategyGroupIDHadInt")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupIDHadInt_MC15",     "ISF_MCTruthStrategyGroupIDHadInt_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloMuBrem",        "ISF_MCTruthStrategyGroupCaloMuBrem")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloMuBrem_MC15",   "ISF_MCTruthStrategyGroupCaloMuBrem_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloDecay",         "ISF_MCTruthStrategyGroupCaloDecay")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloDecay_MC15",    "ISF_MCTruthStrategyGroupCaloDecay_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getValidationTruthStrategy",             "ISF_ValidationTruthStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getLLPTruthStrategy",                    "ISF_LLPTruthStrategy")
# Truth Strategy Modifiers
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getKeepLLPDecayChildrenStrategy",        "ISF_KeepLLPDecayChildrenStrategy")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getKeepHadronicInteractionChildrenStrategy", "ISF_KeepHadronicInteractionChildrenStrategy")
