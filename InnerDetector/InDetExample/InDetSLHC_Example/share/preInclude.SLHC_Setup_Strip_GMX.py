from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.doGMX.set_Value_and_Lock(True)

# Tools configuration of the GMX based Strip description

from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
SCT_GMX_DetectorTool.GmxFilename = "ITkStrip.gmx"
SCT_GMX_DetectorTool.OutputLevel = VERBOSE

from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("SLHC_SctSensorSD").GmxSensor=True
