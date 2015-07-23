# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigL2SiTrackFinder.TrigL2SiTrackFinderConf import TrigL2SiTrackFinder

from TrigOnlineSpacePointTool.TrigOnlineSpacePointTool_Config import ConfiguredOnlineSpacePointProviderTool
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionToolConf import TrigTRT_TrackExtensionTool
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionTool_Config import TrigTRT_TrackExtensionTool_eGamma
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetTrackFitter
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2ResidualCalculator

from TrigInDetRecoTools.TrigInDetRecoTools_Config import TrigL2PattRecoStrategyFactory

strFactory = TrigL2PattRecoStrategyFactory()

from AthenaCommon.AppMgr import ToolSvc

ospTool = ConfiguredOnlineSpacePointProviderTool(doBS=True)
trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False, PtCut=2500.0)
trackFitter = TrigInDetTrackFitter(OfflineClusters=False)
resCalc = TrigL2ResidualCalculator(OfflineClusters=False)

ToolSvc += ospTool
ToolSvc += trtTool
ToolSvc += trackFitter
ToolSvc += resCalc

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

#Monitoring
class TrigL2SiTrackFinder_CommonMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2SiTrackFinder_OnlineMonitoring"):
        super(TrigL2SiTrackFinder_CommonMonitoring, self).__init__(name)
    def addSPHistograms(self):
        self.Histograms += [ defineHistogram('roi_nSPsPIX',
                                             type='TH1F',
                                             title="Number of Pixel SPs",
                                             xbins = 500, xmin=-0.5, xmax=4999.5)]
        self.Histograms += [ defineHistogram('roi_nSPsSCT',
                                             type='TH1F',
                                             title="Number of SCT SPs",
                                             xbins = 500, xmin=-0.5, xmax=4999.5)]
        self.Histograms += [ defineHistogram('roi_eta',
                                             type='TH1F',
                                             title="Eta of the input RoI",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('roi_phi',
                                             type='TH1F',
                                             title="Phi of the input RoI",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('roi_phiWidth',
                                             type='TH1F',
                                             title="Phi width of the input RoI",
                                             xbins = 100, xmin=0, xmax=6.4)]
        self.Histograms += [ defineHistogram('roi_etaWidth',
                                             type='TH1F',
                                             title="Eta width of the input RoI",
                                             xbins = 100, xmin=0, xmax=5)]
    def addDataErrorHistograms(self):
        self.Histograms += [ defineHistogram('roi_lastStageExecuted',
                                             type='TH1F',
                                             title="Last Step Successfully Executed",
                                             xbins = 8 , xmin=-0.5, xmax=7.5,
                             labels='Start : GetRoI : GetSPs : MissingLayers : ZFinder : Groups : TrackFit : Extend2TRT' ) ]
        self.Histograms += [ defineHistogram('dataErrors',
                                             type='TH1F',
                                             title="Data Errors",
                                             xbins = 4 , xmin=0., xmax=4.,
                             labels='SP_Abort : SP_Recoverable : TRT_Abort : TRT_Recoverable' ) ]
        # SCT Errors are defined in : InnerDetector/InDetConditions/SCT_ConditionsServices/SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('dataErrors_SCT',
                                             type='TH1F',
                                             title="SCT Data Errors",
                                             xbins = 14 , xmin=0., xmax=14,
                             labels='SCT_Decoding : SCT_TimeOut : SCT_BCID : SCT_LVL1ID : SCT_Preamble : SCT_Formatter : SCT_Trailer : SCT_TrailerOverFlow : SCT_HeaderTrailerLimit : SCT_ABCD : SCT_Raw : SCT_MaskedLink : SCT_RODClockError : SCT_TruncatedROD' ) ]
        # PIX Errors are defined in : InnerDetector/InDetConditions/PixelConditionsServices/PixelConditionsServices/IPixelByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('dataErrors_PIX',
                                             type='TH1F',
                                             title="Pixel Data Errors",
                                             xbins = 9 , xmin=0., xmax=9.,
                             labels='PIX_TimeOut : PIX_BCID : PIX_LVL1ID : PIX_Preamble : PIX_Trailer : PIX_Flagged : PIX_DisabledFE : PIX_TruncatedROD : PIX_Decoding')]
        # TRT Errors are defined in : InnerDetector/InDetConditions/TRT_ConditionsServices/src/TRT_ByteStream_ConditionsSvc.cxx
        self.Histograms += [ defineHistogram('dataErrors_TRT',
                                             type='TH1F',
                                             title="TRT Data Errors",
                                             xbins = 6 , xmin=0., xmax=6.,
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
        self.Histograms += [ defineHistogram('roi_nSPs , time_PattRecoOnly',
                                             type='TH2F',
                                             title="PattReco time vs nSPs",
                                             xbins = 200 ,  xmin=0.0, xmax=3000.0, 
                                             ybins = 100 ,  ymin=0.0, ymax=200.0)]
        self.Histograms += [ defineHistogram('roi_nTracks , time_PattRecoOnly',
                                             type='TH2F',
                                             title="PattReco time vs nTracks",
                                             xbins = 50 ,  xmin=0.0, xmax=200.0, 
                                             ybins = 100 ,  ymin=0.0, ymax=200.0)]
        self.Histograms += [ defineHistogram('roi_nZvertices , time_PattRecoOnly',
                                             type='TH2F',
                                             title="PattReco time vs nZvertices",
                                             xbins = 10 ,  xmin=-0.5, xmax=9.5, 
                                             ybins = 200 ,  ymin=0.0, ymax=100.0)]
        self.Histograms += [ defineHistogram('roi_zVertices',
                                             type='TH1F',
                                             title="ZFinder Vertices",
                                             xbins = 401, xmin=-200, xmax=200)]
        self.Histograms += [ defineHistogram('time_PattRecoOnly',
                                             type='TH1F',
                                             title="Pure PattReco time",
                                             xbins = nbin , xmin=0.0, xmax=200.0)]
    
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
                                             xbins = 20, xmin=-0.5, xmax=19.5)]
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

        self.Histograms += [ defineHistogram('roi_nZvertices',
                                             type='TH1F',
                                             title="ZFinder Number of Vertices",
                                             xbins = 10, xmin=-0.5, xmax=9.5)]

        self.Histograms += [ defineHistogram('sp_x , sp_y',
                                             type='TH2F',
                                             title="Spacepoints-on-track xy",
                                             xbins = 200 ,  xmin=-600.0, xmax=600.0, 
                                             ybins = 200 ,  ymin=-600.0, ymax=600.0)]

        self.Histograms += [ defineHistogram('sp_z , sp_r',
                                             type='TH2F',
                                             title="Spacepoints-on-track rz",
                                             xbins = 400 ,  xmin=-2800.0, xmax=2800.0, 
                                             ybins = 100 ,  ymin=0.0, ymax=600.0)]
        
