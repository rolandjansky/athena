print '#BTAG# - MANU'
# This python script loads and configures 
# all the services, algorithms and tools 
# needed for b-tagging.
#         devivie@lal.in2p3.fr
#         vacavant@in2p3.fr 

import string

#
# ========== First tweak flags if needed
#
from AthenaCommon.BFieldFlags import jobproperties

if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags

# -- disable whole b-tagging if not in collision mode or if solenoid is off: 
if not jobproperties.Beam.beamType == "collisions":
  print '#BTAG# - Disabling the whole b-tagging since beamType is not collisions...'
  BTaggingFlags.Active = False

if jobproperties.BField.solenoidOn() == False:
  print '#BTAG# - Disabling the whole b-tagging since solenoid is OFF...'
  BTaggingFlags.Active = False

# -- for reference mode:
if BTaggingFlags.Runmodus == 'reference':
  BTaggingFlags.JetProbFlip = False
  BTaggingFlags.IP2DFlip = False
  BTaggingFlags.IP2DPos = False
  BTaggingFlags.IP2DNeg = False
  BTaggingFlags.IP2DSpcFlip = False
  BTaggingFlags.IP2DSpcPos = False
  BTaggingFlags.IP2DSpcNeg = False
  BTaggingFlags.IP3DFlip = False
  BTaggingFlags.IP3DPos = False
  BTaggingFlags.IP3DNeg = False
  BTaggingFlags.IP3DSpcFlip = False
  BTaggingFlags.IP3DSpcPos = False
  BTaggingFlags.IP3DSpcNeg = False
  BTaggingFlags.SV1Flip = False
  BTaggingFlags.SV2Flip = False
  BTaggingFlags.TrackCountingFlip = False
  BTaggingFlags.MultiSVTag = False
  BTaggingFlags.QGTag = False
  BTaggingFlags.MV1 = False
  BTaggingFlags.MV1c = False
  BTaggingFlags.MV2 = False
  BTaggingFlags.MV2c00 = False
  BTaggingFlags.MV2c10 = False
  BTaggingFlags.MV2c20 = False
  BTaggingFlags.MV1Flip = False
  BTaggingFlags.MV1cFlip = False
  BTaggingFlags.MV2Flip = False
  BTaggingFlags.MV2c00Flip = False
  BTaggingFlags.MV2c10Flip = False
  BTaggingFlags.MV2c20Flip = False
  

# -- disable taggers accordingly to reconstruction scoping levels:
from RecExConfig.RecFlags import rec
print '#BTAG# B-tagging descoping:  level = ', rec.ScopingLevel()
if rec.ScopingLevel() >= 2:
  print '#BTAG# - Disabling low-priority b-tagging algorithms:',
  for tagger in BTaggingFlags._tags:
    if ( tagger in BTaggingFlags.LowPriorityTaggers ):
      print tagger,
      BTaggingFlags.Disable(tagger)
  print
if rec.ScopingLevel() >= 3:
  print '#BTAG# - Disabling medium-priority b-tagging algorithms:',
  for tagger in BTaggingFlags._tags:
    if ( tagger in BTaggingFlags.MediumPriorityTaggers ):
      print tagger,
      BTaggingFlags.Disable(tagger)
  print
if rec.ScopingLevel() >= 4:
  print '#BTAG# - Disabling high-priority b-tagging algorithms:',
  for tagger in BTaggingFlags._tags:
    if ( tagger in BTaggingFlags.HighPriorityTaggers ):
      print tagger,
      BTaggingFlags.Disable(tagger)
  print

# -- now disable specific taggers if geometry not defined:
from AthenaCommon.DetFlags import DetFlags
if not DetFlags.detdescr.Muon_on():
  print '#BTAG# - Disabling soft-muon tagger since muon detdescr. not found'
  BTaggingFlags.Disable('SoftMu')
  BTaggingFlags.Disable('SoftMuChi2')
if not DetFlags.detdescr.Calo_on():
  print '#BTAG# - Disabling soft-electron tagger since calo detdescr. not found'
  BTaggingFlags.Disable('SoftEl')
