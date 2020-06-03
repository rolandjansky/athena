/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// conditions
#include "PixelCalibAlgs/PixMapOverlayWriter.h"
#include "PixelConditionsData/SpecialPixelMap.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"

// geometry
#include "InDetIdentifier/PixelID.h"

// CoralDB
#include "CoralDB/CoralDB.h"
#include "CoralDB/Alias.h"
#include "CoralDB/Connection.h"

// Coral
#include "CoralBase/MessageStream.h"

PixMapOverlayWriter::PixMapOverlayWriter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_specialPixelMapSvc("SpecialPixelMapSvc", name),
  m_moduleStatus(1),
  m_connectionString("oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL"),
  m_connectivityTag("PIT-ALL-V39"),
  m_aliasTag("PIT-ALL-V39"),
  m_pixelID(0)
{
  declareProperty("ConnectionString", m_connectionString, "DB connection string"); 
  declareProperty("ConnectivityTag", m_connectivityTag, "Connectivity tag"); 
  declareProperty("AliasTag", m_aliasTag, "Alias tag"); 
  declareProperty("Modules", m_modules, "Module IDs to use for overlay");
  declareProperty("ModuleStatus", m_moduleStatus, "Module status to be used for specified modules");
  declareProperty("ModuleIDFragments", m_moduleIDFragments, "Beginning characters of module IDs");
  declareProperty("PP0s", m_pp0s, "PP0 IDs"); 
  declareProperty("RODs", m_rods, "ROD IDs"); 
  declareProperty("CoolingLoops", m_coolingLoops, "Cooling loop IDs");
  declareProperty("GenericObjects", m_genericObjects, "Generic object IDs");
  declareProperty("Frontends", m_frontends, "(module ID, frontend number) pairs"); 
  declareProperty("ColumnPairs", m_columnPairs, "(module ID, frontend number, column pair number) triplets"); 
  declareProperty("SpecialPixelMapSvc", m_specialPixelMapSvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

PixMapOverlayWriter::~PixMapOverlayWriter(){}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixMapOverlayWriter::initialize(){

  ATH_MSG_INFO( "Initializing PixMapOverlayWriter" );
   
  StatusCode sc = m_specialPixelMapSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve SpecialPixelMapSvc" );
    return StatusCode::FAILURE;
  } 
  
  sc = detStore()->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode PixMapOverlayWriter::execute(){

  auto spm = std::make_unique<DetectorSpecialPixelMap>();
  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
    spm->push_back(new ModuleSpecialPixelMap());
  }


  CoralDB::CoralDB* db = new CoralDB::CoralDB(m_connectionString, coral::ReadOnly, coral::Info, true);
    
  db->setObjectDictionaryTag("PIXEL"); // so far only tested for "PIXEL"
  db->setConnectivityTag(m_connectivityTag);
  db->setAliasTag(m_aliasTag);

  CoralDB::ObjectDictionaryMap objectDictionary;
  db->getObjectDictionary(objectDictionary, db->compoundTag());
    

  
  if(m_moduleIDFragments.size() != 0 || m_coolingLoops.size() != 0 || m_pp0s.size() != 0 
     || m_rods.size() != 0 || m_genericObjects.size() != 0 || m_frontends.size() != 0 
     || m_columnPairs.size() != 0){
    
    for(CoralDB::ObjectDictionaryMap::const_iterator object = objectDictionary.begin(); 
	object != objectDictionary.end(); ++object){

      for(std::vector<std::string>::const_iterator moduleIDFragment = m_moduleIDFragments.begin();
	  moduleIDFragment != m_moduleIDFragments.end(); ++moduleIDFragment){
	
	unsigned int fragmentSize = (*moduleIDFragment).size();
	
	if((*object).first.compare(0, fragmentSize, *moduleIDFragment, 0, fragmentSize) == 0){
	  if((*object).second == "MODULE"){
	    
	    m_modules.push_back(std::atoi(db->findAlias((*object).first, "HASHID").c_str()));
	  }
	}
      }
    }
    
    
    std::vector<std::string>* objectsToCheck = new std::vector<std::string>();
    
    for(std::vector<unsigned int>::const_iterator coolingLoop = m_coolingLoops.begin(); 
	coolingLoop != m_coolingLoops.end(); ++coolingLoop){
      
      std::ostringstream coolingLoopID;
      coolingLoopID << "cooling_loop" << *coolingLoop;

      try{
	if(db->objectType(coolingLoopID.str()) == "COOLING-LOOP"){
	  objectsToCheck->push_back(coolingLoopID.str());
	}
	else{
	  ATH_MSG_ERROR( coolingLoopID.str() << " is of type " << db->objectType(coolingLoopID.str()) 
			 << " (expected COOLING-LOOP), ignoring this object ID" );
	}
      }
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << coolingLoopID.str()
		       << ", ignoring this object ID" );
      }
    }
    
    for(std::vector<std::string>::const_iterator pp0 = m_pp0s.begin(); pp0 != m_pp0s.end(); ++pp0){

      try{
	if(db->objectType(*pp0) == "PP0"){
	  objectsToCheck->push_back(*pp0);
	}
	else{
	  ATH_MSG_ERROR( *pp0 << " is of type " << db->objectType(*pp0) 
			 << " (expected PP0), ignoring this object ID" );
	}
      }
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << *pp0 
		       << ", ignoring this object ID" );
      }
    }

    for(std::vector<std::string>::const_iterator rod = m_rods.begin(); rod != m_rods.end(); ++rod){
      
      try{
	if(db->objectType(*rod) == "RODBOC"){
	  objectsToCheck->push_back(*rod);
	}
	else{
	  ATH_MSG_ERROR( *rod << " is of type " << db->objectType(*rod) 
			 << " (expected RODBOC), ignoring this object ID" );
	}
      }
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << *rod 
		       << ", ignoring this object ID" );
      }
    }

    for(std::vector<std::string>::const_iterator genericObject = m_genericObjects.begin(); 
	genericObject != m_genericObjects.end(); ++genericObject){
      
      try{
	db->objectType(*genericObject);
	objectsToCheck->push_back(*genericObject);
      }
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << *genericObject
		       << ", ignoring this object ID" );
      }
    }
      
    while(!objectsToCheck->empty()){
      
      std::vector<std::string>* moreObjectsToCheck = new std::vector<std::string>();
      
      for(std::vector<std::string>::const_iterator object = objectsToCheck->begin();
	  object != objectsToCheck->end(); ++object){
	
	std::vector<CoralDB::Connection> connections = db->findAllConnections(*object);
	
	for(std::vector<CoralDB::Connection>::const_iterator connection = connections.begin();
	    connection != connections.end(); ++connection){
	  
	  if(db->objectType((*connection).toId()) == "MODULE"){
	    
	    m_modules.push_back(std::atoi(db->findAlias((*connection).toId(), "HASHID").c_str()));
	  }
	  else{
	    
	    moreObjectsToCheck->push_back((*connection).toId());
	  }
	}
      }
      
      delete objectsToCheck;
      objectsToCheck = moreObjectsToCheck;
    }
    
    delete objectsToCheck;



    for(std::map<std::string, std::vector<int> >::const_iterator module = m_frontends.begin();
	module != m_frontends.end(); ++module){

      unsigned int modid_parsed= std::atoi((module->first).c_str());

      IdentifierHash moduleHash = m_pixelID->wafer_hash(Identifier(modid_parsed));
      
      try{

	if( moduleHash < m_pixelID->wafer_hash_max() ){

	    for( std::vector<int>::const_iterator frontend = module->second.begin();
		 frontend != module->second.end(); ++frontend ){
	    
	      if( (*frontend) >= 0 && (*frontend) < 16 ){
		
		(*spm)[moduleHash]->setchipsPerModule( m_specialPixelMapSvc->getChips(moduleHash) );
		(*spm)[moduleHash]->setChipStatus( (*frontend), m_moduleStatus);

	      }
	      else{
		ATH_MSG_ERROR( "Frontend index out of range: " << module->second
			       << ", ignoring this frontend index" );
	      }
	    } //for
	  } //if 2
	  else{
	    ATH_MSG_ERROR( "Module hash out of range: " << moduleHash 
			   << ", ignoring this module hash" );
	  }
      }//try
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << module->first
		       << ", ignoring this object ID" );
      }
    }


    for(std::map<std::string, std::vector<int> >::const_iterator module = m_columnPairs.begin();
	module != m_columnPairs.end(); ++module){
      
      try{
	if(db->objectType(module->first) == "MODULE"){
	  
	  IdentifierHash moduleHash = 
	    m_pixelID->wafer_hash(Identifier(std::atoi(db->findAlias(module->first, "HASHID").c_str())));
	
	  if( moduleHash < m_pixelID->wafer_hash_max() ){
	    
	    for( unsigned int i = 0; i < module->second.size(); i += 2 ){
	      
	      if( module->second[i] >= 0 && module->second[i] < 16 ){
		
		if( module->second[i+1] >= 0 && module->second[i+1] < 9 ){
		  
		  (*spm)[moduleHash]->setColumnPairStatus(module->second[i], module->second[i+1], m_moduleStatus);
		}
		else{
		  ATH_MSG_ERROR( "Column pair index out of range: " << module->second[i+1]
				 << ", ignoring this column pair index" );
		}
	      }
	      else{
		ATH_MSG_ERROR( "Frontend index out of range: " << module->second[i]
			       << ", ignoring this frontend index" );
	      }
	    }
	  }
	  else{
	    ATH_MSG_ERROR( "Module hash out of range: " << moduleHash 
			   << ", ignoring this module hash" );
	  }
	}
	else{
	  ATH_MSG_ERROR( module->first << " is of type " << db->objectType(module->first) 
			 << " (expected MODULE), ignoring this object ID" );
	}
      }
      catch(...){
	ATH_MSG_ERROR( "Unable to look up type of object " << module->first
		       << ", ignoring this object ID" );
      }

    }
     
  }
 

  for(std::vector<unsigned int>::const_iterator module = m_modules.begin();
      module != m_modules.end(); ++module){

    if(m_pixelID->wafer_hash(Identifier(*module)) < m_pixelID->wafer_hash_max()){
      (*spm)[m_pixelID->wafer_hash(Identifier(*module))]->setModuleStatus(m_moduleStatus);
    }
    else{
      ATH_MSG_FATAL( "Invalid module ID: " << *module );
      return StatusCode::FAILURE;
    }
  }

  // Write table of modules
  ATH_MSG_INFO("HashID   IDentifier                   Module name         \tStatus" );
  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
    Identifier modid = m_pixelID->wafer_id(i);
    unsigned int mylabel = 0;
    if ( sizeof(modid.get_compact())==4 ) mylabel = modid.get_compact();
    else mylabel = (modid.get_compact()>>32);
    std::string modfullid = " "+m_pixelID->show_to_string(modid);
    std::ostringstream ss; ss<<mylabel; 
    std::string name("NONE");
    try { 
      name = db->findId(ss.str(),"HASHID");
    } 
    catch(...) {
      name = "NONAME";
    }
    const ModuleSpecialPixelMap *modspm = spm->module(i);
    std::string mystatus("");
    if ( modspm ) mystatus=modspm->to_string();
    else mystatus="Not found";
    ATH_MSG_INFO(" " << std::right << std::setw(4) << i << "    " << std::setw(10) << ss.str()+" " << std::left << std::setw(20) << modfullid << name << std::setw(0)<< '\t' << mystatus);
  } 
  delete db;
 
  DetectorSpecialPixelMap* spmFromDB = 0;

  StatusCode sc = detStore()->retrieve(spmFromDB,"PixMapOverlay");
    
  if( sc.isSuccess() ){ 
    ATH_MSG_INFO( "Merging special pixel map with reference map at " << "PixMapOverlay" );
    spm->merge(spmFromDB);
  } else {
    ATH_MSG_WARNING( "Unable to retrieve special pixel map from database" );
  }


  sc = m_specialPixelMapSvc->registerCondAttrListCollection(spm.get());
  
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to register CondAttrListCollection" );
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixMapOverlayWriter::finalize(){
  return StatusCode::SUCCESS;
}
