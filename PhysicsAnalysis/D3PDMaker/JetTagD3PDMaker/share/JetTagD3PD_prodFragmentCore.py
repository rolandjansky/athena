## main BTAGD3PD job option fragment for reco trf

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from JetTagD3PDMaker.JetTagGhostAssocHelper import addToIncludeBlocks


if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags

if not 'JetTagD3PD_JetFitterCharm' in dir():
  JetTagD3PD_JetFitterCharm=False

if not 'JetTagD3PD_MV3' in dir():
  JetTagD3PD_MV3=False

if not 'JetTagD3PD_SecondSoftMu' in dir():
  JetTagD3PD_SecondSoftMu=True

if not 'JetTagD3PD_redoTagging' in dir():
  JetTagD3PD_redoTagging=False

if not 'JetTagD3PD_slimD3PD' in dir():
  JetTagD3PD_slimD3PD=True

if not 'JetTagD3PD_calibTaggers' in dir():
  JetTagD3PD_calibTaggers=True

if not 'JetTagD3PD_doTrigNavigSlim' in dir():
  JetTagD3PD_doTrigNavigSlim=False

if not 'JetTagD3PD_flavor' in dir():
  JetTagD3PD_flavor="Full"

if not 'JetTagD3PD_rerunJVF' in dir():
  JetTagD3PD_rerunJVF=False

if not 'JetTagD3PD_runJVF' in dir():
  JetTagD3PD_runJVF=JetTagD3PD_rerunJVF

if not 'JetTagD3PD_addGhostTrackAssociation' in dir():
  JetTagD3PD_addGhostTrackAssociation=False

if not 'JetTagD3PD_addGhostBHadronAssociation' in dir():
  JetTagD3PD_addGhostBHadronAssociation=False

if not 'JetTagD3PD_addGhostTruthAssociation' in dir():
  JetTagD3PD_addGhostTruthAssociation=False


## set to 'AOD' for rel 17.0 or earlier
if not 'JetTagD3PD_CollectionPostfix' in dir():
  JetTagD3PD_CollectionPostfix="ReTagged"

### jet collection
if not 'JetTagD3PD_AntiKt4TopoEM' in dir():
  JetTagD3PD_AntiKt4TopoEM=True
  
if not 'JetTagD3PD_AntiKt6TopoEM' in dir():
  JetTagD3PD_AntiKt6TopoEM=True

if not 'JetTagD3PD_AntiKt4LCTopo' in dir():
  JetTagD3PD_AntiKt4LCTopo=True

if not 'JetTagD3PD_AntiKt6LCTopo' in dir():
  JetTagD3PD_AntiKt6LCTopo=True

if not 'JetTagD3PD_AntiKt4Truth' in dir():
  JetTagD3PD_AntiKt4Truth=True

if not 'JetTagD3PD_AntiKt4MuonNonInteractingTruth' in dir():
  JetTagD3PD_AntiKt4MuonNonInteractingTruth=True

if not 'JetTagD3PD_AntiKt6MuonNonInteractingTruth' in dir():
  JetTagD3PD_AntiKt6MuonNonInteractingTruth=True

if not 'JetTagD3PD_AntiKt4TruthPurePU' in dir():
  JetTagD3PD_AntiKt4TruthPurePU=False

if not 'JetTagD3PD_PurePUInput' in dir():
  JetTagD3PD_PurePUInput='INav4MomTruthEventINTIME'

if not 'JetTagD3PD_AntiKtZ4TrackJets' in dir():
  JetTagD3PD_AntiKtZ4TrackJets=True

if not 'JetTagD3PD_AddCAJets' in dir():
  JetTagD3PD_AddCAJets=True

if not 'JetTagD3PD_addMuonScatteringSignificance' in dir():
  JetTagD3PD_addMuonScatteringSignificance=False

if not rec.doTruth():
  JetTagD3PD_AntiKt4MuonNonInteractingTruth=False
  JetTagD3PD_AntiKt6MuonNonInteractingTruth=False
  JetTagD3PD_AntiKt4Truth=False
  JetTagD3PD_AntiKt4TruthPurePU=False

