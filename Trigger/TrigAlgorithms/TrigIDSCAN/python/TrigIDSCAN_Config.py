# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigIDSCAN.TrigIDSCANConf import TrigIDSCAN
from TrigIDSCAN.TrigIDSCANConf import ShifterTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointTool_Config import ConfiguredOnlineSpacePointProviderTool
from IDScanZFinder.IDScanZFinderConf import IDScanZFinder
from IDScanHitFilter.IDScanHitFilterConf import IDScanHitFilter
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionToolConf import TrigTRT_TrackExtensionTool
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionTool_Config import TrigTRT_TrackExtensionTool_eGamma
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetTrackFitter
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2ResidualCalculator

ospTool = ConfiguredOnlineSpacePointProviderTool()
trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
trackFitter = TrigInDetTrackFitter(OfflineClusters=False)
resCalc = TrigL2ResidualCalculator(OfflineClusters=False)

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import L2IDTrackingCuts
RegSelZmax = L2IDTrackingCuts.RegSelZmax()

# from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ToolSvc

ToolSvc += ospTool
ToolSvc += trtTool
ToolSvc += trackFitter
ToolSvc += resCalc


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

#Monitoring
class TrigIDSCAN_CommonMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigIDSCAN_OnlineMonitoring"):
        super(TrigIDSCAN_CommonMonitoring, self).__init__(name)
    def addSPHistograms(self):
        self.Histograms += [ defineHistogram('roi_nSPsPIX',
                                             type='TH1F',
                                             title="Number of Pixel SPs",
                                             xbins = 600, xmin=-0.5, xmax=599.5)]
        self.Histograms += [ defineHistogram('roi_nSPsSCT',
                                             type='TH1F',
                                             title="Number of SCT SPs",
                                             xbins = 400, xmin=-0.5, xmax=399.5)]
        self.Histograms += [ defineHistogram('roi_eta',
                                             type='TH1F',
                                             title="Eta of the input RoI",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('roi_phi',
                                             type='TH1F',
                                             title="Phi of the input RoI",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
    def addDataErrorHistograms(self):
        self.Histograms += [ defineHistogram('roi_lastStageExecuted',
                                             type='TH1F',
                                             title="Last Step Successfully Executed",
                                             xbins = 8 , xmin=-0.5, xmax=7.5,
                             labels='Start : GetRoI : GetSPs : MissingLayers : ZFinder : Groups : TrackFit : Extend2TRT' ) ]
        self.Histograms += [ defineHistogram('dataErrors',
                                             type='TH1F',
                                             title="Data Errors",
                                             xbins = 5 , xmin=0., xmax=5.,
                             labels='SP_Abort : SP_Recoverable : TRT_Abort : TRT_Recoverable' ) ]
        # SCT Errors are defined in : InnerDetector/InDetConditions/SCT_ConditionsServices/SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('dataErrors_SCT',
                                             type='TH1F',
                                             title="SCT Data Errors",
                                             xbins = 15 , xmin=0., xmax=15,
                             labels='SCT_Decoding : SCT_TimeOut : SCT_BCID : SCT_LVL1ID : SCT_Preamble : SCT_Formatter : SCT_Trailer : SCT_TrailerOverFlow : SCT_HeaderTrailerLimit : SCT_ABCD : SCT_Raw : SCT_MaskedLink : SCT_RODClockError : SCT_TruncatedROD' ) ]
        # PIX Errors are defined in : InnerDetector/InDetConditions/PixelConditionsServices/PixelConditionsServices/IPixelByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('dataErrors_PIX',
                                             type='TH1F',
                                             title="Pixel Data Errors",
                                             xbins = 10 , xmin=0., xmax=10.,
                             labels='PIX_TimeOut : PIX_BCID : PIX_LVL1ID : PIX_Preamble : PIX_Trailer : PIX_Flagged : PIX_DisabledFE : PIX_TruncatedROD : PIX_Decoding')]
        # TRT Errors are defined in : InnerDetector/InDetConditions/TRT_ConditionsServices/src/TRT_ByteStream_ConditionsSvc.cxx
        self.Histograms += [ defineHistogram('dataErrors_TRT',
                                             type='TH1F',
                                             title="TRT Data Errors",
                                             xbins = 10 , xmin=0., xmax=10.,
                             labels='TRT_LVL1ID : TRT_BCID : TRT_MISSING : TRT_ERROR : TRT_SID : TRT_ROB')]
    def addSiTimingHistograms(self,nbin,min,max):
        self.Histograms += [ defineHistogram('time_SiDataRegSel',
                                             type='TH1F',
                                             title="RegSel Time Pixel and SCT",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_SiDataTotal',
                                             type='TH1F',
                                             title="SiDataPreparation Total",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_PIXSPFormation',
                                             type='TH1F',
                                             title="SP formation time Pixel",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_SCTSPFormation',
                                             type='TH1F',
                                             title="SP formation time SCT",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_PIXBStoClusters',
                                             type='TH1F',
                                             title="BS-to-Clusters Pixel",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_SCTBStoClusters',
                                             type='TH1F',
                                             title="BS-to-Clusters SCT",
                                             xbins = nbin , xmin=min, xmax=max)]
    def addTRTTimingHistograms(self,nbin,min,max):
        self.Histograms += [ defineHistogram('time_TRTDataPrep',
                                             type='TH1F',
                                             title="TRT DataPrep total time",
                                             xbins = nbin, xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTBStoDC',
                                             type='TH1F',
                                             title="BS-to-DriftCircles TRT",
                                             xbins = nbin, xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTRobProv',
                                             type='TH1F',
                                             title="RobDataProvider time TRT",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTDataTotal',
                                             type='TH1F',
                                             title="TRT DataProvider Time",
                                             xbins = nbin, xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTRobPreload',
                                             type='TH1F',
                                             title="TRT RobPreloader Time",
                                             xbins = nbin, xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTRoads',
                                             type='TH1F',
                                             title="TRT Roads",
                                             xbins = nbin , xmin=min, xmax=max)]
        self.Histograms += [ defineHistogram('time_TRTPDAF',
                                             type='TH1F',
                                             title="TRT PDAF Tracking",
                                             xbins = nbin , xmin=min, xmax=max)]
    def addTrackHistograms(self):
        self.Histograms += [ defineHistogram('trk_nSiHits',
                                             type='TH1F',
                                             title="Total number of Silicon Hits per Track",
                                             xbins = 10, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('trk_nPIXHits',
                                             type='TH1F',
                                             title="Number of Pixel Hits per Track",
                                             xbins = 10, xmin=-0.5, xmax=9.5)]
        self.Histograms += [ defineHistogram('trk_nSCTHits',
                                             type='TH1F',
                                             title="Number of SCT Hits per Track",
                                             xbins = 10, xmin=-0.5, xmax=9.5)]
        self.Histograms += [ defineHistogram('trk_nTRTHits',
                                             type='TH1F',
                                             title="Number of TRT Hits per Track",
                                             xbins = 100, xmin=-0.5, xmax=99.5)]
        self.Histograms += [ defineHistogram('trk_nTRTStraws',
                                             type='TH1F',
                                             title="NStraw_TRT",
                                             xbins = 100, xmin=-0.5, xmax=99.5)]
        self.Histograms += [ defineHistogram('trk_chi2dof',
                                             type='TH1F',
                                             title="ChiSqd / nDoF",
                                             xbins = 100, xmin=0.0, xmax=5)]
        self.Histograms += [ defineHistogram('trk_pt',
                                             type='TH1F',
                                             title="pt",
                                             xbins = 100, xmin=0.0, xmax=40000)]
        self.Histograms += [ defineHistogram('trk_phi0',
                                             type='TH1F',
                                             title="phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('trk_eta',
                                             type='TH1F',
                                             title="eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('trk_dPhi0',
                                             type='TH1F',
                                             title="dphi",
                                             xbins = 160, xmin=-0.8, xmax=0.8)]
        self.Histograms += [ defineHistogram('trk_dEta',
                                             type='TH1F',
                                             title="deta",
                                             xbins = 160, xmin=-0.8, xmax=0.8)]
    def addIDScanHistograms(self):
        self.Histograms += [ defineHistogram('roi_nZvertices',
                                             type='TH1F',
                                             title="ZFinder Number of Vertices",
                                             xbins = 6, xmin=-0.5, xmax=5.5)]
        self.Histograms += [ defineHistogram('roi_ZvrtxPos',
                                             type='TH1F',
                                             title="Z Position of Vertex",
                                             xbins = 100, xmin=-400., xmax=400.)]
        self.Histograms += [ defineHistogram('roi_SPxShift',
                                             type='TH1F',
                                             title="x-shift applied to SPs",
                                             xbins = 100, xmin=-1000., xmax=1000.)]

    def addResidualHistograms(self):
        self.Histograms += [ defineHistogram('hit_PIXBarrelPhiResidual',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelEtaResidual',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelPhiPull',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track phi pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelEtaPull',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track eta pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_SCTBarrelResidual',
                                             type='TH1F',
                                             title="SCT Barrel hit-track residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTBarrelPull',
                                             type='TH1F',
                                             title="SCT Barrel hit-track pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapPhiResidual',
                                             type='TH1F',
                                             title="Pixel EC hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapEtaResidual',
                                             type='TH1F',
                                             title="Pixel EC hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapPhiPull',
                                             type='TH1F',
                                             title="Pixel EC hit-track phi pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapEtaPull',
                                             type='TH1F',
                                             title="Pixel EC hit-track eta pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_SCTEndCapResidual',
                                             type='TH1F',
                                             title="SCT EC hit-track residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_SCTEndCapPull',
                                             type='TH1F',
                                             title="SCT EC hit-track pull",
                                             xbins = 100, xmin=-5., xmax=5.)]

class TrigIDSCAN_OnlineMonitoring(TrigIDSCAN_CommonMonitoring):
    def __init__ (self, name="TrigIDSCAN_OnlineMonitoring", resHistos=False):
        super(TrigIDSCAN_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        # self.addSiTimingHistograms(250,0.,50.)
        # self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()
        self.addIDScanHistograms()
        if resHistos :
            self.addResidualHistograms()
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 200, xmin=-10, xmax=10)]
        self.Histograms += [ defineHistogram('trk_a0beam',
                                             type='TH1F',
                                             title="a0beam",
                                             xbins = 200, xmin=-10, xmax=10)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 200, xmin=-400, xmax=400)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=-0.5, xmax=99.5)]


