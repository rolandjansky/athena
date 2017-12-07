#################################################
# EF Electron Hypothesis Algorithm Configuration:
# Ryan Mackenzie White <ryan.white@cern.ch>
#################################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFElectronHypo
from AthenaCommon.SystemOfUnits import GeV

# Include EGammaPIDdefs for loose,medium,tight definitions
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
from TrigEgammaHypo.TrigEgammaPidTools import * 
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TrigEgammaHypo.TrigEFElectronHypoConfig")

isolation_dict = {'ivarloose':[-1, -1, -1,0.100,-1,-1],
                  'ivarmedium':[-1, -1, -1,0.065,-1,-1],
                  'ivartight':[-1, -1, -1,0.05,-1,-1],
                  'iloose':[0.100, -1, -1,-1,-1,-1]}

caloisolation_dict = {'icaloloose':[-1, -1, -1,0.2,-1,-1],
                  'icalomedium':[-1, -1, -1,0.15,-1,-1],
                  'icalotight':[-1, -1, -1,0.1,-1,-1]}

log.debug("Track isolation %s",isolation_dict)
log.debug("Calo isolation %s",caloisolation_dict)

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
        
        #Load Tool Service
        from AthenaCommon.AppMgr import ToolSvc
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
        self.IsEMrequiredBits = 0X0
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]


class TrigEFElectronHypo_e_ID_HI (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_HI, self ).__init__( name ) 
        # Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]

 
# Likelihood only chains for alignment studies 
class TrigEFElectronHypo_e_LH (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, lhInfo):
        super( TrigEFElectronHypo_e_LH, self ).__init__( name ) 
        # Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        
        self.UseAthenaElectronLHIDSelectorTool = True
        if(IDinfo == 'lhvloose'):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHVLooseToolName[lhInfo]
        elif(IDinfo == 'lhloose'):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHLooseToolName[lhInfo]
        elif(IDinfo == 'lhmedium'):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHMediumToolName[lhInfo]
        elif(IDinfo == 'lhtight'):
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHTightToolName[lhInfo]

# --- eXX LH chains for alignment with isolation 
class TrigEFElectronHypo_e_LH_Iso (TrigEFElectronHypo_e_LH):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, lhInfo, isoInfo):
        super( TrigEFElectronHypo_e_LH_Iso, self ).__init__( name, threshold, IDinfo, lhInfo ) 
# Set the properties        
        self.CaloCutsOnly = False
        self.ApplyEtIsEM = True
        #Isolation
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        caloiso = [x for x in isoInfo if 'icalo' in x]
        trkiso = [x for x in isoInfo if 'icalo' not in x]

        self.PtConeSizes = 6
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 6
        self.PtConeCut          = [-1, -1, -1,-1,-1,-1]   
        self.EtConeCut          = [-1, -1, -1,-1, -1, -1]
        
        if(len(trkiso)>0):
            self.RelPtConeCut       = isolation_dict[trkiso[0]] 
        else:
            self.RelPtConeCut       = [-1, -1, -1,-1,-1,-1]

        if len(caloiso) > 0:
            self.RelEtConeCut       = caloisolation_dict[caloiso[0]]
        else:
            self.RelEtConeCut = [-1, -1, -1,-1, -1, -1]

# --- W T&P supporting trigger
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_WTP (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_WTP, self ).__init__( name ) 
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        self.IsEMrequiredBits =  SelectionDefElectron.Electron_trk
        self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName["loose"]

#-----------------------------------------------------------------------
# --- eXX Particle ID selection CaloCuts only
# --- loose, medium, tight isEM
# --- Currently Not for likelihood, but derives from e_ID class 
#-----------------------------------------------------------------------
# Dictionary for mapping PID to toolname
class TrigEFElectronHypo_e_ID_CaloOnly (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_CaloOnly, self ).__init__( name, threshold, IDinfo ) 
        
        # Set the properties        
        self.CaloCutsOnly = True
        self.IsEMrequiredBits = 0X0
        
        if( 'lh' in IDinfo):
            self.UseAthenaElectronLHIDSelectorTool = True
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronCaloToolName[IDinfo]
        else:
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]

# --- eXX IsEM Selection uses the Et of the object
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
        caloiso = [x for x in isoInfo if 'icalo' in x]
        trkiso = [x for x in isoInfo if 'icalo' not in x]

        self.PtConeSizes = 6
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 6
        self.PtConeCut          = [-1, -1, -1,-1,-1,-1]   
        self.EtConeCut          = [-1, -1, -1,-1, -1, -1]
        
        if(len(trkiso)>0):
            self.RelPtConeCut       = isolation_dict[trkiso[0]] 
        else:
            self.RelPtConeCut       = [-1, -1, -1,-1,-1,-1]

        if len(caloiso) > 0:
            self.RelEtConeCut       = caloisolation_dict[caloiso[0]]
        else:
            self.RelEtConeCut = [-1, -1, -1,-1, -1, -1]


#-----------------------------------------------------------------------
# --- eXX Particle ID and Isolation performance chains
# --- derives from e_ID
# --- iloose, imedium
# --- run selection but accept all 
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_ID_perf (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_perf, self ).__init__( name, threshold, IDinfo )
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

# Add back some dummy classes
# Just to ensure menu compiles
class TrigEFElectronHypo_e_ID_EtIsEM (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFElectronHypo_e_ID_EtIsEM, self ).__init__( name)
        self.AcceptAll = True

class TrigEFElectronHypo_e_ID_EtIsEM_Iso (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFElectronHypo_e_ID_EtIsEM_Iso, self ).__init__( name)
        self.AcceptAll = True