if not JetTagD3PD_redoTagging:
  JetTagD3PD_AntiKt4MuonNonInteractingTruth=False
  JetTagD3PD_AntiKt6MuonNonInteractingTruth=False
  JetTagD3PD_AntiKtZ4TrackJets=False
  JetTagD3PD_AntiKt4TruthPurePU=False


if JetTagD3PD_rerunJVF or JetTagD3PD_runJVF:
  from JetRec.JetMomentGetter import make_JetMomentGetter
  from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool

if JetTagD3PD_rerunJVF:
  # parameters are irrelevant, these will work for any jets
  # optional : rename the moment if you don't want to overwrite the existing one
  # jvatool.JVFMomentName = "JVF2"
  if JetTagD3PD_AntiKt4TopoEM:
    myjvatoolakt4em = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
    make_JetMomentGetter( 'AntiKt4TopoEMJets' , [myjvatoolakt4em] ) 

  if JetTagD3PD_AntiKt6TopoEM:
    myjvatoolakt6em = getJetVertexAssociationTool('AntiKt', 0.6, 'Topo') 
    make_JetMomentGetter( 'AntiKt6TopoEMJets' , [myjvatoolakt6em] ) 

  if JetTagD3PD_AntiKt4LCTopo:
    myjvatoolakt4lc = getJetVertexAssociationTool('AntiKt', 0.4, 'LCTopo')
    make_JetMomentGetter( 'AntiKt4LCTopoJets' , [myjvatoolakt4lc] ) 

  if JetTagD3PD_AntiKt6LCTopo:
    myjvatoolakt6lc = getJetVertexAssociationTool('AntiKt', 0.6, 'LCTopo')  
    make_JetMomentGetter( 'AntiKt6LCTopoJets' , [myjvatoolakt6lc] ) 



if JetTagD3PD_runJVF:
  if rec.doTruth() :
    if JetTagD3PD_AntiKt4Truth:
      myjvatoolakt4t = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
      make_JetMomentGetter( 'AntiKt4TruthJets' , [myjvatoolakt4t] ) 


BTaggingFlags.Jets=[ "AntiKt4Truth","AntiKt4TruthPurePU",
                     "AntiKt4MuonNonInteractingTruth","AntiKt6MuonNonInteractingTruth", 
                     "AntiKtZ4Track"]


if JetTagD3PD_AntiKt4TopoEM:
  BTaggingFlags.Jets+=["AntiKt4TopoEM"]
if JetTagD3PD_AntiKt6TopoEM:
  BTaggingFlags.Jets+=["AntiKt6TopoEM"]
if JetTagD3PD_AntiKt4LCTopo:
  BTaggingFlags.Jets+=["AntiKt4LCTopo"]
if JetTagD3PD_AntiKt6LCTopo: 
  BTaggingFlags.Jets+=["AntiKt6LCTopo"]

BTaggingFlags.JetsWithInfoPlus=[ "AntiKt4TopoEM", "AntiKt6TopoEM",
                                 "AntiKt4LCTopo", "AntiKt6LCTopo",
                                 "AntiKt4Truth","AntiKt4TruthPurePU",
                                 "AntiKt4MuonNonInteractingTruth","AntiKt6MuonNonInteractingTruth",
                                 "AntiKtZ4Track" ]
  
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4Truth->AntiKt4TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4TruthPurePU->AntiKt4TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4MuonNonInteractingTruth->AntiKt4TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt6MuonNonInteractingTruth->AntiKt6TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKtZ4Track->AntiKt4TopoEM" ]


if JetTagD3PD_AddCAJets:
  include("JetTagD3PDMaker/JetTagD3PD_configFatJets.py")

