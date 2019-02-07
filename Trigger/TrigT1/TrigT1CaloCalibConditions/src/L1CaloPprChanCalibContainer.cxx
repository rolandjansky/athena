/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalib.h"

L1CaloPprChanCalibContainer::L1CaloPprChanCalibContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(eErrorCode, "ErrorCode", "unsigned int");
  this->addSpecification(ePedMean, "PedMean", "double");
  this->addSpecification(ePedValue, "PedValue", "unsigned int");
  this->addSpecification(ePedFirSum, "PedFirSum", "unsigned int");
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
  this->addSpecification(eLutCpStrategy, "LutCpStrategy", "unsigned short");
  this->addSpecification(eLutCpOffset, "LutCpOffset", "unsigned short");
  this->addSpecification(eLutCpNoiseCut, "LutCpNoiseCut", "unsigned short");
  this->addSpecification(eLutCpSlope, "LutCpSlope", "unsigned short");
  this->addSpecification(eLutCpPar1, "LutCpPar1", "short");
  this->addSpecification(eLutCpPar2, "LutCpPar2", "short");
  this->addSpecification(eLutCpPar3, "LutCpPar3", "short");
  this->addSpecification(eLutCpPar4, "LutCpPar4", "short");
  this->addSpecification(eLutCpScale, "LutCpScale", "unsigned short");
  this->addSpecification(eLutJepStrategy, "LutJepStrategy", "unsigned short");
  this->addSpecification(eLutJepOffset, "LutJepOffset", "unsigned short");
  this->addSpecification(eLutJepNoiseCut, "LutJepNoiseCut", "unsigned short");
  this->addSpecification(eLutJepSlope, "LutJepSlope", "unsigned short");
  this->addSpecification(eLutJepPar1, "LutJepPar1", "short");
  this->addSpecification(eLutJepPar2, "LutJepPar2", "short");
  this->addSpecification(eLutJepPar3, "LutJepPar3", "short");
  this->addSpecification(eLutJepPar4, "LutJepPar4", "short");
  this->addSpecification(eLutJepScale, "LutJepScale", "unsigned short");
}

