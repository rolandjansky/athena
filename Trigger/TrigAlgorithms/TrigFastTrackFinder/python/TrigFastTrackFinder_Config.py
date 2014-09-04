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
                                     'Jet', 'Bphysics', 'FullScan', 'BeamSpot', 'Tile', 'FullScan_ZF_Only']
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
        nclustersmin = {}
        nholesmax = {}
        nholesgapmax = {}
        seedsfilterlevel = {}
        freeclusterscut = {}
        tripletminptfrac = {}
        tripletcutroieta = {}
        tripletcutroiphi = {}
        tripletdofilter = {} 
        usenewseeding   = {}
        tripletrzmax = {}
        tripletchi2max = {}
        tripletphiscale = {}
        tripletnbest = {}
        uselikelihood = {}
        tripletminllr = {}
        tripletmintriplets = {}
        tripletsmaxlayers = {}
        tripletsdomaxlayers = {}
        tripletcloneremovethr = {}
        doclonemerge = {}
        docloneremove = {}
        doredundantseeds = {}
        numseedstotry = {}
        clusterprefit = {}
        layeronedepth = {}
        layertwodepth = {}
        ptmin = {}
        dospacepointphifiltering = {}

        for i in self.settings.allowedInstanceNames :
            width[i] = 10.0
            usezvtool[i] = True
            nclustersmin[i] = 7
            nholesmax[i] = 2
            nholesgapmax[i] = 2
            seedsfilterlevel[i] = 0
            freeclusterscut[i] = 5
            tripletminptfrac[i] = 1.0
            tripletcutroieta[i] = False
            tripletcutroiphi[i] = False
            tripletdofilter[i] = True
            usenewseeding[i]   = True
            tripletrzmax[i] = 8.0
            tripletchi2max[i] = 25.0
            tripletphiscale[i] = 0.0006
            tripletnbest[i] = 2
            uselikelihood[i] = False
            tripletminllr[i] = -200.0
            tripletmintriplets[i] = 2
            tripletsmaxlayers[i] = 5
            tripletsdomaxlayers[i] = False
            tripletcloneremovethr[i] = 0.6
            doclonemerge[i] = False
            docloneremove[i] = False
            doredundantseeds[i] = False
            numseedstotry[i] = 2
            clusterprefit[i] = False
            layeronedepth[i] = 2
            layertwodepth[i] = 2
            ptmin[i] = 1.0*GeV
            dospacepointphifiltering[i] = True
       
        # customize setting here: for example width['Tau'] = 20.
        tripletdofilter['eGamma'] = True
        #tripletphiscale['eGamma'] =  0.005 # to account for brem
        clusterprefit['eGamma'] = True
        numseedstotry['eGamma'] = 1
        doredundantseeds['eGamma'] = True
        tripletcutroieta['eGamma'] = True
        tripletcutroiphi['eGamma'] = True

        tripletdofilter['Muon'] = True
        clusterprefit['Muon'] = True
        numseedstotry['Muon'] = 1
        doredundantseeds['Muon'] = True
        tripletcutroieta['Muon'] = True
        tripletcutroiphi['Muon'] = True
        
        tripletchi2max['Tau'] = 12.0
        clusterprefit['Tau'] = False
        tripletnbest['Tau'] = 2
        numseedstotry['Tau'] = 1
        doredundantseeds['Tau'] = False
        tripletdofilter['Tau'] = True
        tripletcutroieta['Tau'] = True
        tripletcutroiphi['Tau'] = True

        clusterprefit['Jet'] = False
        tripletnbest['Jet'] = 2
        numseedstotry['Jet'] = 1
        doredundantseeds['Jet'] = False
        tripletcutroieta['Jet'] = True
        tripletcutroiphi['Jet'] = True


        # extend settings database
        self.settings.db['RoadWidth']=width
        self.settings.db['useZvertexTool'] = usezvtool
        self.settings.db['nClustersMin'] = nclustersmin
        self.settings.db['nHolesMax'] = nholesmax
        self.settings.db['nHolesGapMax'] = nholesgapmax
        self.settings.db['SeedsFilterLevel'] = seedsfilterlevel
        self.settings.db['FreeClustersCut'] = freeclusterscut
        self.settings.db['Triplet_DoFilter'] = tripletdofilter
        self.settings.db['Triplet_MinPtFrac'] = tripletminptfrac
        self.settings.db['Triplet_CutRoiEta'] = tripletcutroieta
        self.settings.db['Triplet_CutRoiPhi'] = tripletcutroiphi
        self.settings.db['UseNewSeeding'] = usenewseeding
        self.settings.db['Triplet_RZMax'] = tripletrzmax
        self.settings.db['Triplet_Chi2Max'] = tripletchi2max
        self.settings.db['Triplet_FilterPhiScale'] = tripletphiscale
        self.settings.db['Triplet_NBest'] = tripletnbest
        self.settings.db['UseLikelihood'] = uselikelihood
        self.settings.db['Triplet_MinLLR'] = tripletminllr
        self.settings.db['Triplet_MinTriplets'] = tripletmintriplets
        self.settings.db['Triplet_MaxLayers'] = tripletsmaxlayers
        self.settings.db['Triplet_DoMaxLayers'] = tripletsdomaxlayers
        self.settings.db['Triplet_CloneRemoveThreshold'] = tripletcloneremovethr
        self.settings.db['doCloneMerge'] = doclonemerge
        self.settings.db['doCloneRemove'] = docloneremove
        self.settings.db['doSeedRedundancyCheck'] = doredundantseeds
        self.settings.db['numSeedsToTry'] = numseedstotry
        self.settings.db['Triplet_ClusterPrefit'] = clusterprefit
        self.settings.db['layerOneDepth'] = layeronedepth
        self.settings.db['layerTwoDepth'] = layertwodepth
        self.settings.db['pTmin'] = ptmin
        self.settings.db['doSpacePointPhiFiltering'] = dospacepointphifiltering 
        
    def configureInstance(self, ftfInstance, instName) :
        if instName in self.settings.allowedInstanceNames :
            from AthenaCommon.DetFlags import DetFlags
            from AthenaCommon.AppMgr import ToolSvc
            from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

            ## SCT and Pixel detector elements road builder
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPixelConditionsSummarySvc, InDetTrigSCTConditionsSummarySvc
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
            InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_FTF')
            InDetTrigSiDetElementsRoadMaker_FTF.RoadWidth = self.settings[('RoadWidth',instName)]
            ToolSvc += InDetTrigSiDetElementsRoadMaker_FTF

            if DetFlags.haveRIO.pixel_on():
                from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
                from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
                InDetTrigPixelConditionsSummarySvc = PixelConditionsSummarySvc(PixelConditionsSetup.instanceName('PixelConditionsSummarySvc'))
            else:
                InDetTrigPixelConditionsSummarySvc = None
           
            if DetFlags.haveRIO.SCT_on():
                from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
                from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
                InDetTrigSCTConditionsSummarySvc = SCT_ConditionsSummarySvc(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummarySvc'))
            else:
                InDetTrigSCTConditionsSummarySvc = None

            from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigSpacePointConversionTool
            spTool = TrigSpacePointConversionTool()
            spTool.DoPhiFiltering = self.settings[('doSpacePointPhiFiltering',instName)]
            ToolSvc += spTool
            ftfInstance.SpacePointProviderTool=spTool

           
            from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigPatternPropagator
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternUpdator, InDetTrigRotCreator
            
            CombinatorialTrackFinder_FTF = InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinder_FTF_'+instName,
                                                                                PropagatorTool = InDetTrigPatternPropagator,
                                                                                UpdatorTool    = InDetTrigPatternUpdator,
                                                                                RIOonTrackTool   = InDetTrigRotCreator,
                                                                                AssosiationTool  = InDetTrigPrdAssociationTool,
                                                                                usePixel         = DetFlags.haveRIO.pixel_on(),
                                                                                useSCT           = DetFlags.haveRIO.SCT_on(),
                                                                                PixelClusterContainer = 'PixelTrigClusters',
                                                                                SCT_ClusterContainer = 'SCT_TrigClusters',
                                                                                PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                                                SctSummarySvc = InDetTrigSCTConditionsSummarySvc)
            ToolSvc += CombinatorialTrackFinder_FTF

            from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk

            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
            TrackMaker_FTF = InDet__SiTrackMaker_xk(name = 'InDetTrigSiTrackMaker_FTF_'+instName,
                                                    RoadTool       = InDetTrigSiDetElementsRoadMaker_FTF,
                                                    CombinatorialTrackFinder = CombinatorialTrackFinder_FTF,
                                                    pTmin          = self.settings[('pTmin',instName)],
                                                    nClustersMin   = EFIDTrackingCuts.minClusters(),
                                                    nHolesMax      = EFIDTrackingCuts.nHolesMax(),
                                                    nHolesGapMax   = EFIDTrackingCuts.nHolesGapMax(),
                                                    SeedsFilterLevel = self.settings[('SeedsFilterLevel',instName)],
                                                    Xi2max         = EFIDTrackingCuts.Xi2max(),
                                                    Xi2maxNoAdd    = EFIDTrackingCuts.Xi2maxNoAdd(),
                                                    nWeightedClustersMin= EFIDTrackingCuts.nWeightedClustersMin(),
                                                    Xi2maxMultiTracks         = EFIDTrackingCuts.Xi2max(),
                                                    UseAssociationTool       = False)

            ToolSvc += TrackMaker_FTF
            ftfInstance.offlineTrackMaker = TrackMaker_FTF
            #print TrackMaker_FTF

            from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigInDetRoadMakerTool
            roadMakingTool = TrigInDetRoadMakerTool(name = 'TrigInDetRoadMakerTool_FTF_' + instName,
                                                    EtaBin = False, EtaBinWidth = 5.0, 
                                                    LayerOneDepth=self.settings[('layerOneDepth',instName)],
                                                    LayerTwoDepth=self.settings[('layerTwoDepth',instName)]) 


            print roadMakingTool
            ToolSvc += roadMakingTool
            ftfInstance.RoadMakerTool = roadMakingTool
            ftfInstance.FreeClustersCut = self.settings[('FreeClustersCut',instName)]
            ftfInstance.Triplet_DoFilter = self.settings[('Triplet_DoFilter',instName)]
            ftfInstance.Triplet_MinPtFrac = self.settings[('Triplet_MinPtFrac',instName)]
            ftfInstance.Triplet_CutRoiEta = self.settings[('Triplet_CutRoiEta',instName)]
            ftfInstance.Triplet_CutRoiPhi = self.settings[('Triplet_CutRoiPhi',instName)]
            ftfInstance.UseNewSeeding     = self.settings[('UseNewSeeding',instName)]
            ftfInstance.Triplet_RZMax = self.settings[('Triplet_RZMax',instName)]
            ftfInstance.Triplet_Chi2Max = self.settings[('Triplet_Chi2Max',instName)]
            ftfInstance.Triplet_FilterPhiScale = self.settings[('Triplet_FilterPhiScale',instName)]
            ftfInstance.Triplet_NBest = self.settings[('Triplet_NBest',instName)]
            ftfInstance.Triplet_UseLikelihood = self.settings[('UseLikelihood',instName)]
            ftfInstance.Triplet_MinLLR = self.settings[('Triplet_MinLLR',instName)]
            ftfInstance.Triplet_MinTriplets = self.settings[('Triplet_MinTriplets',instName)]
            ftfInstance.Triplet_MaxLayers = self.settings[('Triplet_MaxLayers',instName)]
            ftfInstance.Triplet_DoMaxLayers = self.settings[('Triplet_DoMaxLayers',instName)]
            ftfInstance.Triplet_CloneRemoveThreshold = self.settings[('Triplet_CloneRemoveThreshold',instName)]
            ftfInstance.doCloneMerge = self.settings[('doCloneMerge',instName)]
            ftfInstance.doCloneRemove = self.settings[('doCloneRemove',instName)]
            ftfInstance.doSeedRedundancyCheck = self.settings[('doSeedRedundancyCheck',instName)]
            ftfInstance.numSeedsToTry = self.settings[('numSeedsToTry',instName)]
            ftfInstance.Triplet_ClusterPrefit = self.settings[('Triplet_ClusterPrefit',instName)]
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
    def __init__(self, instName, seqName, etaHalfWidth = 0.1, phiHalfWidth = 0.1, doRefit = True):
        TrigFastTrackFinder.__init__(self,instName)
        self.LayerNumberTool=numberingTool
        self.doRefit       = doRefit
        #self.doRefit       = False
        #print instName
        #print seqName
       
        self.phiHalfWidth = phiHalfWidth
        self.etaHalfWidth = etaHalfWidth
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