if JetTagD3PD_redoTagging:
  from JetRec.JetRecFlags import jetFlags
  jetFlags.noStandardConfig = True
  jetFlags.doBTagging       = True

  BTaggingFlags.Active=True
  BTaggingFlags.Runmodus  = "analysis"

  BTaggingFlags.doJetTagNtuple = False
  BTaggingFlags.doJetTagSlimNtuple=False
  BTaggingFlags.doJetTagFakeRateNtuple=False
  BTaggingFlags.doJetTagEfficiencyNtuple=False
  BTaggingFlags.SoftMuChi2=True
  BTaggingFlags.SoftMu=True
  BTaggingFlags.NewGbbNNTag=True
  BTaggingFlags.JetFitterCharm=JetTagD3PD_JetFitterCharm
  BTaggingFlags.JetFitterCharmNeg=JetTagD3PD_JetFitterCharm
  BTaggingFlags.MV3_bVSu=JetTagD3PD_MV3
  BTaggingFlags.MV3_bVSc=JetTagD3PD_MV3
  BTaggingFlags.MV3_cVSu=JetTagD3PD_MV3
  BTaggingFlags.MV3_bVSuFlip=JetTagD3PD_MV3
  BTaggingFlags.MV3_bVScFlip=JetTagD3PD_MV3
  BTaggingFlags.MV3_cVSuFlip=JetTagD3PD_MV3
  BTaggingFlags.GbbNNTag=False
  BTaggingFlags.QGTag=True
  BTaggingFlags.writeSecondaryVertices=True
  BTaggingFlags.SecondSoftMu=JetTagD3PD_SecondSoftMu
  BTaggingFlags.SecondSoftMuChi2=JetTagD3PD_SecondSoftMu
  BTaggingFlags.IP1D=False ### no calibrations
  BTaggingFlags.IP2DFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DPos=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DNeg=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DSpc=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DSpcFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DSpcPos=JetTagD3PD_calibTaggers
  BTaggingFlags.IP2DSpcNeg=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DPos=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DNeg=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DSpc=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DSpcFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DSpcPos=JetTagD3PD_calibTaggers
  BTaggingFlags.IP3DSpcNeg=JetTagD3PD_calibTaggers
  BTaggingFlags.SV1Flip =JetTagD3PD_calibTaggers
  BTaggingFlags.SV2Flip=JetTagD3PD_calibTaggers
  BTaggingFlags.JetProbFlip=False
  BTaggingFlags.SoftEl=False ## no calib
  BTaggingFlags.JetFitterNNFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.TrackCounting=JetTagD3PD_calibTaggers
  BTaggingFlags.TrackCountingFlip=JetTagD3PD_calibTaggers
  BTaggingFlags.MV1Flip=JetTagD3PD_calibTaggers
  BTaggingFlags.MV2Flip=JetTagD3PD_calibTaggers
  BTaggingFlags.JetProb=False

  include( "BTagging/BTagging_LoadTools.py" )

  ### stop btag and create the new jet collection 
  ###(in the case of new taggers with new code not available in the jet code)
  jetFlags.doBTagging       = False
  if JetTagD3PD_AntiKt4MuonNonInteractingTruth: 
    from JetRec.JetGetters import make_StandardJetGetter 
    make_StandardJetGetter('AntiKt',0.4,'Truth', 
                           disable=False, includeMuons = True, 
                           useInteractingOnly=False, 
                           outputCollectionName='AntiKt4MuonNonInteractingTruthJets')
    if JetTagD3PD_runJVF:
      myjvatoolakt4tmn = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
      make_JetMomentGetter( 'AntiKt4MuonNonInteractingTruthJets' , [myjvatoolakt4tmn]) 


  if JetTagD3PD_AntiKt6MuonNonInteractingTruth: 
    from JetRec.JetGetters import make_StandardJetGetter 
    make_StandardJetGetter('AntiKt',0.6,'Truth', 
                           disable=False, includeMuons = True, 
                           useInteractingOnly=False, 
                           outputCollectionName='AntiKt6MuonNonInteractingTruthJets')
    if JetTagD3PD_runJVF:
      myjvatoolakt6tmn = getJetVertexAssociationTool('AntiKt', 0.6, 'Topo')
      make_JetMomentGetter( 'AntiKt6MuonNonInteractingTruthJets' , [myjvatoolakt6tmn] )

  if JetTagD3PD_AntiKt4TruthPurePU: 
    from JetRec.JetGetters import make_StandardJetGetter 
    putjalg = make_StandardJetGetter('AntiKt',0.4,'Truth', 
                                     disable=False, includeMuons = True, 
                                     doTruthPileup=True,
                                     outputCollectionName='AntiKt4TruthPurePUJets').jetAlgorithmHandle()
    itool = putjalg.AlgTools[0]
    itool.InputCollectionKeys = [JetTagD3PD_PurePUInput]
    if JetTagD3PD_runJVF:
      myjvatoolakt4put = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
      make_JetMomentGetter( 'AntiKt4TruthPurePUJets' , [myjvatoolakt4put] ) 


  if JetTagD3PD_AntiKtZ4TrackJets:
    include("JetTagD3PDMaker/JetTagD3PD_makeTrackJets.py")
    if JetTagD3PD_runJVF:
      myjvatoolakt4tr = getJetVertexAssociationTool('AntiKt', 0.4, 'Track')
      make_JetMomentGetter( 'AntiKtZ4TrackJets' , [myjvatoolakt4tr] )

  jetFlags.doBTagging       = True

 #### the rerun btagging using the btag code
  if JetTagD3PD_AntiKt4MuonNonInteractingTruth: 
    BTaggingFlags.RetagJets += ["AntiKt4MuonNonInteractingTruth"]
  if JetTagD3PD_AntiKt6MuonNonInteractingTruth:
    BTaggingFlags.RetagJets += ["AntiKt6MuonNonInteractingTruth"] 
  if JetTagD3PD_AntiKt4TruthPurePU: 
    BTaggingFlags.RetagJets += ["AntiKt4TruthPurePU"]
  if JetTagD3PD_AntiKtZ4TrackJets:
    BTaggingFlags.RetagJets += ["AntiKtZ4Track"]

  include( "BTagging/BTagging_jobOptions.py" )
