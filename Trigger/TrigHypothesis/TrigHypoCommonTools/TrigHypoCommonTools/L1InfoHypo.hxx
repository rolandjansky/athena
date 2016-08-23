#ifndef TRIGHYPOCOMMONTOOLS_L1INFOHYPO_HXX
#define TRIGHYPOCOMMONTOOLS_L1INFOHYPO_HXX
/*
  L1InfoHypo
*/
#include <string>
#include <vector>
#include <set>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInterfaces/IMonitoredAlgo.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

class IRegSelSvc;

namespace HLT 
{
  class ILvl1ResultAccessTool;
  class Chain;
}

class L1InfoHypo : public HLT::HypoAlgo {
public:
  L1InfoHypo(const std::string& name, ISvcLocator* svc);
  ~L1InfoHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();
  HLT::ErrorCode hltEndRun();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE,bool &Pass);
  HLT::ErrorCode hltFinalize();


protected:
  // Flow of the algorithm

protected:

  // JobOption properties

  bool m_alwaysPass;
  bool m_invertSelection;
  bool m_invertBitMaskSelection;
  bool m_invertL1ItemNameSelection;
  bool m_useBeforePrescaleBit;
  std::vector<std::string> m_L1ItemNames;

  unsigned int m_triggerTypeBitMask;
  unsigned int m_L1TriggerBitMask;

  unsigned int m_triggerTypeBit;
  unsigned int m_L1TriggerBit;

  // Output object

  // Internal data structures used by this algorithm

  // Monitored Quantities


private:
  ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool;
};

#endif // TRIGHYPOCOMMONTOOLS_L1INFOHYPO_HXX