L1CaloPprChanCalibContainer::L1CaloPprChanCalibContainer(const std::string& folderKey)
  : L1CaloPprChanCalibContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloPprChanCalibContainer::makePersistent() const
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
    attrList[specificationName(ePedFirSum)].setValue(item.pedFirSum());
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
    attrList[specificationName(eLutCpStrategy)].setValue(item.lutCpStrategy());
    attrList[specificationName(eLutCpOffset)].setValue(item.lutCpOffset());
    attrList[specificationName(eLutCpNoiseCut)].setValue(item.lutCpNoiseCut());
    attrList[specificationName(eLutCpSlope)].setValue(item.lutCpSlope());
    attrList[specificationName(eLutCpPar1)].setValue(item.lutCpPar1());
    attrList[specificationName(eLutCpPar2)].setValue(item.lutCpPar2());
    attrList[specificationName(eLutCpPar3)].setValue(item.lutCpPar3());
    attrList[specificationName(eLutCpPar4)].setValue(item.lutCpPar4());
    attrList[specificationName(eLutCpScale)].setValue(item.lutCpScale());
    attrList[specificationName(eLutJepStrategy)].setValue(item.lutJepStrategy());
    attrList[specificationName(eLutJepOffset)].setValue(item.lutJepOffset());
    attrList[specificationName(eLutJepNoiseCut)].setValue(item.lutJepNoiseCut());
    attrList[specificationName(eLutJepSlope)].setValue(item.lutJepSlope());
    attrList[specificationName(eLutJepPar1)].setValue(item.lutJepPar1());
    attrList[specificationName(eLutJepPar2)].setValue(item.lutJepPar2());
    attrList[specificationName(eLutJepPar3)].setValue(item.lutJepPar3());
    attrList[specificationName(eLutJepPar4)].setValue(item.lutJepPar4());
    attrList[specificationName(eLutJepScale)].setValue(item.lutJepScale());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloPprChanCalibContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();
  
  // In the case of overlay, we need multiple instances of L1CaloPprChanCalibContainer
  // Take the last element in the map
  if (condAttrListCollectionMap.empty()) return;
  auto it = condAttrListCollectionMap.rbegin();

  auto attrListCollection = it->second;

  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto errorCode = attrList[specificationName(eErrorCode)].data<unsigned int>();
    auto pedMean = attrList[specificationName(ePedMean)].data<double>();
    auto pedValue = attrList[specificationName(ePedValue)].data<unsigned int>();
    auto pedFirSum = attrList[specificationName(ePedFirSum)].data<unsigned int>();
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
    auto lutCpStrategy = attrList[specificationName(eLutCpStrategy)].data<unsigned short>();
    auto lutCpOffset = attrList[specificationName(eLutCpOffset)].data<unsigned short>();
    auto lutCpNoiseCut = attrList[specificationName(eLutCpNoiseCut)].data<unsigned short>();
    auto lutCpSlope = attrList[specificationName(eLutCpSlope)].data<unsigned short>();
    auto lutCpPar1 = attrList[specificationName(eLutCpPar1)].data<short>();
    auto lutCpPar2 = attrList[specificationName(eLutCpPar2)].data<short>();
    auto lutCpPar3 = attrList[specificationName(eLutCpPar3)].data<short>();
    auto lutCpPar4 = attrList[specificationName(eLutCpPar4)].data<short>();
    auto lutCpScale = attrList[specificationName(eLutCpScale)].data<unsigned short>();
    auto lutJepStrategy = attrList[specificationName(eLutJepStrategy)].data<unsigned short>();
    auto lutJepOffset = attrList[specificationName(eLutJepOffset)].data<unsigned short>();
    auto lutJepNoiseCut = attrList[specificationName(eLutJepNoiseCut)].data<unsigned short>();
    auto lutJepSlope = attrList[specificationName(eLutJepSlope)].data<unsigned short>();
    auto lutJepPar1 = attrList[specificationName(eLutJepPar1)].data<short>();
    auto lutJepPar2 = attrList[specificationName(eLutJepPar2)].data<short>();
    auto lutJepPar3 = attrList[specificationName(eLutJepPar3)].data<short>();
    auto lutJepPar4 = attrList[specificationName(eLutJepPar4)].data<short>();
    auto lutJepScale = attrList[specificationName(eLutJepScale)].data<unsigned short>();

    addPprChanCalib(L1CaloPprChanCalib(chanNum, errorCode, pedMean, pedValue, pedFirSum, extBcidThreshold, satBcidThreshLow, satBcidThreshHigh, satBcidLevel, bcidEnergyRangeLow, bcidEnergyRangeHigh, firStartBit, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5, lutCpStrategy, lutCpOffset, lutCpNoiseCut, lutCpSlope, lutCpPar1, lutCpPar2, lutCpPar3, lutCpPar4, lutCpScale, lutJepStrategy, lutJepOffset, lutJepNoiseCut, lutJepSlope, lutJepPar1, lutJepPar2, lutJepPar3, lutJepPar4, lutJepScale));
  }
  
//   }
}

const L1CaloPprChanCalib* L1CaloPprChanCalibContainer::pprChanCalib(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_pprChanCalibs),
                            std::end(m_pprChanCalibs),
                            channelId,
                            [](const L1CaloPprChanCalib& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_pprChanCalibs)) return nullptr;
  return &(*it);
}

void L1CaloPprChanCalibContainer::addPprChanCalib(const L1CaloPprChanCalib& pprChanCalib)
{
  // insert into the correct position mainting the sorted vector
  m_pprChanCalibs.insert(std::lower_bound(std::begin(m_pprChanCalibs),
                                           std::end(m_pprChanCalibs),
                                           pprChanCalib.channelId(),
                                           [](const L1CaloPprChanCalib& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          pprChanCalib);
}
