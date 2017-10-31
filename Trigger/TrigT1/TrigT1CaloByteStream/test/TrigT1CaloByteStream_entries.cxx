#include "DataModel/DataVector.h"

// Post-LS1
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
// Pre-LS1
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
// Both
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JetElement.h"

// Post-LS1
#include "../src/CpByteStreamV2Cnv.h"
#include "../src/CpmRoiByteStreamV2Cnv.h"
#include "../src/CpReadByteStreamV2Cnv.h"
#include "../src/JepByteStreamV2Cnv.h"
#include "../src/JepReadByteStreamV2Cnv.h"
#include "../src/JepRoiByteStreamV2Cnv.h"
#include "../src/JepRoiReadByteStreamV2Cnv.h"
// Pre-LS1
#include "../src/CpByteStreamV1Cnv.h"
#include "../src/CpmRoiByteStreamV1Cnv.h"
#include "../src/CpReadByteStreamV1Cnv.h"
#include "../src/JepByteStreamV1Cnv.h"
#include "../src/JepReadByteStreamV1Cnv.h"
#include "../src/JepRoiByteStreamV1Cnv.h"
#include "../src/JepRoiReadByteStreamV1Cnv.h"
// Both
#include "../src/CpReadByteStreamV1V2Cnv.h"
#include "../src/JepReadByteStreamV1V2Cnv.h"
#include "../src/PpmByteStreamCnv.h"
#include "../src/RodHeaderByteStreamCnv.h"
#include "../src/L1CaloErrorByteStreamCnv.h"

// Post-LS1
#include "../src/CpByteStreamV2Tool.h"
#include "../src/CpmRoiByteStreamV2Tool.h"
#include "../src/JepByteStreamV2Tool.h"
#include "../src/JepRoiByteStreamV2Tool.h"
// Pre-LS1
#include "../src/CpByteStreamV1Tool.h"
#include "../src/CpmRoiByteStreamV1Tool.h"
#include "../src/JepByteStreamV1Tool.h"
#include "../src/JepRoiByteStreamV1Tool.h"
// Both
#include "../src/RodHeaderByteStreamTool.h"
#include "../src/L1CaloErrorByteStreamTool.h"

#include "../src/PpmByteStreamSubsetTool.h"
#include "../src/TriggerTowerSelectionTool.h"
#include "../src/TrigT1CaloDataAccess.h"

// Post-LS1
#include "CpmTesterV2.h"
#include "JemTesterV2.h"
#include "CpmErrors.h"
#include "JemErrors.h"
// Pre-LS1
#include "CpmTesterV1.h"
#include "JemTesterV1.h"
// Both
#include "PpmMappingTester.h"
#include "PpmSubsetTester.h"
#include "PpmTester.h"
#include "RodTester.h"
#include "ErrorTester.h"

namespace LVL1BS {

// Post-LS1
typedef DataVector<LVL1::CMXCPTob>   CMXCPTobCollection;
typedef DataVector<LVL1::CMXCPHits>  CMXCPHitsCollection;
typedef DataVector<LVL1::CMXJetTob>  CMXJetTobCollection;
typedef DataVector<LVL1::CMXJetHits> CMXJetHitsCollection;
typedef DataVector<LVL1::CMXEtSums>  CMXEtSumsCollection;
typedef DataVector<LVL1::JEMTobRoI>  JEMTobRoICollection;
// Pre-LS1
typedef DataVector<LVL1::CPMHits>    CPMHitsCollection;
typedef DataVector<LVL1::CMMCPHits>  CMMCPHitsCollection;
typedef DataVector<LVL1::CMMJetHits> CMMJetHitsCollection;
typedef DataVector<LVL1::CMMEtSums>  CMMEtSumsCollection;
typedef DataVector<LVL1::JEMHits>    JEMHitsCollection;
typedef DataVector<LVL1::JEMRoI>     JEMRoICollection;
// Both
typedef DataVector<LVL1::CPMTower>   CPMTowerCollection;
typedef DataVector<LVL1::JetElement> JetElementCollection;
typedef DataVector<LVL1::JEMEtSums>  JEMEtSumsCollection;

// Post-LS1
typedef CpReadByteStreamV2Cnv<CMXCPTobCollection>      CpReadCRByteStreamV2CnvT;
typedef CpReadByteStreamV2Cnv<CMXCPHitsCollection>     CpReadCCByteStreamV2CnvT;
typedef JepReadByteStreamV2Cnv<CMXJetTobCollection>    JepReadCTByteStreamV2CnvT;
typedef JepReadByteStreamV2Cnv<CMXJetHitsCollection>   JepReadCJByteStreamV2CnvT;
typedef JepReadByteStreamV2Cnv<CMXEtSumsCollection>    JepReadCEByteStreamV2CnvT;
typedef JepRoiReadByteStreamV2Cnv<JEMTobRoICollection> JepRoiReadJRByteStreamV2CnvT;
typedef JepRoiReadByteStreamV2Cnv<LVL1::CMXRoI>        JepRoiReadCRByteStreamV2CnvT;
// Pre-LS1
typedef CpReadByteStreamV1Cnv<CPMHitsCollection>       CpReadCHByteStreamV1CnvT;
typedef CpReadByteStreamV1Cnv<CMMCPHitsCollection>     CpReadCCByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<CMMJetHitsCollection>   JepReadCJByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<CMMEtSumsCollection>    JepReadCEByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<JEMHitsCollection>      JepReadJHByteStreamV1CnvT;
typedef JepRoiReadByteStreamV1Cnv<JEMRoICollection>    JepRoiReadJRByteStreamV1CnvT;
typedef JepRoiReadByteStreamV1Cnv<LVL1::CMMRoI>        JepRoiReadCRByteStreamV1CnvT;
// Both
typedef JepReadByteStreamV1V2Cnv<JetElementCollection> JepReadJEByteStreamV1V2CnvT;
typedef JepReadByteStreamV1V2Cnv<JEMEtSumsCollection>  JepReadESByteStreamV1V2CnvT;
}

