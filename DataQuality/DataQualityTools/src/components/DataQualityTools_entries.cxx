#include "DataQualityTools/DataQualityFatherMonTool.h"
//#include "DataQualityTools/DQTMuTrkEff.h"
#include "DataQualityTools/DQTDetSynchMonTool.h"
#include "DataQualityTools/DQTMuonIDTrackTool.h"
//#include "DataQualityTools/DQTElectronQualityTool.h"
//#include "DataQualityTools/DQTCaloClusterTool.h"
//#include "DataQualityTools/DQTNonCollBkg_ZDC.h"
#include "DataQualityTools/DQTBackgroundMon.h"
//#include "DataQualityTools/DQTGlobalWFinderTool.h"
//#include "DataQualityTools/DQTRateMonTool.h"
#include "DataQualityTools/DQTDataFlowMonTool.h"
//#include "DataQualityTools/DQTTopLeptonJetsFinderTool.h"
//#include "DataQualityTools/DQTGlobalTopDilFinderTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DataQualityTools/DQTGlobalWZFinderTool.h"
#include "DataQualityTools/DQTLumiMonTool.h"

DECLARE_TOOL_FACTORY(DataQualityFatherMonTool)
//DECLARE_TOOL_FACTORY(DQTMuTrkEff)
DECLARE_TOOL_FACTORY(DQTDetSynchMonTool)
DECLARE_TOOL_FACTORY(DQTMuonIDTrackTool)
//DECLARE_TOOL_FACTORY(DQTElectronQualityTool)
//DECLARE_TOOL_FACTORY(DQTGlobalWFinderTool)
//DECLARE_TOOL_FACTORY(DQTNonCollBkg_ZDC)
DECLARE_TOOL_FACTORY(DQTBackgroundMon)
//DECLARE_TOOL_FACTORY(DQTRateMonTool)
//DECLARE_TOOL_FACTORY(DQTCaloClusterTool)
DECLARE_TOOL_FACTORY(DQTDataFlowMonTool)
//DECLARE_TOOL_FACTORY(DQTTopLeptonJetsFinderTool)
//DECLARE_TOOL_FACTORY(DQTGlobalTopDilFinderTool)
DECLARE_TOOL_FACTORY(DQTGlobalWZFinderTool)
DECLARE_TOOL_FACTORY(DQTLumiMonTool)

DECLARE_FACTORY_ENTRIES(DataQualityTools) {
  DECLARE_ALGTOOL(DataQualityFatherMonTool);
//  DECLARE_ALGTOOL(DQTMuTrkEff);
  DECLARE_ALGTOOL(DQTDetSynchMonTool);
//  DECLARE_ALGTOOL(DQTCaloClusterTool);
  DECLARE_ALGTOOL(DQTMuonIDTrackTool);
//  DECLARE_ALGTOOL(DQTElectronQualityTool);
//  DECLARE_ALGTOOL(DQTGlobalWFinderTool);
//  DECLARE_ALGTOOL(DQTRateMonTool);	
  //  DECLARE_ALGTOOL(DQTNonCollBkg_ZDC);
  DECLARE_ALGTOOL(DQTBackgroundMon);
  DECLARE_ALGTOOL(DQTDataFlowMonTool);
//  DECLARE_ALGTOOL(DQTGlobalTopDilFinderTool);
//  DECLARE_ALGTOOL(DQTTopLeptonJetsFinderTool);
  DECLARE_ALGTOOL(DQTGlobalWZFinderTool);
  DECLARE_ALGTOOL(DQTLumiMonTool);
}
