/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SpecialPixelMapSvc.h"

// Athena
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

// coral
#include "CoralBase/Attribute.h" 
#include "CoralBase/AttributeListSpecification.h" 
#include "CoralBase/MessageStream.h"
#include "CoralBase/Blob.h"

// CoralDB
#include "CoralDB/CoralDB.h"
#include "CoralDB/Alias.h"
#include "CoralDB/Connection.h"

// STL
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

// geometry
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelGeoModel/IBLParameterSvc.h"
//Includes related to determining presence of ITK
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"


static bool isIBL(false);

SpecialPixelMapSvc::SpecialPixelMapSvc(const std::string& name, ISvcLocator* sl) :
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_iovDbSvc("IOVDbSvc", name),
  m_overlayFolder(""),
  m_overlayLongFolder(""),
  m_overlayKey("PixMapOverlay"),
  m_overlayLongKey("PixMapOverlayLong"),
  m_moduleLevelOverlay(false),
  m_dataSource("None"),
  m_outputFolder(""),
  m_outputLongFolder(""),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_geoModelSvc("GeoModelSvc",name),
  m_registerCallback(true),
  m_verbosePixelID(true),
  m_binaryPixelStatus(true),
  m_markSpecialRegions(true),
  m_mergePixelMaps(true),
  m_differentialUpdates(false),
  m_useDualFolderStructure(false),
  m_maskLayers(false),
  m_writeBlobs(false),
  m_forceNewDBContent(false),
  m_connectionString("oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL"),
  m_connectivityTag("PIT-ALL-V39"),
  m_aliasTag("PIT-ALL-V39"),
  m_fileListFileName("filelist"),
  m_fileListFileDir("filelistdir"),
  m_killingModule(0.),
  m_pixelID(0),
  m_pixman(0), 
  m_dummy(0) 
{
  declareProperty("DBFolders", m_condAttrListCollectionKeys, "list of database folders to be accessed"); 
  declareProperty("SpecialPixelMapKeys", m_specialPixelMapKeys, "StoreGate keys at which pixel maps are to be stored"); 
  declareProperty("OverlayFolder", m_overlayFolder, "Name of overlay DB folder"); 
  declareProperty("OverlayLongFolder", m_overlayLongFolder, "Name of long overlay DB folder"); 
  declareProperty("OverlayKey", m_overlayKey, "StoreGate key for overlay map"); 
  declareProperty("OverlayLongKey", m_overlayLongKey, "StoreGate key for long overlay map"); 
  declareProperty("ModuleLevelOverlay", m_moduleLevelOverlay, "Apply module-level overlay if true, otherwise pixel-level"); 
  declareProperty("DataSource", m_dataSource, "source of pixel map data used in create(): Textfiles, Database or None");
  declareProperty("OutputFolder", m_outputFolder, "Name of output folder");
  declareProperty("OutputLongFolder", m_outputLongFolder, "Name of output folder for long maps");
  declareProperty("IBLParameterService", m_IBLParameterSvc);
  declareProperty("GeoModelService", m_geoModelSvc);
  declareProperty("ModuleIDsForPrinting", m_moduleIDsForPrinting, "IDs or IDhash(IBL) of modules that printed in print()");
  declareProperty("RegisterCallback", m_registerCallback, "switch registration of callback on/off");
  declareProperty("PrintVerbosePixelID",m_verbosePixelID, "print (chip/column/row) (\"verbose\") or unsigned int");
  declareProperty("PrintBinaryPixelStatus",m_binaryPixelStatus, "print status in binary format or as unsigned int");
  declareProperty("MarkSpecialRegions",m_markSpecialRegions, "switch marking on/off; does not affect already marked regions");
  declareProperty("MergePixelMaps", m_mergePixelMaps, 
		  "if true, a merged map is stored in place of the first map in \"SpecialPixelMapKeys\"");
  declareProperty("DifferentialUpdates", m_differentialUpdates, "Switch for differential updates");
  declareProperty("UseDualFolderStructure", m_useDualFolderStructure, 
		  "Use dual folder structure when creating a CondAttrListCollection");
  declareProperty("MaskLayers",  m_maskLayers, "Mask full layers/disks in overlay" );
  declareProperty("WriteBlobs", m_writeBlobs, "Switch between blob and clob writing");
  declareProperty("ForceNewDBContent",m_forceNewDBContent);
  declareProperty("ConnectionString", m_connectionString, "Connection string for connectivity DB (used when reading text files)"); 
  declareProperty("ConnectivityTag", m_connectivityTag, "Connectivity tag"); 
  declareProperty("AliasTag", m_aliasTag, "Alias tag"); 
  declareProperty("FileList", m_fileListFileName, "list of text files"); 
  declareProperty("FileListDir", m_fileListFileDir, "directory of list of text files");
  declareProperty("KillingModules", m_killingModule, "Probability of Killing module");
  declareProperty("LayersToMask", m_layersToMask, "Which barrel layers to mask out, goes from 0 to N-1");
  declareProperty("DisksToMask", m_disksToMask, "Which endcap disks to mask out, goes from -N+1 to N+1 , skipping zero");
  declareProperty("MakingDummyMaps", m_dummy, "Making dummy run2 maps from Run1 Pixel maps");

}


SpecialPixelMapSvc::~SpecialPixelMapSvc(){}


StatusCode SpecialPixelMapSvc::queryInterface(const InterfaceID& riid, void** ppvIf){
  if(interfaceID() == riid){
    *ppvIf = dynamic_cast< SpecialPixelMapSvc* > (this);
  } 
  else if(ISpecialPixelMapSvc::interfaceID() == riid){
    *ppvIf = dynamic_cast<ISpecialPixelMapSvc*> (this);
  } 
  else{
    return AthService::queryInterface(riid, ppvIf);
  }

  addRef();
  return StatusCode::SUCCESS;
}


