from JetRec.JetMomentGetter import make_JetMomentGetter
from JetMomentTools.JetMomentToolsConf import JetMomentsFromCalib
from RecExConfig.RecFlags import rec
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags

mt = JetMomentsFromCalib()
from JetCalibTools.MakeCalibSequences import alternateCalibSequence
from JetRec.JetAlgConfiguration import checkAndUpdateOptions
#Request the GSC calib sequence, starting from the jet *final scale* (i.e. ETAJES for TopoEMJets)
d = checkAndUpdateOptions(input='Topo')
StructSeq = alternateCalibSequence("CAL:STRUCT1",d )
mt.Calibrator = StructSeq # link the calibrator to the moment tool.
# redefine StructSeq, and set options to save only the last
# 2 moments of the GSC sequence.
StructSeq = StructSeq.CalibToolSequence[-1]
StructSeq.MomentTag = "GSCFactor"
StructSeq.SimpleFactorCorrection = True
for t in StructSeq.CalibToolSequence[:-1]: #switch off all moments writing but the last.
    t.WriteMoments = False
    # schedule the alg computing jet moments :
make_JetMomentGetter("AntiKt4TopoEMJets", [ mt ] )
make_JetMomentGetter("AntiKt6TopoEMJets", [ mt ] )
if QcdD3PDMakerFlags.doRecJet():
    if QcdD3PDMakerFlags.doTopoEMJet():
        make_JetMomentGetter("AntiKt4TopoEMNewJets", [ mt ] )
        make_JetMomentGetter("AntiKt6TopoEMNewJets", [ mt ] )
    if QcdD3PDMakerFlags.doLCTopoJet():
        make_JetMomentGetter("AntiKt4LCTopoNewJets", [ mt ] )
        make_JetMomentGetter("AntiKt5LCTopoNewJets", [ mt ] )
        make_JetMomentGetter("AntiKt6LCTopoNewJets", [ mt ] )
