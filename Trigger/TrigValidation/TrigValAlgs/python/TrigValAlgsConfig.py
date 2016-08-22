# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigValAlgs.TrigValAlgsConf import TrigCountDumper
from TrigValAlgs.TrigValAlgsConf import TrigDecisionChecker
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
from TrigValAlgs.TrigValAlgsConf import TrigSlimValAlg
from TrigValAlgs.TrigValAlgsConf import TrigEDMAuxChecker
# TrigCountDumper configurable
# Run with TrigCountDumper configured from AOD header
class TrigCountDumper ( TrigCountDumper ):
  __slots__ = []
  def __init__(self, name="TrigCountDumper"):
    super( TrigCountDumper, self ).__init__( name )
    from AthenaCommon.Logging import logging  # loads logger
    log = logging.getLogger( name )
  
  def setDefaults(self, handle):
    WriteEventDecision=False
    MonitoredChains = [ ]
    MonitoringBlock = 100

# TrigDecisionChecker configurable
# Run with TrigDecisionTool configured from AOD header
class TrigDecisionChecker ( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecisionChecker"):
        super( TrigDecisionChecker, self ).__init__( name )

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        #print "TrigDecisionChecker.py : adding TrigDecisionTool"
        #from TrigDecision.TrigDecisionConfig import TrigDecisionTool_AOD
        #from AthenaCommon.AppMgr import ToolSvc
        #ToolSvc += TrigDecisionTool_AOD('TrigDecisionTool')


    def setDefaults(self, handle):

        # Use monitoring list to configure TrigDecChecker
        import TrigBjetMonitoring.TrigBjetMonitCategory as bjet
        import TrigBphysMonitoring.TrigBphysMonitCategory as bphys
        import TrigEgammaMonitoring.TrigEgammaMonitCategory as egamma
        import TrigJetMonitoring.TrigJetMonitCategory as jets
        import TrigMETMonitoring.TrigMETMonitCategory as met
        import TrigMinBiasMonitoring.TrigMinBiasMonitCategory as minbias
        import TrigMuonMonitoring.TrigMuonMonitCategory as muon
        import TrigTauMonitoring.TrigTauMonitCategory as tau
        
        #if not hasattr( handle, 'TrigDecisionTool' ) :
        #    print "TrigDecisionChecker.py : adding TrigDecisionTool"
        #    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
        #    handle.TrigDecisionTool = TrigDecisionTool('TrigDecisionTool')
        WriteEventDecision=False
        CheckTrigPassBits=True
        MonitoredChains = [ ]
        MonitoringBlock = 100

        self.MuonItems = muon.monitoring_muonNonIso + muon.monitoring_muonIso + muon.monitoring_MSonly + muon.monitoring_muonEFFS
        self.ElectronItems = egamma.monitoringTP_electron + egamma.monitoring_electron 
        self.ElectronItems.append('HLT_e0_perf_L1EM15')
        self.PhotonItems = egamma.monitoring_photon 
        self.MinBiasItems = minbias.monitoring_minbias
        self.JetItems = jets.primary_l1jet + jets.primary_jet
        self.BjetItems = bjet.monitoring_bjet
        self.TauItems = tau.monitoring_tau 
        self.MetItems = met.monitoring_met


# Run with TrigDecisionTool configured from XML files
class TrigDecisionChecker_XMLConfig( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecChecker"):
        super( TrigDecisionChecker_XMLConfig, self ).__init__( name )

    def setDefaults(self, handle):

        #if not hasattr( handle, 'TrigDecisionTool' ) :
        #    from TrigDecision.TrigDecisionConfig import TrigDecisionTool_XMLConfig
        #    handle.TrigDecisionTool = TrigDecisionTool_XMLConfig('TrigDecisionTool_xml')
        WriteEventDecision=False
        MonitoredChains = [ ]
        MonitoringBlock = 100


# TrigEDMChecker configurable
class TrigEDMChecker ( TrigEDMChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMChecker"):
        super( TrigEDMChecker, self ).__init__( name )

    def setDefaults(self, handle) :

        doDumpAll = True
        doDumpTrigMissingET = False
        doDumpMuonFeature = False
        doDumpCombinedMuonFeature = False
        doDumpTrigPhotonContainer = False
        doDumpTrigEMCluster = False
        doDumpTrigTauCluster = False
        doDumpTrigMuonEFContainer = False
        doDumpTrigElectronContainer = False
        doDumpTrigTau = False
        doDumpTrigInDetTrackCollection = False
        doDumpTrigVertexCollection = False
        doDumpTrigEFBphysContainer = False
        doDumpTrigL2BphysContainer = False
        doDumpTrigT2Jet = False
        doDumpTrigEFBjetContainer = False
        doDumpTrigL2BjetContainer = False

# TrigSlimValAlg configurable
class TrigSlimValAlg ( TrigSlimValAlg ):
  __slots__ = []
  def __init__(self, name="TrigSlimValAlg"):
    super( TrigSlimValAlg, self ).__init__( name )

  def setDefaults(self, handle):

    self.TrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"
    self.Navigation = "HLT::Navigation/Navigation"
    self.SlimmingTool = "HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool"


def getEDMAuxList():
    from TrigEDMConfig.TriggerEDM import getTriggerObjList,TriggerHLTList
    tlist=getTriggerObjList('AODFULL',[TriggerHLTList])
    objlist=[]
    for t,kset in tlist.iteritems():
        for k in kset:
             if 'Aux' in k: 
                 s = k.split('-',1)[0]
                 objlist += [s]
    return objlist 
# TrigEDMChecker configurable
class TrigEDMAuxChecker ( TrigEDMAuxChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMAuxChecker"):
        super( TrigEDMAuxChecker, self ).__init__( name )

    def setDefaults(self, handle) :
        #self.AuxContainerList=['HLT_xAOD__PhotonContainer_egamma_PhotonsAux.']
        self.AuxContainerList=getEDMAuxList()
    
    
