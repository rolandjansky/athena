/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGACCESSTOOL_H
#define TRIGGERMENUANALYSIS_TRIGACCESSTOOL_H
/*
  TrigAccessTool.h
*/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "StoreGate/StoreGateSvc.h"

class HLTObjectsInRoI;
class L1RoIObjects;

namespace HLT {
  class NavigationCore;
}
namespace Trig {
  class ChainGroup;
}

class TrigAccessTool : public AthAlgTool {
public:
  static const InterfaceID& interfaceID();

public:
  TrigAccessTool(const std::string& type, const std::string& name, 
		 const IInterface* p);
  virtual ~TrigAccessTool();

  StatusCode initialize();
  StatusCode finalize();

  inline StoreGateSvc* getStoreGateSvc();
  inline Trig::TrigDecisionTool* getTrigDecisionTool();

  HLTObjectsInRoI* findRoI(int isig, const HLT::TriggerElement* te, 
			   std::vector<HLTObjectsInRoI*>& objs, 
			   HLTObjectsInRoI* (*func)(), int level);

  bool TE_AseedsB(const HLT::TriggerElement* teA,
		  const HLT::TriggerElement* teB, 
		  HLT::NavigationCore* navitool); // NOT USED
  bool RoI_AseedsB(const HLTObjectsInRoI* roiA,
		   const HLT::TriggerElement* teB, 
		   HLT::NavigationCore* navitool);
  bool RoI_AseedsB(const HLTObjectsInRoI* roiA,
		   const HLT::TriggerElement* teB, 
		   Trig::TrigDecisionTool* tdt);

  bool getL1RoIObjects(L1RoIObjects& objs);

  bool getChainObjects(const std::string& chain_name, 
		       std::vector<HLTObjectsInRoI*>& objs, 
		       HLTObjectsInRoI* (*func)());
  template<class U>
  bool getChainObjects(const std::string& chain_name, 
		       std::vector<HLTObjectsInRoI*>& objs);

  void createChainGroup(const std::string& chain_name);

  bool decodeHLTResults();

  void printL1Results() const;

protected:
  bool decodeHLTResult(const std::string& name);

protected:
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

  std::map<std::string, const Trig::ChainGroup*> m_chainGroups;
};

StoreGateSvc* TrigAccessTool::getStoreGateSvc() {
  return &(*evtStore());
}

Trig::TrigDecisionTool* TrigAccessTool::getTrigDecisionTool() {
  return const_cast<Trig::TrigDecisionTool*>( &(*m_trigDecisionTool));
}

namespace __Kadda__ada15__ {
  template<class U>
  HLTObjectsInRoI* create() {
    void* x = new U();
    return reinterpret_cast<HLTObjectsInRoI*>(x);
  }
}
template<class U>
bool TrigAccessTool::getChainObjects(const std::string& chain_name, 
				     std::vector<HLTObjectsInRoI*>& objs) {
  return getChainObjects(chain_name, objs, __Kadda__ada15__::create<U>);
}


#endif // TRIGGERMENUANALYSIS_TRIGACCESSTOOL_H
