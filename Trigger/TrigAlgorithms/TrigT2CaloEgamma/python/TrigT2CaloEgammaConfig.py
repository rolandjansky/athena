# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

from TrigT2CaloCalibration.EgammaCalibrationConfig import (EgammaHitsCalibrationBarrelConfig,
                                                           EgammaHitsCalibrationEndcapConfig,
                                                           EgammaGapCalibrationConfig,
                                                           EgammaTransitionRegionsConfig,
                                                           EgammaSshapeCalibrationBarrelConfig,
                                                           EgammaSshapeCalibrationEndcapConfig)

#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=======================================================================
# Defines a sub-container for the algorithm switches
class RingerReFexFlags(JobPropertyContainer):
    """ RingerReFex information """
    
# add the CaloRingerFlags flags container to the top container
jobproperties.add_Container(RingerReFexFlags)
#=======================================================================
class DoQuarter(JobProperty):
    """ master switch for quarter rings Algorithm
    """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False
jobproperties.RingerReFexFlags.add_JobProperty(DoQuarter)

class DumpCells(JobProperty):
    """ master switch for dump cells property
    """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False
jobproperties.RingerReFexFlags.add_JobProperty(DumpCells)

ringerReFexFlags = jobproperties.RingerReFexFlags
#=======================================================================

def enableDumpCells():
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import ringerReFexFlags
    ringerReFexFlags.DumpCells.set_Value_and_Lock(True)
    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3, addExtraCollectionsToEDMList
    addExtraCollectionsToEDMList(TriggerHLTListRun3,[['xAOD::TrigRingerRingsAuxContainer#HLT_FastCaloRingerAux.cells_eta.cells_phi.cells_et.cells_sampling.cells_size.rings_sum']]) 

class RingerReFexConfig( CompFactory.RingerReFex ):

  __slots__ = []

  def __init__(self, name = "RingerReMaker"):
    super(RingerReFexConfig, self).__init__(name)
    
    from TrigT2CaloEgamma.RingerConstants import Layer
    from TrigT2CaloEgamma.RingerConstants import DETID as det

    self.EtaBins              = [0.0000, 2.500] # bin pairs: min < eta <= max, PS,barrel,crack,endcap
    self.RingerKey            = "FastCaloRings"
    self.GlobalCenter         = False
    self.EtaSearchWindowSize  = 0.1
    self.PhiSearchWindowSize  = 0.1
    self.NRings               = [8, 64, 8, 8, 4, 4, 4]
    self.DeltaEta             = [0.025, 0.003125, 0.025, 0.05, 0.1, 0.1, 0.1]
    self.DeltaPhi             = [0.098174770424681, 0.098174770424681, 0.024543692606170, 0.024543692606170,
                                 0.098174770424681, 0.098174770424681, 0.098174770424681]
    self.UseTile              = True

    def same(value):
        return [value]*len(self.NRings)

    self.Detectors = [ [det.TTEM], [det.TTEM], [det.TTEM], [det.TTEM], [det.TTHEC, det.TILE], [det.TTHEC, det.TTHEC, det.TILE], [det.TTHEC, det.TILE] ]
    self.Samplings = [ [0]       , [1]       , [2]       , [3]       , [0        , -1  ]    , [1        , 2        , -1  ]    , [3        , -1      ] ]
    self.Samples   = [ 
                          [ Layer.PreSamplerB,Layer.PreSamplerE ], # TTEM:0
                          [ Layer.EMB1,       Layer.EME1 ], # TTEM: 1
                          [ Layer.EMB2,       Layer.EME2 ], # TTEM: 2
                          [ Layer.EMB3,       Layer.EME3 ], # TTEM: 3
                          [ Layer.HEC0,       Layer.TileBar0, Layer.TileGap2, Layer.TileExt0 ], # TTHEC: 0, TILE
                          [ Layer.HEC1,       Layer.HEC2,     Layer.TileBar1, Layer.TileGap0, Layer.TileExt1 ], # TTHEC: 1,2, TILE
                          [ Layer.HEC3,       Layer.TileBar2, Layer.TileGap1, Layer.TileExt2 ] # TTHEC: 3, TILE
                      ]

    # NOTE: This properties should be test soon
    #self.DoQuarter        = same( ringerReFexFlags.DoQuarter )
    self.DumpCells         = ringerReFexFlags.DumpCells.get_Value()
    self.DoQuarter         = same( False )
    self.DoEtaAxesDivision = same(True)
    self.DoPhiAxesDivision = same(True)
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram( "TIME_total", title="Total Time;time [us]",xbins=100, xmin=0, xmax=500,type='TH1F', path='EXPERT')
    monTool.defineHistogram( "TIME_load_cells", title="Load Cells Time;time [us]",xbins=50, xmin=0, xmax=100,type='TH1F', path='EXPERT')
    self.MonTool = monTool

