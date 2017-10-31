# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################
## EF Calo Hypo Configuration
## Ryan Mackenzie White <ryan.white@cern.ch>
############################################
from AthenaCommon.Logging import logging #AT
import traceback 
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFCaloHypo
from AthenaCommon.SystemOfUnits import GeV

from TrigEgammaHypo.TrigEgammaPidTools import * 

from egammaTools.egammaToolsFactories import EMFourMomBuilder, EMShowerBuilder 
TrigEMShowerBuilderTool = EMShowerBuilder(
                name = "TrigEgammaShowerBuilder",
                CellsName = "",
                Print = True,
                )
TrigEMFourMomBuilder = EMFourMomBuilder()

class TrigEFCaloHypoBase (TrigEFCaloHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFCaloHypoBase, self ).__init__( name )
        from TrigEgammaHypo.TrigEFCaloHypoMonitoring import TrigEFCaloHypoValidationMonitoring, TrigEFCaloHypoOnlineMonitoring
        validation = TrigEFCaloHypoValidationMonitoring()
        online     = TrigEFCaloHypoOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]
        mlog = logging.getLogger( 'TrigEFCaloHypoBase:' )
        #Load Tool Service
        from AthenaCommon.AppMgr import ToolSvc
        from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
        ToolSvc += LuminosityToolOnline()
        self.ShowerBuilderTool=TrigEMShowerBuilderTool
        self.FourMomBuilderTool=TrigEMFourMomBuilder
        #-----------------------------------------------------------

class TrigEFCaloHypo_All (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFCaloHypo_All, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = True

class TrigEFCaloHypo_EtCut (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFCaloHypo_EtCut, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = False

class TrigEFCaloHypo_e_ID (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFCaloHypo_e_ID, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = False
        self.ApplyIsEM = False
        self.ApplyLH = False
        if( 'lh' in IDinfo):
            self.LHSelectorToolName='AsgElectronLikelihoodTool/'+ElectronCaloToolName[IDinfo]
            self.ApplyLH = True
        elif( 'bloose' in IDinfo or 'bloose1' in IDinfo or 'bloose2' in IDinfo or 'bloose3' in IDinfo  or 'blooseCalo' in IDinfo   ):
            self.ApplyIsEM = True
            from TrigEgammaHypo.TrigEgammaPidTools import BLooseISEMBits
            self.IsEMrequiredBits =  BLooseISEMBits[IDinfo]
            self.SelectorToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]
        else:
            self.ApplyIsEM = True
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            self.SelectorToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]
        

class TrigEFCaloHypo_g_ID (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFCaloHypo_g_ID, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = False
        self.ApplyPhotonIsEM = True
        self.IsEMrequiredBits =  PhotonIsEMBits[IDinfo]
        self.PhotonSelectorToolName =  'AsgPhotonIsEMSelector/'+PhotonToolName[IDinfo] 
