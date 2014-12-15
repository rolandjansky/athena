# BTagging_Rel19_LoadTools.py
# This python script loads and configures 
# all the services, algorithms and tools 
# needed for b-tagging in release 19.
#         guirriec@in2p3.fr
# March 2014


# Import the jet flags.

#from JetRec.JetToolSupport import jtm

from BTagging.BTaggingFlags import BTaggingFlags
print "Using output level ", BTaggingFlags.OutputLevel
###### btagging
from BTagging.BTaggingConf import Analysis__JetBTaggerTool as JetBTaggerTool

from AthenaCommon.AppMgr import ToolSvc

#Secondary vertex reconstruction tool: 
from BTagging.BTaggingConf import Analysis__BTagSecVertexing  
myBTagSecVtx = Analysis__BTagSecVertexing(
  name              = "myBTagSecVtx",
  OutputLevel           = BTaggingFlags.OutputLevel
)
ToolSvc += myBTagSecVtx 


#Add services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()



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



# -- Truth labeling tool:
from JetRec.JetRecFlags import jetFlags
if jetFlags.useTruth():
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
    BTagJetTruthMatching = Analysis__JetQuarkLabel(name = "BTagJetTruthMatching")
    BTagJetTruthMatching.McEventCollection = TheTruthCollectionKey
    ToolSvc += BTagJetTruthMatching
    if BTaggingFlags.OutputLevel < 3:
      print BTagJetTruthMatching

    from BTagging.BTaggingConf import Analysis__BTagLabeling
    thisBTagLabeling = Analysis__BTagLabeling(
        name              = "thisBTagLabeling",
        JetTruthMatchTool = BTagJetTruthMatching
        #JetQGPartonTruthMatchTool = BTagJetQGPartonTruthMatching,  #MANU??
        #LeptonTruthTool   = BTagLeptonTruthTool
        )
    ToolSvc += thisBTagLabeling
    if BTaggingFlags.OutputLevel < 3:
      print thisBTagLabeling

else: #useTruth()
    BTagJetTruthMatching = None
    thisBTagLabeling  = None

# -- MAIN b-tagging tool:
from BTagging.BTaggingConf import Analysis__BTagTool
myBTagTool = Analysis__BTagTool(
  name                  = "myBTagTool",
  Runmodus              = BTaggingFlags.Runmodus,
  #PrimaryVertexName     = BTaggingFlags.PrimaryVertexCollectionName,
  #BaselineTagger        = BTaggingFlags.BaselineTagger,
  BTagLabelingTool      = thisBTagLabeling,
  #BTagSecVertexing   = myBTagSecVtx,
  #BTagRemovingTool      = thisBTagRemoving,
  OutputLevel           = BTaggingFlags.OutputLevel
)
ToolSvc += myBTagTool




from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
BTagTrackToJetAssociator = Analysis__ParticleToJetAssociator(
          name        = "BTagTrackToJetAssociator"
        )
ToolSvc += BTagTrackToJetAssociator

