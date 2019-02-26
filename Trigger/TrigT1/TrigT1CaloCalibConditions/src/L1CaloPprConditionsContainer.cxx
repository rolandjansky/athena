/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"

#include <iostream>
#include <set>

const unsigned int L1CaloPprConditionsContainer::s_vectorSize;

L1CaloPprConditionsContainer::L1CaloPprConditionsContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection"),
        m_bcidDecision1(0),
	m_satOverride1(0),
	m_bcidDecision2(0),
	m_satOverride2(0),
	m_bcidDecision3(0),
	m_satOverride3(0),
	m_peakFinderCond(0),
	m_decisionSource(0)
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
	this->addSpecification(eDecisionSource,      std::string("CR15_DecisionSource"),  std::string("UInt32"));

	this->addSpecification(eFirCoeff1, std::string("FirCoeff1"), std::string("short"));
	this->addSpecification(eFirCoeff2, std::string("FirCoeff2"), std::string("short"));
	this->addSpecification(eFirCoeff3, std::string("FirCoeff3"), std::string("short"));
	this->addSpecification(eFirCoeff4, std::string("FirCoeff4"), std::string("short"));
	this->addSpecification(eFirCoeff5, std::string("FirCoeff5"), std::string("short"));

	this->addSpecification(eLutStrategy,    std::string("LutStrategy"),   std::string("UInt16"));
	this->addSpecification(eLutOffset,      std::string("LutOffset"),     std::string("UInt16"));
	this->addSpecification(eLutSlope,       std::string("LutSlope"),      std::string("UInt16"));
	this->addSpecification(eLutNoiseCut,    std::string("LutNoiseCut"),   std::string("UInt16"));
	this->addSpecification(ePedValue,       std::string("PedValue"),      std::string("UInt32"));
	this->addSpecification(ePedMean,        std::string("PedMean"),       std::string("Double"));

	m_mCoolFoldersKeysMap[L1CaloPprConditionsContainer::ePprChanCalib] = std::string("/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib");
	m_mCoolFoldersKeysMap[L1CaloPprConditionsContainer::ePprChanDefaults] = std::string("/TRIGGER/L1Calo/V1/Configuration/PprChanDefaults");

	m_mPprConditionsVec.reserve(s_vectorSize);
	this->clear();
}

L1CaloPprConditionsContainer::L1CaloPprConditionsContainer(const std::map<L1CaloPprConditionsContainer::eCoolFolders, std::string>& folderKeysMap):AbstractL1CaloPersistentCondition("CondAttrListCollection"),
	m_mCoolFoldersKeysMap(folderKeysMap),
        m_bcidDecision1(0),
	m_satOverride1(0),
	m_bcidDecision2(0),
	m_satOverride2(0),
	m_bcidDecision3(0),
	m_satOverride3(0),
	m_peakFinderCond(0),
	m_decisionSource(0)
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
	this->addSpecification(eDecisionSource,      std::string("CR15_DecisionSource"),  std::string("UInt32"));

	this->addSpecification(eFirCoeff1, std::string("FirCoeff1"), std::string("short"));
	this->addSpecification(eFirCoeff2, std::string("FirCoeff2"), std::string("short"));
	this->addSpecification(eFirCoeff3, std::string("FirCoeff3"), std::string("short"));
	this->addSpecification(eFirCoeff4, std::string("FirCoeff4"), std::string("short"));
	this->addSpecification(eFirCoeff5, std::string("FirCoeff5"), std::string("short"));

	this->addSpecification(eLutStrategy,    std::string("LutStrategy"),   std::string("UInt16"));
	this->addSpecification(eLutOffset,      std::string("LutOffset"),     std::string("UInt16"));
	this->addSpecification(eLutSlope,       std::string("LutSlope"),      std::string("UInt16"));
	this->addSpecification(eLutNoiseCut,    std::string("LutNoiseCut"),   std::string("UInt16"));
	this->addSpecification(ePedValue,       std::string("PedValue"),      std::string("UInt32"));
	this->addSpecification(ePedMean,        std::string("PedMean"),       std::string("Double"));

	m_mPprConditionsVec.reserve(s_vectorSize);
	this->clear();
}

L1CaloPprConditionsContainer::~L1CaloPprConditionsContainer()
{
        this->clear();
}

std::string L1CaloPprConditionsContainer::coolFolderKey(L1CaloPprConditionsContainer::eCoolFolders efolder) const {

	std::map<L1CaloPprConditionsContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
	if(it!=m_mCoolFoldersKeysMap.end()) {
		return it->second;
	} else {
		return std::string("");
	}
}

