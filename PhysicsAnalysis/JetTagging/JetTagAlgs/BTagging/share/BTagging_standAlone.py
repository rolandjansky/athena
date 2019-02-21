# jobOption file to redo the b-tagging on an ESD, AOD or DPD file
# This jobO will: 
# - possibly re-reconstruct tracks
# - retag the existing Cone4Tower jet collection --> new collection Cone4TowerJetsAOD
# - build and b-tag a new Kt4LCTopo jet collection --> new collection Kt4LCTopoJets

# --- Define requested features:
IDSimulationOnly = False # for inner detector only simulation
WriteNewAODFile  = False # to write the results in a new AOD: experimental, not supported
UseGoodRunLBList = False # to run only on events in the GRL list (cf. end of this file)
ReDoTrkEgComMuon = False # to re-reconstruct: only for experts
# --- Define some tags: careful to choose the correct ones, in doubt leave empty
OverrideConditionsTag = ""
OverridePixMapOverlay = ""

# This is strange... until the getStandardBTaggerTool in the JetGetters has been modified

#myjets = [ 'AntiKt4Topo','AntiKt6Topo', 'AntiKtZ4Track' ]
myjets = [ 'AntiKtZ4Track' ]

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [ '/tmp/jdevivi/ESD.186186._001566.pool.root.1' ]
athenaCommonFlags.EvtMax.set_Value_and_Lock(5)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)

from AthenaCommon.GlobalFlags import globalflags
if OverrideConditionsTag != "":
  print '#BTAG# Overriding conditions tag with ', OverrideConditionsTag
  globalflags.ConditionsTag.set_Value_and_Lock(OverrideConditionsTag)

# --- Re-reconstruction steering flags: do not change if not an expert !
#rec.readAOD.set_Value_and_Lock(True)
rec.doTrigger.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(ReDoTrkEgComMuon and rec.readESD())
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doDumpTES.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)

if rec.doESD():
  rec.Commissioning=True
  rec.doMonitoring=False
  # Since I put doESD at True to allow for ID reco, I have to swich off many things
  # High level objects
  recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
  recAlgs.doMuonIDCombined.set_Value_and_Lock(False)
  recAlgs.doMuGirl.set_Value_and_Lock(False)
  recAlgs.doTileMuID.set_Value_and_Lock(False)
  recAlgs.doMuonSpShower.set_Value_and_Lock(False)
  recAlgs.doCaloTrkMuId.set_Value_and_Lock(False)
  recAlgs.doMissingET.set_Value_and_Lock(False)
  recAlgs.doTrigger.set_Value_and_Lock(False)
  rec.doTau.set_Value_and_Lock(False)
  # However, we redo tracking, so egamma and Combined muons have to be run again
  # to match to the new TrackParticleCandidate container. (I redo only Staco/MuTag)
  rec.doEgamma.set_Value_and_Lock(True)
  rec.doMuonCombined.set_Value_and_Lock(True)
  recAlgs.doStaco.set_Value_and_Lock(True)
  recAlgs.doMuTag.set_Value_and_Lock(True)
  # Calo Clusters etc... objects
  from CaloRec.CaloRecFlags import jobproperties as cal
  cal.CaloRecFlags.Enabled.set_Value_and_Lock(False)
  # Muon basic objects
  from MuonRecExample.MuonRecFlags import muonRecFlags
  muonRecFlags.doMoore.set_Value_and_Lock(False)
  muonRecFlags.doMuonboy.set_Value_and_Lock(False)
  # Configuration for re-running the ID
  from InDetRecExample.InDetJobProperties import InDetFlags
  #InDetFlags.primaryVertexSetup    = 'DefaultFullFinding'
  #InDetFlags.useBeamConstraint     = False 
  #InDetFlags.primaryVertexCutSetup = "StartUp"
  InDetFlags.doLowPt               = True
  #InDetFlags.doMinBias             = True
  InDetFlags.doSlimming            = False #if true, pb in 15.6.7... to be undestood

# --- jet builder configuration:
# specific configuration for fixes from Jet group
from JetRec.JetRecFlags import jetFlags
jetFlags.noStandardConfig = True
jetFlags.doJVF            = False
jetFlags.doBTagging       = True
jetFlags.inputFileType    = "AOD"
if rec.readESD():
  jetFlags.inputFileType  = "ESD"
jetFlags.Enabled          = False

# the main jobOpt
include("RecExCommon/RecExCommon_topOptions.py")


# --- b-tagging configuration:
if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.Runmodus                   = "analysis"
BTaggingFlags.Jets                       = myjets[:]
BTaggingFlags.JetsWithInfoPlus           = myjets[:]
BTaggingFlags.OutputLevel                = INFO
BTaggingFlags.doJetTagNtuple             = True
if globalflags.DataSource() == "data":
  BTaggingFlags.JetTagNtupleName           = "JetTagNtuple.dataFULL.root"
  BTaggingFlags.JetTagSlimNtupleName       = "JetTagNtuple.dataSLIM.root"
else:
  BTaggingFlags.JetTagNtupleName           = "JetTagNtuple.simuFULL.root"
  BTaggingFlags.JetTagSlimNtupleName       = "JetTagNtuple.simuSLIM.root"

