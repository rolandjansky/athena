# Bytestream to TrigT1Calo objects conversions for data with Run 2 formats
include.block("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

# the following include is needed to load correctly the trigger towers on/off, TT/Cells maps
include( "CaloConditions/CaloConditions_jobOptions.py" )
# To setup correctly the LArCablingService when doLAr is off in the top option.
from RecExConfig.RecFlags import rec
if not rec.doLArg():
    include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
    include( "LArIdCnv/LArIdCnv_joboptions.py" )

# Topo
include("L1TopoByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__CpmRoiByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__JepRoiByteStreamV2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__PpmByteStreamReadV1V2Tool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__RodHeaderByteStreamTool
from TrigT1CaloByteStream.TrigT1CaloByteStreamConf import LVL1BS__L1CaloErrorByteStreamTool

ToolSvc = Service("ToolSvc")
ToolSvc += LVL1BS__CpByteStreamV2Tool("CpByteStreamV2Tool")
ToolSvc += LVL1BS__CpmRoiByteStreamV2Tool("CpmRoiByteStreamV2Tool")
ToolSvc += LVL1BS__JepByteStreamV2Tool("JepByteStreamV2Tool")
ToolSvc += LVL1BS__JepRoiByteStreamV2Tool("JepRoiByteStreamV2Tool")
ToolSvc += LVL1BS__PpmByteStreamReadV1V2Tool("PpmByteStreamReadV1V2Tool")
ToolSvc += LVL1BS__RodHeaderByteStreamTool("RodHeaderByteStreamTool")
ToolSvc += LVL1BS__L1CaloErrorByteStreamTool("L1CaloErrorByteStreamTool")

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )



# =============================================================================
# PPM
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowers"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowersMuon"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersMuonAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/xAODTriggerTowersSpare"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersSpareAux."]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/ZdcTriggerTowers"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/ZdcTriggerTowersAux."]

# Without prefix
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/TriggerTowers"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/TriggerTowersAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/TriggerTowersMuon"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/TriggerTowersMuonAux."]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/TriggerTowersSpare"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/TriggerTowersSpareAux."]
# =============================================================================
# CPM
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTowerContainer/CPMTowers" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTowerAuxContainer/CPMTowersAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTowerContainer/CPMTowersOverlap" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTowerAuxContainer/CPMTowersOverlapAux." ]
# =============================================================================
# CMXCPTOB
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXCPTobContainer/CMXCPTobs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXCPTobAuxContainer/CMXCPTobsAux." ]
# =============================================================================
# CMXCPHITS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXCPHitsContainer/CMXCPHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXCPHitsAuxContainer/CMXCPHitsAux." ]
# =============================================================================
# CMXJETTOB
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXJetTobContainer/CMXJetTobs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXJetTobAuxContainer/CMXJetTobsAux." ]
# =============================================================================
# CMXJETHITS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXJetHitsContainer/CMXJetHits" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXJetHitsAuxContainer/CMXJetHitsAux." ]
# =============================================================================
# CMXETSUMS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXEtSumsContainer/CMXEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXEtSumsAuxContainer/CMXEtSumsAux." ]
# =============================================================================
# JEMETSUMS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMEtSumsContainer/JEMEtSums" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMEtSumsAuxContainer/JEMEtSumsAux." ]

# =============================================================================
# CPMTOBROIS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTobRoIContainer/CPMTobRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTobRoIAuxContainer/CPMTobRoIsAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTobRoIContainer/CPMTobRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CPMTobRoIAuxContainer/CPMTobRoIsRoIBAux." ]

# =============================================================================
# JEMTOBROIS
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMTobRoIContainer/JEMTobRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMTobRoIAuxContainer/JEMTobRoIsAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMTobRoIContainer/JEMTobRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JEMTobRoIAuxContainer/JEMTobRoIsRoIBAux." ]

# =============================================================================
# JETELEMENT
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JetElementContainer/JetElements"]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::JetElementAuxContainer/JetElementsAux." ]

ByteStreamAddressProviderSvc.TypeNames += ["xAOD::JetElementContainer/JetElementsOverlap"]
ByteStreamAddressProviderSvc.TypeNames += ["xAOD::JetElementAuxContainer/JetElementsOverlapAux."]
# =============================================================================
# CMXROI
# =============================================================================
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXRoIContainer/CMXRoIs" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXRoIAuxContainer/CMXRoIsAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXRoIContainer/CMXRoIsRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::CMXRoIAuxContainer/CMXRoIsRoIBAux."]
# =============================================================================
# RODHEADER
# =============================================================================

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeaders" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersPP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersPPAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersCP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersCPAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersCPRoI" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersCPRoIAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersJEP" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersJEPAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersJEPRoI" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersJEPRoIAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersCPRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersCPRoIBAux." ]

ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderContainer/RODHeadersJEPRoIB" ]
ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::RODHeaderAuxContainer/RODHeadersJEPRoIBAux." ]

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
