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
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TrigEgammaHypo.TrigEFPhotonHypoConfig")

class TrigEFPhotonIsoCutDefs ():
    def __init__(self):
        self.EtCone = {'icalovloose':[-1.,-1.,-1.],
                'icaloloose':[-1.,-1.,-1.],
                'icalomedium':[-1.,-1.,-1.],
                'icalotight':[-1.,-1.,-1.]
                }
        self.RelEtCone = {'icalovloose':[0.1,0.,0.],
                'icaloloose':[0.07,0.,0.],
                'icalomedium':[0.,0.,0],
                'icalotight':[0.,0.,0.03]
                }
        self.Offset = {'icalovloose':[0.,0.,0.],
                'icaloloose':[0.,0.,0.],
                'icalomedium':[0.,0.,0],
                'icalotight':[0.,0.,2.45*GeV]
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
        caloiso = [x for x in isoInfo if 'icalo' in x]
        # Following is hack to allow us to switch the trigger names
        # Please remove once moved to icalo
        if(len(caloiso)!=1):
            log.warning('WARNING, no calo isolation in chain dict, update trigger names')
            self.EtConeSizes  = 3
            self.EtConeCut    = [-1, -1, -1]
            self.RelEtConeCut = [-1,-1,-1] 
            self.IsoOffset    = [0,0,0] 
        else:
            #EtCone Size              =  20, 30, 40
            self.EtConeSizes  = 3
            self.EtConeCut    = [-1, -1, -1]
            self.RelEtConeCut = isoCuts.IsolationCuts(caloiso[0])
            self.IsoOffset    = isoCuts.OffsetValue(caloiso[0])
