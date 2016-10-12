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
        self.ApplyIsEM = False
        self.usePhotonCuts = False
        self.emEt = float(threshold)*GeV

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
        self.emEt = float(threshold)*GeV
        
        from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
        from TrigEgammaHypo.TrigEgammaPidTools import PhotonToolName
        from TrigEgammaHypo.TrigEgammaPidTools import PhotonIsEMBits
        PhotonPidTools()
        self.IsEMrequiredBits =  PhotonIsEMBits[IDinfo]
        if IDinfo == 'loose1' or IDinfo == 'medium1':
            self.egammaElectronCutIDToolName = PhotonToolName[IDinfo] 
        if IDinfo == 'loose' or IDinfo == 'medium' or IDinfo == 'tight' or IDinfo == 'tight1':
            self.usePhotonCuts = True
            self.egammaPhotonCutIDToolName =  PhotonToolName[IDinfo] 

class EFPhotonHypo_g_ID_CaloOnly_Iso (EFPhotonHypo_g_ID_CaloOnly):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( EFPhotonHypo_g_ID_CaloOnly_Iso, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.ApplyIsolation = True
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 3
        self.RelEtConeCut       = [0.100, -1, -1]
        self.EtConeCut          = [-1, -1, -1]
