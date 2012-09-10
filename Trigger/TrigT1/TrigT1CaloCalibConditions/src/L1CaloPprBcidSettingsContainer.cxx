/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprBcidSettingsContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>

L1CaloPprBcidSettingsContainer::L1CaloPprBcidSettingsContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eExtBcidThreshold,    std::string("ExtBcidThreshold"),     std::string("UInt16"));
	this->addSpecification(eSatBcidThreshLow,    std::string("SatBcidThreshLow"),     std::string("UInt16"));
	this->addSpecification(eSatBcidThreshHigh,   std::string("SatBcidThreshHigh"),    std::string("UInt16"));
	this->addSpecification(eSatBcidLevel,        std::string("SatBcidLevel"),         std::string("UInt16"));
	this->addSpecification(eBcidEnergyRangeLow,  std::string("BcidEnergyRangeLow"),   std::string("UInt16"));
	this->addSpecification(eBcidEnergyRangeHigh, std::string("BcidEnergyRangeHigh"),  std::string("UInt16"));
	this->addSpecification(eFirStartBit,         std::string("FirStartBit"),          std::string("UInt16"));
	this->addSpecification(eBcidDecision1,       std::string("CR12_BcidDecision1"),   std::string("UInt32"));
	this->addSpecification(eSatOverride1,        std::string("CR12_SatOverride1"),    std::string("UInt32"));
	this->addSpecification(eBcidDecision2,       std::string("CR13_BcidDecision2"),   std::string("UInt32"));
	this->addSpecification(eSatOverride2,        std::string("CR13_SatOverride2"),    std::string("UInt32"));
	this->addSpecification(eBcidDecision3,       std::string("CR14_BcidDecision3"),   std::string("UInt32"));
	this->addSpecification(eSatOverride3,        std::string("CR14_SatOverride3"),    std::string("UInt32"));
	this->addSpecification(ePeakFinderCond,      std::string("CR15_PeakFinderCond"),  std::string("UInt32"));
	this->addSpecification(eDecisionSource,      std::string("CR15_DecisionSource"), std::string("UInt32"));

	m_mCoolFoldersKeysMap[L1CaloPprBcidSettingsContainer::eBCIDChanCalib] = std::string("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib");
	m_mCoolFoldersKeysMap[L1CaloPprBcidSettingsContainer::eBCIDChanDefaults] = std::string("/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults");
}

L1CaloPprBcidSettingsContainer::L1CaloPprBcidSettingsContainer(const std::map<L1CaloPprBcidSettingsContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap)
{
	// Define DB rows names and types in order to construct the AttributeListSpecification object
	this->addSpecification(eExtBcidThreshold,    std::string("ExtBcidThreshold"),     std::string("UInt16"));
	this->addSpecification(eSatBcidThreshLow,    std::string("SatBcidThreshLow"),     std::string("UInt16"));
	this->addSpecification(eSatBcidThreshHigh,   std::string("SatBcidThreshHigh"),    std::string("UInt16"));
	this->addSpecification(eSatBcidLevel,        std::string("SatBcidLevel"),         std::string("UInt16"));
	this->addSpecification(eBcidEnergyRangeLow,  std::string("BcidEnergyRangeLow"),   std::string("UInt16"));
	this->addSpecification(eBcidEnergyRangeHigh, std::string("BcidEnergyRangeHigh"),  std::string("UInt16"));
	this->addSpecification(eFirStartBit,         std::string("FirStartBit"),          std::string("UInt16"));
	this->addSpecification(eBcidDecision1,       std::string("CR12_BcidDecision1"),   std::string("UInt32"));
	this->addSpecification(eSatOverride1,        std::string("CR12_SatOverride1"),    std::string("UInt32"));
	this->addSpecification(eBcidDecision2,       std::string("CR13_BcidDecision2"),   std::string("UInt32"));
	this->addSpecification(eSatOverride2,        std::string("CR13_SatOverride2"),    std::string("UInt32"));
	this->addSpecification(eBcidDecision3,       std::string("CR14_BcidDecision3"),   std::string("UInt32"));
	this->addSpecification(eSatOverride3,        std::string("CR14_SatOverride3"),    std::string("UInt32"));
	this->addSpecification(ePeakFinderCond,      std::string("CR15_PeakFinderCond"),  std::string("UInt32"));
	this->addSpecification(eDecisionSource,      std::string("CR15_DecisionSource"), std::string("UInt32"));
}

std::string L1CaloPprBcidSettingsContainer::coolFolderKey(L1CaloPprBcidSettingsContainer::eCoolFolders efolder) const {

	std::map<L1CaloPprBcidSettingsContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPprBcidSettingsContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPprBcidSettingsContainer::eBCIDChanCalib));
	v.push_back(this->coolFolderKey(L1CaloPprBcidSettingsContainer::eBCIDChanDefaults));
	return v;
}

std::string L1CaloPprBcidSettingsContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPprBcidSettingsContainer::eBCIDChanCalib);
}

DataObject* L1CaloPprBcidSettingsContainer::makePersistent() const {
	// this container is not supposed to be saved to COOL
	return 0;
}