class TrigIDSCAN_ValidationMonitoring(TrigIDSCAN_CommonMonitoring):
    def __init__ (self, name="TrigIDSCAN_ValidationMonitoring", resHistos=False):
        super(TrigIDSCAN_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addSiTimingHistograms(250,0.,50.)
        self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()
        self.addIDScanHistograms()
        if resHistos :
            self.addResidualHistograms()
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 200, xmin=-10, xmax=10)]
        self.Histograms += [ defineHistogram('trk_a0beam',
                                             type='TH1F',
                                             title="a0beam",
                                             xbins = 200, xmin=-10, xmax=10)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 200, xmin=-400, xmax=400)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=-0.5, xmax=99.5)]


#Cosmic Monitoring
class TrigIDSCAN_Cosmic_Monitoring(TrigIDSCAN_CommonMonitoring):
    def __init__ (self, name="TrigIDSCAN_Cosmic_Monitoring", resHistos=False):
        super(TrigIDSCAN_Cosmic_Monitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addSiTimingHistograms(250,0.,50.)
        self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()
        self.addIDScanHistograms()
        if resHistos :
            self.addResidualHistograms()
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 100, xmin=-300, xmax=300)]
        self.Histograms += [ defineHistogram('trk_a0beam',
                                             type='TH1F',
                                             title="a0beam",
                                             xbins = 100, xmin=-300, xmax=300)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 100, xmin=-800, xmax=800)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=-0.5, xmax=99.5)]