if not DetFlags.detdescr.ID_on():
  print '#BTAG# - Disabling all b-tagging algorithms since ID detdescr. not found:',
  for tagger in BTaggingFlags._tags:
    print tagger,
    BTaggingFlags.Disable(tagger)
  print

# -- flags are now fully defined:  
BTaggingFlags.Print()

if BTaggingFlags.Active:

  # -- determine the keys of the McEventCollection to be used and of the JetCollections already there in the input
  TheTruthCollectionKey   = 'TruthEvent'
  BTaggingFlags.RetagJets = BTaggingFlags.Jets
  if BTaggingFlags.AutoInspectInputFile:
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.InputFormat == 'pool':
      try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        BTaggingFlags.RetagJets = [ ]
        for i in inputFileSummary['eventdata_items']:
          if i[0] == 'McEventCollection':
            TheTruthCollectionKey = i[1]
          if i[0] == 'JetCollection':
            jetC1 = (i[1]).replace('AODJets','')
            jetC = jetC1.replace('Jets','')
            if jetC in BTaggingFlags.Jets:
              BTaggingFlags.RetagJets += [ jetC ]
      except Exception, err:
        print '#BTAG# - b-tagging: automatic inspection of input file failed (file too old?)'              
  print '#BTAG# --> b-tagging: using ',TheTruthCollectionKey, ' as truth key'
  print '#BTAG# --> b-tagging: re-tagging those jet collections: ', BTaggingFlags.RetagJets

  #
  # ========== Add services
  #

  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  if not hasattr(svcMgr, 'THistSvc'):
     from GaudiSvc.GaudiSvcConf import THistSvc
     svcMgr += THistSvc()

  # 
  # ========== Add algorithms that must run before b-tagging
  #

  if not 'topSequence' in dir():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

  # -- Converter ParticleJets -> Jets if needed to read PJets (old AODs like r13)
  if BTaggingFlags.ConvertParticleJets:
    include ("JetRec/ParticleJetCompatibility.py")

  # 
  # ========== Add tools now
  #
  from AthenaCommon.AppMgr import ToolSvc

  # -- for calibration from COOL
  include( "JetTagCalibration/BTagCalibrationBroker_jobOptions.py" )

  # -- to label jets with truth information
  from RecExConfig.RecFlags import rec
  if rec.doTruth:
    # Jet Label
    if BTaggingFlags.jetTruthMatching == 'matchQuark' or BTaggingFlags.jetTruthMatching == 'jetTrackTruthMatching':
      if BTaggingFlags.jetTruthMatching == 'matchQuark': # this is default!
        from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
        BTagJetTruthMatching = Analysis__JetQuarkLabel(name = "BTagJetTruthMatching")
      elif BTaggingFlags.jetTruthMatching == 'jetTrackTruthMatching':
        from ParticleJetTools.ParticleJetToolsConf import Analysis__JetTrackTruthMatching
        BTagJetTruthMatching = Analysis__JetTrackTruthMatching(
                         name = "BTagJetTruthMatching",
                         TrackParticlesName        = BTaggingFlags.TrackParticleCollectionName,
                         TrackParticleTruthMapName = BTaggingFlags.TrackParticleTruthCollectionName
                        )
    BTagJetTruthMatching.McEventCollection = TheTruthCollectionKey
    ToolSvc += BTagJetTruthMatching
    if BTaggingFlags.OutputLevel < 3:
      print BTagJetTruthMatching

    if  BTaggingFlags.QGTag:
      from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQGPartonLabel
      BTagJetQGPartonTruthMatching = Analysis__JetQGPartonLabel(name = "BTagJetQGPartonTruthMatching")
      BTagJetQGPartonTruthMatching.McEventCollection = TheTruthCollectionKey
      ToolSvc += BTagJetQGPartonTruthMatching
      if BTaggingFlags.OutputLevel < 3:
        print BTagJetQGPartonTruthMatching
    else:
      BTagJetQGPartonTruthMatching = None

    # Lepton Truth Match
    #if (BTaggingFlags.SoftMu | BTaggingFlags.SoftEl):
    from ParticleJetTools.ParticleJetToolsConf import Analysis__FindLeptonTruth
    BTagLeptonTruthTool = Analysis__FindLeptonTruth(name = "BTagLeptonTruthTool")
    BTagLeptonTruthTool.McEventCollection = TheTruthCollectionKey
    ToolSvc += BTagLeptonTruthTool
    if BTaggingFlags.OutputLevel < 3:
      print BTagLeptonTruthTool 

    # Labeling tool:
    from BTagging.BTaggingConf import Analysis__BTagLabeling
    thisBTagLabeling = Analysis__BTagLabeling(
                                 name              = "thisBTagLabeling",
                                 JetTruthMatchTool = BTagJetTruthMatching,
                                 JetQGPartonTruthMatchTool = BTagJetQGPartonTruthMatching,  #MANU??
                                 LeptonTruthTool   = BTagLeptonTruthTool
                                )
    ToolSvc += thisBTagLabeling
    if BTaggingFlags.OutputLevel < 3:
      print thisBTagLabeling

  else: #rec.doTruth
    BTagJetTruthMatching = None
    BTagJetQGPartonTruthMatching = None
    BTagLeptonTruthTool  = None
    thisBTagLabeling  = None

  # Secondary vertex reconsruction tool:
  from BTagging.BTaggingConf import Analysis__BTagSecVertexing
  thisBTagSecVertexing = Analysis__BTagSecVertexing(
                          name              = "thisBTagSecVertexing"
                          )
  ToolSvc += thisBTagSecVertexing
  if BTaggingFlags.OutputLevel < 3:
    print thisBTagSecVertexing

  # Removing tool:
  from BTagging.BTaggingConf import Analysis__BTagRemoving
  thisBTagRemoving = Analysis__BTagRemoving(
                           name                        = "thisBTagRemoving",
                           storeSecondaryVerticesInJet = BTaggingFlags.writeSecondaryVertices,
                           listOfTracksToRemove        = [], 
                           listOfSvxToRemove           = [] 
                          )
  ToolSvc += thisBTagRemoving
  if BTaggingFlags.OutputLevel < 3:
    print thisBTagRemoving

  # -- MAIN b-tagging tool:
  from BTagging.BTaggingConf import Analysis__BTagTool
  myBTagTool = Analysis__BTagTool(
                                  name                  = "myBTagTool",
                                  Runmodus              = BTaggingFlags.Runmodus,
                                  PrimaryVertexName     = BTaggingFlags.PrimaryVertexCollectionName,
                                  BaselineTagger        = BTaggingFlags.BaselineTagger,
                                  BTagLabelingTool      = thisBTagLabeling,
                                  BTagSecVertexingTool  = thisBTagSecVertexing,
                                  BTagRemovingTool      = thisBTagRemoving,
                                  OutputLevel           = BTaggingFlags.OutputLevel
                                 )
  ToolSvc += myBTagTool

  # -- associate tracks to jet: (moved back to use only one tool for the time being for all jet collections)
  from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
  BTagTrackToJetAssociator = Analysis__ParticleToJetAssociator(
                                name        = "BTagTrackToJetAssociator",
                                OutputLevel = BTaggingFlags.OutputLevel 
                               )
  ToolSvc += BTagTrackToJetAssociator
  if BTaggingFlags.OutputLevel < 3:
    print BTagTrackToJetAssociator

  # -- associate leptons to jet: (same tool for all jet collections)
  if ( BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 ):
    BTagMuonToJetAssociator = Analysis__ParticleToJetAssociator(
                   name        = "BTagMuonToJetAssociator",
                   trackCone   = 1.0, #for the time being... has to be studied 
                   useVariableSizedTrackCone = False,
                   OutputLevel = BTaggingFlags.OutputLevel
                  )
    ToolSvc += BTagMuonToJetAssociator
    if BTaggingFlags.OutputLevel < 3:
      print BTagMuonToJetAssociator
  else:
    BTagMuonToJetAssociator = None

  if BTaggingFlags.SoftEl:
    BTagElectronToJetAssociator = Analysis__ParticleToJetAssociator(
                   name        = "BTagElectronToJetAssociator",
                   trackCone   = 0.4, #for the time being... has to be studied 
                   useVariableSizedTrackCone = False,
                   OutputLevel = BTaggingFlags.OutputLevel # does not work right now
                  )
    ToolSvc += BTagElectronToJetAssociator
    if BTaggingFlags.OutputLevel < 3:
      print BTagElectronToJetAssociator
  else:
    BTagElectronToJetAssociator = None


  # -- extrapolation to propagate tracks to primary vertex:
  from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
  AtlasExtrapolator = AtlasExtrapolator()
  ToolSvc += AtlasExtrapolator

  from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
  BTagTrackToVertexTool = Reco__TrackToVertex(
                                              name = "BTagTrackToVertexTool",
                                              Extrapolator = AtlasExtrapolator
                                             )
  ToolSvc += BTagTrackToVertexTool
  if BTaggingFlags.OutputLevel < 3:
    print BTagTrackToVertexTool

  # -- tracking tools to sign the impact parameter:
  if (BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
      BTaggingFlags.JetProb |
      BTaggingFlags.TrackCounting |
      BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 | BTaggingFlags.SoftEl):
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    BTagLinTrkFactory = Trk__FullLinearizedTrackFactory(name              = "BTagFullLinearizedTrackFactory",
                                                        Extrapolator      = AtlasExtrapolator)
    ToolSvc += BTagLinTrkFactory
    if BTaggingFlags.OutputLevel < 3:
      print BTagLinTrkFactory
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    BTagTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name="BTagTrackToVertexIPEstimator",
                                                                 OutputLevel = BTaggingFlags.OutputLevel,
                                                                 Extrapolator=AtlasExtrapolator,
                                                                 LinearizedTrackFactory=BTagLinTrkFactory)
    ToolSvc += BTagTrackToVertexIPEstimator
    if BTaggingFlags.OutputLevel < 3:
      print BTagTrackToVertexIPEstimator

  # -- load secondary vertex tools: 
  if (BTaggingFlags.SV1 | BTaggingFlags.SV2 | 
      BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
      BTaggingFlags.JetProb |
      BTaggingFlags.TrackCounting):
    include( "BTagging/BTagging_InDetVKalVxInJetTool.py" )  
  #  myBTagTool.SecVtxFinderList+= [ InDetVKalVxInJetTool ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ InDetVKalVxInJetTool ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["InDetVKalVxInJetTool"]

  if BTaggingFlags.SV0:
    include( "BTagging/BTagging_InDetVKalVxNegInJetTool.py" )  
  #  myBTagTool.SecVtxFinderList+= [ InDetVKalVxNegInJetTool ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ InDetVKalVxNegInJetTool ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["InDetVKalVxNegInJetTool"]
    
  if BTaggingFlags.MultiSV:

