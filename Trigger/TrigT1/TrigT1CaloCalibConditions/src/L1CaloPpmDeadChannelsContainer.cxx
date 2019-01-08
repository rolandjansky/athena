/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannels.h"

L1CaloPpmDeadChannelsContainer::L1CaloPpmDeadChannelsContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eErrorCode, "ErrorCode", "unsigned int");
  this->addSpecification(eNoiseCut, "NoiseCut", "unsigned int");
}

L1CaloPpmDeadChannelsContainer::L1CaloPpmDeadChannelsContainer(const std::string& folderKey)
  : L1CaloPpmDeadChannelsContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPpmDeadChannelsContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_ppmDeadChannelss) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eErrorCode)].setValue(item.errorCode());
    attrList[specificationName(eNoiseCut)].setValue(item.noiseCut());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPpmDeadChannelsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();
  
  // In the case of overlay, we need multiple instances of L1CaloPpmDeadChannelsContainer
  // Take the last element in the map  
  if (condAttrListCollectionMap.empty()) return;
  auto it = condAttrListCollectionMap.rbegin();

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto errorCode = attrList[specificationName(eErrorCode)].data<unsigned int>();
    auto noiseCut = attrList[specificationName(eNoiseCut)].data<unsigned int>();

    addPpmDeadChannels(L1CaloPpmDeadChannels(chanNum, errorCode, noiseCut));
  }
}

const L1CaloPpmDeadChannels* L1CaloPpmDeadChannelsContainer::ppmDeadChannels(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_ppmDeadChannelss),
                            std::end(m_ppmDeadChannelss),
                            channelId,
                            [](const L1CaloPpmDeadChannels& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_ppmDeadChannelss)) return nullptr;
  return &(*it);
}

void L1CaloPpmDeadChannelsContainer::addPpmDeadChannels(const L1CaloPpmDeadChannels& ppmDeadChannels)
{
  // insert into the correct position mainting the sorted vector
  m_ppmDeadChannelss.insert(std::lower_bound(std::begin(m_ppmDeadChannelss),
                                           std::end(m_ppmDeadChannelss),
                                           ppmDeadChannels.channelId(),
                                           [](const L1CaloPpmDeadChannels& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          ppmDeadChannels);
}
