include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )

#from GaudiHandles import ServiceHandle
from GaudiKernel.GaudiHandles import ServiceHandle
 
# Import Digitization job properties 
from Digitization.DigitizationFlags import jobproperties 

# Get ToolSvc and ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr

# make sure ToolSvc is there...
if not "ToolSvc"         in theApp.ExtSvc and \
   not "ToolSvc/ToolSvc" in theApp.ExtSvc:
    theApp.ExtSvc += [ "ToolSvc/ToolSvc"]
    pass

# Import Beam job properties
from AthenaCommon.BeamFlags import jobproperties

# Setup the DCS folders and Svc used in the sctSiliconConditionsSvc
if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
if not conddb.folderRequested('/SCT/DCS/HV'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/HV")


from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")
ServiceMgr += InDetSCT_DCSConditionsSvc

# Lorentz Angle Service
from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc

# Silicon conditions service (set up by LorentzAngleSvcSetup)
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
ServiceMgr += SCT_SiliconConditionsSvc()
sctSiliconConditionsSvc=ServiceMgr.SCT_SiliconConditionsSvc

# Silicon properties service
from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc;
sctSiPropertiesSvc = SiPropertiesSvc(name = "SCT_SiPropertiesSvc",
                                     DetectorName="SCT",
                                     SiConditionsServices = sctSiliconConditionsSvc)
ServiceMgr += sctSiPropertiesSvc


from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()

from IOVDbSvc.CondDB import conddb
conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")

# setup PixelCalibDbTool in ToolSvc
#PixelCalibDbTool = Service("ToolSvc.PixelCalibDbTool")
ToolSvc.PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
ToolSvc.PixelCalibDbTool.CalibLocation ="PixCalibKey"
ToolSvc.PixelCalibDbTool.WriteDB =False



from FastSiDigitization.FastSiDigitizationConf import SCT_FastDigitization

# now add config class to algsequence()
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

job += SCT_FastDigitization("SCT_FastDigitization")
sct = job.SCT_FastDigitization

#if not jobproperties.Digitization.doXingByXingPileUp():
    # this will create one digitization with all default properties
#    from SCT_Digitization.SCT_DigitizationConf import SCT_FastDigitization
#    job += SCT_FastDigitization("SCT_FastDigitization")
#    sct = job.SCT_FastDigitization
#else:
    # this will create one digitization tool with all default properties
#from SCT_Digitization.SCT_DigitizationConf import SCT_FastDigitizationTool
#job.PileUpToolsAlg.PileUpTools += [ SCT_FastDigitizationTool("SCT_FastDigitizationTool") ]
#sct = job.PileUpToolsAlg.PileUpTools[ "SCT_FastDigitizationTool" ]
#xing times in ns
#sct.FirstXing = -50
#sct.LastXing = 25

#self.__inDetClusterMaker__    = None

#sct.InputObjectName = "SCT_Hits"
#sct.OutputObjectName = "SCT_Digits"

# set the random service, stream name, seeds
rndmStream   = "SCT_Digitization"
#sct.RndmSvc = jobproperties.Digitization.rndmSvc()
jobproperties.Digitization.rndmSeedList.addSeed(rndmStream, 49261510, 105132394 )



