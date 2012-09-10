/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_ABSTRACTL1CALOPERSISTENTCONDITION_H
#define TRIGT1CALOCALIBCONDITIONS_ABSTRACTL1CALOPERSISTENTCONDITION_H

#include "TrigT1CaloCalibConditions/IL1CaloPersistenceCapable.h"

#include "GaudiKernel/DataObject.h"
#include "CoralBase/AttributeListSpecification.h"

#include <string>

/**
 *  AbstractL1CaloConditionContainer abstract base class
 *  for L1Calo persistent conditions container objects
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class AbstractL1CaloPersistentCondition : public IL1CaloPersistenceCapable {

  public:
  	virtual ~AbstractL1CaloPersistentCondition() {};

    virtual DataObject* makePersistent() const;

    virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);
    virtual void makeTransient(const std::map<std::string, AthenaAttributeList*> condAttributeList);

    std::string specificationName(int specId) const;
	std::string specificationType(int specId) const;
    void addSpecification(int specId, const std::string& specName, const std::string& specType);
    coral::AttributeListSpecification* createAttributeListSpecification() const;

    inline virtual std::string conditionType() const { return m_strConditionType; };

	virtual std::vector<std::string> coolInputKeys() const { return std::vector<std::string>(); };
	virtual std::string coolOutputKey() const { return std::string(""); };

    virtual void clear();

  protected:
    AbstractL1CaloPersistentCondition(std::string condType) { m_strConditionType = condType; };

  private:
  	typedef std::map<int, std::string> AttrSpecificationMap;
    AttrSpecificationMap m_attrSpecificationNameMap;
    AttrSpecificationMap m_attrSpecificationTypeMap;
    std::string m_strConditionType;

};
#endif