from BTagging.BTaggingConf import Analysis__BTagTrackAssociation
myBTagTrackAssociation = Analysis__BTagTrackAssociation(
  name                  = "myBTagTrackAssociation",
  TrackToJetAssociatorList   = [ BTagTrackToJetAssociator ],
  TrackToJetAssocNameList    = ["BTagTrackToJetAssociator"],
  TrackContainerNameList     = ["InDetTrackParticles"],
  #TrackParticleContainerName = BTaggingFlags.TrackParticleCollectionName,
  #Runmodus              = BTaggingFlags.Runmodus,
  #PrimaryVertexName     = BTaggingFlags.PrimaryVertexCollectionName,
  #BaselineTagger        = BTaggingFlags.BaselineTagger,
  #BTagLabelingTool      = thisBTagLabeling,
  #BTagSecVertexingTool  = thisBTagSecVertexing,
  #BTagRemovingTool      = thisBTagRemoving,
  OutputLevel           = BTaggingFlags.OutputLevel
)
ToolSvc += myBTagTrackAssociation
# track assoc for bb-tag
if BTaggingFlags.MultiSVbb1 | BTaggingFlags.MultiSVbb2:

  BTagTrackToJetAssociatorBB = Analysis__ParticleToJetAssociator(
    name        = "BTagTrackToJetAssociatorBB",
    OutputLevel           = BTaggingFlags.OutputLevel,
    shareTracks           = False,
    useVariableSizedTrackCone = True,
    coneSizeFitPar1 = 3.15265e-01,
    coneSizeFitPar2 = -3.66502e-01,
    coneSizeFitPar3 = -1.56387e-05,
  )
  ToolSvc += BTagTrackToJetAssociatorBB

  myBTagTrackAssociation.TrackToJetAssociatorList += [ BTagTrackToJetAssociatorBB ]
  myBTagTrackAssociation.TrackToJetAssocNameList  += ["BTagTrackToJetAssociatorBB"]
  myBTagTrackAssociation.TrackContainerNameList   += ["InDetTrackParticles"]

  include( "BTagging/BTagging_InDetVKalMultiVxInJetTool.py" ) 
  myBTagSecVtx.SecVtxFinderList             += [ InDetVKalMultiVxInJetTool ]
  myBTagSecVtx.SecVtxFinderTrackNameList    += ["BTagTrackToJetAssociatorBB"]
  myBTagSecVtx.SecVtxFinderxAODBaseNameList += ["MSV"]
  myBTagSecVtx.MSVVariableFactory = MSVVariablesFactory
#
from AthenaCommon.Resilience import treatException,protectedInclude
protectedInclude( "JetTagCalibration/BTagCalibrationBroker_jobOptions.py" )

BTaggingFlags.CalibrationSingleFolder = True


#myJets=["AntiKt4Truth"]
#myJets=["AntiKt4TopoEM"]


#myJetCollectionForCalib = []
#myJetCollectionForCalib+= myJets
#theFolders = []
#myCalibrationChannelAliases = ['AntiKt4Truth->AntiKt4TopoEM']
#from JetTagCalibration.JetTagCalibrationConf import Analysis__CalibrationBroker
#BTagCalibrationBrokerTool = Analysis__CalibrationBroker(
#                                                          name = "BTagCalibrationBrokerTool",
#                                                          folderRoot = "/GLOBAL/BTagCalib/",
#                                                          folders = theFolders,
#                                                          channels = myJetCollectionForCalib,
#                                                          channelAliases = myCalibrationChannelAliases,
#                                                          shadowFoldersAndChannels = True,   
#                                                       )
#ToolSvc += BTagCalibrationBrokerTool

# from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
# IP3DNewLikelihood = Analysis__NewLikelihoodTool( 
#                                                  name = "IP3DNewLikelihoodTool",
#                                                 OutputLevel = BTaggingFlags.OutputLevel,
#                                                  taggerName = "IP3D",
#                                                  calibrationTool = BTagCalibrationBrokerTool
#                                                )
# ToolSvc += IP3DNewLikelihood





# from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
# IP3DBasicTrackGradeFactory = Analysis__BasicTrackGradeFactory(name = "IP3DBasicTrackGradeFactory",
#                                                              OutputLevel = BTaggingFlags.OutputLevel,
#                                                              TrackSummaryTool = None,
#                                                               useSharedHitInfo = True,
# )
# ToolSvc += IP3DBasicTrackGradeFactory

# TrackToVertex tool
#
# -- extrapolation to propagate tracks to primary vertex:
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
AtlasExtrapolator = AtlasExtrapolator()
ToolSvc += AtlasExtrapolator

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
myBTagLinTrkFactory = Trk__FullLinearizedTrackFactory(name              = "BTagFullLinearizedTrackFactory",
                                                        Extrapolator      = AtlasExtrapolator)
