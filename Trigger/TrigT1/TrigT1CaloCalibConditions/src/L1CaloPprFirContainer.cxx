/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprFirContainer.h"


#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>


L1CaloPprFirContainer::L1CaloPprFirContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eFirStartBit, std::string("FirStartBit"), std::string("unsigned short"));
	this->addSpecification(eFirCoeff1, std::string("FirCoeff1"), std::string("short"));
	this->addSpecification(eFirCoeff2, std::string("FirCoeff2"), std::string("short"));
	this->addSpecification(eFirCoeff3, std::string("FirCoeff3"), std::string("short"));
	this->addSpecification(eFirCoeff4, std::string("FirCoeff4"), std::string("short"));
	this->addSpecification(eFirCoeff5, std::string("FirCoeff5"), std::string("short"));

	m_mCoolFoldersKeysMap[L1CaloPprFirContainer::eFIRChanCalib] = std::string("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib");
}

L1CaloPprFirContainer::L1CaloPprFirContainer(const std::map<L1CaloPprFirContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eFirStartBit, std::string("FirStartBit"), std::string("unsigned short"));
	this->addSpecification(eFirCoeff1, std::string("FirCoeff1"), std::string("short"));
	this->addSpecification(eFirCoeff2, std::string("FirCoeff2"), std::string("short"));
	this->addSpecification(eFirCoeff3, std::string("FirCoeff3"), std::string("short"));
	this->addSpecification(eFirCoeff4, std::string("FirCoeff4"), std::string("short"));
	this->addSpecification(eFirCoeff5, std::string("FirCoeff5"), std::string("short"));
}

std::string L1CaloPprFirContainer::coolFolderKey(L1CaloPprFirContainer::eCoolFolders efolder) const {

	std::map<L1CaloPprFirContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPprFirContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPprFirContainer::eFIRChanCalib));
	return v;
}

std::string L1CaloPprFirContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPprFirContainer::eFIRChanCalib);
}

DataObject* L1CaloPprFirContainer::makePersistent() const {

	if(m_mFIRMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;

	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	L1CaloPprFirMap::const_iterator pos = m_mFIRMap.begin();
	for(;pos!=m_mFIRMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloPprFir&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		const std::vector<int>& v = p.firCoefficients();
		unsigned int firSB(p.firStartBit());
		short firCoeff1 = short(v.at(0));
		short firCoeff2 = short(v.at(1));
		short firCoeff3 = short(v.at(2));
		short firCoeff4 = short(v.at(3));
		short firCoeff5 = short(v.at(4));

		attrList[ this->specificationName(eFirCoeff1) ].setValue( firCoeff1 );
		attrList[ this->specificationName(eFirCoeff2) ].setValue( firCoeff2 );
		attrList[ this->specificationName(eFirCoeff3) ].setValue( firCoeff3 );
		attrList[ this->specificationName(eFirCoeff4) ].setValue( firCoeff4 );
		attrList[ this->specificationName(eFirCoeff5) ].setValue( firCoeff5 );
		attrList[ this->specificationName(eFirStartBit) ].setValue((unsigned short)firSB);

		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
}

void L1CaloPprFirContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_map;

	it_map = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprFirContainer::eFIRChanCalib));
	if(it_map!=condAttrListCollectionMap.end()) {

		CondAttrListCollection* attrListCollection = it_map->second;

		//loop over CondAttrListCollection
		CondAttrListCollection::const_iterator it = attrListCollection->begin();
		for(;it!=attrListCollection->end();++it) {

			AthenaAttributeList attrList = it->second;

			// Get value of each row for the current channel
			CondAttrListCollection::ChanNum chanNum = it->first;

			unsigned int firStartBit = (unsigned int) attrList[ this->specificationName(eFirStartBit) ].data<unsigned short>();
			int firCoeff1 = (int) attrList[ this->specificationName(eFirCoeff1) ].data<short>();
			int firCoeff2 = (int) attrList[ this->specificationName(eFirCoeff2) ].data<short>();
			int firCoeff3 = (int) attrList[ this->specificationName(eFirCoeff3) ].data<short>();
			int firCoeff4 = (int) attrList[ this->specificationName(eFirCoeff4) ].data<short>();
			int firCoeff5 = (int) attrList[ this->specificationName(eFirCoeff5) ].data<short>();
			
			L1CaloPprFir l1caloFir(chanNum, firStartBit, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5);
			m_mFIRMap[chanNum] = l1caloFir;
		}
	} else {
		std::cout<<"L1CaloPprFirContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprFirContainer::eFIRChanCalib) << std::endl;
	}
}

bool L1CaloPprFirContainer::check4Bits(unsigned char value) const {
	if ( (value>>4)!=0 ) return false;
	else return true;
}

bool L1CaloPprFirContainer::checkRange4Bits2Complement(int value) const {
	if ( (value<-8) || (value>7) ) return false;
	else return true;
}

bool L1CaloPprFirContainer::checkRangeUnsignedInt(int value) const {
	if ( (value<0) || (value>15) ) return false;
	else return true;
}

int L1CaloPprFirContainer::uChar4BitsToSignedInt(unsigned char value) const {
	if(value>7) {
		return (int) (value - 16);
	}
	return (int) value;
}

unsigned char L1CaloPprFirContainer::intToUChar4Bits2Complement(int value) const {
	if(value<0) {
		return (unsigned char) (value + 16);
	} else
	return (unsigned char) value;
}


const L1CaloPprFir* L1CaloPprFirContainer::fir(unsigned int channelId) const {
	L1CaloPprFirMap::const_iterator pos = m_mFIRMap.find(channelId);
	if(pos==m_mFIRMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPprFir* L1CaloPprFirContainer::fir(const L1CaloCoolChannelId& channelId) const {
	return this->fir(channelId.id());
}

void L1CaloPprFirContainer::addFir(unsigned int channelId, const L1CaloPprFir& fir) {
	m_mFIRMap[channelId] = fir;
}

void L1CaloPprFirContainer::addFir(const L1CaloCoolChannelId& channelId, const L1CaloPprFir& fir) {
	this->addFir(channelId.id(), fir);
}

void L1CaloPprFirContainer::dump() const {
	L1CaloPprFirContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPprFirContainer::clear() {
	m_mFIRMap.clear();
}
