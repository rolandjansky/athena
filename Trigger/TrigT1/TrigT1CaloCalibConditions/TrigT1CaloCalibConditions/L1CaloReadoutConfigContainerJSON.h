// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINERJSON_H
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINERJSON_H

#include <map>
#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigJSON.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class CondAttrListCollection;
class L1CaloReadoutConfigJSON;

/***
* Container of L1CaloReadoutConfig objects. Automatically created using:
*
* ../python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V2/Configuration/ReadoutConfig
*/
class L1CaloReadoutConfigContainerJSON : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { ebaselinePointer, enumFadcSlices, el1aFadcSlice, enumLutSlices, el1aLutSlice, enumProcSlices, el1aProcSlice, enumTopoSlices, el1aTopoSlice, enumFexSlices, el1aFexSlice, elatencyPpmFadc, elatencyPpmLut, elatencyCpmInput, elatencyCpmHits, elatencyCpmRoi, elatencyJemInput, elatencyJemRoi, elatencyCpCmxBackplane, elatencyCpCmxLocal, elatencyCpCmxCable, elatencyCpCmxSystem, elatencyCpCmxInfo, elatencyJetCmxBackplane, elatencyJetCmxLocal, elatencyJetCmxCable, elatencyJetCmxSystem, elatencyJetCmxInfo, elatencyJetCmxRoi, elatencyEnergyCmxBackplane, elatencyEnergyCmxLocal, elatencyEnergyCmxCable, elatencyEnergyCmxSystem, elatencyEnergyCmxInfo, elatencyEnergyCmxRoi, elatencyTopo, einternalLatencyJemJet, einternalLatencyJemSum, ebcOffsetJemJet, ebcOffsetJemSum, ebcOffsetCmx, ebcOffsetTopo, eformatTypePpm, eformatTypeCpJep, eformatTypeTopo, ecompressionThresholdPpm, ecompressionThresholdCpJep, ecompressionThresholdTopo, ecompressionBaselinePpm, ereadout80ModePpm, einputReadoutModeFex, ereadoutOffsetEfex, ereadoutOffsetGfex, ereadoutOffsetJfex, ereadoutOffsetPh1Topo};
public:
  L1CaloReadoutConfigContainerJSON();
  L1CaloReadoutConfigContainerJSON(const std::string& folderKey);
  virtual ~L1CaloReadoutConfigContainerJSON() = default;

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, const CondAttrListCollection*>&) override;
  virtual DataObject* makePersistent() const override;
  virtual void clear() override { m_readoutConfigsJSON.clear(); }

  // getters
  const L1CaloReadoutConfigJSON* readoutConfigJSON(unsigned int channelId) const;
  const L1CaloReadoutConfigJSON* readoutConfigJSON(const L1CaloCoolChannelId& channelId) const {
    return readoutConfigJSON(channelId.id());
  }

  using const_iterator = std::vector<L1CaloReadoutConfigJSON>::const_iterator;
  const_iterator begin() const { return m_readoutConfigsJSON.begin(); }
  const_iterator end() const { return m_readoutConfigsJSON.end(); }

  // setters
  void addReadoutConfigJSON(const L1CaloReadoutConfigJSON& readoutConfigJSON);

private:
  std::vector<L1CaloReadoutConfigJSON> m_readoutConfigsJSON;
};

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(L1CaloReadoutConfigContainerJSON,  1337315229);

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINERJSON_H
