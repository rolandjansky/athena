# jobO fragment to build jets from TrackParticles

from AthenaCommon.AppMgr import ToolSvc

from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging    = True

# Special loose track selection for making jet
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
theBTagTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(
    name                 = "BTagTrackJetTrackSelectorTool",
    pTMin                = 500.,
    etaMax               = 9999.,
    nHitBLayer           = 0,
    nHitPix              = 0,
    nHitBLayerPlusPix    = 0,
    nHitSct              = 0,
    nHitSi               = 5,
    nHitTrt              = 0,
    IPd0Max              = 100.,  # d0 cut
    IPz0Max              = 100.,  # z0*sin(theta) cut
    z0Max                = 1000., # z0 cut
    fitChi2OnNdfMax      = 1000, 
    d0significanceMax    = -1.,
    z0significanceMax    = -1.,
    TrackSummaryTool     = None,
    OutputLevel          = 3)
ToolSvc += theBTagTrackSelectorTool

# JetFinder (Anti-Kt)
from JetRec.JetGetters import *
##theBTagFastJetKt = getFastKt(jetalgname = "BTagTrackJetFinder",
import JetRec.JetFinderConfig as thebtagJetFinder
theBTagFastJetKt = thebtagJetFinder.getFastKt(jetalgname = "BTagTrackJetFinder",
                     radius     = 0.4,
                     algorithm  = 'anti-kt',
                     flipvalues = 2)
ToolSvc += theBTagFastJetKt

# Setup clustering tool
from JetRecTools.JetRecToolsConf import JetTrackZClusterTool
BTagTrackZClusterTool = JetTrackZClusterTool(
    name                       = "BTagTrackZClusterTool",
    TrackJetMinMulti           = 2,
    TrackJetMinPt              = 2000.,
    UseVtxSeeding              = True,
    DeltaZRange                = 10000.0,
    TrackParticleContainerName = "TrackParticleCandidate",
    VxContainerName            = "VxPrimaryCandidate",
    TrackSelector              = theBTagTrackSelectorTool,
    JetFinder                  = theBTagFastJetKt,
    OutputLevel                = 3)

BTagToolList  = [ BTagTrackZClusterTool, JetSorterTool() ]
BTagToolList += getStandardBTaggerTool('AntiKtZ',0.4,'Track') 

my_att_dict = dict(jet_attribute_dict)
my_att_dict['_finderType'] = 'AntiKtZ'
my_att_dict['_finderParams'] = [0.4]
my_att_dict['_inputType'] = 'Track'
my_att_dict['_inputColl'] = []
BTagTrackJetAlg = make_customJetGetter(my_att_dict, BTagToolList).jetAlgorithmHandle() 

# Use the TrackParticle constituents instead of re-doing the TrackParticle-Jet Association
BTagTrackJetAlg.AlgTools['JetBTagger'].jetFinderBasedOn = "Tracks"
BTagTrackJetAlg.AlgTools['JetBTagger'].OutputLevel      = BTaggingFlags.OutputLevel

if BTaggingFlags.OutputLevel < 3:
  print BTagTrackJetAlg

