from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager("SCTMonManager")

SCTMonMan = topSequence.SCTMonManager

#SCTMonMan.FileKey = "GLOBAL"
#SCTMonMan.ManualDataTypeSetup = True
#SCTMonMan.DataType= "monteCarlo"
#SCTMonMan.Environment         = "user"
#SCTMonMan.ManualRunLBSetup    = True
#SCTMonMan.Run                 = 1
#SCTMonMan.LumiBlock           = 1

#Changed at the insistence of Andreas.Wildauer@cern.ch
#from DataQualityTools.DQMonFlags import DQMonFlags
from AthenaMonitoring.DQMonFlags import DQMonFlags

SCTMonMan.FileKey = DQMonFlags.monManFileKey()
SCTMonMan.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
SCTMonMan.ManualRunLBSetup = DQMonFlags.monManManualRunLBSetup()
SCTMonMan.DataType     = DQMonFlags.monManDataType()
SCTMonMan.Environment  = DQMonFlags.monManEnvironment()
SCTMonMan.Run          = DQMonFlags.monManRun()
SCTMonMan.LumiBlock    = DQMonFlags.monManLumiBlock()

#Currently this takes the latest/default tag of the db
#Check with Shaun as to which one we need for first data, and for M6.
#twiki somewhere ?
#include("SCT_Monitoring/SCT_Monitoring_ConditionsAccess.py")
########################################################################
# Conditions access
# These lines were previously in SCT_Monitoring_ConditionsAccess.py
########################################################################

from IOVDbSvc.CondDB import conddb

SCTConfigurationFolderPath="/SCT/DAQ/Configuration/"

if not conddb.folderRequested(SCTConfigurationFolderPath+"Chip"):
        conddb.addFolder("SCT",SCTConfigurationFolderPath+"Chip")
if not conddb.folderRequested(SCTConfigurationFolderPath+"Module"):
        conddb.addFolder("SCT",SCTConfigurationFolderPath+"Module")
if not conddb.folderRequested(SCTConfigurationFolderPath+"MUR"):
        conddb.addFolder("SCT",SCTConfigurationFolderPath+"MUR")


# Load condtions tool
from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup("SCT_MonConditionsSummaryTool")
sct_ConditionsSummaryToolSetup.setup()
SCT_MonConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()

# Load conditions configuration tool
from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setToolName("SCT_MonConfigurationConditionsTool")
sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"Chip")
sct_ConfigurationConditionsToolSetup.setModuleFolder(SCTConfigurationFolderPath+"Module")
sct_ConfigurationConditionsToolSetup.setMurFolder(SCTConfigurationFolderPath+"MUR")
sct_ConfigurationConditionsToolSetup.setup()
SCT_MonConfigurationConditionsTool = sct_ConfigurationConditionsToolSetup.getTool()

SCT_MonConditionsSummaryTool.ConditionsTools=[SCT_MonConfigurationConditionsTool.getFullName()]#,"InDetSCT_FlaggedConditionSvc", "InDetSCT_MonitorConditionsSvc"]
#SCT_MonConditionsSummaryTool.checkStripsInsideModule = False  If we have already vetoed against bad wafera or modules, this will improve performance when checking strips.


from SCT_Monitoring.SCT_MonitoringConf import SCTInitialiseTool
SCTInitTool = SCTInitialiseTool (
  name = "SCTInitialiseTool"
  )
#ToolSvc += SCTInitTool
SCTMonMan.AthenaMonTools += [SCTInitTool]

#from AthenaCommon.AppMgr import ToolSvc as toolSvc
from SCT_Monitoring.SCT_MonitoringConf import SCTHitsNoiseMonTool
SCTHitsTool = SCTHitsNoiseMonTool (
     name = "SCTHitsNoiseMonTool"
     )
SCTHitsTool.OutputLevel = 4;
SCTHitsTool.doHitmapHistos= False;
SCTHitsTool.CheckRate= 1000;
SCTHitsTool.doPositiveEndcap= True;
SCTHitsTool.doNegativeEndcap= True;