StatusCode SpecialPixelMapSvc::initialize()
{
  ATH_MSG_INFO( "Initializing SpecialPixelMapSvc" );
  StatusCode sc = setProperties();
  if(sc.isFailure()){
    ATH_MSG_FATAL( "Unable to set properties" );
    return StatusCode::FAILURE;
  }
  if (m_IBLParameterSvc.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Could not retrieve IBLParameterSvc" << endmsg;
  }
  else {
    bool useSpecialPixelMap=true;
    m_IBLParameterSvc->setBoolParameters(useSpecialPixelMap,"EnableSpecialPixels");
    if (!useSpecialPixelMap) return StatusCode::SUCCESS; //to prevent error during initialization
  }
  
  ModuleSpecialPixelMap::m_markSpecialRegions = m_markSpecialRegions;
  // Retrieve Detector Store
  sc = m_detStore.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve detector store" );
    return StatusCode::FAILURE;
  }
  
  sc = m_iovDbSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve IOVDbSvc" );
    return StatusCode::FAILURE;
  }
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 
  sc = m_detStore->retrieve( m_pixman, "Pixel" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID manager" );
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }
  itermin = m_pixman->getDetectorElementBegin(); 
  itermax = m_pixman->getDetectorElementEnd();
  // 
  // determine if ITK is present
  if (m_geoModelSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endmsg;
    return (StatusCode::FAILURE);
  }
  if(m_geoModelSvc->geoConfig()!=GeoModel::GEO_RUN1 && m_geoModelSvc->geoConfig()!=GeoModel::GEO_TESTBEAM)isIBL=true; 
  if(!isIBL)isIBL = m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN1 && m_pixelID->wafer_hash_max()==2048; // second chance !
  m_chips.clear(); 
  ATH_MSG_DEBUG( "ModuleHashMax: "<<m_pixelID->wafer_hash_max()<<" isIBL "<<isIBL );
  // Check all detector elements in the present geometry setup 
  for( iter=itermin; iter !=itermax; ++iter){ 
    const InDetDD::SiDetectorElement* element = *iter; 
    if(element !=0){ 
      Identifier ident = element->identify(); 
      if(m_pixelID->is_pixel(ident)){  // OK this Element is included 
	const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
	if(!design)continue;
	unsigned int mchips = design->numberOfCircuits();
	int mrow =design->rows();  
	if(mchips==8||abs(m_pixelID->barrel_ec(ident))==2||(m_pixelID->barrel_ec(ident)==0&&m_pixelID->layer_disk(ident)>0)){
	  mchips *=2; // guess numberOfCircuits()
	  mrow /=2; // number of chips in row 
	} 
	unsigned int i(0);
	if(mrow==336)i = 1;
	else if(mrow==672) i = 2;
	else if(mrow==339) i = 3;
	else if(mrow==678) i = 4;
	unsigned int idsh = m_pixelID->wafer_hash(ident);
	m_chips[idsh]=mchips*10 + i; 
	//m_calibobjs.push_back(std::make_pair(ident,mchips)); 
	// write up the dump calibration here with default value:
      }
    }
  }

  //Register callback to update the DetectorSpecialPixelMap in the detector store
  //whenever the CondAttrListCollection is updated from the Database
  if(m_registerCallback){
    for(std::vector<std::string>::const_iterator key = m_condAttrListCollectionKeys.begin();
	key !=  m_condAttrListCollectionKeys.end(); key++){
      ATH_MSG_DEBUG( "IOVCALLBACK for key " << *key );
      const DataHandle<CondAttrListCollection> attrListColl;
      if( (m_detStore->regFcn(&ISpecialPixelMapSvc::IOVCallBack, dynamic_cast<ISpecialPixelMapSvc*>(this),
			      attrListColl, *key)).isFailure()){ 
	ATH_MSG_FATAL( "Unable to register callback for folder " << *key );
	return StatusCode::FAILURE; 
      }
    }
  }

  if (m_maskLayers &&  !m_layersToMask.size() && !m_disksToMask.size()){
    ATH_MSG_DEBUG( "Layer/Disk masking enabled, but no layer/disk specified!" );
    m_maskLayers = false;
  }

  if (!m_maskLayers &&  (m_layersToMask.size() || m_disksToMask.size())){
    ATH_MSG_DEBUG( "Layer/Disk to mask specified, but masking is disabled!" );
  } 

  if (m_maskLayers &&  m_disksToMask.size() && (std::find(m_disksToMask.begin(), m_disksToMask.end(),0)!=m_disksToMask.end())){
    ATH_MSG_WARNING( "0th Disk not defined (-N to N) - check your setup!" );
  }   

  return StatusCode::SUCCESS;
}

StatusCode SpecialPixelMapSvc::finalize(){
  ATH_MSG_DEBUG( "Finalizing SpecialPixelMapSvc" );
  return Service::finalize(); 
}


