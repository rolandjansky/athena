# block include of file, this is used by many packages
include.block ("InDetRecExample/SCTConditionsAccess.py")

isData = (globalflags.DataSource == 'data')

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb
    
#
# --- Load SCT Conditions Services
#
SCTConfigurationFolderPath='/SCT/DAQ/Config/'
#if its COMP200, use old folders...
if (conddb.dbdata == "COMP200"):
  SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
#...but now check if we want to override that decision with explicit flag (if there is one)
try:
  if InDetFlags.ForceCoraCool():
    SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
except:
  pass
    
try:
  if InDetFlags.ForceCoolVectorPayload():
    SCTConfigurationFolderPath='/SCT/DAQ/Config/'
except:
  pass
        
try:
  if (InDetFlags.ForceCoolVectorPayload() and InDetFlags.ForceCoraCool()):
    print ('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')
    SCTConfigurationFolderPath=''
except:
  pass

# Load folders that have to exist for both MC and Data
SCTChipConfigurationPath=SCTConfigurationFolderPath+'Chip'
SCTModuleConfigurationPath=SCTConfigurationFolderPath+'Module'
SCTMurConfigurationPath=SCTConfigurationFolderPath+'MUR'
if not conddb.folderRequested(SCTChipConfigurationPath):
  conddb.addFolderSplitMC("SCT",SCTChipConfigurationPath,SCTChipConfigurationPath)
if not conddb.folderRequested(SCTModuleConfigurationPath):
  conddb.addFolderSplitMC("SCT",SCTModuleConfigurationPath,SCTModuleConfigurationPath)
if not conddb.folderRequested(SCTMurConfigurationPath):
  conddb.addFolderSplitMC("SCT",SCTMurConfigurationPath,SCTMurConfigurationPath)

if not conddb.folderRequested('/SCT/DAQ/Calibration/NPtGainDefects'):
  conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NPtGainDefects","/SCT/DAQ/Calibration/NPtGainDefects")
if not conddb.folderRequested('/SCT/DAQ/Calibration/NoiseOccupancyDefects'):
  conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NoiseOccupancyDefects","/SCT/DAQ/Calibration/NoiseOccupancyDefects")

if not athenaCommonFlags.isOnline():
  if not conddb.folderRequested('/SCT/Derived/Monitoring'):
    conddb.addFolder("SCT_OFL","/SCT/Derived/Monitoring")
    
    
# Load conditions summary service
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
ServiceMgr += InDetSCT_ConditionsSummarySvc
if (InDetFlags.doPrintConfigurables()):
  print (InDetSCT_ConditionsSummarySvc)
    
# Load conditions configuration service
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
InDetSCT_ConfigurationConditionsSvc = SCT_ConfigurationConditionsSvc(name = "InDetSCT_ConfigurationConditionsSvc")
ServiceMgr += InDetSCT_ConfigurationConditionsSvc
if (InDetFlags.doPrintConfigurables()):
  print (InDetSCT_ConfigurationConditionsSvc)

# Load calibration conditions service
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
InDetSCT_ReadCalibDataSvc = SCT_ReadCalibDataSvc(name = "InDetSCT_ReadCalibDataSvc")
ServiceMgr += InDetSCT_ReadCalibDataSvc
if (InDetFlags.doPrintConfigurables()):
  print (InDetSCT_ReadCalibDataSvc)
    
# Load flagged condition service
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
InDetSCT_FlaggedConditionSvc = SCT_FlaggedConditionSvc(name = "InDetSCT_FlaggedConditionSvc")
ServiceMgr += InDetSCT_FlaggedConditionSvc
if (InDetFlags.doPrintConfigurables()):
  print (InDetSCT_FlaggedConditionSvc)
    
# Load conditions Monitoring service
if not athenaCommonFlags.isOnline():
  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
  InDetSCT_MonitorConditionsSvc = SCT_MonitorConditionsSvc(name          = "InDetSCT_MonitorConditionsSvc",
                                                           OutputLevel   = INFO,
                                                           WriteCondObjs = False,
                                                           RegisterIOV   = False,
                                                           ReadWriteCool = True)
  ServiceMgr += InDetSCT_MonitorConditionsSvc
  if (InDetFlags.doPrintConfigurables()):
    print (InDetSCT_MonitorConditionsSvc)

