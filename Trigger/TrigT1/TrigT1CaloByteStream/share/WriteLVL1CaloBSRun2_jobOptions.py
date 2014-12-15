# TrigT1Calo object containers to bytestream conversion
include.block("TrigT1CaloByteStream/WriteLVL1CaloBSRun2_jobOptions.py")

# the following include is needed to load correctly the trigger towers on/off, TT/Cells maps
include( "CaloConditions/CaloConditions_jobOptions.py" )
# To setup correctly the LArCablingService when doLAr is off in the top option.
if not rec.doLArg():
    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
    include( "LArIdCnv/LArIdCnv_joboptions.py" )

from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmRoiByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepRoiByteStreamV2Tool
ToolSvc = Service("ToolSvc")
ToolSvc += LVL1BS__PpmByteStreamV2Tool("PpmByteStreamTool",
           PpmMappingTool="LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool")
ToolSvc += LVL1BS__CpByteStreamV2Tool("CpByteStreamV2Tool")
ToolSvc += LVL1BS__CpmRoiByteStreamV2Tool("CpmRoiByteStreamV2Tool")
ToolSvc += LVL1BS__JepByteStreamV2Tool("JepByteStreamV2Tool")
ToolSvc += LVL1BS__JepRoiByteStreamV2Tool("JepRoiByteStreamV2Tool")

StreamBS = AthenaOutputStream( "StreamBS" )
StreamBS.ItemList += [ "6207#*" ]        # TriggerTower
StreamBS.ItemList += [ "1270847937#*" ]  # CPBSCollectionV2
StreamBS.ItemList += [ "80981142#*" ]    # CPMTobRoI
StreamBS.ItemList += [ "1243139662#*" ]  # JEPBSCollectionV2
StreamBS.ItemList += [ "1316106213#*" ]  # JEPRoIBSCollectionV2
