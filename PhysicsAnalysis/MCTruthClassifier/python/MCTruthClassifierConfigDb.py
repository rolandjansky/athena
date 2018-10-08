# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("MCTruthClassifier.MCTruthClassifierConfig.getMCTruthClassifier", "MCTruthClassifier")
addTool("MCTruthClassifier.MCTruthClassifierConfig.getJetMCTruthClassifier", "JetMCTruthClassifier")
