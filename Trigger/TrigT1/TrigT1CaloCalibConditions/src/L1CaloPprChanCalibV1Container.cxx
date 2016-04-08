/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibV1Container.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibV1.h"

L1CaloPprChanCalibV1Container::L1CaloPprChanCalibV1Container()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eErrorCode, "ErrorCode", "unsigned int");
  this->addSpecification(ePedMean, "PedMean", "double");
  this->addSpecification(ePedValue, "PedValue", "unsigned int");
  this->addSpecification(eExtBcidThreshold, "ExtBcidThreshold", "unsigned short");
  this->addSpecification(eSatBcidThreshLow, "SatBcidThreshLow", "unsigned short");
  this->addSpecification(eSatBcidThreshHigh, "SatBcidThreshHigh", "unsigned short");
  this->addSpecification(eSatBcidLevel, "SatBcidLevel", "unsigned short");
  this->addSpecification(eBcidEnergyRangeLow, "BcidEnergyRangeLow", "unsigned short");
  this->addSpecification(eBcidEnergyRangeHigh, "BcidEnergyRangeHigh", "unsigned short");
  this->addSpecification(eFirStartBit, "FirStartBit", "unsigned short");
  this->addSpecification(eFirCoeff1, "FirCoeff1", "short");
  this->addSpecification(eFirCoeff2, "FirCoeff2", "short");
  this->addSpecification(eFirCoeff3, "FirCoeff3", "short");
  this->addSpecification(eFirCoeff4, "FirCoeff4", "short");
  this->addSpecification(eFirCoeff5, "FirCoeff5", "short");
  this->addSpecification(eLutStrategy, "LutStrategy", "unsigned short");
  this->addSpecification(eLutOffset, "LutOffset", "unsigned short");
  this->addSpecification(eLutNoiseCut, "LutNoiseCut", "unsigned short");
  this->addSpecification(eLutSlope, "LutSlope", "unsigned short");
}

L1CaloPprChanCalibV1Container::L1CaloPprChanCalibV1Container(const std::string& folderKey)
  : L1CaloPprChanCalibV1Container() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanCalibV1Container::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_pprChanCalibs) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(eErrorCode)].setValue(item.errorCode());
    attrList[specificationName(ePedMean)].setValue(item.pedMean());
    attrList[specificationName(ePedValue)].setValue(item.pedValue());
    attrList[specificationName(eExtBcidThreshold)].setValue(item.extBcidThreshold());
    attrList[specificationName(eSatBcidThreshLow)].setValue(item.satBcidThreshLow());
    attrList[specificationName(eSatBcidThreshHigh)].setValue(item.satBcidThreshHigh());
    attrList[specificationName(eSatBcidLevel)].setValue(item.satBcidLevel());
    attrList[specificationName(eBcidEnergyRangeLow)].setValue(item.bcidEnergyRangeLow());
    attrList[specificationName(eBcidEnergyRangeHigh)].setValue(item.bcidEnergyRangeHigh());
    attrList[specificationName(eFirStartBit)].setValue(item.firStartBit());
    attrList[specificationName(eFirCoeff1)].setValue(item.firCoeff1());
    attrList[specificationName(eFirCoeff2)].setValue(item.firCoeff2());
    attrList[specificationName(eFirCoeff3)].setValue(item.firCoeff3());
    attrList[specificationName(eFirCoeff4)].setValue(item.firCoeff4());
    attrList[specificationName(eFirCoeff5)].setValue(item.firCoeff5());
    attrList[specificationName(eLutStrategy)].setValue(item.lutStrategy());
    attrList[specificationName(eLutOffset)].setValue(item.lutOffset());
    attrList[specificationName(eLutNoiseCut)].setValue(item.lutNoiseCut());
    attrList[specificationName(eLutSlope)].setValue(item.lutSlope());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanCalibV1Container::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto errorCode = attrList[specificationName(eErrorCode)].data<unsigned int>();
    auto pedMean = attrList[specificationName(ePedMean)].data<double>();
    auto pedValue = attrList[specificationName(ePedValue)].data<unsigned int>();
    auto extBcidThreshold = attrList[specificationName(eExtBcidThreshold)].data<unsigned short>();
    auto satBcidThreshLow = attrList[specificationName(eSatBcidThreshLow)].data<unsigned short>();
    auto satBcidThreshHigh = attrList[specificationName(eSatBcidThreshHigh)].data<unsigned short>();
    auto satBcidLevel = attrList[specificationName(eSatBcidLevel)].data<unsigned short>();
    auto bcidEnergyRangeLow = attrList[specificationName(eBcidEnergyRangeLow)].data<unsigned short>();
    auto bcidEnergyRangeHigh = attrList[specificationName(eBcidEnergyRangeHigh)].data<unsigned short>();
    auto firStartBit = attrList[specificationName(eFirStartBit)].data<unsigned short>();
    auto firCoeff1 = attrList[specificationName(eFirCoeff1)].data<short>();
    auto firCoeff2 = attrList[specificationName(eFirCoeff2)].data<short>();
    auto firCoeff3 = attrList[specificationName(eFirCoeff3)].data<short>();
    auto firCoeff4 = attrList[specificationName(eFirCoeff4)].data<short>();
    auto firCoeff5 = attrList[specificationName(eFirCoeff5)].data<short>();
    auto lutStrategy = attrList[specificationName(eLutStrategy)].data<unsigned short>();
    auto lutOffset = attrList[specificationName(eLutOffset)].data<unsigned short>();
    auto lutNoiseCut = attrList[specificationName(eLutNoiseCut)].data<unsigned short>();
    auto lutSlope = attrList[specificationName(eLutSlope)].data<unsigned short>();

    addPprChanCalibV1(L1CaloPprChanCalibV1(chanNum, errorCode, pedMean, pedValue, extBcidThreshold, satBcidThreshLow, satBcidThreshHigh, satBcidLevel, bcidEnergyRangeLow, bcidEnergyRangeHigh, firStartBit, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5, lutStrategy, lutOffset, lutNoiseCut, lutSlope));
  }
}

const L1CaloPprChanCalibV1* L1CaloPprChanCalibV1Container::pprChanCalibV1(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanCalibs),
                            std::end(m_pprChanCalibs),
                            channelId,
                            [](const L1CaloPprChanCalibV1& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_pprChanCalibs)) return nullptr;
  return &(*it);
}

void L1CaloPprChanCalibV1Container::addPprChanCalibV1(const L1CaloPprChanCalibV1& pprChanCalib)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanCalibs.insert(std::lower_bound(std::begin(m_pprChanCalibs),
                                           std::end(m_pprChanCalibs),
                                           pprChanCalib.channelId(),
                                           [](const L1CaloPprChanCalibV1& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          pprChanCalib);
}