else:
  BTaggingFlags.Active=False
  JetTagD3PD_AntiKt4Truth=False


if JetTagD3PD_AddCAJets:
  JetTagD3PDFlags.AddFatJets=True
  include( "BTagging/BTagging_LoadTools.py" )
  from JetTagD3PDMaker.JetTagJSMaker import jtcreateJSJets
  from AthenaCommon.AppMgr import topSequence
  myJetKeys = []
  for dC in dictsConfig:
    xx = jtcreateJSJets(dC[0], dC[1], topSequence)
    myJetKeys += [xx]
  JetTagD3PDKeys.FatJetsList=myJetKeys


from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

jettagd3pd_jetnamepostfix="" 
if JetTagD3PD_redoTagging:
  jettagd3pd_jetnamepostfix=JetTagD3PD_CollectionPostfix


if JetTagD3PD_AntiKt4TopoEM:
  JetTagD3PDFlags.JetCollections+=['AntiKt4TopoEMJets'+jettagd3pd_jetnamepostfix]
  addToIncludeBlocks('AntiKt4TopoEMJets'+jettagd3pd_jetnamepostfix,['ConstituentScale','TracksMoments'])
if JetTagD3PD_AntiKt6TopoEM:
  JetTagD3PDFlags.JetCollections+=['AntiKt6TopoEMJets'+jettagd3pd_jetnamepostfix]
  addToIncludeBlocks('AntiKt6TopoEMJets'+jettagd3pd_jetnamepostfix,['ConstituentScale','TracksMoments'])
if JetTagD3PD_AntiKt4LCTopo:
  JetTagD3PDFlags.JetCollections+=['AntiKt4LCTopoJets'+jettagd3pd_jetnamepostfix]
  addToIncludeBlocks('AntiKt4LCTopoJets'+jettagd3pd_jetnamepostfix,['ConstituentScale','TracksMoments'])
