/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

void L1CaloPprDisabledChannelContainer::makeTransient(const std::map<std::string, const CondAttrListCollection*>& condAttrListCollectionMap) {

  CondAttrListCollection::const_iterator it_attr;
  CondAttrListCollection::const_iterator it_attrE;

  if (condAttrListCollectionMap.empty()) return;

  // Reading cool paths and reading attributes 
  
  for (const auto& [name, coll] : condAttrListCollectionMap) {
    if (name.find("PpmDeadChannels")!=std::string::npos){
      auto it_deadChannelsAttrListCollection = condAttrListCollectionMap.find(name);
      const CondAttrListCollection* deadChannelsAttrListCollection = it_deadChannelsAttrListCollection->second;

      
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

      
      
    } // End PpmDeadChannels
    
    if(name.find("PprChanCalib")!=std::string::npos){
      auto it_calibChannelsAttrListCollection = condAttrListCollectionMap.find(name);
      const CondAttrListCollection* calibChannelsAttrListCollection =  it_calibChannelsAttrListCollection->second;


      
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
      
      
    

    } // End PprChanCalib

    if(name.find("DisabledTowers")!=std::string::npos){
      auto it_disabledTowersAttrListCollection = condAttrListCollectionMap.find(name);
      const CondAttrListCollection* disabledTowersAttrListCollection = it_disabledTowersAttrListCollection->second;
      
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
      
      

      
    } // End DisabledTowers
    
  } // End cool loop 
  
  




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
