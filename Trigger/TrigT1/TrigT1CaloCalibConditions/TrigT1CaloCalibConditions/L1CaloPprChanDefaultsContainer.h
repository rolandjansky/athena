// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"

class CondAttrListCollection;

/***
* Container of L1CaloPprChanDefaults objects. Automatically created using:
*
* python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --include CR12_BcidDecision1,CR12_SatOverride1,CR13_BcidDecision2,CR13_SatOverride2,CR14_BcidDecision3,CR14_SatOverride3,CR15_PeakFinderCond,CR15_DecisionSource /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults
*/
class L1CaloPprChanDefaultsContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { eBcidDecision1, eSatOverride1, eBcidDecision2, eSatOverride2, eBcidDecision3, eSatOverride3, ePeakFinderCond, eDecisionSource, eBypassLutCp, eBypassLutJep, eInDataSample, eReadOut80Active, eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5, eSatHigh, eSatLow, eStartBit, eEnergyLevelHigh, eEnergyLevelLow, eSatLevel, eSaturationValue, eNumBcRaw, eNumBcBcid, ePedCorrEnable, eSat80High, eSat80Low };

public:
  L1CaloPprChanDefaultsContainer();
  L1CaloPprChanDefaultsContainer(const std::string& folderKey);
  virtual ~L1CaloPprChanDefaultsContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_pprChanDefaultss.clear(); }

  // getters
  const L1CaloPprChanDefaults* pprChanDefaults(unsigned int channelId) const;
  const L1CaloPprChanDefaults* pprChanDefaults(const L1CaloCoolChannelId& channelId) const {
    return pprChanDefaults(channelId.id());
  }

  using iterator = std::vector<L1CaloPprChanDefaults>::iterator;
  iterator begin() { return m_pprChanDefaultss.begin(); }
  iterator end() { return m_pprChanDefaultss.end(); }

  using const_iterator = std::vector<L1CaloPprChanDefaults>::const_iterator;
  const_iterator begin() const { return m_pprChanDefaultss.begin(); }
  const_iterator end() const { return m_pprChanDefaultss.end(); }

  // setters
  void addPprChanDefaults(const L1CaloPprChanDefaults& pprChanDefaults);

private:
  std::vector<L1CaloPprChanDefaults> m_pprChanDefaultss;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults";
};

CLASS_DEF( L1CaloPprChanDefaultsContainer, 1269446613, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSCONTAINER_H
