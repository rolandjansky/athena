from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
from BTagging.BTaggingFlags import BTaggingFlags
from JetRec.JetRecFlags import jetFlags
from RecExConfig.RecFlags import rec

BTaggingFlags.Jets = []
#BTaggingFlags.Jets += ['AntiKt4TopoEMJets','AntiKt6TopoEMJets', 
#                       'AntiKt4LCTopoJets','AntiKt6LCTopoJets'] # for the default jets
if rec.doTruth():
    BTaggingFlags.Jets += ['AntiKt4Truth']
if rec.doTruth() and QcdD3PDMakerFlags.doRecTruthJet():
    BTaggingFlags.Jets += ['AntiKt4TruthNew','AntiKt5TruthNew','AntiKt6TruthNew',
                           'AntiKt4TruthWithNoInt', 'AntiKt6TruthWithNoInt',
                           'AntiKt4TruthWithMuNoInt', 'AntiKt6TruthWithMuNoInt']
if QcdD3PDMakerFlags.doRecJet() and QcdD3PDMakerFlags.doTopoEMJet():
    BTaggingFlags.Jets += ['AntiKt4TopoEMNew','AntiKt6TopoEMNew']
if QcdD3PDMakerFlags.doRecJet() and QcdD3PDMakerFlags.doLCTopoJet():
    BTaggingFlags.Jets += ['AntiKt4LCTopoNew','AntiKt5LCTopoNew','AntiKt6LCTopoNew']

BTaggingFlags.JetsWithInfoPlus = BTaggingFlags.Jets[:]
#BTaggingFlags.CalibrationTag="BTagCalibALL-07-02" # up to now it is not linked to the global flag


BTaggingFlags.CalibrationChannelAliases += [
    'AntiKt4Truth->AntiKt4TopoEM',
    'AntiKt4TruthNew->AntiKt4TopoEM',
    'AntiKt5TruthNew->AntiKt4TopoEM',
    'AntiKt6TruthNew->AntiKt6TopoEM',
    'AntiKt4TruthWithNoInt->AntiKt4TopoEM',
    'AntiKt6TruthWithNoInt->AntiKt6TopoEM',
    'AntiKt4TruthWithMuNoInt->AntiKt4TopoEM',
    'AntiKt6TruthWithMuNoInt->AntiKt6TopoEM',
    'AntiKt4TopoEMNew->AntiKt4TopoEM',
    'AntiKt6TopoEMNew->AntiKt6TopoEM',
    'AntiKt4LCTopoNew->AntiKt4LCTopo',
    'AntiKt6LCTopoNew->AntiKt6LCTopo',
    'AntiKt5LCTopoNew->AntiKt4LCTopo',]

#jetFlags.noStandardConfig = True
jetFlags.doBTagging     = True
BTaggingFlags.Active    = True        # default
BTaggingFlags.Runmodus  = "analysis"  # default
BTaggingFlags.JetFitterCharm=True
#BTaggingFlags.MV3_bVSu=True
#BTaggingFlags.MV3_bVSc=True
#BTaggingFlags.MV3_cVSu=True
include( "BTagging/BTagging_LoadTools.py" )
#include( "BTagging/BTagging_jobOptions.py" )

from JetTagD3PDMaker.JetTagD3PDMakerFlags               import JetTagD3PDFlags
#JetTagD3PDFlags.Taggers+=["JetFitterCharm","MV3_bVSu","MV3_bVSc","MV3_cVSu"]
JetTagD3PDFlags.Taggers+=["JetFitterCharm"]
JetTagD3PDFlags.JetFitterCharmTagInfo=True