// declare 
// Post-LS1
DECLARE_CONVERTER_FACTORY( LVL1BS::CpByteStreamV2Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpmRoiByteStreamV2Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpReadCRByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpReadCCByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepByteStreamV2Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiByteStreamV2Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadCTByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadCJByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadCEByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiReadJRByteStreamV2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiReadCRByteStreamV2CnvT )
// Pre-LS1
DECLARE_CONVERTER_FACTORY( LVL1BS::CpByteStreamV1Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpmRoiByteStreamV1Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpReadCHByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::CpReadCCByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepByteStreamV1Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiByteStreamV1Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadCJByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadCEByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadJHByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiReadJRByteStreamV1CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepRoiReadCRByteStreamV1CnvT )
// Both
DECLARE_CONVERTER_FACTORY( LVL1BS::CpReadByteStreamV1V2Cnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadJEByteStreamV1V2CnvT )
DECLARE_CONVERTER_FACTORY( LVL1BS::JepReadESByteStreamV1V2CnvT )
//DECLARE_CONVERTER_FACTORY( LVL1BS::PpmByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::RodHeaderByteStreamCnv )
DECLARE_CONVERTER_FACTORY( LVL1BS::L1CaloErrorByteStreamCnv )

// Post-LS1
DECLARE_COMPONENT( LVL1BS::CpByteStreamV2Tool )
DECLARE_COMPONENT( LVL1BS::CpmRoiByteStreamV2Tool )
DECLARE_COMPONENT( LVL1BS::JepByteStreamV2Tool )
DECLARE_COMPONENT( LVL1BS::JepRoiByteStreamV2Tool )
// Pre-LS1
DECLARE_COMPONENT( LVL1BS::CpByteStreamV1Tool )
DECLARE_COMPONENT( LVL1BS::CpmRoiByteStreamV1Tool )
DECLARE_COMPONENT( LVL1BS::JepByteStreamV1Tool )
DECLARE_COMPONENT( LVL1BS::JepRoiByteStreamV1Tool )
// Both
DECLARE_COMPONENT( LVL1BS::RodHeaderByteStreamTool )
DECLARE_COMPONENT( LVL1BS::L1CaloErrorByteStreamTool )

DECLARE_COMPONENT( LVL1BS::PpmByteStreamSubsetTool )
DECLARE_COMPONENT( LVL1BS::TriggerTowerSelectionTool )
DECLARE_COMPONENT( LVL1BS::TrigT1CaloDataAccess )

// Post-LS1
DECLARE_COMPONENT( LVL1BS::CpmTesterV2 )
DECLARE_COMPONENT( LVL1BS::JemTesterV2 )
DECLARE_COMPONENT( LVL1BS::CpmErrors )
DECLARE_COMPONENT( LVL1BS::JemErrors )
// Pre-LS1
DECLARE_COMPONENT( LVL1BS::CpmTesterV1 )
DECLARE_COMPONENT( LVL1BS::JemTesterV1 )
// Both
DECLARE_COMPONENT( LVL1BS::PpmTester )
DECLARE_COMPONENT( LVL1BS::RodTester )
DECLARE_COMPONENT( LVL1BS::ErrorTester )
DECLARE_COMPONENT( LVL1BS::PpmSubsetTester )
DECLARE_COMPONENT( LVL1BS::PpmMappingTester )

