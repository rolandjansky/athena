/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxGainContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/MsgStream.h"

#include <string>

L1CaloRxGainContainer::L1CaloRxGainContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	//this->addSpecification(eIdentifier, std::string("Identifier"), std::string("unsigned int"));
	//this->addSpecification(eCrate, std::string("RecCrate"), std::string("string"));
	//this->addSpecification(eSlot, std::string("RecSlot"), std::string("unsigned int"));
	//this->addSpecification(eChannel, std::string("RecChannel"), std::string("unsigned int"));
	this->addSpecification(eGain, std::string("gains"), std::string("int"));
	this->addSpecification(eComment, std::string("Comment"), std::string("string"));

	m_mCoolFoldersKeysMap[L1CaloRxGainContainer::eRxGainFolder] = std::string("/receivers/Gains");
}

L1CaloRxGainContainer::L1CaloRxGainContainer(const std::map<L1CaloRxGainContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	//this->addSpecification(eIdentifier, std::string("Identifier"), std::string("unsigned int"));
	//this->addSpecification(eCrate, std::string("RecCrate"), std::string("string"));
	//this->addSpecification(eSlot, std::string("RecSlot"), std::string("unsigned int"));
	//this->addSpecification(eChannel, std::string("RecChannel"), std::string("unsigned int"));
	this->addSpecification(eGain, std::string("gains"), std::string("int"));
	this->addSpecification(eComment, std::string("Comment"), std::string("string"));
}

std::string L1CaloRxGainContainer::coolFolderKey(L1CaloRxGainContainer::eCoolFolders efolder) const {

	std::map<L1CaloRxGainContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloRxGainContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloRxGainContainer::eRxGainFolder));
	return v;
}

std::string L1CaloRxGainContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloRxGainContainer::eRxGainFolder);
}

DataObject* L1CaloRxGainContainer::makePersistent() const {
	if(m_mRxGainMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return 0;


	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	L1CaloRxGainMap::const_iterator pos = m_mRxGainMap.begin();
	for(;pos!=m_mRxGainMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloRxGain&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		//attrList[ this->specificationName(eIdentifier) ].setValue(p->identifier());
		//attrList[ this->specificationName(eCrate) ].setValue(p->crate());
		//attrList[ this->specificationName(eSlot) ].setValue(p->slot());
		//attrList[ this->specificationName(eChannel) ].setValue(p->channel());
		attrList[ this->specificationName(eGain) ].setValue(p.gain());
		attrList[ this->specificationName(eComment) ].setValue(p.comment());

		// add AttributeList corresponding to channel iChanNumber to the CondAttrListCollection
		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
}

void L1CaloRxGainContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_map;

	it_map = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloRxGainContainer::eRxGainFolder));
	if(it_map!=condAttrListCollectionMap.end()) {

		CondAttrListCollection* attrListCollection = it_map->second;

		//loop over CondAttrListCollection
		CondAttrListCollection::const_iterator it = attrListCollection->begin();
		for(;it!=attrListCollection->end();++it) {

                const coral::AttributeList& attrList = it->second;

		// Get value of each row for the current channel
		CondAttrListCollection::ChanNum chanNum = 	it->first;

		//unsigned int identifier = attrList[ this->specificationName(eIdentifier) ].data<unsigned int>();
		//std::string crate = attrList[ this->specificationName(eCrate) ].data<std::string>();
		//unsigned int slot = attrList[ this->specificationName(eSlot) ].data<unsigned int>();
		//unsigned int channel = attrList[ this->specificationName(eChannel) ].data<unsigned int>();
		unsigned int gain = attrList[ this->specificationName(eGain) ].data<int>();
		std::string comment = attrList[ this->specificationName(eComment) ].data<std::string>();

		// Create transient L1CaloPedestal obj
		//L1CaloRxGain* p = new L1CaloRxGain( chanNum, identifier, crate, slot, channel, gain, comment);
		L1CaloRxGain l1caloRxGain(chanNum, gain, comment);
		m_mRxGainMap[chanNum] = l1caloRxGain;
		}
	} else {
		std::cout<<"L1CaloRxGainContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloRxGainContainer::eRxGainFolder) << std::endl;
	}
}

const L1CaloRxGain* L1CaloRxGainContainer::rxGain(unsigned int channelId) const {
	L1CaloRxGainMap::const_iterator pos = m_mRxGainMap.find(channelId);
	if(pos==m_mRxGainMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloRxGain* L1CaloRxGainContainer::rxGain(const L1CaloRxCoolChannelId& channelId) const {
	return this->rxGain(channelId.id());
}

void L1CaloRxGainContainer::addRxGain(unsigned int channelId, const L1CaloRxGain& pprLut) {
	m_mRxGainMap[channelId] = pprLut;
}

void L1CaloRxGainContainer::addRxGain(const L1CaloRxCoolChannelId& channelId, const L1CaloRxGain& pprLut) {
	return this->addRxGain(channelId.id(), pprLut);
}

void L1CaloRxGainContainer::dump() const {
	L1CaloRxGainContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloRxGainContainer::clear() {
	m_mRxGainMap.clear();
}
