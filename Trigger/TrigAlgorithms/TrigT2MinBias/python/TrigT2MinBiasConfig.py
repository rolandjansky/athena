# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from TrigT2MinBias.TrigT2MinBiasConf import T2ZdcFex, T2ZdcHypo
from TrigT2MinBias.TrigT2MinBiasConf import T2MbtsFex, T2MbtsHypo 
from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePoints, TrigCountSpacePointsHypo
from TrigT2MinBias.TrigT2MinBiasConf import TrigCountTrtHits, TrigCountTrtHitsHypo

#from TrigOnlineSpacePointTool.TrigOnlineSpacePointTool_Config import ConfiguredOnlineSpacePointProviderTool
from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool

# Monitoring
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

# Properties for histogram dimensions
from TrigT2MinBias.TrigT2MinBiasProperties import trigT2MinBiasProperties

#ospTool = ConfiguredOnlineSpacePointProviderTool()
trtTool = ConfiguredTrigTRT_DriftCircleProviderTool

from AthenaCommon.AppMgr import ToolSvc

#ToolSvc += ospTool


fexes = {}
hypos = {}

#----------------------------------------------------------------------------------------
# SPC monitoring

class MbSpFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbSpFexMonitoring"):
        super(MbSpFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])


        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigSpacePointCounts
        # object.

        self.Histograms += [ defineHistogram('PixelClusEndcapC_Tot',title='Pixel CL ToT ECC;Time Over Threshold;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusTotBins(),
                                             xmin = trigT2MinBiasProperties.pixelClusTotMin(),
                                             xmax = trigT2MinBiasProperties.pixelClusTotMax(),
                                             opt="kVecUO") ]
        
        # Include the underflow and overflow bins as regular bins in the monitoring histogram 
        self.Histograms += [ defineHistogram('PixelClusEndcapC_Size',title='Pixel CL Size in ECC;CL Size;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusSizeBins()+2,
                                             xmin = trigT2MinBiasProperties.pixelClusSizeMin()-1.0,
                                             xmax = trigT2MinBiasProperties.pixelClusSizeMax()+1.0,
                                             opt="kVec") ]
        
        self.Histograms += [ defineHistogram('PixelClusBarrel_Tot',title='Pixel CL ToT in Barrel;Time Over Threshold;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusTotBins(),
                                             xmin = trigT2MinBiasProperties.pixelClusTotMin(),
                                             xmax = trigT2MinBiasProperties.pixelClusTotMax(),
                                             opt="kVecUO") ]
        
        # Include the underflow and overflow bins as regular bins in the monitoring histogram
        self.Histograms += [ defineHistogram('PixelClusBarrel_Size',title='Pixel CL Size in Barrel;Cluster Size;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusSizeBins()+2,
                                             xmin = trigT2MinBiasProperties.pixelClusSizeMin()-1.0,
                                             xmax = trigT2MinBiasProperties.pixelClusSizeMax()+1.0,
                                             opt="kVec") ]

        self.Histograms += [ defineHistogram('PixelClusEndcapA_Tot',title='Pixel CL ToT in ECA;Time Over Threshold;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusTotBins(),
                                             xmin = trigT2MinBiasProperties.pixelClusTotMin(),
                                             xmax = trigT2MinBiasProperties.pixelClusTotMax(),
                                             opt="kVecUO") ]

        # Include the underflow and overflow bins as regular bins in the monitoring histogram
        self.Histograms += [ defineHistogram('PixelClusEndcapA_Size',title='Pixel CL Size in ECA;CL Size;Pixel CL',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.pixelClusSizeBins()+2,
                                             xmin = trigT2MinBiasProperties.pixelClusSizeMin()-1.0,
                                             xmax = trigT2MinBiasProperties.pixelClusSizeMax()+1.0,
                                             opt="kVec") ]
        
        self.Histograms += [ defineHistogram('sctSpEndcapC',
                                             type  = 'TH1I',
                                             title = "SCT SP in ECC;SCT SP;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]
        
        self.Histograms += [ defineHistogram('sctSpBarrel',
                                             type  = 'TH1I',
                                             title = "SCT SP in Barrel;SCT SP;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]
        
        self.Histograms += [ defineHistogram('sctSpEndcapA',
                                             type  = 'TH1I',
                                             title = "SCT SP in ECA;SCT SP;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]
        

        self.Histograms += [ defineHistogram('PIX_SPCount, SCT_SPCount',
                                             type  = 'TH2I',
                                             title = "SP ; pix SP after ToT cut; sct SP",
                                             xbins = 50, xmin=-0.5, xmax=49.5,
                                             ybins = 50, ymin=-0.5, ymax=49.5)]

        #--------------------------------------------------------------------------------------------
        # more online monitoring
        #

        self.Histograms += [ defineHistogram('PIX_SPCount',
                                             type  = 'TH1I',
                                             title = "L2 total no. of PIX SP  after ToT cut; L2 PIX SP;evts",
                                             xbins = 2500, xmin=-0.5, xmax=4999.5)]
        self.Histograms += [ defineHistogram('SCT_SPCount',
                                             type  = 'TH1I',
                                             title = "L2 total no. of SCT SP;SCT SP;evts",
                                             xbins = 2500, xmin=-0.5, xmax=4999.5)]

        self.Histograms += [ defineHistogram('PixClBarrel',
                                             type  = 'TH1I',
                                             title = "L2 Barr PixSP; L2 PIX SP after ToT cut;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]

        self.Histograms += [ defineHistogram('PixClEndcapA',
                                             type  = 'TH1I',
                                             title = "L2 pix CL ECA;PIX CL after ToT cut;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]

        self.Histograms += [ defineHistogram('PixClEndcapC',
                                             type  = 'TH1I',
                                             title = "L2 pix CL in ECC;PIX CL after ToT cut;evts",
                                             xbins = 2000, xmin=-0.5, xmax=1999.5)]

        # monitor spacepoints per module
        self.Histograms += [ defineHistogram('PixSpPerModule',
                                             type  = 'TH1I',
                                             title = "L2 Pix SP per module;PIX SP after ToT cut;entries",
                                             xbins = 300, xmin=-0.5, xmax=299.5)]
                                             
        self.Histograms += [ defineHistogram('SctSpPerModule',
                                             type  = 'TH1I',
                                             title = "L2 Sct SP per module;SCT SP;entries",
                                             xbins = 500, xmin=-0.5, xmax=499.5)]

        self.Histograms += [ defineHistogram('RatioA',
                                             type  = 'TH1F',
                                             title = "ratioA; : pix cl size!=1/all;evts",
                                             xbins = 100, xmin=0., xmax=1.)]

        self.Histograms += [ defineHistogram('RatioB',
                                             type  = 'TH1F',
                                             title = "ratioB; : pix cl size==2/cl==1;evts",
                                             xbins = 100, xmin=0., xmax=10.)]

        # monitor data errors
                
        # PIX Errors are defined in : InnerDetector/InDetConditions/PixelConditionsServices/PixelConditionsServices/IPixelByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('PixelDataErrors',
                                             type='TH1F',
                                             title="Pixel DataErrors",
                                             xbins = 9 , xmin=-0.5, xmax=8.5,
                                             labels='PIX_TimeOut : PIX_BCID : PIX_L1ID : PIX_Preamble : PIX_Trailer : PIX_Flagged : PIX_DisabledFE : PIX_TruncatedROD : PIX_Decoding')]

        # SCT Errors are defined in : InnerDetector/InDetConditions/SCT_ConditionsServices/SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h
        self.Histograms += [ defineHistogram('SctDataErrors',
                                             type='TH1F',
                                             title="SCT BS Errors",
                                             xbins = 14 , xmin=-0.5, xmax=14.5,
                                             labels=' SCT_Decoding : SCT_TimeOut : SCT_BCID : SCT_L1ID : SCT_Preamble : SCT_Formatter : SCT_Trailer : SCT_TrailerOverFlow : SCT_HeaderTrailerLimit : SCT_ABCD : SCT_Raw : SCT_MaskedLink : SCT_RODClockError : SCT_TruncatedROD ')]

        # pixel monitoring histograms
        disks = ["Disk 1", "Disk 2", "Disk 3"]
        modules = ["M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0", "M1A", "M2A", "M3A", "M4A", "M5A", "M6A"]

        pixBarrL0 = ["B11_S2","B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
                    "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
                    "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
                    "B10_S1", "B10_S2", "B11_S1"]

        pixBarrL1 = ["B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
                    "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
                    "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
                    "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
                    "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
                    "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
                    "B19_S1", "B19_S2"]

        pixBarrL2 = ["B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
                    "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
                    "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
                    "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
                    "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
                    "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
                    "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2",
                    "B22_S1", "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2",
                    "B25_S1", "B25_S2", "B26_S1", "B26_S2", 
                    "B01_S1"]
        
        labelDescr = disks
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'

        self.Histograms += [ defineHistogram("pixECA_clust_occ_disk, pixECA_clust_occ_phi",
                                             type  = 'TH2I',
                                             title = "Pixel ECA cl occ; disk number ;phi index",
                                             xbins = 3, xmin=-0.5, xmax=2.5,
                                             ybins = 48,ymin=-0.5, ymax=47.5,
                                             labels=xyLabels) ]

        labelDescr = modules+pixBarrL0
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'

        self.Histograms += [ defineHistogram("pixBarrL0_clust_occ_eta, pixBarrL0_clust_occ_phi",
                                             type  = 'TH2I',
                                             title = "PixelBarrel layer0 cl occ; eta index ;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 22, ymin=-0.5, ymax=21.5,
                                             labels = xyLabels) ]
        labelDescr = modules+pixBarrL1
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'
            
        self.Histograms += [ defineHistogram("pixBarrL1_clust_occ_eta, pixBarrL1_clust_occ_phi",
                                             type  = 'TH2I',
                                             title = "PixelBarrel layer1 cl occ; eta index ;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 38, ymin=-0.5, ymax=37.5,
                                             labels = xyLabels) ]

        labelDescr = modules+pixBarrL2
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'
        
        self.Histograms += [ defineHistogram("pixBarrL2_clust_occ_eta, pixBarrL2_clust_occ_phi",
                                             type  = 'TH2I',
                                             title = "PixelBarrel layer2 cl occ; eta index ;phi index module",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 52, ymin=-0.5, ymax=51.5,
                                             labels = xyLabels) ]

        labelDescr = disks
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'
        
        self.Histograms += [ defineHistogram("pixECC_clust_occ_disk, pixECC_clust_occ_phi",
                                             type  = 'TH2I',
                                             title = "Pixel ECC cl occ; disk number ;phi index",
                                             xbins = 3, xmin=-0.5, xmax=2.5,
                                             ybins = 48,ymin=-0.5, ymax=47.5,
                                             labels = xyLabels) ]
        # hot module monitoring for sct
        disks = ["Disk 1", "Disk 2", "Disk 3", "Disk 4", "Disk 5", "Disk 6", "Disk 7", "Disk 8", "Disk 9" ]

        labelDescr = disks
        xyLabels = ''
        for c in labelDescr:
            xyLabels +=  c+':'

        self.Histograms += [ defineHistogram("sctECA_sp_occ_disk, sctECA_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SCT ECA disk spacepoint occupancy;  ;phi index",
                                             xbins = 9, xmin=-0.5, xmax=8.5,
                                             ybins = 56,ymin=-0.5, ymax=56.5,
                                             labels = xyLabels) ]

        self.Histograms += [ defineHistogram("sctECC_sp_occ_disk, sctECC_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SCT ECC disk sp occ;  ;phi index",
                                             xbins = 9, xmin=-0.5, xmax=8.5,
                                             ybins = 56,ymin=-0.5, ymax=56.5,
                                             labels = xyLabels) ]


        self.Histograms += [ defineHistogram("sctBarrL1_sp_occ_eta, sctBarrL1_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SctBarrel layer1 sp occ; eta index;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 56, ymin=-0.5, ymax=55.5 )]

        self.Histograms += [ defineHistogram("sctBarrL2_sp_occ_eta, sctBarrL2_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SctBarrel layer2 sp occ; eta index;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 56, ymin=-0.5, ymax=55.5 )]

        self.Histograms += [ defineHistogram("sctBarrL3_sp_occ_eta, sctBarrL3_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SctBarrel layer3 sp occ; eta index;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 56, ymin=-0.5, ymax=55.5 )]

        self.Histograms += [ defineHistogram("sctBarrL4_sp_occ_eta, sctBarrL4_sp_occ_phi",
                                             type  = 'TH2I',
                                             title = "SctBarrel layer4 sp occ; eta index;phi index",
                                             xbins = 13, xmin=-6.5, xmax=6.5,
                                             ybins = 56, ymin=-0.5, ymax=55.5 )]

        
class MbSpHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbSpHypoMonitoring"):
        super(MbSpHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])


        self.Histograms += [ defineHistogram('PIX_SPCountSel, SCT_SPCountSel',
                                             type  = 'TH2I',
                                             title = "Selected SP; pix SP after ToT cut; sct SP",
                                             xbins = 50, xmin=-0.5, xmax=49.5,
                                             ybins = 50, ymin=-0.5, ymax=49.5)]

        self.Histograms += [ defineHistogram('PIX_SPCountHypo, SCT_SPCountHypo',
                                             type  = 'TH2I',
                                             title = "SP hypo; pix SP after ToT cut; sct SP",
                                             xbins = 50, xmin=-0.5, xmax=49.5,
                                             ybins = 50, ymin=-0.5, ymax=49.5)]

        self.Histograms += [ defineHistogram('PIX_SPCountSel',
                                             type='TH1I',
                                             title="Selected SP; pix SP after ToT cut; events",
                                             xbins = 1250, xmin=-0.5, xmax=4999.5)]
        self.Histograms += [ defineHistogram('SCT_SPCountSel',
                                             type='TH1I',
                                             title="Selected SP; sct SP; events",
                                             xbins = 1250, xmin=-0.5, xmax=4999.5)]

        self.Histograms += [ defineHistogram('PIX_SPCountHypo',
                                             type='TH1I',
                                             title="Pixel SP after Hypo;L2 PIX SP; events",
                                             xbins = 1250, xmin=-0.5, xmax=4999.5)]
        self.Histograms += [ defineHistogram('SCT_SPCountHypo',
                                             type='TH1I',
                                             title="SCT SP after Hypo;L2 SCT SP; events",
                                             xbins = 1250, xmin=-0.5, xmax=4999.5)]

#----------------------------------------------------------------------------------------
# TRT monitoring

class MbTrtFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbTrtFexMonitoring"):
        super(MbTrtFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])


        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigSpacePointCounts
        # object.

        self.Histograms += [ defineHistogram('TrtRdoEndcapC_Tot',title='TRT RDO ToT ECC;Time Over Threshold;TRT RDO',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.trtHitTotBins(),
                                             xmin = trigT2MinBiasProperties.trtHitTotMin(),
                                             xmax = trigT2MinBiasProperties.trtHitTotMax(),
                                             opt="kVecUO") ]

        self.Histograms += [ defineHistogram('TrtRdoBarrel_Tot',title='TRT RDO ToT Barrel;Time Over Threshold;TRT RDO',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.trtHitTotBins(),
                                             xmin = trigT2MinBiasProperties.trtHitTotMin(),
                                             xmax = trigT2MinBiasProperties.trtHitTotMax(),
                                             opt="kVecUO") ]

        self.Histograms += [ defineHistogram('TrtRdoEndcapA_Tot',title='TRT RDO ToT ECA;Time Over Threshold;TRT RDO',
                                             type = 'TH1F',
                                             xbins = trigT2MinBiasProperties.trtHitTotBins(),
                                             xmin = trigT2MinBiasProperties.trtHitTotMin(),
                                             xmax = trigT2MinBiasProperties.trtHitTotMax(),
                                             opt="kVecUO") ]

        #---------------------------------------------------------------------------------------------
        # Other monitoring

        self.Histograms += [ defineHistogram('NTrtRdo',
                                             type  = 'TH1I',
                                             title = "Number of TRT Hits;# Hits;Evts",
                                             xbins = 2000, xmin=1000., xmax=11000.)]
        
        ## errors are defined in InnerDetector/InDetConditions/TRT_ConditionsServices/src/TRT_ByteStream_ConditionsSvc.cxx
        self.Histograms += [ defineHistogram('TrtDataErrors',
                                             type='TH1F',
                                             title="TRT BS Errors",
                                             xbins = 6 , xmin=-0.5, xmax=5.5,
                                             labels='TRT_L1ID : TRT_BCID : TRT_MISSING : TRT_ERROR : TRT_SID : TRT_ROB ')]


class MbTrtHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbTrtHypoMonitoring"):
        super(MbTrtHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])


        #--------------------------------------------------------------------------------------------
        # Monitoring of data stored in output TrigSpacePointCounts
        # object.

        self.Histograms += [ defineHistogram('NTrtRDO',title='TRT RDO after ToT cut;#trt hits;',
                                             type  = 'TH1I',
                                             xbins = 2000, xmin=1000., xmax=11000.5)]


# end of monitoring, what follws are configurations of algorithms
#----------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------
# SPC algos configurable

class MbSpFex ( TrigCountSpacePoints ) :
    __slots__ = []
    def __init__ (self, name):
        super(MbSpFex, self).__init__(name)
        
        # Set the histogram dimensions from TrigT2MinBiasProperties.
        self.PixelClusToTCut = trigT2MinBiasProperties.pixelClusToTCut()
        self.PixelClusTotBins = trigT2MinBiasProperties.pixelClusTotBins()
        self.PixelClusTotMin = trigT2MinBiasProperties.pixelClusTotMin()
        self.PixelClusTotMax = trigT2MinBiasProperties.pixelClusTotMax()
        self.PixelClusSizeBins = trigT2MinBiasProperties.pixelClusSizeBins()
        self.PixelClusSizeMin = trigT2MinBiasProperties.pixelClusSizeMin()
        self.PixelClusSizeMax = trigT2MinBiasProperties.pixelClusSizeMax()
        
        #self.SpacePointProviderTool = ospTool
        time = TrigTimeHistToolConfig("MbSpFexTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 400]
        self.AthenaMonTools += [ MbSpFexMonitoring(), time]


MbSpFex_1 = MbSpFex  # remove once cosmic clice migrated

L2MbSpFex =  MbSpFex("L2MbSpFex")
L2MbSpFex.PixelSP_ContainerName = "PixelTrigSpacePoints"
L2MbSpFex.SCT_SP_ContainerName = "SCT_TrigSpacePoints"
L2MbSpFex.PixelModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex.SCTModuleThreshold = 1000000 # Disable dynamic masking.
fexes["L2MbSpFex"] = L2MbSpFex

L2MbSpFex_BLayer =  MbSpFex("L2MbSpFex_BLayer")
L2MbSpFex_BLayer.PixelSP_ContainerName = "PixelTrigSpacePoints"
L2MbSpFex_BLayer.SCT_SP_ContainerName = "SCT_TrigSpacePoints"
L2MbSpFex_BLayer.PixelModuleThreshold = 30 # Enable dynamic masking.
L2MbSpFex_BLayer.SCTModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex_BLayer.OnlyCountBLayer = True # Pixel counter only uses inner BLayer
fexes["L2MbSpFex_BLayer"] = L2MbSpFex_BLayer

L2MbSpFex_ncb =  MbSpFex("L2MbSpFex_ncb")
L2MbSpFex_ncb.PixelSP_ContainerName = "PixelTrigSpacePoints"
L2MbSpFex_ncb.SCT_SP_ContainerName = "SCT_TrigSpacePoints"
L2MbSpFex_ncb.PixelModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex_ncb.SCTModuleThreshold = 30 # Enable dynamic masking.
L2MbSpFex_ncb.ReadPixelSp = False # Disable the Pixel readout
fexes["L2MbSpFex_ncb"] = L2MbSpFex_ncb

L2MbSpFex_SCTNoiseSup =  MbSpFex("L2MbSpFex_SCTNoiseSup")
L2MbSpFex_SCTNoiseSup.PixelSP_ContainerName = "PixelTrigSpacePoints"
L2MbSpFex_SCTNoiseSup.SCT_SP_ContainerName = "SCT_TrigSpacePoints"
L2MbSpFex_SCTNoiseSup.PixelModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex_SCTNoiseSup.SCTModuleThreshold = 40 # Set to remove noisy SCT endcap HV trips.
fexes["L2MbSpFex_SCTNoiseSup"] = L2MbSpFex_SCTNoiseSup

L2MbSpFex_noPix =  MbSpFex("L2MbSpFex_noPix")
L2MbSpFex_noPix.PixelSP_ContainerName = "PixelTrigSpacePoints"
L2MbSpFex_noPix.SCT_SP_ContainerName = "SCT_TrigSpacePoints"
L2MbSpFex_noPix.PixelModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex_noPix.SCTModuleThreshold = 1000000 # Disable dynamic masking.
L2MbSpFex_noPix.ReadPixelSp = False # Disable the Pixel readout
fexes["L2MbSpFex_noPix"] = L2MbSpFex_noPix
        
class MbSpHypo ( TrigCountSpacePointsHypo ):    
    __slots__ = []
    def __init__ (self, name="MbSpHypo"):
        super(MbSpHypo, self).__init__(name)
        self.PixelCLToTCut = trigT2MinBiasProperties.pixelClusToTCut()
        time = TrigTimeHistToolConfig("MbSpHypoTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ MbSpHypoMonitoring(), time]


MbSpHypo_1 = MbSpHypo  # remove once cosmic clice migrated


#             Sp hypo configuratables
L2MbSpHypo = MbSpHypo("L2MbSpHypo")
L2MbSpHypo.AcceptAll = False
L2MbSpHypo.Veto = False
L2MbSpHypo.TriggerTypeAND = True
L2MbSpHypo.TotalPixelClus = 2.
L2MbSpHypo.PixelClusRatioA = -1. # Disable
L2MbSpHypo.PixelClusRatioB = -1. # Disable
L2MbSpHypo.PixelClusEndcapC = -1. # Disable
L2MbSpHypo.PixelClusBarrel = -1. # Disable
L2MbSpHypo.PixelClusEndcapA = -1. # Disable
L2MbSpHypo.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo.TotalSctSp = 3.
L2MbSpHypo.SctSpEndcapC = -1. # Disable
L2MbSpHypo.SctSpBarrel = -1. # Disable
L2MbSpHypo.SctSpEndcapA = -1. # Disable
L2MbSpHypo.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo.SctSpBarrel_max = -1. # Disable
L2MbSpHypo.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpHypo"] = L2MbSpHypo

#             Sp hypo configuratables
L2MbSpHypo_veto = MbSpHypo("L2MbSpHypo_veto")
L2MbSpHypo_veto.AcceptAll = False
L2MbSpHypo_veto.Veto = True
L2MbSpHypo_veto.TriggerTypeAND = True
L2MbSpHypo_veto.TotalPixelClus = 2.
L2MbSpHypo_veto.PixelClusRatioA = -1. # Disable
L2MbSpHypo_veto.PixelClusRatioB = -1. # Disable
L2MbSpHypo_veto.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_veto.PixelClusBarrel = -1. # Disable
L2MbSpHypo_veto.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_veto.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_veto.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo_veto.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_veto.TotalSctSp = 3.
L2MbSpHypo_veto.SctSpEndcapC = -1. # Disable
L2MbSpHypo_veto.SctSpBarrel = -1. # Disable
L2MbSpHypo_veto.SctSpEndcapA = -1. # Disable
L2MbSpHypo_veto.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_veto.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_veto.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpHypo_veto"] = L2MbSpHypo_veto



#             Sp hypo configuratables veto large multiplicity
L2MbSpHypoLow = MbSpHypo("L2MbSpHypoLow")
L2MbSpHypoLow.AcceptAll = False
L2MbSpHypoLow.Veto = False
L2MbSpHypoLow.TriggerTypeAND = False ## ???
L2MbSpHypoLow.VetoLargeSP = True
L2MbSpHypoLow.TotalPixelClus = 200. # 
L2MbSpHypoLow.TotalPixelClusMin = 2. # Used only if  VetoLargeSP
L2MbSpHypoLow.PixelClusRatioA = -1. # Disable
L2MbSpHypoLow.PixelClusRatioB = -1. # Disable
L2MbSpHypoLow.PixelClusEndcapC = -1. # Disable
L2MbSpHypoLow.PixelClusBarrel = -1. # Disable
L2MbSpHypoLow.PixelClusEndcapA = -1. # Disable
L2MbSpHypoLow.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypoLow.PixelClusBarrel_max = -1. # Disable
L2MbSpHypoLow.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypoLow.TotalSctSp = -1. # Disable
L2MbSpHypoLow.TotalSctSpMin = -1. # Used only if  VetoLargeSP
L2MbSpHypoLow.SctSpEndcapC = -1. # Disable
L2MbSpHypoLow.SctSpBarrel = -1. # Disable
L2MbSpHypoLow.SctSpEndcapA = -1. # Disable
L2MbSpHypoLow.SctSpEndcapC_max = -1. # Disable
L2MbSpHypoLow.SctSpBarrel_max = -1. # Disable
L2MbSpHypoLow.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpHypoLow"] = L2MbSpHypoLow


L2MbSpHypo_PT = MbSpHypo("L2MbSpHypo_PT") # Pass through version
L2MbSpHypo_PT.AcceptAll = True
hypos["L2MbSpHypo_PT"] = L2MbSpHypo_PT

# ... oneD version
L2MbSpHypo_oneD = MbSpHypo("L2MbSpHypo_oneD")
L2MbSpHypo_oneD.AcceptAll = False
L2MbSpHypo_oneD.Veto = False
L2MbSpHypo_oneD.TriggerTypeAND = False
L2MbSpHypo_oneD.TotalPixelClus = -1. # Disable
L2MbSpHypo_oneD.PixelClusRatioA = -1. # Disable
L2MbSpHypo_oneD.PixelClusRatioB = 0.02
L2MbSpHypo_oneD.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_oneD.PixelClusBarrel = -1. # Disable
L2MbSpHypo_oneD.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_oneD.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_oneD.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo_oneD.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_oneD.PixelClusEndcaps_cut = -1. # Disable
L2MbSpHypo_oneD.TotalSctSp = -1. # Disable
L2MbSpHypo_oneD.SctSpEndcapC = -1. # Disable
L2MbSpHypo_oneD.SctSpBarrel = -1. # Disable
L2MbSpHypo_oneD.SctSpEndcapA = -1. # Disable
L2MbSpHypo_oneD.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_oneD.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_oneD.SctSpEndcapA_max = -1. # Disable
L2MbSpHypo_oneD.SctSpEndcaps_cut = -1. # Disable
hypos["L2MbSpHypo_oneD"] = L2MbSpHypo_oneD


# ... fwd version
L2MbSpHypo_fwd = MbSpHypo("L2MbSpHypo_fwd")
L2MbSpHypo_fwd.AcceptAll = False
L2MbSpHypo_fwd.Veto = False
L2MbSpHypo_fwd.TriggerTypeAND = False
L2MbSpHypo_fwd.TotalPixelClus = 5.
L2MbSpHypo_fwd.PixelClusRatioA = -1. # Disable
L2MbSpHypo_fwd.PixelClusRatioB = -1. # Disable
L2MbSpHypo_fwd.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_fwd.PixelClusBarrel = -1. # Disable
L2MbSpHypo_fwd.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_fwd.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_fwd.PixelClusBarrel_max = 30.
L2MbSpHypo_fwd.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_fwd.PixelClusEndcaps_cut = 2.
L2MbSpHypo_fwd.TotalSctSp = 4.
L2MbSpHypo_fwd.SctSpEndcapC = -1. # Disable
L2MbSpHypo_fwd.SctSpBarrel = -1. # Disable
L2MbSpHypo_fwd.SctSpEndcapA = -1. # Disable
L2MbSpHypo_fwd.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_fwd.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_fwd.SctSpEndcapA_max = -1. # Disable
L2MbSpHypo_fwd.SctSpEndcaps_cut = -1. # Disable
hypos["L2MbSpHypo_fwd"] = L2MbSpHypo_fwd


# ... cos version
L2MbSpHypo_cos = MbSpHypo("L2MbSpHypo_cos")
L2MbSpHypo_cos.AcceptAll = False
L2MbSpHypo_cos.Veto = False
L2MbSpHypo_cos.TriggerTypeAND = True
L2MbSpHypo_cos.TotalPixelClus = 3.
L2MbSpHypo_cos.PixelClusRatioA = -1. # Disable
L2MbSpHypo_cos.PixelClusRatioB = -1. # Disable
L2MbSpHypo_cos.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_cos.PixelClusBarrel = -1. # Disable
L2MbSpHypo_cos.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_cos.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_cos.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo_cos.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_cos.PixelClusEndcaps_cut = -1. # Disable
L2MbSpHypo_cos.TotalSctSp = 2.
L2MbSpHypo_cos.SctSpEndcapC = -1. # Disable
L2MbSpHypo_cos.SctSpBarrel = -1. # Disable
L2MbSpHypo_cos.SctSpEndcapA = -1. # Disable
L2MbSpHypo_cos.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_cos.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_cos.SctSpEndcapA_max = -1. # Disable
L2MbSpHypo_cos.SctSpEndcaps_cut = -1. # Disable
hypos["L2MbSpHypo_cos"] = L2MbSpHypo_cos

# Setup for 2016 SCT only non collision bacground filtering
# 2016: 50 SP, 2017 Sep+: 100 SP 
L2MbSpHypo_ncb = MbSpHypo("L2MbSpHypo_ncb")
L2MbSpHypo_ncb.AcceptAll = False
L2MbSpHypo_ncb.Veto = False
L2MbSpHypo_ncb.TriggerTypeAND = False # SCT only
L2MbSpHypo_ncb.SctSpEndcaps_cut = 100 # max(SCT_EC_A, SCT_EC_C)
L2MbSpHypo_ncb.TotalPixelClus = -1. # Disable
L2MbSpHypo_ncb.PixelClusRatioA = -1. # Disable
L2MbSpHypo_ncb.PixelClusRatioB = -1. # Disable
L2MbSpHypo_ncb.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_ncb.PixelClusBarrel = -1. # Disable
L2MbSpHypo_ncb.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_ncb.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_ncb.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo_ncb.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_ncb.PixelClusEndcaps_cut = -1. # Disable
L2MbSpHypo_ncb.TotalSctSp = -1. # Disable
L2MbSpHypo_ncb.SctSpEndcapC = -1. # Disable
L2MbSpHypo_ncb.SctSpBarrel = -1. # Disable
L2MbSpHypo_ncb.SctSpEndcapA = -1. # Disable
L2MbSpHypo_ncb.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_ncb.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_ncb.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpHypo_ncb"] = L2MbSpHypo_ncb

# Setup for 2016 PIX blayer counting
L2MbSpHypo_blayer = MbSpHypo("L2MbSpHypo_blayer")
L2MbSpHypo_blayer.AcceptAll = False
L2MbSpHypo_blayer.Veto = False
L2MbSpHypo_blayer.TriggerTypeAND = False # PIX only
L2MbSpHypo_blayer.SctSpEndcaps_cut = -1 # Disable
L2MbSpHypo_blayer.TotalPixelClus = 50 # Minimum number of hits in the BLayer (layer restriction in FEX) 
L2MbSpHypo_blayer.PixelClusRatioA = -1. # Disable
L2MbSpHypo_blayer.PixelClusRatioB = -1. # Disable
L2MbSpHypo_blayer.PixelClusEndcapC = -1. # Disable
L2MbSpHypo_blayer.PixelClusBarrel = -1. # Disable
L2MbSpHypo_blayer.PixelClusEndcapA = -1. # Disable
L2MbSpHypo_blayer.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypo_blayer.PixelClusBarrel_max = -1. # Disable
L2MbSpHypo_blayer.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypo_blayer.PixelClusEndcaps_cut = -1. # Disable
L2MbSpHypo_blayer.TotalSctSp = -1. # Disable
L2MbSpHypo_blayer.SctSpEndcapC = -1. # Disable
L2MbSpHypo_blayer.SctSpBarrel = -1. # Disable
L2MbSpHypo_blayer.SctSpEndcapA = -1. # Disable
L2MbSpHypo_blayer.SctSpEndcapC_max = -1. # Disable
L2MbSpHypo_blayer.SctSpBarrel_max = -1. # Disable
L2MbSpHypo_blayer.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpHypo_blayer"] = L2MbSpHypo_blayer

# ... setup for halo event trigger
L2MbSpHypoBg = MbSpHypo("L2MbSpHypoBg")
L2MbSpHypoBg.AcceptAll = False
L2MbSpHypoBg.Veto = False
L2MbSpHypoBg.TriggerTypeAND = False
L2MbSpHypoBg.TotalPixelClus = 9.
L2MbSpHypoBg.PixelClusRatioA = -1. # Disable
L2MbSpHypoBg.PixelClusRatioB = -1. # Disable
L2MbSpHypoBg.PixelClusEndcapC = -1. # Disable
L2MbSpHypoBg.PixelClusBarrel = -1. # Disable
L2MbSpHypoBg.PixelClusEndcapA = -1. # Disable
L2MbSpHypoBg.PixelClusEndcapC_max = -1. # Disable
L2MbSpHypoBg.PixelClusBarrel_max = -1. # Disable
L2MbSpHypoBg.PixelClusEndcapA_max = -1. # Disable
L2MbSpHypoBg.PixelClusEndcaps_cut = -1. # Disable
L2MbSpHypoBg.TotalSctSp = -1. # Disable
L2MbSpHypoBg.SctSpEndcapC = -1. # Disable
L2MbSpHypoBg.SctSpBarrel = -1. # Disable
L2MbSpHypoBg.SctSpEndcapA = -1. # Disable
L2MbSpHypoBg.SctSpEndcapC_max = -1. # Disable
L2MbSpHypoBg.SctSpBarrel_max = -1. # Disable
L2MbSpHypoBg.SctSpEndcapA_max = -1. # Disable
L2MbSpHypoBg.SctSpEndcaps_cut = -1. # Disable
hypos["L2MbSpHypoBg"] = L2MbSpHypoBg

# ... setup for MbSpMh
L2MbSpMhHypo = MbSpHypo("L2MbSpMhHypo")
L2MbSpMhHypo.AcceptAll = False
L2MbSpMhHypo.Veto = False
L2MbSpMhHypo.TriggerTypeAND = True
L2MbSpMhHypo.TotalPixelClus = 349.
L2MbSpMhHypo.PixelClusRatioA = -1. # Disable
L2MbSpMhHypo.PixelClusRatioB = -1. # Disable
L2MbSpMhHypo.PixelClusEndcapC = -1. # Disable
L2MbSpMhHypo.PixelClusBarrel = -1. # Disable
L2MbSpMhHypo.PixelClusEndcapA = -1. # Disable
L2MbSpMhHypo.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhHypo.PixelClusBarrel_max = -1. # Disable
L2MbSpMhHypo.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhHypo.TotalSctSp = 599.
L2MbSpMhHypo.SctSpEndcapC = -1. # Disable
L2MbSpMhHypo.SctSpBarrel = -1. # Disable
L2MbSpMhHypo.SctSpEndcapA = -1. # Disable
L2MbSpMhHypo.SctSpEndcapC_max = -1. # Disable
L2MbSpMhHypo.SctSpBarrel_max = -1. # Disable
L2MbSpMhHypo.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhHypo"] = L2MbSpMhHypo

# ... setup for MbSpMh120
L2MbSpMhPcHypo = MbSpHypo("L2MbSpMhPcHypo")
L2MbSpMhPcHypo.AcceptAll = False
L2MbSpMhPcHypo.Veto = False
L2MbSpMhPcHypo.TriggerTypeAND = True 
L2MbSpMhPcHypo.TotalPixelClus = 749.
L2MbSpMhPcHypo.PixelClusRatioA = -1. # Disable
L2MbSpMhPcHypo.PixelClusRatioB = -1. # Disable
L2MbSpMhPcHypo.PixelClusEndcapC = -1. # Disable
L2MbSpMhPcHypo.PixelClusBarrel = -1. # Disable
L2MbSpMhPcHypo.PixelClusEndcapA = -1. # Disable
L2MbSpMhPcHypo.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhPcHypo.PixelClusBarrel_max = -1. # Disable
L2MbSpMhPcHypo.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhPcHypo.TotalSctSp = 1499.
L2MbSpMhPcHypo.SctSpEndcapC = -1. # Disable
L2MbSpMhPcHypo.SctSpBarrel = -1. # Disable
L2MbSpMhPcHypo.SctSpEndcapA = -1. # Disable
L2MbSpMhPcHypo.SctSpEndcapC_max = -1. # Disable
L2MbSpMhPcHypo.SctSpBarrel_max = -1. # Disable
L2MbSpMhPcHypo.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhPcHypo"] = L2MbSpMhPcHypo


# ... setup for MbSpMhNoPixHypo low pile up
L2MbSpMhNoPixHypo = MbSpHypo("L2MbSpMhNoPixHypo")
L2MbSpMhNoPixHypo.AcceptAll = False
L2MbSpMhNoPixHypo.Veto = False
L2MbSpMhNoPixHypo.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo.TotalSctSp = 1999.
L2MbSpMhNoPixHypo.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo"] = L2MbSpMhNoPixHypo

# ... setup for MbSpMhNoPixHypo_eff
L2MbSpMhNoPixHypo_eff = MbSpHypo("L2MbSpMhNoPixHypo_eff")
L2MbSpMhNoPixHypo_eff.AcceptAll = False
L2MbSpMhNoPixHypo_eff.Veto = False
L2MbSpMhNoPixHypo_eff.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo_eff.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_eff.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo_eff.TotalSctSp = 1499.
L2MbSpMhNoPixHypo_eff.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_eff.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo_eff.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_eff.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_eff.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_eff.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo_eff"] = L2MbSpMhNoPixHypo_eff



# ... setup for MbSpMhNoPixHypo (medium pile-up)
L2MbSpMhNoPixHypo_medium = MbSpHypo("L2MbSpMhNoPixHypo_medium")
L2MbSpMhNoPixHypo_medium.AcceptAll = False
L2MbSpMhNoPixHypo_medium.Veto = False
L2MbSpMhNoPixHypo_medium.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo_medium.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_medium.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo_medium.TotalSctSp = 2999.
L2MbSpMhNoPixHypo_medium.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_medium.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo_medium.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_medium.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_medium.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_medium.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo_medium"] = L2MbSpMhNoPixHypo_medium

# ... setup for MbSpMhNoPixHypo_eff
L2MbSpMhNoPixHypo_medium_eff = MbSpHypo("L2MbSpMhNoPixHypo_medium_eff")
L2MbSpMhNoPixHypo_medium_eff.AcceptAll = False
L2MbSpMhNoPixHypo_medium_eff.Veto = False
L2MbSpMhNoPixHypo_medium_eff.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo_medium_eff.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.TotalSctSp = 2499.
L2MbSpMhNoPixHypo_medium_eff.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_medium_eff.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo_medium_eff"] = L2MbSpMhNoPixHypo_medium_eff




# ... setup for MbSpMhNoPixHypo high pile up
L2MbSpMhNoPixHypo_tight = MbSpHypo("L2MbSpMhNoPixHypo_tight")
L2MbSpMhNoPixHypo_tight.AcceptAll = False
L2MbSpMhNoPixHypo_tight.Veto = False
L2MbSpMhNoPixHypo_tight.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo_tight.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_tight.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo_tight.TotalSctSp = 4999.
L2MbSpMhNoPixHypo_tight.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_tight.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo_tight.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_tight.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_tight.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_tight.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo_tight"] = L2MbSpMhNoPixHypo_tight

# ... setup for MbSpMhNoPixHypo_eff
L2MbSpMhNoPixHypo_tight_eff = MbSpHypo("L2MbSpMhNoPixHypo_tight_eff")
L2MbSpMhNoPixHypo_tight_eff.AcceptAll = False
L2MbSpMhNoPixHypo_tight_eff.Veto = False
L2MbSpMhNoPixHypo_tight_eff.TriggerTypeAND = False # Must use OR because Pixel is disabled
L2MbSpMhNoPixHypo_tight_eff.TotalPixelClus = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusRatioA = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusRatioB = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusBarrel = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.PixelClusEndcapA_max = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.TotalSctSp = 4499.
L2MbSpMhNoPixHypo_tight_eff.SctSpEndcapC = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.SctSpBarrel = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.SctSpEndcapA = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.SctSpEndcapC_max = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.SctSpBarrel_max = -1. # Disable
L2MbSpMhNoPixHypo_tight_eff.SctSpEndcapA_max = -1. # Disable
hypos["L2MbSpMhNoPixHypo_tight_eff"] = L2MbSpMhNoPixHypo_tight_eff


class L2MbSpUPC(MbSpHypo):
    def __init__(self, name, minTh, maxTh):        
        super(L2MbSpUPC, self).__init__(name)
        self.VetoLargeSP = True
        self.TriggerTypeAND = False
        self.TotalPixelClus = maxTh+0.1 # this is negated selection due to the VetoLargeSP and we want inclusive 8 - 24 selection
        self.TotalPixelClusMin = minTh-0.1 
        self.TotalSctSp=-1


hypos["L2MbSpUPC_nominal"] = L2MbSpUPC("L2MbSpUPC_nominal", 8, 24)
hypos["L2MbSpUPC_loose"]   = L2MbSpUPC("L2MbSpUPC_loose", 6, 28)



# .... setup for HI High Multiplicity chains (SCT SP=2000)
class L2MbSpMhNoPixHypo_hip(MbSpHypo):
    def __init__(self, name, threshold):
        super(L2MbSpMhNoPixHypo_hip, self).__init__(name)
        
        self.AcceptAll = False
        self.TriggerTypeAND = False # Must use OR because Pixel is disabled
        self.TotalPixelClus = -1. # Disable
        self.PixelClusRatioA = -1. # Disable
        self.PixelClusRatioB = -1. # Disable
        self.PixelClusEndcapC = -1. # Disable
        self.PixelClusBarrel = -1. # Disable 
        self.PixelClusEndcapA = -1. # Disable
        self.PixelClusEndcapC_max = -1. # Disable
        self.PixelClusBarrel_max = -1. # Disable 
        self.PixelClusEndcapA_max = -1. # Disable
        self.TotalSctSp = threshold  # here is the only one actively used setting
        self.SctSpEndcapC = -1. # Disable
        self.SctSpBarrel = -1. # Disable
        self.SctSpEndcapA = -1. # Disable
        self.SctSpEndcapC_max = -1. # Disable
        self.SctSpBarrel_max = -1. # Disable
        self.SctSpEndcapA_max = -1. # Disable

# lowish cuts needed for pp
hypos["L2MbSpMhNoPixHypo_hip_600"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_600", 600.)

hypos["L2MbSpMhNoPixHypo_hip_700"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_700", 700.)

hypos["L2MbSpMhNoPixHypo_hip_800"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_800", 800.)

hypos["L2MbSpMhNoPixHypo_hip_900"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_900", 900.)

hypos["L2MbSpMhNoPixHypo_hip_1000"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1000", 1000.)

hypos["L2MbSpMhNoPixHypo_hip_1150"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1150", 1150.)

hypos["L2MbSpMhNoPixHypo_hip_1300"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1300", 1300.)

hypos["L2MbSpMhNoPixHypo_hip_1400"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1400", 1400.)

hypos["L2MbSpMhNoPixHypo_hip_1450"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1450", 1450.)

hypos["L2MbSpMhNoPixHypo_hip_1500"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1500", 1500.)

hypos["L2MbSpMhNoPixHypo_hip_1600"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1600", 1600.)

hypos["L2MbSpMhNoPixHypo_hip_1800"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_1800", 1800.)

hypos["L2MbSpMhNoPixHypo_hip_2000"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2000", 2000.)

hypos["L2MbSpMhNoPixHypo_hip_2200"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2200", 2200.)

hypos["L2MbSpMhNoPixHypo_hip_2400"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2400", 2400.)

hypos["L2MbSpMhNoPixHypo_hip_2600"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2600", 2600.)

# .... setup for HI High Multiplicity chains (SCT SP=2700) 
hypos["L2MbSpMhNoPixHypo_hip_2700"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2700", 2700.)
hypos["L2MbSpMhNoPixHypo_hip_2800"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_2800", 2800.)

# .... setup for HI High Multiplicity chains (SCT SP=3800) 
hypos["L2MbSpMhNoPixHypo_hip_3800"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_3800", 3800.)

# .... setup for HI High Multiplicity chains (SCT SP=4000)
hypos["L2MbSpMhNoPixHypo_hip_4000"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_4000", 4000.) 


# .... setup for HI High Multiplicity chains (SCT SP=4200)
hypos["L2MbSpMhNoPixHypo_hip_4200"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_4200", 4200.) 


# .... setup for HI High Multiplicity chains (SCT SP=4400)
hypos["L2MbSpMhNoPixHypo_hip_4400"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_4400", 4400.) 

# .... setup for HI High Multiplicity chains (SCT SP=4600)
hypos["L2MbSpMhNoPixHypo_hip_4600"] = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_4600", 4600.) 




#-----------------------------------------------------------------------
# TRT algos configurable

class MbTrtFex ( TrigCountTrtHits ) :
    __slots__ = []
    def __init__ (self, name="MbTrtFex"):
        super(MbTrtFex, self).__init__(name)

        # Set the histogram dimensions from TrigT2MinBiasProperties.
        self.TotBins = trigT2MinBiasProperties.trtHitTotBins()
        self.TotMin = trigT2MinBiasProperties.trtHitTotMin()
        self.TotMax = trigT2MinBiasProperties.trtHitTotMax()
        
        self.TrtDataProviderTool =  trtTool
        
        time = TrigTimeHistToolConfig("MbTrtFexTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 500]
        self.AthenaMonTools += [ MbTrtFexMonitoring(), time]

fexes["L2MbTrtFex"] = MbTrtFex("L2MbTrtFex")

MbTrtFex_1 = MbTrtFex # remove once cosmic clice migrated


class MbTrtHypo ( TrigCountTrtHitsHypo ):    
    __slots__ = []
    def __init__ (self, name="MbTrtHypo"):
        super(MbTrtHypo, self).__init__(name)

        time = TrigTimeHistToolConfig("MbTrtHypoTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 100]
        self.AthenaMonTools += [ MbTrtHypoMonitoring(), time]

MbTrtHypo_1 = MbTrtHypo # remove once cosmic clice migrated


#Trt hypo configurables
L2MbTrtHypo =  MbTrtHypo("L2MbTrtHypo")
L2MbTrtHypo.AcceptAll = False
L2MbTrtHypo.TimeOverThresholdCut = 18 
L2MbTrtHypo.TrtHitsEndcaps = 60.
L2MbTrtHypo.TotalTrtHits = -1.

hypos["L2MbTrtHypo"] = L2MbTrtHypo

#...PT version
L2MbTrtHypo_PT = MbTrtHypo("L2MbTrtHypo_PT") # Pass through
L2MbTrtHypo_PT.AcceptAll = True
del L2MbTrtHypo_PT.AthenaMonTools["MbTrtHypoTimers"]  # remove timing tools

hypos["L2MbTrtHypo_PT"] = L2MbTrtHypo_PT

# ...cos version
L2MbTrtHypo_cos =  MbTrtHypo("L2MbTrtHypo_cos")
L2MbTrtHypo_cos.AcceptAll = False
L2MbTrtHypo_cos.TimeOverThresholdCut = 18 
L2MbTrtHypo_cos.TrtHitsEndcaps = 7500.
L2MbTrtHypo_cos.TotalTrtHits = -1.
del L2MbTrtHypo_cos.AthenaMonTools["MbTrtHypoTimers"]  # remove timing tools


hypos["L2MbTrtHypo_cos"] = L2MbTrtHypo_cos


#-----------------------------------------------------------------------

# MBTS

class MbMbtsFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbMbtsFexMonitoring"):
        super(MbMbtsFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])
        self.Histograms += [ defineHistogram('TriggerEnergies,TriggerTimes',
                                             type='TH2F',
                                             title="MBTS Energy and Time from DSP;Energy [pC]; Time relative to L1 [ns];",
                                             xbins = 51, xmin=-5.5, xmax=45.5, 
                                             ybins = 100, ymin=-100., ymax=100.)]        

        self.Histograms += [ defineHistogram('BCID, TimeDiff_A_C',
                                             type='TH2F',
                                             title='Mbts Time Difference vs BCID; BCID; time difference [ns];',
                                             xbins = 3564, xmin=-0.5, xmax=3563.5,
                                             ybins = 20, ymin=-40., ymax=40.)]

        self.Histograms += [ defineHistogram('TriggerID,TriggerEnergies',
                                             type='TH2F',
                                             title='Mbts Energy vs MBTS ID; MBTS ID; Energy [pC];',
                                             xbins = 32, xmin=0,    xmax=32, 
                                             ybins = 51, ymin=-5.5, ymax=45.5)]        

        self.Histograms += [ defineHistogram('TriggerID,TriggerTimes',
                                             type='TH2F',
                                             title='MBTS Time vs MBTS ID; MBTS ID;Time relative to L1 [ns];',
                                             xbins = 32, xmin=0,    xmax=32, 
                                             ybins = 100, ymin=-100., ymax=100.)]        
	
        self.Histograms += [ defineHistogram('TimeDiff_A_C',
                                             type='TH1F',
                                             title="MBTS Fex time difference of mean times A-C;time diff [ns];",
                                             xbins = 100, xmin=-100., xmax=100.)]

        self.Histograms += [ defineHistogram('MultiplicityEBA, MultiplicityEBC',
                                             type='TH2I',
                                             title="Mbts Fex Multiplicity; multiplicity A-side;multiplicity C-side;",
                                             xbins = 17, xmin=-0.5, xmax=16.5,
                                             ybins = 17, ymin=-0.5, ymax=16.5)]
        
class MbMbtsHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbMbtsHypoMonitoring"):
        super(MbMbtsHypoMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])
        self.Histograms += [ defineHistogram('TimeDiff_A_C',
                                             type='TH1F',
                                             title="MBTS Hypo time difference of mean times A-C;time diff [ns];",
                                             xbins = 100, xmin=-100., xmax=100.)]
        self.Histograms += [ defineHistogram('MultiplicityEBA, MultiplicityEBC',
                                             type='TH2I',
                                             title="Mbts Hypo Multiplicity; multiplicity A-side;multiplicity C-side;",
                                             xbins = 17, xmin=-0.5, xmax=16.5,
                                             ybins = 17, ymin=-0.5, ymax=16.5)]

        self.Histograms += [ defineHistogram('SelMultiplicityEBA, SelMultiplicityEBC',
                                             type='TH2I',
                                             title="Mbts Multiplicity of selected events; mult A-side;mult C-side;",
                                             xbins = 17, xmin=-0.5, xmax=16.5,
                                             ybins = 17, ymin=-0.5, ymax=16.5)]

#class MbZdcFexMonitoring(TrigGenericMonitoringToolConfig):
#    def __init__ (self, name="MbZdcFexMonitoring"):
#        super(MbZdcFexMonitoring, self).__init__(name)
#        self.defineTarget( ["Online", "Validation", "Cosmic"])
#        self.Histograms += [ defineHistogram('TriggerEnergies,TriggerTimes',
#                                             type='TH2F',
#                                             title="MBTS Energy and Time from DSP;Energy [pC]; Time relative to L1 [ns];",
#                                             xbins = 51, xmin=-5.5, xmax=45.5, 
#                                             ybins = 100, ymin=-100., ymax=100.)]        


# end of monitoring tools config what foloows are fexes/hypos configs
#-----------------------------------------------------------------------

class MbMbtsFex( T2MbtsFex ):
    __slots__ = []
    def __init__ (self, name ):
        super(MbMbtsFex, self).__init__(name)
        self.Threshold = trigT2MinBiasProperties.mbtsThreshold() # pC        
        time = TrigTimeHistToolConfig("MbMbtsFexTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 50]
        self.AthenaMonTools += [ MbMbtsFexMonitoring(), time]

L2MbMbtsFex = MbMbtsFex("L2MbMbtsFex")
fexes["L2MbMbtsFex"] = L2MbMbtsFex



class MbMbtsHypo ( T2MbtsHypo ):
    __slots__ = []
    def __init__ (self, name ):
        super(MbMbtsHypo, self).__init__(name)
        self.Threshold = trigT2MinBiasProperties.mbtsThreshold() # pC
        self.AthenaMonTools += [ MbMbtsHypoMonitoring() ]


L2MbMbtsHypo_PT = MbMbtsHypo("L2MbMbtsHypo_PT") #pass through
L2MbMbtsHypo_PT.AcceptAll = True
L2MbMbtsHypo_PT.MbtsCounters = 2
L2MbMbtsHypo_PT.Coincidence = False
L2MbMbtsHypo_PT.TimeCut = -1 # Disable
#L2MbMbtsHypo_PT.Threshold = -100. # pC
hypos["L2MbMbtsHypo_PT"] = L2MbMbtsHypo_PT


L2MbMbtsHypo1NTime = MbMbtsHypo("L2MbMbtsHypo_1_NTime") # No time cut 
L2MbMbtsHypo1NTime.AcceptAll = False
L2MbMbtsHypo1NTime.MbtsCounters = 1
L2MbMbtsHypo1NTime.Coincidence = False
L2MbMbtsHypo1NTime.TimeCut = -1 # Disable
#L2MbMbtsHypo1NTime.Threshold = 0.18 # pC
hypos['L2MbMbtsHypo_1_NTime'] = L2MbMbtsHypo1NTime


L2MbMbtsHypo1 = MbMbtsHypo("L2MbMbtsHypo_1")
L2MbMbtsHypo1.AcceptAll = False
L2MbMbtsHypo1.MbtsCounters = 1
L2MbMbtsHypo1.Coincidence = False
L2MbMbtsHypo1.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo1.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_1"] = L2MbMbtsHypo1

L2MbMbtsHypo2NTime = MbMbtsHypo("L2MbMbtsHypo_2_NTime") # No time cut 
L2MbMbtsHypo2NTime.AcceptAll = False
L2MbMbtsHypo2NTime.MbtsCounters = 2
L2MbMbtsHypo2NTime.Coincidence = False
L2MbMbtsHypo2NTime.TimeCut = -1 # Disable
#L2MbMbtsHypo2NTime.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_2_NTime"] = L2MbMbtsHypo2NTime

L2MbMbtsHypo2 = MbMbtsHypo("L2MbMbtsHypo_2")
L2MbMbtsHypo2.AcceptAll = False
L2MbMbtsHypo2.MbtsCounters = 2
L2MbMbtsHypo2.Coincidence = False
L2MbMbtsHypo2.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo2.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_2"] =L2MbMbtsHypo2

L2MbMbtsHypo11NTime = MbMbtsHypo("L2MbMbtsHypo_1_1_NTime") # No time cut 
L2MbMbtsHypo11NTime.AcceptAll = False
L2MbMbtsHypo11NTime.MbtsCounters = 1
L2MbMbtsHypo11NTime.Coincidence = True
L2MbMbtsHypo11NTime.TimeCut = -1 # Disable
#L2MbMbtsHypo11NTime.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_1_1_NTime"] = L2MbMbtsHypo11NTime

L2MbMbtsHypo11 = MbMbtsHypo("L2MbMbtsHypo_1_1")
L2MbMbtsHypo11.AcceptAll = False
L2MbMbtsHypo11.MbtsCounters = 1
L2MbMbtsHypo11.Coincidence = True
L2MbMbtsHypo11.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_1_1"] = L2MbMbtsHypo11

L2MbMbtsHypo22 = MbMbtsHypo("L2MbMbtsHypo_2_2")
L2MbMbtsHypo22.AcceptAll = False
L2MbMbtsHypo22.MbtsCounters = 2
L2MbMbtsHypo22.Coincidence = True
L2MbMbtsHypo22.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_2_2"] = L2MbMbtsHypo22

L2MbMbtsHypo5 = MbMbtsHypo("L2MbMbtsHypo_5")
L2MbMbtsHypo5.AcceptAll = False
L2MbMbtsHypo5.MbtsCounters = 5
L2MbMbtsHypo5.Coincidence = False
L2MbMbtsHypo5.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_5"] = L2MbMbtsHypo5


L2MbMbtsHypo44 = MbMbtsHypo("L2MbMbtsHypo_4_4")
L2MbMbtsHypo44.AcceptAll = False
L2MbMbtsHypo44.MbtsCounters = 4
L2MbMbtsHypo44.Coincidence = True
L2MbMbtsHypo44.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_4_4"] = L2MbMbtsHypo44

L2MbMbtsHypo_3_3_or = MbMbtsHypo("L2MbMbtsHypo_3_3_or")
L2MbMbtsHypo_3_3_or.AcceptAll = False
L2MbMbtsHypo_3_3_or.MbtsCounters = 3
L2MbMbtsHypo_3_3_or.Coincidence = False
L2MbMbtsHypo_3_3_or.Or = True
L2MbMbtsHypo_3_3_or.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_3_3_or"] = L2MbMbtsHypo_3_3_or


## again for UPC events
L2MbMbtsHypo_4_4_or = MbMbtsHypo("L2MbMbtsHypo_4_4_or")
L2MbMbtsHypo_4_4_or.AcceptAll = False
L2MbMbtsHypo_4_4_or.MbtsCounters = 4
L2MbMbtsHypo_4_4_or.Coincidence = False
L2MbMbtsHypo_4_4_or.Or = True
L2MbMbtsHypo_4_4_or.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_4_4_or"] = L2MbMbtsHypo_4_4_or 

## hypo for CEP events
# the following mean that only events with 0_0, 1_0, 0_1 and 1_1 will pass
L2MbMbtsHypo_1_1_inn_veto = MbMbtsHypo("L2MbMbtsHypo_1_1_inn_veto")
L2MbMbtsHypo_1_1_inn_veto.AcceptAll = False
L2MbMbtsHypo_1_1_inn_veto.MbtsCounters = 2
L2MbMbtsHypo_1_1_inn_veto.Coincidence = True
L2MbMbtsHypo_1_1_inn_veto.Or = False
L2MbMbtsHypo_1_1_inn_veto.Veto = True
L2MbMbtsHypo_1_1_inn_veto.MBTSMode = 1 # inner
L2MbMbtsHypo_1_1_inn_veto.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_1_1_inn_veto"] = L2MbMbtsHypo_1_1_inn_veto 

#photoproduction in PbPb ATR-12470
# the following mean that only events with 0_2+, 2+_0, 2+_1 and 1_2+ will pass
L2MbMbtsHypo_1_1_inn_one_side_veto = MbMbtsHypo("L2MbMbtsHypo_1_1_inn_one_side_veto")
L2MbMbtsHypo_1_1_inn_one_side_veto.AcceptAll = False
L2MbMbtsHypo_1_1_inn_one_side_veto.MbtsCounters = 2
L2MbMbtsHypo_1_1_inn_one_side_veto.Coincidence = False
L2MbMbtsHypo_1_1_inn_one_side_veto.Or = True
L2MbMbtsHypo_1_1_inn_one_side_veto.Veto = True
L2MbMbtsHypo_1_1_inn_one_side_veto.MBTSMode = 1 # inner
L2MbMbtsHypo_1_1_inn_one_side_veto.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()
#L2MbMbtsHypo11.Threshold = 0.18 # pC
hypos["L2MbMbtsHypo_1_1_inn_one_side_veto"] = L2MbMbtsHypo_1_1_inn_one_side_veto

#########
### ZDC
#########

class MbZdcFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MbZdcFexMonitoring"):
        super(MbZdcFexMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"])
        self.Histograms += [ defineHistogram('TriggerEnergies,TriggerTimes',
                                             type='TH2F',
                                             title="ZDC Energy and Time from DSP;Energy [pC]; Time relative to L1 [ns];",
                                             xbins = 53, xmin=-5.5, xmax=100.5, 
                                             ybins = 100, ymin=-100., ymax=100.)]   


class MbZdcFex( T2ZdcFex ):
    __slots__ = []
    def __init__ (self, name="MbZdcFex" ):
        super(MbZdcFex, self).__init__(name)
        time = TrigTimeHistToolConfig("MbZdcFexTimers")
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ MbZdcFexMonitoring(), time]


### ZDC Fex (Old pre 2013) is replaced by L2MbZdcFex_LG see below
L2MbZdcFex = MbZdcFex("L2MbZdcFex")
L2MbZdcFex.ZdcEnRecoOption = 0 ## this parameter should control the LowGain=0 or HighGain=1
L2MbZdcFex.ZdcTimeRecoOption = 0
L2MbZdcFex.ZcdEnThreshold = -900
L2MbZdcFex.ZcdTimeThreshold = -900
L2MbZdcFex.ZcdChiThreshold = -900
fexes["L2MbZdcFex"] = L2MbZdcFex



### ZDC Fex HiGain
L2MbZdcFex_HG = MbZdcFex("L2MbZdcFex_HG")
L2MbZdcFex_HG.ZdcEnRecoOption = 1 ## this parameter should control the LowGain=0 or HighGain=1
L2MbZdcFex_HG.ZdcTimeRecoOption = 1
L2MbZdcFex_HG.ZcdEnThreshold = -900
L2MbZdcFex_HG.ZcdTimeThreshold = -900
L2MbZdcFex_HG.ZcdChiThreshold = -900
fexes["L2MbZdcFex_HG"] = L2MbZdcFex_HG


### ZDC High Gain
L2MbZdcFex_LG = MbZdcFex("L2MbZdcFex_LG")
L2MbZdcFex_LG.ZdcEnRecoOption = 0 ## this parameter should control the LowGain=0 or HighGain=1
L2MbZdcFex_LG.ZdcTimeRecoOption = 0
L2MbZdcFex_LG.ZcdEnThreshold = -900
L2MbZdcFex_LG.ZcdTimeThreshold = -900
L2MbZdcFex_LG.ZcdChiThreshold = -900
fexes["L2MbZdcFex_LG"] = L2MbZdcFex_LG


class ZdcHypoMonitoring(TrigGenericMonitoringToolConfig):
	def __init__ (self, name="ZdcHypoMonitoring"):
		super(ZdcHypoMonitoring, self).__init__(name)
		self.defineTarget( ["Online", "Validation", "Cosmic"])


		self.Histograms += [ defineHistogram('MultiplicityZDC_A',
					type  = 'TH1I',
					title = "Num ZDC Modules Side A",
					xbins = 5, xmin=-0.5, xmax=5.5)]

		self.Histograms += [ defineHistogram('MultiplicityZDC_C',
					type  = 'TH1I',
					title = "Num ZDC Modules Side C",
					xbins = 5, xmin=-0.5, xmax=5.5)]

		self.Histograms += [ defineHistogram('TimeZDC_A',
					type  = 'TH1F',
					title = "ZDC Mean Time Side A",
					xbins = 100, xmin=-10, xmax=10)]

		self.Histograms += [ defineHistogram('TimeZDC_C',
					type  = 'TH1F',
					title = "ZDC Mean Time Side C",
					xbins = 100, xmin=-10, xmax=10)]

		self.Histograms += [ defineHistogram('SumEnergyZDC_A',
					type  = 'TH1F',
                                        title = "ZDC Sum Energy Side A",
					xbins = 500, xmin=0, xmax=5000)]

		self.Histograms += [ defineHistogram('SumEnergyZDC_C',
                                             	type  = 'TH1F',
                                             	title = "ZDC Sum Energy Side C",
                                             	xbins = 500, xmin=0, xmax=5000)]

		self.Histograms += [ defineHistogram('TimeDiff_A_C',
                                             	type  = 'TH1F',
                                             	title = "ZDC Time Diff (A-C)",
                                             	xbins = 100, xmin=-10, xmax=10)]

# after selection

		self.Histograms += [ defineHistogram('SelMultiplicityZDC_A',
                                             	type  = 'TH1I',
                                             	title = "Num ZDC Modules Side A (After)",
                                             	xbins = 5, xmin=-0.5, xmax=5.5)]

		self.Histograms += [ defineHistogram('SelMultiplicityZDC_C',
                                             	type  = 'TH1I',
                                             	title = "Num ZDC Modules Side C (After)",
                                             	xbins = 5, xmin=-0.5, xmax=5.5)]

		self.Histograms += [ defineHistogram('SelTimeZDC_A',
                                             	type  = 'TH1F',
                                             	title = "ZDC Mean Time Side A (After)",
                                             	xbins = 100, xmin=-10, xmax=10)]

		self.Histograms += [ defineHistogram('SelTimeZDC_C',
                                             	type  = 'TH1F',
                                             	title = "ZDC Mean Time Side C (After)",
                                             	xbins = 100, xmin=-10, xmax=10)]

		self.Histograms += [ defineHistogram('SelSumEnergyZDC_A',
                                             	type  = 'TH1F',
                                             	title = "ZDC Sum Energy Side A (After)",
                                             	xbins = 500, xmin=0, xmax=5000)]

		self.Histograms += [ defineHistogram('SelSumEnergyZDC_C',
                                             	type  = 'TH1F',
                                             	title = "ZDC Sum Energy Side C (After)",
                                             	xbins = 500, xmin=0, xmax=5000)]

		self.Histograms += [ defineHistogram('SelTimeDiff_A_C',
                                             	type  = 'TH1F',
                                             	title = "ZDC Time Diff (A-C) (After)",
                                             	xbins = 100, xmin=-10, xmax=10)]



class ZdcHypo ( T2ZdcHypo ):    
    __slots__ = []
    def __init__ (self, name="ZdcHypo"):
        super(ZdcHypo, self).__init__(name)
        time = TrigTimeHistToolConfig("ZdcHypoTimers")
        time.NumberOfHistBins = 100
        time.TimerHistLimits = [0, 10]
        self.AthenaMonTools += [ ZdcHypoMonitoring(), time]


L2MbZdcHypo_PT = ZdcHypo("L2MbZdcHypo_PT")
L2MbZdcHypo_PT.AcceptAll = True
L2MbZdcHypo_PT.TimeLogic = 0
L2MbZdcHypo_PT.EnergyLogic = 1
L2MbZdcHypo_PT.MultiplicityLogic = 0
L2MbZdcHypo_PT.TimeOffset = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_PT.Pedestal = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_PT.EnergyCalibration = [1., 1., 1., 1., 
	1., 1., 1., 1.]
L2MbZdcHypo_PT.TimeModuleCut = 99999.
L2MbZdcHypo_PT.SumEnergyCut = [-1., 99999., -1., 99999.]
L2MbZdcHypo_PT.MultCut = [ -1 , -1 ]
L2MbZdcHypo_PT.TimeCut= -1.
hypos["L2MbZdcHypo_PT"] = L2MbZdcHypo_PT

### ZDC single side low threshold side A (Pb remnant is A)
L2MbZdcHypo_hip_low_sideA = ZdcHypo("L2MbZdcHypo_hip_low_sideA")
L2MbZdcHypo_hip_low_sideA.AcceptAll = False
L2MbZdcHypo_hip_low_sideA.TimeLogic = 0
L2MbZdcHypo_hip_low_sideA.EnergyLogic = 2 ## OR
L2MbZdcHypo_hip_low_sideA.MultiplicityLogic = 0
L2MbZdcHypo_hip_low_sideA.TimeOffset = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_low_sideA.Pedestal = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_low_sideA.EnergyCalibration = [1., 1., 1., 1., 
	1., 1., 1., 1.]
L2MbZdcHypo_hip_low_sideA.TimeModuleCut = 99999.
L2MbZdcHypo_hip_low_sideA.SumEnergyCut = [180., 99999., 99999., -1] # 1<A<2 || 3<C<4
L2MbZdcHypo_hip_low_sideA.MultCut = [ -1 , -1 ]
L2MbZdcHypo_hip_low_sideA.TimeCut= -1.
hypos["L2MbZdcHypo_hip_low_sideA"] = L2MbZdcHypo_hip_low_sideA


### ZDC single side low threshold side C (Pb remnant is C)
L2MbZdcHypo_hip_low_sideC = ZdcHypo("L2MbZdcHypo_hip_low_sideC")
L2MbZdcHypo_hip_low_sideC.AcceptAll = False
L2MbZdcHypo_hip_low_sideC.TimeLogic = 0
L2MbZdcHypo_hip_low_sideC.EnergyLogic = 2 ## OR
L2MbZdcHypo_hip_low_sideC.MultiplicityLogic = 0
L2MbZdcHypo_hip_low_sideC.TimeOffset = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_low_sideC.Pedestal = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_low_sideC.EnergyCalibration = [1., 1., 1., 1., 
	1., 1., 1., 1.]
L2MbZdcHypo_hip_low_sideC.TimeModuleCut = 99999.
L2MbZdcHypo_hip_low_sideC.SumEnergyCut = [99999.,-1., 180., 99999.] # 1<A<2 || 3<C<4
L2MbZdcHypo_hip_low_sideC.MultCut = [ -1 , -1 ]
L2MbZdcHypo_hip_low_sideC.TimeCut= -1.
hypos["L2MbZdcHypo_hip_low_sideC"] = L2MbZdcHypo_hip_low_sideC


### ZDC single side high threshold side A (Pb remnant is A)
L2MbZdcHypo_hip_hi_sideA = ZdcHypo("L2MbZdcHypo_hip_hi_sideA")
L2MbZdcHypo_hip_hi_sideA.AcceptAll = False
L2MbZdcHypo_hip_hi_sideA.TimeLogic = 0
L2MbZdcHypo_hip_hi_sideA.EnergyLogic = 2 ## OR
L2MbZdcHypo_hip_hi_sideA.MultiplicityLogic = 0
L2MbZdcHypo_hip_hi_sideA.TimeOffset = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_hi_sideA.Pedestal = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_hi_sideA.EnergyCalibration = [1., 1., 1., 1., 
	1., 1., 1., 1.]
L2MbZdcHypo_hip_hi_sideA.TimeModuleCut = 99999.
L2MbZdcHypo_hip_hi_sideA.SumEnergyCut = [300., 99999., 99999., -1.] # 1<A<2 || 3<C<4
L2MbZdcHypo_hip_hi_sideA.MultCut = [ -1 , -1 ]
L2MbZdcHypo_hip_hi_sideA.TimeCut= -1.
hypos["L2MbZdcHypo_hip_hi_sideA"] = L2MbZdcHypo_hip_hi_sideA


### ZDC single side high threshold side C (Pb remnant is C)
L2MbZdcHypo_hip_hi_sideC = ZdcHypo("L2MbZdcHypo_hip_hi_sideC")
L2MbZdcHypo_hip_hi_sideC.AcceptAll = False
L2MbZdcHypo_hip_hi_sideC.TimeLogic = 0
L2MbZdcHypo_hip_hi_sideC.EnergyLogic = 2 ## OR
L2MbZdcHypo_hip_hi_sideC.MultiplicityLogic = 0
L2MbZdcHypo_hip_hi_sideC.TimeOffset = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_hi_sideC.Pedestal = [0., 0., 0., 0., 
	0., 0., 0., 0.]
L2MbZdcHypo_hip_hi_sideC.EnergyCalibration = [1., 1., 1., 1., 
	1., 1., 1., 1.]
L2MbZdcHypo_hip_hi_sideC.TimeModuleCut = 99999.
L2MbZdcHypo_hip_hi_sideC.SumEnergyCut = [99999.,-1., 300., 99999.] # 1<A<2 || 3<C<4
L2MbZdcHypo_hip_hi_sideC.MultCut = [ -1 , -1 ]
L2MbZdcHypo_hip_hi_sideC.TimeCut= -1.
hypos["L2MbZdcHypo_hip_hi_sideC"] = L2MbZdcHypo_hip_hi_sideC

### ZDC both sides low threshold
L2MbZdcHypo_sideAC_zdc_LG = ZdcHypo("L2MbZdcHypo_sideAC_zdc_LG")
L2MbZdcHypo_sideAC_zdc_LG.AcceptAll = False
L2MbZdcHypo_sideAC_zdc_LG.TimeLogic = 0
L2MbZdcHypo_sideAC_zdc_LG.EnergyLogic = 1 ## AND
L2MbZdcHypo_sideAC_zdc_LG.MultiplicityLogic = 0
L2MbZdcHypo_sideAC_zdc_LG.TimeOffset = [0., 0., 0., 0., 0., 0., 0., 0.]
L2MbZdcHypo_sideAC_zdc_LG.Pedestal = [0., 0., 0., 0., 0., 0., 0., 0.]
L2MbZdcHypo_sideAC_zdc_LG.EnergyCalibration = [1., 1., 1., 1., 1., 1., 1., 1.]
L2MbZdcHypo_sideAC_zdc_LG.TimeModuleCut = 99999.
L2MbZdcHypo_sideAC_zdc_LG.SumEnergyCut = [0.,trigT2MinBiasProperties.zdcVetoThresholdLG(), 0.,trigT2MinBiasProperties.zdcVetoThresholdLG()] # 1<A<2 || 3<C<4 - LONG LINE!
L2MbZdcHypo_sideAC_zdc_LG.MultCut = [ -1 , -1 ]
L2MbZdcHypo_sideAC_zdc_LG.TimeCut= -1.
hypos["L2MbZdcHypo_sideAC_zdc_LG"] = L2MbZdcHypo_sideAC_zdc_LG
