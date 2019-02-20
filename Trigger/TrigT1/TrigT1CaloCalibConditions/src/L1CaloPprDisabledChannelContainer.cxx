/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"

#include <string>
#include <iostream>


L1CaloPprDisabledChannelContainer::L1CaloPprDisabledChannelContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eChanDeadErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eChanCalibErrorCode, std::string("ErrorCode"), std::string("UInt32"));

	// noiseCut from PpmDeadChannels db
	this->addSpecification(eNoiseCut, std::string("NoiseCut"), std::string("UInt16"));

	// disabledBits from DisabledTowers db
	this->addSpecification(eDisabledBits, std::string("disabledBits"), std::string("UInt32"));

	//OKS stuff
	this->addSpecification(eCaloDisableMask, std::string("caloDisableMask"), std::string("Int64"));
	this->addSpecification(eEtaDisableMask, std::string("etaDisableMask"), std::string("Int64"));
	this->addSpecification(ePhiDisableMask, std::string("phiDisableMask"), std::string("Int64"));
	this->addSpecification(eChanDisableMask, std::string("chanDisableMask"), std::string("Int64"));

	m_mCoolFoldersKeysMap[L1CaloPprDisabledChannelContainer::ePpmDeadChannels] = std::string("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels");
	m_mCoolFoldersKeysMap[L1CaloPprDisabledChannelContainer::ePprChanCalib] = std::string("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib");
	m_mCoolFoldersKeysMap[L1CaloPprDisabledChannelContainer::eDisabledTowers] = std::string("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers");
	//m_mCoolFoldersKeysMap[L1CaloPprDisabledChannelContainer::eOKS2COOL] = std::string("/TECHRUN/L1CaloDisabledRegions");
}

L1CaloPprDisabledChannelContainer::L1CaloPprDisabledChannelContainer(const std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eChanDeadErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eChanCalibErrorCode, std::string("ErrorCode"), std::string("UInt32"));
	this->addSpecification(eNoiseCut, std::string("NoiseCut"), std::string("UInt16"));
	this->addSpecification(eDisabledBits, std::string("disabledBits"), std::string("UInt32"));

	//OKS stuff
	this->addSpecification(eCaloDisableMask, std::string("caloDisableMask"), std::string("Int64"));
	this->addSpecification(eEtaDisableMask, std::string("etaDisableMask"), std::string("Int64"));
	this->addSpecification(ePhiDisableMask, std::string("phiDisableMask"), std::string("Int64"));
	this->addSpecification(eChanDisableMask, std::string("chanDisableMask"), std::string("Int64"));
}

std::string L1CaloPprDisabledChannelContainer::coolFolderKey(L1CaloPprDisabledChannelContainer::eCoolFolders efolder) const {

	std::map<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPprDisabledChannelContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePpmDeadChannels));
	v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePprChanCalib));
	v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainer::eDisabledTowers));
	//v.push_back(this->coolFolderKey(L1CaloPprDisabledChannelContainer::eOKS2COOL));
	return v;
}

std::string L1CaloPprDisabledChannelContainer::coolOutputKey() const {
	//return this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePpmDeadChannels);
	return std::string("");
}

DataObject* L1CaloPprDisabledChannelContainer::makePersistent() const {
	return 0;
/*
	if(m_mPprDisabledChannelMap.size()==0) return 0;

	// Create AttributeListSpecification according to the attributes to be recorded
	// this is required to create the AttributeList objs
	coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
	if(!attrSpecification->size()) return NULL;

	CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

	L1CaloPprDisabledChannelMap::const_iterator pos = m_mPprDisabledChannelMap.begin();
	for(;pos!=m_mPprDisabledChannelMap.end();++pos) {
		const unsigned int channelId = pos->first;
		const L1CaloPprDisabledChannel&  p = pos->second;

		AthenaAttributeList attrList(*attrSpecification);

		// Set value of each row for the current channel
		attrList[ this->specificationName(eChanDeadErrorCode) ].setValue(p.deadErrorCode().errorCode());
		attrList[ this->specificationName(eChanCalibErrorCode) ].setValue(p.calibErrorCode.errorCode());

		attrListCollection->add(channelId, attrList);
	}

	return (DataObject*) attrListCollection;
*/
}

void L1CaloPprDisabledChannelContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

//	std::cout<< "*** L1CaloPprDisabledChannelContainer::makeTransient() ***"<<std::endl;
//	std::cout<< "***"<<condAttrListCollectionMap.size()<<"***"<<std::endl;

	this->clear();

	std::map<std::string, CondAttrListCollection*>::const_iterator it_deadChannels = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePpmDeadChannels));
	if(it_deadChannels==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprDisabledChannelContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePpmDeadChannels) << std::endl;
		return;
	}
	CondAttrListCollection* deadChannelsAttrListCollection = it_deadChannels->second;

	std::map<std::string, CondAttrListCollection*>::const_iterator it_calibChannels = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePprChanCalib));
	if(it_calibChannels==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprDisabledChannelContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprDisabledChannelContainer::ePprChanCalib) << std::endl;
		return;
	}
	CondAttrListCollection* calibChannelsAttrListCollection = it_calibChannels->second;

	std::map<std::string, CondAttrListCollection*>::const_iterator it_disabledTowers = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprDisabledChannelContainer::eDisabledTowers));
	if(it_disabledTowers==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprDisabledChannelContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprDisabledChannelContainer::eDisabledTowers) << std::endl;
		return;
	}
	CondAttrListCollection* disabledTowersAttrListCollection = it_disabledTowers->second;
/*
	std::map<std::string, CondAttrListCollection*>::const_iterator it_oksChannels = condAttrListCollectionMap.find(this->coolFolderKey(L1CaloPprDisabledChannelContainer::eOKS2COOL));
	if(it_oksChannels==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprDisabledChannelContainer : Could not find requested CondAttrListCollection "<< this->coolFolderKey(L1CaloPprDisabledChannelContainer::eOKS2COOL) << std::endl;
		return;
	}
	CondAttrListCollection* oksChannelsAttrListCollection = it_oksChannels->second;
*/

//	std::cout<< "*** Filling dead ***"<<std::endl;

	CondAttrListCollection::const_iterator it_attr;
	CondAttrListCollection::const_iterator it_attrE;

	//Dead channel
	it_attr  = deadChannelsAttrListCollection->begin();
	it_attrE = deadChannelsAttrListCollection->end();
	for(;it_attr!=it_attrE;++it_attr) {

                const coral::AttributeList& attrList(it_attr->second);

		// Get value of each row for the current channel
		CondAttrListCollection::ChanNum chanNum(it_attr->first);

		ChanDeadErrorCode errorCode(attrList[ this->specificationName(eChanDeadErrorCode) ].data<unsigned int>());

		unsigned int noiseCut = attrList[this->specificationName(eNoiseCut)].data<unsigned int>();

		// test if the channel is really bad for the given IOV
		if(!errorCode.chanValid() || noiseCut > 0) {
			L1CaloPprDisabledChannelMap::iterator it_chan =  m_mPprDisabledChannelMap.find(chanNum);
			if(it_chan==m_mPprDisabledChannelMap.end()) {
				m_mPprDisabledChannelMap[chanNum] = L1CaloPprDisabledChannel(chanNum);
			}
			m_mPprDisabledChannelMap[chanNum].deadErrorCode(errorCode);
			m_mPprDisabledChannelMap[chanNum].setNoiseCut(noiseCut);
		}
	}

