###############################################################
#
# Loads Detector Description for Pixel and SCT
# and prints out various quanities for each detector element.
#
###############################################################


# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()
DetFlags.readRDOPool.pixel_setOn()
DetFlags.readRDOPool.SCT_setOn()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion='ATLAS-R1-2012-03-01-00'

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-01-00')

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# Use MagneticFieldSvc
import MagFieldServices.SetupField

# Set up the job.
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms

from InDetDetDescrExample.InDetDetDescrExampleConf import ReadSiDetectorElements

ReadPixelElements = ReadSiDetectorElements("ReadPixelElements")
ReadPixelElements.ManagerName = "Pixel"
job += ReadPixelElements;

ReadSCTElements = ReadSiDetectorElements("ReadSCTElements")
ReadSCTElements.ManagerName = "SCT"
job += ReadSCTElements;


#
# Configure services
#
from AthenaCommon.AppMgr import ServiceMgr

#
# Both pixel and SCT
#
from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc

#
# Pixel
#
# Load DCS service
include( "PixelConditionsServices/PixelDCSSvc_jobOptions.py" )

## Silicon conditions service (set up by LorentzAngleSvcSetup)
pixelSiliconConditionsSvc = lorentzAngleSvc.PixelSiliconConditionsSvc
## Or directly from ServiceMgr (its the same instance)
#pixelSiliconConditionsSvc=ServiceMgr.PixelSiliconConditionsSvc
## Or if you want a different instance than used by LorentzAngleSvcSetup
#from PixelConditionsServices.PixelConditionsServicesConf import PixelSiliconConditionsSvc
#pixelSiliconConditionsSvc = PixelSiliconConditionsSvc("OtherPixel_SiliconConditionsSvc")
#ServiceMgr += pixelSiliconConditionsSvc

# Silicon properties service
pixelSiPropertiesSvc = SiPropertiesSvc(name = "PixelSiPropertiesSvc",
                                     DetectorName="Pixel",
                                     SiConditionsServices = pixelSiliconConditionsSvc)
ServiceMgr += pixelSiPropertiesSvc

ReadPixelElements.SiLorentzAngleSvc = lorentzAngleSvc.pixel
ReadPixelElements.SiPropertiesSvc   = pixelSiPropertiesSvc
ReadPixelElements.SiConditionsSvc   = pixelSiliconConditionsSvc

ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool'].LorentzAngleSvc=lorentzAngleSvc.pixel

#
# SCT
#
# Load DCS Tool
from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()
InDetSCT_DCSConditionsTool = sct_DCSConditionsToolSetup.getTool()

## Silicon conditions tool (set up by LorentzAngleSvcSetup)
sctSiliconConditionsTool = lorentzAngleSvc.SCT_SiliconConditionsTool

# Silicon properties tool
from SiPropertiesSvc.SCT_SiPropertiesToolSetup import SCT_SiPropertiesToolSetup
sct_SiPropertiesToolSetup = SCT_SiPropertiesToolSetup()
sct_SiPropertiesToolSetup.setSiliconTool(sctSiliconConditionsTool)
sct_SiPropertiesToolSetup.setup()
sctSiPropertiesTool = sct_SiPropertiesToolSetup.getTool()

ReadSCTElements.SiLorentzAngleSvc = lorentzAngleSvc.sct
ReadSCTElements.UseConditionsTools = True
ReadSCTElements.SiPropertiesTool = sctSiPropertiesTool
ReadSCTElements.SiConditionsTool = sctSiliconConditionsTool

ServiceMgr.GeoModelSvc.DetectorTools['SCT_DetectorTool'].LorentzAngleSvc=lorentzAngleSvc.sct

print ReadPixelElements
print lorentzAngleSvc.pixel
print pixelSiliconConditionsSvc
print pixelSiPropertiesSvc

print ReadSCTElements
print lorentzAngleSvc.sct
print sctSiliconConditionsTool
print sctSiPropertiesTool


#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.ReadPixelElements.OutputLevel = INFO
job.ReadSCTElements.OutputLevel = INFO

# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

# MC run number and timestamp for conditions data
# Simulation/RunDependentSim/RunDependentSimComps/python/RunDMCFlags.py
import AthenaCommon.AtlasUnixGeneratorJob
ServiceMgr.EventSelector.RunNumber = 195847 # MC12a run number
ServiceMgr.EventSelector.InitialTimeStamp  = 1328040250 # MC12a timestamp
ServiceMgr.EventSelector.TimeStampInterval = 0

ServiceMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"

# ReadSiDetectorElement properties
#
# ManagerName:      Manager name.
# LoopOverElements: Set false to avoid loop over all elements
#
###############################################################
