# Imports
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRec_defaults import *
from JetCalibTools.JetCalibToolsConf import *
from JetCalibTools.MakeCalibSequences import *
from JetCalibTools.SetupJetCalibrators import doJetOriginCorrection

from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.RecFlags import rec
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

if QcdD3PDMakerFlags.doRecJet():
  #B-tag setting
  from BTagging.BTaggingFlags import BTaggingFlags
  BTaggingFlags.Jets += ['AntiKt4TopoNew','AntiKt6TopoNew','AntiKt4TopoEMNew','AntiKt6TopoEMNew',
                         'AntiKt4TowerNew','AntiKt6TowerNew','AntiKt4TowerEMNew','AntiKt6TowerEMNew',
                         'AntiKt4LCTopoNew','AntiKt6LCTopoNew']

  BTaggingFlags.CalibrationChannelAliases += [
   'AntiKt4TopoNew->AntiKt4Topo,AntiKt4H1Topo',
   'AntiKt6TopoNew->AntiKt6Topo,AntiKt6H1Topo,AntiKt6H1Tower',
   'AntiKt4TopoEMNew->AntiKt4TopoEM,AntiKt4H1Topo',
   'AntiKt6TopoEMNew->AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower',
   'AntiKt4TowerNew->AntiKt4Tower,AntiKt4H1Tower',
   'AntiKt6TowerNew->AntiKt6Tower,AntiKt6H1Tower',
   'AntiKt4TowerEMNew->AntiKt4Tower,AntiKt4H1Tower',
   'AntiKt6TowerEMNew->AntiKt6Tower,AntiKt6H1Tower',
   'AntiKt4LCTopoNew->AntiKt4LCTopo',
   'AntiKt6LCTopoNew->AntiKt6LCTopo'
   ]

  #BTaggingFlags.JetFitterNN = False
  include("BTagging/BTagging_LoadTools.py")

  #
  if QcdD3PDMakerFlags.doTopoJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
    #TopoJets
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:GCW_ORIGIN', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoNewJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:GCW_ORIGIN', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoNewJets')
    #TopoJets (EM+JES)
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMNewJets')
    make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMNewJets')

  if not rec.readAOD():
    if QcdD3PDMakerFlags.doTowerJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
      #TopoTowerJets (GCW+JES)
      make_StandardJetGetter('AntiKt',0.4,'TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:GCW_ORIGIN', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TowerNewJets')
      make_StandardJetGetter('AntiKt',0.6,'TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:GCW_ORIGIN', GCWScheme='Snowmass', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TowerNewJets')
    if QcdD3PDMakerFlags.doTowerJet():   # not default
      #TopoTowerJets (EM+JES)
      make_StandardJetGetter('AntiKt',0.4,'TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TowerEMNewJets')
      make_StandardJetGetter('AntiKt',0.6,'TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TowerEMNewJets')

    if QcdD3PDMakerFlags.doGhostJet():   # not default
      #GhostJets
      make_StandardJetGetter('AntiKt', 0.4, 'Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy=True, calibName='EM:GCW', GCWScheme='GhostTower', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4GhostTowerGCWJets')
      make_StandardJetGetter('AntiKt', 0.6, 'Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy=True, calibName='EM:GCW', GCWScheme='GhostTower', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6GhostTowerGCWJets')
      make_StandardJetGetter('AntiKt', 0.4, 'Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy=True, calibName='EM', GCWScheme='GhostTower', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4GhostTowerEMJets')
      make_StandardJetGetter('AntiKt', 0.6, 'Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy=True, calibName='EM', GCWScheme='GhostTower', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6GhostTowerEMJets')

  if QcdD3PDMakerFlags.doLCTopoJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
    #LC
    make_StandardJetGetter('AntiKt',0.4,'LCTopo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='LC:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4LCTopoNewJets')
    make_StandardJetGetter('AntiKt',0.6,'LCTopo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='LC:ORIGIN', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6LCTopoNewJets')

  if QcdD3PDMakerFlags.doLCTopoJet():   # not default
    make_StandardJetGetter('AntiKt', 0.4,
                           'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=7*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True,
                           writeMoments=True,outputCollectionName='AntiKt4LCTopoCBJets')
    make_StandardJetGetter('AntiKt', 0.6,
                           'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=7*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True,
                           writeMoments=True,outputCollectionName='AntiKt6LCTopoCBJets')

  #TrackJets
  if QcdD3PDMakerFlags.doTrackJet():   # not default
    include('QcdD3PDMaker/TrackJet_jobOptions.py')

#Truth
if rec.doTruth() and QcdD3PDMakerFlags.doRecTruthJet():
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt4TruthNewJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt6TruthNewJets')

  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithNoIntJets')

  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithMuNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithMuNoIntJets')
