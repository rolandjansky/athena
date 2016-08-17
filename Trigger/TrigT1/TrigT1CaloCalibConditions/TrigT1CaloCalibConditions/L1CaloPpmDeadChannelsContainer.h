// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELSCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannels.h"

class CondAttrListCollection;

/***
* Container of L1CaloPpmDeadChannels objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude Comment /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels
*/
class L1CaloPpmDeadChannelsContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eErrorCode, eNoiseCut };
public:
  L1CaloPpmDeadChannelsContainer();
  L1CaloPpmDeadChannelsContainer(const std::string& folderKey);
  virtual ~L1CaloPpmDeadChannelsContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_ppmDeadChannelss.clear(); }

  // getters
  const L1CaloPpmDeadChannels* ppmDeadChannels(unsigned int channelId) const;
  const L1CaloPpmDeadChannels* ppmDeadChannels(const L1CaloCoolChannelId& channelId) const {
    return ppmDeadChannels(channelId.id());
  }

  using const_iterator = std::vector<L1CaloPpmDeadChannels>::const_iterator;
  const_iterator begin() const { return m_ppmDeadChannelss.begin(); }
  const_iterator end() const { return m_ppmDeadChannelss.end(); }

  // setters
  void addPpmDeadChannels(const L1CaloPpmDeadChannels& ppmDeadChannels);

private:
  std::vector<L1CaloPpmDeadChannels> m_ppmDeadChannelss;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels";
};

CLASS_DEF( L1CaloPpmDeadChannelsContainer, 1134360424, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELSCONTAINER_H
