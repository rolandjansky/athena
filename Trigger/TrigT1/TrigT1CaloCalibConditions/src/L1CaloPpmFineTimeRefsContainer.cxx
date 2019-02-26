/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefsContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>


L1CaloPpmFineTimeRefsContainer::L1CaloPpmFineTimeRefsContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eRefValue, std::string("referenceValue"), std::string("double"));
	this->addSpecification(eCriterion, std::string("criterionValue"), std::string("double"));
	this->addSpecification(eCalibValue, std::string("calibrationFactor"), std::string("double"));


	m_mCoolFoldersKeysMap[L1CaloPpmFineTimeRefsContainer::ePpmFineTimeRefss] = std::string("/TRIGGER/L1Calo/V1/References/FineTimeReferences");
}

L1CaloPpmFineTimeRefsContainer::L1CaloPpmFineTimeRefsContainer(const std::map<L1CaloPpmFineTimeRefsContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eRefValue, std::string("referenceValue"), std::string("double"));
	this->addSpecification(eCriterion, std::string("criterionValue"), std::string("double"));
	this->addSpecification(eCalibValue, std::string("calibrationFactor"), std::string("double"));
}

std::string L1CaloPpmFineTimeRefsContainer::coolFolderKey(L1CaloPpmFineTimeRefsContainer::eCoolFolders efolder) const {

	std::map<L1CaloPpmFineTimeRefsContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPpmFineTimeRefsContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPpmFineTimeRefsContainer::ePpmFineTimeRefss));
	return v;
}

std::string L1CaloPpmFineTimeRefsContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPpmFineTimeRefsContainer::ePpmFineTimeRefss);
}

DataObject* L1CaloPpmFineTimeRefsContainer::makePersistent() const {

	if(m_mPpmFineTimeRefsMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;

	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);
	
	L1CaloPpmFineTimeRefsMap::const_iterator pos = m_mPpmFineTimeRefsMap.begin();
	for(;pos!=m_mPpmFineTimeRefsMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloPpmFineTimeRefs&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		attrList[ this->specificationName(eErrorCode) ].setValue(p.errorCode().errorCode());
 		attrList[ this->specificationName(eRefValue) ].setValue(p.refValue());
 		attrList[ this->specificationName(eCriterion) ].setValue(p.criterion());
 		attrList[ this->specificationName(eCalibValue) ].setValue(p.calibValue());

		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
}

void L1CaloPpmFineTimeRefsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_map;

	it_map = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPpmFineTimeRefsContainer::ePpmFineTimeRefss));
	if(it_map!=condAttrListCollectionMap.end()) {

		CondAttrListCollection* attrListCollection = it_map->second;

		//loop over CondAttrListCollection
		CondAttrListCollection::const_iterator it = attrListCollection->begin();
		for(;it!=attrListCollection->end();++it) {

                        const coral::AttributeList& attrList = it->second;

			// Get value of each row for the current channel
			CondAttrListCollection::ChanNum chanNum = it->first;

			FineTimeErrorCode errorCode(attrList[ this->specificationName(eErrorCode) ].data<unsigned int>());
			double refVal = attrList[ this->specificationName(eRefValue) ].data<double>();
			double criterion = attrList[ this->specificationName(eCriterion) ].data<double>();
			double calibVal = attrList[ this->specificationName(eCalibValue) ].data<double>();

			L1CaloPpmFineTimeRefs l1caloFineTimeRefs(chanNum, errorCode, refVal, criterion, calibVal);
			m_mPpmFineTimeRefsMap[chanNum] = l1caloFineTimeRefs;
		}
	} else {
		std::cout<<"L1CaloPpmFineTimeRefsContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPpmFineTimeRefsContainer::ePpmFineTimeRefss) << std::endl;
	}
}

const L1CaloPpmFineTimeRefs* L1CaloPpmFineTimeRefsContainer::ppmFineTimeRefs(unsigned int channelId) const {
	L1CaloPpmFineTimeRefsMap::const_iterator pos = m_mPpmFineTimeRefsMap.find(channelId);
	if(pos==m_mPpmFineTimeRefsMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPpmFineTimeRefs* L1CaloPpmFineTimeRefsContainer::ppmFineTimeRefs(const L1CaloCoolChannelId& channelId) const {
	return this->ppmFineTimeRefs(channelId.id());
}

void L1CaloPpmFineTimeRefsContainer::addPpmFineTimeRefs(unsigned int channelId, const L1CaloPpmFineTimeRefs& fir) {
	m_mPpmFineTimeRefsMap[channelId] = fir;
}

void L1CaloPpmFineTimeRefsContainer::addPpmFineTimeRefs(const L1CaloCoolChannelId& channelId, const L1CaloPpmFineTimeRefs& fir) {
	this->addPpmFineTimeRefs(channelId.id(), fir);
}

void L1CaloPpmFineTimeRefsContainer::dump() const {
	L1CaloPpmFineTimeRefsContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPpmFineTimeRefsContainer::clear() {
	m_mPpmFineTimeRefsMap.clear();
}
