# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# GSF Track Fex Algorithm Configuration:
# taken from egammaTools
# Ryan Mackenzie White <ryan.white@cern.ch> 
##############################

from TrigEgammaHypo import TrigEgammaHypoConf
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags  import globalflags
from InDetRecExample.InDetJobProperties import InDetFlags

from egammaRec.Factories import ToolFactory

from egammaTools.InDetTools import egammaExtrapolator
# do the configuration
import egammaRec.EMCommonRefitter
GSFBuildInDetExtrapolator= egammaExtrapolator()

# EM extrapolator
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools

from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
GSFRefitterTool = egammaTrkRefitterTool(name = 'GSFRefitterTool',
                                        FitterTool = egammaRec.EMCommonRefitter.GSFTrackFitter,
                                        useBeamSpot = False,
                                        ReintegrateOutliers=True,
                                        OutputLevel =7)
ToolSvc += GSFRefitterTool

# ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
#
from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
GSFBuildInDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "GSFBuildInDetPrdAssociationTool",
                                                                        PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')
ToolSvc += GSFBuildInDetPrdAssociationTool        
#
# ----------- Load SummaryTool
#
# Loading Configurable HoleSearchTool
#
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
GSFBuildHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "GSFBuildHoleSearchTool",
                                                         Extrapolator = GSFBuildInDetExtrapolator,
                                                         usePixel      = DetFlags.haveRIO.pixel_on(),
                                                         useSCT        = DetFlags.haveRIO.SCT_on(),
                                                         CountDeadModulesAfterLastHit = True)

from AthenaCommon.AppMgr import ServiceMgr
if (DetFlags.haveRIO.SCT_on()):
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
    InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
    ServiceMgr += InDetSCT_ConditionsSummarySvc
    GSFBuildHoleSearchTool.SctSummarySvc = ServiceMgr.InDetSCT_ConditionsSummarySvc
else:
    GSFBuildHoleSearchTool.SctSummarySvc = None
    
ToolSvc += GSFBuildHoleSearchTool
print    GSFBuildHoleSearchTool
#
# Load BLayer tool
#
GSFBuildTestBLayerTool = None
if DetFlags.haveRIO.pixel_on() :
    from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
    ServiceMgr += PixelConditionsSummarySvc()            
    GSFBuildTestBLayerTool = InDet__InDetTestBLayerTool(name            = "GSFBuildTestBLayerTool",
                                                        PixelSummarySvc = ServiceMgr.PixelConditionsSummarySvc,
                                                        Extrapolator    = GSFBuildInDetExtrapolator)
    ToolSvc += GSFBuildTestBLayerTool
    print  GSFBuildTestBLayerTool

# Configurable version of TRT_ElectronPidTools
#
GSFBuildTRT_ElectronPidTool = None
if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC() and not InDetFlags.doHighPileup() :
    
    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
    GSFBuildTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(name ="TrigGSF_InDet_TRT_LocalOccupancy")
    ToolSvc += GSFBuildTRT_LocalOccupancy
    print GSFBuildTRT_LocalOccupancy
        
    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
    GSFBuildTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2(name   = "TrigGSF_GSFBuildTRT_ElectronPidTool",
                                                                 TRT_LocalOccupancyTool = GSFBuildTRT_LocalOccupancy,
                                                                 isData = (globalflags.DataSource == 'data') )

    ToolSvc += GSFBuildTRT_ElectronPidTool
    print GSFBuildTRT_ElectronPidTool

#
# Configurable version of PixelToTPIDTOol
#
GSFBuildPixelToTPIDTool = None
if DetFlags.haveRIO.pixel_on():                 
    from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
    GSFBuildPixelToTPIDTool = InDet__PixelToTPIDTool(name = "TrigGSFBuildPixelToTPIDTool")
    GSFBuildPixelToTPIDTool.ReadFromCOOL = True
    ToolSvc += GSFBuildPixelToTPIDTool
    print  GSFBuildPixelToTPIDTool
#
# Configrable version of loading the InDetTrackSummaryHelperTool
#
from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
GSFBuildTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "TrigGSF_GSFBuildTrackSummaryHelperTool",
                                                                     AssoTool        = GSFBuildInDetPrdAssociationTool,
                                                                     PixelToTPIDTool = GSFBuildPixelToTPIDTool,
                                                                     TestBLayerTool  = GSFBuildTestBLayerTool,
                                                                     DoSharedHits    = False,
                                                                     HoleSearch      = GSFBuildHoleSearchTool,
                                                                     usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                     useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                     useTRT          = DetFlags.haveRIO.TRT_on())
