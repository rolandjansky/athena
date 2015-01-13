# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinderConf import TrigFastTrackFinder

from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigL2LayerNumberTool
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

from AthenaCommon.AppMgr import ToolSvc
import math

numberingTool = TrigL2LayerNumberTool()
ToolSvc += numberingTool


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

from AthenaCommon.SystemOfUnits import *

class CommonSettings() :
    def __init__(self):
        self.allowedInstanceNames = ['Muon', 'eGamma', 'muonIso', 'Tau', 'TauCore', 'TauIso',
                                     'Jet', 'Bphysics', 'FullScan', 'BeamSpot', 'Tile', 'FullScan_ZF_Only', 'Cosmic']
        self.db = {}
   
    def __getitem__(self, (quantity, slice)):
        v = None
        try:
            q = self.db[quantity]
            try:
                v = q[slice]
            except:
                print 'Settings has no instance %s ' % slice
        except:
            print 'Settings has no setting %s ' % quantity
        return v

    def __setitem__(self, (quantity, slice), value):
        try:
            q = self.db[quantity]
            try:
                q[slice] = value
            except:
                print 'Settings has no instance %s ' % slice
        except:
            print 'Settings has no setting %s ' % quantity

class ConfigurationFactory() :
    def __init__(self):
        self.settings = CommonSettings()
       
        width = {}
        usezvtool = {}
        seedsfilterlevel = {}
        tripletminptfrac = {}
        tripletnmaxphislice = {}
        tripletmaxbufferlength = {}
        doredundantseeds = {}
        ptmin = {}
        dospacepointphifiltering = {}
        doubletz0max = {}
        tripletd0max = {}
        tripletd0_pps_max = {}
        triplet_do_pss = {}
        trackinitiald0max = {}

        for i in self.settings.allowedInstanceNames :
            width[i] = 10.0
            usezvtool[i] = True
            seedsfilterlevel[i] = 0
            tripletminptfrac[i] = 1.0
            tripletnmaxphislice[i] = 53
            tripletmaxbufferlength[i] = 3
            doredundantseeds[i] = True
            ptmin[i] = 1.0*GeV
            dospacepointphifiltering[i] = True
            doubletz0max[i] = 230.0
            tripletd0max[i] = 4.0
            tripletd0_pps_max[i] = 1.7
            trackinitiald0max[i] = 20
            triplet_do_pss[i] = False
       
        doredundantseeds['Tau'] = False
        doredundantseeds['Jet'] = False

        doredundantseeds['Cosmic'] = False
        tripletnmaxphislice['Cosmic'] = 2
        doubletz0max['Cosmic'] = 1000.0
        tripletd0max['Cosmic'] = 1000.0
        tripletd0_pps_max['Cosmic'] = 1000.0
        trackinitiald0max['Cosmic'] = 1000.0



        # extend settings database
        self.settings.db['RoadWidth']=width
        self.settings.db['useZvertexTool'] = usezvtool
        self.settings.db['SeedsFilterLevel'] = seedsfilterlevel
        self.settings.db['Triplet_MinPtFrac'] = tripletminptfrac
        self.settings.db['Triplet_nMaxPhiSlice'] = tripletnmaxphislice
        self.settings.db['Triplet_MaxBufferLength'] = tripletmaxbufferlength
        self.settings.db['doSeedRedundancyCheck'] = doredundantseeds
        self.settings.db['pTmin'] = ptmin
        self.settings.db['doSpacePointPhiFiltering'] = dospacepointphifiltering 
        self.settings.db["Doublet_Z0Max"] = doubletz0max
        self.settings.db["Triplet_D0Max"] = tripletd0max
        self.settings.db["Triplet_D0_PPS_Max"] = tripletd0_pps_max
        self.settings.db["TrackInitialD0Max"] = trackinitiald0max
        self.settings.db["TripletDoPSS"] = triplet_do_pss

    def configureInstance(self, ftfInstance, instName) :
        if instName in self.settings.allowedInstanceNames :
            from AthenaCommon.AppMgr import ToolSvc

            #Spacepoint conversion
            from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigSpacePointConversionTool
            spTool = TrigSpacePointConversionTool()
            spTool.DoPhiFiltering = self.settings[('doSpacePointPhiFiltering',instName)]
            ToolSvc += spTool
            ftfInstance.SpacePointProviderTool=spTool

            ## SCT and Pixel detector elements road builder
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
            InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_FTF')
            InDetTrigSiDetElementsRoadMaker_FTF.RoadWidth = self.settings[('RoadWidth',instName)]
            if instName=="Cosmic": 
              from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigSiDetElementsRoadMakerCosmics
              InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMakerCosmics.clone('InDetTrigSiDetElementsRoadMaker_FTF')
            ToolSvc += InDetTrigSiDetElementsRoadMaker_FTF


            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiComTrackFinder
            InDetTrigSiComTrackFinder_FTF = InDetTrigSiComTrackFinder.clone("InDetTrigSiComTrackFinder_FTF")
            ToolSvc += InDetTrigSiComTrackFinder_FTF
            
            
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
            TrackingCuts = EFIDTrackingCuts
            if instName=="Cosmic":
              from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsCosmics
              TrackingCuts = EFIDTrackingCutsCosmics

            from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk

            TrackMaker_FTF = InDet__SiTrackMaker_xk(name = 'InDetTrigSiTrackMaker_FTF_'+instName,
                                                    RoadTool       = InDetTrigSiDetElementsRoadMaker_FTF,
                                                    CombinatorialTrackFinder = InDetTrigSiComTrackFinder_FTF,
                                                    pTmin          = self.settings[('pTmin',instName)],
                                                    nClustersMin   = TrackingCuts.minClusters(),
                                                    nHolesMax      = TrackingCuts.nHolesMax(),
                                                    nHolesGapMax   = TrackingCuts.nHolesGapMax(),
                                                    SeedsFilterLevel = self.settings[('SeedsFilterLevel',instName)],
                                                    Xi2max         = TrackingCuts.Xi2max(),
                                                    Xi2maxNoAdd    = TrackingCuts.Xi2maxNoAdd(),
                                                    nWeightedClustersMin= TrackingCuts.nWeightedClustersMin(),
                                                    Xi2maxMultiTracks         = TrackingCuts.Xi2max(),
                                                    UseAssociationTool       = False)

            if instName=="Cosmic":
              TrackMaker_FTF.RoadTool.CosmicTrack=True
            ToolSvc += TrackMaker_FTF
            ftfInstance.offlineTrackMaker = TrackMaker_FTF
            print TrackMaker_FTF
            print InDetTrigSiDetElementsRoadMaker_FTF

            ftfInstance.Triplet_MinPtFrac = self.settings[('Triplet_MinPtFrac',instName)]
            ftfInstance.Triplet_nMaxPhiSlice = self.settings[('Triplet_nMaxPhiSlice',instName)]
            ftfInstance.Triplet_MaxBufferLength = self.settings[('Triplet_MaxBufferLength',instName)]
            ftfInstance.doSeedRedundancyCheck = self.settings[('doSeedRedundancyCheck',instName)]
            ftfInstance.Doublet_Z0Max        = self.settings[('Doublet_Z0Max',instName)]
            ftfInstance.Triplet_D0Max        = self.settings[('Triplet_D0Max',instName)]
            ftfInstance.Triplet_D0_PPS_Max   = self.settings[('Triplet_D0_PPS_Max',instName)]
            ftfInstance.TrackInitialD0Max   = self.settings[('TrackInitialD0Max',instName)]
            ftfInstance.TripletDoPSS   = self.settings[('TripletDoPSS',instName)]

            ftfInstance.pTmin = self.settings[('pTmin',instName)]

        else :
            print "Instance "+instName+" of TrigFastTrackFinder is not supported!"
            ftfInstance.offlineTrackMaker = None
   

