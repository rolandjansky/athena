# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Usage:
#
#  from SCT_ConditionsServices import SCT_ConditionsServicesSetup
#
# For more configuration one can get access to the service configurables via
# members summarySvc, monitorSvc, etc. Eg:
#   SCT_ConditionsServicesSetup.monitorSvc.OutputLevel = VERBOSE
#   print SCT_ConditionsServicesSetup.monitorSvc.OutputLevel
# 
# Alternatively, you can also access the services directly by e.g:
#   from SCT_ConditionsServices.SCT_ConditionsServicesSetup import InDetSCT_ConditionsSummarySvc, InDetSCT_FlaggedConditionSvc
#   InDetSCT_ConditionsSummarySvc.OutputLevel = VERBOSE

class SCT_ConditionsServicesSetup:
    "Class to simplify setup of SCT_ConditionsSerivces"
    def __init__(self):

        from AthenaCommon.AppMgr import ServiceMgr
        self.svcMgr = ServiceMgr

        from IOVDbSvc.CondDB import conddb
        self.condDB = conddb

        self.isMC = False
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource() == 'geant4': isMC = True

        self.summarySvc  = self.initSummarySvc('InDetSCT_ConditionsSummarySvc')     
        self.flaggedSvc  = self.initFlaggedSvc('InDetSCT_FlaggedConditionSvc')      
        self.configSvc   = self.initConfigSvc('InDetSCT_ConfigurationConditionsSvc')
        self.monitorSvc  = self.initMonitorSvc('InDetSCT_MonitorConditionsSvc')
        self.bsErrSvc    = self.initBSErrSvc('InDetSCT_ByteStreamErrorsSvc')
        self.calibSvc    = self.initCalibSvc('InDetSCT_ReadCalibDataSvc')
        #if not self.isMC: self.dcsSvc      = self.initDcsSvc('InDetSCT_DCSConditionsSvc')     
        pass

    def initSummarySvc(self, instanceName):
        "Init summary conditions servic"
        
        if hasattr(self.svcMgr,instanceName):
            summarySvc = getattr(self.svcMgr, instanceName); 
        else:
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
            summarySvc = SCT_ConditionsSummarySvc(name = instanceName)
            self.svcMgr += summarySvc
        return summarySvc

    def initFlaggedSvc(self, instanceName):
        "Init flagged conditions service"
        
        if hasattr(self.svcMgr,instanceName):
            flaggedSvc = getattr(self.svcMgr, instanceName); 
        else :
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
            flaggedSvc = SCT_FlaggedConditionSvc(name = instanceName)
            self.svcMgr += flaggedSvc

        self.summarySvc.ConditionsServices+=[instanceName]
        return flaggedSvc

    def initConfigSvc(self, instanceName):
        "Init configuration conditions service"

        if hasattr(self.svcMgr,instanceName):
            configSvc = getattr(self.svcMgr, instanceName); 
        else:
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
            configSvc = SCT_ConfigurationConditionsSvc(name = instanceName)
            self.svcMgr += configSvc

        self.summarySvc.ConditionsServices+=[instanceName]

        if not self.condDB.folderRequested('/SCT/DAQ/Configuration/Chip'):
            self.condDB.addFolder("SCT","/SCT/DAQ/Configuration/Chip")
        if not self.condDB.folderRequested('/SCT/DAQ/Configuration/Module'):
            self.condDB.addFolder("SCT","/SCT/DAQ/Configuration/Module")
        if not self.condDB.folderRequested('/SCT/DAQ/Configuration/MUR'):
            self.condDB.addFolder("SCT","/SCT/DAQ/Configuration/MUR")

        return configSvc

    def initMonitorSvc(self, instanceName):
        "Init monitoring conditions service"

        if hasattr(self.svcMgr,instanceName):
            monitorSvc = getattr(self.svcMgr, instanceName); 
        else:        
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
            monitorSvc = SCT_MonitorConditionsSvc(name = instanceName, 
                                                       WriteCondObjs = False,
                                                       RegisterIOV   = False,
                                                       ReadWriteCool = True)
                                                       #OutputLevel = INFO)
            self.svcMgr += monitorSvc

        self.summarySvc.ConditionsServices+=[instanceName]

        if not self.condDB.folderRequested('/SCT/Derived/Monitoring'):
            self.condDB.addFolder("SCT","/SCT/Derived/Monitoring")
        
        return monitorSvc

    def initDcsSvc(self, instanceName):
        "Init DCS conditions service"

        if hasattr(self.svcMgr,instanceName):
            dcsSvc = getattr(self.svcMgr, instanceName); 
        else:        
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
            dcsSvc = SCT_DCSConditionsSvc(name = instanceName)
            self.svcMgr += dcsSvc

        self.summarySvc.ConditionsServices+=[instanceName]

        if not self.condDB.folderRequested('/SCT/DCS/HV'):
            self.condDB.addFolder("DCS_OFL","/SCT/DCS/HV")
        if not self.condDB.folderRequested('/SCT/DCS/MODTEMP'):
            self.condDB.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
        if not self.condDB.folderRequested('/SCT/DCS/MPS/LV'):
            self.condDB.addFolder("DCS_OFL","/SCT/DCS/MPS/LV")
        if not self.condDB.folderRequested('/SCT/DCS/CHANSTAT'):
            self.condDB.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
            
        return dcsSvc           

    def initBSErrSvc(self, instanceName):
        "Init ByteStream errors service"
        
        if hasattr(self.svcMgr,instanceName):
            bsErrSvc = getattr(self.svcMgr, instanceName); 
        else:
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
            bsErrSvc = SCT_ByteStreamErrorsSvc(name = instanceName)
            self.svcMgr += bsErrSvc

        self.summarySvc.ConditionsServices+=[instanceName]
        return  bsErrSvc

    def initCalibSvc(self, instanceName):
        "Init Calibration Data service"
        
        if hasattr(self.svcMgr,instanceName):
            calibSvc = getattr(self.svcMgr, instanceName); 
        else:
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
            calibSvc = SCT_ReadCalibDataSvc(name = instanceName)
            self.svcMgr += calibSvc

        self.summarySvc.ConditionsServices+=[instanceName]

        if not self.condDB.folderRequested('/SCT/DAQ/Calibration/ISNPtGainSummaryData'):
            self.condDB.addFolder("SCT","/SCT/DAQ/Calibration/ISNPtGainSummaryDat")
        if not self.condDB.folderRequested('/SCT/DAQ/Calibration/ISNoiseOccupancySummaryData'):
            self.condDB.addFolder("SCT","/SCT/DAQ/Calibration/ISNoiseOccupancySummaryData")

        return  calibSvc
    
    pass

# Create instance of setup service 
sctConditionsSvc = SCT_ConditionsServicesSetup()

InDetSCT_ConditionsSummarySvc        = sctConditionsSvc.summarySvc
InDetSCT_ConfigurationConditionsSvc  = sctConditionsSvc.configSvc
InDetSCT_FlaggedConditionSvc         = sctConditionsSvc.flaggedSvc
InDetSCT_MonitorConditionsSvc        = sctConditionsSvc.monitorSvc
InDetSCT_ByteStreamErrorsSvc         = sctConditionsSvc.bsErrSvc
InDetSCT_ReadCalibDataSvc            = sctConditionsSvc.calibSvc
#if not self.isMC: InDetSCT_DCSConditionsSvc            = sctConditionsSvc.dcsSvc

