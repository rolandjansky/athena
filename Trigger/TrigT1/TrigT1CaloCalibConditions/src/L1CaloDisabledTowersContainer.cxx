/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"

#include <algorithm>
#include <memory>

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloDisabledTowers.h"

L1CaloDisabledTowersContainer::L1CaloDisabledTowersContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(edisabledBits, "disabledBits", "unsigned int");
}

L1CaloDisabledTowersContainer::L1CaloDisabledTowersContainer(const std::string& folderKey)
  : L1CaloDisabledTowersContainer() // delegating constructor
{
  // cppcheck-suppress useInitializationList
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloDisabledTowersContainer::makePersistent() const
{
  using std::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_disabledTowerss) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(edisabledBits)].setValue(item.disabledBits());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloDisabledTowersContainer::makeTransient(const std::map<std::string, const CondAttrListCollection*>& condAttrListCollectionMap)
{
  clear();
  
  // In the case of overlay, we need multiple instances of L1CaloPprChanDefaultsContainer
  // Take the last element in the map  
  if (condAttrListCollectionMap.empty()) return;
  auto it = condAttrListCollectionMap.rbegin();
  
  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto disabledBits = attrList[specificationName(edisabledBits)].data<unsigned int>();

    addDisabledTowers(L1CaloDisabledTowers(chanNum, disabledBits));
  }
}

const L1CaloDisabledTowers* L1CaloDisabledTowersContainer::disabledTowers(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_disabledTowerss),
                            std::end(m_disabledTowerss),
                            channelId,
                            [](const L1CaloDisabledTowers& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_disabledTowerss)) return nullptr;
  return &(*it);
}

void L1CaloDisabledTowersContainer::addDisabledTowers(const L1CaloDisabledTowers& disabledTowers)
{
  // insert into the correct position mainting the sorted vector
  m_disabledTowerss.insert(std::lower_bound(std::begin(m_disabledTowerss),
                                           std::end(m_disabledTowerss),
                                           disabledTowers.channelId(),
                                           [](const L1CaloDisabledTowers& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          disabledTowers);
}