if JetTagD3PD_AntiKt6LCTopo: 
  JetTagD3PDFlags.JetCollections+=['AntiKt6LCTopoJets'+jettagd3pd_jetnamepostfix]
  addToIncludeBlocks('AntiKt6LCTopoJets'+jettagd3pd_jetnamepostfix,['ConstituentScale','TracksMoments'])



  ### configure btagging for additional jet collections
if JetTagD3PD_AntiKt4MuonNonInteractingTruth:
  JetTagD3PDFlags.JetCollections+=['AntiKt4MuonNonInteractingTruthJets'+jettagd3pd_jetnamepostfix]
  JetTagD3PDFlags.D3PDPropDict().update(
    {"AntiKt4MuonNonInteractingTruthJets"+jettagd3pd_jetnamepostfix+"_level":0})
  if JetTagD3PD_runJVF:
    JetTagD3PDFlags.D3PDPropDict().update(
      {"AntiKt4MuonNonInteractingTruthJets"+jettagd3pd_jetnamepostfix+"_include":["JVtx"]})

if JetTagD3PD_AntiKt6MuonNonInteractingTruth:
  JetTagD3PDFlags.JetCollections+=['AntiKt6MuonNonInteractingTruthJets'+jettagd3pd_jetnamepostfix]
  JetTagD3PDFlags.D3PDPropDict().update(
        {"AntiKt6MuonNonInteractingTruthJets"+jettagd3pd_jetnamepostfix+"_level":0})
  if JetTagD3PD_runJVF:
    JetTagD3PDFlags.D3PDPropDict().update(
      {"AntiKt6MuonNonInteractingTruthJets"+jettagd3pd_jetnamepostfix+"_include":["JVtx"]})

if JetTagD3PD_AntiKt4TruthPurePU:
  JetTagD3PDFlags.JetCollections+=['AntiKt4TruthPurePUJets'+jettagd3pd_jetnamepostfix]
  JetTagD3PDFlags.D3PDPropDict().update(
    {"AntiKt4TruthPurePUJets"+jettagd3pd_jetnamepostfix+"_level":0})
  if JetTagD3PD_runJVF:
    JetTagD3PDFlags.D3PDPropDict().update(
      {"AntiKt4TruthPurePUJets"+jettagd3pd_jetnamepostfix+"_include":["JVtx"]})


if JetTagD3PD_AntiKtZ4TrackJets:
  JetTagD3PDFlags.JetCollections+=['AntiKtZ4TrackJets'+jettagd3pd_jetnamepostfix]
  if JetTagD3PD_runJVF:
    JetTagD3PDFlags.D3PDPropDict().update(
      {"AntiKtZ4TrackJets"+jettagd3pd_jetnamepostfix+"_include":["JVtx"]})

if JetTagD3PD_AntiKt4Truth:
  JetTagD3PDFlags.JetCollections+=['AntiKt4TruthJets'+jettagd3pd_jetnamepostfix]
  JetTagD3PDFlags.D3PDPropDict().update(
    {"AntiKt4TruthJets"+jettagd3pd_jetnamepostfix+"_level":0})
  if JetTagD3PD_rerunJVF:
    JetTagD3PDFlags.D3PDPropDict().update(
      {"AntiKt4TruthJets"+jettagd3pd_jetnamepostfix+"_include":["JVtx"]})



from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

 
JetTagD3PDFlags.NtupleType = JetTagD3PD_flavor
### hack to make it work until remi fixes the label in BuildVertexD3PDObject
JetTagD3PDKeys.PrimaryVertexGetterLabel = JetTagD3PDKeys.PrimaryVertexPrefix()

JetTagD3PDFlags.AddSecondMuonCollection=BTaggingFlags.SecondSoftMu

