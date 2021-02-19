# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReSamp1Fex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReSamp2Fex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReEmEnFex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import EgammaReHadEnFex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import RingerReFex
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgamma
#from TrigT2CaloEgamma.TrigT2CaloEgammaConf import T2CaloEgammaReFastAlgo


from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaHitsCalibrationBarrelConfig, EgammaHitsCalibrationEndcapConfig, EgammaGapCalibrationConfig
from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaTransitionRegionsConfig

from TrigT2CaloCalibration.EgammaCalibrationConfig import EgammaSshapeCalibrationBarrelConfig, EgammaSshapeCalibrationEndcapConfig
from TrigT2CaloEgamma.TrigT2CaloEgammaMonitoring import TrigT2CaloEgammaValMonitoring, TrigT2CaloEgammaCosmicMonitoring, TrigT2CaloEgammaOnMonitoring
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

t2catime = TrigTimeHistToolConfig("Time")

class EgammaReSamp2FexConfig (CompFactory.EgammaReSamp2Fex):
   __slots__ = []
   def __init__ (self, name="EgammaReSamp2FexConfig", *args, **kwargs):
       super(EgammaReSamp2FexConfig, self).__init__(name, *args, **kwargs)
       self.MaxDetaHotCell=0.15
       self.MaxDphiHotCell=0.15

class EgammaReSamp1FexConfig (CompFactory.EgammaReSamp1Fex):
   __slots__ = []
   def __init__ (self, name="EgammaReSamp1Fex", *args, **kwargs):
       super(EgammaReSamp1FexConfig, self).__init__(name, *args, **kwargs)

