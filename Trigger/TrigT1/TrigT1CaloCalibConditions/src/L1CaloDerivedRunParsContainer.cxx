/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunParsContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloDerivedRunPars.h"

L1CaloDerivedRunParsContainer::L1CaloDerivedRunParsContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(etimingRegime, "timingRegime", "string");
  this->addSpecification(etierZeroTag, "tierZeroTag", "string");
}

L1CaloDerivedRunParsContainer::L1CaloDerivedRunParsContainer(const std::string& folderKey)
  : L1CaloDerivedRunParsContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloDerivedRunParsContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_derivedRunParss) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(etimingRegime)].setValue(item.timingRegime());
    attrList[specificationName(etierZeroTag)].setValue(item.tierZeroTag());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloDerivedRunParsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto timingRegime = attrList[specificationName(etimingRegime)].data<std::string>();
    auto tierZeroTag = attrList[specificationName(etierZeroTag)].data<std::string>();

    addDerivedRunPars(L1CaloDerivedRunPars(chanNum, timingRegime, tierZeroTag));
  }
}

const L1CaloDerivedRunPars* L1CaloDerivedRunParsContainer::derivedRunPars(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_derivedRunParss),
                            std::end(m_derivedRunParss),
                            channelId,
                            [](const L1CaloDerivedRunPars& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_derivedRunParss)) return nullptr;
  return &(*it);
}

void L1CaloDerivedRunParsContainer::addDerivedRunPars(const L1CaloDerivedRunPars& derivedRunPars)
{
  // insert into the correct position mainting the sorted vector
  m_derivedRunParss.insert(std::lower_bound(std::begin(m_derivedRunParss),
                                           std::end(m_derivedRunParss),
                                           derivedRunPars.channelId(),
                                           [](const L1CaloDerivedRunPars& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          derivedRunPars);
}


void L1CaloDerivedRunParsContainer::dump() const {
  L1CaloDerivedRunParsContainer::const_iterator it = this->begin();
  for(;it!=this->end();++it) {
    std::cout << " * item: " << *it <<std::endl;
  }
}