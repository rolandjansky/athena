/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"

L1CaloRunParametersContainer::L1CaloRunParametersContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(erunType, "runType", "string");
  this->addSpecification(erunActionName, "runActionName", "string");
  this->addSpecification(erunActionVersion, "runActionVersion", "unsigned int");
  this->addSpecification(ereadoutConfig, "readoutConfig", "string");
  this->addSpecification(ereadoutConfigID, "readoutConfigID", "unsigned int");
  this->addSpecification(ettcConfiguration, "ttcConfiguration", "string");
  this->addSpecification(ettcConfigurationID, "ttcConfigurationID", "unsigned int");
  this->addSpecification(etriggerMenu, "triggerMenu", "string");
  this->addSpecification(ecalibration, "calibration", "string");
  this->addSpecification(econditions, "conditions", "string");
}

L1CaloRunParametersContainer::L1CaloRunParametersContainer(const std::string& folderKey)
  : L1CaloRunParametersContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloRunParametersContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_runParameterss) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(erunType)].setValue(item.runType());
    attrList[specificationName(erunActionName)].setValue(item.runActionName());
    attrList[specificationName(erunActionVersion)].setValue(item.runActionVersion());
    attrList[specificationName(ereadoutConfig)].setValue(item.readoutConfig());
    attrList[specificationName(ereadoutConfigID)].setValue(item.readoutConfigID());
    attrList[specificationName(ettcConfiguration)].setValue(item.ttcConfiguration());
    attrList[specificationName(ettcConfigurationID)].setValue(item.ttcConfigurationID());
    attrList[specificationName(etriggerMenu)].setValue(item.triggerMenu());
    attrList[specificationName(ecalibration)].setValue(item.calibration());
    attrList[specificationName(econditions)].setValue(item.conditions());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloRunParametersContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto runType = attrList[specificationName(erunType)].data<std::string>();
    auto runActionName = attrList[specificationName(erunActionName)].data<std::string>();
    auto runActionVersion = attrList[specificationName(erunActionVersion)].data<unsigned int>();
    auto readoutConfig = attrList[specificationName(ereadoutConfig)].data<std::string>();
    auto readoutConfigID = attrList[specificationName(ereadoutConfigID)].data<unsigned int>();
    auto ttcConfiguration = attrList[specificationName(ettcConfiguration)].data<std::string>();
    auto ttcConfigurationID = attrList[specificationName(ettcConfigurationID)].data<unsigned int>();
    auto triggerMenu = attrList[specificationName(etriggerMenu)].data<std::string>();
    auto calibration = attrList[specificationName(ecalibration)].data<std::string>();
    auto conditions = attrList[specificationName(econditions)].data<std::string>();

    addRunParameters(L1CaloRunParameters(chanNum, runType, runActionName, runActionVersion, readoutConfig, readoutConfigID, ttcConfiguration, ttcConfigurationID, triggerMenu, calibration, conditions));
  }
}

const L1CaloRunParameters* L1CaloRunParametersContainer::runParameters(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_runParameterss),
                            std::end(m_runParameterss),
                            channelId,
                            [](const L1CaloRunParameters& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_runParameterss)) return nullptr;
  return &(*it);
}

void L1CaloRunParametersContainer::addRunParameters(const L1CaloRunParameters& runParameters)
{
  // insert into the correct position mainting the sorted vector
  m_runParameterss.insert(std::lower_bound(std::begin(m_runParameterss),
                                           std::end(m_runParameterss),
                                           runParameters.channelId(),
                                           [](const L1CaloRunParameters& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          runParameters);
}

void L1CaloRunParametersContainer::dump() const {
  L1CaloRunParametersContainer::const_iterator it = this->begin();
  for(;it!=this->end();++it) {
    std::cout << " * item: " << *it <<std::endl;
  }
}
