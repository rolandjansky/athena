// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1CONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1CONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloPprChanDefaultsV1;

/***
* Container of L1CaloPprChanDefaultsV1 objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --include CR12_BcidDecision1,CR12_SatOverride1,CR13_BcidDecision2,CR13_SatOverride2,CR14_BcidDecision3,CR14_SatOverride3,CR15_PeakFinderCond,CR15_DecisionSource /TRIGGER/L1Calo/V1/Configuration/PprChanDefaults
*/
class L1CaloPprChanDefaultsV1Container : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eBcidDecision1, eSatOverride1, eBcidDecision2, eSatOverride2, eBcidDecision3, eSatOverride3, ePeakFinderCond, eDecisionSource };
public:
  L1CaloPprChanDefaultsV1Container();
  L1CaloPprChanDefaultsV1Container(const std::string& folderKey);
  virtual ~L1CaloPprChanDefaultsV1Container() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanDefaultsV1s.clear(); }

  // getters
  const L1CaloPprChanDefaultsV1* pprChanDefaultsV1(unsigned int channelId) const;
  const L1CaloPprChanDefaultsV1* pprChanDefaultsV1(const L1CaloCoolChannelId& channelId) const {
    return pprChanDefaultsV1(channelId.id());
  }

  using const_iterator = std::vector<L1CaloPprChanDefaultsV1>::const_iterator;
  const_iterator begin() const;
  const_iterator end() const;

  // setters
  void addPprChanDefaultsV1(const L1CaloPprChanDefaultsV1& pprChanDefaultsV1);

private:
  std::vector<L1CaloPprChanDefaultsV1> m_pprChanDefaultsV1s;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults";
};

CLASS_DEF( L1CaloPprChanDefaultsV1Container, 1191077590, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1CONTAINER_H
