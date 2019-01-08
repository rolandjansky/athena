// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGYCONTAINER_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGYCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloPprChanStrategy;

/***
* Container of L1CaloPprChanStrategy objects. Automatically created using:
*
* ../athena/Trigger/TrigT1/TrigT1CaloCalibConditions/python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V2/Configuration/PprChanStrategy
*/
class L1CaloPprChanStrategyContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eStrategy, eCode, eTimingRegime, eDescription };
public:
  L1CaloPprChanStrategyContainer();
  L1CaloPprChanStrategyContainer(const std::string& folderKey);
  virtual ~L1CaloPprChanStrategyContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanStrategys.clear(); }

  // getters
  const L1CaloPprChanStrategy* pprChanStrategy(unsigned int channelId) const;
  const L1CaloPprChanStrategy* pprChanStrategy(const L1CaloCoolChannelId& channelId) const {
    return pprChanStrategy(channelId.id());
  }

  using const_iterator = std::vector<L1CaloPprChanStrategy>::const_iterator;
  const_iterator begin() const { return m_pprChanStrategys.begin(); }
  const_iterator end() const { return m_pprChanStrategys.end(); }

  // setters
  void addPprChanStrategy(const L1CaloPprChanStrategy& pprChanStrategy);

  void dump() const;

private:
  std::vector<L1CaloPprChanStrategy> m_pprChanStrategys;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Configuration/PprChanStrategy";
};

CLASS_DEF( L1CaloPprChanStrategyContainer, 1240855406, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGYCONTAINER_H