### different track association for bb
    BTagTrackToJetAssociatorBB = Analysis__ParticleToJetAssociator(
      name        = "BTagTrackToJetAssociatorBB",
      OutputLevel = BTaggingFlags.OutputLevel,
      shareTracks=False,
      useVariableSizedTrackCone= True,
      coneSizeFitPar1 = 3.15265e-01,
      coneSizeFitPar2 = -3.66502e-01,
      coneSizeFitPar3 = -1.56387e-05,
      )
    ToolSvc += BTagTrackToJetAssociatorBB
    include( "BTagging/BTagging_InDetVKalMultiVxInJetTool.py" )  
  #  myBTagTool.SecVtxFinderList+= [ InDetVKalMultiVxInJetTool ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["TracksBB"]
    thisBTagSecVertexing.SecVtxFinderList+= [ InDetVKalMultiVxInJetTool ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["TracksBB"]
    thisBTagRemoving.listOfSvxToRemove+= ["InDetVKalMultiVxInJetTool"]
    thisBTagRemoving.listOfTracksToRemove = ["TracksBB"]
    
  from BTagging.BTaggingConf import Analysis__BTagTrackAssociation
  thisBTagTrackAssociation = Analysis__BTagTrackAssociation(
                name                       = "thisBTagTrackAssociation",
                BTagAssociation            = BTaggingFlags.doStandardAssoc,
                TrackParticleContainerName = BTaggingFlags.TrackParticleCollectionName,
                MuonToJetAssociator        = BTagMuonToJetAssociator,     # OK: always the same
                ElectronToJetAssociator    = BTagElectronToJetAssociator) # will also not be duplicated in memory

  # -- configure leptons:
  # and configure removal tool
  if BTaggingFlags.SoftMu:
    thisBTagTrackAssociation.MuonContainerName       = BTaggingFlags.MuonCollectionName
  if BTaggingFlags.SoftMuChi2:
    thisBTagTrackAssociation.MuonContainerName       = BTaggingFlags.MuonCollectionName
  if BTaggingFlags.SoftEl:
    thisBTagTrackAssociation.ElectronContainerName   = BTaggingFlags.ElectronCollectionName
    thisBTagTrackAssociation.PhotonContainerName     = BTaggingFlags.PhotonCollectionName
  if BTaggingFlags.MultiSV:
    thisBTagTrackAssociation.AdditionalTrackToJetAssocName  = [ "TracksBB" ]
    thisBTagTrackAssociation.AdditionalTrackToJetAssociator = [ToolSvc.BTagTrackToJetAssociatorBB ]
    thisBTagRemoving.listOfTracksToRemove                   = ["TracksBB"]

        
  ToolSvc += thisBTagTrackAssociation
  if BTaggingFlags.OutputLevel < 3:
    print thisBTagTrackAssociation



  if (BTaggingFlags.SV1Flip | BTaggingFlags.SV2Flip):
    include( "BTagging/BTagging_InDetVKalVxNegativeTagInJetTool.py" )  
  #  myBTagTool.SecVtxFinderList+= [ InDetVKalVxNegativeTagInJetTool ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ InDetVKalVxNegativeTagInJetTool ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["InDetVKalVxNegativeTagInJetTool"]
    
  if BTaggingFlags.BasicJetFitter:
    include( "BTagging/BTagging_JetFitterFinder.py" )
  #  myBTagTool.SecVtxFinderList+= [ JetFitterVxFinder ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ JetFitterVxFinder ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["JetFitterVxFinder"]
        
  if BTaggingFlags.JetFitterTag | BTaggingFlags.JetFitterNN:
    include( "BTagging/BTagging_NewJetFitterFinder.py" )
  #  myBTagTool.SecVtxFinderList+= [ NewJetFitterVxFinder ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ NewJetFitterVxFinder ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["NewJetFitterVxFinder"]
        
  if BTaggingFlags.JetFitterTagFlip | BTaggingFlags.JetFitterNNFlip:
    include( "BTagging/BTagging_NewJetFitterFinderNeg.py" )
  #  myBTagTool.SecVtxFinderList+= [ NewJetFitterVxFinderFlip ]
  #  myBTagTool.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagSecVertexing.SecVtxFinderList+= [ NewJetFitterVxFinderFlip ]
    thisBTagSecVertexing.SecVtxFinderTrackNameList+=["Tracks"]
    thisBTagRemoving.listOfSvxToRemove+= ["NewJetFitterVxFinderFlip"]
        
  # check removal tool
  print thisBTagRemoving.listOfTracksToRemove
  print thisBTagRemoving.listOfSvxToRemove

  # -- load all the taggers ! :


  if BTaggingFlags.IP1D:
    include( "BTagging/BTagging_IP1DTag.py" )
    myBTagTool.TagToolList += [ IP1DTagTool ]
  if BTaggingFlags.IP2D:
    include( "BTagging/BTagging_IP2DTag.py" )
    myBTagTool.TagToolList += [ IP2DTagTool ]
  if BTaggingFlags.IP2DFlip:
    include( "BTagging/BTagging_IP2DFlipTag.py" )
    myBTagTool.TagToolList += [ IP2DFlipTagTool ]
  if BTaggingFlags.IP2DPos:
    include( "BTagging/BTagging_IP2DPosTag.py" )
    myBTagTool.TagToolList += [ IP2DPosTagTool ]
  if BTaggingFlags.IP2DNeg:
    include( "BTagging/BTagging_IP2DNegTag.py" )
    myBTagTool.TagToolList += [ IP2DNegTagTool ]
  if BTaggingFlags.IP2DSpc:
    include( "BTagging/BTagging_IP2DSpcTag.py" )
    myBTagTool.TagToolList += [ IP2DSpcTagTool ]
  if BTaggingFlags.IP2DSpcFlip:
    include( "BTagging/BTagging_IP2DSpcFlipTag.py" )
    myBTagTool.TagToolList += [ IP2DSpcFlipTagTool ]
  if BTaggingFlags.IP2DSpcPos:
    include( "BTagging/BTagging_IP2DSpcPosTag.py" )
    myBTagTool.TagToolList += [ IP2DSpcPosTagTool ]
  if BTaggingFlags.IP2DSpcNeg:
    include( "BTagging/BTagging_IP2DSpcNegTag.py" )
    myBTagTool.TagToolList += [ IP2DSpcNegTagTool ]
  if BTaggingFlags.IP3D:
    include( "BTagging/BTagging_IP3DTag.py" )
    myBTagTool.TagToolList += [ IP3DTagTool ]
  if BTaggingFlags.IP3DFlip:
    include( "BTagging/BTagging_IP3DFlipTag.py" )
    myBTagTool.TagToolList += [ IP3DFlipTagTool ]
  if BTaggingFlags.IP3DPos:
    include( "BTagging/BTagging_IP3DPosTag.py" )
    myBTagTool.TagToolList += [ IP3DPosTagTool ]
  if BTaggingFlags.IP3DNeg:
    include( "BTagging/BTagging_IP3DNegTag.py" )
    myBTagTool.TagToolList += [ IP3DNegTagTool ]
  if BTaggingFlags.IP3DSpc:
    include( "BTagging/BTagging_IP3DSpcTag.py" )
    myBTagTool.TagToolList += [ IP3DSpcTagTool ]
  if BTaggingFlags.IP3DSpcFlip:
    include( "BTagging/BTagging_IP3DSpcFlipTag.py" )
    myBTagTool.TagToolList += [ IP3DSpcFlipTagTool ]
  if BTaggingFlags.IP3DSpcPos:
    include( "BTagging/BTagging_IP3DSpcPosTag.py" )
    myBTagTool.TagToolList += [ IP3DSpcPosTagTool ]
  if BTaggingFlags.IP3DSpcNeg:
    include( "BTagging/BTagging_IP3DSpcNegTag.py" )
    myBTagTool.TagToolList += [ IP3DSpcNegTagTool ]

  if BTaggingFlags.SV1:
    include( "BTagging/BTagging_SV1Tag.py" )
    myBTagTool.TagToolList += [ SV1TagTool ]
  if BTaggingFlags.SV1Flip:
    include( "BTagging/BTagging_SV1FlipTag.py" )
    myBTagTool.TagToolList += [ SV1FlipTagTool ]
  if BTaggingFlags.SV2:
    include( "BTagging/BTagging_SV2Tag.py" )
    myBTagTool.TagToolList += [ SV2TagTool ]
  if BTaggingFlags.SV2Flip:
    include( "BTagging/BTagging_SV2FlipTag.py" )
    myBTagTool.TagToolList += [ SV2FlipTagTool ]
  if BTaggingFlags.SV0:
    include( "BTagging/BTagging_SV0Tag.py" )
    myBTagTool.TagToolList += [ SV0TagTool ]

  if BTaggingFlags.MultiSV:
    include( "BTagging/BTagging_MultiSVTag.py" )
    myBTagTool.TagToolList += [ MultiSVTagTool ]

  if BTaggingFlags.JetProb:
    include( "BTagging/BTagging_JetProbTag.py" )
    myBTagTool.TagToolList += [ JetProbTagTool ]
    if BTaggingFlags.JetProbFlip:
      include( "BTagging/BTagging_JetProbFlipTag.py" )
      myBTagTool.TagToolList += [ JetProbFlipTagTool ]

  if BTaggingFlags.SoftEl:
    include( "BTagging/BTagging_SoftElectronTag.py" )
    myBTagTool.TagToolList += [ SoftElectronTagTool ]
  if BTaggingFlags.SoftMu:
    include( "BTagging/BTagging_SoftMuonTag.py" )
    myBTagTool.TagToolList += [ SoftMuonTagTool ]
  if BTaggingFlags.SoftMuChi2:
    include( "BTagging/BTagging_SoftMuonTagChi2.py" )
    myBTagTool.TagToolList += [ SoftMuonTagChi2Tool ]

  if BTaggingFlags.BasicJetFitter:
    include( "BTagging/BTagging_JetFitterTag.py" )
    myBTagTool.TagToolList += [ JetFitterTagTool ]
    if BTaggingFlags.IP3D:
      include( "BTagging/BTagging_JetFitterCOMB.py" )
      myBTagTool.TagToolList  += [ JetFitterCOMB ];
  if BTaggingFlags.JetFitterTag:
    include( "BTagging/BTagging_NewJetFitterTag.py" )
    myBTagTool.TagToolList += [ NewJetFitterTagTool ]
    if BTaggingFlags.IP3D:
      include( "BTagging/BTagging_NewJetFitterCOMB.py" )
      myBTagTool.TagToolList += [ NewJetFitterCOMB ]
  if BTaggingFlags.JetFitterNN:
    include( "BTagging/BTagging_JetFitterTagNN.py" )
    myBTagTool.TagToolList += [ JetFitterTagToolNN ]
    if BTaggingFlags.IP3D:
      include( "BTagging/BTagging_JetFitterCOMBNN.py" )
      myBTagTool.TagToolList += [ JetFitterCOMBNN ]

  if BTaggingFlags.JetFitterTagFlip:
    include( "BTagging/BTagging_NewJetFitterTagNeg.py" )
    myBTagTool.TagToolList += [ NewJetFitterFlipTagTool ]
    if BTaggingFlags.IP3DNeg:
      include( "BTagging/BTagging_NewJetFitterCOMBNeg.py" )
      myBTagTool.TagToolList += [ NewJetFitterFlipCOMB ]
  if BTaggingFlags.JetFitterNNFlip:
    include( "BTagging/BTagging_JetFitterTagNNNeg.py" )
    myBTagTool.TagToolList += [ JetFitterFlipTagToolNN ]
    if BTaggingFlags.IP3DPos:
      include( "BTagging/BTagging_JetFitterCOMBNN_IP3DPos.py" )
      myBTagTool.TagToolList += [ JetFitterIP3DPosCOMBNN ]
    if BTaggingFlags.IP3DNeg:
      include( "BTagging/BTagging_JetFitterCOMBNN_IP3DNeg.py" )
      myBTagTool.TagToolList += [ JetFitterIP3DNegCOMBNN ]
  if BTaggingFlags.IPFordG: 
    from BTagging.IPFordG import add_ipfordg
    ipfordg = add_ipfordg(
      ToolSvc, BTagCalibrationBrokerTool, BTaggingFlags, 
      BTagTrackToVertexTool, BTagTrackToVertexIPEstimator)
    myBTagTool.TagToolList += [ipfordg]
  if BTaggingFlags.TrackCounting:
    include( "BTagging/BTagging_TrackCounting.py" )
    myBTagTool.TagToolList += [ TrackCountingTool ]      
  if BTaggingFlags.TrackCountingFlip:
    include( "BTagging/BTagging_TrackCountingFlip.py" )
    myBTagTool.TagToolList += [ TrackCountingFlipTool ]

  if BTaggingFlags.QGTag:
    include( "BTagging/BTagging_QGTag.py" )
    myBTagTool.TagToolList += [ QGTag ]

  if BTaggingFlags.MV1:
    include( "BTagging/BTagging_MV1Tag.py" )
    myBTagTool.TagToolList += [ MV1TagTool ]
  if BTaggingFlags.MV1Flip:
    include( "BTagging/BTagging_MV1FlipTag.py" )
    myBTagTool.TagToolList += [ MV1FlipTagTool ]
  if BTaggingFlags.MV1c:
    include( "BTagging/BTagging_MV1cTag.py" )
    myBTagTool.TagToolList += [ MV1cTagTool ]
  if BTaggingFlags.MV1cFlip:
    include( "BTagging/BTagging_MV1cFlipTag.py" )
    myBTagTool.TagToolList += [ MV1cFlipTagTool ]
  if BTaggingFlags.MV2:
    include( "BTagging/BTagging_MV2Tag.py" )
    myBTagTool.TagToolList += [ MV2TagTool ]
  if BTaggingFlags.MV2c00:
    include( "BTagging/BTagging_MV2c00Tag.py" )
    myBTagTool.TagToolList += [ MV2c00TagTool ]
  if BTaggingFlags.MV2c10:
    include( "BTagging/BTagging_MV2c10Tag.py" )
    myBTagTool.TagToolList += [ MV2c10TagTool ]
  if BTaggingFlags.MV2c20:
    include( "BTagging/BTagging_MV2c20Tag.py" )
    myBTagTool.TagToolList += [ MV2c20TagTool ]
  if BTaggingFlags.MV2Flip:
    include( "BTagging/BTagging_MV2FlipTag.py" )
    myBTagTool.TagToolList += [ MV2FlipTagTool ]
  if BTaggingFlags.MV2c00Flip:
    include( "BTagging/BTagging_MV2c00FlipTag.py" )
    myBTagTool.TagToolList += [ MV2c00FlipTagTool ]
  if BTaggingFlags.MV2c10Flip:
    include( "BTagging/BTagging_MV2c10FlipTag.py" )
    myBTagTool.TagToolList += [ MV2c10FlipTagTool ]
  if BTaggingFlags.MV2c20Flip:
    include( "BTagging/BTagging_MV2c20FlipTag.py" )
    myBTagTool.TagToolList += [ MV2c20FlipTagTool ]



  # -- b-tagging tool is now fully configured.

  # 
  # ========== Deal with calibration histogram files
  #

  # -- for analysis (should be in COOL now):
  if BTaggingFlags.Runmodus == 'analysis':

    if ( BTaggingFlags.lifetime1D | BTaggingFlags.lifetime2D | BTaggingFlags.lifetime3D | 
         BTaggingFlags.secVtxFitBU | BTaggingFlags.secVtxFitTD ):
      dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
      dataPathList.insert(0, os.curdir)
      from AthenaCommon.Utils.unixtools import FindFile
      JetTagsigname = FindFile( 'all_Cone4TowerParticleJets_matchQuark_FastVertexFitter_sig.root', dataPathList, os.R_OK )
      JetTagbkgname = FindFile( 'all_Cone4TowerParticleJets_matchQuark_FastVertexFitter_bkg.root', dataPathList, os.R_OK )
      svcMgr.THistSvc.Input  += ["SigCali DATAFILE='"+JetTagsigname+"' OPT='OLD'"]
      svcMgr.THistSvc.Input  += ["BkgCali DATAFILE='"+JetTagbkgname+"' OPT='OLD'"]

    writeSmoothedHistos = False
    if writeSmoothedHistos:
      svcMgr.THistSvc.Output += ["ControlFile DATAFILE='smoothcontrol.root' OPT='RECREATE'"]

  # -- for reference mode:
  if BTaggingFlags.Runmodus == 'reference':

    svcMgr.THistSvc.Output += ["RefFile DATAFILE='BTagCalibALL.root' OPT='RECREATE'"]
    #for key in BTaggingFlags.Jets:
    #  for tagger in ['JetProb','IP1D','IP2D','IP2DSpc','IP3D','IP3DSpc','SV1','SV2','SoftMu','SoftEl']:
    #    if BTaggingFlags.IsEnabled(tagger):
    #      RefileName = "BTaggingRef"+tagger+key+".root"
    #      svcMgr.THistSvc.Output += ["RefFile"+tagger+key+" DATAFILE='"+RefileName+"' OPT='RECREATE'"]

    if ( BTaggingFlags.lifetime1D | BTaggingFlags.lifetime2D | BTaggingFlags.lifetime3D | 
         BTaggingFlags.secVtxFitBU | BTaggingFlags.secVtxFitTD ):
      svcMgr.THistSvc.Output += ["SigCali DATAFILE='cern.sig.referencehistos.root' OPT='RECREATE'"]
      svcMgr.THistSvc.Output += ["BkgCali DATAFILE='cern.bkg.referencehistos.root' OPT='RECREATE'"]

