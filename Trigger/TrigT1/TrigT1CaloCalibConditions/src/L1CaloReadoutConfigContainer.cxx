/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainer.h"

#include <algorithm>
#include <memory>

#include "CxxUtils/make_unique.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloReadoutConfig.h"

L1CaloReadoutConfigContainer::L1CaloReadoutConfigContainer()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(edescription, "description", "string");
  this->addSpecification(ebaselinePointer, "baselinePointer", "unsigned int");
  this->addSpecification(enumFadcSlices, "numFadcSlices", "unsigned int");
  this->addSpecification(el1aFadcSlice, "l1aFadcSlice", "unsigned int");
  this->addSpecification(enumLutSlices, "numLutSlices", "unsigned int");
  this->addSpecification(el1aLutSlice, "l1aLutSlice", "unsigned int");
  this->addSpecification(enumProcSlices, "numProcSlices", "unsigned int");
  this->addSpecification(el1aProcSlice, "l1aProcSlice", "unsigned int");
  this->addSpecification(enumTopoSlices, "numTopoSlices", "unsigned int");
  this->addSpecification(el1aTopoSlice, "l1aTopoSlice", "unsigned int");
  this->addSpecification(elatencyPpmFadc, "latencyPpmFadc", "unsigned int");
  this->addSpecification(elatencyPpmLut, "latencyPpmLut", "unsigned int");
  this->addSpecification(elatencyCpmInput, "latencyCpmInput", "unsigned int");
  this->addSpecification(elatencyCpmHits, "latencyCpmHits", "unsigned int");
  this->addSpecification(elatencyCpmRoi, "latencyCpmRoi", "unsigned int");
  this->addSpecification(elatencyJemInput, "latencyJemInput", "unsigned int");
  this->addSpecification(elatencyJemRoi, "latencyJemRoi", "unsigned int");
  this->addSpecification(elatencyCpCmxBackplane, "latencyCpCmxBackplane", "unsigned int");
  this->addSpecification(elatencyCpCmxLocal, "latencyCpCmxLocal", "unsigned int");
  this->addSpecification(elatencyCpCmxCable, "latencyCpCmxCable", "unsigned int");
  this->addSpecification(elatencyCpCmxSystem, "latencyCpCmxSystem", "unsigned int");
  this->addSpecification(elatencyCpCmxInfo, "latencyCpCmxInfo", "unsigned int");
  this->addSpecification(elatencyJetCmxBackplane, "latencyJetCmxBackplane", "unsigned int");
  this->addSpecification(elatencyJetCmxLocal, "latencyJetCmxLocal", "unsigned int");
  this->addSpecification(elatencyJetCmxCable, "latencyJetCmxCable", "unsigned int");
  this->addSpecification(elatencyJetCmxSystem, "latencyJetCmxSystem", "unsigned int");
  this->addSpecification(elatencyJetCmxInfo, "latencyJetCmxInfo", "unsigned int");
  this->addSpecification(elatencyJetCmxRoi, "latencyJetCmxRoi", "unsigned int");
  this->addSpecification(elatencyEnergyCmxBackplane, "latencyEnergyCmxBackplane", "unsigned int");
  this->addSpecification(elatencyEnergyCmxLocal, "latencyEnergyCmxLocal", "unsigned int");
  this->addSpecification(elatencyEnergyCmxCable, "latencyEnergyCmxCable", "unsigned int");
  this->addSpecification(elatencyEnergyCmxSystem, "latencyEnergyCmxSystem", "unsigned int");
  this->addSpecification(elatencyEnergyCmxInfo, "latencyEnergyCmxInfo", "unsigned int");
  this->addSpecification(elatencyEnergyCmxRoi, "latencyEnergyCmxRoi", "unsigned int");
  this->addSpecification(elatencyTopo, "latencyTopo", "unsigned int");
  this->addSpecification(einternalLatencyJemJet, "internalLatencyJemJet", "unsigned int");
  this->addSpecification(einternalLatencyJemSum, "internalLatencyJemSum", "unsigned int");
  this->addSpecification(ebcOffsetJemJet, "bcOffsetJemJet", "unsigned int");
  this->addSpecification(ebcOffsetJemSum, "bcOffsetJemSum", "unsigned int");
  this->addSpecification(ebcOffsetCmx, "bcOffsetCmx", "int");
  this->addSpecification(ebcOffsetTopo, "bcOffsetTopo", "int");
  this->addSpecification(eformatTypePpm, "formatTypePpm", "string");
  this->addSpecification(eformatTypeCpJep, "formatTypeCpJep", "string");
  this->addSpecification(eformatTypeTopo, "formatTypeTopo", "string");
  this->addSpecification(ecompressionThresholdPpm, "compressionThresholdPpm", "unsigned int");
  this->addSpecification(ecompressionThresholdCpJep, "compressionThresholdCpJep", "unsigned int");
  this->addSpecification(ecompressionThresholdTopo, "compressionThresholdTopo", "unsigned int");
  this->addSpecification(ecompressionBaselinePpm, "compressionBaselinePpm", "unsigned int");
  this->addSpecification(ereadout80ModePpm, "readout80ModePpm", "unsigned int");
}