SCTHitsTool.tracksName=InDetKeys.SCTTracks()
#SCTHitsTool.tracksName="ExtendedTracks"


#ToolSvc += SCTHitsTool
SCTMonMan.AthenaMonTools += [SCTHitsTool]

from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonTool
SCTTracksMonTool = SCTTracksMonTool (
     name = "SCTTracksMonTool"
     )
SCTTracksMonTool.OutputLevel = 4;

SCTTracksMonTool.tracksName=InDetKeys.SCTTracks()
#SCTTracksMonTool.tracksName="ExtendedTracks"

SCTTracksMonTool.CheckRate= 1000;
SCTTracksMonTool.doPositiveEndcap= True;
SCTTracksMonTool.doNegativeEndcap= True;
SCTTracksMonTool.doUnbiasedCalc= True;
#ToolSvc += SCTTracksMonTool;
SCTMonMan.AthenaMonTools += [SCTTracksMonTool]

from SCT_Monitoring.SCT_MonitoringConf import SCTErrMonTool
SCTErrMonTool = SCTErrMonTool (
     name = "SCTErrMonTool"
     )
SCTErrMonTool.OutputLevel = 4;
SCTErrMonTool.histoPathBase="/stat";
SCTErrMonTool.CheckRate= 1000;
SCTErrMonTool.doPositiveEndcap= True;
SCTErrMonTool.doNegativeEndcap= True;
from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()
SCTErrMonTool.SCT_DCSConditionsTool=sct_DCSConditionsToolSetup.getTool()
#ToolSvc += SCTErrMonTool;
SCTMonMan.AthenaMonTools +=[SCTErrMonTool]

               
from SCT_Monitoring.SCT_MonitoringConf import SCTHitEffMonTool
SCTHitEffMonTool = SCTHitEffMonTool("SCTHitEffMonTool")

SCTHitEffMonTool.TrackName=InDetKeys.SCTTracks()

SCTHitEffMonTool.DetectorMode                   = 3
SCTHitEffMonTool.OutputLevel = 4
SCTHitEffMonTool.IsCosmic = False
SCTHitEffMonTool.UseMasks = False
SCTHitEffMonTool.LookAtDatabase = True
## Load flagged condition tool
from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
sct_FlaggedConditionToolSetup.setup()
InDetSCT_FlaggedConditionTool = sct_FlaggedConditionToolSetup.getTool()

SCTHitEffMonTool.FlaggedConditionTool = InDetSCT_FlaggedConditionTool
SCTHitEffMonTool.ChronoTime = False
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool

SCT_MonHoleSearch = InDet__InDetTrackHoleSearchTool(name ="SCT_MonHoleSearch",
                                                    Extrapolator = InDetExtrapolator,
                                                    ExtendedListOfHoles = True,
                                                    Cosmics =InDetFlags.doCosmics(),
                                                    SctSummaryTool=SCT_MonConditionsSummaryTool)
   
ToolSvc += SCT_MonHoleSearch
if (InDetFlags.doPrintConfigurables()):
   print      SCT_MonHoleSearch
   
SCTHitEffMonTool.HoleSearch=SCT_MonHoleSearch
if globalflags.DataSource != "data":
    SCTHitEffMonTool.IsSim                   = True
#ToolSvc+=SCTHitEffMonTool
SCTMonMan.AthenaMonTools += [SCTHitEffMonTool]

from SCT_Monitoring.SCT_MonitoringConf import SCTTimeDependentMonTool
SCTTimeDependentMonTool = SCTTimeDependentMonTool("SCTTimeDependentMonTool")
#ToolSvc+=SCTTimeDependentMonTool
#SCTMonMan.AthenaMonTools += [SCTTimeDependentMonTool]

## # Auditors 
## from GaudiSvc.GaudiSvcConf import AuditorSvc
## ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

## AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
## AthenaPoolCnvSvc.UseDetailChronoStat = TRUE


