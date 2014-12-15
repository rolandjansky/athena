# TrigT1Calo object containers to bytestream conversion
include.block("TrigT1CaloByteStream/WriteLVL1CaloBSRun1_jobOptions.py")

# the following include is needed to load correctly the trigger towers on/off, TT/Cells maps
include( "CaloConditions/CaloConditions_jobOptions.py" )
# To setup correctly the LArCablingService when doLAr is off in the top option.
if not rec.doLArg():
    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
    include( "LArIdCnv/LArIdCnv_joboptions.py" )

from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmRoiByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepRoiByteStreamV1Tool
ToolSvc = Service("ToolSvc")
ToolSvc += LVL1BS__PpmByteStreamV1Tool("PpmByteStreamToolV1",
           PpmMappingTool="LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool")
ToolSvc += LVL1BS__PpmByteStreamV2Tool("PpmByteStreamToolV2",
           PpmMappingTool="LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool")
ToolSvc += LVL1BS__CpByteStreamV1Tool("CpByteStreamV1Tool")
ToolSvc += LVL1BS__CpmRoiByteStreamV1Tool("CpmRoiByteStreamV1Tool")
ToolSvc += LVL1BS__JepByteStreamV1Tool("JepByteStreamV1Tool")
ToolSvc += LVL1BS__JepRoiByteStreamV1Tool("JepRoiByteStreamV1Tool")

StreamBS = AthenaOutputStream( "StreamBS" )
StreamBS.ItemList += [ "1174216565#*" ] # AOD::TriggerTowerContainer_v2
StreamBS.ItemList += [ "216508938#*" ]  # CPMRoI
StreamBS.ItemList += [ "1270847938#*" ] # CPBSCollectionV1
StreamBS.ItemList += [ "1243139661#*" ] # JEPBSCollectionV1
StreamBS.ItemList += [ "1316106214#*" ] # JEPRoIBSCollectionV1