L1CaloReadoutConfigContainer::L1CaloReadoutConfigContainer(const std::string& folderKey)
  : L1CaloReadoutConfigContainer() // delegating constructor
{
  m_coolFolderKey = folderKey;
}


DataObject* L1CaloReadoutConfigContainer::makePersistent() const
{
  using CxxUtils::make_unique;

  if(m_coolFolderKey.empty()) return nullptr;

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_readoutConfigs) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(edescription)].setValue(item.description());
    attrList[specificationName(ebaselinePointer)].setValue(item.baselinePointer());
    attrList[specificationName(enumFadcSlices)].setValue(item.numFadcSlices());
    attrList[specificationName(el1aFadcSlice)].setValue(item.l1aFadcSlice());
    attrList[specificationName(enumLutSlices)].setValue(item.numLutSlices());
    attrList[specificationName(el1aLutSlice)].setValue(item.l1aLutSlice());
    attrList[specificationName(enumProcSlices)].setValue(item.numProcSlices());
    attrList[specificationName(el1aProcSlice)].setValue(item.l1aProcSlice());
    attrList[specificationName(enumTopoSlices)].setValue(item.numTopoSlices());
    attrList[specificationName(el1aTopoSlice)].setValue(item.l1aTopoSlice());
    attrList[specificationName(elatencyPpmFadc)].setValue(item.latencyPpmFadc());
    attrList[specificationName(elatencyPpmLut)].setValue(item.latencyPpmLut());
    attrList[specificationName(elatencyCpmInput)].setValue(item.latencyCpmInput());
    attrList[specificationName(elatencyCpmHits)].setValue(item.latencyCpmHits());
    attrList[specificationName(elatencyCpmRoi)].setValue(item.latencyCpmRoi());
    attrList[specificationName(elatencyJemInput)].setValue(item.latencyJemInput());
    attrList[specificationName(elatencyJemRoi)].setValue(item.latencyJemRoi());
    attrList[specificationName(elatencyCpCmxBackplane)].setValue(item.latencyCpCmxBackplane());
    attrList[specificationName(elatencyCpCmxLocal)].setValue(item.latencyCpCmxLocal());
    attrList[specificationName(elatencyCpCmxCable)].setValue(item.latencyCpCmxCable());
    attrList[specificationName(elatencyCpCmxSystem)].setValue(item.latencyCpCmxSystem());
    attrList[specificationName(elatencyCpCmxInfo)].setValue(item.latencyCpCmxInfo());
    attrList[specificationName(elatencyJetCmxBackplane)].setValue(item.latencyJetCmxBackplane());
    attrList[specificationName(elatencyJetCmxLocal)].setValue(item.latencyJetCmxLocal());
    attrList[specificationName(elatencyJetCmxCable)].setValue(item.latencyJetCmxCable());
    attrList[specificationName(elatencyJetCmxSystem)].setValue(item.latencyJetCmxSystem());
    attrList[specificationName(elatencyJetCmxInfo)].setValue(item.latencyJetCmxInfo());
    attrList[specificationName(elatencyJetCmxRoi)].setValue(item.latencyJetCmxRoi());
    attrList[specificationName(elatencyEnergyCmxBackplane)].setValue(item.latencyEnergyCmxBackplane());
    attrList[specificationName(elatencyEnergyCmxLocal)].setValue(item.latencyEnergyCmxLocal());
    attrList[specificationName(elatencyEnergyCmxCable)].setValue(item.latencyEnergyCmxCable());
    attrList[specificationName(elatencyEnergyCmxSystem)].setValue(item.latencyEnergyCmxSystem());
    attrList[specificationName(elatencyEnergyCmxInfo)].setValue(item.latencyEnergyCmxInfo());
    attrList[specificationName(elatencyEnergyCmxRoi)].setValue(item.latencyEnergyCmxRoi());
    attrList[specificationName(elatencyTopo)].setValue(item.latencyTopo());
    attrList[specificationName(einternalLatencyJemJet)].setValue(item.internalLatencyJemJet());
    attrList[specificationName(einternalLatencyJemSum)].setValue(item.internalLatencyJemSum());
    attrList[specificationName(ebcOffsetJemJet)].setValue(item.bcOffsetJemJet());
    attrList[specificationName(ebcOffsetJemSum)].setValue(item.bcOffsetJemSum());
    attrList[specificationName(ebcOffsetCmx)].setValue(item.bcOffsetCmx());
    attrList[specificationName(ebcOffsetTopo)].setValue(item.bcOffsetTopo());
    attrList[specificationName(eformatTypePpm)].setValue(item.formatTypePpm());
    attrList[specificationName(eformatTypeCpJep)].setValue(item.formatTypeCpJep());
    attrList[specificationName(eformatTypeTopo)].setValue(item.formatTypeTopo());
    attrList[specificationName(ecompressionThresholdPpm)].setValue(item.compressionThresholdPpm());
    attrList[specificationName(ecompressionThresholdCpJep)].setValue(item.compressionThresholdCpJep());
    attrList[specificationName(ecompressionThresholdTopo)].setValue(item.compressionThresholdTopo());
    attrList[specificationName(ecompressionBaselinePpm)].setValue(item.compressionBaselinePpm());
    attrList[specificationName(ereadout80ModePpm)].setValue(item.readout80ModePpm());
    
    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloReadoutConfigContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap)
{
  clear();

  auto it = condAttrListCollectionMap.find(m_coolFolderKey);
  if(it == std::end(condAttrListCollectionMap)) return;

  auto attrListCollection = it->second;
  for(const auto& item : *attrListCollection) {
    auto chanNum = item.first;
    const auto& attrList = item.second;
    
    auto description = attrList[specificationName(edescription)].data<std::string>();
    auto baselinePointer = attrList[specificationName(ebaselinePointer)].data<unsigned int>();
    auto numFadcSlices = attrList[specificationName(enumFadcSlices)].data<unsigned int>();
    auto l1aFadcSlice = attrList[specificationName(el1aFadcSlice)].data<unsigned int>();
    auto numLutSlices = attrList[specificationName(enumLutSlices)].data<unsigned int>();
    auto l1aLutSlice = attrList[specificationName(el1aLutSlice)].data<unsigned int>();
    auto numProcSlices = attrList[specificationName(enumProcSlices)].data<unsigned int>();
    auto l1aProcSlice = attrList[specificationName(el1aProcSlice)].data<unsigned int>();
    auto numTopoSlices = attrList[specificationName(enumTopoSlices)].data<unsigned int>();
    auto l1aTopoSlice = attrList[specificationName(el1aTopoSlice)].data<unsigned int>();
    auto latencyPpmFadc = attrList[specificationName(elatencyPpmFadc)].data<unsigned int>();
    auto latencyPpmLut = attrList[specificationName(elatencyPpmLut)].data<unsigned int>();
    auto latencyCpmInput = attrList[specificationName(elatencyCpmInput)].data<unsigned int>();
    auto latencyCpmHits = attrList[specificationName(elatencyCpmHits)].data<unsigned int>();
    auto latencyCpmRoi = attrList[specificationName(elatencyCpmRoi)].data<unsigned int>();
    auto latencyJemInput = attrList[specificationName(elatencyJemInput)].data<unsigned int>();
    auto latencyJemRoi = attrList[specificationName(elatencyJemRoi)].data<unsigned int>();
    auto latencyCpCmxBackplane = attrList[specificationName(elatencyCpCmxBackplane)].data<unsigned int>();
    auto latencyCpCmxLocal = attrList[specificationName(elatencyCpCmxLocal)].data<unsigned int>();
    auto latencyCpCmxCable = attrList[specificationName(elatencyCpCmxCable)].data<unsigned int>();
    auto latencyCpCmxSystem = attrList[specificationName(elatencyCpCmxSystem)].data<unsigned int>();
    auto latencyCpCmxInfo = attrList[specificationName(elatencyCpCmxInfo)].data<unsigned int>();
    auto latencyJetCmxBackplane = attrList[specificationName(elatencyJetCmxBackplane)].data<unsigned int>();
    auto latencyJetCmxLocal = attrList[specificationName(elatencyJetCmxLocal)].data<unsigned int>();
    auto latencyJetCmxCable = attrList[specificationName(elatencyJetCmxCable)].data<unsigned int>();
    auto latencyJetCmxSystem = attrList[specificationName(elatencyJetCmxSystem)].data<unsigned int>();
    auto latencyJetCmxInfo = attrList[specificationName(elatencyJetCmxInfo)].data<unsigned int>();
    auto latencyJetCmxRoi = attrList[specificationName(elatencyJetCmxRoi)].data<unsigned int>();
    auto latencyEnergyCmxBackplane = attrList[specificationName(elatencyEnergyCmxBackplane)].data<unsigned int>();
    auto latencyEnergyCmxLocal = attrList[specificationName(elatencyEnergyCmxLocal)].data<unsigned int>();
    auto latencyEnergyCmxCable = attrList[specificationName(elatencyEnergyCmxCable)].data<unsigned int>();
    auto latencyEnergyCmxSystem = attrList[specificationName(elatencyEnergyCmxSystem)].data<unsigned int>();
    auto latencyEnergyCmxInfo = attrList[specificationName(elatencyEnergyCmxInfo)].data<unsigned int>();
    auto latencyEnergyCmxRoi = attrList[specificationName(elatencyEnergyCmxRoi)].data<unsigned int>();
    auto latencyTopo = attrList[specificationName(elatencyTopo)].data<unsigned int>();
    auto internalLatencyJemJet = attrList[specificationName(einternalLatencyJemJet)].data<unsigned int>();
    auto internalLatencyJemSum = attrList[specificationName(einternalLatencyJemSum)].data<unsigned int>();
    auto bcOffsetJemJet = attrList[specificationName(ebcOffsetJemJet)].data<unsigned int>();
    auto bcOffsetJemSum = attrList[specificationName(ebcOffsetJemSum)].data<unsigned int>();
    auto bcOffsetCmx = attrList[specificationName(ebcOffsetCmx)].data<int>();
    auto bcOffsetTopo = attrList[specificationName(ebcOffsetTopo)].data<int>();
    auto formatTypePpm = attrList[specificationName(eformatTypePpm)].data<std::string>();
    auto formatTypeCpJep = attrList[specificationName(eformatTypeCpJep)].data<std::string>();
    auto formatTypeTopo = attrList[specificationName(eformatTypeTopo)].data<std::string>();
    auto compressionThresholdPpm = attrList[specificationName(ecompressionThresholdPpm)].data<unsigned int>();
    auto compressionThresholdCpJep = attrList[specificationName(ecompressionThresholdCpJep)].data<unsigned int>();
    auto compressionThresholdTopo = attrList[specificationName(ecompressionThresholdTopo)].data<unsigned int>();
    auto compressionBaselinePpm = attrList[specificationName(ecompressionBaselinePpm)].data<unsigned int>();
    auto readout80ModePpm = attrList[specificationName(ereadout80ModePpm)].data<unsigned int>();

    addReadoutConfig(L1CaloReadoutConfig(chanNum, description, baselinePointer, numFadcSlices, l1aFadcSlice, numLutSlices, l1aLutSlice, numProcSlices, l1aProcSlice, numTopoSlices, l1aTopoSlice, latencyPpmFadc, latencyPpmLut, latencyCpmInput, latencyCpmHits, latencyCpmRoi, latencyJemInput, latencyJemRoi, latencyCpCmxBackplane, latencyCpCmxLocal, latencyCpCmxCable, latencyCpCmxSystem, latencyCpCmxInfo, latencyJetCmxBackplane, latencyJetCmxLocal, latencyJetCmxCable, latencyJetCmxSystem, latencyJetCmxInfo, latencyJetCmxRoi, latencyEnergyCmxBackplane, latencyEnergyCmxLocal, latencyEnergyCmxCable, latencyEnergyCmxSystem, latencyEnergyCmxInfo, latencyEnergyCmxRoi, latencyTopo, internalLatencyJemJet, internalLatencyJemSum, bcOffsetJemJet, bcOffsetJemSum, bcOffsetCmx, bcOffsetTopo, formatTypePpm, formatTypeCpJep, formatTypeTopo, compressionThresholdPpm, compressionThresholdCpJep, compressionThresholdTopo, compressionBaselinePpm, readout80ModePpm));
  }
}

const L1CaloReadoutConfig* L1CaloReadoutConfigContainer::readoutConfig(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_readoutConfigs),
                            std::end(m_readoutConfigs),
                            channelId,
                            [](const L1CaloReadoutConfig& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_readoutConfigs)) return nullptr;
  return &(*it);
}

void L1CaloReadoutConfigContainer::addReadoutConfig(const L1CaloReadoutConfig& readoutConfig)
{
  // insert into the correct position mainting the sorted vector
  m_readoutConfigs.insert(std::lower_bound(std::begin(m_readoutConfigs),
                                           std::end(m_readoutConfigs),
                                           readoutConfig.channelId(),
                                           [](const L1CaloReadoutConfig& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          readoutConfig);
}
