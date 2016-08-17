/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsV1Container.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsV1.h"

L1CaloPprChanDefaultsV1Container::L1CaloPprChanDefaultsV1Container()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eBcidDecision1, "CR12_BcidDecision1", "unsigned int");
  this->addSpecification(eSatOverride1, "CR12_SatOverride1", "unsigned int");
  this->addSpecification(eBcidDecision2, "CR13_BcidDecision2", "unsigned int");
  this->addSpecification(eSatOverride2, "CR13_SatOverride2", "unsigned int");
  this->addSpecification(eBcidDecision3, "CR14_BcidDecision3", "unsigned int");
  this->addSpecification(eSatOverride3, "CR14_SatOverride3", "unsigned int");
  this->addSpecification(ePeakFinderCond, "CR15_PeakFinderCond", "unsigned int");
  this->addSpecification(eDecisionSource, "CR15_DecisionSource", "unsigned int");
}

L1CaloPprChanDefaultsV1Container::L1CaloPprChanDefaultsV1Container(const std::string& folderKey)
  : L1CaloPprChanDefaultsV1Container() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanDefaultsV1Container::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_pprChanDefaultsV1s) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eBcidDecision1)].setValue(item.bcidDecision1());
    attrList[specificationName(eSatOverride1)].setValue(item.satOverride1());
    attrList[specificationName(eBcidDecision2)].setValue(item.bcidDecision2());
    attrList[specificationName(eSatOverride2)].setValue(item.satOverride2());
    attrList[specificationName(eBcidDecision3)].setValue(item.bcidDecision3());
    attrList[specificationName(eSatOverride3)].setValue(item.satOverride3());
    attrList[specificationName(ePeakFinderCond)].setValue(item.peakFinderCond());
    attrList[specificationName(eDecisionSource)].setValue(item.decisionSource());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanDefaultsV1Container::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto BcidDecision1 = attrList[specificationName(eBcidDecision1)].data<unsigned int>();
    auto SatOverride1 = attrList[specificationName(eSatOverride1)].data<unsigned int>();
    auto BcidDecision2 = attrList[specificationName(eBcidDecision2)].data<unsigned int>();
    auto SatOverride2 = attrList[specificationName(eSatOverride2)].data<unsigned int>();
    auto BcidDecision3 = attrList[specificationName(eBcidDecision3)].data<unsigned int>();
    auto SatOverride3 = attrList[specificationName(eSatOverride3)].data<unsigned int>();
    auto PeakFinderCond = attrList[specificationName(ePeakFinderCond)].data<unsigned int>();
    auto DecisionSource = attrList[specificationName(eDecisionSource)].data<unsigned int>();

    addPprChanDefaultsV1(L1CaloPprChanDefaultsV1(chanNum, BcidDecision1, SatOverride1, BcidDecision2, SatOverride2, BcidDecision3, SatOverride3, PeakFinderCond, DecisionSource));
  }
}

const L1CaloPprChanDefaultsV1* L1CaloPprChanDefaultsV1Container::pprChanDefaultsV1(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanDefaultsV1s),
                             std::end(m_pprChanDefaultsV1s),
                             channelId,
                             [](const L1CaloPprChanDefaultsV1& el, unsigned int val) -> bool {
                               return el.channelId() < val;
                             });
  if(it == std::end(m_pprChanDefaultsV1s)) return nullptr;
  return &(*it);
}

void L1CaloPprChanDefaultsV1Container::addPprChanDefaultsV1(const L1CaloPprChanDefaultsV1& pprChanDefaultsV1)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanDefaultsV1s.insert(std::lower_bound(std::begin(m_pprChanDefaultsV1s),
                                               std::end(m_pprChanDefaultsV1s),
                                               pprChanDefaultsV1.channelId(),
                                               [](const L1CaloPprChanDefaultsV1& el, unsigned int va) -> bool {
                                                 return el.channelId() < va;
                                               }),
                              pprChanDefaultsV1);
}
