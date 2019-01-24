/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategyContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategy.h"

L1CaloPprChanStrategyContainer::L1CaloPprChanStrategyContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eStrategy, "Strategy", "string");
  this->addSpecification(eCode, "Code", "unsigned int");
  this->addSpecification(eTimingRegime, "TimingRegime", "string");
  this->addSpecification(eDescription, "Description", "string");
}

L1CaloPprChanStrategyContainer::L1CaloPprChanStrategyContainer(const std::string& folderKey)
  : L1CaloPprChanStrategyContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanStrategyContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_pprChanStrategys) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eStrategy)].setValue(item.strategy());
    attrList[specificationName(eCode)].setValue(item.code());
    attrList[specificationName(eTimingRegime)].setValue(item.timingRegime());
    attrList[specificationName(eDescription)].setValue(item.description());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanStrategyContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto strategy = attrList[specificationName(eStrategy)].data<std::string>();
    auto code = attrList[specificationName(eCode)].data<unsigned int>();
    auto timingRegime = attrList[specificationName(eTimingRegime)].data<std::string>();
    auto description = attrList[specificationName(eDescription)].data<std::string>();

    addPprChanStrategy(L1CaloPprChanStrategy(chanNum, strategy, code, timingRegime, description));
  }
}

const L1CaloPprChanStrategy* L1CaloPprChanStrategyContainer::pprChanStrategy(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanStrategys),
                            std::end(m_pprChanStrategys),
                            channelId,
                            [](const L1CaloPprChanStrategy& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_pprChanStrategys)) return nullptr;
  return &(*it);
}

void L1CaloPprChanStrategyContainer::addPprChanStrategy(const L1CaloPprChanStrategy& pprChanStrategy)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanStrategys.insert(std::lower_bound(std::begin(m_pprChanStrategys),
                                           std::end(m_pprChanStrategys),
                                           pprChanStrategy.channelId(),
                                           [](const L1CaloPprChanStrategy& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          pprChanStrategy);
}

void L1CaloPprChanStrategyContainer::dump() const {
  L1CaloPprChanStrategyContainer::const_iterator it = this->begin();
  for(;it!=this->end();++it) {
    std::cout << " * item: " << *it <<std::endl;
  }
}
