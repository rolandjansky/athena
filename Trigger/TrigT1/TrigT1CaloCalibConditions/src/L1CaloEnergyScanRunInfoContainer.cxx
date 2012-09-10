/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloEnergyScanRunInfoContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>

L1CaloEnergyScanRunInfoContainer::L1CaloEnergyScanRunInfoContainer() :
  AbstractL1CaloPersistentCondition("AthenaAttributeList"),
  m_runNumber(0),
  m_gainStrategy("")
{
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eRunNumber,   std::string("RunNumber"),   std::string("unsigned int"));
    this->addSpecification(eGainStrategy,std::string("GainStrategy"),std::string("string"));

    m_mCoolFoldersKeysMap[L1CaloEnergyScanRunInfoContainer::eEnergyScanRunInfo] = std::string("/TRIGGER/L1Calo/V1/Results/EnergyScanRunInfo");
}

L1CaloEnergyScanRunInfoContainer::L1CaloEnergyScanRunInfoContainer(const std::map<L1CaloEnergyScanRunInfoContainer::eCoolFolders, std::string>& folderKeysMap) : 
  AbstractL1CaloPersistentCondition("AthenaAttributeList"),
  m_mCoolFoldersKeysMap(folderKeysMap),
  m_runNumber(0),
  m_gainStrategy("")
{
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eRunNumber,   std::string("RunNumber"),   std::string("unsigned int"));
    this->addSpecification(eGainStrategy,std::string("GainStrategy"),std::string("string"));
}

std::string L1CaloEnergyScanRunInfoContainer::coolFolderKey(L1CaloEnergyScanRunInfoContainer::eCoolFolders efolder) const {

    std::map<L1CaloEnergyScanRunInfoContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
    if(it!=m_mCoolFoldersKeysMap.end()) {
        return it->second;
    } else {
        return std::string("");
    }
}

std::vector<std::string> L1CaloEnergyScanRunInfoContainer::coolInputKeys() const {
    std::vector<std::string> v;
    v.push_back(this->coolFolderKey(L1CaloEnergyScanRunInfoContainer::eEnergyScanRunInfo));
    return v;
}

std::string L1CaloEnergyScanRunInfoContainer::coolOutputKey() const {
    return this->coolFolderKey(L1CaloEnergyScanRunInfoContainer::eEnergyScanRunInfo);
}

DataObject* L1CaloEnergyScanRunInfoContainer::makePersistent() const {

    // Create AttributeListSpecification according to the attributes to be recorded
    // this is required to create the AttributeList objs
    coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
    if(!attrSpecification->size()) return NULL;

    AthenaAttributeList* attrList = new AthenaAttributeList(*attrSpecification);

    (*attrList)[ this->specificationName(eRunNumber) ].setValue(m_runNumber);
    (*attrList)[ this->specificationName(eGainStrategy) ].setValue(m_gainStrategy);

    return (DataObject*) attrList;
}

void L1CaloEnergyScanRunInfoContainer::makeTransient(const std::map<std::string, AthenaAttributeList*> athenaAttributeListMap) {

    std::map<std::string, AthenaAttributeList*>::const_iterator
      it_map(athenaAttributeListMap.find(this->coolFolderKey(L1CaloEnergyScanRunInfoContainer::eEnergyScanRunInfo)));

    if(it_map!=athenaAttributeListMap.end()) {

        const AthenaAttributeList& attrList(*(it_map->second));

	m_runNumber = attrList[ this->specificationName(eRunNumber) ].data<unsigned int>();
	m_gainStrategy = attrList[ this->specificationName(eGainStrategy) ].data<std::string>();

    } else {
        std::cout << "L1CaloEnergyScanRunInfoContainer : Could not find requested AthenaAttributeList "
                  << this->coolFolderKey(L1CaloEnergyScanRunInfoContainer::eEnergyScanRunInfo) << std::endl;
    }
}