std::vector<std::string> L1CaloPprConditionsContainer::coolInputKeys() const {
	std::vector<std::string> v;
	v.push_back(this->coolFolderKey(L1CaloPprConditionsContainer::ePprChanCalib));
	v.push_back(this->coolFolderKey(L1CaloPprConditionsContainer::ePprChanDefaults));
	return v;
}

std::string L1CaloPprConditionsContainer::coolOutputKey() const {
	return this->coolFolderKey(L1CaloPprConditionsContainer::ePprChanCalib);
}

DataObject* L1CaloPprConditionsContainer::makePersistent() const {
	// this container is not supposed to be saved to COOL
	return 0;
}

void L1CaloPprConditionsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

	this->clear();

	std::string chanCalibFolderKey(this->coolFolderKey(L1CaloPprConditionsContainer::ePprChanCalib));
	std::map<std::string, CondAttrListCollection*>::const_iterator it_pprChanCalibAttrListCollection = condAttrListCollectionMap.find(chanCalibFolderKey);
	if(it_pprChanCalibAttrListCollection==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprConditionsContainer : Could not find requested CondAttrListCollection "<< chanCalibFolderKey << std::endl;
		return;
	}

	CondAttrListCollection* chanCalibAttrListCollection = it_pprChanCalibAttrListCollection->second;

	std::string chanDefaultsFolderKey(this->coolFolderKey(L1CaloPprConditionsContainer::ePprChanDefaults));
	std::map<std::string, CondAttrListCollection*>::const_iterator it_pprChanDefaultsAttrListCollection = condAttrListCollectionMap.find(chanDefaultsFolderKey);
	if(it_pprChanDefaultsAttrListCollection==condAttrListCollectionMap.end()) {
		std::cout<<"L1CaloPprConditionsContainer : Could not find requested CondAttrListCollection "<< chanDefaultsFolderKey << std::endl;
		return;
	}

	CondAttrListCollection* chanDefaultsAttrListCollection = it_pprChanDefaultsAttrListCollection->second;
	
	// There should be only one channel (channel#1) in the Default folder
	// we just retrieve that one, waiting for a better method to retrieve that information.
	const int defaultChannel = 1;
	const coral::AttributeList& chanDefaultAttrList(chanDefaultsAttrListCollection->attributeList(defaultChannel));

        m_bcidDecision1 = chanDefaultAttrList[ this->specificationName(eBcidDecision1) ].data<unsigned int>();
	m_satOverride1 = chanDefaultAttrList[ this->specificationName(eSatOverride1) ].data<unsigned int>();
	m_bcidDecision2 = chanDefaultAttrList[ this->specificationName(eBcidDecision2) ].data<unsigned int>();
	m_satOverride2 = chanDefaultAttrList[ this->specificationName(eSatOverride2) ].data<unsigned int>();
	m_bcidDecision3 = chanDefaultAttrList[ this->specificationName(eBcidDecision3) ].data<unsigned int>();
	m_satOverride3 = chanDefaultAttrList[ this->specificationName(eSatOverride3) ].data<unsigned int>();
	m_peakFinderCond = chanDefaultAttrList[ this->specificationName(ePeakFinderCond) ].data<unsigned int>();
	m_decisionSource = chanDefaultAttrList[ this->specificationName(eDecisionSource) ].data<unsigned int>();

	//loop over CondAttrListCollection
	CondAttrListCollection::const_iterator it_AttrListColl = chanCalibAttrListCollection->begin();
	for(;it_AttrListColl!=chanCalibAttrListCollection->end();++it_AttrListColl) {

		CondAttrListCollection::ChanNum chanNum(it_AttrListColl->first);
		const coral::AttributeList& chanCalibAttrList(it_AttrListColl->second);

		unsigned short extBcidThreshold = chanCalibAttrList[ this->specificationName(eExtBcidThreshold) ].data<unsigned short>();
		unsigned short satBcidThreshLow = chanCalibAttrList[ this->specificationName(eSatBcidThreshLow) ].data<unsigned short>();
		unsigned short satBcidThreshHigh = chanCalibAttrList[ this->specificationName(eSatBcidThreshHigh) ].data<unsigned short>();
		unsigned short satBcidLevel = chanCalibAttrList[ this->specificationName(eSatBcidLevel) ].data<unsigned short>();

		unsigned short bcidEnergyRangeLow = chanCalibAttrList[ this->specificationName(eBcidEnergyRangeLow) ].data<unsigned short>();
		unsigned short bcidEnergyRangeHigh = chanCalibAttrList[ this->specificationName(eBcidEnergyRangeHigh) ].data<unsigned short>();

		unsigned short firStartBit = chanCalibAttrList[ this->specificationName(eFirStartBit) ].data<unsigned short>();

		short int firCoeff1 = chanCalibAttrList[ this->specificationName(eFirCoeff1) ].data<short>();
		short int firCoeff2 = chanCalibAttrList[ this->specificationName(eFirCoeff2) ].data<short>();
		short int firCoeff3 = chanCalibAttrList[ this->specificationName(eFirCoeff3) ].data<short>();
		short int firCoeff4 = chanCalibAttrList[ this->specificationName(eFirCoeff4) ].data<short>();
		short int firCoeff5 = chanCalibAttrList[ this->specificationName(eFirCoeff5) ].data<short>();

		unsigned short lutStrategy = chanCalibAttrList[ this->specificationName(eLutStrategy) ].data<unsigned short>();
		unsigned short lutOffset = chanCalibAttrList[ this->specificationName(eLutOffset) ].data<unsigned short>();
		unsigned short lutSlope = chanCalibAttrList[ this->specificationName(eLutSlope) ].data<unsigned short>(); 
		unsigned short lutNoiseCut = chanCalibAttrList[ this->specificationName(eLutNoiseCut) ].data<unsigned short>();
		unsigned int pedValue = chanCalibAttrList[ this->specificationName(ePedValue) ].data<unsigned int>();
		float pedMean = (float) chanCalibAttrList[ this->specificationName(ePedMean) ].data<double>();

		L1CaloCoolChannelId coolId(chanNum);
		unsigned int index = (coolId.crate()<<10)+(coolId.module()<<6)+(coolId.subModule()<<2)+coolId.channel();
		if (index < s_vectorSize) {
		    L1CaloPprConditions* l1Calocond = new L1CaloPprConditions(extBcidThreshold, satBcidThreshLow, satBcidThreshHigh,
		                                      satBcidLevel, bcidEnergyRangeLow, bcidEnergyRangeHigh,
		    				      firStartBit, firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5,
						      lutStrategy, lutOffset, lutSlope, lutNoiseCut, pedValue, pedMean);
		    m_mPprConditionsVec[index] = l1Calocond;
	        }
	}
}

