from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
from JetCalibTools.JetRecalibrator import makeJetRecalibrator
makeJetRecalibrator("AntiKt4TopoEMJets", "EM:OFFSET_ETAJES")
makeJetRecalibrator("AntiKt6TopoEMJets", "EM:OFFSET_ETAJES")
makeJetRecalibrator("AntiKt4LCTopoJets", "LC:OFFSET_ETAJES")
makeJetRecalibrator("AntiKt6LCTopoJets", "LC:OFFSET_ETAJES")