BTaggingFlags.JetsForNtuple              = [ 'AntiKt4TopoJets', 'AntiKtZ4TrackJets' ]
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4H1NewTopo->Cone4H1Tower", "AntiKt4Topo->Cone4H1Tower" ]
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt6H1NewTopo->Cone4H1Tower", "AntiKt6Topo->Cone4H1Tower" ]
BTaggingFlags.CalibrationChannelAliases += [ "AntiKtZ4Track->Cone4H1Tower", "AntiKtZ4Track->Cone4H1Tower" ]

if IDSimulationOnly:
  BTaggingFlags.SoftMu = False
include( "BTagging/BTagging_LoadTools.py" )

# -- if TrackJets are to be b-tagged, build them:
if 'AntiKtZ4Track' in BTaggingFlags.Jets:
  print '#BTAG# AntiKtZ4TrackJets to be tagged ! Creating them now...'
  include( "BTagging/BTagging_BuildAntiKtZ4TrackJets.py" )
  ##JBdV test
  BTaggingFlags.RetagJets += [ 'AntiKtZ4Track' ]
  

# --- jet builder configuration : these are the "standard jets" to start with (cut at EM scale+quality moments)

#from JetRec.JetGetters import *
#from AthenaCommon.SystemOfUnits import GeV
#make_StandardJetGetter('AntiKt', 0.4, 'Topo', disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt4H1NewTopoJets', doEMCut=True, minPt=4*GeV, addJetQualityMoments=True)
#make_StandardJetGetter('AntiKt', 0.6, 'Topo', disable=False, doCalib=True, calibName='H1NumInv', outputCollectionName='AntiKt6H1NewTopoJets', doEMCut=True, minPt=4*GeV, addJetQualityMoments=True)

include( "BTagging/BTagging_jobOptions.py" )
#topSequence.JTNtBuilder.JetCollectionList 

#Add the moments to the already existing collection (the BADCHANNEL ones are not there...)

#from JetRec.JetMomentGetter import make_JetMomentGetter
#from JetRecTools.JetRecToolsConf import JetCaloQualityTool
#cqual_tool = JetCaloQualityTool(
#  name                     = "CaloQualTool",
#  doSamplingBasedVariables    = True,
#  doCellBasedVariables        = True,
#  doConstituentBasedVariables = True
#  )
#make_JetMomentGetter("AntiKt4TopoJetsAOD", [cqual_tool] )

#ReOrder the sequence, so that the JTNtBuilder is run after
# the algorithm that attached the moments to the already existing collections
if BTaggingFlags.doJetTagNtuple:
  theApp.TopAlg = [ e for e in theApp.TopAlg if e not in ['JetTagNtupleBuilder/JTNtBuilder'] ]
  theApp.TopAlg += ['JetTagNtupleBuilder/JTNtBuilder']
if BTaggingFlags.doJetTagSlimNtuple:
  theApp.TopAlg = [ e for e in theApp.TopAlg if e not in ['JetTagNtupleBuilder/JTNtSlimBuilder'] ]
  theApp.TopAlg += ['JetTagNtupleBuilder/JTNtSlimBuilder']

# override DB: to have the worst case for pixel dead modules
if OverridePixMapOverlay != "":
  print '#BTAG# Overriding PixMapOverlay with ',OverridePixMapOverlay
  conddb.addOverride('/PIXEL/PixMapOverlay', OverridePixMapOverlay)

# --- output a new AOD file if requested (not fully validated ! strange behavior for re-tagged collections !)
if WriteNewAODFile:
  from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
  StreamAOD = AthenaPoolOutputStream( "StreamAOD", "newAOD.pool.root", True )
  StreamAOD.ForceRead = True 
  StreamAOD.ExtendProvenanceRecord = False 
  StreamAOD.TakeItemsFromInput=True
  taggedJetList = []
  for key in BTaggingFlags.Jets:
    if key in BTaggingFlags.RetagJets:
#      newJetColl = "JetCollection#"+key+"JetsAOD"
      newJetColl = "JetCollection#"+key+"JetsReTagged"
    else:
      newJetColl = "JetCollection#"+key+"Jets"
    print '#BTAG# jet collection: ',newJetColl
    taggedJetList +=  [ newJetColl ]
  StreamAOD.ItemList += taggedJetList
  print '#BTAG# writing out :',StreamAOD.ItemList

# --- Filter on GRL first:
## For data, use the Good Run / Lumi Block List
## use grlb algorithm for event selection.
if  globalflags.DataSource() == "data" and UseGoodRunLBList:
  # configure the Selector tool
  from GoodRunsLists.GoodRunsListsConf import *
  ToolSvc += GoodRunsListSelectorTool(
    name            = "GoodRunsListSelectorTool",
    GoodRunsListVec = [ 'CollisionsNoTorReq.xml' ],
    OutputLevel     = 4)
  # add the filter sequence with its Selector alg (which uses the Selector tool).
  # It is run before the topSequence
  from AthenaCommon.AlgSequence import AthSequencer
  seq = AthSequencer("AthMasterSeq")
  from GoodRunsListsUser.GoodRunsListsUserConf import *
  seq += GRLTriggerSelectorAlg('GRLTriggerAlg')


print "List of all top algorithms"
print theApp.TopAlg
print topSequence
#ServiceMgr.StoreGateSvc.Dump=True
