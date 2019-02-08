// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARSCONTAINER_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARSCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloDerivedRunPars;

/***
* Container of L1CaloDerivedRunPars objects. Automatically created using:
*
* ../python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V1/Conditions/DerivedRunPars
*/
class L1CaloDerivedRunParsContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { etimingRegime, etierZeroTag };
public:
  L1CaloDerivedRunParsContainer();
  L1CaloDerivedRunParsContainer(const std::string& folderKey);
  virtual ~L1CaloDerivedRunParsContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_derivedRunParss.clear(); }

  // getters
  const L1CaloDerivedRunPars* derivedRunPars(unsigned int channelId) const;
  const L1CaloDerivedRunPars* derivedRunPars(const L1CaloCoolChannelId& channelId) const {
    return derivedRunPars(channelId.id());
  }

  using const_iterator = std::vector<L1CaloDerivedRunPars>::const_iterator;
  const_iterator begin() const { return m_derivedRunParss.begin(); }
  const_iterator end() const { return m_derivedRunParss.end(); }

  // setters
  void addDerivedRunPars(const L1CaloDerivedRunPars& derivedRunPars);

  void dump() const;

private:
  std::vector<L1CaloDerivedRunPars> m_derivedRunParss;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars";
};

CLASS_DEF( L1CaloDerivedRunParsContainer, 1081812192, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARSCONTAINER_H