StatusCode SpecialPixelMapSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)){
  StatusCode sc = StatusCode::SUCCESS;

  //
  std::list<std::string>::const_iterator key; 

  for(key=keys.begin(); key != keys.end(); ++key){ 
    ATH_MSG_INFO( "IOVCALLBACK for key " << *key << " number " << I );
    
    if(m_condAttrListCollectionKeys.size() != m_specialPixelMapKeys.size()){
      ATH_MSG_ERROR( "DBFolders/Keys size mismatch" );
      return StatusCode::FAILURE;
    }

    unsigned int index = 0;

    while( index < m_condAttrListCollectionKeys.size() && *key != m_condAttrListCollectionKeys[index] ){
      index++;
    } 

    if( *key != m_condAttrListCollectionKeys[index] ){
      ATH_MSG_FATAL( "Callback key does not match any string in DBFolders" );
      return StatusCode::FAILURE;
    }
    
    DetectorSpecialPixelMap* spm = 0;
    
    if(m_detStore->contains<DetectorSpecialPixelMap>(m_specialPixelMapKeys[index])){
      sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[index]);
      if(sc.isSuccess()){
	ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at " 
		       << m_specialPixelMapKeys[index] << " retrieved" );
      }
      else{
	ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[index] );
	return StatusCode::FAILURE;
      }

      sc = m_detStore->remove(spm);
      
      if(sc.isSuccess()){
	ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[index] << " removed" );
      }
      else{
	ATH_MSG_FATAL( "Unable to remove old DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[index] );
	return StatusCode::FAILURE;
      }
    }
    
    // construct a new DetectorSpecialPixelMap
    sc = createFromDetectorStore(*key, m_specialPixelMapKeys[index]);

    if(sc.isSuccess()){
      ATH_MSG_DEBUG( "New DetectorSpecialPixelMap recorded at " 
		     << m_specialPixelMapKeys[index] );
    }
    else{
      ATH_MSG_ERROR( "Unable to record new DetectorSpecialPixelMap at "
		     << m_specialPixelMapKeys[index] );
      return StatusCode::FAILURE;
    }
    // m_iovDbSvc->dropObject(*key,false); // commented bug 55586
  }
  
  
  // update merged special pixel map
  
  if(m_mergePixelMaps || !m_overlayFolder.empty()){
    
    DetectorSpecialPixelMap* spm = 0;
    
    // if first map in list has not yet been rebuilt, rebuild it now
    
    if(std::find(keys.begin(), keys.end(), m_condAttrListCollectionKeys[0]) == keys.end()){  //already checked for size 
    
      if(m_detStore->contains<DetectorSpecialPixelMap>(m_specialPixelMapKeys[0])){
	sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[0]);
	if(sc.isSuccess()){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at " 
			 << m_specialPixelMapKeys[0] << " retrieved" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at "
			 << m_specialPixelMapKeys[0] );
	  return StatusCode::FAILURE;
	}
      
	sc = m_detStore->remove(spm);

	if(sc.isSuccess()){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at "
			 << m_specialPixelMapKeys[0] << " removed" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to remove old DetectorSpecialPixelMap at "
			 << m_specialPixelMapKeys[0] );
	  return StatusCode::FAILURE;
	}
      }
    
      // construct a new DetectorSpecialPixelMap
      sc = createFromDetectorStore(m_condAttrListCollectionKeys[0], m_specialPixelMapKeys[0]);

      if(sc.isSuccess()){
	ATH_MSG_DEBUG( "New DetectorSpecialPixelMap recorded at " 
		       << m_specialPixelMapKeys[0] );
      }
      else{
	ATH_MSG_ERROR( "Unable to record new DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[0] );
	return StatusCode::FAILURE;
      }
    }
  }  
   
  // merge additional maps

  if( m_mergePixelMaps ){
    
    DetectorSpecialPixelMap* spm = 0;
    
    if(m_detStore->contains<DetectorSpecialPixelMap>(m_specialPixelMapKeys[0])){
      sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[0]);
      if(sc.isSuccess()){
	ATH_MSG_DEBUG( "DetectorSpecialPixelMap at " 
		       << m_specialPixelMapKeys[0] << " retrieved" );
      }
      else{
	ATH_MSG_FATAL( "Unable to retrieve DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[0] );
	return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_FATAL( "No base DetectorSpecialPixelMap present in detector store " );
      return StatusCode::FAILURE;
    }
    
    DetectorSpecialPixelMap* spmMod = 0;    
    
    for(unsigned int i = 1; i < m_specialPixelMapKeys.size(); i++){
      if(m_detStore->contains<DetectorSpecialPixelMap>(m_specialPixelMapKeys[i])){
	sc = m_detStore->retrieve(spmMod, m_specialPixelMapKeys[i]);
	if(sc.isSuccess()){
	  ATH_MSG_DEBUG( "DetectorSpecialPixelMap at " 
			 << m_specialPixelMapKeys[i] << " retrieved" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to retrieve DetectorSpecialPixelMap at "
			 << m_specialPixelMapKeys[i] );
	  return StatusCode::FAILURE;
	}
	
	spm->merge(spmMod);
      }
      else{
	ATH_MSG_WARNING( "No DetectorSpecialPixelMap present in detector store at "
			 << m_specialPixelMapKeys[i] );
      }
    }
  }


  // apply overlay
  
  if( !m_overlayFolder.empty() ){
    
    DetectorSpecialPixelMap* overlay = 0;
    
    if(m_detStore->contains<DetectorSpecialPixelMap>(m_overlayKey)){
      sc = m_detStore->retrieve(overlay, m_overlayKey);
      if( sc.isSuccess() ){
	ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at " << m_overlayKey
		       << " retrieved" );
      }
      else{
	ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at "
		       << m_overlayKey );
	return StatusCode::FAILURE;
      }
      
      sc = m_detStore->remove(overlay);

      if( sc.isSuccess() ){
	ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at "
		       << m_overlayKey << " removed" );
      }
      else{
	ATH_MSG_FATAL( "Unable to remove old DetectorSpecialPixelMap at "
		       << m_overlayKey );
	return StatusCode::FAILURE;
      }
    }
    
    // fill overlay map
    
    sc = createFromDetectorStore(m_overlayFolder, m_overlayKey, !m_moduleLevelOverlay);
    
    if( sc.isSuccess() ){
      ATH_MSG_DEBUG( "New DetectorSpecialPixelMap recorded at " 
		     << m_overlayKey );
    }
    else{
      ATH_MSG_ERROR( "Unable to record new DetectorSpecialPixelMap at "
		     << m_overlayKey );
      return StatusCode::FAILURE;
    }
    
    sc = m_detStore->retrieve(overlay, m_overlayKey);
    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to retrieve DetectorSpecialPixelMap at " 
		     << m_overlayKey );
      return sc;
    }

    if(m_maskLayers){
      
      DetectorSpecialPixelMap* overlay = 0;
    
      if(m_detStore->contains<DetectorSpecialPixelMap>(m_overlayKey)){
	sc = m_detStore->retrieve(overlay, m_overlayKey);
	if( sc.isSuccess() ){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at " << m_overlayKey
			 << " retrieved" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at "
			 << m_overlayKey );
	  return StatusCode::FAILURE;
	}
      }

      if (overlay){
	
      DetectorSpecialPixelMap* maskOverlay = 0;	

      if(m_detStore->contains<DetectorSpecialPixelMap>("MaskingOverlay")){
	sc = m_detStore->retrieve(maskOverlay, "MaskingOverlay");// using "internal" key
	if( sc.isSuccess() ){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at MaskingOverlay retrieved" );
	  sc = m_detStore->remove(maskOverlay);//remove old overlay to be replaced with new one in det store
	  if(!sc.isSuccess()) {ATH_MSG_FATAL( "Unable to remove old Masking Overlay!" );return StatusCode::FAILURE;}
	}
	else{
	  ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at MaskingOverlay" );
	  return StatusCode::FAILURE;
	}
      }

       sc = createMaskingOverlay("MaskingOverlay");

      if(!sc.isSuccess()) {ATH_MSG_FATAL( "Unable to create new Masking Overlay!" );return StatusCode::FAILURE;}
      
      sc = m_detStore->retrieve(maskOverlay, "MaskingOverlay");//get newly created masking overlay back from detector store

      if(!sc.isSuccess()) {ATH_MSG_FATAL( "Unable to retrieve new Masking Overlay!" );return StatusCode::FAILURE;}
      
       overlay->merge(maskOverlay);//an old overlay exists, merge the new one with it	  

	}

      else{
	sc = createMaskingOverlay(m_overlayKey);//no old overlay exists, just add the new one with the expected key

	if(!sc.isSuccess()) {ATH_MSG_FATAL( "Unable to create new Masking Overlay!" );return StatusCode::FAILURE;}

      }

    }
      
    if( !m_overlayLongFolder.empty() ){
    
      DetectorSpecialPixelMap* overlayLong = 0;
      
      if(m_detStore->contains<DetectorSpecialPixelMap>(m_overlayLongKey)){
	sc = m_detStore->retrieve(overlayLong, m_overlayLongKey);
	if( sc.isSuccess() ){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at " << m_overlayLongKey
			 << " retrieved" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to retrieve old DetectorSpecialPixelMap at "
			 << m_overlayLongKey );
	  return StatusCode::FAILURE;
	}
	
	sc = m_detStore->remove(overlayLong);
	
	if( sc.isSuccess() ){
	  ATH_MSG_DEBUG( "Old DetectorSpecialPixelMap at "
			 << m_overlayLongKey << " removed" );
	}
	else{
	  ATH_MSG_FATAL( "Unable to remove old DetectorSpecialPixelMap at "
			 << m_overlayLongKey );
	  return StatusCode::FAILURE;
	}
      }
      
      // fill long overlay map
      
      sc = createFromDetectorStore(m_overlayLongFolder, m_overlayLongKey, !m_moduleLevelOverlay);
      
      if( sc.isSuccess() ){
	ATH_MSG_DEBUG( "New DetectorSpecialPixelMap recorded at " 
		       << m_overlayLongKey );
      }
      else{
	ATH_MSG_ERROR( "Unable to record new DetectorSpecialPixelMap at "
		       << m_overlayLongKey );
	return StatusCode::FAILURE;
      }
      
      sc = m_detStore->retrieve(overlayLong, m_overlayLongKey);
      if( !sc.isSuccess() ){
	ATH_MSG_FATAL( "Unable to retrieve DetectorSpecialPixelMap at " 
		       << m_overlayLongKey );
	return sc;
      }

      overlay->merge( overlayLong );
    }
    
    // apply overlay to first map in m_specialPixelMapKeys
    
    DetectorSpecialPixelMap* spm = 0;
    
    if(m_detStore->contains<DetectorSpecialPixelMap>(m_specialPixelMapKeys[0])){
      sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[0]);
      if(sc.isSuccess()){
	ATH_MSG_DEBUG( "DetectorSpecialPixelMap at " 
		       << m_specialPixelMapKeys[0] << " retrieved" );
      }
      else{
	ATH_MSG_FATAL( "Unable to retrieve DetectorSpecialPixelMap at "
		       << m_specialPixelMapKeys[0] );
	return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_FATAL( "No base DetectorSpecialPixelMap present in detector store " );
      
      return StatusCode::FAILURE;
    }
    
    for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
      
      if( m_moduleLevelOverlay ){
	if( (*overlay)[i] != 0 ){
	  *(*spm)[i] = *(*overlay)[i];
	}
      }
      else{

	unsigned int moduleStatus = (*overlay)[i]->moduleStatus();
	
	if( moduleStatus != 0 ){
	  (*spm)[i]->setModuleStatus(moduleStatus);
	}
	
	if( (*overlay)[i]->hasSpecialChips() ){
	  for( unsigned int chip = 0; chip < (*overlay)[i]->chipsPerModule(); ++chip ){
	    
	    unsigned int chipStatus = (*overlay)[i]->chipStatus(chip);
	    
	    if( chipStatus != 0 ){
	    (*spm)[i]->setChipStatus(chip, chipStatus);
	    }
	  }
	}

	if( (*overlay)[i]->hasSpecialColumnPairs() ){
	  for( unsigned int chip = 0; chip < (*overlay)[i]->chipsPerModule(); ++chip ){
	    if( (*overlay)[i]->hasSpecialColumnPairs(chip) ){
	      for( unsigned int columnPair = 0; 
		   columnPair < (*overlay)[i]->columnsPerChip() / 2; ++columnPair ){
	      
		unsigned int columnPairStatus = (*overlay)[i]->columnPairStatus(chip, columnPair);
		
		if( columnPairStatus != 0 ){
		  (*spm)[i]->setColumnPairStatus(chip, columnPair, columnPairStatus);
		}
	      }
	    }
	  }
	}
	
	for(ModuleSpecialPixelMap::const_iterator pixel = (*overlay)[i]->begin(); 
	    pixel != (*overlay)[i]->end(); ++pixel){
	  (*spm)[i]->setPixelStatus(pixel->first, pixel->second);
	}
      }
    }
  }

  return sc; 
} 