ToolSvc += GSFBuildTrackSummaryHelperTool
print      GSFBuildTrackSummaryHelperTool
#
# Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
#
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
GSFBuildInDetTrackSummaryTool = Trk__TrackSummaryTool(name = "TrigGSF_GSFBuildInDetTrackSummaryTool",
                                                      InDetSummaryHelperTool = GSFBuildTrackSummaryHelperTool,
                                                      doSharedHits           = False,
                                                      InDetHoleSearchTool    = GSFBuildHoleSearchTool,
                                                      TRT_ElectronPidTool    = GSFBuildTRT_ElectronPidTool,
                                                      PixelToTPIDTool        = GSFBuildPixelToTPIDTool)
ToolSvc += GSFBuildInDetTrackSummaryTool
print      GSFBuildInDetTrackSummaryTool
#
# --- load patricle creator tool
#
from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
GSFBuildInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(name                    = "TrigGSF_GSFBuildInDetParticleCreatorTool",
                                                                 KeepParameters          = True,
                                                                 Extrapolator            = GSFBuildInDetExtrapolator,
                                                                 TrackSummaryTool        = GSFBuildInDetTrackSummaryTool,
                                                                 UseTrackSummaryTool     = False,
                                                                 ForceTrackSummaryUpdate = False)
# Otherwise Tracks and CombinedInDetTracks will be different when slimming

ToolSvc += GSFBuildInDetParticleCreatorTool
print GSFBuildInDetParticleCreatorTool
#
# --- do track slimming
#
from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
GSFBuildInDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name  = "TrigGSF_GSFBuildInDetTrackSlimmingTool",
                                                             KeepParameters = True,
                                                             KeepOutliers   = True )
ToolSvc += GSFBuildInDetTrkSlimmingTool
print GSFBuildInDetTrkSlimmingTool

# ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
GSFBuildInDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "TrigGSF_GSFBuildInDetPrdAssociationTool",PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')
ToolSvc += GSFBuildInDetPrdAssociationTool  
# ----------- Load SummaryTool
# Loading Configurable HoleSearchTool

# Add the monitoring class
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigGSFTrackFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigGSFTrackFexOnlineMonitoring"):
        super(TrigGSFTrackFexOnlineMonitoring, self).__init__(name)
        self.defineTarget( "Online" )
        self.Histograms += [ defineHistogram('nClusters', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nSelTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nFailedFitTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nFailedSiTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nRefitTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]

class TrigGSFTrackFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigGSFTrackFexValidationMonitoring"):
        super(TrigGSFTrackFexValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )
        self.Histograms += [ defineHistogram('nClusters', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nSelTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nFailedFitTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nFailedSiTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nRefitTracks', type='TH1I', title="GSF Track Fex Tracks;nTracks;Nevents", xbins=50, xmin=0, xmax=50) ]

# postinit monitoring
def configureTrigGSFTrackFexMonitoring(tool):
    validation = TrigGSFTrackFexValidationMonitoring()
    online     = TrigGSFTrackFexOnlineMonitoring()
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig("Time")
    tool.AthenaMonTools = [ time, online, validation ]


TrigGSFTrackFex_Electron = ToolFactory(TrigEgammaHypoConf.TrigGSFTrackFex, name = "TrigGSFTrackFex_Electron",
        AcceptAll = True,
        TrackRefitTool= GSFRefitterTool,
        TrackParticleCreatorTool=GSFBuildInDetParticleCreatorTool,
        TrackSlimmingTool=GSFBuildInDetTrkSlimmingTool,
        TrackSummaryTool=GSFBuildInDetTrackSummaryTool,
        ExtrapolationTool = EMExtrapolationTools,
        # do the configuration (from old EMBremCollectionBuilderBase)
        minNoSiHits=4,
        broadDeltaEta=0.1,   # this is multiplied by 2 for the Candidate Match , so +- 0.2 in eta
        broadDeltaPhi=0.15,   # this is multiplied by 2 for the Candidate Match , so +- 0.3 in phi
        narrowDeltaEta=0.05, 
        #These have to be relaxed enough for the conversions
        narrowDeltaPhi=0.05,  
        narrowDeltaPhiBrem=0.20, #Dominated by the needs of assymetric conversions
        narrowDeltaPhiRescale=0.05,  
        narrowDeltaPhiRescaleBrem=0.1,
        postInit = [configureTrigGSFTrackFexMonitoring],
        )

