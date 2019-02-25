/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloHVCorrectionsContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>

L1CaloHVCorrectionsContainer::L1CaloHVCorrectionsContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eRxMean, std::string("RxMean"), std::string("float"));
    this->addSpecification(eAffectedCells1, std::string("AffectedCells1"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells2, std::string("AffectedCells2"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells3, std::string("AffectedCells3"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells4, std::string("AffectedCells4"), std::string("unsigned char"));
    this->addSpecification(eLayerMean1, std::string("LayerMean1"), std::string("float"));
    this->addSpecification(eLayerMean2, std::string("LayerMean2"), std::string("float"));
    this->addSpecification(eLayerMean3, std::string("LayerMean3"), std::string("float"));
    this->addSpecification(eLayerMean4, std::string("LayerMean4"), std::string("float"));

    m_mCoolFoldersKeysMap[L1CaloHVCorrectionsContainer::eHVCorrections] = std::string("/TRIGGER/L1Calo/V1/Results/HVCorrections");
}

L1CaloHVCorrectionsContainer::L1CaloHVCorrectionsContainer(const std::map<L1CaloHVCorrectionsContainer::eCoolFolders, std::string>& folderKeysMap) : 
  AbstractL1CaloPersistentCondition("CondAttrListCollection"),
  m_mCoolFoldersKeysMap(folderKeysMap)
{
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eRxMean, std::string("RxMean"), std::string("float"));
    this->addSpecification(eAffectedCells1, std::string("AffectedCells1"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells2, std::string("AffectedCells2"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells3, std::string("AffectedCells3"), std::string("unsigned char"));
    this->addSpecification(eAffectedCells4, std::string("AffectedCells4"), std::string("unsigned char"));
    this->addSpecification(eLayerMean1, std::string("LayerMean1"), std::string("float"));
    this->addSpecification(eLayerMean2, std::string("LayerMean2"), std::string("float"));
    this->addSpecification(eLayerMean3, std::string("LayerMean3"), std::string("float"));
    this->addSpecification(eLayerMean4, std::string("LayerMean4"), std::string("float"));
}

std::string L1CaloHVCorrectionsContainer::coolFolderKey(L1CaloHVCorrectionsContainer::eCoolFolders efolder) const {

    std::map<L1CaloHVCorrectionsContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
    if(it!=m_mCoolFoldersKeysMap.end()) {
        return it->second;
    } else {
        return std::string("");
    }
}

std::vector<std::string> L1CaloHVCorrectionsContainer::coolInputKeys() const {
    std::vector<std::string> v;
    v.push_back(this->coolFolderKey(L1CaloHVCorrectionsContainer::eHVCorrections));
    return v;
}

std::string L1CaloHVCorrectionsContainer::coolOutputKey() const {
    return this->coolFolderKey(L1CaloHVCorrectionsContainer::eHVCorrections);
}

DataObject* L1CaloHVCorrectionsContainer::makePersistent() const {
    if(m_mHVCorrectionsMap.size()==0) return 0;

    // Create AttributeListSpecification according to the attributes to be recorded
    // this is required to create the AttributeList objs
    coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
    if(!attrSpecification->size()) return NULL;

    CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

    L1CaloHVCorrectionsMap::const_iterator pos = m_mHVCorrectionsMap.begin();
    L1CaloHVCorrectionsMap::const_iterator end = m_mHVCorrectionsMap.end();
    for(; pos!=end; ++pos) {
        const unsigned int channelId = pos->first;
        const L1CaloHVCorrections& p = pos->second;
	const std::vector<int>& affectedCells(p.affectedCells());
	const std::vector<float>& layerMeans(p.layerMeans());
	const unsigned char nlayers = affectedCells.size();
	const unsigned char affectedCells1 = (nlayers > 0) ? affectedCells[0] : 0;
	const unsigned char affectedCells2 = (nlayers > 1) ? affectedCells[1] : 0;
	const unsigned char affectedCells3 = (nlayers > 2) ? affectedCells[2] : 0;
	const unsigned char affectedCells4 = (nlayers > 3) ? affectedCells[3] : 0;
	const float         layerMean1     = (nlayers > 0) ? layerMeans[0] : 0.;
	const float         layerMean2     = (nlayers > 1) ? layerMeans[1] : 0.;
	const float         layerMean3     = (nlayers > 2) ? layerMeans[2] : 0.;
	const float         layerMean4     = (nlayers > 3) ? layerMeans[3] : 0.;

        AthenaAttributeList attrList(*attrSpecification);

        // Set value of each row for the current channel
        attrList[ this->specificationName(eRxMean) ].setValue(p.rxMean());
        attrList[ this->specificationName(eAffectedCells1) ].setValue(affectedCells1);
        attrList[ this->specificationName(eAffectedCells2) ].setValue(affectedCells2);
        attrList[ this->specificationName(eAffectedCells3) ].setValue(affectedCells3);
        attrList[ this->specificationName(eAffectedCells4) ].setValue(affectedCells4);
        attrList[ this->specificationName(eLayerMean1) ].setValue(layerMean1);
        attrList[ this->specificationName(eLayerMean2) ].setValue(layerMean2);
        attrList[ this->specificationName(eLayerMean3) ].setValue(layerMean3);
        attrList[ this->specificationName(eLayerMean4) ].setValue(layerMean4);

        attrListCollection->add(channelId, attrList);
    }

    return (DataObject*) attrListCollection;
}

void L1CaloHVCorrectionsContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

    this->clear();

    std::map<std::string, CondAttrListCollection*>::const_iterator
      it_map(condAttrListCollectionMap.find(this->coolFolderKey(L1CaloHVCorrectionsContainer::eHVCorrections)));

    if(it_map!=condAttrListCollectionMap.end()) {

        CondAttrListCollection* attrListCollection = it_map->second;

        //loop over CondAttrListCollection
        CondAttrListCollection::const_iterator pos = attrListCollection->begin();
        CondAttrListCollection::const_iterator end = attrListCollection->end();
        for(; pos!= end; ++pos) {

            const coral::AttributeList& attrList = pos->second;

            // Get value of each row for the current channel
            CondAttrListCollection::ChanNum chanNum(pos->first);

            float rxMean = attrList[ this->specificationName(eRxMean) ].data<float>();
	    std::vector<int> affectedCells;
	    std::vector<float> layerMeans;
	    float layerMean1 = attrList[ this->specificationName(eLayerMean1) ].data<float>();
	    if (layerMean1 >= 1.) {
	      layerMeans.push_back(layerMean1);
	      affectedCells.push_back((int)attrList[ this->specificationName(eAffectedCells1) ].data<unsigned char>());
	      float layerMean2 = attrList[ this->specificationName(eLayerMean2) ].data<float>();
	      if (layerMean2 >= 1.) {
	        layerMeans.push_back(layerMean2);
	        affectedCells.push_back((int)attrList[ this->specificationName(eAffectedCells2) ].data<unsigned char>());
	        float layerMean3 = attrList[ this->specificationName(eLayerMean3) ].data<float>();
	        if (layerMean3 >= 1.) {
	          layerMeans.push_back(layerMean3);
	          affectedCells.push_back((int)attrList[ this->specificationName(eAffectedCells3) ].data<unsigned char>());
	          float layerMean4 = attrList[ this->specificationName(eLayerMean4) ].data<float>();
	          if (layerMean4 >= 1.) {
	            layerMeans.push_back(layerMean4);
	            affectedCells.push_back((int)attrList[ this->specificationName(eAffectedCells4) ].data<unsigned char>());
                  }
                }
              }
            }

            L1CaloHVCorrections l1CaloHVCorrections(chanNum, rxMean, affectedCells, layerMeans);
            m_mHVCorrectionsMap[chanNum] = l1CaloHVCorrections;
        }
    } else {
        std::cout << "L1CaloHVCorrectionsContainer : Could not find requested CondAttrListCollection "
                  << this->coolFolderKey(L1CaloHVCorrectionsContainer::eHVCorrections) << std::endl;
    }
}

const L1CaloHVCorrections* L1CaloHVCorrectionsContainer::hvCorrections(unsigned int channelId) const {
    L1CaloHVCorrectionsMap::const_iterator pos = m_mHVCorrectionsMap.find(channelId);
    if(pos==m_mHVCorrectionsMap.end()) return 0;
    else return &(pos->second);
}

const L1CaloHVCorrections* L1CaloHVCorrectionsContainer::hvCorrections(const L1CaloRxCoolChannelId& channelId) const {
    return this->hvCorrections(channelId.id());
}

void L1CaloHVCorrectionsContainer::addHVCorrections(unsigned int channelId, const L1CaloHVCorrections& hvCorrections) {
    m_mHVCorrectionsMap[channelId] = hvCorrections;
}

void L1CaloHVCorrectionsContainer::addHVCorrections(const L1CaloRxCoolChannelId& channelId, const L1CaloHVCorrections& hvCorrections) {
    this->addHVCorrections(channelId.id(), hvCorrections);
}

void L1CaloHVCorrectionsContainer::clear() {
    m_mHVCorrectionsMap.clear();
}
