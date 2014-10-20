# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################
# EF Electron Hypothesis Algorithm Configuration:
# Ryan Mackenzie White <ryan.white@cern.ch>
#################################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFElectronHypo
from AthenaCommon.SystemOfUnits import GeV
#Load Tool Service
from AthenaCommon.AppMgr import ToolSvc

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
NoElossMaterialEffectsUpdator.EnergyLoss = False
ToolSvc += NoElossMaterialEffectsUpdator

#setup MaterialEffectsUpdator arrays
MyUpdators = []
MyUpdators += [AtlasMaterialEffectsUpdator] # for ID
MyUpdators += [NoElossMaterialEffectsUpdator] # for Calo
#MyUpdators += [NoElossMaterialEffectsUpdator] # for muon

MySubUpdators = []
MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon

###############################################################
# Include EGammaPIDdefs for loose,medium,tight definitions
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron

import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum

PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaParameters 
from ROOT import egammaPID

# Include electron menus for LH and Cut-based
from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import electronLHmenu
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu

class TrigEFElectronHypoBase (TrigEFElectronHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFElectronHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigEFElectronHypoMonitoring import TrigEFElectronHypoValidationMonitoring, TrigEFElectronHypoOnlineMonitoring, TrigEFElectronHypoCosmicMonitoring
        validation = TrigEFElectronHypoValidationMonitoring()
        online     = TrigEFElectronHypoOnlineMonitoring()
        cosmic     = TrigEFElectronHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        #AT Jan 2010: self.Extrapolator = theAtlasExtrapolator

        self.histoPath = "/EXPERT"        

#-----------------------------------------------------------------------
# --- Following classes to be used with TriggerMenu
# --- Rely on TM to configure 
# --- RMWhite 2014
#
# No Cut for EF
class TrigEFElectronHypo_e_NoCut (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_NoCut, self ).__init__( name ) 
        self.AcceptAll = True
        self.CaloCutsOnly = False
        self.ApplyIsEM = False
        self.ApplyEtIsEM = False
        self.IsEMrequiredBits = 0X0
        self.emEt = float(threshold)*GeV
#
#-----------------------------------------------------------------------
# --- Et Cut only -- threshold passed from TM chain name
# --- No cut applied at L2
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_EtCut (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_EtCut, self ).__init__( name ) 
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = False
        self.ApplyEtIsEM = False
        self.IsEMrequiredBits = 0X0
        self.emEt = float(threshold)*GeV

#-----------------------------------------------------------------------
# --- eXX Particle ID selection
# --- loose, medium, tight isEM
# --- mvtloose, mvtmedium, mvttight LH
# --- W T&P supporting trigger
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_ID (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID, self ).__init__( name ) 
# Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AthenaSelectorTool")
            print traceback.format_exc()
            return False
        
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AthenaSelectorTool")
            print traceback.format_exc()
            return False
        from AthenaCommon.AppMgr import ToolSvc
        # Add the PID tools
        print IDinfo
        if IDinfo == 'loose1':
            self.IsEMrequiredBits = SelectionDefElectron.ElectronLoose1
            if hasattr(ToolSvc, "AsgElectronIsEMLoose1Selector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMLoose1Selector"
            else:    
                theelectroncutloose1id=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMLoose1Selector",egammaPID.ElectronIDLoose1,electronPIDmenu.menuTrig2012)
                self.egammaElectronCutIDToolName = theelectroncutloose1id.getFullName()
                ToolSvc+=theelectroncutloose1id       
        elif IDinfo == 'loose': 
            self.IsEMrequiredBits = SelectionDefElectron.ElectronLooseHLT
            if hasattr(ToolSvc, "AsgElectronIsEMLooseSelector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMLooseSelector"
            else:
                theelectroncutlooseid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMLooseSelector",egammaPID.ElectronIDLooseHLT,electronPIDmenu.menuTrigDC14)
                self.egammaElectronCutIDToolName = theelectroncutlooseid.getFullName()
                ToolSvc+=theelectroncutlooseid       
        elif IDinfo == 'medium1':
            self.IsEMrequiredBits = SelectionDefElectron.ElectronMedium1
            if hasattr(ToolSvc, "AsgElectronIsEMMedium1Selector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMMedium1Selector"
            else:    
                theelectroncutmedium1id=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMMedium1Selector",egammaPID.ElectronIDMedium1,electronPIDmenu.menuTrig2012)
                self.egammaElectronCutIDToolName = theelectroncutmedium1id.getFullName()
                ToolSvc+=theelectroncutmedium1id       
        elif IDinfo == 'medium':
            self.IsEMrequiredBits = SelectionDefElectron.ElectronMediumHLT
            if hasattr(ToolSvc, "AsgElectronIsEMMediumSelector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMMediumSelector"
            else:
                theelectroncutmediumid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMMediumSelector",egammaPID.ElectronIDMediumHLT,electronPIDmenu.menuTrigDC14)
                self.egammaElectronCutIDToolName = theelectroncutmediumid.getFullName()
                ToolSvc+=theelectroncutmediumid       
        elif IDinfo == 'tight1':
            self.IsEMrequiredBits = SelectionDefElectron.ElectronTight1
            if hasattr(ToolSvc, "AsgElectronIsEMTight1Selector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMTight1Selector"
            else:    
                theelectroncuttight1id=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMTight1Selector",egammaPID.ElectronIDTight1,electronPIDmenu.menuTrig2012)
                self.egammaElectronCutIDToolName = theelectroncuttight1id.getFullName()
                ToolSvc+=theelectroncuttight1id       
        elif IDinfo == 'tight':
            self.IsEMrequiredBits = SelectionDefElectron.ElectronTightHLT
            if hasattr(ToolSvc, "AsgElectronIsEMTightSelector"):
                self.egammaElectronCutIDToolName = "AsgElectronIsEMTightSelector"
            else:
                theelectroncuttightid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMTightSelector",egammaPID.ElectronIDTightHLT,electronPIDmenu.menuTrigDC14)
                self.egammaElectronCutIDToolName = theelectroncuttightid.getFullName()
                ToolSvc+=theelectroncuttightid       
        elif IDinfo == 'lhloose':
            self.UseAthenaElectronLHIDSelectorTool = True
            if hasattr(ToolSvc,"AsgElectronLHLooseSelector"):
                self.AthenaElectronLHIDSelectorToolName="AsgElectronLHLooseSelector"
            else:
                theelectronlhlooseid=ConfiguredAsgElectronLikelihoodTool("AsgElectronLHLooseSelector",
                       LikeEnum.Loose,
                       menu=electronLHmenu.trigger2015,
                       usePVContainer = False)
                self.AthenaElectronLHIDSelectorToolName=theelectronlhlooseid.getFullName()
                ToolSvc+=theelectronlhlooseid       
        elif IDinfo == 'lhmedium':
            self.UseAthenaElectronLHIDSelectorTool = True
            if hasattr(ToolSvc,"AsgElectronLHMediumSelector"):
                self.AthenaElectronLHIDSelectorToolName="AsgElectronLHMediumSelector"
            else:
                theelectronlhmediumid=ConfiguredAsgElectronLikelihoodTool("AsgElectronLHMediumSelector",
                       LikeEnum.Medium,
                       menu=electronLHmenu.trigger2015,
                       usePVContainer = False)
                self.AthenaElectronLHIDSelectorToolName=theelectronlhmediumid.getFullName()
                ToolSvc+=theelectronlhmediumid       
        elif IDinfo == 'lhtight':
            self.UseAthenaElectronLHIDSelectorTool = True
            if hasattr(ToolSvc,"AsgElectronLHTightSelector"):
                self.AthenaElectronLHIDSelectorToolName="AsgElectronLHTightSelector"
            else:
                theelectronlhtightid=ConfiguredAsgElectronLikelihoodTool("AsgElectronLHTightSelector",
                       LikeEnum.Tight,
                       menu=electronLHmenu.trigger2015,
                       usePVContainer = False)
                self.AthenaElectronLHIDSelectorToolName=theelectronlhtightid.getFullName()
                ToolSvc+=theelectronlhtightid       
        else:
            raise RuntimeError('INCORRECT IDinfo: No SelectorTool configured')
        
        
        
# --- W T&P supporting trigger
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_WTP (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_WTP, self ).__init__( name ) 
# Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
# Import the SelectorTools        
        try:
            from TrigEgammaRec.TrigEgammaAthElectronIsEMSelectors import TrigEgammaAthElectronIsEMSelector
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AthenaSelectorTool")
            print traceback.format_exc()
            return False
# Need to check selection to map for W T&P trigger        
        self.IsEMrequiredBits = SelectionDefElectron.Electron_trk
        theelectroncutid=TrigEgammaAthElectronIsEMSelector("athElectronIsEMSelector_"+self.name(),TrigEgammaIDQuality.ElectronIDLoose,electronPIDmenu.menuTrig2012)
        self.egammaElectronCutIDToolName = theelectroncutid.getFullName()

#-----------------------------------------------------------------------
# --- eXX Particle ID selection CaloCuts only
# --- loose, medium, tight isEM
# --- Currently Not for likelihood, but derives from e_ID class 
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_ID_CaloOnly (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_CaloOnly, self ).__init__( name, threshold, IDinfo ) 
# Set the properties        
        self.CaloCutsOnly = True

#-----------------------------------------------------------------------
# --- eXX IsEM Selection uses the Et of the object
class TrigEFElectronHypo_e_ID_EtIsEM (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_EtIsEM, self ).__init__( name, threshold, IDinfo ) 
# Set the properties        
        self.CaloCutsOnly = True
        self.ApplyEtIsEM = True

#-----------------------------------------------------------------------
# --- eXX Particle ID and Isolation
# --- derives from e_ID
# --- iloose, imedium
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_Iso (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( TrigEFElectronHypo_e_Iso, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  15, 20, 25, 30, 35, 40
        self.EtConeSizes = 6
        self.RelEtConeCut       = [-1, -1, -1, -1, -1, -1]
        self.EtConeCut          = [-1, -1, -1, -1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 3
        self.RelPtConeCut       = [0.100, -1, -1]
        self.PtConeCut          = [-1, -1, -1]

#-----------------------------------------------------------------------
# --- eXX Particle ID and Isolation performance chains
# --- derives from e_ID
# --- iloose, imedium
# --- run selection but accept all 
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_ID_perf (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_IsD_perf, self ).__init__( name, threshold, IDinfo )
        self.AcceptAll = True

class TrigEFElectronHypo_e_Iso_perf (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( TrigEFElectronHypo_e_Iso_perf, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.AcceptAll = True
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  15, 20, 25, 30, 35, 40
        self.EtConeSizes = 6
        self.RelEtConeCut       = [-1, -1, -1, -1, -1, -1]
        self.EtConeCut          = [-1, -1, -1, -1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 3
        self.RelPtConeCut       = [0.100, -1, -1]
        self.PtConeCut          = [-1, -1, -1]

