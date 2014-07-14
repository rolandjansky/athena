# Bytestream to TrigT1Calo objects conversions for data with Run 2 formats
include.block("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

# the following include is needed to load correctly the trigger towers on/off, TT/Cells maps
include( "CaloConditions/CaloConditions_jobOptions.py" )
# To setup correctly the LArCablingService when doLAr is off in the top option.
from RecExConfig.RecFlags import rec
if not rec.doLArg():
    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
    include( "LArIdCnv/LArIdCnv_joboptions.py" )

from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmRoiByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepRoiByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamTool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__RodHeaderByteStreamTool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__L1CaloErrorByteStreamTool
ToolSvc = Service("ToolSvc")
ToolSvc += LVL1BS__CpByteStreamV2Tool("CpByteStreamV2Tool")
ToolSvc += LVL1BS__CpmRoiByteStreamV2Tool("CpmRoiByteStreamV2Tool")
ToolSvc += LVL1BS__JepByteStreamV2Tool("JepByteStreamV2Tool")
ToolSvc += LVL1BS__JepRoiByteStreamV2Tool("JepRoiByteStreamV2Tool")
ToolSvc += LVL1BS__PpmByteStreamTool("PpmByteStreamTool",
           PpmMappingTool="LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool")
ToolSvc += LVL1BS__RodHeaderByteStreamTool("RodHeaderByteStreamTool")
ToolSvc += LVL1BS__L1CaloErrorByteStreamTool("L1CaloErrorByteStreamTool")

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowers" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowersOverlap" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMXCPTob>/CMXCPTobs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMXCPHits>/CMXCPHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTobRoI>/CPMTobRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTobRoI>/CPMTobRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElements" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElementsOverlap" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMEtSums>/JEMEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMXJetTob>/CMXJetTobs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMXJetHits>/CMXJetHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMXEtSums>/CMXEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMTobRoI>/JEMTobRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMTobRoI>/JEMTobRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::CMXRoI/CMXRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::CMXRoI/CMXRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowers" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowersSpare" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowersMuon" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeaders" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersPP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersCP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersCPRoI" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersJEP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersJEPRoI" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersCPRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::RODHeader>/RODHeadersJEPRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "std::vector<unsigned int>/L1CaloUnpackingErrors" ]

#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()
#from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmTesterV1
#topSequence += LVL1BS__CpmTesterV1("CpmTesterV1")
#from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmTesterV2
#topSequence += LVL1BS__CpmTesterV2("CpmTesterV2")
#topSequence += LVL1BS__CpmTesterV2("CpmTesterV2", CMXCPTobPrint=2, CMXCPHitsPrint=2)
#from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JemTesterV1
#topSequence += LVL1BS__JemTesterV1("JemTesterV1")
#from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JemTesterV2
#topSequence += LVL1BS__JemTesterV2("JemTesterV2")
#topSequence += LVL1BS__JemTesterV2("JemTesterV2", CMXJetTobPrint=1)
