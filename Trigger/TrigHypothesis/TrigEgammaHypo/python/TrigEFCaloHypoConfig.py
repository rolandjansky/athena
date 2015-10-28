# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################
## EF Calo Hypo Configuration
## Ryan Mackenzie White <ryan.white@cern.ch>
############################################
from AthenaCommon.Logging import logging #AT
import traceback 
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFCaloHypo
from AthenaCommon.SystemOfUnits import GeV

# Include EGammaPIDdefs for loose,medium,tight definitions
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefPhoton 
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
        self.SelectorToolName = "AsgElectronIsEMSelector/AsgElectronIsEMLoose1Selector"
        self.LHSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"

class TrigEFCaloHypo_EtCut (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFCaloHypo_EtCut, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = False
        self.SelectorToolName = "AsgElectronIsEMSelector/AsgElectronIsEMLoose1Selector"
        self.LHSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"

class TrigEFCaloHypo_e_ID (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFCaloHypo_e_ID, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = False
        self.ApplyIsEM = False
        self.ApplyLH = False
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronCaloToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronCaloHypoToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronIsEMBits
        
        if( 'lh' in IDinfo):
            self.SelectorToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseCaloSelector"
            self.LHSelectorToolName='AsgElectronLikelihoodTool/'+ElectronCaloToolName[IDinfo]
            self.ApplyLH = True
        else:
            self.ApplyIsEM = True
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            self.LHSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"
            if('1' in IDinfo):
                if( float(threshold) < 20 ):
                    self.SelectorToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]
                else:
                    self.SelectorToolName = 'AsgElectronIsEMSelector/'+ElectronCaloHypoToolName[IDinfo]
            else:
                self.SelectorToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]
        

class TrigEFCaloHypo_g_ID (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFCaloHypo_g_ID, self ).__init__( name )
        self.emEt = float(threshold)*GeV
        self.AcceptAll = True
        self.ApplyIsEM = True
        self.ApplyLH = False
        
        from AthenaCommon.AppMgr import ToolSvc           
        if IDinfo == 'loose1' or IDinfo == 'loose' :
            self.IsEMrequiredBits = SelectionDefPhoton.PhotonLooseEF #includ Rhad , Reta , Weta2 and Eratio
            self.SelectorToolName = "AsgElectronIsEMSelector/AsgPhotonIsEMLoose1Selector"
            self.LHSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseCaloSelector"
        else: 
            self.IsEMrequiredBits = SelectionDefPhoton.PhotonMediumEF #includ Rhad , Reta , Weta2 and Eratio
            self.SelectorToolName = "AsgElectronIsEMSelector/AsgPhotonIsEMMedium1Selector"
            self.LHSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHMediumCaloSelector"
