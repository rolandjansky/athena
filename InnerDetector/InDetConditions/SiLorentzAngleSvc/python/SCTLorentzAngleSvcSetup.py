# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Usage:
#
#  from SiLorentzAngleSvc.LorentzAngleSvcSetup import SCTLorentzAngleSvcSetup
#
# By default will determine whether to use DB or not from GeoModel.
# To force usage of DB (that is, use SiliconConditionsServices which should
# normally read from DB):
#
#  SCTLorentzAngleSvcSetup.forceUseDB()
#
# For more configuration one can get access to the service configurable via
# members pixel and sct: Eg:
#  lorentzAngleSvc.pixel.OutputLevel = VERBOSE
#  lorentzAngleSvc.sct.OutputLevel = VERBOSE
#  print lorentzAngleSvc.pixel
#  print lorentzAngleSvc.sct
#
# You can also access the pixel and sct SiliconConditionsSvc:
#
#  lorentzAngleSvc.PixelSiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
#  lorentzAngleSvc.SCT_SiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
#  print lorentzAngleSvc.PixelSiliconConditionsSvc
#  print lorentzAngleSvc.SCT_SiliconConditionsSvc
#
# 
# Alternatively, you can also access the services by:
#  from SiLorentzAngleSvc.LorentzAngleSvcSetup import PixelLorentzAngleSvc,SCTLorentzAngleSvc
# 
#  PixelLorentzAngleSvc.ForceUseDB = True
#  SCTLorentzAngleSvc.ForceUseDB = True
#  PixelLorentzAngleSvc.OutputLevel = VERBOSE
#  SCTLorentzAngleSvc.OutputLevel = VERBOSE
#

class SCTLorentzAngleSvcSetup:
    "Class to simplify setup of LorentzAngleSvc"
    def __init__(self):

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc
   
        from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc

        # Init SCTLorentzAngleSvc
        if hasattr(svcMgr,'SCTLorentzAngleSvc'):
            sctLorentzAngleSvc = svcMgr.SCTLorentzAngleSvc
        else :
            sctLorentzAngleSvc = SiLorentzAngleSvc(name="SCTLorentzAngleSvc",
                                                   DetectorName="SCT")
            svcMgr+=sctLorentzAngleSvc     
            
       
        # Init PixelSiliconConditionsSvc
        if hasattr(svcMgr,'SCT_SiliconConditionsSvc'):
            sctSiliconConditionsSvc = svcMgr.SCT_SiliconConditionsSvc
        else :
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
            sctSiliconConditionsSvc=SCT_SiliconConditionsSvc()
            svcMgr+=sctSiliconConditionsSvc

        # Pass the silicon conditions services to the Lorentz angle service
        # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
        # if loaded first.
        sctLorentzAngleSvc.UseMagFieldSvc = True
        sctLorentzAngleSvc.SiConditionsServices = sctSiliconConditionsSvc

        self.SCTLorentzAngleSvc = sctLorentzAngleSvc
        self.sctSiliconConditionsSvc = sctSiliconConditionsSvc
        self.SCT_SiliconConditionsSvc = sctSiliconConditionsSvc


    # Force the Lorentz angle sercive to use SiliconConditions service (which are assumed to use the DB)
    # Default is to decide based on GeoModel.
    def forceUseDB(self) :
        "Force usage of conditions DB"
        self.SCT_SiliconConditionsSvc.CheckGeoModel = False
        self.SCTLorentzAngleSvc.useSctDefaults = False

    # Force to use the defaults from GeoModel. In case it is not possible to use DCS
    def forceUseGeoModel(self) :
        "Force usage of GeoModel defaults"
        self.SCT_SiliconConditionsSvc.ForceUseGeoModel = True

# configuration instance
sctLorentzAngleSvcSetup = SCTLorentzAngleSvcSetup()
SCTLorentzAngleSvc = sctLorentzAngleSvcSetup.SCTLorentzAngleSvc
