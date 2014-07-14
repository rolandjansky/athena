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
#from QcdD3PDMaker.CommonD3PDMakerFlags import CommonD3PDMakerFlags
from D3PDMakerConfig.CommonD3PDMakerFlags import CommonD3PDMakerFlags
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

# ----------------------------------
# Rho variables needed for Area offset substraction
from EventShapeRec.EventEtDensityConfig import calculateSimpleEventDensity
calculateSimpleEventDensity(Radius=0.4, SignalState="UNCALIBRATED")
calculateSimpleEventDensity(Radius=0.4, SignalState="CALIBRATED")
calculateSimpleEventDensity(Radius=0.6, SignalState="UNCALIBRATED")
calculateSimpleEventDensity(Radius=0.6, SignalState="CALIBRATED")
# ----------------------------------

# ----------------------------------
# import latest calibration tools 
from ApplyJetCalibration.SetupAthenaCalibration import doApplyJES
# ----------------------------------

# ----------------------------------
# Schedule jets grooming here, after rho variables.
from D3PDMakerConfig.CommonJSjets import createJSJets, getGroomingTools

from D3PDMakerConfig.CommonGroomedJetsConfig import getGroomedJetsConfig
dictsConfig = getGroomedJetsConfig()

groomedJetKeys = []

for dC in dictsConfig:
       xx = createJSJets(dC[0], dC[1])
       groomedJetKeys += [xx]
# ----------------------------------

