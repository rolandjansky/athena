# Imports
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRec_defaults import *
from JetCalibTools.JetCalibToolsConf import *
from JetCalibTools.MakeCalibSequences import *
from JetCalibTools.SetupJetCalibrators import doJetOriginCorrection

from JetSubstructureD3PDMaker.JSjets import createJSJets
from JetSubstructureD3PDMaker.JSD3PD import JSD3PD

from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.RecFlags import rec
from RecExConfig.InputFilePeeker import inputFileSummary
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from IOVDbSvc.CondDB import conddb

#---
# Jet Width
WidthT = JetWidthTool("JetWidth")
#---

from JetMomentTools.JetMomentToolsConf import LowEtConstituentsFracTool
ConstMomTool = LowEtConstituentsFracTool("LowEtConstituentsFrac")

#set inputFileType
if rec.readAOD():
  jetFlags.inputFileType = 'AOD'


#Truth
if rec.doTruth() and QcdD3PDMakerFlags.doRecTruthJet():
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt4TruthNewJets')
  make_StandardJetGetter('AntiKt',0.5,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt5TruthNewJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt6TruthNewJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithNoIntJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithMuNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithMuNoIntJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt4TruthWithMuJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt6TruthWithMuJets')

#Truth pile-up jets
if rec.doTruth() and QcdD3PDMakerFlags.doTruthPileupJet():
  pass


if QcdD3PDMakerFlags.doRecJet():
  #TrackJets
  if QcdD3PDMakerFlags.doTrackJet():   # not default
    include('QcdD3PDMaker/TrackJet_jobOptions.py')

  if QcdD3PDMakerFlags.doTopoJet(): # off in default 
    #TopoJets (GCW+JES)
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:GCW_ETAJES', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoNewJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:GCW_ETAJES', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoNewJets')

  #if QcdD3PDMakerFlags.doTopoEMJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
  if QcdD3PDMakerFlags.doTopoEMJet():
    #TopoJets (EM+JES)
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMNewJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMNewJets')

  if QcdD3PDMakerFlags.doTopoEMLowPtJet(): # low pT jets
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='EM:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMLowPtJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='EM:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMLowPtJets')
  
  if not rec.readAOD():
    if QcdD3PDMakerFlags.doTowerEMJet():   # not default
      #TopoTowerJets (EM+JES)
      make_StandardJetGetter('AntiKt',0.4,'TopoTower', doEMCut=False, minPt=4*GeV, doCalib=False, addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TowerEMNewJets')
      make_StandardJetGetter('AntiKt',0.6,'TopoTower', doEMCut=False, minPt=4*GeV, doCalib=False, addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TowerEMNewJets')

    if QcdD3PDMakerFlags.doGhostJet():   # not default
      #GhostJets
      make_StandardJetGetter('AntiKt',0.4, 'Tower',  doEMCut=False, doCalib=False, outputCollectionName='AntiKt4GhostTowerNewJets', addJetQualityMoments=True, replaceNegEnergy=True)
      make_StandardJetGetter('AntiKt',0.6, 'Tower',  doEMCut=False, doCalib=False, outputCollectionName='AntiKt6GhostTowerNewJets', addJetQualityMoments=True, replaceNegEnergy=True)

  #if QcdD3PDMakerFlags.doLCTopoJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
  if QcdD3PDMakerFlags.doLCTopoJet():
    #LC
    make_StandardJetGetter('AntiKt',0.4,'LCTopo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='LC:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4LCTopoNewJets')
    make_StandardJetGetter('AntiKt',0.6,'LCTopo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='LC:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6LCTopoNewJets')

  if QcdD3PDMakerFlags.doLCTopoLowPtJet():
    make_StandardJetGetter('AntiKt',0.4,'LCTopo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='LC:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4LCTopoLowPtJets')
    make_StandardJetGetter('AntiKt',0.6,'LCTopo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='LC:OFFSET_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6LCTopoLowPtJets')

  if QcdD3PDMakerFlags.doLCTopoJet():
    make_StandardJetGetter('AntiKt',0.5,'LCTopo', doEMCut=False, minPt=4*GeV, doCalib=False, addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt5LCTopoNewJets')

  if QcdD3PDMakerFlags.doLCTopoCBJet():   # not default
    make_StandardJetGetter('AntiKt', 0.4, 'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=4*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True, writeMoments=True,outputCollectionName='AntiKt4LCTopoCBJets')
    make_StandardJetGetter('AntiKt', 0.6, 'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=4*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True, writeMoments=True,outputCollectionName='AntiKt6LCTopoCBJets')
