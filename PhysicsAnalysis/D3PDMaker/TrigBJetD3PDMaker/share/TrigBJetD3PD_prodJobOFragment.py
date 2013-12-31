# $Id: TrigBJetD3PD_prodJobOFragment.py 559501 2013-08-27 23:10:01Z dferreir $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "TrigBJet D3PD cart".

# This jobO should not be included more than once:
include.block( "TrigBJetD3PDMaker/TrigBJetD3PD_prodJobOFragment.py" )

# redo offline btagging, to get the negative weights
if not 'JetTagD3PD_redoTagging' in dir():
   JetTagD3PD_redoTagging=True

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

from BTagging.BTaggingFlags import BTaggingFlags

BTaggingFlags.Jets=[ "AntiKt4TopoEM", "AntiKt6TopoEM",# "AntiKt4Truth",
                     "AntiKt4MuonNonInteractingTruth","AntiKt6MuonNonInteractingTruth", "AntiKtZ4Track"]
BTaggingFlags.JetsWithInfoPlus=[ "AntiKt4TopoEM", "AntiKt6TopoEM",#"AntiKt4Truth",
                                 "AntiKt4MuonNonInteractingTruth","AntiKt6MuonNonInteractingTruth","AntiKtZ4Track" ]
  
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4Truth->AntiKt4TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4MuonNonInteractingTruth->AntiKt4TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt6MuonNonInteractingTruth->AntiKt6TopoEM" ]
BTaggingFlags.CalibrationChannelAliases+=[ "AntiKtZ4Track->AntiKt4TopoEM" ]

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
    BTaggingFlags.writeSecondaryVertices=True
    BTaggingFlags.SecondSoftMu=False
    BTaggingFlags.IP1D = True
    BTaggingFlags.IP2DFlip=True
    BTaggingFlags.IP2DPos=True
    BTaggingFlags.IP2DNeg=True
    BTaggingFlags.IP2DSpc=True
    BTaggingFlags.IP2DSpcFlip=True
    BTaggingFlags.IP2DSpcPos=True
    BTaggingFlags.IP2DSpcNeg=True
    BTaggingFlags.IP3DFlip=True
    BTaggingFlags.IP3DPos=True
    BTaggingFlags.IP3DNeg=True
    BTaggingFlags.IP3DSpc=True
    BTaggingFlags.IP3DSpcFlip=True
    BTaggingFlags.IP3DSpcPos=True
    BTaggingFlags.IP3DSpcNeg=True
    BTaggingFlags.SV1Flip =True
    BTaggingFlags.SV2Flip=True
    BTaggingFlags.JetProbFlip=True
    BTaggingFlags.SoftEl=True
    BTaggingFlags.JetFitterNNFlip=True
    BTaggingFlags.TrackCounting=True
    BTaggingFlags.TrackCountingFlip=True

    include( "BTagging/BTagging_LoadTools.py" )
    include( "BTagging/BTagging_jobOptions.py" )

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

JetTagD3PDFlags.JetCollections=['AntiKt4TopoEMJets',
                                'AntiKt6TopoEMJets']


from RecExConfig.RecFlags import rec
if rec.doTruth:
    JetTagD3PDFlags.JetCollections+=['AntiKt4TruthJets']
    JetTagD3PDFlags.D3PDPropDict().update(AntiKt4TruthJetsAOD_level=0)

JetTagD3PDFlags.NtupleType = ""
### hack to make it work until remi fixes the label in BuildVertexD3PDObject
JetTagD3PDKeys.PrimaryVertexGetterLabel = JetTagD3PDKeys.PrimaryVertexPrefix()

if rec.readAOD:
  JetTagD3PDFlags.AddPixelClusterObject=False

if rec.readAOD:
  JetTagD3PDFlags.AddPixCond=False

if JetTagD3PDFlags.AddPixCond():
  if not 'InDetPixelConditionsSummarySvc' in dir() and InDetRecTestBLayerTool==None:
    include("JetTagD3PDMaker/JetTagD3PD_LoadPixelCondServices.py")

JetTagD3PDFlags.SkimD3PD = False