class TrigIDSCAN_eGamma(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_eGamma"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.1
        self.etaHalfWidth = 0.1
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
#       self.useRoiDescriptorWidths = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_eGamma", NumberOfPeaks = 4,
                                    PhiBinSize = 0.1, UseOnlyPixels = False, MinZBinSize = 0.3,
                                    ZBinSizeEtaCoeff = 0.15, DPhiDEta = -0.035, ChargeAware = True,
                                    ZHistoPerPhi = False, NeighborMultiplier = 1, nVrtxSeparation = 5,
                                    nFirstLayers = 4, TripletMode = 1,
                                    VrtxMixing = 0, VrtxDistCut = 0.02, PreferCentralZ = True,
                                    MaxZ = RegSelZmax)
        zfinderTool.TripletDZ = 10
        zfinderTool.TripletDP = 0.05
        zfinderTool.TripletDK = 0.00001
        
        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_eGamma",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 0.5, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = False,
                                              d0Cut_loose = 10, d0Cut_tight = 10)

        hitfilterTool.dPhidRBinSize_tight = 0.00000300
        hitfilterTool.phi0Bins_tight = 16000

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]




class TrigIDSCAN_Muon(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Muon"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.1
        self.etaHalfWidth = 0.1
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.useRoiDescriptorWidths = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Muon", NumberOfPeaks = 4,
                                    PhiBinSize = 0.15, UseOnlyPixels = False, MinZBinSize = 0.3,
                                    ZBinSizeEtaCoeff = 0.15, DPhiDEta = -0.045, ChargeAware = True,
                                    ZHistoPerPhi = False, NeighborMultiplier = 1, nVrtxSeparation = 5,
                                    nFirstLayers = 4, TripletMode = 1,
                                    VrtxMixing = 0, VrtxDistCut = 0.1, PreferCentralZ = True,
                                    MaxZ = RegSelZmax)


        zfinderTool.TripletDZ = 15
        zfinderTool.TripletDP = 0.1
        zfinderTool.TripletDK = 0.0001
        
        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Muon",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                        PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                        LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                        RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = True,
                                        d0Cut_loose = 10, d0Cut_tight = 10)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = True
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]