### which ntuple configuration to load?
if JetTagD3PDFlags.NtupleType() == "":
  print "custom configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Debug":
  include("JetTagD3PDMaker/DebugNtuple_LoadConfig.py")
  print "Debug configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Full":
  include("JetTagD3PDMaker/FullNtuple_LoadConfig.py")
  print "Full configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Efficiency":
    include("JetTagD3PDMaker/EffNtuple_LoadConfig.py")
    print "Efficiency configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Slim":
    include("JetTagD3PDMaker/SlimNtuple_LoadConfig.py")
    print "Slim configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "FakeRate":
    include("JetTagD3PDMaker/FakeRateNtuple_LoadConfig.py")
    print "FakeRate configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Physics":
    include("JetTagD3PDMaker/PhysicsNtuple_LoadConfig.py")
    print "Physics configuration used for JetTagD3PD"


if rec.readAOD():
  JetTagD3PDFlags.AddPixelClusterObject=False

if rec.readAOD():
  JetTagD3PDFlags.AddPixCond=False

if JetTagD3PDFlags.AddPixCond():
  if not 'InDetPixelConditionsSummarySvc' in dir() and InDetRecTestBLayerTool==None:
    include("JetTagD3PDMaker/JetTagD3PD_LoadPixelCondServices.py")



if not JetTagD3PD_redoTagging or not JetTagD3PD_calibTaggers:
  JetTagD3PDFlags.InfoBaseCalib=False
  JetTagD3PDFlags.JetFitterTagFlipInfo=False
  JetTagD3PDFlags.JetFitterFlipVxOnJetAxisAssoc=False
  JetTagD3PDFlags.JetFitterFlipTwoTrackVertexAssoc=False
  JetTagD3PDFlags.Taggers = JetTagD3PDFlags.DefaultTaggers()
  JetTagD3PDFlags.JetFitterVertexFinderObjects = ['NewJetFitterVxFinder']
  JetTagD3PDFlags.AddElectronInJetsObject = False
  JetTagD3PDFlags.AddPhotonInJetsObject = False

if not JetTagD3PD_redoTagging:
  JetTagD3PDFlags.AddVxOnJetAxisInJetsObject = False
  JetTagD3PDFlags.AddTwoTrackVertexInJetsObject = False
  JetTagD3PDFlags.JetFitterVertexFinderInfo = False
  JetTagD3PDFlags.JetVKalVxBadTrack = False
  JetTagD3PDFlags.SoftMuonInfo=False
  JetTagD3PDFlags.NewGbbNNInfo=False
  JetTagD3PDFlags.QGInfo=False
  JetTagD3PDFlags.JetFitterCharmTagInfo=False
else:
  JetTagD3PDFlags.GbbNNInfo=False

JetTagD3PDFlags.SoftElectronInfo=False ## no calib
JetTagD3PDFlags.JetElectronAssoc = False
JetTagD3PDFlags.JetPhotonAssoc = False
JetTagD3PDFlags.SoftElectronInfo = False
JetTagD3PDFlags.AddElectronInJetsObject = False
JetTagD3PDFlags.AddPhotonInJetsObject = False

if not JetTagD3PD_JetFitterCharm:
  JetTagD3PDFlags.JetFitterCharmTagInfo=False
else:
  JetTagD3PDFlags.Taggers+=["JetFitterCharm","JetFitterCharmNeg"]

if JetTagD3PD_MV3:
   JetTagD3PDFlags.Taggers+=["MV3_bVSu","MV3_bVSc","MV3_cVSu","MV3_bVSuFlip","MV3_bVScFlip","MV3_cVSuFlip"]


### truth ghost association
### after d3pd config to check track assoc getter label

JetTagD3PD_ListOfJets = []

if JetTagD3PD_AntiKt4TopoEM:
  JetTagD3PD_ListOfJets+=['AntiKt4TopoEMJets']
if JetTagD3PD_AntiKt6TopoEM:
  JetTagD3PD_ListOfJets+=['AntiKt6TopoEMJets']
if JetTagD3PD_AntiKt4LCTopo:
  JetTagD3PD_ListOfJets+=['AntiKt4LCTopoJets']
