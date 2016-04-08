// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERSCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowers.h"

class CondAttrListCollection;

/***
* Container of L1CaloDisabledTowers objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V1/Conditions/DisabledTowers
*/
class L1CaloDisabledTowersContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { edisabledBits };
public:
  L1CaloDisabledTowersContainer();
  L1CaloDisabledTowersContainer(const std::string& folderKey);
  virtual ~L1CaloDisabledTowersContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_disabledTowerss.clear(); }

  // getters
  const L1CaloDisabledTowers* disabledTowers(unsigned int channelId) const;
  const L1CaloDisabledTowers* disabledTowers(const L1CaloCoolChannelId& channelId) const {
    return disabledTowers(channelId.id());
  }

  using const_iterator = std::vector<L1CaloDisabledTowers>::const_iterator;
  const_iterator begin() const { return m_disabledTowerss.begin(); }
  const_iterator end() const { return m_disabledTowerss.end(); }

  // setters
  void addDisabledTowers(const L1CaloDisabledTowers& disabledTowers);

private:
  std::vector<L1CaloDisabledTowers> m_disabledTowerss;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Conditions/DisabledTowers";
};

CLASS_DEF( L1CaloDisabledTowersContainer, 1100693700, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERSCONTAINER_H