class TrigIDSCAN_muonIso(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_muonIso"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.2
        self.etaHalfWidth = 0.2
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.useRoiDescriptorWidths = False
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_muonIso", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False,
                                    MaxZ = RegSelZmax)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_muonIso",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = True)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]






class TrigIDSCAN_Tau(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Tau"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.3
        self.etaHalfWidth = 0.3
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = False
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Tau", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False,
                                    MaxZ = RegSelZmax)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Tau",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = True)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,50]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]




class TrigIDSCAN_Jet(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Jet"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.2
        self.etaHalfWidth = 0.2
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = False
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Jet", NumberOfPeaks = 3,
                                    PhiBinSize = 0.8,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.1, DPhiDEta = -0.02, ChargeAware = False,
                                    ZHistoPerPhi = False, TripletMode = 1, NeighborMultiplier = 1,
				    VrtxDistCut = 0.01, TripletDZ = 6, TripletDK = 0.00005,
                                    TripletDP = 0.005, MaxZ = RegSelZmax)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Jet",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.0, EtaBinSize = 0.02, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = False)
        
        hitfilterTool.dPhidRBinSize_loose = 0.00000375
        hitfilterTool.phi0Bins_loose = 12000
        hitfilterTool.etaCutInner_tight = 0.005
        hitfilterTool.etaCutOuter_tight = 0.004
        hitfilterTool.d0Cut_tight = 0.4

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.etaCutInner_loose = 0.1
        hitfilterTool.etaCutOuter_loose = 0.05
        
        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,50]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]