#Monitoring
class TrigFastTrackFinder_CommonMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFastTrackFinder_OnlineMonitoring"):
        super(TrigFastTrackFinder_CommonMonitoring, self).__init__(name)
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
                             labels='Start : GetRoI : GetSPs : MissingLayers : ZFinder : Groups : TrackFit' ) ]
   
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
    
   

class TrigFastTrackFinder_OnlineMonitoring(TrigFastTrackFinder_CommonMonitoring):
    def __init__ (self, name="TrigFastTrackFinder_OnlineMonitoring"):
        super(TrigFastTrackFinder_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addTrackHistograms()
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
    def __init__ (self, name="TrigFastTrackFinder_ValidationMonitoring"):
        super(TrigFastTrackFinder_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.addSPHistograms()
        self.addTimingHistograms(150,0.,150.)
        self.addDataErrorHistograms()
        self.addTrackHistograms()
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
    def __init__ (self, name="TrigFastTrackFinder_Cosmic_Monitoring"):
        super(TrigFastTrackFinder_Cosmic_Monitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.addSPHistograms()
        self.addDataErrorHistograms()
        self.addTrackHistograms()
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




class TrigFastTrackFinderBase(TrigFastTrackFinder):
    __slots__ = []
    def __init__(self, instName, seqName):
        TrigFastTrackFinder.__init__(self,instName)
        self.LayerNumberTool=numberingTool
        self.MinHits = 5
        
        self.retrieveBarCodes = False
        #self.SignalBarCodes = [10001] #single particles
        self.SignalBarCodes = [11 ,12] #z->mumu
       
        self.OutputCollectionSuffix=instName.replace("TrigFastTrackFinder_","")
        from AthenaCommon.AppMgr import ToolSvc

        timeHist = TrigTimeHistToolConfig("Time")
        timeHist.TimerHistLimits = [0,2000]
        self.AthenaMonTools = [ TrigFastTrackFinder_ValidationMonitoring("TrigFastTrackFinder_ValidationMonitoring"),
                                TrigFastTrackFinder_OnlineMonitoring("TrigFastTrackFinder_OnlineMonitoring"),
                                timeHist ]

        from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
        self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

        cfg = ConfigurationFactory()
        cfg.configureInstance(self,seqName)
        print self


class TrigFastTrackFinder_Muon(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Muon"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Muon","Muon")

class TrigFastTrackFinder_FullScan(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_FullScan"):
    TrigFastTrackFinderBase.__init__(self,"TrigFastTrackFinder_FullScan","FullScan")

class TrigFastTrackFinder_eGamma(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_eGamma"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_eGamma","eGamma")

class TrigFastTrackFinder_eGamma_L2(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_eGamma_L2"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_eGamma_L2","eGamma")

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

class TrigFastTrackFinder_Cosmic(TrigFastTrackFinderBase):
  def __init__(self, name = "TrigFastTrackFinder_Cosmic"):
    TrigFastTrackFinderBase.__init__(self, "TrigFastTrackFinder_Cosmic","Cosmic")
