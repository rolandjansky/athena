/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKDetectorTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <fstream>
#include <iostream>

using namespace std;

FTKDetectorTool::FTKDetectorTool(const std::string &algname,const std::string &name, const IInterface *ifc)
  : AlgTool(algname,name,ifc) 
  , m_log( msgSvc() , name )
  , m_dumppath("deadmap.dat")
  , m_storeGate( 0 )
  , m_detStore( 0 )
  , m_evtStore( 0 )
  , m_PIX_mgr( 0 )
  , m_SCT_mgr( 0 )
  , m_pixelContainer( 0 )
  , m_sctContainer( 0 )
  , m_pixelCondSummarySvc("PixelConditionsSummarySvc",name)
  , m_sctCondSummarySvc("InDetSCT_ConditionsSummarySvc",name)
  , m_pixelId( 0 )
  , m_sctId( 0 )
  , m_pixelClustersName( "PixelClusters" )
  , m_sctClustersName( "SCT_Clusters" )
  , m_FTK_BadModuleMapPath( "badModulemap_FTK.bmap" )
  , m_ATLAS_BadModuleMapPath( "badModulemap_ATLAS.bmap" )
  , m_doBadModuleMap(true)
  , m_pmap_path()
  , m_bad_module_map(0x0)
{
  declareInterface<FTKDetectorToolI>(this);
  
  declareProperty("DumpPath",                 m_dumppath);
  declareProperty("pixelClustersName",        m_pixelClustersName);
  declareProperty("SCT_ClustersName",         m_sctClustersName);
  declareProperty("PixelSummarySvc" ,         m_pixelCondSummarySvc);
  declareProperty("SctSummarySvc" ,           m_sctCondSummarySvc);
  declareProperty("FTK_BadModuleMapPath" ,    m_FTK_BadModuleMapPath );
  declareProperty("ATLAS_BadModuleMapPath" ,  m_ATLAS_BadModuleMapPath );
  declareProperty("pmap_path",                m_pmap_path);
}

FTKDetectorTool::~FTKDetectorTool()
{
  if (m_bad_module_map) delete m_bad_module_map;
}


StatusCode FTKDetectorTool::initialize()
{
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endreq;
    return StatusCode::FAILURE;
  }
  if( service("DetectorStore",m_detStore).isFailure() ) {
    m_log << MSG::FATAL <<"DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_pixelCondSummarySvc << endreq;
    return StatusCode::FAILURE;
  } 
  if ( m_sctCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_sctCondSummarySvc << endreq;
    return StatusCode::FAILURE;
  } 

  m_log << MSG::INFO << "Read the logical layer definitions" << endreq;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    m_log << MSG::FATAL << "Main plane map definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      m_log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }
    m_log << MSG::INFO << "m_pmap->getNPlanes() = " << m_pmap->getNPlanes() << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode FTKDetectorTool::finalize()
{
  return StatusCode::SUCCESS;
}

