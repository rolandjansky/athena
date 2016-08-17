/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanExtraContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanExtra.h"

L1CaloPprChanExtraContainer::L1CaloPprChanExtraContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eErrorCode, "ErrorCode", "unsigned int");
  this->addSpecification(ePedCorrEnable, "PedCorrEnable", "unsigned short");
  this->addSpecification(eSat80BcidThreshHigh, "Sat80BcidThreshHigh", "unsigned short");
  this->addSpecification(eSat80BcidThreshLow, "Sat80BcidThreshLow", "unsigned short");
  this->addSpecification(eThreshNumSamplesPeak, "ThreshNumSamplesPeak", "unsigned short");
  this->addSpecification(eThreshNumSamplesSat, "ThreshNumSamplesSat", "unsigned short");
  this->addSpecification(eSatOverride1, "SatOverride1", "unsigned short");
  this->addSpecification(eSatOverride2, "SatOverride2", "unsigned short");
  this->addSpecification(eSatOverride3, "SatOverride3", "unsigned short");
  this->addSpecification(eLutCpPar5, "LutCpPar5", "short");
  this->addSpecification(eLutCpPar6, "LutCpPar6", "short");
  this->addSpecification(eLutCpPar7, "LutCpPar7", "short");
  this->addSpecification(eLutJepPar5, "LutJepPar5", "short");
  this->addSpecification(eLutJepPar6, "LutJepPar6", "short");
  this->addSpecification(eLutJepPar7, "LutJepPar7", "short");
}

L1CaloPprChanExtraContainer::L1CaloPprChanExtraContainer(const std::string& folderKey)
  : L1CaloPprChanExtraContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanExtraContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_pprChanExtras) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eErrorCode)].setValue(item.errorCode());
    attrList[specificationName(ePedCorrEnable)].setValue(item.pedCorrEnable());
    attrList[specificationName(eSat80BcidThreshHigh)].setValue(item.sat80BcidThreshHigh());
    attrList[specificationName(eSat80BcidThreshLow)].setValue(item.sat80BcidThreshLow());
    attrList[specificationName(eThreshNumSamplesPeak)].setValue(item.threshNumSamplesPeak());
    attrList[specificationName(eThreshNumSamplesSat)].setValue(item.threshNumSamplesSat());
    attrList[specificationName(eSatOverride1)].setValue(item.satOverride1());
    attrList[specificationName(eSatOverride2)].setValue(item.satOverride2());
    attrList[specificationName(eSatOverride3)].setValue(item.satOverride3());
    attrList[specificationName(eLutCpPar5)].setValue(item.lutCpPar5());
    attrList[specificationName(eLutCpPar6)].setValue(item.lutCpPar6());
    attrList[specificationName(eLutCpPar7)].setValue(item.lutCpPar7());
    attrList[specificationName(eLutJepPar5)].setValue(item.lutJepPar5());
    attrList[specificationName(eLutJepPar6)].setValue(item.lutJepPar6());
    attrList[specificationName(eLutJepPar7)].setValue(item.lutJepPar7());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanExtraContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto errorCode = attrList[specificationName(eErrorCode)].data<unsigned int>();
    auto pedCorrEnable = attrList[specificationName(ePedCorrEnable)].data<unsigned short>();
    auto sat80BcidThreshHigh = attrList[specificationName(eSat80BcidThreshHigh)].data<unsigned short>();
    auto sat80BcidThreshLow = attrList[specificationName(eSat80BcidThreshLow)].data<unsigned short>();
    auto threshNumSamplesPeak = attrList[specificationName(eThreshNumSamplesPeak)].data<unsigned short>();
    auto threshNumSamplesSat = attrList[specificationName(eThreshNumSamplesSat)].data<unsigned short>();
    auto satOverride1 = attrList[specificationName(eSatOverride1)].data<unsigned short>();
    auto satOverride2 = attrList[specificationName(eSatOverride1)].data<unsigned short>();
    auto satOverride3 = attrList[specificationName(eSatOverride1)].data<unsigned short>();
    auto lutCpPar5 = attrList[specificationName(eLutCpPar5)].data<short>();
    auto lutCpPar6 = attrList[specificationName(eLutCpPar6)].data<short>();
    auto lutCpPar7 = attrList[specificationName(eLutCpPar7)].data<short>();
    auto lutJepPar5 = attrList[specificationName(eLutJepPar5)].data<short>();
    auto lutJepPar6 = attrList[specificationName(eLutJepPar6)].data<short>();
    auto lutJepPar7 = attrList[specificationName(eLutJepPar7)].data<short>();

    addPprChanExtra(L1CaloPprChanExtra(chanNum, errorCode, pedCorrEnable, sat80BcidThreshHigh, sat80BcidThreshLow, threshNumSamplesPeak, threshNumSamplesSat, satOverride1, satOverride2, satOverride3, lutCpPar5, lutCpPar6, lutCpPar7, lutJepPar5, lutJepPar6, lutJepPar7));
  }
}

const L1CaloPprChanExtra* L1CaloPprChanExtraContainer::pprChanExtra(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanExtras),
                            std::end(m_pprChanExtras),
                            channelId,
                            [](const L1CaloPprChanExtra& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_pprChanExtras)) return nullptr;
  return &(*it);
}

void L1CaloPprChanExtraContainer::addPprChanExtra(const L1CaloPprChanExtra& pprChanExtra)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanExtras.insert(std::lower_bound(std::begin(m_pprChanExtras),
                                           std::end(m_pprChanExtras),
                                           pprChanExtra.channelId(),
                                           [](const L1CaloPprChanExtra& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          pprChanExtra);
}