StatusCode SpecialPixelMapSvc::registerCondAttrListCollection(const DetectorSpecialPixelMap* spm) const{
  
  coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification(); 
  
  attrSpec->extend("moduleID", "int");

  if( m_writeBlobs ){
    attrSpec->extend("SpecialPixelMap", "blob" );
  }
  else{
    attrSpec->extend("ModuleSpecialPixelMap_Clob", "string");
  }

  coral::AttributeListSpecification* attrSpecLong = new coral::AttributeListSpecification(); 
  attrSpecLong->extend("moduleID", "int");
  attrSpecLong->extend("SpecialPixelMap", "blob" );
  
  const DetectorSpecialPixelMap* referenceMap = 0;
  
  if( m_differentialUpdates ){
    
    StatusCode sc = m_detStore->retrieve(referenceMap, m_specialPixelMapKeys[0]);
    
    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to retrieve reference special pixel map" );
      return StatusCode::FAILURE;
    }
  }

  CondAttrListCollection* attrListColl = new CondAttrListCollection(true);
  CondAttrListCollection* attrListCollLong = 0;

  if( m_useDualFolderStructure ){
    attrListCollLong = new CondAttrListCollection(true);
  }

  CondAttrListCollection::ChanNum chanNum = 0;
  
  for(DetectorSpecialPixelMap::const_iterator moduleMap = spm->begin(); moduleMap != spm->end(); ++moduleMap){

    ++chanNum;

    if( m_differentialUpdates && (*moduleMap)->to_string() == (*referenceMap)[moduleMap - spm->begin()]->to_string() ){
      continue;
    }

    coral::AttributeList attrList(*attrSpec);
    std::ostringstream attrStr;
    
    Identifier moduleID ( m_pixelID->wafer_id(moduleMap - spm->begin()) );
    
    unsigned int hashID;
    if(isIBL){ 
      hashID = m_pixelID->wafer_hash(moduleID); 
    }
    else{ 
      hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
      ( m_pixelID->layer_disk(moduleID) << 23) +
      ( m_pixelID->phi_module(moduleID) << 17) +
      ( (m_pixelID->eta_module(moduleID) + 6) << 13);
    }
   
    if( m_writeBlobs ){
      
      coral::Blob* blob = (*moduleMap)->to_blob();
      
      attrList["moduleID"].setValue( static_cast<int>( hashID ) );
      attrList["SpecialPixelMap"].setValue(*blob);
      
      if( msgLvl(MSG::DEBUG) ){
	attrList.toOutputStream(attrStr);
      }
      ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );
	
      attrListColl->add(chanNum, attrList);

      delete blob;
    }
    else{

      std::string clob = (*moduleMap)->to_string();
      std::string referenceClob;
      
      if( m_useDualFolderStructure && m_differentialUpdates ){
	referenceClob = (*referenceMap)[moduleMap - spm->begin()]->to_string();
      }
      
      if( m_useDualFolderStructure && clob.size() > 3999 ){

	coral::AttributeList attrListLong(*attrSpecLong);
	
	coral::Blob* blob = (*moduleMap)->to_blob();
	
	attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
	attrListLong["SpecialPixelMap"].setValue(*blob);
	  
	if( msgLvl(MSG::DEBUG) ){
	  attrListLong.toOutputStream(attrStr);
	}
	ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );
	  
	attrListCollLong->add(chanNum, attrListLong);

	delete blob;

	ModuleSpecialPixelMap moduleMap;
	clob = moduleMap.to_string();
      }
      else if( m_useDualFolderStructure && referenceClob.size() > 3999 ){
	
	coral::AttributeList attrListLong(*attrSpecLong);
	
	ModuleSpecialPixelMap moduleMap;
	coral::Blob* blob = moduleMap.to_blob();
	
	attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
	attrListLong["SpecialPixelMap"].setValue(*blob);
	  
	if( msgLvl(MSG::DEBUG) ){
	  attrListLong.toOutputStream(attrStr);
	}
	ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );
	  
	attrListCollLong->add(chanNum, attrListLong);

	delete blob;
      }
      
      attrList["moduleID"].setValue( static_cast<int>( hashID ) );
      attrList["ModuleSpecialPixelMap_Clob"].setValue( clob );
      
      if( msgLvl(MSG::DEBUG) ){
	attrList.toOutputStream(attrStr);
      }
      ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );
      
      attrListColl->add(chanNum, attrList);
    }
  }

  if( m_useDualFolderStructure && attrListCollLong->size() == 0 ){
    
    coral::AttributeList attrListLong(*attrSpecLong);

    coral::Blob* blob = 0;
    
    std::string referenceClob;
      
    if( m_differentialUpdates ){
      referenceClob = (*referenceMap)[0]->to_string();
    }
    
    if( m_differentialUpdates && referenceClob.size() > 3999 ){
	blob = (*referenceMap)[0]->to_blob();
    }
    else{
      ModuleSpecialPixelMap moduleMap;
      blob = moduleMap.to_blob();
    }
    
    Identifier moduleID ( m_pixelID->wafer_id(0) );
    
    unsigned int hashID;
    if(isIBL){
      hashID = m_pixelID->wafer_hash(moduleID);
    }
    else{
      hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
      ( m_pixelID->layer_disk(moduleID) << 23) +
      ( m_pixelID->phi_module(moduleID) << 17) +
      ( (m_pixelID->eta_module(moduleID) + 6) << 13);
    }
    attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
    attrListLong["SpecialPixelMap"].setValue(*blob);
    
    attrListCollLong->add(1, attrListLong);
    
    delete blob;
  }    
  
  StatusCode sc = m_detStore->record(attrListColl, m_outputFolder);
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to record CondAttrListCollection" );
    return StatusCode::FAILURE;
  }

  if( m_useDualFolderStructure ){
    sc = m_detStore->record(attrListCollLong, m_outputLongFolder);
    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to record CondAttrListCollection" );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SpecialPixelMapSvc::createFromDetectorStore(const std::string condAttrListCollectionKey, 
    const std::string pixelMapKey,
    bool fillMissing ) const{

  DetectorSpecialPixelMap* spm = new DetectorSpecialPixelMap();

  spm->resize(m_pixelID->wafer_hash_max(),NULL);

  StatusCode sc = m_detStore->record(spm, pixelMapKey);
  if ( !sc.isSuccess() ){
    ATH_MSG_ERROR( "Unable to record DetectorSpecialPixelMap at "  << pixelMapKey );
    return StatusCode::FAILURE; 
  }

  const CondAttrListCollection* attrListColl = 0;

  sc = m_detStore->retrieve(attrListColl, condAttrListCollectionKey);
  if( !sc.isSuccess() ){ 
    ATH_MSG_ERROR( "Unable to retrieve CondAttrListCollection," << " constructing empty DetectorSpecialPixelMap" );
  }
  else{ 
    ATH_MSG_DEBUG( "CondAttrListCollection retrieved" );

    if( attrListColl->size() == 0 ){
      ATH_MSG_INFO( "CondAttrListCollection is empty," << " constructing empty DetectorSpecialPixelMap" );
    }
    else{

      for(CondAttrListCollection::const_iterator attrList = attrListColl->begin(); attrList != attrListColl->end(); ++attrList){

        std::ostringstream attrStr;
        (*attrList).second.toOutputStream(attrStr);
        ATH_MSG_VERBOSE( "ChanNum " << (*attrList).first << " Attribute list " << attrStr.str() );

        CondAttrListCollection::ChanNum chanNum = (*attrList).first;
        CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
        const IOVRange& range = (*iovIt).second;

        if(range.start().isTimestamp()) {
          ATH_MSG_VERBOSE( "Range timestamp : from " << range.start().timestamp() << " until " << range.stop().timestamp() );
        }
        else {
          ATH_MSG_VERBOSE( "Range R/E : from " << range.start().run() << " " << range.start().event() << " until " << range.stop().run() << " " << range.stop().event() );
        }

        const unsigned int moduleID = static_cast<unsigned int>((*attrList).second["moduleID"].data<const int>());
        unsigned int idhash; 
        if (m_forceNewDBContent) idhash = IdentifierHash(moduleID);
        else if(isIBL){ 
          if(m_dummy || IdentifierHash(moduleID)>m_pixelID->wafer_hash_max()){
            //	    continue (useful to transport the old DB to new DB with IBL!;
            int component = static_cast<int>((moduleID & (3 << 25)) / 33554432) * 2 - 2;
            unsigned int layer = (moduleID & (3 << 23)) / 8388608  ;
            if(component==0)layer +=1; // shift layer
            unsigned int phi = (moduleID & (63 << 17)) / 131072 ;
            int eta = static_cast<int>((moduleID & (15 << 13)) / 8192) - 6 ;
            Identifier id = m_pixelID->wafer_id( component, layer, phi, eta );
            idhash =  m_pixelID->wafer_hash(id);
          }
          else{
            idhash = IdentifierHash(moduleID);
          }
        }
        else {
          int component = static_cast<int>((moduleID & (3 << 25)) / 33554432) * 2 - 2;
          unsigned int layer = (moduleID & (3 << 23)) / 8388608 ;
          unsigned int phi = (moduleID & (63 << 17)) / 131072 ;
          int eta = static_cast<int>((moduleID & (15 << 13)) / 8192) - 6 ;
          Identifier id = m_pixelID->wafer_id( component, layer, phi, eta );
          idhash =  m_pixelID->wafer_hash(id);
        }
        if( idhash < m_pixelID->wafer_hash_max()){

          coral::AttributeList::const_iterator attribute = (*attrList).second.begin();
          ++attribute;
          if( (*attribute).specification().typeName() == "blob" ){

            const coral::Blob& blob = (*attrList).second["SpecialPixelMap"].data<const coral::Blob>();
            delete (*spm)[idhash];
            (*spm)[idhash] = new ModuleSpecialPixelMap(blob, getChips(idhash) );
          }
          else{

            const std::string& clob = (*attrList).second["ModuleSpecialPixelMap_Clob"].data<const std::string>();
            delete (*spm)[idhash];
            (*spm)[idhash] = new ModuleSpecialPixelMap(clob, getChips(idhash) );
          }
        }
        else{
          ATH_MSG_FATAL( "Module hashID out of range: " << idhash );
          return StatusCode::FAILURE;
        }
      }
    }
  }
  if( fillMissing ){
    for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
      if( (*spm)[i] == 0 ){
	(*spm)[i] = new ModuleSpecialPixelMap();
	(*spm)[i]->setchipsPerModule(getChips(i));	
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SpecialPixelMapSvc::createFromTextfiles( bool fillMissing ) const{
  
  ATH_MSG_DEBUG( "Entering createFromTextFiles()" );

  if(m_specialPixelMapKeys.size() == 0){
    ATH_MSG_FATAL( "No StoreGate keys for special pixel map specified" );
    return StatusCode::FAILURE;
  }
  else if(m_specialPixelMapKeys.size() > 1){
    ATH_MSG_INFO( "Multiple StoreGate keys for special pixel map specified" );
    ATH_MSG_INFO( "Trying to store pixel map at " << m_specialPixelMapKeys[0] );
  }

  DetectorSpecialPixelMap* spm = new DetectorSpecialPixelMap;

  spm->resize(m_pixelID->wafer_hash_max(), NULL);

  StatusCode sc = m_detStore->record(spm, m_specialPixelMapKeys[0]);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to record SpecialPixelMap" );
    return StatusCode::FAILURE;
  }
    
  if(m_condAttrListCollectionKeys.size() == 0){
    ATH_MSG_FATAL( "No database folder specified" );
    return StatusCode::FAILURE;
  }
  else if(m_condAttrListCollectionKeys.size() > 1){
    ATH_MSG_INFO( "Multiple database folders specified" );
    ATH_MSG_INFO( "Trying to store special pixel map at " 
		  << m_condAttrListCollectionKeys[0] );
  }

  std::map<unsigned int, unsigned int> moduleIDMap;
  std::vector<std::string> prodIDs;
  
  CoralDB::CoralDB* db = new CoralDB::CoralDB(m_connectionString, coral::ReadOnly, coral::Info, true);
   if(!isIBL){

     db->setObjectDictionaryTag("PIXEL");
     db->setConnectivityTag(m_connectivityTag);
     db->setAliasTag(m_aliasTag);
  
     CoralDB::ObjectDictionaryMap objectDictionary;
     db->getObjectDictionary(objectDictionary, db->compoundTag());

     for(CoralDB::ObjectDictionaryMap::const_iterator object = objectDictionary.begin(); 
	 object != objectDictionary.end(); ++object){
       
       if((*object).second == "MODULE"){
	 
	 std::string prodID = db->findAlias((*object).first, "PRODID");
	 prodIDs.push_back(prodID);
	 unsigned int serialNumber = std::atoi(prodID.substr(1,6).c_str());
	 moduleIDMap[serialNumber] = std::atoi(db->findAlias((*object).first, "HASHID").c_str());
       }
     }
   }
   std::string mydir = m_fileListFileDir+"/";
   std::ifstream filelist((mydir+m_fileListFileName).c_str(), std::ios_base::in);

   while(filelist.good()){
    std::string filename;
    filelist >> filename;

    if(filelist.good()){
      
      static size_t offset = std::string::npos - 1;
      
      if(!isIBL){
	if(offset == std::string::npos - 1){
	
	  std::vector<std::string>::const_iterator prodID = prodIDs.begin();

	  while(prodID != prodIDs.end() && offset >= std::string::npos - 1){
	    offset = filename.find(*prodID);
	    ++prodID;
	  }

	  if(offset == std::string::npos){
	    ATH_MSG_ERROR( "Unable to extract module ID from file name" );
	    return StatusCode::FAILURE;
	  }
	}

	unsigned int serialNumber = std::atoi(filename.substr(offset + 1,6).c_str()); // need to fix ?

	ATH_MSG_INFO( "Reading file " << mydir+filename );
      
	if(moduleIDMap.empty()){
	  ATH_MSG_ERROR( "moduleIDMap is empty " );
	}
	else if(moduleIDMap.find(serialNumber) != moduleIDMap.end()){
	
	  unsigned int moduleID = moduleIDMap[serialNumber];

	  int component = static_cast<int>((moduleID & (3 << 25)) / 33554432) * 2 - 2;

	  unsigned int layer = (moduleID & (3 << 23)) / 8388608 ;

	  unsigned int phi = (moduleID & (63 << 17)) / 131072 ;
	  int eta = static_cast<int>((moduleID & (15 << 13)) / 8192) - 6 ;
		
	  Identifier id( m_pixelID->wafer_id( component, layer, phi, eta ) );
	  unsigned int idhash = m_pixelID->wafer_hash(id);
	  if(idhash < m_pixelID->wafer_hash_max()){
	    if(m_moduleLevelOverlay){
	      std::ifstream in((mydir+filename).c_str(), std::ios_base::in);
              if(in.good()){
                unsigned int status;
                in>>status;
                if(status){
                  if( (*spm)[idhash] == 0 ){
                    (*spm)[idhash] = new ModuleSpecialPixelMap();
                    (*spm)[idhash]->setchipsPerModule( getChips(idhash) );
                    (*spm)[idhash]->setModuleStatus(status);
                  }
                }
              }
            }
            else{
              (*spm)[idhash] = new ModuleSpecialPixelMap((mydir+filename).c_str(), getChips(idhash));
            }
	  }
	}
      }
      else{ // IBL
	//
	offset = filename.find("[");
        if(offset==std::string::npos){
            ATH_MSG_ERROR( "Cound not find the token '['" );
            return StatusCode::FAILURE;
        }
        std::string tmpstr;
        try {
          tmpstr = filename.substr(offset,20);
        }
        catch(std::exception &ex) {
          ATH_MSG_ERROR( "Cound not get string, exception caught: " << ex.what() );
        }
	std::istringstream ss(tmpstr);
	char c;
	int system,subsystem,component, eta;
	unsigned int layer,phi;
	ss>>c>>system;
	ss>>c>>subsystem;
	ss>>c>>component;
	ss>>c>>layer;
	ss>>c>>phi;
	ss>>c>>eta;
	if(system!=2&&subsystem!=1){
	  ATH_MSG_ERROR( "Unable to extract module ID from file name" );
	  return StatusCode::FAILURE;        
        }
	else{

	  ATH_MSG_INFO( "Reading file " << mydir+filename );

          Identifier id( m_pixelID->wafer_id( component, layer, phi, eta ) );
	  unsigned int idhash = m_pixelID->wafer_hash(id);
          if(idhash < m_pixelID->wafer_hash_max()){
	    if(m_moduleLevelOverlay){
	      std::ifstream in((mydir+filename).c_str(), std::ios_base::in);
              if(in.good()){
                unsigned int status;
                in>>status;
                if(status){
                  if( (*spm)[idhash] == 0 ){
                    (*spm)[idhash] = new ModuleSpecialPixelMap();
                    (*spm)[idhash]->setchipsPerModule( getChips(idhash) );
                    (*spm)[idhash]->setModuleStatus(status);
                  }
                }
              }
            }
            else{
              (*spm)[idhash] = new ModuleSpecialPixelMap((mydir+filename).c_str(), getChips(idhash) );
            }
	  }
	  else{
	    ATH_MSG_FATAL( "Reading file: Module hashID out of range: " << idhash );
	    return StatusCode::FAILURE;
	  }
        }
	//
      }
    }
  }	
   
  if( fillMissing ){
    for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
      if( (*spm)[i] == 0 ){
	(*spm)[i] = new ModuleSpecialPixelMap();
	(*spm)[i]->setchipsPerModule( getChips(i) );	
      }
    }
  }

  sc = registerCondAttrListCollection(spm);

  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to create and register CondAttrListCollection" );
    return StatusCode::FAILURE;
  }
  
  delete db;

  return StatusCode::SUCCESS;
}


