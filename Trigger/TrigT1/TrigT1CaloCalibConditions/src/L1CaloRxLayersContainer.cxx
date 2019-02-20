/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxLayersContainer.h"

#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <iostream>

L1CaloRxLayersContainer::L1CaloRxLayersContainer():AbstractL1CaloPersistentCondition("CondAttrListCollection") {
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eNLayers,  std::string("NLayers"),  std::string("unsigned char"));
    this->addSpecification(eName1,    std::string("Name1"),    std::string("unsigned char"));
    this->addSpecification(eName2,    std::string("Name2"),    std::string("unsigned char"));
    this->addSpecification(eName3,    std::string("Name3"),    std::string("unsigned char"));
    this->addSpecification(eName4,    std::string("Name4"),    std::string("unsigned char"));
    this->addSpecification(eNCells1,  std::string("NCells1"),  std::string("unsigned char"));
    this->addSpecification(eNCells2,  std::string("NCells2"),  std::string("unsigned char"));
    this->addSpecification(eNCells3,  std::string("NCells3"),  std::string("unsigned char"));
    this->addSpecification(eNCells4,  std::string("NCells4"),  std::string("unsigned char"));

    m_mCoolFoldersKeysMap[L1CaloRxLayersContainer::eRxLayers] = std::string("/TRIGGER/L1Calo/V1/Results/RxLayers");
}

L1CaloRxLayersContainer::L1CaloRxLayersContainer(const std::map<L1CaloRxLayersContainer::eCoolFolders, std::string>& folderKeysMap) : 
  AbstractL1CaloPersistentCondition("CondAttrListCollection"),
  m_mCoolFoldersKeysMap(folderKeysMap)
{
    // Define DB rows names and types in order to construct the AttributeListSpecification object
    this->addSpecification(eNLayers,  std::string("NLayers"),  std::string("unsigned char"));
    this->addSpecification(eName1,    std::string("Name1"),    std::string("unsigned char"));
    this->addSpecification(eName2,    std::string("Name2"),    std::string("unsigned char"));
    this->addSpecification(eName3,    std::string("Name3"),    std::string("unsigned char"));
    this->addSpecification(eName4,    std::string("Name4"),    std::string("unsigned char"));
    this->addSpecification(eNCells1,  std::string("NCells1"),  std::string("unsigned char"));
    this->addSpecification(eNCells2,  std::string("NCells2"),  std::string("unsigned char"));
    this->addSpecification(eNCells3,  std::string("NCells3"),  std::string("unsigned char"));
    this->addSpecification(eNCells4,  std::string("NCells4"),  std::string("unsigned char"));
}

std::string L1CaloRxLayersContainer::coolFolderKey(L1CaloRxLayersContainer::eCoolFolders efolder) const {

    std::map<L1CaloRxLayersContainer::eCoolFolders, std::string>::const_iterator it = m_mCoolFoldersKeysMap.find(efolder);
    if(it!=m_mCoolFoldersKeysMap.end()) {
        return it->second;
    } else {
        return std::string("");
    }
}

std::vector<std::string> L1CaloRxLayersContainer::coolInputKeys() const {
    std::vector<std::string> v;
    v.push_back(this->coolFolderKey(L1CaloRxLayersContainer::eRxLayers));
    return v;
}

std::string L1CaloRxLayersContainer::coolOutputKey() const {
    return this->coolFolderKey(L1CaloRxLayersContainer::eRxLayers);
}