#        self.Histograms += [ defineHistogram('roi_ZvrtxPos',
#                                             type='TH1F',
#                                             title="Z Position of Vertex",
#                                             xbins = 100, xmin=-400., xmax=400.)]
#        self.Histograms += [ defineHistogram('roi_SPxShift',
#                                             type='TH1F',
#                                             title="x-shift applied to SPs",
#                                             xbins = 100, xmin=-1000., xmax=1000.)]

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

class TrigL2SiTrackFinder_OnlineMonitoring(TrigL2SiTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigL2SiTrackFinder_OnlineMonitoring", resHistos=False):
        super(TrigL2SiTrackFinder_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.addSPHistograms()
        self.addDataErrorHistograms()
#        self.addSiTimingHistograms(250,0.,250.)
#        self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()

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


class TrigL2SiTrackFinder_ValidationMonitoring(TrigL2SiTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigL2SiTrackFinder_ValidationMonitoring", resHistos=False):
        super(TrigL2SiTrackFinder_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addSiTimingHistograms(150,0.,150.)
        self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()

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
                                             xbins = 200, xmin=-0.5, xmax=199.5)]


#Cosmic Monitoring
class TrigL2SiTrackFinder_Cosmic_Monitoring(TrigL2SiTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigL2SiTrackFinder_Cosmic_Monitoring", resHistos=False):
        super(TrigL2SiTrackFinder_Cosmic_Monitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addSiTimingHistograms(250,0.,50.)
        self.addTRTTimingHistograms(250,0.,100.)
        self.addTrackHistograms()

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




class TrigL2SiTrackFinderBase(TrigL2SiTrackFinder):
    __slots__ = []
    def __init__(self, instName, strName, phiHalfWidth = 0.1, etaHalfWidth = 0.1, doTRT = True, doRes = False, doTrackFit = True, doBrem = False ):

        name = "TrigL2SiTrackFinder_"+instName+strName
        if doBrem :
            name += "_Brem"
        TrigL2SiTrackFinder.__init__(self,name)
        
        self.SpacePointProviderTool=ospTool
        if instName=="FullScan" or instName=="BeamSpot" or instName=="FullScan_ZF_Only" :
            self.FullScanMode = True
        else :
            self.FullScanMode = False
        
        self.phiHalfWidth = phiHalfWidth
        self.etaHalfWidth = etaHalfWidth
        self.MinHits = 5
        self.PrintDiagnosticMessages = False
        self.doTrackFit = doTrackFit
        self.doTRTpropagation = doTRT
        self.useRoiDescriptorWidths = True
        if instName=="Bphysics" :
            self.useRoiDescriptorWidths = False
        
        self.OutputCollectionSuffix=instName

        self.attachVertices=False
        if instName=="FullScan_ZF_Only" and strName=='A' :
            self.attachVertices = True

        from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
        self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

        from AthenaCommon.AppMgr import ToolSvc

        strategy = strFactory.createStrategy(instName, strName, phiHalfWidth, etaHalfWidth)
        ToolSvc += strategy
        self.RecoStrategy = strategy

        if doBrem :
            trtBrTool = TrigTRT_TrackExtensionTool_eGamma()
            ToolSvc += trtBrTool
            self.TrigTRT_TrackExtensionTool=trtBrTool
        else :
            self.TrigTRT_TrackExtensionTool=trtTool
            
        self.TrigInDetTrackFitter = trackFitter
        self.TrigL2ResidualCalculator = resCalc
        self.doResMonitoring = doRes
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,2000]
        self.AthenaMonTools = [ TrigL2SiTrackFinder_ValidationMonitoring("TrigL2SiTrackFinder_ValidationMonitoring",self.doResMonitoring),
                                TrigL2SiTrackFinder_OnlineMonitoring("TrigL2SiTrackFinder_OnlineMonitoring",self.doResMonitoring),
                                timeHist ]



class TrigL2SiTrackFinder_MuonA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_MuonA"):
    TrigL2SiTrackFinderBase.__init__(self, "Muon", "A", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_MuonB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_MuonB"):
    TrigL2SiTrackFinderBase.__init__(self, "Muon", "B", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_MuonC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_MuonC"):
    TrigL2SiTrackFinderBase.__init__(self, "Muon", "C", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_eGammaA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_eGammaA"):
    TrigL2SiTrackFinderBase.__init__(self, "eGamma", "A", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_eGammaB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_eGammaB"):
    TrigL2SiTrackFinderBase.__init__(self, "eGamma", "B", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_eGammaC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_eGammaC"):
    TrigL2SiTrackFinderBase.__init__(self, "eGamma", "C", 0.1, 0.1, True, False, True)


class TrigL2SiTrackFinder_eGammaA_Brem(TrigL2SiTrackFinderBase):
    def __init__(self, name = "TrigL2SiTrackFinder_eGammaA_Brem"):
            TrigL2SiTrackFinderBase.__init__(self, "eGamma", "A", 0.1, 0.1, True, False, True, True)

class TrigL2SiTrackFinder_JetA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_JetA"):
    TrigL2SiTrackFinderBase.__init__(self, "Jet", "A", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_JetB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_JetB"):
    TrigL2SiTrackFinderBase.__init__(self, "Jet", "B", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_JetC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_JetC"):
    TrigL2SiTrackFinderBase.__init__(self, "Jet", "C", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_TauA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauA"):
    TrigL2SiTrackFinderBase.__init__(self, "Tau", "A", 0.3, 0.3, True, False, True)


class TrigL2SiTrackFinder_TauB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauB"):
    TrigL2SiTrackFinderBase.__init__(self, "Tau", "B", 0.3, 0.3, True, False, True)


class TrigL2SiTrackFinder_TauC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauC"):
    TrigL2SiTrackFinderBase.__init__(self, "Tau", "C", 0.3, 0.3, True, False, True)

class TrigL2SiTrackFinder_muonIsoA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_muonIsoA"):
    TrigL2SiTrackFinderBase.__init__(self, "muonIso", "A", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_muonIsoB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_muonIsoB"):
    TrigL2SiTrackFinderBase.__init__(self, "muonIso", "B", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_muonIsoC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_muonIsoC"):
    TrigL2SiTrackFinderBase.__init__(self, "muonIso", "C", 0.2, 0.2, True, False, True)


class TrigL2SiTrackFinder_BphysicsA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BphysicsA"):
    TrigL2SiTrackFinderBase.__init__(self, "Bphysics", "A", 0.75, 0.75, True, False, True)


class TrigL2SiTrackFinder_BphysicsB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BphysicsB"):
    TrigL2SiTrackFinderBase.__init__(self, "Bphysics", "B", 0.75, 0.75, True, False, True)


class TrigL2SiTrackFinder_BphysicsC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BphysicsC"):
    TrigL2SiTrackFinderBase.__init__(self, "Bphysics", "C", 0.75, 0.75, True, False, True)


class TrigL2SiTrackFinder_FullScanA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_FullScanA"):
    TrigL2SiTrackFinderBase.__init__(self,"FullScan", "A", 3.14159265358, 3.0, True, False, True)


class TrigL2SiTrackFinder_FullScanB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_FullScanB"):
    TrigL2SiTrackFinderBase.__init__(self,"FullScan", "B", 3.14159265358, 3.0, True, False, True)

class TrigL2SiTrackFinder_FullScanC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_FullScanC"):
    TrigL2SiTrackFinderBase.__init__(self,"FullScan", "C", 3.14159265358, 3.0, True, False, True)


class TrigL2SiTrackFinder_FullScan_ZF_OnlyA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_FullScan_ZF_OnlyA"):
    TrigL2SiTrackFinderBase.__init__(self,"FullScan_ZF_Only", "A", 3.14159265358, 3.0, False, False, False)


class TrigL2SiTrackFinder_BeamSpotA(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BeamSpotA"):
    TrigL2SiTrackFinderBase.__init__(self,"BeamSpot", "A", 3.14159265358, 3.0, False, False, True)


class TrigL2SiTrackFinder_BeamSpotB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BeamSpotB"):
    TrigL2SiTrackFinderBase.__init__(self,"BeamSpot", "B", 3.14159265358, 3.0, False, False, True)

class TrigL2SiTrackFinder_BeamSpotC(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BeamSpotC"):
    TrigL2SiTrackFinderBase.__init__(self,"BeamSpot", "C", 3.14159265358, 3.0, False, False, True)


#--- experimental 2-step tau reconstruction
class TrigL2SiTrackFinder_TauCoreB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauCoreB"):
    TrigL2SiTrackFinderBase.__init__(self, "TauCore", "B", 0.2, 0.2, True, False, True)
    self.OutputCollectionSuffix="TauCoreB"

class TrigL2SiTrackFinder_TauIsoB(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauIsoB"):
    TrigL2SiTrackFinderBase.__init__(self, "TauIso", "B", 0.3, 0.3, True, False, True)
    self.VertexSeededMode = True
  

#----------------- instances for FTK studies -----------------------------------------------------

class TrigL2SiTrackFinder_BeamSpotF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BeamSpotF"):
    TrigL2SiTrackFinderBase.__init__(self,"BeamSpot", "F", 3.14159265358, 3.0, False, False, False)

class TrigL2SiTrackFinder_FullScanF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_FullScanF"):
    TrigL2SiTrackFinderBase.__init__(self,"FullScan", "F", 3.14159265358, 3.0, False, False, False)


class TrigL2SiTrackFinder_muonIsoF(TrigL2SiTrackFinderBase):
    def __init__(self, name = "TrigL2SiTrackFinder_muonIsoF"):
        TrigL2SiTrackFinderBase.__init__(self, "muonIso", "F", 0.2, 0.2, False, False, False)

class TrigL2SiTrackFinder_MuonF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_MuonF"):
    TrigL2SiTrackFinderBase.__init__(self, "Muon", "F", 0.1, 0.1, False, False, False)

class TrigL2SiTrackFinder_eGammaF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_eGammaF"):
    TrigL2SiTrackFinderBase.__init__(self, "eGamma", "F", 0.1, 0.1, False, False, False)


class TrigL2SiTrackFinder_TauF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_TauF"):
    TrigL2SiTrackFinderBase.__init__(self, "Tau", "F", 0.3, 0.3, False, False, False)


class TrigL2SiTrackFinder_JetF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_JetF"):
    TrigL2SiTrackFinderBase.__init__(self, "Jet", "F", 0.2, 0.2, False, False, False)


class TrigL2SiTrackFinder_BphysicsF(TrigL2SiTrackFinderBase):
  def __init__(self, name = "TrigL2SiTrackFinder_BphysicsF"):
    TrigL2SiTrackFinderBase.__init__(self, "Bphysics", "F", 0.75, 0.75, False, False, False)

#-------------------------------------------------------------------------------------------------

 