#Truth
if rec.doTruth() and CommonD3PDMakerFlags.doRecTruthJet():
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt4TruthNewJets')
  #make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt6TruthNewJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithNuJets')
  #make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithNuJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt4TruthWithMuNuJets')
  #make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt6TruthWithMuNuJets')
  make_StandardJetGetter('AntiKt',0.4,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt4TruthWithMuJets')
  #make_StandardJetGetter('AntiKt',0.6,'Truth', doEMCut=False, minPt=4*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt6TruthWithMuJets')

  make_StandardJetGetter('AntiKt',0.5,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt5TruthNewJets')
  #make_StandardJetGetter('AntiKt',0.7,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt7TruthNewJets')
  make_StandardJetGetter('AntiKt',0.5,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt5TruthWithNuJets')
  #make_StandardJetGetter('AntiKt',0.7,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt7TruthWithNuJets')
  make_StandardJetGetter('AntiKt',0.5,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt5TruthWithMuNuJets')
  #make_StandardJetGetter('AntiKt',0.7,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt7TruthWithMuNuJets')
  make_StandardJetGetter('AntiKt',0.5,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt5TruthWithMuJets')
  #make_StandardJetGetter('AntiKt',0.7,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt7TruthWithMuJets')


  make_StandardJetGetter('AntiKt',0.2,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt2TruthNewJets')
  make_StandardJetGetter('AntiKt',0.3,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False, outputCollectionName='AntiKt3TruthNewJets')
  make_StandardJetGetter('AntiKt',0.2,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt2TruthWithNuJets')
  make_StandardJetGetter('AntiKt',0.3,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=False,useInteractingOnly=False, outputCollectionName='AntiKt3TruthWithNuJets')
  make_StandardJetGetter('AntiKt',0.2,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt2TruthWithMuNuJets')
  make_StandardJetGetter('AntiKt',0.3,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=False, outputCollectionName='AntiKt3TruthWithMuNuJets')
  make_StandardJetGetter('AntiKt',0.2,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt2TruthWithMuJets')
  make_StandardJetGetter('AntiKt',0.3,'Truth', doEMCut=False, minPt=10*GeV, addJetQualityMoments=False,includeMuons=True,useInteractingOnly=True, outputCollectionName='AntiKt3TruthWithMuJets')

  # truth WZ jets
  from TruthD3PDMaker.TruthJetFilterConfig             import TruthJetFilterConfig
  TruthJetFilterConfig (topSequence, writePartons = False, writeHadrons = True, excludeWZdecays = True, photonCone=0.1)
  from ParticleBuilderOptions.AODFlags import AODFlags
  AODFlags.MissingEtTruth = True
  AODFlags.TruthParticleJet = True
  AODFlags.McEventKey="GEN_EVENT"
  antikt4truthAlgWZ = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False,
                                            inputCollectionNames=['FilteredD3PDTruth'],
                                            outputCollectionName='AntiKt4TruthJets_WZ',
                                            useInteractingOnly=False,
                                            includeMuons=True
                                            ).jetAlgorithmHandle()


#Truth pile-up jets
if rec.doTruth() and CommonD3PDMakerFlags.doTruthPileupJet():
  pass


if CommonD3PDMakerFlags.doRecJet():
  #TrackJets
  if CommonD3PDMakerFlags.doTrackJet():   
    include("D3PDMakerConfig/CommonJetTrackZClusterTool_jobOptions.py")

  if CommonD3PDMakerFlags.doTopoEMJet():
    #TopoJets (EM+JES)
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:ApplyAreaOffsetJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMNewJets')
    #make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='EM:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMNewJets')

  if CommonD3PDMakerFlags.doTopoEMLowPtJet(): # low pT jets, currently off
    make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='EM:OFFSET_ORIGIN_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMLowPtJets')
    #make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='EM:OFFSET_ORIGIN_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMLowPtJets')
  
  if not rec.readAOD():
    if CommonD3PDMakerFlags.doTowerEMJet():   # not default
      #TopoTowerJets (EM+JES)
      make_StandardJetGetter('AntiKt',0.4,'TopoTower', doEMCut=False, minPt=4*GeV, doCalib=False, addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TowerEMNewJets')
      #make_StandardJetGetter('AntiKt',0.6,'TopoTower', doEMCut=False, minPt=4*GeV, doCalib=False, addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TowerEMNewJets')

    if CommonD3PDMakerFlags.doGhostJet():   # not default
      #GhostJets
      make_StandardJetGetter('AntiKt',0.4, 'Tower',  doEMCut=False, doCalib=False, outputCollectionName='AntiKt4GhostTowerNewJets', addJetQualityMoments=True, replaceNegEnergy=True)
      #make_StandardJetGetter('AntiKt',0.6, 'Tower',  doEMCut=False, doCalib=False, outputCollectionName='AntiKt6GhostTowerNewJets', addJetQualityMoments=True, replaceNegEnergy=True)

  if CommonD3PDMakerFlags.doLCTopoLowPtJet():
    make_StandardJetGetter('AntiKt',0.4,'LCTopo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4LCTopoLowPtJets')
    #make_StandardJetGetter('AntiKt',0.6,'LCTopo', doEMCut=False, minPt=4*GeV, maxPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6LCTopoLowPtJets')

  if CommonD3PDMakerFlags.doLCTopoJet():
    make_StandardJetGetter('AntiKt',0.2,'LCTopo', doEMCut=False, minPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt2LCTopoNewJets')
    make_StandardJetGetter('AntiKt',0.3,'LCTopo', doEMCut=False, minPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt3LCTopoNewJets')
    make_StandardJetGetter('AntiKt',0.4,'LCTopo', doEMCut=False, minPt=4*GeV, doCalib=True, calibName='LC:ApplyAreaOffsetJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4LCTopoNewJets')
    make_StandardJetGetter('AntiKt',0.5,'LCTopo', doEMCut=False, minPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt5LCTopoNewJets')
    #make_StandardJetGetter('AntiKt',0.6,'LCTopo', doEMCut=False, minPt=6*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6LCTopoNewJets')
    #make_StandardJetGetter('AntiKt',0.7,'LCTopo', doEMCut=False, minPt=10*GeV, doCalib=True, calibName='LC:ApplyAreaOffset', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt7LCTopoNewJets')

  if CommonD3PDMakerFlags.doLCTopoCBJet():   # not default
    make_StandardJetGetter('AntiKt', 0.4, 'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=4*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True, writeMoments=True,outputCollectionName='AntiKt4LCTopoCBJets')
    #make_StandardJetGetter('AntiKt', 0.6, 'LCTopo',precalibTools=[ConstMomTool],doEMCut=False,minPt=4*GeV,doCalib=True,calibName="LC:CONST",addJetQualityMoments=True, writeMoments=True,outputCollectionName='AntiKt6LCTopoCBJets')



from QCDTemplateTool.QCDTemplateToolConf import JetElectronSelector
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += JetElectronSelector( "JetElectronSelectorInstance90" )
topSequence.JetElectronSelectorInstance90.OutputLevel=5 #otherwise we get INFO: Execute() each for each event
topSequence.JetElectronSelectorInstance90.InputJetContainer="AntiKt4TopoEMNewJets"
topSequence.JetElectronSelectorInstance90.OutputJetElectronContainer="JetElectronContainer90"
topSequence.JetElectronSelectorInstance90.OutputJetMETContainer="JetMETContainer90"
topSequence.JetElectronSelectorInstance90.OutputJetContainer="JetMinusJetElectronContainer90"
topSequence.JetElectronSelectorInstance90.ExcludeCrackRegion=True
topSequence.JetElectronSelectorInstance90.CalibrationFactor=0.90


#copy cell based moments (copied from QcdD3PDMaker/share)


from JetMomentTools.JetCopyMomentsAlg import JetCopyMomentsAlg
topSequence+= JetCopyMomentsAlg('AntiKt4TopoEMJetsCoypMomAlg', JetCollectionSrc = 'AntiKt4TopoEMJets',JetCollectionDest = 'AntiKt4TopoEMNewJets',
                                Moments= [ 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                           'LArQuality','HECQuality',
                                           'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                           #'GSCFactorF','WidthFraction',
                                           'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
#topSequence+= JetCopyMomentsAlg('AntiKt6TopoEMJetsCoypMomAlg', JetCollectionSrc = 'AntiKt6TopoEMJets',JetCollectionDest = 'AntiKt6TopoEMNewJets',
#                                Moments= [ 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
#                                           'LArQuality','HECQuality',
#                                           'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
#                                           #'GSCFactorF','WidthFraction',
#                                           'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt4LCTopoJetsCoypMomAlg', JetCollectionSrc = 'AntiKt4LCTopoJets',JetCollectionDest = 'AntiKt4LCTopoNewJets',
                                Moments= [ 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
#topSequence+= JetCopyMomentsAlg('AntiKt6LCTopoJetsCoypMomAlg',JetCollectionSrc = 'AntiKt6LCTopoJets',JetCollectionDest = 'AntiKt6LCTopoNewJets',
#                                Moments= [ 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
#                                          'LArQuality','HECQuality',
#                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
#                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt5LCTopoJetsCoypMomAlg',JetCollectionSrc = 'AntiKt4LCTopoJets',JetCollectionDest = 'AntiKt5LCTopoNewJets',
                                Moments= [ 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])


#switch off in jet alg Btagging for selected jet collection. 
# Btagging for these algs will be made by a dedicated alg with more taggers
def removeBTagger(alg):
  del(alg.AlgTools['JetBTagger'])
removeBTagger(topSequence.AntiKt4TopoEMNewJets)
#removeBTagger(topSequence.AntiKt6TopoEMNewJets)
removeBTagger(topSequence.AntiKt4LCTopoNewJets)
#removeBTagger(topSequence.AntiKt6LCTopoNewJets)

# add origin recalculator for largeR jets
from JetMomentTools.JetMomentsConfigHelpers import specialMoment
specialMoment("AntiKt10LCTopoJets", ["origin"])
specialMoment("CamKt12LCTopoJets", ["origin"])

try:
  include( "MissingET/MissingET_jobOptions.py" )
except Exception:
  treatException("Could not set up MissingET. Switched off. makeCommonD3PD.jets !")
  recAlgs.doMissingET=False


