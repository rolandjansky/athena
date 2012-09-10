/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>


L1CaloPpmDeadChannelContainer::L1CaloPpmDeadChannelContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eComment, std::string("Comment"), std::string("String255"));

	m_mCoolFoldersKeysMap[L1CaloPpmDeadChannelContainer::ePpmDeadChannels] = std::string("/TRIGGER/L1Calo/Calibration/PpmDeadChannels");
}

L1CaloPpmDeadChannelContainer::L1CaloPpmDeadChannelContainer(const std::map<L1CaloPpmDeadChannelContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eComment, std::string("Comment"), std::string("String255"));
}

std::string L1CaloPpmDeadChannelContainer::coolFolderKey(L1CaloPpmDeadChannelContainer::eCoolFolders efolder) const {

	std::map<L1CaloPpmDeadChannelContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPpmDeadChannelContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPpmDeadChannelContainer::ePpmDeadChannels));
	return v;
}

std::string L1CaloPpmDeadChannelContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPpmDeadChannelContainer::ePpmDeadChannels);
}

DataObject* L1CaloPpmDeadChannelContainer::makePersistent() const {

	if(m_mPpmDeadChannelMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;

	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	L1CaloPpmDeadChannelMap::const_iterator pos = m_mPpmDeadChannelMap.begin();
	for(;pos!=m_mPpmDeadChannelMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloPpmDeadChannel&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		attrList[ this->specificationName(eErrorCode) ].setValue(p.errorCode().errorCode());
		attrList[ this->specificationName(eComment) ].setValue(p.comment());

		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
}

void L1CaloPpmDeadChannelContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_map;

	it_map = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPpmDeadChannelContainer::ePpmDeadChannels));
	if(it_map!=condAttrListCollectionMap.end()) {

		CondAttrListCollection* attrListCollection = it_map->second;

		//loop over CondAttrListCollection
		CondAttrListCollection::const_iterator it = attrListCollection->begin();
		for(;it!=attrListCollection->end();++it) {

			AthenaAttributeList attrList = it->second;

			// Get value of each row for the current channel
			CondAttrListCollection::ChanNum chanNum = it->first;

			ChanDeadErrorCode errorCode(attrList[ this->specificationName(eErrorCode) ].data<unsigned int>());
			std::string comment = attrList[ this->specificationName(eComment) ].data<std::string>();


			L1CaloPpmDeadChannel l1caloDeadChannel(chanNum, errorCode, comment);
			m_mPpmDeadChannelMap[chanNum] = l1caloDeadChannel;
		}
	} else {
		std::cout<<"L1CaloPpmDeadChannelContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPpmDeadChannelContainer::ePpmDeadChannels) << std::endl;
	}
}

const L1CaloPpmDeadChannel* L1CaloPpmDeadChannelContainer::ppmDeadChannel(unsigned int channelId) const {
	L1CaloPpmDeadChannelMap::const_iterator pos = m_mPpmDeadChannelMap.find(channelId);
	if(pos==m_mPpmDeadChannelMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPpmDeadChannel* L1CaloPpmDeadChannelContainer::ppmDeadChannel(const L1CaloCoolChannelId& channelId) const {
	return this->ppmDeadChannel(channelId.id());
}

void L1CaloPpmDeadChannelContainer::addPpmDeadChannel(unsigned int channelId, const L1CaloPpmDeadChannel& fir) {
	m_mPpmDeadChannelMap[channelId] = fir;
}

void L1CaloPpmDeadChannelContainer::addPpmDeadChannel(const L1CaloCoolChannelId& channelId, const L1CaloPpmDeadChannel& fir) {
	this->addPpmDeadChannel(channelId.id(), fir);
}

void L1CaloPpmDeadChannelContainer::dump() const {
	L1CaloPpmDeadChannelContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPpmDeadChannelContainer::clear() {
	m_mPpmDeadChannelMap.clear();
}