const L1CaloPprConditions* L1CaloPprConditionsContainer::pprConditions(unsigned int channelId) const {
        L1CaloCoolChannelId coolId(channelId);
	return pprConditions(coolId);
}

const L1CaloPprConditions* L1CaloPprConditionsContainer::pprConditions(const L1CaloCoolChannelId& channelId) const {
        unsigned int index = (channelId.crate()<<10)+(channelId.module()<<6)+(channelId.subModule()<<2)+channelId.channel();
	if (index < s_vectorSize) return m_mPprConditionsVec[index];
	else return 0;
}

void L1CaloPprConditionsContainer::dump() const {
        std::cout << "bcidDecision1: "  << m_bcidDecision1  << ", "
	          << "satOverride1: "   << m_satOverride1   << ", "
	          << "bcidDecision2: "  << m_bcidDecision2  << ", "
		  << "satOverride2: "   << m_satOverride2   << ", "
		  << "bcidDecision3: "  << m_bcidDecision3  << ", "
		  << "satOverride3: "   << m_satOverride3   << ", "
		  << "peakFinderCond: " << m_peakFinderCond << ", "
		  << "decisionSource: " << m_decisionSource << std::endl;
	L1CaloPprConditionsVec::const_iterator it  = m_mPprConditionsVec.begin();
	L1CaloPprConditionsVec::const_iterator itE = m_mPprConditionsVec.end();
	for (int index = 0; it != itE; ++it, ++index) {
		if (*it) std::cout << "index " << index << " * item: " << *it << std::endl;
	}
}

void L1CaloPprConditionsContainer::clear() {
        L1CaloPprConditionsVec::const_iterator pos  = m_mPprConditionsVec.begin();
	L1CaloPprConditionsVec::const_iterator posE = m_mPprConditionsVec.end();
	for(; pos != posE; ++pos) if (*pos) delete *pos;
	L1CaloPprConditions* p = 0;
	m_mPprConditionsVec.assign(s_vectorSize, p);
}