class TrigIDSCAN_Bphysics(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Bphysics"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.75
        self.etaHalfWidth = 0.75
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.FindMultipleZ = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Bphysics",  NumberOfPeaks = 3,
                                    PhiBinSize = 1.0, UseOnlyPixels = False, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False, VrtxDistCut = 0.1, nVrtxSeparation = 20,
                                    MaxZ = RegSelZmax)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Bphysics",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                        PhiBinSize = 3.5, EtaBinSize = 0.02, LayerThreshold = 4.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = True)

        hitfilterTool.dPhidRBinSize_tight = 0.00000300
        hitfilterTool.phi0Bins_tight = 16000
        hitfilterTool.dPhidRBinSize_loose = 0.00000375
        hitfilterTool.phi0Bins_loose = 12000
        hitfilterTool.etaCutInner_tight = 0.04
        hitfilterTool.etaCutOuter_tight = 0.02
        hitfilterTool.d0Cut_tight = 4.0
        hitfilterTool.etaCutInner_loose = 0.1
        hitfilterTool.etaCutOuter_loose = 0.05

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,100]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]




class TrigIDSCAN_FullScan(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_FullScan"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.FullScanMode = True
        self.GenerateRoI = True
        self.phiHalfWidth = 3.14159265358
        self.etaHalfWidth = 3.0
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.FindMultipleZ = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DetMaskCheck = True
        self.DumpPrefix = "spacepoints"
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_FullScan", FullScanMode = self.FullScanMode,
                                     NumberOfPeaks = 3,
                                    PhiBinSize = 1.0,UseOnlyPixels = False, MinZBinSize = 0.4, # nominal = 0.4, tuned=0.5
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False, MaxZ = RegSelZmax, PreferCentralZ = True, VrtxDistCut = 0.1, nVrtxSeparation = 20)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_FullScan",  fullScan=self.FullScanMode,
                                              phiHalfWidth = self.phiHalfWidth,  etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 3.5, EtaBinSize = 0.02, LayerThreshold = 4.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = True)

        
        hitfilterTool.dPhidRBinSize_tight = 0.00000300
        hitfilterTool.phi0Bins_tight = 16000
        # hitfilterTool.deltadzdrCut_tight = 0.03
        hitfilterTool.etaCutInner_tight = 0.04
        hitfilterTool.etaCutOuter_tight = 0.02
        hitfilterTool.d0Cut_tight = 4.0
        
        # nominal = 4 and 0.05, tuned = 20 and 0.1
        hitfilterTool.d0Cut_loose = 4.0
        hitfilterTool.deltadzdrCut_loose = 0.05
        hitfilterTool.etaCutInner_loose = 0.1
        hitfilterTool.etaCutOuter_loose = 0.05
        hitfilterTool.dPhidRBinSize_loose = 0.00000375
        hitfilterTool.phi0Bins_loose = 12000

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,200]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]