class EgammaReEmEnFexConfig (CompFactory.EgammaReEmEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaReEmEnFex", *args, **kwargs):
       super(EgammaReEmEnFexConfig, self).__init__(name, *args, **kwargs)

class EgammaReHadEnFexConfig (CompFactory.EgammaReHadEnFex):
   __slots__ = []
   def __init__ (self, name="EgammaReHadEnFex", *args, **kwargs):
       super(EgammaReHadEnFexConfig, self).__init__(name, *args, **kwargs)

class EgammaAllFex (CompFactory.EgammaAllFex):
   __slots__ = []
   def __init__ (self, name="EgammaAllFex"):
       super(EgammaAllFex, self).__init__(name)

class EgammaAllFexHadConfig (CompFactory.EgammaAllFex):
   __slots__ = []
   def __init__ (self, name="EgammaAllFexHadConfig"):
       super(EgammaAllFexHadConfig, self).__init__(name)
       self.IncludeHad=True

class RingerReFexConfig( CompFactory.RingerReFex ):

  __slots__ = []

  def __init__(self, name = "RingerReMaker"):
    super(RingerReFexConfig, self).__init__(name)
    self.EtaBins              = [0.0000, 999.999] # bin pairs: min < eta <= max, PS,barrel,crack,endcap
    self.GlobalCenter         = False
    self.RingerKey            = "L2CaloRings"
    self.EtaSearchWindowSize  = 0.1
    self.PhiSearchWindowSize  = 0.1
    self.DEtaRings            = [0.025, 0.003125, 0.025, 0.05, 0.1, 0.1, 0.1]
    self.DPhiRings            = [0.098174770424681, 0.098174770424681, 0.024543692606170, 0.024543692606170,
                                 0.098174770424681, 0.098174770424681, 0.098174770424681]
    self.NRings               = [8, 64, 8, 8, 4, 4, 4]
    self.NLayersRings         = [2, 2, 2, 2, 4, 5, 4]
    self.NMaxCells            = [320, 512, 272, 128, 128, 128, 128]
    self.UseHad               = True
    from TrigT2CaloEgamma.RingerConstants import Layer
    self.LayersRings          = [
                                 Layer.PreSamplerB,Layer.PreSamplerE,
                                 Layer.EMB1,       Layer.EME1,
                                 Layer.EMB2,       Layer.EME2,
                                 Layer.EMB3,       Layer.EME3,
                                 Layer.HEC0,       Layer.TileBar0, Layer.TileGap2, Layer.TileExt0,
                                 Layer.HEC1,       Layer.HEC2,     Layer.TileBar1, Layer.TileGap0, Layer.TileExt1,
                                 Layer.HEC3,       Layer.TileBar2, Layer.TileGap1, Layer.TileExt2
                                ]
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram( "TIME_total", title="Total Time;time[ms]",xbins=50, xmin=0, xmax=5,type='TH1F', path='EXPERT')
    monTool.defineHistogram( "TIME_load_cells", title="Load Cells Time;time[ms]",xbins=50, xmin=0, xmax=5,type='TH1F', path='EXPERT')
    self.MonTool = monTool


class T2CaloEgamma_All (CompFactory.T2CaloEgammaReFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_All"):
       super(T2CaloEgamma_All, self).__init__(name)
       self.IReAlgToolList= [EgammaAllFexHadConfig()]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       self.TrigEMClusterKey="TrigT2CaloAll"
       self.AthenaMonTools += [ TrigT2CaloEgammaValMonitoring(),
                                TrigT2CaloEgammaCosmicMonitoring(),
                                TrigT2CaloEgammaOnMonitoring()]
       self.doTiming=True

class T2CaloEgamma_AllEm (CompFactory.T2CaloEgammaReFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_AllEm"):
       super(T2CaloEgamma_AllEm, self).__init__(name)
       self.IReAlgToolList= [EgammaAllFex()]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.EtaWidthForID = 0.1
       self.PhiWidthForID = 0.1
       self.TrigEMClusterKey="TrigT2CaloAll"
       self.AthenaMonTools = [ ]
       self.doTiming=True

class T2CaloEgamma_ReFastAlgo (CompFactory.T2CaloEgammaReFastAlgo):
    __slots__ = []
    def __init__ (self, name="T2CaloEgamma_ReFastAlgo", ClustersName="HLT_FastCaloEMClusters", doRinger=False, RingerKey="HLT_FastCaloRinger"):
        super(T2CaloEgamma_ReFastAlgo, self).__init__(name)
        # here put your customizations
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
            from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
            svcMgr += TrigCaloDataAccessSvc()
        samp2 = EgammaReSamp2FexConfig(name="ReFaAlgoSamp2FexConfig",
                                        trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
        samp1 = EgammaReSamp1FexConfig("ReFaAlgoSamp1FexConfig",
                                        trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
        sampe = EgammaReEmEnFexConfig("ReFaAlgoEmEnFexConfig",
                                        trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
        samph = EgammaReHadEnFexConfig("ReFaAlgoHadEnFexConfig",
                                        trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)

        samph.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]

        self.IReAlgToolList = [ samp2, samp1, sampe, samph ]
        self.ExtraInputs = [( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTHEC' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TILE' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALHAD' ) ]
        
        if doRinger:
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import RingerReFexConfig
            ringer = RingerReFexConfig('ReFaAlgoRingerFexConfig')
            ringer.RingerKey= RingerKey #"HLT_FastCaloRinger"
            ringer.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
            ringer.ClustersName = ClustersName
            #ToolSvc+=ringer
            self.IReAlgToolList+= [ringer]

        self.EtaWidth = 0.2
        self.PhiWidth = 0.2
        #self.EtaWidthForID = 0.1
        #self.PhiWidthForID = 0.1
        #self.TrigEMClusterKey="TrigT2CaloEgamma"
        self.CalibListEndcap=[EgammaSshapeCalibrationEndcapConfig()]
        self.CalibListBarrel=[EgammaSshapeCalibrationBarrelConfig()]
        self.CalibListBarrel+=[EgammaHitsCalibrationBarrelConfig()]
        self.CalibListBarrel+=[EgammaGapCalibrationConfig()]
        self.CalibListBarrel+=[EgammaTransitionRegionsConfig()]
        self.CalibListEndcap+=[EgammaHitsCalibrationEndcapConfig()]
        self.CalibListEndcap+=[EgammaGapCalibrationConfig()]

        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool('MonTool')
        monTool.defineHistogram('TrigEMCluster_eT', path='EXPERT', type='TH1F', title="T2Calo Egamma E_T; E_T [ GeV ] ; Nclusters", xbins=80, xmin=0.0, xmax=80.0)
        monTool.defineHistogram('TrigEMCluster_had1', path='EXPERT', type='TH1F', title="T2Calo Egamma had E_T samp1; had E_T samp1 [ GeV ] ; Nclusters", xbins=80, xmin=0.0, xmax=8.0)
        monTool.defineHistogram('TrigEMCluster_eta', path='EXPERT', type='TH1F', title="T2Calo Egamma #eta; #eta ; Nclusters", xbins=100, xmin=-2.5, xmax=2.5)
        monTool.defineHistogram('TrigEMCluster_phi', path='EXPERT', type='TH1F', title="T2Calo Egamma #phi; #phi ; Nclusters", xbins=128, xmin=-3.2, xmax=3.2)
        monTool.defineHistogram('TrigEMCluster_rEta', path='EXPERT', type='TH1F', title="T2Calo Egamma rEta; rEta (e237/e277) ; Nclusters", xbins=140, xmin=-0.2, xmax=1.2)
        monTool.defineHistogram('TIME_exec', path='EXPERT', type='TH1F', title="T2Calo Egamma time; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0)
        monTool.defineHistogram('TrigEMCluster_eta,TIME_exec', path='EXPERT', type='TH2F', title="T2Calo Egamma time vs #eta ; #eta ; time [ us ]", xbins=100, xmin=-2.5, xmax=2.5, ybins=80, ymin=0.0, ymax=8000.0)

        self.MonTool = monTool

