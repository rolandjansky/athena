/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprLutContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"

#include <string>
#include <iostream>

L1CaloPprLutContainer::L1CaloPprLutContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eLutStrategy,   std::string("LutStrategy"),   std::string("UInt16"));
	this->addSpecification(eLutOffset,     std::string("LutOffset"),     std::string("UInt16"));
	this->addSpecification(eLutSlope,      std::string("LutSlope"),      std::string("UInt16"));
	this->addSpecification(eLutNoiseCut,   std::string("LutNoiseCut"),   std::string("UInt16"));
	this->addSpecification(eLutParameters, std::string("LutParameters"), std::string("Blob64k"));
	this->addSpecification(ePedValue,      std::string("PedValue"),      std::string("UInt32"));
	this->addSpecification(ePedMean,       std::string("PedMean"),       std::string("Double"));
	this->addSpecification(eErrorCode,     std::string("ErrorCode"),     std::string("UInt32"));

	m_mCoolFoldersKeysMap[L1CaloPprLutContainer::ePprLutChanCalib] = std::string("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib");
}

L1CaloPprLutContainer::L1CaloPprLutContainer(const std::map<L1CaloPprLutContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eLutStrategy,   std::string("LutStrategy"),   std::string("UInt16"));
	this->addSpecification(eLutOffset,     std::string("LutOffset"),     std::string("UInt16"));
	this->addSpecification(eLutSlope,      std::string("LutSlope"),      std::string("UInt16"));
	this->addSpecification(eLutNoiseCut,   std::string("LutNoiseCut"),   std::string("UInt16"));
	this->addSpecification(eLutParameters, std::string("LutParameters"), std::string("Blob64k"));
	this->addSpecification(ePedValue,      std::string("PedValue"),      std::string("UInt32"));
	this->addSpecification(ePedMean,       std::string("PedMean"),       std::string("Double"));
	this->addSpecification(eErrorCode,     std::string("ErrorCode"),     std::string("UInt32"));
}

std::string L1CaloPprLutContainer::coolFolderKey(L1CaloPprLutContainer::eCoolFolders efolder) const {

	std::map<L1CaloPprLutContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPprLutContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPprLutContainer::ePprLutChanCalib));
	return v;
}

std::string L1CaloPprLutContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPprLutContainer::ePprLutChanCalib);
}

DataObject* L1CaloPprLutContainer::makePersistent() const {
	if(m_mPprLutMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;

	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	L1CaloPprLutMap::const_iterator pos = m_mPprLutMap.begin();
	for(;pos!=m_mPprLutMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloPprLut&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		attrList[ this->specificationName(eLutStrategy) ].setValue(p.lutStrategy());
		attrList[ this->specificationName(eLutOffset) ].setValue(p.lutOffset());
		attrList[ this->specificationName(eLutSlope) ].setValue(p.lutSlope());
		attrList[ this->specificationName(eLutNoiseCut) ].setValue(p.lutNoiseCut());
		attrList[ this->specificationName(eLutParameters) ].setValue(p.lutParameters());
		//attrList[ this->specificationName(ePedValue) ].setValue(p.pedValue());
		//attrList[ this->specificationName(eErrorCode) ].setValue(p.errorCode().errorCode());

		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
}

void L1CaloPprLutContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_map;

	it_map = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprLutContainer::ePprLutChanCalib));
	if(it_map!=condAttrListCollectionMap.end()) {

		CondAttrListCollection* attrListCollection = it_map->second;

		//loop over CondAttrListCollection
		CondAttrListCollection::const_iterator it = attrListCollection->begin();
		for(;it!=attrListCollection->end();++it) {

                        const coral::AttributeList& attrList = it->second;

			// Get value of each row for the current channel
			CondAttrListCollection::ChanNum chanNum(it->first);

			unsigned short lutStrategy = attrList[ this->specificationName(eLutStrategy) ].data<unsigned short>();
			unsigned short lutOffset = attrList[ this->specificationName(eLutOffset) ].data<unsigned short>();
			unsigned short lutSlope = attrList[ this->specificationName(eLutSlope) ].data<unsigned short>();
			unsigned short lutNoiseCut = attrList[ this->specificationName(eLutNoiseCut) ].data<unsigned short>();
			coral::Blob lutParameters = attrList[ this->specificationName(eLutParameters) ].data<coral::Blob>();
			unsigned int pedValue = attrList[ this->specificationName(ePedValue) ].data<unsigned int>();
			double pedMean = attrList[ this->specificationName(ePedMean) ].data<double>();
			ChanCalibErrorCode errorCode(attrList[ this->specificationName(eErrorCode) ].data<unsigned int>());

			L1CaloPprLut l1caloPptLut(chanNum, lutStrategy, lutOffset, lutSlope, lutNoiseCut, lutParameters, pedValue, pedMean, errorCode);
			m_mPprLutMap[chanNum] = l1caloPptLut;
		}
	} else {
		std::cout<<"L1CaloPprLutContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprLutContainer::ePprLutChanCalib) << std::endl;
	}
}

const L1CaloPprLut* L1CaloPprLutContainer::pprLut(unsigned int channelId) const {
	L1CaloPprLutMap::const_iterator pos = m_mPprLutMap.find(channelId);
	if(pos==m_mPprLutMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPprLut* L1CaloPprLutContainer::pprLut(const L1CaloCoolChannelId& channelId) const {
	return this->pprLut(channelId.id());
}

void L1CaloPprLutContainer::addPprLut(unsigned int channelId, const L1CaloPprLut& pprLut) {
	m_mPprLutMap[channelId] = pprLut;
}

void L1CaloPprLutContainer::addPprLut(const L1CaloCoolChannelId& channelId, const L1CaloPprLut& pprLut) {
	return this->addPprLut(channelId.id(), pprLut);
}

void L1CaloPprLutContainer::dump() const {
	L1CaloPprLutContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPprLutContainer::clear() {
	m_mPprLutMap.clear();
}
