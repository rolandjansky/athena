/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainerJSON.h"

#include <algorithm>
#include <memory>

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigJSON.h"
#include "nlohmann/json.hpp"
#include "CoralBase/Blob.h"



L1CaloReadoutConfigContainerJSON::L1CaloReadoutConfigContainerJSON()
  : AbstractL1CaloPersistentCondition("CondAttrListCollection") 
{
  this->addSpecification(ebaselinePointer, "baselinePointer", "unsigned int");
  this->addSpecification(enumFadcSlices, "numFadcSlices", "unsigned int");
  this->addSpecification(el1aFadcSlice, "l1aFadcSlice", "unsigned int");
  this->addSpecification(enumLutSlices, "numLutSlices", "unsigned int");
  this->addSpecification(el1aLutSlice, "l1aLutSlice", "unsigned int");
  this->addSpecification(enumProcSlices, "numProcSlices", "unsigned int");
  this->addSpecification(el1aProcSlice, "l1aProcSlice", "unsigned int");
  this->addSpecification(enumTopoSlices, "numTopoSlices", "unsigned int");
  this->addSpecification(el1aTopoSlice, "l1aTopoSlice", "unsigned int");
  this->addSpecification(enumFexSlices, "numFexSlices", "unsigned int");
  this->addSpecification(el1aFexSlice, "l1aFexSlice", "unsigned int");
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
  this->addSpecification(einputReadoutModeFex, "inputReadoutModeFex", "string");
  this->addSpecification(ereadoutOffsetEfex, "readoutOffsetEfex", "unsigned int");
  this->addSpecification(ereadoutOffsetGfex, "readoutOffsetGfex", "unsigned int");
  this->addSpecification(ereadoutOffsetJfex, "readoutOffsetJfex", "unsigned int");
  this->addSpecification(ereadoutOffsetPh1Topo, "readoutOffsetPh1Topo", "unsigned int");


}



DataObject* L1CaloReadoutConfigContainerJSON::makePersistent() const
{

  auto* attrSpecification = this->createAttributeListSpecification();
  if(!attrSpecification || !attrSpecification->size()) return nullptr;

  auto attrListCollection = std::make_unique<CondAttrListCollection>(true);
  for(const auto& item : m_readoutConfigsJSON) {
    AthenaAttributeList attrList(*attrSpecification);
    attrList[specificationName(ebaselinePointer)].setValue(item.baselinePointer());
    attrList[specificationName(enumFadcSlices)].setValue(item.numFadcSlices());
    attrList[specificationName(el1aFadcSlice)].setValue(item.l1aFadcSlice());
    attrList[specificationName(enumLutSlices)].setValue(item.numLutSlices());
    attrList[specificationName(el1aLutSlice)].setValue(item.l1aLutSlice());
    attrList[specificationName(enumProcSlices)].setValue(item.numProcSlices());
    attrList[specificationName(el1aProcSlice)].setValue(item.l1aProcSlice());
    attrList[specificationName(enumTopoSlices)].setValue(item.numTopoSlices());
    attrList[specificationName(el1aTopoSlice)].setValue(item.l1aTopoSlice());
    attrList[specificationName(enumFexSlices)].setValue(item.numFexSlices());
    attrList[specificationName(el1aFexSlice)].setValue(item.l1aFexSlice());
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
    attrList[specificationName(einputReadoutModeFex)].setValue(item.inputReadoutModeFex());
    attrList[specificationName(ereadoutOffsetEfex)].setValue(item.readoutOffsetEfex());
    attrList[specificationName(ereadoutOffsetGfex)].setValue(item.readoutOffsetGfex());
    attrList[specificationName(ereadoutOffsetJfex)].setValue(item.readoutOffsetJfex());
    attrList[specificationName(ereadoutOffsetPh1Topo)].setValue(item.readoutOffsetPh1Topo());

    attrListCollection->add(item.channelId(), attrList);
  }
  return static_cast<DataObject*>(attrListCollection.release());
}

