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

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

isData = (globalflags.DataSource == 'data')

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
from SiPropertiesSvc.PixelSiPropertiesToolSetup import PixelSiPropertiesToolSetup
pixelSiPropertiesToolSetup = PixelSiPropertiesToolSetup()
pixelSiPropertiesToolSetup.setup()
pixelSiPropertiesTool = pixelSiPropertiesToolSetup.getTool()

ReadPixelElements.UseConditionsTools = True
ReadPixelElements.SiLorentzAngleTool = lorentzAngleSvc.pixel
ReadPixelElements.SiPropertiesTool   = pixelSiPropertiesTool
ReadPixelElements.SiConditionsTool   = None

ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool'].LorentzAngleTool=lorentzAngleSvc.pixel
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

ReadSCTElements.UseConditionsTools = True
ReadSCTElements.SiLorentzAngleTool = lorentzAngleSvc.sct
ReadSCTElements.SiPropertiesTool = sctSiPropertiesTool
ReadSCTElements.SiConditionsTool = sctSiliconConditionsTool
ReadSCTElements.DetEleCollKey = "SCT_DetectorElementCollection"

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

# ReadSiDetectorElement properties
#
# ManagerName:      Manager name.
# LoopOverElements: Set false to avoid loop over all elements
#
###############################################################