ToolSvc += myBTagLinTrkFactory

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
BTagTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name="BTagTrackToVertexIPEstimator",
                                                                 Extrapolator=AtlasExtrapolator,
                                                                 LinearizedTrackFactory=myBTagLinTrkFactory)
ToolSvc += BTagTrackToVertexIPEstimator

from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
BTagTrackToVertexTool = Reco__TrackToVertex(
                                           name = "BTagTrackToVertexTool",
                                           Extrapolator = AtlasExtrapolator
                                           )
ToolSvc += BTagTrackToVertexTool


from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
IP3DTrackSelector = Analysis__TrackSelector( 
                                         useBLayerHitPrediction = True,
                                         name = "IP3DTrackSelector",
                                         trackToVertexTool = BTagTrackToVertexTool,
                                         OutputLevel = BTaggingFlags.OutputLevel
                                        )
ToolSvc += IP3DTrackSelector

# -- load secondary vertex tools:
if (BTaggingFlags.SV1 | BTaggingFlags.SV2 | 
    BTaggingFlags.IP1D | BTaggingFlags.IP2D | BTaggingFlags.IP3D |
    BTaggingFlags.JetProb |
    BTaggingFlags.TrackCounting):
  include( "BTagging/BTagging_InDetVKalVxInJetTool.py" ) 
  myBTagSecVtx.SecVtxFinderList+= [ InDetVKalVxInJetTool ]
  myBTagSecVtx.SecVtxFinderTrackNameList+=["BTagTrackToJetAssociator"]
  myBTagSecVtx.SecVtxFinderxAODBaseNameList+=["SV1"]


if BTaggingFlags.SV0:
    include( "BTagging/BTagging_InDetVKalVxNegInJetTool.py" )  
    myBTagSecVtx.SecVtxFinderList+= [ InDetVKalVxNegInJetTool ]
    myBTagSecVtx.SecVtxFinderTrackNameList+=["BTagTrackToJetAssociator"]
    myBTagSecVtx.SecVtxFinderxAODBaseNameList+=["SV0"]


#jet fitter vertexing (flags off for the moment)
if BTaggingFlags.JetFitterTag | BTaggingFlags.JetFitterNN:
   include( "BTagging/BTagging_NewJetFitterFinder.py" )
   myBTagSecVtx.SecVtxFinderList+= [ NewJetFitterVxFinder ]
   myBTagSecVtx.SecVtxFinderTrackNameList+=["BTagTrackToJetAssociator"]
   myBTagSecVtx.SecVtxFinderxAODBaseNameList+=["JetFitter"]
   myBTagSecVtx.JetFitterVariableFactory = JetFitterVariablesFactory




# -- load all the taggers ! :
#from JetTagTools.JetTagToolsConf import Analysis__DummyTag
#DummyTagTool = Analysis__DummyTag("mydummy")
#ToolSvc+=DummyTagTool
#myBTagTool.TagToolList+= [DummyTagTool]

#IP2D
if BTaggingFlags.IP2D :
    include( "BTagging/BTagging_IP2DTag.py" )
    myBTagTool.TagToolList+= [ IP2DTagTool ]

#IP3D
if BTaggingFlags.IP3D :
    include( "BTagging/BTagging_IP3DTag.py" )
    myBTagTool.TagToolList+= [ IP3DTagTool ]

#SV1
if BTaggingFlags.SV1 :
    include( "BTagging/BTagging_SV1Tag.py" )
    myBTagTool.TagToolList += [ SV1TagTool ]


#SV0
if BTaggingFlags.SV0 :
    include( "BTagging/BTagging_SV0Tag.py" )
    myBTagTool.TagToolList += [ SV0TagTool ]

#JetFitter
if BTaggingFlags.JetFitterNN:
    include( "BTagging/BTagging_JetFitterTagNN.py" )
    myBTagTool.TagToolList += [ JetFitterTagToolNN ]
    if BTaggingFlags.IP3D:
        include( "BTagging/BTagging_JetFitterCOMBNN.py" )
        myBTagTool.TagToolList += [ JetFitterCOMBNN ]