void FTKDetectorTool::makeBadModuleMap(){
  if (!m_doBadModuleMap) return;

  // list of the dead module, using the ftkrawhit format
  list<FTKRawHit> badmodule_rawlist;   
  
  // take the list of dead pixels
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(is_bad){
      FTKRawHit tmpmodraw;
      
      tmpmodraw.setHitType(ftk::PIXEL);
      tmpmodraw.setBarrelEC(m_pixelId->barrel_ec(id));
      tmpmodraw.setLayer(m_pixelId->layer_disk(id));
      tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
      tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
      tmpmodraw.setPhiSide(m_pixelId->phi_index(id));
      tmpmodraw.setEtaStrip(m_pixelId->eta_index(id));
      tmpmodraw.normalizeLayerID();
      
      badmodule_rawlist.push_back(tmpmodraw);      
    }
  }
  
  // take the list of the dead SCT modules
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(is_bad){
      FTKRawHit tmpmodraw;
      
      tmpmodraw.setHitType(ftk::SCT);
      tmpmodraw.setBarrelEC(m_sctId->barrel_ec(id));
      tmpmodraw.setLayer(m_sctId->layer_disk(id));
      tmpmodraw.setPhiModule(m_sctId->phi_module(id));
      tmpmodraw.setEtaModule(m_sctId->eta_module(id));
      tmpmodraw.setPhiSide(m_sctId->side(id));
      tmpmodraw.setEtaStrip(m_sctId->strip(id));
      tmpmodraw.normalizeLayerID();
      
      badmodule_rawlist.push_back(tmpmodraw);     
    }
  }
  
  
  m_log << MSG::INFO << "Total number of dead modules find: " << badmodule_rawlist.size() << endl;
  
  // prepare the map of the bad module, the size is equal to the map of used layers
  m_bad_module_map = new list<FTKHit>[m_pmap->getNPlanes()];
  /* convert the dead module list in he FTKHit format, taking into account
     the plane-map configuration */
  list<FTKRawHit>::const_iterator ibadmod = badmodule_rawlist.begin();
  unsigned nbadmod_effective(0);
  for (;ibadmod!=badmodule_rawlist.end();++ibadmod) {
    const FTKRawHit &badmodraw = *ibadmod;
    // the module in the unused layers are skipped 
    if (m_pmap->getMap(badmodraw.getHitType(),badmodraw.getBarrelEC()!=0,badmodraw.getLayer()).getPlane() != -1) {
      // the bad module is used by FTK
      FTKHit tmpmodhit = badmodraw.getFTKHit(m_pmap);
      m_bad_module_map[tmpmodhit.getPlane()].push_back(tmpmodhit);
      nbadmod_effective += 1; // increment the counter
    }
  }
}




void FTKDetectorTool::dumpDeadModuleSummary()  
{
  if (!m_doBadModuleMap) return;
 
  ofstream mapfile_ATLAS_BadModuleMap(m_ATLAS_BadModuleMapPath.c_str());
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(is_bad){
      
      mapfile_ATLAS_BadModuleMap << "B\t"
				 << 1  << '\t' // 1  pixel 0 sct
				 << m_pixelId->barrel_ec(id) << '\t'
				 << m_pixelId->layer_disk(id) << '\t'
				 << m_pixelId->phi_module(id) << '\t'
				 << m_pixelId->eta_module(id) << '\t'
				 << 0 //it means m_pixelId don't have side(id) 
				 << std::endl;
    }
  }
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(is_bad){
      mapfile_ATLAS_BadModuleMap  << "B\t"
				 << 0  << '\t' // 1  pixel 0 sct 
				 << m_sctId->barrel_ec(id) << '\t'
				 << m_sctId->layer_disk(id) << '\t'
				 << m_sctId->phi_module(id) << '\t'
				 << m_sctId->eta_module(id) << '\t'
				 << m_sctId->side(id) << '\t'
				 << std::endl;
    }
  }
  mapfile_ATLAS_BadModuleMap.close();
  
  ofstream mapfile_FTK_BadModuleMap(m_FTK_BadModuleMapPath.c_str());
  for(int ip=0;ip!=m_pmap->getNPlanes();++ip){
    list<FTKHit> &layer_map = m_bad_module_map[ip];
    for( list<FTKHit>::iterator ite_list = layer_map.begin();
	 ite_list != layer_map.end(); ++ite_list ){
      int BEC(0);
      if((*ite_list).getIsBarrel()) BEC = 0;
      else if((*ite_list).getCSide()) BEC = 2;
      else if((*ite_list).getASide()) BEC = -2;
      mapfile_FTK_BadModuleMap <<m_pmap->isPixel((*ite_list).getPlane()) <<'\t' //SCT = 0 or pixel = 1
			       <<  BEC <<'\t'  // Barrel = 0 or EndCap A = 2 or EndCap C = -2 	
			       << (*ite_list).getSector() <<'\t'
			       << (*ite_list).getPlane() <<'\t'
			       << (*ite_list).getEtaModule() <<'\t'
			       << (*ite_list).getPhiModule() <<'\t'
			       << (*ite_list).getSection() <<'\t'
			       <<  std::endl;
    }
  }
  mapfile_FTK_BadModuleMap.close();

  m_doBadModuleMap = false;
}
