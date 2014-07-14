# Call as UserAlgs
# Run on ESD so that we can fully re-construct Jets from Towers

# Imports
from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetCalibTools.JetCalibToolsConf import *
from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.RecFlags import rec

from JetRec.JetRecFlags import jetFlags
jetFlags.inputFileType = inputFileType

make_StandardJetGetter('AntiKt', 0.6, 'H1Topo', outputCollectionName='AntiKt6H1TopoMyJets')
make_StandardJetGetter('AntiKt', 0.4, 'LCTopo')
make_StandardJetGetter('AntiKt', 0.6, 'LCTopo', outputCollectionName='AntiKt6LCTopoMyJets')


if rec.doTruth():
  make_StandardJetGetter('AntiKt', 0.6, 'Truth')