//============================//

StatusCode SpecialPixelMapSvc::createFromDetectorStore( bool fillMissing ) const{

  StatusCode sc;
  
  if(m_condAttrListCollectionKeys.size() != m_specialPixelMapKeys.size()){
    ATH_MSG_ERROR( "DBFolders/SpecialPixelMapKeys size mismatch" );
    return StatusCode::FAILURE;
  }

  for(unsigned int i = 0; i < m_condAttrListCollectionKeys.size(); i++){
    createFromDetectorStore(m_condAttrListCollectionKeys[i], m_specialPixelMapKeys[i], fillMissing);
  }
  
  return StatusCode::SUCCESS;
}

//
StatusCode SpecialPixelMapSvc::createDeadModuleList() const{

  if(m_specialPixelMapKeys.size() == 0){
    ATH_MSG_FATAL( "No StoreGate keys for special pixel map specified" );
    return StatusCode::FAILURE;
  }
  else if(m_specialPixelMapKeys.size() > 1){
    ATH_MSG_INFO( "Multiple StoreGate keys for special pixel map specified" );
    ATH_MSG_INFO( "Trying to store pixel map at " << m_specialPixelMapKeys[0] );
  }

  DetectorSpecialPixelMap* spm = new DetectorSpecialPixelMap;

  spm->resize(m_pixelID->wafer_hash_max());

  StatusCode sc = m_detStore->record(spm, m_specialPixelMapKeys[0]);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to record SpecialPixelMap" );
    return StatusCode::FAILURE;
  }
  if(m_condAttrListCollectionKeys.size() == 0){
    ATH_MSG_FATAL( "No database folder specified" );
    return StatusCode::FAILURE;
  }
  else if(m_condAttrListCollectionKeys.size() > 1){
    ATH_MSG_INFO( "Multiple database folders specified" );
    ATH_MSG_INFO( "Trying to store special pixel map at "
                  << m_condAttrListCollectionKeys[0] );
  }
  int mk(0);
  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
    if( (*spm)[i] == 0 ){
      (*spm)[i] = new ModuleSpecialPixelMap();
      (*spm)[i]->setchipsPerModule( getChips(i) );
      if(m_killingModule > ((double)rand() / RAND_MAX)){ // kill the modules
        Identifier mID( m_pixelID->wafer_id(i) );
        ATH_MSG_INFO( "List of modules killed "<<mk<<" hash "<<i<<" ["<<m_pixelID->barrel_ec(mID)<<","<< m_pixelID->layer_disk(mID)<<","<<
                      m_pixelID->phi_module(mID)<<","<<m_pixelID->eta_module(mID)<<"]");
        (*spm)[i]->setModuleStatus(1);
        ++mk;
      }
    }
  }

  sc = registerCondAttrListCollection(spm);

  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to create and register CondAttrListCollection" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SpecialPixelMapSvc::createMaskingOverlay(const std::string internalKey) const{

  DetectorSpecialPixelMap* spm = new DetectorSpecialPixelMap;

  spm->resize(m_pixelID->wafer_hash_max());

  StatusCode sc = m_detStore->record(spm, internalKey);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to record SpecialPixelMap" );
    return StatusCode::FAILURE;
  }
  if(m_condAttrListCollectionKeys.size() == 0){
    ATH_MSG_FATAL( "No database folder specified" );
    return StatusCode::FAILURE;
  }
  else if(m_condAttrListCollectionKeys.size() > 1){
    ATH_MSG_INFO( "Multiple database folders specified" );
    ATH_MSG_INFO( "Trying to store special pixel map at "
                  << m_condAttrListCollectionKeys[0] );
  }
  int mk(0);
  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
    Identifier mID( m_pixelID->wafer_id(i) );
    (*spm)[i] = new ModuleSpecialPixelMap();
    (*spm)[i]->setchipsPerModule( getChips(i) ); 
    if (
	(m_pixelID->barrel_ec(mID) == 0 && (std::find(m_layersToMask.begin(), m_layersToMask.end(), m_pixelID->layer_disk(mID)) != m_layersToMask.end())) ||
	(m_pixelID->barrel_ec(mID) == 2  && (std::find(m_disksToMask.begin(), m_disksToMask.end(), (m_pixelID->layer_disk(mID) + 1)) != m_disksToMask.end())) ||
	(m_pixelID->barrel_ec(mID) == -2 && (std::find(m_disksToMask.begin(), m_disksToMask.end(), -1*(m_pixelID->layer_disk(mID) + 1)) != m_disksToMask.end()))
	){
      (*spm)[i]->setModuleStatus(1);
      ATH_MSG_INFO( "List of modules killed "<<mk<<" hash "<<i<<" ["<<m_pixelID->barrel_ec(mID)<<","<< m_pixelID->layer_disk(mID)<<","<<
		    m_pixelID->phi_module(mID)<<","<<m_pixelID->eta_module(mID)<<"]");
      ++mk;
    }
  }
  
  sc = registerCondAttrListCollection(spm);
  
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to create and register CondAttrListCollection" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//============================//

