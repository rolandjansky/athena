# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Usage:
#
#  from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import pixelLorentzAngleSvcSetup
#
# By default will determine whether to use DB or not from GeoModel.
# To force usage of DB (that is, use SiliconConditionsServices which should
# normally read from DB):
#
#  pixelLorentzAngleSvcSetup.forceUseDB()
#
# For more configuration one can get access to the service configurable via
#  pixelLorentzAngleSvcSetup.PixelLorentzAngleSvc.OutputLevel = VERBOSE
#  print pixelLorentzAngleSvcSetup.PixelLorentzAngleSvc
#
# You can also access the pixel SiliconConditionsSvc:
#
#  pixelLorentzAngleSvcSetup.PixelSiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
#  print pixelLorentzAngleSvcSetup.PixelSiliconConditionsSvc
# 
# Alternatively, you can also access the services by:
#  from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import PixelLorentzAngleSvc
# 
#  PixelLorentzAngleSvc.ForceUseDB = True
#  PixelLorentzAngleSvc.OutputLevel = VERBOSE
#

class PixelLorentzAngleSvcSetup:
    "Class to simplify setup of LorentzAngleSvc"
    def __init__(self):

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc
        
        from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc

        # Init PixelLorentzAngleSvc
        if hasattr(svcMgr,'PixelLorentzAngleSvc'):
            pixelLorentzAngleSvc = svcMgr.PixelLorentzAngleSvc
        else :
            pixelLorentzAngleSvc = SiLorentzAngleSvc(name="PixelLorentzAngleSvc",
                                                     DetectorName="Pixel")
            svcMgr+=pixelLorentzAngleSvc     

        # Init PixelSiliconConditionsSvc
        if hasattr(svcMgr,'PixelSiliconConditionsSvc'):
            pixelSiliconConditionsSvc = svcMgr.PixelSiliconConditionsSvc
        else :
            from PixelConditionsServices.PixelConditionsServicesConf import PixelSiliconConditionsSvc
            pixelSiliconConditionsSvc=PixelSiliconConditionsSvc()
            svcMgr+=pixelSiliconConditionsSvc

        pixelSiliconConditionsSvc.DepletionVoltage=10.0

        # Pass the silicon conditions services to the Lorentz angle service
        # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
        # if loaded first.
        pixelLorentzAngleSvc.UseMagFieldSvc = True
        pixelLorentzAngleSvc.SiConditionsServices = pixelSiliconConditionsSvc
        #pixelLorentzAngleSvc.CorrectionFactor = 0.900
        #Load Correction factor from database
        from IOVDbSvc.CondDB import conddb
        #conddb.addFolder("","/PIXEL/LorentzAngleScale<db>sqlite://;schema=PixelLorentzAngle_MC.db;dbname=OFLP200</db><tag>PIXELLorentzAngleScale-Simu-001</tag>")
        #conddb.addFolder("PIXEL_OFL","/PIXEL/LorentzAngleScale")

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.isOnline():
          conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/LorentzAngleScale", "/PIXEL/LorentzAngleScale")
        else:
          conddb.addFolder("PIXEL_OFL", "/PIXEL/LorentzAngleScale")
        pixelLorentzAngleSvc.CorrDBFolder="/PIXEL/LorentzAngleScale"
        
        self.PixelLorentzAngleSvc = pixelLorentzAngleSvc
        self.pixelSiliconConditionsSvc = pixelSiliconConditionsSvc
        self.PixelSiliconConditionsSvc = pixelSiliconConditionsSvc


    # Force the Lorentz angle sercive to use SiliconConditions service (which are assumed to use the DB)
    # Default is to decide based on GeoModel.
    def forceUseDB(self) :
        "Force usage of conditions DB"
        self.PixelSiliconConditionsSvc.CheckGeoModel = False
        self.PixelSiliconConditionsSvc.UseDBForHV = True
        self.PixelLorentzAngleSvc.usePixelDefaults = False
    

    # Force to use the defaults from GeoModel. In case it is not possible to use DCS
    def forceUseGeoModel(self) :
        "Force usage of GeoModel defaults"
        self.PixelSiliconConditionsSvc.ForceUseGeoModel = True

# configuration instance
pixelLorentzAngleSvcSetup = PixelLorentzAngleSvcSetup()
PixelLorentzAngleSvc = pixelLorentzAngleSvcSetup.PixelLorentzAngleSvc
