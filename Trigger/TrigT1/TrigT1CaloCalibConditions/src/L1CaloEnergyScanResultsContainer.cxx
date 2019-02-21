/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResultsContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/ChanFitErrorCode.h"

#include <string>
#include <iostream>

L1CaloEnergyScanResultsContainer::L1CaloEnergyScanResultsContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eOffset,      std::string("Offset"),      std::string("double"));
    this->addSpecification(eSlope,       std::string("Slope"),       std::string("double"));
    this->addSpecification(eChi2,        std::string("Chi2"),        std::string("double"));
    this->addSpecification(eNdf,        std::string("Ndf"),        std::string("unsigned char"));
    this->addSpecification(eBlob,        std::string("Blob"),        std::string("blob"));
    this->addSpecification(eBlobStrategy,std::string("BlobStrategy"),std::string("unsigned char"));
    this->addSpecification(eErrorCode,   std::string("ErrorCode"),   std::string("unsigned int"));

    m_mCoolFoldersKeysMap[L1CaloEnergyScanResultsContainer::eEnergyScanResults] = std::string("/TRIGGER/L1Calo/V1/Results/EnergyScanResults");
}

L1CaloEnergyScanResultsContainer::L1CaloEnergyScanResultsContainer(const std::map<L1CaloEnergyScanResultsContainer::eCoolFolders, std::string>& folderKeysMap) : 
  AbstractL1CaloPersistentCondition("CondAttrListCollection"),
  m_mCoolFoldersKeysMap(folderKeysMap)
{
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eOffset,      std::string("Offset"),      std::string("double"));
    this->addSpecification(eSlope,       std::string("Slope"),       std::string("double"));
    this->addSpecification(eChi2,        std::string("Chi2"),        std::string("double"));
    this->addSpecification(eNdf,        std::string("Ndf"),        std::string("unsigned char"));
    this->addSpecification(eBlob,        std::string("Blob"),        std::string("blob"));
    this->addSpecification(eBlobStrategy,std::string("BlobStrategy"),std::string("unsigned char"));
    this->addSpecification(eErrorCode,   std::string("ErrorCode"),   std::string("unsigned int"));
}

std::string L1CaloEnergyScanResultsContainer::coolFolderKey(L1CaloEnergyScanResultsContainer::eCoolFolders efolder) const {

    std::map<L1CaloEnergyScanResultsContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
    if(it!=m_mCoolFoldersKeysMap.end()) {
        return it->second;
    } else {
        return std::string("");
    }
}

std::vector<std::string> L1CaloEnergyScanResultsContainer::coolInputKeys() const {
    std::vector<std::string> v;
    v.push_back(this->coolFolderKey(L1CaloEnergyScanResultsContainer::eEnergyScanResults));
    return v;
}

std::string L1CaloEnergyScanResultsContainer::coolOutputKey() const {
    return this->coolFolderKey(L1CaloEnergyScanResultsContainer::eEnergyScanResults);
}

DataObject* L1CaloEnergyScanResultsContainer::makePersistent() const {
    if(m_mEnergyScanResultsMap.size()==0) return 0;

    // Create AttributeListSpecification according to the attributes to be recorded
    // this is required to create the AttributeList objs
    coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
    if(!attrSpecification->size()) return NULL;

    CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

    L1CaloEnergyScanResultsMap::const_iterator pos = m_mEnergyScanResultsMap.begin();
    L1CaloEnergyScanResultsMap::const_iterator end = m_mEnergyScanResultsMap.end();
    for(; pos!=end; ++pos) {
        const unsigned int channelId = pos->first;
        const L1CaloEnergyScanResults&  p = pos->second;

        AthenaAttributeList attrList(*attrSpecification);

        // Set value of each row for the current channel
        attrList[ this->specificationName(eOffset) ].setValue(p.offset());
        attrList[ this->specificationName(eSlope) ].setValue(p.slope());
        attrList[ this->specificationName(eChi2) ].setValue(p.chi2());
        attrList[ this->specificationName(eNdf) ].setValue(p.ndf());
        attrList[ this->specificationName(eBlob) ].setValue(p.blob());
        attrList[ this->specificationName(eBlobStrategy) ].setValue(p.blobStrategy());
        attrList[ this->specificationName(eErrorCode) ].setValue(p.errorCode().errorCode());

        attrListCollection->add(channelId, attrList);
    }

    return (DataObject*) attrListCollection;
}

void L1CaloEnergyScanResultsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

    this->clear();

    std::map<std::string, CondAttrListCollection*>::const_iterator
      it_map(condAttrListCollectionMap.find(this->coolFolderKey(L1CaloEnergyScanResultsContainer::eEnergyScanResults)));

    if(it_map!=condAttrListCollectionMap.end()) {

        CondAttrListCollection* attrListCollection = it_map->second;

        //loop over CondAttrListCollection
        CondAttrListCollection::const_iterator pos = attrListCollection->begin();
        CondAttrListCollection::const_iterator end = attrListCollection->begin();
        for(; pos!= end; ++pos) {

            const coral::AttributeList& attrList = pos->second;

            // Get value of each row for the current channel
            CondAttrListCollection::ChanNum chanNum(pos->first);

            double offset = attrList[ this->specificationName(eOffset) ].data<double>();
            double slope = attrList[ this->specificationName(eSlope) ].data<double>();
            double chi2 = attrList[ this->specificationName(eChi2) ].data<double>();
            unsigned char ndf = attrList[ this->specificationName(eNdf) ].data<unsigned char>();
            coral::Blob blob = attrList[ this->specificationName(eBlob) ].data<coral::Blob>();
            unsigned char blobStrategy = attrList[ this->specificationName(eBlobStrategy) ].data<unsigned char>();
            ChanFitErrorCode errorCode(attrList[ this->specificationName(eErrorCode) ].data<unsigned int>());

            L1CaloEnergyScanResults l1CaloEnergyScanResults(chanNum, offset, slope, chi2, ndf, blob, blobStrategy, errorCode);
            m_mEnergyScanResultsMap[chanNum] = l1CaloEnergyScanResults;
        }
    } else {
        std::cout << "L1CaloEnergyScanResultsContainer : Could not find requested CondAttrListCollection "
                  << this->coolFolderKey(L1CaloEnergyScanResultsContainer::eEnergyScanResults) << std::endl;
    }
}

const L1CaloEnergyScanResults* L1CaloEnergyScanResultsContainer::energyScanResults(unsigned int channelId) const {
    L1CaloEnergyScanResultsMap::const_iterator pos = m_mEnergyScanResultsMap.find(channelId);
    if(pos==m_mEnergyScanResultsMap.end()) return 0;
    else return &(pos->second);
}

const L1CaloEnergyScanResults* L1CaloEnergyScanResultsContainer::energyScanResults(const L1CaloCoolChannelId& channelId) const {
    return this->energyScanResults(channelId.id());
}

void L1CaloEnergyScanResultsContainer::addEnergyScanResults(unsigned int channelId, const L1CaloEnergyScanResults& energyScanResults) {
    m_mEnergyScanResultsMap[channelId] = energyScanResults;
}

void L1CaloEnergyScanResultsContainer::addEnergyScanResults(const L1CaloCoolChannelId& channelId, const L1CaloEnergyScanResults& energyScanResults) {
    return this->addEnergyScanResults(channelId.id(), energyScanResults);
}

void L1CaloEnergyScanResultsContainer::clear() {
    m_mEnergyScanResultsMap.clear();
}