StatusCode SpecialPixelMapSvc::create() const{

  StatusCode sc(StatusCode::SUCCESS);

  if(m_killingModule>0) sc = createDeadModuleList();

  if(m_dataSource == "Textfiles"){
    sc = createFromTextfiles();
  }
  else if(m_dataSource == "Database"){
    sc = createFromDetectorStore();
  }
  else if(m_dataSource == "None"){
    sc = StatusCode::SUCCESS;
  }
  else{ 
    ATH_MSG_ERROR( "Please specify \"Textfiles\", \"Database\" or \"None\" "
		   << "as DataSource in the job options" );
    return StatusCode::FAILURE;
  }
  return sc;
}


//============================//

StatusCode SpecialPixelMapSvc::print() const{

  if(m_moduleIDsForPrinting.empty()){
    ATH_MSG_INFO( "No modules selected for printing" );
    ATH_MSG_INFO( "Select modules by adding their IDs to ModuleIDsForPrinting"
		  << " in the job options" );
  }
  else{
    //first, print the conditions objects stored in the database
    StatusCode sc;
    ATH_MSG_INFO( "Entering print()" );
    
    if(m_condAttrListCollectionKeys.size() != m_specialPixelMapKeys.size()){
      ATH_MSG_FATAL( "DBFolders/SpecialPixelMapKeys size mismatch" );
      return StatusCode::FAILURE;
    }
    
    for(unsigned int i = 0; i < m_condAttrListCollectionKeys.size(); i++){
      
      const CondAttrListCollection* attrListColl = 0;
      
      ATH_MSG_DEBUG( "Printing CondAttrListCollection at " 
		     << m_condAttrListCollectionKeys[i] );
      
      sc = m_detStore->retrieve(attrListColl, m_condAttrListCollectionKeys[i]);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Unable to retrieve CondAttrListCollection" );
      }
      else {
	ATH_MSG_DEBUG( "CondAttrListCollection retrieved" );
	
	for (CondAttrListCollection::const_iterator attrList = attrListColl->begin(); attrList != attrListColl->end(); ++attrList){
	  std::ostringstream attrStr;
	  
	  const unsigned int moduleID = static_cast<unsigned int>((*attrList).second["moduleID"].data<const int>());

	  std::ostringstream to_str;
	  to_str << moduleID;

	  if((std::find(m_moduleIDsForPrinting.begin(), m_moduleIDsForPrinting.end(), to_str.str())
	      != m_moduleIDsForPrinting.end()) || (*(m_moduleIDsForPrinting.begin()) == std::string("all") &&
						     m_moduleIDsForPrinting.size() == 1)){
	    
	    (*attrList).second.toOutputStream(attrStr);
	    ATH_MSG_DEBUG( "ChanNum " << (*attrList).first << " Attribute list " << attrStr.str() );
	    
	    CondAttrListCollection::ChanNum chanNum = (*attrList).first;
	    CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
	    if (iovIt != attrListColl->iov_end()) {
	      const IOVRange& range = (*iovIt).second;
	      if(range.start().isTimestamp()) {
		ATH_MSG_DEBUG( "Range timestamp: from " << range.start().timestamp()
			       << " to " << range.stop().timestamp() );
	      }
	      else {
		ATH_MSG_DEBUG( "Range R/E: from " << range.start().run() << " " 
			       << range.start().event()
			       << " to " << range.stop().run() << " " 
			       << range.stop().event() );
	      }
	    }
	    else {
	      ATH_MSG_DEBUG( "No range found" );
	    }
	  }
	}
      }
      
      //second, print the DetectorSpecialPixelMap itself
      
      const DetectorSpecialPixelMap* spm = 0;
      
      ATH_MSG_INFO( "Printing special pixel map at " << m_specialPixelMapKeys[i] );
      
      sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[i]);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Unable to retrieve DetectorSpecialPixelMap" );
	return(StatusCode::FAILURE);
      }
      else {
	ATH_MSG_DEBUG( "DetectorSpecialPixelMap retrieved" );
	
	for (DetectorSpecialPixelMap::const_iterator module = spm->begin(); module != spm->end(); ++module){
	  std::ostringstream attrStr;
	  
	  std::ostringstream to_str;
	  if(isIBL){ // idhash
	    to_str << module - spm->begin();
	  }
	  else{ // identifier 
	    to_str <<m_pixelID->wafer_id(module - spm->begin());
	  }
	  
	  if((std::find(m_moduleIDsForPrinting.begin(), m_moduleIDsForPrinting.end(), to_str.str())
	      != m_moduleIDsForPrinting.end()) || (*(m_moduleIDsForPrinting.begin()) == std::string("all") && 
						   m_moduleIDsForPrinting.size() == 1)){
	    
	    Identifier moduleID( m_pixelID->wafer_id(module - spm->begin()) );
	    
	    (*module)->print( m_pixelID->barrel_ec(moduleID),
			      m_pixelID->layer_disk(moduleID),
			      m_pixelID->phi_module(moduleID),
			      m_pixelID->eta_module(moduleID),
			      m_verbosePixelID, m_binaryPixelStatus);
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}


//============================//

const DetectorSpecialPixelMap *SpecialPixelMapSvc::getPixelMap(unsigned int index) const {
  const DetectorSpecialPixelMap *spm = 0;
  if (index < m_specialPixelMapKeys.size()) {
    StatusCode sc = m_detStore->retrieve(spm, m_specialPixelMapKeys[index]);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to retrieve DetectorSpecialPixelMap: "
		     << m_specialPixelMapKeys[index] );
      return 0;
    }
  } else {
    return 0;
  }
  return spm;
}

unsigned int SpecialPixelMapSvc::getChips(const unsigned int & index) const{
  unsigned int i =index;
  std::map<unsigned int, unsigned int> ::const_iterator it= m_chips.find(i);
  if(it !=m_chips.end()) return (*it).second;
  ATH_MSG_ERROR( " Unable to retrieve the number of chips for module idhash "<<index);
  return 0;
}
