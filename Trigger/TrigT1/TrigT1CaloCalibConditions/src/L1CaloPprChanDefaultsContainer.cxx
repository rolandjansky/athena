/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"

L1CaloPprChanDefaultsContainer::L1CaloPprChanDefaultsContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eBcidDecision1, "CR12_BcidDecision1", "int");
  this->addSpecification(eSatOverride1, "CR12_SatOverride1", "int");
  this->addSpecification(eBcidDecision2, "CR13_BcidDecision2", "int");
  this->addSpecification(eSatOverride2, "CR13_SatOverride2", "int");
  this->addSpecification(eBcidDecision3, "CR14_BcidDecision3", "int");
  this->addSpecification(eSatOverride3, "CR14_SatOverride3", "int");
  this->addSpecification(ePeakFinderCond, "CR15_PeakFinderCond", "int");
  this->addSpecification(eDecisionSource, "CR15_DecisionSource", "int");

  this->addSpecification(eBypassLutCp, "CR00_BypassLutCp", "int");
  this->addSpecification(eBypassLutJep, "CR00_BypassLutJep", "int");
  this->addSpecification(eInDataSample, "CR00_InDataSample", "int");
  this->addSpecification(eReadOut80Active, "CR00_ReadOut80Active", "int");
  this->addSpecification(eFirCoeff1, "CR04_FirCoeff1", "int");
  this->addSpecification(eFirCoeff2, "CR04_FirCoeff2", "int");
  this->addSpecification(eFirCoeff3, "CR05_FirCoeff3", "int");
  this->addSpecification(eFirCoeff4, "CR06_FirCoeff4", "int");
  this->addSpecification(eFirCoeff5, "CR06_FirCoeff5", "int");
  this->addSpecification(eSatHigh, "CR07_SatHigh", "int");
  this->addSpecification(eSatLow, "CR08_SatLow", "int");
  this->addSpecification(eSatLevel, "CR09_SatLevel", "int");
  this->addSpecification(eEnergyLevelLow, "CR10_EnergyLevelLow", "int");
  this->addSpecification(eEnergyLevelHigh, "CR11_EnergyLevelHigh", "int");
  this->addSpecification(eStartBit, "CR15_StartBit", "int");
  this->addSpecification(eSaturationValue, "CR33_SaturationValue", "int");
  this->addSpecification(eNumBcRaw, "CR16_NumBcRaw", "int");
  this->addSpecification(eNumBcBcid, "CR16_NumBcBcid", "int");
  this->addSpecification(ePedCorrEnable, "CR43_PedCorrEnable", "int");
  // this->addSpecification(eSat80High, "CR52_Sat80High", "int");
  // this->addSpecification(eSat80Low, "CR53_Sat80Low", "int");
}

