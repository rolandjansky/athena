

from JetRecTools.JetRecToolsConf import JetTrackZClusterTool
from JetRec.JetRecConf import JetFastJetFinderTool
from JetRec.JetRecFlags import jetFlags
from JetRec.JetGetters import *

#------------------------------------------------------------------------

# Setup tool so that it can be used

#------------------------------------------------------------------------

JetTrackZClusterTool_Z4 = JetTrackZClusterTool( "JetTrackZClusterTool_Z4" )
JetTrackZClusterTool_Z4.TrackJetMinMulti = 2
JetTrackZClusterTool_Z4.TrackJetMinPt = 5000 # MeV

JetTrackZClusterTool_Z4.UseVtxSeeding = True
JetTrackZClusterTool_Z4.DeltaZRange = 10000.0
JetTrackZClusterTool_Z4.TrackParticleContainerName = "TrackParticleCandidate"
JetTrackZClusterTool_Z4.VxContainerName = "VxPrimaryCandidate"
JetTrackZClusterTool_Z4.OutputLevel = INFO
#--------------------------------------------------------------

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
#--------------------------------------------------------------

ToolSvc += InDet__InDetDetailedTrackSelectorTool( "InDetDetailedTrackSelectorTool_Z4")
ToolSvc.InDetDetailedTrackSelectorTool_Z4.pTMin                = 500 # MeV

ToolSvc.InDetDetailedTrackSelectorTool_Z4.etaMax               = 2.5
ToolSvc.InDetDetailedTrackSelectorTool_Z4.nHitBLayer           = 0
ToolSvc.InDetDetailedTrackSelectorTool_Z4.nHitPix              = 1
ToolSvc.InDetDetailedTrackSelectorTool_Z4.nHitSct              = 6
ToolSvc.InDetDetailedTrackSelectorTool_Z4.nHitSi               = 7 #7

ToolSvc.InDetDetailedTrackSelectorTool_Z4.nHitTrt              = 0
ToolSvc.InDetDetailedTrackSelectorTool_Z4.IPd0Max              = 1.5 # d0 cut

ToolSvc.InDetDetailedTrackSelectorTool_Z4.IPz0Max              = 1.5 # z0*sin(theta) cut

ToolSvc.InDetDetailedTrackSelectorTool_Z4.z0Max                = 200 # z0 cut

ToolSvc.InDetDetailedTrackSelectorTool_Z4.fitChi2OnNdfMax      = 10000 #3.5

ToolSvc.InDetDetailedTrackSelectorTool_Z4.d0significanceMax    = -1.
ToolSvc.InDetDetailedTrackSelectorTool_Z4.z0significanceMax    = -1.
ToolSvc.InDetDetailedTrackSelectorTool_Z4.OutputLevel = INFO


JetTrackZClusterTool_Z4.TrackSelector = ToolSvc.InDetDetailedTrackSelectorTool_Z4 # 2 tracks threshold at 500MeV

ToolSvc.InDetDetailedTrackSelectorTool_Z4.TrackSummaryTool     =    InDetTrackSummaryTool
ToolSvc.InDetDetailedTrackSelectorTool_Z4.Extrapolator         =    InDetExtrapolator
#--------------------------------------------------------------

#--------------------------------------------------------------

JetFastJetFinderToolAntiKt_Z4 = JetFastJetFinderTool("JetFastJetFinderToolAntiKt_Z4")
JetFastJetFinderToolAntiKt_Z4.Algorithm    = "anti-kt"
JetFastJetFinderToolAntiKt_Z4.Radius       = 0.4
JetFastJetFinderToolAntiKt_Z4.RecombScheme = "E"
JetFastJetFinderToolAntiKt_Z4.Strategy     = "Best"
JetFastJetFinderToolAntiKt_Z4.FailIfMisconfigured = True
JetFastJetFinderToolAntiKt_Z4.Inclusive = True
JetFastJetFinderToolAntiKt_Z4.CalculateJetArea = False
JetFastJetFinderToolAntiKt_Z4.StoreNFlipValues = 2
ToolSvc += JetFastJetFinderToolAntiKt_Z4
JetTrackZClusterTool_Z4.JetFinder = JetFastJetFinderToolAntiKt_Z4


#BTagToolList  = [ JetTrackZClusterTool_Z4, JetSorterTool() ]
#BTagToolList += getStandardBTaggerTool('AntiKtZ',0.4,'Track')
#--------------------------------------------------------------

my_att_dict = dict(jet_attribute_dict)
# Fill the attribute dictionary

my_att_dict['_finderType'] = 'AntiKtZ'
my_att_dict['_finderParams'] = [0.4]
my_att_dict['_inputType'] = 'Track'
my_att_dict['_inputColl'] = []
#jetRec_Z4 = make_customJetGetter(my_att_dict, BTagToolList).jetAlgorithmHandle()
jetRec_Z4 = make_customJetGetter(my_att_dict, [JetTrackZClusterTool_Z4,JetSorterTool()]).jetAlgorithmHandle()



