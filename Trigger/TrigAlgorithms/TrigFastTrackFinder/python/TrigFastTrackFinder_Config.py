# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinderConf import TrigFastTrackFinder

from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigL2LayerNumberTool

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigFastTrackFinderMonitoring(GenericMonitoringTool):
    def __init__ (self, name, doResMon=False):
        name = "TrigFastTrackFinder_" + name
        super(TrigFastTrackFinderMonitoring, self).__init__(name)
        self.HistPath = name
        self.addSPHistograms(name)
        self.addDataErrorHistograms()
        self.addTimingHistograms(name)
        self.addTrackHistograms(name)
        if doResMon:
            self.addResidualHistograms()

    def addSPHistograms(self, name):
        if name=='Electron' or name=='Muon' or name=='TauCore' or name=='MuonIso' or name=='TauIso':
            self.defineHistogram('roi_nSPsPIX', path='EXPERT',type='TH1F',title="Number of Pixel SPs", xbins = 50, xmin=-0.5, xmax=4999.5)
            self.defineHistogram('roi_nSPsSCT', path='EXPERT',type='TH1F',title="Number of SCT SPs", xbins = 50, xmin=-0.5, xmax=4999.5)
            self.defineHistogram('roi_phiWidth',path='EXPERT',type='TH1F',title="Phi width of the input RoI",xbins = 100, xmin=0, xmax=1.0)
            self.defineHistogram('roi_etaWidth',path='EXPERT',type='TH1F',title="Eta width of the input RoI",xbins = 100, xmin=0, xmax=1.0)
        else:
            self.defineHistogram('roi_nSPsPIX', path='EXPERT',type='TH1F',title="Number of Pixel SPs", xbins = 500, xmin=-0.5, xmax=49999.5)
            self.defineHistogram('roi_nSPsSCT', path='EXPERT',type='TH1F',title="Number of SCT SPs", xbins = 500, xmin=-0.5, xmax=99999.5)
            self.defineHistogram('roi_phiWidth',path='EXPERT',type='TH1F',title="Phi width of the input RoI",xbins = 100, xmin=0, xmax=6.4)
            self.defineHistogram('roi_etaWidth',path='EXPERT',type='TH1F',title="Eta width of the input RoI",xbins = 100, xmin=0, xmax=5)
        self.defineHistogram('roi_eta',     path='EXPERT',type='TH1F',title='Eta of the input RoI;;Entries', xbins=100, xmin=-5, xmax=5)
        self.defineHistogram('roi_phi',     path='EXPERT',type='TH1F',title="Phi of the input RoI",xbins = 100, xmin=-3.2, xmax=3.2)
        self.defineHistogram('roi_z',       path='EXPERT',type='TH1F',title="z of the input RoI",xbins = 200, xmin=-400, xmax=400)
        self.defineHistogram('roi_zWidth',  path='EXPERT',type='TH1F',title="z width of the input RoI",xbins = 100, xmin=0, xmax=500)

    def addDataErrorHistograms(self):
        self.defineHistogram('roi_lastStageExecuted',path='EXPERT',type='TH1F',title="Last Step Successfully Executed", xbins = 8 , xmin=-0.5, xmax=7.5,
                             xlabels=["Start","GetRoI","GetSPs","ZFinder","Triplets","TrackMaker","TrackFitter","TrackConverter"])
   
    def addTimingHistograms(self, name):
        if name=='Electron' or name=='Muon' or name=='TauCore' or name=='MuonIso' or name=='TauIso':
            self.defineHistogram('roi_nSPs, TIME_PattReco',   path='EXPERT',type='TH2F',title="PattReco time; nSPs",    xbins = 200, xmin=0.0, xmax=3000.0, ybins = 100, ymin=0.0, ymax=400.0)
            self.defineHistogram('roi_nTracks, TIME_PattReco',path='EXPERT',type='TH2F',title="PattReco time; nTracks", xbins = 50,  xmin=0.0, xmax=200.0,  ybins = 100, ymin=0.0, ymax=400.0)
            self.defineHistogram('TIME_PattReco',             path='EXPERT',type='TH1F',title="Pure PattReco time (ms)",     xbins = 200, xmin=0.0, xmax=400.0)
            self.defineHistogram('TIME_SpacePointConversion', path='EXPERT',type='TH1F',title="SP Conversion time (ms)",     xbins = 200, xmin=0.0, xmax=20.0)
            self.defineHistogram('TIME_ZFinder',              path='EXPERT',type='TH1F',title="ZFinder time (ms)",           xbins = 200, xmin=0.0, xmax=1000.0)
            self.defineHistogram('TIME_Triplets',             path='EXPERT',type='TH1F',title="Triplets Making time (ms)",   xbins = 200, xmin=0.0, xmax=400.0)
            self.defineHistogram('TIME_CmbTrack',             path='EXPERT',type='TH1F',title="Combined Tracking time (ms)", xbins = 200, xmin=0.0, xmax=400.0)
            self.defineHistogram('TIME_TrackFitter',          path='EXPERT',type='TH1F',title="Track Fitter time (ms)",      xbins = 200, xmin=0.0, xmax=200.0)
        else:
            self.defineHistogram('roi_nSPs, TIME_PattReco',   path='EXPERT',type='TH2F',title="PattReco time; nSPs",    xbins = 200, xmin=0.0, xmax=200000.0, ybins = 100, ymin=0.0, ymax=40000.0)
            self.defineHistogram('roi_nTracks, TIME_PattReco',path='EXPERT',type='TH2F',title="PattReco time; nTracks", xbins = 50,  xmin=0.0, xmax=10000.0,  ybins = 100, ymin=0.0, ymax=40000.0)
            self.defineHistogram('TIME_PattReco',             path='EXPERT',type='TH1F',title="Pure PattReco time (ms)",     xbins = 200, xmin=0.0, xmax=40000.0)
            self.defineHistogram('TIME_SpacePointConversion', path='EXPERT',type='TH1F',title="SP Conversion time (ms)",     xbins = 200, xmin=0.0, xmax=200.0)
            self.defineHistogram('TIME_ZFinder',              path='EXPERT',type='TH1F',title="ZFinder time (ms)",           xbins = 200, xmin=0.0, xmax=40000.0)
            self.defineHistogram('TIME_Triplets',             path='EXPERT',type='TH1F',title="Triplets Making time (ms)",   xbins = 200, xmin=0.0, xmax=40000.0)
            self.defineHistogram('TIME_CmbTrack',             path='EXPERT',type='TH1F',title="Combined Tracking time (ms)", xbins = 200, xmin=0.0, xmax=40000.0)
            self.defineHistogram('TIME_TrackFitter',          path='EXPERT',type='TH1F',title="Track Fitter time (ms)",      xbins = 200, xmin=0.0, xmax=2000.0)

    def addTrackHistograms(self, name):
        if name=='Electron' or name=='Muon' or name=='TauCore' or name=='MuonIso' or name=='TauIso':
            self.defineHistogram('roi_nSeeds',     path='EXPERT',type='TH1F',title="Number of seeds",xbins =  100, xmin=-0.5, xmax=4999.5)
            self.defineHistogram('roi_nTracks',    path='EXPERT',type='TH1F',title="Number of Tracks",xbins =  50, xmin=-0.5, xmax=199.5)
        else:
            self.defineHistogram('roi_nSeeds',     path='EXPERT',type='TH1F',title="Number of seeds",xbins = 1000, xmin=-0.5, xmax=99999.5)
            self.defineHistogram('roi_nTracks',    path='EXPERT',type='TH1F',title="Number of Tracks",xbins = 100, xmin=-0.5, xmax=9999.5)
        self.defineHistogram('roi_nZvertices', path='EXPERT',type='TH1F',title="Number of z vertices",xbins = 60 ,  xmin=-0.5, xmax=49.5)
        self.defineHistogram('roi_zVertices',  path='EXPERT',type='TH1F',title="ZFinder Vertices",xbins = 501, xmin=-250, xmax=250)
        self.defineHistogram('roi_nTrk_zVtx',  path='EXPERT',type='TH1F',title="Ntrk ZFinder Vertices",xbins = 100, xmin=-0.5, xmax=49.5)
        self.defineHistogram('trk_nSiHits',    path='EXPERT',type='TH1F',title="Total number of Silicon Hits per Track",xbins = 20, xmin=-0.5, xmax=19.5)
        self.defineHistogram('trk_nPIXHits',   path='EXPERT',type='TH1F',title="Number of Pixel Hits per Track",xbins = 10, xmin=-0.5, xmax=9.5)
        self.defineHistogram('trk_nSCTHits',   path='EXPERT',type='TH1F',title="Number of SCT Hits per Track",xbins = 10, xmin=-0.5, xmax=9.5)
        self.defineHistogram('trk_chi2dof',    path='EXPERT',type='TH1F',title="ChiSqd / nDoF",xbins = 100, xmin=0.0, xmax=5)
        self.defineHistogram('trk_pt',         path='EXPERT',type='TH1F',title="pt",xbins = 100, xmin=-2.5e5, xmax=2.5e5)
        self.defineHistogram('trk_phi0',       path='EXPERT',type='TH1F',title="phi",xbins = 100, xmin=-3.2, xmax=3.2)
        self.defineHistogram('trk_eta',        path='EXPERT',type='TH1F',title="eta",xbins = 100, xmin=-5, xmax=5)
        self.defineHistogram('trk_dPhi0',      path='EXPERT',type='TH1F',title="dphi",xbins = 160, xmin=-0.8, xmax=0.8)
        self.defineHistogram('trk_dEta',       path='EXPERT',type='TH1F',title="deta",xbins = 160, xmin=-0.8, xmax=0.8)
        if name=="Cosmic":
            self.defineHistogram('trk_a0',     path='EXPERT',type='TH1F',title="a0",xbins = 100, xmin=-300, xmax=300)
            self.defineHistogram('trk_a0beam', path='EXPERT',type='TH1F',title="a0beam",xbins = 100, xmin=-300, xmax=300)
            self.defineHistogram('trk_z0',     path='EXPERT',type='TH1F',title="z0",xbins = 100, xmin=-800, xmax=800)
        else:
            self.defineHistogram('trk_a0',     path='EXPERT',type='TH1F',title="a0",xbins = 200, xmin=-10, xmax=10)
            self.defineHistogram('trk_a0beam', path='EXPERT',type='TH1F',title="a0beam",xbins = 200, xmin=-10, xmax=10)
            self.defineHistogram('trk_z0',     path='EXPERT',type='TH1F',title="z0",xbins = 200, xmin=-400, xmax=400)

    def addResidualHistograms(self):
        self.defineHistogram('layer_IBL',                 path='EXPERT',type='TH1F',title="IBL layer",xbins = 10, xmin=0., xmax=10.)
        self.defineHistogram('layer_PixB',                path='EXPERT',type='TH1F',title="Pixel Barrel layer",xbins = 10, xmin=0., xmax=10.)
        self.defineHistogram('layer_PixE',                path='EXPERT',type='TH1F',title="Pixel Endcap layer",xbins = 10, xmin=0., xmax=10.)
        self.defineHistogram('layer_SCTB',                path='EXPERT',type='TH1F',title="SCT Barrel layer",xbins = 10, xmin=0., xmax=10.)
        self.defineHistogram('layer_SCTE',                path='EXPERT',type='TH1F',title="SCT Endcap layer",xbins = 10, xmin=0., xmax=10.)
        #
        self.defineHistogram('hit_IBLPhiResidual',        path='EXPERT',type='TH1F',title="IBL hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_IBLEtaResidual',        path='EXPERT',type='TH1F',title="IBL hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_IBLPhiPull',            path='EXPERT',type='TH1F',title="IBL hit-track phi pull",xbins = 100, xmin=-5., xmax=5.)
        self.defineHistogram('hit_IBLEtaPull',            path='EXPERT',type='TH1F',title="IBL hit-track eta pull",xbins = 100, xmin=-5., xmax=5.)
        #
        self.defineHistogram('hit_PIXBarrelPhiResidual',  path='EXPERT',type='TH1F',title="Pixel Barrel hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXBarrelEtaResidual',  path='EXPERT',type='TH1F',title="Pixel Barrel hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXBarrelL1PhiResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L1 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXBarrelL1EtaResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L1 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXBarrelL2PhiResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L2 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXBarrelL2EtaResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L2 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXBarrelL3PhiResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L3 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXBarrelL3EtaResidual',path='EXPERT',type='TH1F',title="Pixel Barrel L3 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXBarrelPhiPull',      path='EXPERT',type='TH1F',title="Pixel Barrel hit-track phi pull",xbins = 100, xmin=-5., xmax=5.)
        self.defineHistogram('hit_PIXBarrelEtaPull',      path='EXPERT',type='TH1F',title="Pixel Barrel hit-track eta pull",xbins = 100, xmin=-5., xmax=5.)
        #
        self.defineHistogram('hit_PIXEndcapPhiResidual',  path='EXPERT',type='TH1F',title="Pixel EC hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXEndcapEtaResidual',  path='EXPERT',type='TH1F',title="Pixel EC hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXEndcapL1PhiResidual',path='EXPERT',type='TH1F',title="Pixel EC L1 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXEndcapL1EtaResidual',path='EXPERT',type='TH1F',title="Pixel EC L1 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXEndcapL2PhiResidual',path='EXPERT',type='TH1F',title="Pixel EC L2 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXEndcapL2EtaResidual',path='EXPERT',type='TH1F',title="Pixel EC L2 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXEndcapL3PhiResidual',path='EXPERT',type='TH1F',title="Pixel EC L3 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_PIXEndcapL3EtaResidual',path='EXPERT',type='TH1F',title="Pixel EC L3 hit-track eta residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_PIXEndcapPhiPull',      path='EXPERT',type='TH1F',title="Pixel EC hit-track phi pull",xbins = 100, xmin=-5., xmax=5.)
        self.defineHistogram('hit_PIXEndcapEtaPull',      path='EXPERT',type='TH1F',title="Pixel EC hit-track eta pull",xbins = 100, xmin=-5., xmax=5.)
        #
        self.defineHistogram('hit_SCTBarrelResidual',     path='EXPERT',type='TH1F',title="SCT Barrel hit-track residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTBarrelL1PhiResidual',path='EXPERT',type='TH1F',title="SCT Barrel L1 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTBarrelL2PhiResidual',path='EXPERT',type='TH1F',title="SCT Barrel L2 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTBarrelL3PhiResidual',path='EXPERT',type='TH1F',title="SCT Barrel L3 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTBarrelL4PhiResidual',path='EXPERT',type='TH1F',title="SCT Barrel L4 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTBarrelPull',         path='EXPERT',type='TH1F',title="SCT Barrel hit-track pull",xbins = 100, xmin=-5., xmax=5.)
        #
        self.defineHistogram('hit_SCTEndcapResidual',     path='EXPERT',type='TH1F',title="SCT EC hit-track residual",xbins = 100, xmin=-1.0, xmax=1.0)
        self.defineHistogram('hit_SCTEndcapL1PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L1 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL2PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L2 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL3PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L3 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL4PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L4 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL5PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L5 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL6PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L6 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL7PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L7 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL8PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L8 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapL9PhiResidual',path='EXPERT',type='TH1F',title="SCT Endcap L9 hit-track phi residual",xbins = 100, xmin=-0.5, xmax=0.5)
        self.defineHistogram('hit_SCTEndcapPull',         path='EXPERT',type='TH1F',title="SCT EC hit-track pull",xbins = 100, xmin=-5., xmax=5.)


remap  = {
    "Muon"     : "muon",
    "MuonFS"   : "muon",
    "MuonLate" : "muon",
    "MuonCore" : "muonCore",
    "MuonIso"  : "muonIso",
    "eGamma"   : "electron",
    "Electron" : "electron",
    "Tau"      : "tau",
    "TauCore"  : "tauCore",
    "TauIso"   : "tauIso",
    "Jet"      : "bjet",
    "JetFS"    : "bjet",
    "FS"       : "bjet",
    "bjetVtx"  : "bjetVtx",
    "FullScan" : "fullScan",
    "BeamSpot" : "beamSpot",
    "Bphysics" : "bphysics",
    "Cosmic"   : "cosmics",
    "MinBias"  : "minBias400",
    "minBias"  : "minBias400"
}

class TrigFastTrackFinderBase(TrigFastTrackFinder):
    __slots__ = []
    def __init__(self, name, type):
        TrigFastTrackFinder.__init__(self,name)
        #Remapping should be now covered by SliceConfigurationSetting
        #remapped_type = remap[type]
        #assert(remapped_type is not None)
        remapped_type = type

        #Global keys/names for collections 
        from TrigInDetConfig.InDetTrigCollectionKeys import TrigPixelKeys, TrigSCTKeys


        self.useNewLayerNumberScheme = True
        
        self.OutputCollectionSuffix = type
        from AthenaCommon.AppMgr import ToolSvc

        numberingTool = TrigL2LayerNumberTool(name = "TrigL2LayerNumberTool_FTF")
        numberingTool.UseNewLayerScheme = self.useNewLayerNumberScheme
        ToolSvc += numberingTool
        self.LayerNumberTool = numberingTool
        
        from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings

        # GPU offloading config begins 
        
        self.useGPU = False
        
        #if type == "FS" : self.useGPU = True
        
        if self.useGPU :
            from TrigInDetAccelerationTool.TrigInDetAccelerationToolConf import TrigInDetAccelerationTool
            accelTool = TrigInDetAccelerationTool(name = "TrigInDetAccelerationTool_FTF")
            ToolSvc += accelTool
            
        
        # GPU offloading config ends

        self.doResMon = InDetTrigSliceSettings[('doResMon',remapped_type)]

        # switch between Run-2/3 monitoring
        self.MonTool = TrigFastTrackFinderMonitoring(type, self.doResMon)
        from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
        self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

        # Run3 monitoring
        self.MonTool = TrigFastTrackFinderMonitoring(type, self.doResMon)

        #Spacepoint conversion
        from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigSpacePointConversionTool
        spTool = TrigSpacePointConversionTool().clone('TrigSpacePointConversionTool_' + remapped_type)
        spTool.DoPhiFiltering = InDetTrigSliceSettings[('doSpPhiFiltering',remapped_type)]
        spTool.UseNewLayerScheme = self.useNewLayerNumberScheme
        spTool.UseBeamTilt = False
        spTool.PixelSP_ContainerName = TrigPixelKeys.SpacePoints
        spTool.SCT_SP_ContainerName  = TrigSCTKeys.SpacePoints
        spTool.layerNumberTool = numberingTool

        from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
        from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
      
        spTool.RegSel_Pixel = makeRegSelTool_Pixel()
        spTool.RegSel_SCT   = makeRegSelTool_SCT()

        ToolSvc += spTool
        self.SpacePointProviderTool=spTool
        self.MinHits = 5 #Only process RoI with more than 5 spacepoints
        
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

        if remapped_type=="cosmics":
          self.Doublet_FilterRZ = False

        ## SCT and Pixel detector elements road builder
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
        InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_FTF')
        InDetTrigSiDetElementsRoadMaker_FTF.RoadWidth = 10.0
        if remapped_type=="cosmics":
          from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigSiDetElementsRoadMakerCosmics
          InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMakerCosmics.clone('InDetTrigSiDetElementsRoadMaker_FTF')


        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiComTrackFinder
        InDetTrigSiComTrackFinder_FTF = InDetTrigSiComTrackFinder.clone("InDetTrigSiComTrackFinder_FTF")
        from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
        from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
        InDetTrigSiComTrackFinder_FTF.SctSummaryTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))
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
          TrackMaker_FTF.CosmicTrack=True

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
          theTrigZFinder = TrigZFinder( name="TrigZFinder_"+remapped_type )
          theTrigZFinder.NumberOfPeaks = 3
          theTrigZFinder.LayerNumberTool=numberingTool
          
          if remapped_type == "beamSpot" : 
            theTrigZFinder.TripletMode = 1
            theTrigZFinder.TripletDZ   = 1
            theTrigZFinder.PhiBinSize  = 0.1
            theTrigZFinder.UseOnlyPixels = True
            theTrigZFinder.MaxLayer      = 3

          theTrigZFinder.FullScanMode = True #TODO: know this from the RoI anyway - should set for every event

          self.trigZFinder = theTrigZFinder
          self.doFastZVertexSeeding = True
          self.zVertexResolution = 1

        TrackMaker_FTF.InputClusterContainerName = ""
        TrackMaker_FTF.InputHadClusterContainerName = ""
        
        
        from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
        self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

        if remapped_type == "tauCore":
          from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigTrackSummaryToolWithHoleSearch
          self.TrackSummaryTool = InDetTrigTrackSummaryToolWithHoleSearch

        self.doCloneRemoval = InDetTrigSliceSettings[('doCloneRemoval',remapped_type)]


class TrigFastTrackFinder_Muon(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Muon"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Muon","Muon")

class TrigFastTrackFinder_MuonFS(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_MuonFS"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_MuonFS","Muon")

class TrigFastTrackFinder_MuonLate(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_MuonLate"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_MuonLate","Muon")

class TrigFastTrackFinder_MuonIso(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_MuonIso"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_MuonIso","Muon")

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

class TrigFastTrackFinder_MinBias(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_MinBias"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_MinBias","MinBias")
