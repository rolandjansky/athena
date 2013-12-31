# Imports
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetRec.JetRecFlags import jetFlags
from JetCalibTools.JetCalibToolsConf import *
from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.RecFlags import rec
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags

#set inputFileType
if rec.readAOD():
  jetFlags.inputFileType = 'AOD'

if QcdD3PDMakerFlags.doRecJet():
  #B-tag setting
  include("BTagging/BTagging_LoadTools.py")

  if QcdD3PDMakerFlags.doTopoJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
    #TopoJets
    make_StandardJetGetter('AntiKt',0.4,'H1Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='H1NumInv', addJetQualityMoments=True, outputCollectionName='AntiKt4H1TopoNewJets')
    make_StandardJetGetter('AntiKt',0.6,'H1Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='H1NumInv', addJetQualityMoments=True, outputCollectionName='AntiKt6H1TopoNewJets')

  if QcdD3PDMakerFlags.doTopoJet():
    #TopoJets (EM)
    make_StandardJetGetter('AntiKt',0.6,'H1Topo', minPt=4*GeV, calibTools=[], inputSuff="EMscale", outputCollectionName='AntiKt6H1TopoEMJets')

  if not rec.readAOD():
    if QcdD3PDMakerFlags.doTowerJet() and not QcdD3PDMakerFlags.doRecJetOnlyAdditional():
      #TopoTowerJets
      make_StandardJetGetter('AntiKt',0.4,'H1TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='H1NumInv', addJetQualityMoments=True, outputCollectionName='AntiKt4H1TowerNewJets', GCWScheme='Snowmass')
      make_StandardJetGetter('AntiKt',0.6,'H1TopoTower', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='H1NumInv', addJetQualityMoments=True, outputCollectionName='AntiKt6H1TowerNewJets', GCWScheme='Snowmass')

    if QcdD3PDMakerFlags.doGhostJet():
      #GhostJets
      make_StandardJetGetter('AntiKt', 0.4, 'H1Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt4H1TowerGhostJets')
      if primDPD.MakePrimaryDPD():
        make_StandardJetGetter('AntiKt', 0.4, 'H1Tower', GCWScheme='GhostTower', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt4H1TowerGhostTowerJets')
        make_StandardJetGetter('AntiKt', 0.4, 'H1Tower', GCWScheme='AbsoluteE', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt4H1TowerGhostAbsoluteEJets')
        make_StandardJetGetter('AntiKt', 0.4, 'H1Tower', GCWScheme='PosCells', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt4H1TowerGhostPosCellsJets')

      make_StandardJetGetter('AntiKt', 0.6, 'H1Tower', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt6H1TowerGhostJets')
      if primDPD.MakePrimaryDPD():
        make_StandardJetGetter('AntiKt', 0.6, 'H1Tower', GCWScheme='GhostTower', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt6H1TowerGhostTowerJets')
        make_StandardJetGetter('AntiKt', 0.6, 'H1Tower', GCWScheme='AbsoluteE', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt6H1TowerGhostAbsoluteEJets')
        make_StandardJetGetter('AntiKt', 0.6, 'H1Tower', GCWScheme='PosCells', doEMCut=False, minPt=7*GeV, replaceNegEnergy = True, addJetQualityMoments=True,  outputCollectionName = 'AntiKt6H1TowerGhostPosCellsJets')


  if QcdD3PDMakerFlags.doLCTopoJet():
    #LCTopoJets
    include('QcdD3PDMaker/LCTopoJets.py')

  #TrackJets
  #include('QcdD3PDMaker/TrackJet_jobOptions.py')

#Truth
if rec.doTruth() and QcdD3PDMakerFlags.doRecTruthJet():
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt4TruthNewJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False)

  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithNoIntJets')

  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithMuNoIntJets')
  make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=7*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithMuNoIntJets')