if JetTagD3PD_AntiKt6LCTopo:
  JetTagD3PD_ListOfJets+=['AntiKt6LCTopoJets']
if JetTagD3PD_AntiKt4Truth:
  JetTagD3PD_ListOfJets+=['AntiKt4TruthJets']
if JetTagD3PD_AntiKt4MuonNonInteractingTruth:
  JetTagD3PD_ListOfJets+=['AntiKt4MuonNonInteractingTruthJets']
if JetTagD3PD_AntiKt6MuonNonInteractingTruth:
  JetTagD3PD_ListOfJets+=['AntiKt6MuonNonInteractingTruthJets']
### track ghost assoc not supported for now for these collections
# if JetTagD3PD_AntiKtZ4TrackJets:
#   JetTagD3PD_ListOfJets+=['AntiKtZ4TrackJets']


JetTagGhostAssocList = []

if JetTagD3PD_addGhostTrackAssociation:
  JetTagD3PDFlags.JetTrackGhostAssoc=True
  JetTagGhostAssocList+=["TrackAssoc"]
  for ijet in JetTagD3PD_ListOfJets:
    from JetTagD3PDMaker.JetTagGhostAssocHelper import addTrackGhostAssociation
    addTrackGhostAssociation(ijet+jettagd3pd_jetnamepostfix)


if JetTagD3PD_addGhostBHadronAssociation:
  JetTagD3PDFlags.JetBHadronGhostAssoc=True
  JetTagGhostAssocList+=["TruthBHadron"]
  for ijet in JetTagD3PD_ListOfJets:
    from JetTagD3PDMaker.JetTagGhostAssocHelper import addBHadronGhostAssociation
    addBHadronGhostAssociation(ijet+jettagd3pd_jetnamepostfix)


if JetTagD3PD_addGhostTruthAssociation:
  JetTagD3PDFlags.JetTruthGhostAssoc=True
  JetTagGhostAssocList+=["TruthAssoc"]
  for ijet in JetTagD3PD_ListOfJets:
    from JetTagD3PDMaker.JetTagGhostAssocHelper import addTruthGhostAssociation
    addTruthGhostAssociation(ijet+jettagd3pd_jetnamepostfix)


if JetTagGhostAssocList:
  for ijet in JetTagD3PD_ListOfJets:
    from JetTagD3PDMaker.JetTagGhostAssocHelper import createAssocObjects
    createAssocObjects(ijet+jettagd3pd_jetnamepostfix,JetTagGhostAssocList)


JetTagD3PD_ListOfTruthJets = []

if JetTagD3PD_AntiKt4Truth:
  JetTagD3PD_ListOfTruthJets+=['AntiKt4TruthJets']
if JetTagD3PD_AntiKt4MuonNonInteractingTruth:
  JetTagD3PD_ListOfTruthJets+=['AntiKt4MuonNonInteractingTruthJets']
if JetTagD3PD_AntiKt6MuonNonInteractingTruth:
  JetTagD3PD_ListOfTruthJets+=['AntiKt6MuonNonInteractingTruthJets']

for ijet in JetTagD3PD_ListOfTruthJets:
   JetTagD3PDFlags.D3PDPropDict().update(
            {ijet+jettagd3pd_jetnamepostfix+"_"+JetTagD3PDKeys.JetConstituantTruthAssocBlockName()+
             "_target":TruthD3PDFlags.GenParticleAssocLabel()})
   addToIncludeBlocks(ijet+jettagd3pd_jetnamepostfix,[JetTagD3PDKeys.JetConstituantTruthAssocBlockName()+"tmp"])

#configure MuonScatteringAngleSignificanceTool
if JetTagD3PD_addMuonScatteringSignificance:
  include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

if not JetTagD3PD_slimD3PD:
  JetTagD3PDFlags.SkimD3PD = False


if not 'JetTagD3PD_smearTracks' in dir():
  JetTagD3PD_smearTracks=False
if JetTagD3PD_smearTracks:
  include('JetTagD3PDMaker/JetTagD3PD_smearTracksPostExec.py')
