# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinderConf import TrigFastTrackFinder

from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigL2LayerNumberTool
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

from AthenaCommon.AppMgr import ToolSvc



from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

from AthenaCommon.SystemOfUnits import *

#Monitoring
class TrigFastTrackFinder_CommonMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFastTrackFinder_OnlineMonitoring"):
        super(TrigFastTrackFinder_CommonMonitoring, self).__init__(name)
    def addSPHistograms(self):
        self.Histograms += [ defineHistogram('roi_nSPsPIX',
                                             type='TH1F',
                                             title="Number of Pixel SPs",
                                             xbins = 500, xmin=-0.5, xmax=49999.5)]
        self.Histograms += [ defineHistogram('roi_nSPsSCT',
                                             type='TH1F',
                                             title="Number of SCT SPs",
                                             xbins = 500, xmin=-0.5, xmax=49999.5)]
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
        self.Histograms += [ defineHistogram('roi_z',
                                             type='TH1F',
                                             title="z of the input RoI",
                                             xbins = 200, xmin=-400, xmax=400)]
        self.Histograms += [ defineHistogram('roi_zWidth',
                                             type='TH1F',
                                             title="z width of the input RoI",
                                             xbins = 100, xmin=0, xmax=500)]
    def addDataErrorHistograms(self):
        self.Histograms += [ defineHistogram('roi_lastStageExecuted',
                                             type='TH1F',
                                             title="Last Step Successfully Executed",
                                             xbins = 8 , xmin=-0.5, xmax=7.5,
                             labels='Start : GetRoI : GetSPs : ZFinder : Triplets : TrackMaker : TrackFitter : TrackConverter' ) ]
   
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
        self.Histograms += [ defineHistogram('trk_chi2dof',
                                             type='TH1F',
                                             title="ChiSqd / nDoF",
                                             xbins = 100, xmin=0.0, xmax=5)]
        self.Histograms += [ defineHistogram('trk_pt',
                                             type='TH1F',
                                             title="pt",
                                             xbins = 400, xmin=-1e6, xmax=1e6)]
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
        self.Histograms += [ defineHistogram('roi_nSeeds',
                                             type='TH1F',
                                             title="Number of seeds",
                                             xbins = 1000, xmin=-0.5, xmax=99999.5)]
        self.Histograms += [ defineHistogram('roi_nZvertices',
                                             type='TH1F',
                                             title="Number of z vertices",
                                             xbins = 60 ,  xmin=-0.5, xmax=49.5)] 
        self.Histograms += [ defineHistogram('roi_zVertices',
                                             type='TH1F',
                                             title="ZFinder Vertices",
                                             xbins = 501, xmin=-250, xmax=250)]
        self.Histograms += [ defineHistogram('roi_nTrk_zVtx',
                                             type='TH1F',
                                             title="Ntrk ZFinder Vertices",
                                             xbins = 100, xmin=-0.5, xmax=49.5)]

        #self.Histograms += [ defineHistogram('sp_x , sp_y',
        #                                     type='TH2F',
        #                                     title="Spacepoints-on-track xy",
        #                                     xbins = 200 ,  xmin=-600.0, xmax=600.0, 
        #                                     ybins = 200 ,  ymin=-600.0, ymax=600.0)]

        #self.Histograms += [ defineHistogram('sp_z , sp_r',
        #                                     type='TH2F',
        #                                     title="Spacepoints-on-track rz",
        #                                     xbins = 400 ,  xmin=-2800.0, xmax=2800.0, 
        #                                     ybins = 100 ,  ymin=0.0, ymax=600.0)]
    #Extra timing histograms for validation only
    def addTimingHistograms(self,nbin,min,max):
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
        self.Histograms += [ defineHistogram('time_PattRecoOnly',
                                             type='TH1F',
                                             title="Pure PattReco time",
                                             xbins = nbin , xmin=0.0, xmax=200.0)]
    def addResidualHistograms(self):
        self.Histograms += [ defineHistogram('layer_IBL',
                                             type='TH1F',
                                             title="IBL layer",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('layer_PixB',
                                             type='TH1F',
                                             title="Pixel Barrel layer",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('layer_PixE',
                                             type='TH1F',
                                             title="Pixel Endcap layer",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('layer_SCTB',
                                             type='TH1F',
                                             title="SCT Barrel layer",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('layer_SCTE',
                                             type='TH1F',
                                             title="SCT Endcap layer",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('hit_IBLPhiResidual',
                                             type='TH1F',
                                             title="IBL hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_IBLEtaResidual',
                                             type='TH1F',
                                             title="IBL hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_IBLPhiPull',
                                             type='TH1F',
                                             title="IBL hit-track phi pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_IBLEtaPull',
                                             type='TH1F',
                                             title="IBL hit-track eta pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelPhiResidual',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelEtaResidual',
                                             type='TH1F',
                                             title="Pixel Barrel hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL1PhiResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L1 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL1EtaResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L1 hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL2PhiResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L2 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL2EtaResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L2 hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL3PhiResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L3 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXBarrelL3EtaResidual',
                                             type='TH1F',
                                             title="Pixel Barrel L3 hit-track eta residual",
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
        self.Histograms += [ defineHistogram('hit_SCTBarrelL1PhiResidual',
                                             type='TH1F',
                                             title="SCT Barrel L1 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTBarrelL2PhiResidual',
                                             type='TH1F',
                                             title="SCT Barrel L2 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTBarrelL3PhiResidual',
                                             type='TH1F',
                                             title="SCT Barrel L3 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTBarrelL4PhiResidual',
                                             type='TH1F',
                                             title="SCT Barrel L4 hit-track phi residual",
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
        self.Histograms += [ defineHistogram('hit_PIXEndCapL1PhiResidual',
                                             type='TH1F',
                                             title="Pixel EC L1 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapL1EtaResidual',
                                             type='TH1F',
                                             title="Pixel EC L1 hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapL2PhiResidual',
                                             type='TH1F',
                                             title="Pixel EC L2 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapL2EtaResidual',
                                             type='TH1F',
                                             title="Pixel EC L2 hit-track eta residual",
                                             xbins = 100, xmin=-1.0, xmax=1.0)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapL3PhiResidual',
                                             type='TH1F',
                                             title="Pixel EC L3 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_PIXEndCapL3EtaResidual',
                                             type='TH1F',
                                             title="Pixel EC L3 hit-track eta residual",
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
        self.Histograms += [ defineHistogram('hit_SCTEndcapL1PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L1 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL2PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L2 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL3PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L3 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL4PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L4 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL5PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L5 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL6PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L6 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL7PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L7 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL8PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L8 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndcapL9PhiResidual',
                                             type='TH1F',
                                             title="SCT Endcap L9 hit-track phi residual",
                                             xbins = 100, xmin=-0.5, xmax=0.5)]
        self.Histograms += [ defineHistogram('hit_SCTEndCapPull',
                                             type='TH1F',
                                             title="SCT EC hit-track pull",
                                             xbins = 100, xmin=-5., xmax=5.)]
    
   

class TrigFastTrackFinder_OnlineMonitoring(TrigFastTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigFastTrackFinder_OnlineMonitoring", doResMon=False):
        super(TrigFastTrackFinder_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addTrackHistograms()
        if doResMon:
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



class TrigFastTrackFinder_ValidationMonitoring(TrigFastTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigFastTrackFinder_ValidationMonitoring", doResMon=False):
        super(TrigFastTrackFinder_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.addSPHistograms()
        self.addTimingHistograms(150,0.,150.)
        self.addDataErrorHistograms()
        self.addTrackHistograms()
        if doResMon:
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
class TrigFastTrackFinder_Cosmic_Monitoring(TrigFastTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigFastTrackFinder_Cosmic_Monitoring", doResMon=False):
        super(TrigFastTrackFinder_Cosmic_Monitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addTrackHistograms()
        if doResMon:
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

def remapper(type):
    #this funcion should not be needed - we don't have to remap in both directions
    remap  = {
        "Muon"     : "muon",
        "MuonCore" : "muonCore",
        "MuonIso"  : "muonIso",
        "eGamma"   : "electron",
        "Tau"      : "tau",
        "TauCore"  : "tauCore",
        "TauIso"   : "tauIso",
        "Jet"      : "bjet",
        #"Jet"      : "bjetVtx",
        "FullScan" : "fullScan",
        "BeamSpot" : "beamSpot",
        "Bphysics" : "bphysics",
        "Cosmic"   : "cosmics",
    }
    if type in remap.keys():
      return remap[type]
    else:
      return type


class TrigFastTrackFinderBase(TrigFastTrackFinder):
    __slots__ = []
    def __init__(self, name, type):
        TrigFastTrackFinder.__init__(self,name)
        remapped_type = remapper(type)

        self.retrieveBarCodes = False#Look at truth information for spacepoints from barcodes
        #self.SignalBarCodes = [10001] #single particles
        self.SignalBarCodes = [11 ,12] #z->mumu
        
        self.useNewLayerNumberScheme = True
        
        self.OutputCollectionSuffix = type
        from AthenaCommon.AppMgr import ToolSvc

        numberingTool = TrigL2LayerNumberTool(name = "TrigL2LayerNumberTool_FTF")
        numberingTool.UseNewLayerScheme = self.useNewLayerNumberScheme
        ToolSvc += numberingTool
        self.LayerNumberTool = numberingTool
        
        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,10000]
        from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
        self.doResMon = InDetTrigSliceSettings[('doResMon',remapped_type)]
        self.AthenaMonTools = [ TrigFastTrackFinder_ValidationMonitoring("TrigFastTrackFinder_ValidationMonitoring", self.doResMon),
                                TrigFastTrackFinder_OnlineMonitoring("TrigFastTrackFinder_OnlineMonitoring", self.doResMon),
                                timeHist ]

        if type=="FTK" or type=="FTKRefit" or type=="FTKMon":
          from TrigFTK_RecExample.TrigFTKLoadTools import theFTK_DataProviderSvc
          self.FTK_DataProviderSvc = theFTK_DataProviderSvc
          self.FTK_Mode=True
        if type=="FTKRefit":    
          self.FTK_Refit=True
        else:
          #Spacepoint conversion
          from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigSpacePointConversionTool
          spTool = TrigSpacePointConversionTool().clone('TrigSpacePointConversionTool_' + remapped_type)
          spTool.DoPhiFiltering = InDetTrigSliceSettings[('doSpPhiFiltering',remapped_type)]
          spTool.UseNewLayerScheme = self.useNewLayerNumberScheme
          spTool.UseBeamTilt = False
          spTool.layerNumberTool = numberingTool
          ToolSvc += spTool
          self.SpacePointProviderTool=spTool
          self.MinSPs = 5 #Only process RoI with more than 5 spacepoints 
          
          self.Triplet_MinPtFrac = 1
          self.Triplet_nMaxPhiSlice = 53
          if remapped_type=="cosmics":
            self.Triplet_nMaxPhiSlice = 2 #Divide detector in 2 halves for cosmics
          
          self.Triplet_MaxBufferLength = 3
          self.doSeedRedundancyCheck = InDetTrigSliceSettings[('checkRedundantSeeds',remapped_type)]
          self.Triplet_D0Max        = InDetTrigSliceSettings[('d0SeedMax',remapped_type)]
          self.Triplet_D0_PPS_Max   = InDetTrigSliceSettings[('d0SeedPPSMax',remapped_type)] 
          self.TrackInitialD0Max = 20.
          if remapped_type=='cosmics':
            self.TrackInitialD0Max = 1000.
            self.TrackZ0Max   = 1000.

          self.TripletDoPSS   = False
          self.pTmin = InDetTrigSliceSettings[('pTmin',remapped_type)]
          self.DoubletDR_Max = InDetTrigSliceSettings[('dRdoubletMax',remapped_type)]
          self.SeedRadBinWidth = InDetTrigSliceSettings[('seedRadBinWidth',remapped_type)]
          self.doTrigInDetTrack = InDetTrigSliceSettings[('doTrigInDetTrack',remapped_type)]

          if remapped_type=="cosmics":
            self.Doublet_FilterRZ = False


          ## SCT and Pixel detector elements road builder
          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
          InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_FTF')
          InDetTrigSiDetElementsRoadMaker_FTF.RoadWidth = 10.0
          if remapped_type=="cosmics":
            from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigSiDetElementsRoadMakerCosmics
            InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMakerCosmics.clone('InDetTrigSiDetElementsRoadMaker_FTF')
          ToolSvc += InDetTrigSiDetElementsRoadMaker_FTF


          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiComTrackFinder
          InDetTrigSiComTrackFinder_FTF = InDetTrigSiComTrackFinder.clone("InDetTrigSiComTrackFinder_FTF")
          ToolSvc += InDetTrigSiComTrackFinder_FTF
        
        
          from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
          TrackingCuts = EFIDTrackingCuts
          if remapped_type=="cosmics":
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsCosmics
            TrackingCuts = EFIDTrackingCutsCosmics

          from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk

          TrackMaker_FTF = InDet__SiTrackMaker_xk(name = 'InDetTrigSiTrackMaker_FTF_'+type,
                                                RoadTool       = InDetTrigSiDetElementsRoadMaker_FTF,
                                                CombinatorialTrackFinder = InDetTrigSiComTrackFinder_FTF,
                                                pTmin          = InDetTrigSliceSettings[('pTmin',remapped_type)],
                                                nClustersMin   = TrackingCuts.minClusters(),
                                                nHolesMax      = TrackingCuts.nHolesMax(),
                                                nHolesGapMax   = TrackingCuts.nHolesGapMax(),
                                                SeedsFilterLevel = 0, # Do not use built-in seeds filter
                                                Xi2max         = TrackingCuts.Xi2max(),
                                                Xi2maxNoAdd    = TrackingCuts.Xi2maxNoAdd(),
                                                nWeightedClustersMin= TrackingCuts.nWeightedClustersMin(),
                                                Xi2maxMultiTracks         = TrackingCuts.Xi2max(),
                                                UseAssociationTool       = False)

          from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
          if type=='eGamma' and InDetTrigFlags.doBremRecovery():
            TrackMaker_FTF.useBremModel = True
  
          if remapped_type=="cosmics":
            TrackMaker_FTF.RoadTool.CosmicTrack=True

          ToolSvc += TrackMaker_FTF
          self.initialTrackMaker = TrackMaker_FTF

          from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetTrackFitter
          theTrigInDetTrackFitter = TrigInDetTrackFitter()
          #theTrigInDetTrackFitter.correctClusterPos = False #Flag to control whether to correct cluster position
          theTrigInDetTrackFitter.correctClusterPos = True  #temporarily to true to improve err(z0) estimates

            

          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigRotCreator
          theTrigInDetTrackFitter.ROTcreator = InDetTrigRotCreator
          ToolSvc += theTrigInDetTrackFitter
          self.trigInDetTrackFitter = theTrigInDetTrackFitter
          from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
          if type=='eGamma' and InDetTrigFlags.doBremRecovery():
            theTrigInDetTrackFitterBrem = TrigInDetTrackFitter(name='theTrigInDetTrackFitterBrem',
                                                               doBremmCorrection = True)
            ToolSvc += theTrigInDetTrackFitterBrem
            self.trigInDetTrackFitter = theTrigInDetTrackFitterBrem

          self.doZFinder = InDetTrigSliceSettings[('doZFinder',remapped_type)]
          if (self.doZFinder):
            from IDScanZFinder.IDScanZFinderConf import TrigZFinder
            theTrigZFinder = TrigZFinder()
            theTrigZFinder.NumberOfPeaks = 3
            theTrigZFinder.LayerNumberTool=numberingTool
            
            theTrigZFinder.FullScanMode = True #TODO: know this from the RoI anyway - should set for every event
            ToolSvc += theTrigZFinder
            self.trigZFinder = theTrigZFinder
            self.doFastZVertexSeeding = True
            self.zVertexResolution = 7.5
          
          from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
          self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

          if remapped_type == "tauCore":
            from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigTrackSummaryToolWithHoleSearch
            self.TrackSummaryTool = InDetTrigTrackSummaryToolWithHoleSearch

          from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2ResidualCalculator
          resCalc = TrigL2ResidualCalculator(OfflineClusters=False)
          ToolSvc += resCalc
          self.TrigL2ResidualCalculator = resCalc
          self.doCloneRemoval = InDetTrigSliceSettings[('doCloneRemoval',remapped_type)]


class TrigFastTrackFinder_Muon(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Muon"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Muon","Muon")

class TrigFastTrackFinder_eGamma(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_eGamma"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_eGamma","eGamma")

class TrigFastTrackFinder_Tau(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Tau"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Tau","Tau")

class TrigFastTrackFinder_TauCore(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_TauCore"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_TauCore","TauCore")

class TrigFastTrackFinder_TauIso(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_TauIso"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_TauIso","Tau")

class TrigFastTrackFinder_Jet(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Jet"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Jet","Jet")

class TrigFastTrackFinder_FTK(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_FTK"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_FTK","FTK")

class TrigFastTrackFinder_FTKRefit(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_FTKRefit"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_FTKRefit","FTKRefit")

class TrigFastTrackFinder_FTKMon(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_FTKMon"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_FTKMon","FTKMon")
