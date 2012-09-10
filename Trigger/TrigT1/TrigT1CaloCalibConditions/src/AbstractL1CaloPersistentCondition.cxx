/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"

#include "CoralBase/AttributeListSpecification.h"

#include <iostream>

DataObject* AbstractL1CaloPersistentCondition::makePersistent() const {
    std::cout << "ERROR - The makePersistent() method you are calling is not implemented by the current class !" << std::endl;
	return 0;
}

void AbstractL1CaloPersistentCondition::makeTransient(const std::map<std::string, CondAttrListCollection*> /* condAttrListCollectionMap */) {
    std::cout << "ERROR - The makeTransient(const map<std::string, CondAttrListCollection*>) method you are calling is not implemented by the current class !" << std::endl;
	return;
}

void AbstractL1CaloPersistentCondition::makeTransient(const std::map<std::string, AthenaAttributeList*> /* athenaAttributeList */) {
    std::cout << "ERROR - The makeTransient(const map<std::string, AthenaAttributeList*>) method you are calling is not implemented by the current class !" << std::endl;
	return;
}

void AbstractL1CaloPersistentCondition::addSpecification(int specId, const std::string& specName, const std::string& specType) {
	m_attrSpecificationNameMap[specId] = specName;
	m_attrSpecificationTypeMap[specId] = specType;
}

coral::AttributeListSpecification* AbstractL1CaloPersistentCondition::createAttributeListSpecification() const {

	coral::AttributeListSpecification* attrSpecification =  new coral::AttributeListSpecification();

	AttrSpecificationMap::const_iterator it_name = m_attrSpecificationNameMap.begin();
	AttrSpecificationMap::const_iterator it_type = m_attrSpecificationTypeMap.begin();

	for(;it_name!=m_attrSpecificationNameMap.end();++it_name,++it_type) {
		attrSpecification->extend(it_name->second, it_type->second);
	}
	return attrSpecification;
}

std::string AbstractL1CaloPersistentCondition::specificationName(int specId) const {
	return m_attrSpecificationNameMap.find(specId)->second;
}

std::string AbstractL1CaloPersistentCondition::specificationType(int specId) const {
	return m_attrSpecificationTypeMap.find(specId)->second;
}

void AbstractL1CaloPersistentCondition::clear() {
	std::cout << "ERROR - The clear() method you are calling is not implemented by the current class !" << std::endl;
	return;
}