class TrigIDSCAN_BeamSpot(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_BeamSpot"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.FullScanMode = True
        self.GenerateRoI = True
        self.phiHalfWidth = 3.14159265358
        self.etaHalfWidth = 3.0
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = False
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DetMaskCheck = True
        self.DumpPrefix = "spacepoints"
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_BeamSpot", FullScanMode = self.FullScanMode,
                                    NumberOfPeaks = 3,
                                    PhiBinSize = 1.0,UseOnlyPixels = False, MinZBinSize = 0.4, # nominal = 0.4, tuned=0.5
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False, MaxZ = RegSelZmax, PreferCentralZ = True)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_BeamSpot",  fullScan=self.FullScanMode,
                                              phiHalfWidth = self.phiHalfWidth,  etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 3.5, EtaBinSize = 0.02, LayerThreshold = 4.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = True)

        
        hitfilterTool.dPhidRBinSize_tight = 0.00000300
        hitfilterTool.phi0Bins_tight = 16000
        # hitfilterTool.deltadzdrCut_tight = 0.03
        hitfilterTool.etaCutInner_tight = 0.04
        hitfilterTool.etaCutOuter_tight = 0.02
        hitfilterTool.d0Cut_tight = 4.0
        
        # nominal = 4 and 0.05, tuned = 20 and 0.1
        hitfilterTool.d0Cut_loose = 4.0
        hitfilterTool.deltadzdrCut_loose = 0.05
        hitfilterTool.etaCutInner_loose = 0.1
        hitfilterTool.etaCutOuter_loose = 0.05
        hitfilterTool.dPhidRBinSize_loose = 0.00000375
        hitfilterTool.phi0Bins_loose = 12000

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        from TrigInDetTrackFitter.TrigInDetTrackFitter_Config import ConfiguredTrigL2_TrackFitter
        newTrackFitter = ConfiguredTrigL2_TrackFitter()
        ToolSvc += newTrackFitter
        self.TrigInDetTrackFitter = newTrackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,200]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",False),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",False),
                                timeHist ]




class TrigIDSCAN_Cosmics(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Cosmics"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.FullScanMode = True
        self.phiHalfWidth = 3.14159265358
        self.etaHalfWidth = 5.0
        self.MinHits = 3
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.doShift = True
        self.UseBeamSpot = False
        self.UseShifterZ = 2
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"

        configuredShifter = ShifterTool(name="ShifterTool_Cosmics",UsePixels=True)
        #configuredShifter.MaxPIXOccupancy = 30
        #configuredShifter.MaxSCTOccupancy = 30

        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += configuredShifter
        self.ShifterTool=configuredShifter

        zfinderTool = IDScanZFinder(name="IDScanZFinder_Cosmics",  FullScanMode = self.FullScanMode,
                                    NumberOfPeaks = 6,
                                    PhiBinSize = 0.5,UseOnlyPixels = False, MinZBinSize = 10.,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False, MaxZ=800.)
        zfinderTool.nFirstLayers = 7

        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool
        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Cosmics", fullScan=self.FullScanMode,
                                        phiHalfWidth = self.phiHalfWidth,  etaHalfWidth = self.etaHalfWidth,
                                        PhiBinSize = 2.4, EtaBinSize = 0.04, LayerThreshold = 2.4,
                                        LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                        RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = False )

#                                        PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 2.4,
#                                        LooseTightBoundary = 10.5, EnhanceLayer0 = True,
#                                        RemoveDuplicates = True, CloneRemovalCut = 2, UsePtSign = False)

#                                        etaCutOuter_loose = 0.3, etaCutInner_loose = 0.3, 
#                                        d0Cut_loose = 20, deltadzdrCut_loose = 0.2,
#                                        etaCutOuter_tight = 0.3, etaCutInner_tight = 0.3, 
#                                        d0Cut_tight = 20, deltadzdrCut_tight = 0.2)

#       hitfilterTool.dPhidRBinSize_loose = 0.00000375
#       hitfilterTool.phi0Bins_loose = 12000
        hitfilterTool.etaCutOuter_loose = 0.05
        hitfilterTool.etaCutInner_loose = 0.1
        hitfilterTool.deltadzdrCut_loose = 0.2
        hitfilterTool.d0Cut_loose = 20
         
#       hitfilterTool.dPhidRBinSize_tight = 0.00000300
#       hitfilterTool.phi0Bins_tight = 16000
        hitfilterTool.etaCutOuter_tight = 0.04
        hitfilterTool.etaCutInner_tight = 0.05
        hitfilterTool.deltadzdrCut_tight = 0.2
        hitfilterTool.d0Cut_tight = 20




        ToolSvc += hitfilterTool
        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,200]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]