# this must be done before importing JetD3PDObject ...
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDMakerFlags
jobproperties.JetTagD3PDMakerFlags.Taggers = ["IP2D", "IP2DNeg","IP2DFlip","IP2DPos",
         "IP2DSpc","IP2DSpcFlip","IP2DSpcPos","IP2DSpcNeg",
         "IP3D","IP3DNeg","IP3DFlip","IP3DPos",
         "IP3DSpc","IP3DSpcFlip","IP3DSpcPos","IP3DSpcNeg",
         "SV0", 
         "SV1", "SV1Flip",
         "SV2", "SV2Flip",
         "JetProb", "JetProbNeg", 
         "TrackCounting2D", "TrackCounting2DNeg",
         "LifetimeTag2D", 
         "SecVtxTagBU", 
         "lhSig",
         "SoftMuonTag", "SoftMuonTagChi2", 
         "SoftElectronTag",
         "JetFitterTagNN", "JetFitterTagNNFlip",
         "JetFitterCOMBNN", "JetFitterCOMBNNIP3DNeg", "JetFitterCOMBNNIP3DPos",
         "GbbNN"
         ]


# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
TrigBjetD3PDStream_msg = logging.getLogger( 'TrigBJetD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteTrigBJetD3PD.isVirtual:
    TrigBjetD3PDStream_msg.error("The TrigBJet D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "TrigBJet D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SUSY D3PD:
streamName = prodFlags.WriteTrigBJetD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteTrigBJetD3PD )
TrigBjetD3PDStream_msg.info( "Configuring TrigBJetD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
TrigBjetD3PDStream = MSMgr.NewRootStream( streamName, fileName, "TrigBjet" )
TrigBjetTriggerD3PDStream = MSMgr.NewRootStream( streamName + ":" + streamName +
                                                "TrigDec", fileName,
                                                "TrigBjetTrigDec" )
TrigBjetD3PDStream.Stream.trigDecisionTree = TrigBjetTriggerD3PDStream

# FIXME: Need to be able to change these in a more controlled manner.
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
TrackD3PDFlags.storeVertexTrackAssociation = True
# This makes it possible to use this version of the package in older
# releases as well:
if hasattr( TrackD3PDFlags, 'storeDetailedTrackPredictionAtBLayer' ):
    TrackD3PDFlags.storeDetailedTrackPredictionAtBLayer = True

# FIXME: When run from AODFix, METRef can use this, but not configure it.
# Patch it up so we don't get errors. [cf. bug 78054]
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, 'InDetDetailedTrackSelectorTool'):
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import\
         InDet__InDetDetailedTrackSelectorTool
    InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(\
        name = "InDetDetailedTrackSelectorTool",
        TrackSummaryTool = 'AtlasTrackSummaryTool')
    ToolSvc += InDetTrackSelectorTool

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject

from JetD3PDMaker.JetD3PDObject                        import JetD3PDObject

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject

from TriggerD3PDMaker.defineTriggerBits                import defineTriggerBits
from TrigMuonD3PDMaker.TrigMuonD3PD                    import TrigMuonD3PDObjects
from TrigBJetD3PDMaker.TrigBJetD3PD                    import TrigBJetD3PDObjects

from RecExConfig.RecFlags                              import rec

from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from TrigBJetD3PDMaker.TrigBJetD3PDFlags import trigbjetD3PDFlags

if trigbjetD3PDFlags.ApplyTriggerSkimming() :
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

    # Now, define the trigger filters
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    runningIndex = 0
    TriggerNames = {}
    globalTriggerPrescale = trigbjetD3PDFlags.GlobalTriggerPrescale

    for k, v in trigbjetD3PDFlags.BJetSliceTriggerNames.iteritems():
       TriggerNames[k] = v
       pass
    for trigName in TriggerNames.keys() :
        runningIndex += 1
        TriggerFilterName = "TrigBjetD3PDStream_TriggerFilter_"+trigName
        topSequence += TriggerFilter( TriggerFilterName,
                                      trigger = trigName )
        if not trigbjetD3PDFlags.ApplyTriggerPrescales() :
            TrigBjetD3PDStream.AddAcceptAlgs(TriggerFilterName)
            TrigBjetTriggerD3PDStream.AddAcceptAlgs(TriggerFilterName)
            pass
        else :
            prescale = globalTriggerPrescale*TriggerNames[trigName] if TriggerNames[trigName]>0 else 1
            TriggerPrescaleName = "TrigBjetD3PDStream_Prescale_"+trigName
            topSequence += PrimaryDPDPrescaler(TriggerPrescaleName,
                                               AcceptAlgs = [TriggerFilterName],
                                               Prescale   =  prescale)
            TrigBjetD3PDStream.AddAcceptAlgs(TriggerPrescaleName)
            TrigBjetTriggerD3PDStream.AddAcceptAlgs(TriggerPrescaleName)
            pass
        pass
    pass

TrigBjetD3PDStream += EventInfoD3PDObject( 10 ) 

# Offline muons
TrigBjetD3PDStream += MuonD3PDObject( 10, sgkey='StacoMuonCollection',
                                      prefix='mu_staco_' )

# Jet blocks    
TrigBjetD3PDStream += JetD3PDObject( 9, sgkey='AntiKt4TopoEMJets',
                                     prefix='jet_antikt4topoemjets_',
                                     exclude = ['Constituents'],
                                     allowMissing = True )

TrigBjetD3PDStream += JetD3PDObject( 9, sgkey='AntiKt6TopoEMJets',
                                     prefix='jet_antikt6topoemjets_',
                                     exclude = ['Constituents'],
                                     allowMissing = True )

# Primary vertex block - May be missing in single-beam data.    
TrigBjetD3PDStream += PrimaryVertexD3PDObject( 1, allowMissing = True,
                                               sgkey = D3PDMakerFlags.VertexSGKey(),
                                               prefix = 'vxp_' )

# Truth
if rec.doTruth():
    from MuonD3PDMaker.TruthMuonD3PDObject import TruthMuonD3PDObject
    TrigBjetD3PDStream += TruthMuonD3PDObject( 2 )
    TrigBjetD3PDStream += JetD3PDObject( 3, sgkey='AntiKt4TruthJets', # this had AOD in the end
                                         prefix='jet_antikt4truth_',
                                         allowMissing = True )
    pass

# Trigger
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
InitTriggerBitsObject = make_Void_D3PDObject( default_name = 'InitTriggerBits' )
defineTriggerBits(InitTriggerBitsObject, 0, 'L1_.*')
defineTriggerBits(InitTriggerBitsObject, 0, 'L2_.*')
defineTriggerBits(InitTriggerBitsObject, 0, 'EF_.*')

TrigBjetD3PDStream += InitTriggerBitsObject(10)

# commented for tests
TrigMuonD3PDObjects( TrigBjetD3PDStream, level = 10, addNaviInfo = True, addConfigInfo = True)
TrigBJetD3PDObjects( TrigBjetD3PDStream, level = 10, doAddL2ID = True, doAddEFID = True,
                     addNaviInfo = True, addConfigInfo = True, addL1Jet = True, addOffTrack = True )


# in the bjet object
#from TrigJetD3PDMaker.TrigJetD3PD import TrigJetD3PDObjects
#TrigJetD3PDObjects(TrigBjetD3PDStream, level = 10)

from TriggerD3PDMaker.JetETROID3PDObject        import JetETROID3PDObject
from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject

TrigBjetD3PDStream += jetMETTriggerBitsD3PDObject(10)
TrigBjetD3PDStream += JetETROID3PDObject (10)

# The L2 JE information:
from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject
TrigBjetD3PDStream += TrigMETD3PDObject(level = 10, prefix = "trig_L2_je_",
                                        sgkey = "HLT_L2JetEtSum", allowMissing = True )


# truth information
if rec.doTruth():
  from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
  TrigBjetD3PDStream += GenEventD3PDObject( 10 )

  from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
  TrigBjetD3PDStream += GenVertexD3PDObject( 10 )

  from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
  TrigBjetD3PDStream += GenParticleD3PDObject( 10 )


# Event metadata
TrigBjetD3PDStream.MetadataTools += [ LBMetadataConfig() ]

print TrigBjetD3PDStream

