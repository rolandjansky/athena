/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DPDUtils/DPDTagTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"

#include "AthenaPoolUtilities/TagMetadataKey.h"
#include "TagEvent/DPDAttributeNames.h"

#include "EventBookkeeperMetaData/SkimDecisionCollection.h"
#include "DBDataModel/CollectionMetadata.h"
#include <string>


DPDTagTool::DPDTagTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_pMetaDataStore("StoreGateSvc/MetaDataStore",name) {
  declareProperty("DESDSkimDecisionCollection", m_desdskimdecisioncontainer="StreamESD_SkimDecisionsContainer");
  //declareProperty("DAODSkimDecisionCollection", m_daodskimdecisioncontainer="StreamAOD_SkimDecisionsContainer");

  declareInterface<DPDTagTool>(this);

}

DPDTagTool::~DPDTagTool() {}


StatusCode DPDTagTool::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return (StatusCode::FAILURE);
  }

  sc = m_pMetaDataStore.retrieve();
  if (sc.isFailure() || 0 == m_pMetaDataStore ) {
     ATH_MSG_ERROR("Could not find metadata store, MetaDataStore");
     return(StatusCode::FAILURE);
  }

  return sc;

}


StatusCode DPDTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int) {

  ATH_MSG_DEBUG("in attributeSpecification()");

  // Build the attribute list - DAODStreams and DESDStreams
  // attrMap[ DPDAttributeNames[DPDId::Daod] ] =AthenaAttributeType(std::string("unsigned int"),  DPDAttributeUnitNames[DPDId::Daod], DPDAttributeGroupNames[DPDId::Daod]);

  attrMap[ DPDAttributeNames[DPDId::Desd] ] =AthenaAttributeType(std::string("unsigned int"),  DPDAttributeUnitNames[DPDId::Desd], DPDAttributeGroupNames[DPDId::Desd]);


  return StatusCode::SUCCESS;

}

StatusCode DPDTagTool::execute(TagFragmentCollection& dpdTagColl, const int){


  ATH_MSG_DEBUG("in execute()");
  // get bit mappings
  DPDBits bits;

  // Get DESD container
  const SkimDecisionCollection* aSkimDecisionContainer(0) ;
  sc = m_storeGate->retrieve(aSkimDecisionContainer, m_desdskimdecisioncontainer);
  if (!sc.isSuccess() || (0 == aSkimDecisionContainer) ) {
    ATH_MSG_WARNING("Could not find DESD SkimDecisionContainer " << m_desdskimdecisioncontainer);
    return(StatusCode::SUCCESS);
  }

  //Fill DESD 
  unsigned int tempDesd = 0x0;
  ATH_MSG_DEBUG("DESD map has " << bits.desdMap.size() << "elements");
  std::map<const std::string, unsigned int>::iterator map_it;
  for (map_it = bits.desdMap.begin(); map_it != bits.desdMap.end(); ++map_it){
    SkimDecisionCollection::const_iterator it = aSkimDecisionContainer->begin();
    SkimDecisionCollection::const_iterator it_end = aSkimDecisionContainer->end();
    for( ; it != it_end; ++it ) {
      ATH_MSG_DEBUG("Skim Name: " << (*it)->getName() << ", value : " << (*it)->isAccepted());
      if ( ((*map_it).first == (*it)->getName()) && (*it)->isAccepted() ) {
        tempDesd |= 1 << (*map_it).second;
      }
    }
  }

  dpdTagColl.insert(DPDAttributeNames[DPDId::Desd],tempDesd);

  // Get DAOD container
  // StreamAOD_SkimDecisionsContainer is removed from AOD (DAODWord removed from autumn 2010 reprocessing)
/*
  const SkimDecisionCollection* aodSkimDecisionContainer(0) ;
  sc = m_storeGate->retrieve(aodSkimDecisionContainer, m_daodskimdecisioncontainer);
  if (!sc.isSuccess() || (0 == aodSkimDecisionContainer) ) {
    ATH_MSG_WARNING("Could not find DAOD SkimDecisionContainer " << m_daodskimdecisioncontainer);
    return(StatusCode::SUCCESS);
  }

  //Fill DAOD 
  unsigned int tempDaod = 0x0;
  ATH_MSG_DEBUG("DAOD map has " << bits.daodMap.size() << "elements"); 
  for (map_it = bits.daodMap.begin(); map_it != bits.daodMap.end(); ++map_it){
    SkimDecisionCollection::const_iterator it = aodSkimDecisionContainer->begin();
    SkimDecisionCollection::const_iterator it_end = aodSkimDecisionContainer->end();
    for( ; it != it_end; ++it ) {
      ATH_MSG_DEBUG("DAOD Skim Name: " << (*it)->getName() << ", value : " << (*it)->isAccepted());
      if ( ((*map_it).first == (*it)->getName()) && (*it)->isAccepted() ) {
        tempDaod |= 1 << (*map_it).second;
      }
    }
  }


  // Finally write the DAODStream to the TAG file
  dpdTagColl.insert(DPDAttributeNames[DPDId::Daod],tempDaod);
*/

  return StatusCode::SUCCESS;

}

StatusCode DPDTagTool::finalize() {

  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "In finalize()" << endreq;

  return StatusCode::SUCCESS;

}

StatusCode DPDTagTool::stop() {

  ATH_MSG_INFO("stop method");

  DPDBits bits;
  // std::map<const std::string, unsigned int> combMap = bits.daodMap;  
  std::map<const std::string, unsigned int> combMap;
  std::map<std::string,unsigned int>::iterator iter;
  for( iter = bits.desdMap.begin(); iter != bits.desdMap.end(); ++iter ) {
     combMap.insert( make_pair(iter->first, iter->second ) );
  }

  // Add DAOD & DESD bit map to metadata 
  std::map<std::string,std::string> bitmap;
  for (std::map<const std::string, unsigned int>::iterator it = combMap.begin(); it != combMap.end(); ++it) {
    std::stringstream bitname;
    //std::string ns = "DAOD";
    std::string ns;
    if ( (*it).first.compare(0,16,"Other_StreamDESD") == 0 ) ns = "DESD";
    TagMetadataKey tkey;
    tkey.setKey(ns+"_StreamBit");
    bitname << "(b=" << (*it).second << ")";
    tkey.setQual(bitname.str());
    bitmap.insert(std::make_pair(tkey.toString(), (*it).first));
  }
 
  CollectionMetadataContainer* bitmaplist = new CollectionMetadataContainer();
  CollectionMetadata* newmap = new CollectionMetadata(bitmap); 
  bitmaplist->push_back(newmap);

  sc = m_pMetaDataStore->record(bitmaplist,"DPDTagTool");
  if (!sc.isSuccess()) {
      //ATH_MSG_ERROR("Unable to store DAOD & DESD maps to CollectionMetadataContainer" );
      ATH_MSG_ERROR("Unable to store DESD maps to CollectionMetadataContainer" );
  } else {
      //ATH_MSG_INFO( "DAOD & DESD maps copied to MetaDataStore");
      ATH_MSG_INFO( "DESD maps copied to MetaDataStore");
      ATH_MSG_INFO("Size: " << newmap->size());
      ATH_MSG_INFO("Contents: ");
      CollectionMetadata::const_iterator i = newmap->begin();
      for (CollectionMetadata::const_iterator j=i; j != newmap->end(); ++j) {
         ATH_MSG_INFO("    "<<j->first<<" "<<j->second);
      }
  }

  return StatusCode::SUCCESS; 

}

