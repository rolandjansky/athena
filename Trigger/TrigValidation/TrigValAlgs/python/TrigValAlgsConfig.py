# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigValAlgs.TrigValAlgsConf import TrigDecisionChecker
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
from TrigValAlgs.TrigValAlgsConf import TrigEDMAuxChecker
import six

# TrigDecisionChecker configurable
# Run with TrigDecisionTool configured from AOD header
class TrigDecisionChecker ( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecisionChecker"):
        super( TrigDecisionChecker, self ).__init__( name )

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

        self.MuonItems = muon.monitoring_muonNonIso + muon.monitoring_muonIso + muon.monitoring_MSonly + muon.monitoring_muonEFFS
        self.BphysicsItems=bphys.monitoring_bphys
        self.ElectronItems = egamma.monitoringTP_electron + egamma.monitoring_electron 
        self.PhotonItems = egamma.monitoring_photon 
        self.MinBiasItems = minbias.monitoring_minbias
        self.JetItems = jets.primary_l1jet + jets.primary_jet
        self.JetItems.append('HLT_j0_perf_L1RD0_FILLED')
        self.BjetItems = bjet.monitoring_bjet
        self.TauItems = tau.monitoring_tau 
        #self.TauItems.append('HLT_tau25_perf_track')
        #self.TauItems.append('HLT_tau25_perf_ptonly')
        self.MetItems = met.monitoring_met


# Run with TrigDecisionTool configured from XML files
class TrigDecisionChecker_XMLConfig( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecChecker"):
        super( TrigDecisionChecker_XMLConfig, self ).__init__( name )


# TrigEDMChecker configurable
class TrigEDMChecker ( TrigEDMChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMChecker"):
        super( TrigEDMChecker, self ).__init__( name )


def getEDMAuxList():
    from TrigEDMConfig.TriggerEDM import getTriggerObjList,TriggerHLTList
    tlist=getTriggerObjList('AODFULL',[TriggerHLTList])
    objlist=[]
    for t,kset in six.iteritems(tlist):
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
    
    