DataObject* L1CaloRxLayersContainer::makePersistent() const {
    if(m_mRxLayersMap.size()==0) return 0;

    // Create AttributeListSpecification according to the attributes to be recorded
    // this is required to create the AttributeList objs
    coral::AttributeListSpecification* attrSpecification = this->createAttributeListSpecification();
    if(!attrSpecification->size()) return NULL;

    CondAttrListCollection* attrListCollection = new CondAttrListCollection(true);

    L1CaloRxLayersMap::const_iterator pos = m_mRxLayersMap.begin();
    L1CaloRxLayersMap::const_iterator end = m_mRxLayersMap.end();
    for(; pos!=end; ++pos) {
        const unsigned int channelId = pos->first;
        const L1CaloRxLayers&   p = pos->second;
	const std::vector<int>& names(p.names());
	const std::vector<int>& ncells(p.ncells());
	const unsigned char nlayers = names.size();
	const unsigned char name1   = (nlayers > 0) ? names[0]  : 0;
	const unsigned char name2   = (nlayers > 1) ? names[1]  : 0;
	const unsigned char name3   = (nlayers > 2) ? names[2]  : 0;
	const unsigned char name4   = (nlayers > 3) ? names[3]  : 0;
	const unsigned char ncells1 = (nlayers > 0) ? ncells[0] : 0;
	const unsigned char ncells2 = (nlayers > 1) ? ncells[1] : 0;
	const unsigned char ncells3 = (nlayers > 2) ? ncells[2] : 0;
	const unsigned char ncells4 = (nlayers > 3) ? ncells[3] : 0;

        AthenaAttributeList attrList(*attrSpecification);

        // Set value of each row for the current channel
        attrList[ this->specificationName(eNLayers) ].setValue(nlayers);
        attrList[ this->specificationName(eName1) ].setValue(name1);
        attrList[ this->specificationName(eName2) ].setValue(name2);
        attrList[ this->specificationName(eName3) ].setValue(name3);
        attrList[ this->specificationName(eName4) ].setValue(name4);
        attrList[ this->specificationName(eNCells1) ].setValue(ncells1);
        attrList[ this->specificationName(eNCells2) ].setValue(ncells2);
        attrList[ this->specificationName(eNCells3) ].setValue(ncells3);
        attrList[ this->specificationName(eNCells4) ].setValue(ncells4);

        attrListCollection->add(channelId, attrList);
    }

    return (DataObject*) attrListCollection;
}

void L1CaloRxLayersContainer::makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap) {

    this->clear();

    std::map<std::string, CondAttrListCollection*>::const_iterator
      it_map(condAttrListCollectionMap.find(this->coolFolderKey(L1CaloRxLayersContainer::eRxLayers)));

    if(it_map!=condAttrListCollectionMap.end()) {

        CondAttrListCollection* attrListCollection = it_map->second;

        //loop over CondAttrListCollection
        CondAttrListCollection::const_iterator pos = attrListCollection->begin();
        CondAttrListCollection::const_iterator end = attrListCollection->end();
        for(; pos!= end; ++pos) {

            const coral::AttributeList& attrList = pos->second;

            // Get value of each row for the current channel
            CondAttrListCollection::ChanNum chanNum(pos->first);

            unsigned char nlayers = attrList[ this->specificationName(eNLayers) ].data<unsigned char>();
	    std::vector<int> names;
	    std::vector<int> ncells;
	    if (nlayers > 0) {
	      names.push_back((int)attrList[ this->specificationName(eName1) ].data<unsigned char>());
	      ncells.push_back((int)attrList[ this->specificationName(eNCells1) ].data<unsigned char>());
            }
	    if (nlayers > 1) {
	      names.push_back((int)attrList[ this->specificationName(eName2) ].data<unsigned char>());
	      ncells.push_back((int)attrList[ this->specificationName(eNCells2) ].data<unsigned char>());
            }
	    if (nlayers > 2) {
	      names.push_back((int)attrList[ this->specificationName(eName3) ].data<unsigned char>());
	      ncells.push_back((int)attrList[ this->specificationName(eNCells3) ].data<unsigned char>());
            }
	    if (nlayers > 3) {
	      names.push_back((int)attrList[ this->specificationName(eName4) ].data<unsigned char>());
	      ncells.push_back((int)attrList[ this->specificationName(eNCells4) ].data<unsigned char>());
            }

            L1CaloRxLayers l1CaloRxLayers(chanNum, names, ncells);
            m_mRxLayersMap[chanNum] = l1CaloRxLayers;
        }
    } else {
        std::cout << "L1CaloRxLayersContainer : Could not find requested CondAttrListCollection "
                  << this->coolFolderKey(L1CaloRxLayersContainer::eRxLayers) << std::endl;
    }
}

const L1CaloRxLayers* L1CaloRxLayersContainer::rxLayers(unsigned int channelId) const {
    L1CaloRxLayersMap::const_iterator pos = m_mRxLayersMap.find(channelId);
    if(pos==m_mRxLayersMap.end()) return 0;
    else return &(pos->second);
}

const L1CaloRxLayers* L1CaloRxLayersContainer::rxLayers(const L1CaloRxCoolChannelId& channelId) const {
    return this->rxLayers(channelId.id());
}

void L1CaloRxLayersContainer::addRxLayers(unsigned int channelId, const L1CaloRxLayers& rxLayers) {
    m_mRxLayersMap[channelId] = rxLayers;
}

void L1CaloRxLayersContainer::addRxLayers(const L1CaloRxCoolChannelId& channelId, const L1CaloRxLayers& rxLayers) {
    this->addRxLayers(channelId.id(), rxLayers);
}

void L1CaloRxLayersContainer::clear() {
    m_mRxLayersMap.clear();
}