#MV1
if BTaggingFlags.MV1 :
    include( "BTagging/BTagging_MV1Tag.py" )
    myBTagTool.TagToolList += [ MV1TagTool ]


#MV1c
if BTaggingFlags.MV1c :
    include( "BTagging/BTagging_MV1cTag.py" )
    myBTagTool.TagToolList += [ MV1cTagTool ]

# MV2
if BTaggingFlags.MV2c00:
    include( "BTagging/BTagging_MV2c00Tag.py" )
    myBTagTool.TagToolList += [ MV2c00TagTool ]
if BTaggingFlags.MV2c10:
    include( "BTagging/BTagging_MV2c10Tag.py" )
    myBTagTool.TagToolList += [ MV2c10TagTool ]
if BTaggingFlags.MV2c20:
    include( "BTagging/BTagging_MV2c20Tag.py" )
    myBTagTool.TagToolList += [ MV2c20TagTool ]


#MVb
if BTaggingFlags.MVb:
    include( "BTagging/BTagging_MVbTag.py" )
    myBTagTool.TagToolList += [ MVbTagTool ]
#MSV
if BTaggingFlags.MultiSVbb1:
    include( "BTagging/BTagging_MultiSVbb1Tag.py" )
    myBTagTool.TagToolList += [ MultiSVbb1TagTool ]
if BTaggingFlags.MultiSVbb2:
    include( "BTagging/BTagging_MultiSVbb2Tag.py" )
    myBTagTool.TagToolList += [ MultiSVbb2TagTool ]

from BTagging.BTaggingConf import Analysis__JetBTaggerTool
myJetBTaggerTool = Analysis__JetBTaggerTool(
  name                  = "myJetBTaggerTool",
  BTagTool              = myBTagTool,
  BTagTrackAssocTool    = myBTagTrackAssociation,
  BTagSecVertexing      = myBTagSecVtx,
  OutputLevel           = BTaggingFlags.OutputLevel
)
ToolSvc += myJetBTaggerTool

if BTaggingFlags.OutputLevel < 3:
    print myBTagSecVtx
    print myJetBTaggerTool




# -- for reference mode:
if BTaggingFlags.Runmodus == 'reference':

    svcMgr.THistSvc.Output += ["RefFile DATAFILE='BTagCalibALL.root' OPT='RECREATE'"]
    #for key in BTaggingFlags.Jets:
    #    for tagger in ['JetProb','IP1D','IP2D','IP2DSpc','IP3D','IP3DSpc','SV1','SV2','SoftMu','SoftEl','JetFitterTagNN','JetFitterCOMBNN']:
    #        if BTaggingFlags.IsEnabled(tagger):
    #            RefileName = "BTaggingRef"+tagger+key+".root"
    #            svcMgr.THistSvc.Output += ["RefFile"+tagger+key+" DATAFILE='"+RefileName+"' OPT='RECREATE'"]

if ( BTaggingFlags.lifetime1D | BTaggingFlags.lifetime2D | BTaggingFlags.lifetime3D | 
         BTaggingFlags.secVtxFitBU | BTaggingFlags.secVtxFitTD ):
    svcMgr.THistSvc.Output += ["SigCali DATAFILE='cern.sig.referencehistos.root' OPT='RECREATE'"]
    svcMgr.THistSvc.Output += ["BkgCali DATAFILE='cern.bkg.referencehistos.root' OPT='RECREATE'"]




#### should be unique per collection
#### replaces the jet rec JetBTaggerTool config or the BJetBuilder config in case of retagging
#jbtt = JetBTaggerTool("antikt4_btagger", DummyProp=99, BTagTool=myBTagTool, BTagName = "Rel19", BTagTrackAssocTool = myBTagTrackAssociation)

#jtm+=jbtt