//std::cout<< "*** Filling calib ***"<<std::endl;

	// calib channels
	it_attr  = calibChannelsAttrListCollection->begin();
	it_attrE = calibChannelsAttrListCollection->end();
	for(;it_attr!=it_attrE;++it_attr) {

                const coral::AttributeList& attrList(it_attr->second);

		// Get value of each row for the current channel
		CondAttrListCollection::ChanNum chanNum(it_attr->first);

		ChanCalibErrorCode errorCode(attrList[ this->specificationName(eChanCalibErrorCode) ].data<unsigned int>());

		// test if the channel is correctly calibrated for the given IOV
		if(!errorCode.chanValid()) {
			L1CaloPprDisabledChannelMap::iterator it_chan =  m_mPprDisabledChannelMap.find(chanNum);
			if(it_chan==m_mPprDisabledChannelMap.end()) {
				m_mPprDisabledChannelMap[chanNum] = L1CaloPprDisabledChannel(chanNum);
			}
			m_mPprDisabledChannelMap[chanNum].calibErrorCode(errorCode);
		}
	}

	// disabled towers
	it_attr  = disabledTowersAttrListCollection->begin();
	it_attrE = disabledTowersAttrListCollection->end();
	for(;it_attr!=it_attrE;++it_attr) {

                const coral::AttributeList& attrList(it_attr->second);

		// Get value of each row for the current channel
		CondAttrListCollection::ChanNum chanNum(it_attr->first);

		unsigned int disabledBits = attrList[this->specificationName(eDisabledBits)].data<unsigned int>();

		if(disabledBits) {
			L1CaloPprDisabledChannelMap::iterator it_chan =  m_mPprDisabledChannelMap.find(chanNum);
			if(it_chan==m_mPprDisabledChannelMap.end()) {
				m_mPprDisabledChannelMap[chanNum] = L1CaloPprDisabledChannel(chanNum);
			}
			m_mPprDisabledChannelMap[chanNum].setDisabledBits(disabledBits);
		}
	}

//std::cout<< "*** Filling oks ***"<<std::endl;
/*
	// oks channels
	// use harcoded value or just take the first and only one channelId ?
	unsigned int defaultChannel = 2844830117;
	AthenaAttributeList Oks2CoolAttrList(oksChannelsAttrListCollection->attributeList(defaultChannel));

	long int caloDisableMask(Oks2CoolAttrList[ this->specificationName(eCaloDisableMask) ].data<long int>());
	long int etaDisableMask(Oks2CoolAttrList[ this->specificationName(eEtaDisableMask) ].data<long int>());
	long int phiDisableMask(Oks2CoolAttrList[ this->specificationName(ePhiDisableMask) ].data<long int>());
	long int chanDisableMask(Oks2CoolAttrList[ this->specificationName(eChanDisableMask) ].data<long int>());

	std::cout<< caloDisableMask<<" "<<etaDisableMask<<" "<<phiDisableMask<<" "<< chanDisableMask<<std::endl;

	//compute here coolchannelId from previous variables
	//CondAttrListCollection::ChanNum chanNum( myComputedId )
	CondAttrListCollection::ChanNum chanNum( 0 );

	L1CaloPprDisabledChannelMap::iterator it_chan = m_mPprDisabledChannelMap.find(chanNum);
	if(it_chan==m_mPprDisabledChannelMap.end()) {
		m_mPprDisabledChannelMap[chanNum] = L1CaloPprDisabledChannel(chanNum);
	}
	m_mPprDisabledChannelMap[chanNum].isMasked(true);
*/
	// add invalid channels ?

}

const L1CaloPprDisabledChannel* L1CaloPprDisabledChannelContainer::pprDisabledChannel(unsigned int channelId) const {
	L1CaloPprDisabledChannelMap::const_iterator pos = m_mPprDisabledChannelMap.find(channelId);
	if(pos==m_mPprDisabledChannelMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPprDisabledChannel* L1CaloPprDisabledChannelContainer::pprDisabledChannel(const L1CaloCoolChannelId& channelId) const {
	return this->pprDisabledChannel(channelId.id());
}

void L1CaloPprDisabledChannelContainer::addPprDisabledChannel(unsigned int channelId, const L1CaloPprDisabledChannel& pprDisabledChannel) {
	m_mPprDisabledChannelMap[channelId] = pprDisabledChannel;
}

void L1CaloPprDisabledChannelContainer::addPprDisabledChannel(const L1CaloCoolChannelId& channelId, const L1CaloPprDisabledChannel& pprDisabledChannel) {
	return this->addPprDisabledChannel(channelId.id(), pprDisabledChannel);
}

void L1CaloPprDisabledChannelContainer::dump() const {
	L1CaloPprDisabledChannelContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPprDisabledChannelContainer::clear() {
	m_mPprDisabledChannelMap.clear();
}
