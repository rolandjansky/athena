import AthenaCommon.AtlasUnixStandardJob

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True
from AthenaCommon.AppMgr import ToolSvc
# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

DetDescrVersion="ATLAS-CSC-01-02-00"
include( "RecExCommon/AllDet_detDescr.py" )

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
svcMgr.PoolSvc.AttemptCatalogPatch = True
# The AOD input file
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

ServiceMgr.EventSelector.BackNavigation = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Number of Events to process
theApp.EvtMax = -1

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='TauTrackEff.root' OPT='RECREATE'" ]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO 

# Truth tau creator:
from TauTrackTools.TauTrackToolsConf import TauID__TruthTauCreatorAlg
truthTauCreatorAlg = TauID__TruthTauCreatorAlg()
topSequence += truthTauCreatorAlg
truthTauCreatorAlg.OutputLevel = INFO

# set up tracking geometry (needed for extrapolation)
include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
from __main__ import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc.BuildGeometryFromTagInfo = False
AtlasTrackingGeometrySvc.CallbackStringCheck = False
# ATLAS extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator()
ToolSvc+=theAtlasExtrapolator
# TrackToVertexIPEstimator (needed for impact parameter calculation)
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
trackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(Extrapolator = theAtlasExtrapolator)
ToolSvc += trackToVertexIPEstimator
# the track selector:
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
tauTrackEffTrackSelectorToolTauCuts = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolTauCuts",
                            pTMin                = 1000,
                            IPd0Max              = 1.5,
                            z0Max                = 1000,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 1,
                            nHitPix              = 2,
                            nHitSct              = 0,
                            nHitSi               = 6,
                            nHitTrt              = 0,
                            fitChi2OnNdfMax      = 3.5,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolTauCuts
print tauTrackEffTrackSelectorToolTauCuts

# Tau Track Efficiency Alg:
from TauTrackTools.TauTrackToolsConf import TauID__TauTrackEfficiencyAlg
tauTrackEffAlgTauCuts = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgTauCuts",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolTauCuts,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffTauCutsCalo" )
topSequence += tauTrackEffAlgTauCuts
tauTrackEffAlgTauCuts.OutputLevel = INFO 
print tauTrackEffAlgTauCuts

tauTrackEffTrackSelectorToolStandardCuts = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolStandardCuts",
                            pTMin                = 1000,
                            IPd0Max              = 2,
                            IPz0Max              = 10,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 0,
                            nHitPix              = 0,
                            nHitSct              = 0,
                            nHitSi               = 7,
                            nHitTrt              = 0,
                            fitChi2OnNdfMax      = 99999,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolStandardCuts
# Tau Track Efficiency Alg:
tauTrackEffAlgStandardCuts = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgStandardCuts",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolStandardCuts,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffStandardCuts" )
topSequence += tauTrackEffAlgStandardCuts

tauTrackEffTrackSelectorToolLeading = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolLeading",
                            pTMin                = 6000,
                            IPd0Max              = 1.5,
                            IPz0Max              = 10000,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 1,
                            nHitPix              = 2,
                            nHitSct              = 0,
                            nHitSi               = 6,
                            nHitTrtPlusOutliers  = 10,
                            TrtMaxEtaAcceptance  = 1.9,
                            fitChi2OnNdfMax      = 3.5,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolLeading
# Tau Track Efficiency Alg:
tauTrackEffAlgLeading = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgLeading",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolLeading,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffLeading" )
topSequence += tauTrackEffAlgLeading


tauTrackEffTrackSelectorToolSecond = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolSecond",
                            pTMin                = 1000,
                            IPd0Max              = 1,
                            IPz0Max              = 10000,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 1,
                            nHitPix              = 0,
                            nHitSct              = 0,
                            nHitSi               = 8,
                            nHitTrt              = 0,
                            fitChi2OnNdfMax      = 1.7,
                            nHitTrtHighEFractionWithOutliersMax = 0.2,
                            TrtMaxEtaAcceptance = 1.9,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolSecond
# Tau Track Efficiency Alg:
tauTrackEffAlgSecond = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgSecond",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolSecond,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffSecond" )
topSequence += tauTrackEffAlgSecond

tauTrackEffTrackSelectorToolThird = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolThird",
                            pTMin                = 1000,
                            IPd0Max              = 1,
                            IPz0Max              = 10000,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 0,
                            nHitPix              = 0,
                            nHitSct              = 0,
                            nHitSi               = 8,
                            nHitTrt              = 0,
                            useTrackQualityInfo  = True,
                            fitChi2OnNdfMax      = 99999,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolThird
# Tau Track Efficiency Alg:
tauTrackEffAlgThird = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgThird",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolThird,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffThird" )
topSequence += tauTrackEffAlgThird

tauTrackEffTrackSelectorToolTauCutsNoChi2 = InDet__InDetDetailedTrackSelectorTool( "TauTrackEffTrackSelectorToolTauCutsNoChi2",
                            pTMin                = 1000,
                            IPd0Max              = 1.5,
                            z0Max                = 1000,
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 1,
                            nHitPix              = 2,
                            nHitSct              = 0,
                            nHitSi               = 6,
                            nHitTrt              = 0,
                            fitChi2OnNdfMax      = 99999.,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
ToolSvc += tauTrackEffTrackSelectorToolTauCutsNoChi2
tauTrackEffAlgTauCutsNoChi2 = TauID__TauTrackEfficiencyAlg( "TauTrackEfficiencyAlgTauCutsNoChi2",
                            TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                            TrackSelectorTool           = tauTrackEffTrackSelectorToolTauCutsNoChi2,
                            DRBins = 60,
                            PtBins = 25,
                            PtMax = 250,
                            PtVisBins = 20,
                            PtVisMax = 500,
                            EtaBins = 10,
                            RootTreeName = "/AANT/TauTrackEffTauCutsCaloNoChi2" )
topSequence += tauTrackEffAlgTauCutsNoChi2

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 
#MessageSvc.OutputLevel = DEBUG

