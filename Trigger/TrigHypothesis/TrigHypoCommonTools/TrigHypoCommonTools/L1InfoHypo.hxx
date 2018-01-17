#ifndef __L1InfoHypo_hxx__
#define __L1InfoHypo_hxx__
/*
  L1InfoHypo
*/
#include <string>
#include <vector>
#include <set>
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

  bool mAlwaysPass;
  bool mInvertSelection;
  bool mInvertBitMaskSelection;
  bool mInvertL1ItemNameSelection;
  bool mUseBeforePrescaleBit;
  std::vector<std::string> mL1ItemNames;

  unsigned int mTriggerTypeBitMask;
  unsigned int mL1TriggerBitMask;

  unsigned int mTriggerTypeBit;
  unsigned int mL1TriggerBit;

  // Output object

  // Internal data structures used by this algorithm

  // Monitored Quantities


private:
  ToolHandle<HLT::ILvl1ResultAccessTool> mlvl1Tool;
};

#endif // __L1InfoHypo_hxx__
