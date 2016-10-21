# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################
## EF Photon Hypo Configuration
## Ryan Mackenzie White <ryan.white@cern.ch>
############################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFPhotonHypo
###############################################################
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonToolName
from TrigEgammaHypo.TrigEgammaPidTools import PhotonIsEMBits

from AthenaCommon.SystemOfUnits import GeV

class TrigEFPhotonIsoCutDefs ():
    def __init__(self):
        self.EtCone = {'ivloose':[-1.,-1.,-1.],
                'iloose':[-1.,-1.,-1.],
                'imedium':[-1.,-1.,-1.],
                'itight':[-1.,-1.,-1.]
                }
        self.RelEtCone = {'ivloose':[0.1,0.,0.],
                'iloose':[0.07,0.,0.],
                'imedium':[0.,0.,0],
                'itight':[0.,0.,0.03]
                }
        self.Offset = {'ivloose':[0.,0.,0.],
                'iloose':[0.,0.,0.],
                'imedium':[0.,0.,0],
                'itight':[0.,0.,2.45*GeV]
                }
    def IsolationCuts(self,isoinfo):
        return self.RelEtCone[isoinfo]
    
    def OffsetValue(self,isoinfo):
        return self.Offset[isoinfo]
            


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
        self.emEt = float(threshold)*GeV

class EFPhotonHypo_g_NoCut (TrigEFPhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( EFPhotonHypo_g_NoCut, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True        

#-----------------------------------------------------------------------
# --- gXX Particle ID selection
# --- loose, medium isEM with Calo-only
#-----------------------------------------------------------------------
class EFPhotonHypo_g_ID_CaloOnly (TrigEFPhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( EFPhotonHypo_g_ID_CaloOnly, self ).__init__( name ) 
        self.AcceptAll = False
        self.ApplyIsEM = True
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits =  PhotonIsEMBits[IDinfo]
        self.egammaPhotonCutIDToolName =  'AsgPhotonIsEMSelector/'+PhotonToolName[IDinfo] 

class EFPhotonHypo_g_ID_CaloOnly_Iso (EFPhotonHypo_g_ID_CaloOnly):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( EFPhotonHypo_g_ID_CaloOnly_Iso, self ).__init__( name, threshold, IDinfo )
        #Isolation
        isoCuts = TrigEFPhotonIsoCutDefs()
        self.ApplyIsolation = True
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 3
        self.EtConeCut          = [-1, -1, -1]
        self.RelEtConeCut = isoCuts.IsolationCuts(isoInfo)
        self.IsoOffset = isoCuts.OffsetValue(isoInfo)