#=======================================================================

class AsymRingerReFexConfig( RingerReFexConfig ):
  __slots__ = []
  def __init__(self, name = "AsymRingerReMaker"):
    super(AsymRingerReFexConfig, self).__init__(name)
    def same(value):
        return [value]*len(self.NRings)
    self.DoQuarter         = same(True)
    self.RingerKey         = "FastCaloAsymRings"

#=======================================================================

class T2CaloEgamma_All (CompFactory.T2CaloEgammaReFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_All"):
       super(T2CaloEgamma_All, self).__init__(name)
      
       from AthenaCommon.AppMgr import ServiceMgr as svcMgr
       if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
           from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
           svcMgr += TrigCaloDataAccessSvc()
       tool = CompFactory.EgammaAllFex("EgammaAllFex",trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
       tool.IncludeHad=True
       tool.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]
       self.IReAlgToolList= [tool]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.ExtraInputs = [( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTHEC' ),
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TILE' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALEM' ),
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALHAD' ), ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel') ]
    
#=======================================================================

class T2CaloEgamma_AllEm (CompFactory.T2CaloEgammaReFastAlgo):
   __slots__ = []
   def __init__ (self, name="T2CaloEgamma_AllEm"):
       super(T2CaloEgamma_AllEm, self).__init__(name)
       
       from AthenaCommon.AppMgr import ServiceMgr as svcMgr
       if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
           from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
           svcMgr += TrigCaloDataAccessSvc()
       tool = CompFactory.EgammaAllFex("EgammaAllEmFex",trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
       tool.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]
       self.IReAlgToolList= [tool]
       self.EtaWidth = 0.1
       self.PhiWidth = 0.1
       self.ExtraInputs = [( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTHEC' ),
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TILE' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALEM' ),
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALHAD' ), ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel') ]

#=======================================================================

class T2CaloEgamma_ReFastAlgo (CompFactory.T2CaloEgammaReFastAlgo):
    __slots__ = []
    def __init__ (self, name="T2CaloEgamma_ReFastAlgo", ClustersName="HLT_FastCaloEMClusters", doRinger=False, RingerKey="HLT_FastCaloRinger"):
        super(T2CaloEgamma_ReFastAlgo, self).__init__(name)
        # here put your customizations
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
            from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
            svcMgr += TrigCaloDataAccessSvc()
          
        samp2 = CompFactory.EgammaReSamp2Fex(name="ReFaAlgoSamp2FexConfig", trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc, MaxDetaHotCell=0.15, MaxDphiHotCell=0.15 )
        samp1 = CompFactory.EgammaReSamp1Fex("ReFaAlgoSamp1FexConfig", trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
        sampe = CompFactory.EgammaReEmEnFex("ReFaAlgoEmEnFexConfig", trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)
        samph = CompFactory.EgammaReHadEnFex("ReFaAlgoHadEnFexConfig", trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc)

        samph.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]

        self.IReAlgToolList = [ samp2, samp1, sampe, samph ]
        self.ExtraInputs = [( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTHEC' ), 
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TILE' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALEM' ), 
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALHAD' ), ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel') ]
        
        if doRinger:
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import RingerReFexConfig
            ringer = RingerReFexConfig('ReFaAlgoRingerFexConfig')
            ringer.RingerKey= RingerKey #"HLT_FastCaloRinger"
            ringer.trigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
            ringer.ClustersName = ClustersName
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
        monTool.defineHistogram('TrigEMCluster_eta,TIME_exec', path='EXPERT', type='TH2F', title="T2Calo Egamma time vs #eta ; #eta ; time [ us ]", 
                                 xbins=100, xmin=-2.5, xmax=2.5, ybins=80, ymin=0.0, ymax=8000.0)

        self.MonTool = monTool




class T2CaloEgamma_ReFastFWDAlgo (CompFactory.T2CaloEgammaForwardReFastAlgo):
    __slots__ = []
    def __init__ (self, name="T2CaloEgamma_ReFastFWDAlgo", ClustersName="HLT_FWDFastCaloEMClusters", 
                        doRinger=False, RingerKey="HLT_FWDFastCaloRinger"):
        super(T2CaloEgamma_ReFastFWDAlgo, self).__init__(name)
        # here put your customizations
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
            from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
            svcMgr += TrigCaloDataAccessSvc()

        self.IReAlgToolList = []
        self.ExtraInputs = [( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTEM' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TTHEC' ), 
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_TILE' ), ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALEM' ), 
                            ( 'IRegSelLUTCondData' , 'ConditionStore+RegSelLUTCondData_FCALHAD' ), ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel') ]
        
        self.EtaWidth = 0.2
        self.PhiWidth = 0.2