L1CaloPprChanDefaultsContainer::L1CaloPprChanDefaultsContainer(const std::string& folderKey)
  : L1CaloPprChanDefaultsContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanDefaultsContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_pprChanDefaultss) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eBcidDecision1)].setValue(item.bcidDecision1());
    attrList[specificationName(eSatOverride1)].setValue(item.satOverride1());
    attrList[specificationName(eBcidDecision2)].setValue(item.bcidDecision2());
    attrList[specificationName(eSatOverride2)].setValue(item.satOverride2());
    attrList[specificationName(eBcidDecision3)].setValue(item.bcidDecision3());
    attrList[specificationName(eSatOverride3)].setValue(item.satOverride3());
    attrList[specificationName(ePeakFinderCond)].setValue(item.peakFinderCond());
    attrList[specificationName(eDecisionSource)].setValue(item.decisionSource());
    attrList[specificationName(eBypassLutCp)].setValue(item.bypassLutCp());
    attrList[specificationName(eBypassLutJep)].setValue(item.bypassLutJep());
    attrList[specificationName(eInDataSample)].setValue(item.inDataSample());
    attrList[specificationName(eReadOut80Active)].setValue(item.readOut80Active());
    try {
      attrList[specificationName(eFirCoeff1)].setValue(item.firCoeff1());
      attrList[specificationName(eFirCoeff2)].setValue(item.firCoeff2());
      attrList[specificationName(eFirCoeff3)].setValue(item.firCoeff3());
      attrList[specificationName(eFirCoeff4)].setValue(item.firCoeff4());
      attrList[specificationName(eFirCoeff5)].setValue(item.firCoeff5());
    } catch(...) {
      // unfortunately the online database uses FIRCoeff and the offline database FirCoeff
      attrList[specificationName(eFirCoeff1).replace(5, 3, "FIR")].setValue(item.firCoeff1());
      attrList[specificationName(eFirCoeff2).replace(5, 3, "FIR")].setValue(item.firCoeff2());
      attrList[specificationName(eFirCoeff3).replace(5, 3, "FIR")].setValue(item.firCoeff3());
      attrList[specificationName(eFirCoeff4).replace(5, 3, "FIR")].setValue(item.firCoeff4());
      attrList[specificationName(eFirCoeff5).replace(5, 3, "FIR")].setValue(item.firCoeff5());
    }
    attrList[specificationName(eSatHigh)].setValue(item.satHigh());
    attrList[specificationName(eSatLow)].setValue(item.satLow());
    attrList[specificationName(eStartBit)].setValue(item.startBit());
    attrList[specificationName(eEnergyLevelHigh)].setValue(item.energyLevelHigh());
    attrList[specificationName(eEnergyLevelLow)].setValue(item.energyLevelLow());
    attrList[specificationName(eSatLevel)].setValue(item.satLevel());
    attrList[specificationName(eSaturationValue)].setValue(item.saturationValue());
    attrList[specificationName(eNumBcRaw)].setValue(item.numBcRaw());
    attrList[specificationName(eNumBcBcid)].setValue(item.numBcBcid());
    attrList[specificationName(ePedCorrEnable)].setValue(item.pedCorrEnable());
    // attrList[specificationName(eSat80High)].setValue(item.sat80High());
    // attrList[specificationName(eSat80Low)].setValue(item.sat80Low());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanDefaultsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
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
    
    auto bcidDecision1 = attrList[specificationName(eBcidDecision1)].data<int>();
    auto satOverride1 = attrList[specificationName(eSatOverride1)].data<int>();
    auto bcidDecision2 = attrList[specificationName(eBcidDecision2)].data<int>();
    auto satOverride2 = attrList[specificationName(eSatOverride2)].data<int>();
    auto bcidDecision3 = attrList[specificationName(eBcidDecision3)].data<int>();
    auto satOverride3 = attrList[specificationName(eSatOverride3)].data<int>();
    auto peakFinderCond = attrList[specificationName(ePeakFinderCond)].data<int>();
    auto decisionSource = attrList[specificationName(eDecisionSource)].data<int>();

    auto bypassLutCp = attrList[specificationName(eBypassLutCp)].data<int>();
    auto bypassLutJep = attrList[specificationName(eBypassLutJep)].data<int>();
    auto inDataSample = attrList[specificationName(eInDataSample)].data<int>();
    auto readOut80Active = attrList[specificationName(eReadOut80Active)].data<int>();
    auto satHigh = attrList[specificationName(eSatHigh)].data<int>();
    auto satLow = attrList[specificationName(eSatLow)].data<int>();
    auto startBit = attrList[specificationName(eStartBit)].data<int>();
    auto energyLevelHigh = attrList[specificationName(eEnergyLevelHigh)].data<int>();
    auto energyLevelLow = attrList[specificationName(eEnergyLevelLow)].data<int>();
    auto satLevel = attrList[specificationName(eSatLevel)].data<int>();
    auto saturationValue = attrList[specificationName(eSaturationValue)].data<int>();
    auto numBcRaw = attrList[specificationName(eNumBcRaw)].data<int>();
    auto numBcBcid = attrList[specificationName(eNumBcBcid)].data<int>();
    auto pedCorrEnable = attrList[specificationName(ePedCorrEnable)].data<int>();
    // auto sat80High = attrList[specificationName(eSat80High)].data<int>();
    // auto sat80Low = attrList[specificationName(eSat80Low)].data<int>();

    int firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5;
    try {
      firCoeff1 = attrList[specificationName(eFirCoeff1)].data<int>();
      firCoeff2 = attrList[specificationName(eFirCoeff2)].data<int>();
      firCoeff3 = attrList[specificationName(eFirCoeff3)].data<int>();
      firCoeff4 = attrList[specificationName(eFirCoeff4)].data<int>();
      firCoeff5 = attrList[specificationName(eFirCoeff5)].data<int>();
    } catch(...) {
      // unfortunately the online database uses FIRCoeff and the offline database FirCoeff
      firCoeff1 = attrList[specificationName(eFirCoeff1).replace(5, 3, "FIR")].data<int>();
      firCoeff2 = attrList[specificationName(eFirCoeff2).replace(5, 3, "FIR")].data<int>();
      firCoeff3 = attrList[specificationName(eFirCoeff3).replace(5, 3, "FIR")].data<int>();
      firCoeff4 = attrList[specificationName(eFirCoeff4).replace(5, 3, "FIR")].data<int>();
      firCoeff5 = attrList[specificationName(eFirCoeff5).replace(5, 3, "FIR")].data<int>();
    }

    addPprChanDefaults(L1CaloPprChanDefaults(chanNum, bcidDecision1, satOverride1, bcidDecision2, satOverride2, bcidDecision3, satOverride3, peakFinderCond, decisionSource, bypassLutCp, bypassLutJep, inDataSample, readOut80Active, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5, satHigh, satLow, startBit, energyLevelHigh, energyLevelLow, satLevel, saturationValue, numBcRaw, numBcBcid, pedCorrEnable /*, sat80High, sat80Low */ ));
  }
}

const L1CaloPprChanDefaults* L1CaloPprChanDefaultsContainer::pprChanDefaults(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanDefaultss),
                            std::end(m_pprChanDefaultss),
                            channelId,
                            [](const L1CaloPprChanDefaults& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_pprChanDefaultss)) return nullptr;
  return &(*it);
}

void L1CaloPprChanDefaultsContainer::addPprChanDefaults(const L1CaloPprChanDefaults& pprChanDefaults)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanDefaultss.insert(std::lower_bound(std::begin(m_pprChanDefaultss),
                                           std::end(m_pprChanDefaultss),
                                           pprChanDefaults.channelId(),
                                           [](const L1CaloPprChanDefaults& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          pprChanDefaults);
}