void L1CaloReadoutConfigContainerJSON::makeTransient(const std::map<std::string, const CondAttrListCollection*>& condAttrListCollectionMap)
{
  clear();

  
  if (condAttrListCollectionMap.empty()) return;  
  auto it = condAttrListCollectionMap.rbegin(); 
  
    

  auto attrListCollection = it->second;
  for(const auto& [chanNum, attrListAux] : *attrListCollection) {
    
    const coral::Blob& blob=attrListAux["json"].data<coral::Blob>();
    std::string s((char*)blob.startingAddress(),blob.size());       
    nlohmann::json attrList = nlohmann::json::parse(s);


    try {

      auto baselinePointer = attrList[specificationName(ebaselinePointer)];
      auto numFadcSlices = attrList[specificationName(enumFadcSlices)];
      auto l1aFadcSlice = attrList[specificationName(el1aFadcSlice)];
      auto numLutSlices = attrList[specificationName(enumLutSlices)];
      auto l1aLutSlice = attrList[specificationName(el1aLutSlice)];
      auto numProcSlices = attrList[specificationName(enumProcSlices)];
      auto l1aProcSlice = attrList[specificationName(el1aProcSlice)];
      auto numTopoSlices = attrList[specificationName(enumTopoSlices)];
      auto l1aTopoSlice = attrList[specificationName(el1aTopoSlice)];
      auto numFexSlices =  attrList[specificationName(enumFexSlices)];
      auto l1aFexSlice = attrList[specificationName(el1aFexSlice)];
      auto latencyPpmFadc = attrList[specificationName(elatencyPpmFadc)];
      auto latencyPpmLut = attrList[specificationName(elatencyPpmLut)];
      auto latencyCpmInput = attrList[specificationName(elatencyCpmInput)];
      auto latencyCpmHits = attrList[specificationName(elatencyCpmHits)];
      auto latencyCpmRoi = attrList[specificationName(elatencyCpmRoi)];
      auto latencyJemInput = attrList[specificationName(elatencyJemInput)];
      auto latencyJemRoi = attrList[specificationName(elatencyJemRoi)];
      auto latencyCpCmxBackplane = attrList[specificationName(elatencyCpCmxBackplane)];
      auto latencyCpCmxLocal = attrList[specificationName(elatencyCpCmxLocal)];
      auto latencyCpCmxCable = attrList[specificationName(elatencyCpCmxCable)];
      auto latencyCpCmxSystem = attrList[specificationName(elatencyCpCmxSystem)];
      auto latencyCpCmxInfo = attrList[specificationName(elatencyCpCmxInfo)];
      auto latencyJetCmxBackplane = attrList[specificationName(elatencyJetCmxBackplane)];
      auto latencyJetCmxLocal = attrList[specificationName(elatencyJetCmxLocal)];
      auto latencyJetCmxCable = attrList[specificationName(elatencyJetCmxCable)];
      auto latencyJetCmxSystem = attrList[specificationName(elatencyJetCmxSystem)];
      auto latencyJetCmxInfo = attrList[specificationName(elatencyJetCmxInfo)];
      auto latencyJetCmxRoi = attrList[specificationName(elatencyJetCmxRoi)];
      auto latencyEnergyCmxBackplane = attrList[specificationName(elatencyEnergyCmxBackplane)];
      auto latencyEnergyCmxLocal = attrList[specificationName(elatencyEnergyCmxLocal)];
      auto latencyEnergyCmxCable = attrList[specificationName(elatencyEnergyCmxCable)];
      auto latencyEnergyCmxSystem = attrList[specificationName(elatencyEnergyCmxSystem)];
      auto latencyEnergyCmxInfo = attrList[specificationName(elatencyEnergyCmxInfo)];
      auto latencyEnergyCmxRoi = attrList[specificationName(elatencyEnergyCmxRoi)];
      auto latencyTopo = attrList[specificationName(elatencyTopo)];
      auto internalLatencyJemJet = attrList[specificationName(einternalLatencyJemJet)];
      auto internalLatencyJemSum = attrList[specificationName(einternalLatencyJemSum)];
      auto bcOffsetJemJet = attrList[specificationName(ebcOffsetJemJet)];
      auto bcOffsetJemSum = attrList[specificationName(ebcOffsetJemSum)];
      auto bcOffsetCmx = attrList[specificationName(ebcOffsetCmx)];
      auto bcOffsetTopo = attrList[specificationName(ebcOffsetTopo)];
      auto formatTypePpm = attrList[specificationName(eformatTypePpm)];
      auto formatTypeCpJep = attrList[specificationName(eformatTypeCpJep)];
      auto formatTypeTopo = attrList[specificationName(eformatTypeTopo)];
      auto compressionThresholdPpm = attrList[specificationName(ecompressionThresholdPpm)];
      auto compressionThresholdCpJep = attrList[specificationName(ecompressionThresholdCpJep)];
      auto compressionThresholdTopo = attrList[specificationName(ecompressionThresholdTopo)];
      auto compressionBaselinePpm = attrList[specificationName(ecompressionBaselinePpm)];
      auto readout80ModePpm = attrList[specificationName(ereadout80ModePpm)];
      auto inputReadoutModeFex  = attrList[specificationName(einputReadoutModeFex)];
      auto readoutOffsetEfex = attrList[specificationName(ereadoutOffsetEfex)];
      auto readoutOffsetGfex = attrList[specificationName(ereadoutOffsetGfex)];
      auto readoutOffsetJfex = attrList[specificationName(ereadoutOffsetJfex)];
      auto readoutOffsetPh1Topo  = attrList[specificationName(ereadoutOffsetPh1Topo)];


      addReadoutConfigJSON(L1CaloReadoutConfigJSON(chanNum, baselinePointer, numFadcSlices, l1aFadcSlice, numLutSlices, l1aLutSlice, numProcSlices, l1aProcSlice, numTopoSlices, l1aTopoSlice, numFexSlices, l1aFexSlice, latencyPpmFadc, latencyPpmLut, latencyCpmInput, latencyCpmHits, latencyCpmRoi, latencyJemInput, latencyJemRoi, latencyCpCmxBackplane, latencyCpCmxLocal, latencyCpCmxCable, latencyCpCmxSystem, latencyCpCmxInfo, latencyJetCmxBackplane, latencyJetCmxLocal, latencyJetCmxCable, latencyJetCmxSystem, latencyJetCmxInfo, latencyJetCmxRoi, latencyEnergyCmxBackplane, latencyEnergyCmxLocal, latencyEnergyCmxCable, latencyEnergyCmxSystem, latencyEnergyCmxInfo, latencyEnergyCmxRoi, latencyTopo, internalLatencyJemJet, internalLatencyJemSum, bcOffsetJemJet, bcOffsetJemSum, bcOffsetCmx, bcOffsetTopo, formatTypePpm, formatTypeCpJep, formatTypeTopo, compressionThresholdPpm, compressionThresholdCpJep, compressionThresholdTopo, compressionBaselinePpm, readout80ModePpm, inputReadoutModeFex,  readoutOffsetEfex, readoutOffsetGfex, readoutOffsetJfex, readoutOffsetPh1Topo));
    } 
    catch(const std::exception& e) {     std::cout << "ERROR - Caught exception during creation of AthenaAttributeList object from ReadoutConfigJSON " << e.what() <<std::endl; }

   
  }}

const L1CaloReadoutConfigJSON* L1CaloReadoutConfigContainerJSON::readoutConfigJSON(unsigned int channelId) const
{
  auto it = std::lower_bound(std::begin(m_readoutConfigsJSON),
                            std::end(m_readoutConfigsJSON),
                            channelId,
                            [](const L1CaloReadoutConfigJSON& el, unsigned int val) -> bool {
                              return el.channelId() < val;
                            });
  if(it == std::end(m_readoutConfigsJSON)) return nullptr;
  return &(*it);
}

void L1CaloReadoutConfigContainerJSON::addReadoutConfigJSON(const L1CaloReadoutConfigJSON& readoutConfigJSON)
{
  // insert into the correct position mainting the sorted vector
  m_readoutConfigsJSON.insert(std::lower_bound(std::begin(m_readoutConfigsJSON),
                                           std::end(m_readoutConfigsJSON),
                                           readoutConfigJSON.channelId(),
                                           [](const L1CaloReadoutConfigJSON& el, unsigned int va) -> bool {
                                             return el.channelId() < va;
                                           }),
                          readoutConfigJSON);
}