void L1CaloPprBcidSettingsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::string chanCalibFolderKey(this->coolFolderKey(L1CaloPprBcidSettingsContainer::eBCIDChanCalib));
	std::map<std::string, CondAttrListCollection*>::const_iterator it_bcidChanCalibAttrListCollection = condAttrListCollectionMap.find(chanCalibFolderKey);
	if(it_bcidChanCalibAttrListCollection==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprBcidSettingsContainer : Could not find requested CondAttrListCollection "<< chanCalibFolderKey << std::endl;
		return;
	}

	CondAttrListCollection* chanCalibAttrListCollection = it_bcidChanCalibAttrListCollection->second;

	std::string chanDefaultsFolderKey(this->coolFolderKey(L1CaloPprBcidSettingsContainer::eBCIDChanDefaults));
	std::map<std::string, CondAttrListCollection*>::const_iterator it_bcidChanDefaultsAttrListCollection = condAttrListCollectionMap.find(chanDefaultsFolderKey);
	if(it_bcidChanDefaultsAttrListCollection==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprBcidSettingsContainer : Could not find requested CondAttrListCollection "<< chanDefaultsFolderKey << std::endl;
		return;
	}

	CondAttrListCollection* chanDefaultsAttrListCollection = it_bcidChanDefaultsAttrListCollection->second;
	// There should be only one channel (channel#1) in the Default folder
	// we just retrieve that one, waiting for a better method to retrieve that information.
	const int defaultChannel = 1;
	AthenaAttributeList chanDefaultAttrList(chanDefaultsAttrListCollection->attributeList(defaultChannel));

	//loop over CondAttrListCollection
	CondAttrListCollection::const_iterator it_AttrListColl = chanCalibAttrListCollection->begin();
	for(;it_AttrListColl!=chanCalibAttrListCollection->end();++it_AttrListColl) {

		CondAttrListCollection::ChanNum chanNum(it_AttrListColl->first);
		AthenaAttributeList chanCalibAttrList(it_AttrListColl->second);


		unsigned short extBcidThreshold = chanCalibAttrList[ this->specificationName(eExtBcidThreshold) ].data<unsigned short>();
		unsigned short satBcidThreshLow = chanCalibAttrList[ this->specificationName(eSatBcidThreshLow) ].data<unsigned short>();
		unsigned short satBcidThreshHigh = chanCalibAttrList[ this->specificationName(eSatBcidThreshHigh) ].data<unsigned short>();
		unsigned short satBcidLevel = chanCalibAttrList[ this->specificationName(eSatBcidLevel) ].data<unsigned short>();

		unsigned short bcidEnergyRangeLow = chanCalibAttrList[ this->specificationName(eBcidEnergyRangeLow) ].data<unsigned short>();
		unsigned short bcidEnergyRangeHigh = chanCalibAttrList[ this->specificationName(eBcidEnergyRangeHigh) ].data<unsigned short>();

		unsigned int firStartBit = (unsigned int) chanCalibAttrList[ this->specificationName(eFirStartBit) ].data<unsigned short>();

		unsigned int bcidDecision1 = chanDefaultAttrList[ this->specificationName(eBcidDecision1) ].data<unsigned int>();
		unsigned int satOverride1 = chanDefaultAttrList[ this->specificationName(eSatOverride1) ].data<unsigned int>();

		unsigned int bcidDecision2 = chanDefaultAttrList[ this->specificationName(eBcidDecision2) ].data<unsigned int>();
		unsigned int satOverride2 = chanDefaultAttrList[ this->specificationName(eSatOverride2) ].data<unsigned int>();

		unsigned int bcidDecision3 = chanDefaultAttrList[ this->specificationName(eBcidDecision3) ].data<unsigned int>();
		unsigned int satOverride3 = chanDefaultAttrList[ this->specificationName(eSatOverride3) ].data<unsigned int>();

		unsigned int peakFinderCond = chanDefaultAttrList[ this->specificationName(ePeakFinderCond) ].data<unsigned int>();
		unsigned int decisionSource = chanDefaultAttrList[ this->specificationName(eDecisionSource) ].data<unsigned int>();

		L1CaloPprBcidSettings l1CaloBCIDSettings(chanNum, extBcidThreshold, satBcidThreshLow, satBcidThreshHigh, satBcidLevel, bcidEnergyRangeLow, bcidEnergyRangeHigh, firStartBit, bcidDecision1, satOverride1, bcidDecision2, satOverride2, bcidDecision3, satOverride3, peakFinderCond, decisionSource);
		m_mBCIDSettingsMap[chanNum] = l1CaloBCIDSettings;
	}
}

const L1CaloPprBcidSettings* L1CaloPprBcidSettingsContainer::bcidSettings(unsigned int channelId) const {
	L1CaloPprBcidSettingsMap::const_iterator pos = m_mBCIDSettingsMap.find(channelId);
	if(pos==m_mBCIDSettingsMap.end()) return 0;
	else return &(pos->second);
}

const L1CaloPprBcidSettings* L1CaloPprBcidSettingsContainer::bcidSettings(const L1CaloCoolChannelId& channelId) const {
	return this->bcidSettings(channelId.id());
}

void L1CaloPprBcidSettingsContainer::addBcidSettings(unsigned int channelId, const L1CaloPprBcidSettings& bcidSettings) {
	m_mBCIDSettingsMap[channelId] = bcidSettings;
}

void L1CaloPprBcidSettingsContainer::addBcidSettings(const L1CaloCoolChannelId& channelId, const L1CaloPprBcidSettings& bcidSettings) {
	this->addBcidSettings(channelId.id(), bcidSettings);
}

void L1CaloPprBcidSettingsContainer::dump() const {
	L1CaloPprBcidSettingsContainer::const_iterator it = this->begin();
	for(;it!=this->end();++it) {
		std::cout << " * item: " <<it->first << " => "<<it->second <<std::endl;
	}
}

void L1CaloPprBcidSettingsContainer::clear() {
	m_mBCIDSettingsMap.clear();
}
