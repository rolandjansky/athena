# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################
## EF Photon Hypo Configuration
## Ryan Mackenzie White <ryan.white@cern.ch>
############################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFPhotonHypo


###############################################################
# Include EGammaPIDdefs for loose,medium,tight definitions
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefPhoton 

import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaParameters

PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import egammaPID

# Include electronPIDmenu from Trigger specific maps 
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu

from AthenaCommon.SystemOfUnits import GeV

class TrigEFPhotonHypoBase (TrigEFPhotonHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFPhotonHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigEFPhotonHypoMonitoring import TrigEFPhotonHypoValidationMonitoring, TrigEFPhotonHypoOnlineMonitoring, TrigEFPhotonHypoCosmicMonitoring
        validation = TrigEFPhotonHypoValidationMonitoring()
        online     = TrigEFPhotonHypoOnlineMonitoring()
        cosmic     = TrigEFPhotonHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]


        #-----------------------------------------------------------
        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'TrigEFPhotonHypoBase:' )

#-----------------------------------------------------------------------
# --- RMWhite 2014
# --- Following classes to be used with TriggerMenu
# --- Rely on TM to configure 
# --- Remove old TMP classes April 22, 2014
#
#-----------------------------------------------------------------------
# --- Et Cut only -- threshold passed from TM chain name
#-----------------------------------------------------------------------
# --- Et cut only for Photons, configured as nocut
# --- Et cut applied at L2 
class EFPhotonHypo_g_EtCut (TrigEFPhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( EFPhotonHypo_g_EtCut, self ).__init__( name ) 
        self.AcceptAll = True
        self.usePhotonCuts = False

class EFPhotonHypo_g_NoCut (TrigEFPhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( EFPhotonHypo_g_NoCut, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True        
        self.usePhotonCuts = False

#-----------------------------------------------------------------------
# --- gXX Particle ID selection
# --- loose, medium isEM with Calo-only
#-----------------------------------------------------------------------
class EFPhotonHypo_g_ID_CaloOnly (TrigEFPhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( EFPhotonHypo_g_ID_CaloOnly, self ).__init__( name ) 
        from AthenaCommon.AppMgr import ToolSvc
        self.AcceptAll = False
        self.ApplyIsEM = True
        self.usePhotonCuts = False
        print IDinfo
        self.emEt = float(threshold)*GeV
        
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AsgElectronSelectorTool")
            print traceback.format_exc()
            return False
        
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AsgPhotonSelectorTool")
            print traceback.format_exc()
            return False
        
        if IDinfo == 'loose' or IDinfo == 'loose1':
            self.IsEMrequiredBits = SelectionDefPhoton.PhotonLooseEF #includ Rhad , Reta , Weta2 and Eratio
            if hasattr(ToolSvc, "AsgPhotonIsEMLooseSelector"):
                self.egammaElectronCutIDToolName = "AsgPhotonIsEMLooseSelector"
            else:    
                thephotoncutlooseid=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMLooseSelector",egammaPID.PhotonIDLooseEF,electronPIDmenu.menuTrig2012)
                self.egammaElectronCutIDToolName = thephotoncutlooseid.getFullName()
                ToolSvc+=thephotoncutlooseid       
        elif IDinfo == 'medium' or IDinfo == 'medium1':
            self.IsEMrequiredBits = SelectionDefPhoton.PhotonMediumEF #includ Rhad , Reta , Weta2 and Eratio
            if hasattr(ToolSvc, "AsgPhotonIsEMMediumSelector"):
                self.egammaElectronCutIDToolName = "AsgPhotonIsEMMediumSelector"
            else:    
                thephotoncutmediumid=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMMediumSelector",egammaPID.PhotonIDMediumEF,electronPIDmenu.menuTrig2012)
                self.egammaElectronCutIDToolName = thephotoncutmediumid.getFullName()
                ToolSvc+=thephotoncutmediumid       
        #--- egammaPhotonCutIDTool without ForcePhotonConversion (same selection as in offline)
        #--- Migrated to ElectronPhotonSelectorTool, menu from offline
        # at High Lumi use Photon selection
        elif IDinfo == 'tight' or IDinfo == 'tight1': 
            self.usePhotonCuts = True
            self.IsEMrequiredBits = SelectionDefPhoton.PhotonTight #includ Rhad , Reta , Weta2 and Eratio
            if hasattr(ToolSvc, "AsgPhotonIsEMTightSelector"):
                self.egammaPhotonCutIDToolName = thephotoncuttightid.getFullName()
            else:    
                thephotoncuttightid=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMTightSelector",egammaPID.PhotonIDTight)
                thephotoncuttightid.ForceConvertedPhotonPID = False
                self.egammaPhotonCutIDToolName = thephotoncuttightid.getFullName()
                ToolSvc+=thephotoncuttightid
            
        else:
            raise RuntimeError('INCORRECT IDinfo: No SelectorTool configured')

        