if InDetFlags.doSCTModuleVeto():
  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoSvc
  InDetSCT_ModuleVetoSvc = SCT_ModuleVetoSvc(name = "InDetSCT_ModuleVetoSvc")
  ServiceMgr += InDetSCT_ModuleVetoSvc
  if (InDetFlags.doPrintConfigurables()):
    print (InDetSCT_ModuleVetoSvc)

# Load bytestream errors service (use default instance without "InDet")
# @TODO find a better to solution to get the correct service for the current job.
if not hasattr(ServiceMgr, "SCT_ByteStreamErrorsSvc"):
  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
  SCT_ByteStreamErrorsSvc = SCT_ByteStreamErrorsSvc(name = "SCT_ByteStreamErrorsSvc")
  ServiceMgr += SCT_ByteStreamErrorsSvc
  if (InDetFlags.doPrintConfigurables()):
    print (ServiceMgr.SCT_ByteStreamErrorsSvc)
    
if InDetFlags.useSctDCS():
  if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
  if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
  if not conddb.folderRequested('/SCT/DCS/HV'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/HV")
               
  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
  InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")        
  if InDetFlags.useHVForSctDCS(): InDetSCT_DCSConditionsSvc.UseDefaultHV = True  #Hack to use ~20V cut for SCT DCS rather than ChanStat for startup
  ServiceMgr += InDetSCT_DCSConditionsSvc
  if (InDetFlags.doPrintConfigurables()):
    print (InDetSCT_DCSConditionsSvc)
print ("Conditions db instance is ", conddb.dbdata)
# Load Tdaq enabled services for data only and add some to summary svc for data only
tdaqFolder = '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
if (conddb.dbdata == "CONDBR2"):
  tdaqFolder = '/TDAQ/Resources/ATLAS/SCT/Robins'
    
if (globalflags.DataSource() == 'data'):       
  # Load TdaqEnabled service
  if not conddb.folderRequested(tdaqFolder):
    conddb.addFolder("TDAQ",tdaqFolder)
    #conddb.addFolder("","<db>COOLONL_TDAQ/COMP200</db> /TDAQ/EnabledResources/ATLAS/SCT/Robins")

  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
  InDetSCT_TdaqEnabledSvc = SCT_TdaqEnabledSvc(name = "InDetSCT_TdaqEnabledSvc")

  ServiceMgr += InDetSCT_TdaqEnabledSvc
  if (InDetFlags.doPrintConfigurables()):
    print (InDetSCT_TdaqEnabledSvc)
        
  # Configure summary service
  InDetSCT_ConditionsSummarySvc.ConditionsServices= [ "InDetSCT_ConfigurationConditionsSvc",
                                                      "InDetSCT_FlaggedConditionSvc",
                                                      "SCT_ByteStreamErrorsSvc",
                                                      "InDetSCT_ReadCalibDataSvc",
                                                      "InDetSCT_TdaqEnabledSvc"]
  if not athenaCommonFlags.isOnline():
    InDetSCT_ConditionsSummarySvc.ConditionsServices += [ "InDetSCT_MonitorConditionsSvc" ]

  if InDetFlags.useSctDCS():
    InDetSCT_ConditionsSummarySvc.ConditionsServices += ["InDetSCT_DCSConditionsSvc"]       

# switch conditions off for SLHC usage
elif InDetFlags.doSLHC():
  InDetSCT_ConditionsSummarySvc.ConditionsServices= []
else :
  InDetSCT_ConditionsSummarySvc.ConditionsServices= [ "InDetSCT_ConfigurationConditionsSvc",
                                                      "InDetSCT_FlaggedConditionSvc",
                                                      "InDetSCT_MonitorConditionsSvc",
                                                      "SCT_ByteStreamErrorsSvc",
                                                      "InDetSCT_ReadCalibDataSvc"]

if InDetFlags.doSCTModuleVeto():
  InDetSCT_ConditionsSummarySvc.ConditionsServices += ["InDetSCT_ModuleVetoSvc"]
        
if (InDetFlags.doPrintConfigurables()):
  print (InDetSCT_ConditionsSummarySvc)
        
# Setup Lorentz angle service.
from SiLorentzAngleSvc.SCTLorentzAngleSvcSetup import sctLorentzAngleSvcSetup
    
if InDetFlags.useSctDCS():
  # Force Lorentz angle calculation to use DCS for data
  # (Not actually using DCS yet but rather temperature and voltage from joboptions.)
  if (globalflags.DataSource() == 'data'):            
    sctLorentzAngleSvcSetup.forceUseDB()
else:
  sctLorentzAngleSvcSetup.forceUseGeoModel()
