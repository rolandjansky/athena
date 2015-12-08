# Bytestream to TrigT1Calo objects conversions for data with Run 1 formats
include.block("TrigT1CaloByteStream/ReadLVL1CaloBSRun1_jobOptions.py")

# the following include is needed to load correctly the trigger towers on/off, TT/Cells maps
include( "CaloConditions/CaloConditions_jobOptions.py" )
# To setup correctly the LArCablingService when doLAr is off in the top option.
from RecExConfig.RecFlags import rec
if not rec.doLArg():
    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
    include( "LArIdCnv/LArIdCnv_joboptions.py" )

from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmRoiByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepRoiByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamV1Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamReadV1V2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__RodHeaderByteStreamTool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__L1CaloErrorByteStreamTool

ToolSvc = Service("ToolSvc")
ToolSvc += LVL1BS__CpByteStreamV1Tool("CpByteStreamV1Tool")
ToolSvc += LVL1BS__CpmRoiByteStreamV1Tool("CpmRoiByteStreamV1Tool")
ToolSvc += LVL1BS__JepByteStreamV1Tool("JepByteStreamV1Tool")
ToolSvc += LVL1BS__JepRoiByteStreamV1Tool("JepRoiByteStreamV1Tool")

ToolSvc += LVL1BS__PpmByteStreamV1Tool("PpmByteStreamV1Tool",
           PpmMappingTool="LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool")
ToolSvc += LVL1BS__PpmByteStreamReadV1V2Tool("PpmByteStreamReadV1V2Tool")

ToolSvc += LVL1BS__RodHeaderByteStreamTool("RodHeaderByteStreamTool")
ToolSvc += LVL1BS__L1CaloErrorByteStreamTool("L1CaloErrorByteStreamTool")

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowers" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMTower>/CPMTowersOverlap" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMHits>/CPMHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMMCPHits>/CMMCPHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMRoI>/CPMRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMRoI>/CPMRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElements" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JetElement>/JetElementsOverlap" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMHits>/JEMHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMEtSums>/JEMEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMMJetHits>/CMMJetHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CMMEtSums>/CMMEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMRoI>/JEMRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::JEMRoI>/JEMRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::CMMRoI/CMMRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::CMMRoI/CMMRoIsRoIB" ]

ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowers" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowersMuon" ]
ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowersSpare" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowers"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowersMuon"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersMuonAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowersSpare"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersSpareAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/ZdcTriggerTowers"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/ZdcTriggerTowersAux."]


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