class TrigIDSCAN_eGamma_Brem(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_eGamma_Brem"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.1
        self.etaHalfWidth = 0.1
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_eGamma_Brem", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_eGamma_Brem",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = False)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        trtBrTool = TrigTRT_TrackExtensionTool_eGamma()
        ToolSvc += trtBrTool
        self.TrigTRT_TrackExtensionTool=trtBrTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]



class TrigIDSCAN_Tile(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Tile"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.2
        self.etaHalfWidth = 0.1
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = True
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Tile", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False)

        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Tile",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = True)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]



class TrigIDSCAN_Muon_NoTRT(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_Muon_NoTRT"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.1
        self.etaHalfWidth = 0.1
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = False
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.useRoiDescriptorWidths = True
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_Muon_NoTRT", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3, UseOnlyPixels = False, MinZBinSize = 0.3,
                                    ZBinSizeEtaCoeff = 0.15, DPhiDEta = -0.025, ChargeAware = True,
                                    ZHistoPerPhi = False, NeighborMultiplier = 3, nVrtxSeparation = 5,
                                    nFirstLayers = 4, TripletMode = 1,
                                    VrtxMixing = 0, VrtxDistCut = 0.10, PreferCentralZ = True)

        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_Muon_NoTRT",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                        PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                        LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                        RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = True,
                                        d0Cut_loose = 10, d0Cut_tight = 10)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = True
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]





class TrigIDSCAN_muonIso_NoTRT(TrigIDSCAN):
    __slots__ = []
    def __init__(self, name = "TrigIDSCAN_muonIso_NoTRT"):
        TrigIDSCAN.__init__(self, name)
        self.SpacePointProviderTool=ospTool
        self.phiHalfWidth = 0.2
        self.etaHalfWidth = 0.2
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.TrkTrackCollName = "IDSCAN_Tracks"
        self.trackConvertorMode = 0
        self.doTRTpropagation = False
        self.ZFinderMode = 0
        self.WriteFlag = False
        self.WriteTruth = False
        self.DumpPrefix = "spacepoints"
        self.DetMaskCheck = True
        self.useRoiDescriptorWidths = False
        self.zMin = -RegSelZmax
        self.zMax =  RegSelZmax
        zfinderTool = IDScanZFinder(name="IDScanZFinder_muonIso_NoTRT", NumberOfPeaks = 4,
                                    PhiBinSize = 0.3,UseOnlyPixels = False, MinZBinSize = 0.4,
                                    ZBinSizeEtaCoeff = 0.2, DPhiDEta = -0.025, ChargeAware = False,
                                    ZHistoPerPhi = False)

        from AthenaCommon.AppMgr import ToolSvc
        
        ToolSvc += zfinderTool
        self.TrigZFinder=zfinderTool

        hitfilterTool = IDScanHitFilter(name="IDScanHitFilter_muonIso_NoTRT",phiHalfWidth = self.phiHalfWidth,
                                        etaHalfWidth = self.etaHalfWidth,
                                              PhiBinSize = 2.4, EtaBinSize = 0.005, LayerThreshold = 3.5,
                                              LooseTightBoundary = 10.5, EnhanceLayer0 = True,
                                              RemoveDuplicates = True, CloneRemovalCut = 3, UsePtSign = True)

        hitfilterTool.deltadzdrCut_loose = 0.12
        hitfilterTool.deltadzdrCut_tight = 0.12

        hitfilterTool.pTcutInMeV = 1000.0

        ToolSvc += hitfilterTool

        self.TrigHitFilter = hitfilterTool
        self.TrigTRT_TrackExtensionTool=trtTool
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = False
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,20]
        self.AthenaMonTools = [ TrigIDSCAN_ValidationMonitoring("TrigIDSCAN_ValidationMonitoring",self.doResMonitoring),
                                TrigIDSCAN_OnlineMonitoring("TrigIDSCAN_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]





