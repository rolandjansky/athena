// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINER_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloReadoutConfig;

/***
* Container of L1CaloReadoutConfig objects. Automatically created using:
*
* ../python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V2/Configuration/ReadoutConfig
*/
class L1CaloReadoutConfigContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { edescription, ebaselinePointer, enumFadcSlices, el1aFadcSlice, enumLutSlices, el1aLutSlice, enumProcSlices, el1aProcSlice, enumTopoSlices, el1aTopoSlice, elatencyPpmFadc, elatencyPpmLut, elatencyCpmInput, elatencyCpmHits, elatencyCpmRoi, elatencyJemInput, elatencyJemRoi, elatencyCpCmxBackplane, elatencyCpCmxLocal, elatencyCpCmxCable, elatencyCpCmxSystem, elatencyCpCmxInfo, elatencyJetCmxBackplane, elatencyJetCmxLocal, elatencyJetCmxCable, elatencyJetCmxSystem, elatencyJetCmxInfo, elatencyJetCmxRoi, elatencyEnergyCmxBackplane, elatencyEnergyCmxLocal, elatencyEnergyCmxCable, elatencyEnergyCmxSystem, elatencyEnergyCmxInfo, elatencyEnergyCmxRoi, elatencyTopo, einternalLatencyJemJet, einternalLatencyJemSum, ebcOffsetJemJet, ebcOffsetJemSum, ebcOffsetCmx, ebcOffsetTopo, eformatTypePpm, eformatTypeCpJep, eformatTypeTopo, ecompressionThresholdPpm, ecompressionThresholdCpJep, ecompressionThresholdTopo, ecompressionBaselinePpm, ereadout80ModePpm };
public:
  L1CaloReadoutConfigContainer();
  L1CaloReadoutConfigContainer(const std::string& folderKey);
  virtual ~L1CaloReadoutConfigContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_readoutConfigs.clear(); }

  // getters
  const L1CaloReadoutConfig* readoutConfig(unsigned int channelId) const;
  const L1CaloReadoutConfig* readoutConfig(const L1CaloCoolChannelId& channelId) const {
    return readoutConfig(channelId.id());
  }

  using const_iterator = std::vector<L1CaloReadoutConfig>::const_iterator;
  const_iterator begin() const { return m_readoutConfigs.begin(); }
  const_iterator end() const { return m_readoutConfigs.end(); }

  // setters
  void addReadoutConfig(const L1CaloReadoutConfig& readoutConfig);

private:
  std::vector<L1CaloReadoutConfig> m_readoutConfigs;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Configuration/ReadoutConfig";
};

CLASS_DEF( L1CaloReadoutConfigContainer, 1122647625, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOREADOUTCONFIGCONTAINER_H
