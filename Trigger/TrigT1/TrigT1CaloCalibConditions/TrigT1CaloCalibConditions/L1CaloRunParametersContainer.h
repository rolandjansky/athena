// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERSCONTAINER_H

#include <map>
#include <string>
#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

class CondAttrListCollection;
class L1CaloRunParameters;

/***
* Container of L1CaloRunParameters objects. Automatically created using:
*
* ./CreateClassesForFolder.py /TRIGGER/L1Calo/V1/Conditions/RunParameters
*/
class L1CaloRunParametersContainer : public DataObject, virtual public AbstractL1CaloPersistentCondition
{
private:
  enum eAttrSpecification { erunType, erunActionName, erunActionVersion, ereadoutConfig, ereadoutConfigID, ettcConfiguration, ettcConfigurationID, etriggerMenu, ecalibration, econditions };
public:
  L1CaloRunParametersContainer();
  L1CaloRunParametersContainer(const std::string& folderKey);
  virtual ~L1CaloRunParametersContainer() {}

  // interface of AbstractL1CaloPersistentCondition
  using AbstractL1CaloPersistentCondition::makeTransient;
  virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>);
  virtual DataObject* makePersistent() const;
  virtual std::vector<std::string> coolInputKeys() const { return {m_coolFolderKey}; }
  virtual std::string coolOutputKey() const { return m_coolFolderKey; }
  virtual void clear() { m_runParameterss.clear(); }

  // getters
  const L1CaloRunParameters* runParameters(unsigned int channelId) const;
  const L1CaloRunParameters* runParameters(const L1CaloCoolChannelId& channelId) const {
    return runParameters(channelId.id());
  }

  using const_iterator = std::vector<L1CaloRunParameters>::const_iterator;
  const_iterator begin() const { return m_runParameterss.begin(); }
  const_iterator end() const { return m_runParameterss.end(); }

  // setters
  void addRunParameters(const L1CaloRunParameters& runParameters);

  void dump() const;
  
private:
  std::vector<L1CaloRunParameters> m_runParameterss;
  std::string m_coolFolderKey = "/TRIGGER/L1Calo/V1/Conditions/RunParameters";
};

CLASS_DEF( L1CaloRunParametersContainer, 1236303918, 1 )

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERSCONTAINER_H
