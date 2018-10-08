/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKDetectorTool.h"
#include "TrigFTKSim/FTKSetup.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <fstream>
#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

using namespace std;

FTKDetectorTool::FTKDetectorTool(const std::string &algname,const std::string &name, const IInterface *ifc)
  : AthAlgTool(algname,name,ifc)
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
  , m_FTK_ModuleIDMapPath("ftk.moduleidmap")
  , m_doBadModuleMap(true)
  , m_pmap_path()
  , m_bad_module_map(0x0),
  m_global2local_path("global-to-local-map.moduleidmap"),
  m_sram_path_pix("sram_lookup_pixel.txt"),
  m_sram_path_sct("sram_lookup_sct.txt"),
  m_rmap_path(""),
    m_rmap(0x0),
    m_dumpAllModules(false)
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

  declareProperty("GlobalToLocalMapPath",m_global2local_path);
  declareProperty("SRAMPathPixel",m_sram_path_pix);
  declareProperty("SRAMPathSCT",m_sram_path_sct);
  declareProperty("rmap_path",m_rmap_path);
  declareProperty("dumpAllModules",m_dumpAllModules);
}

FTKDetectorTool::~FTKDetectorTool()
{
  if (m_bad_module_map) delete m_bad_module_map;
}


StatusCode FTKDetectorTool::initialize()
{
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if( service("DetectorStore",m_detStore).isFailure() ) {
    m_log << MSG::FATAL <<"DetectorStore service not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_pixelCondSummarySvc << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_sctCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_sctCondSummarySvc << endmsg;
    return StatusCode::FAILURE;
  }

  m_log << MSG::INFO << "Read the logical layer definitions" << endmsg;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    m_log << MSG::FATAL << "Main plane map definition missing" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      m_log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
      return StatusCode::FAILURE;
    }
    m_log << MSG::INFO << "Number of logical layers = " << m_pmap->getNPlanes() << endmsg;
  }

  if (m_rmap_path.empty()) {
	  m_log << MSG::INFO << "Tower/Region map not set, module maps cannot be created" << endmsg;
  }
  else {
	  m_log << MSG::INFO << "Tower/Region map path: " << m_rmap_path << endmsg;
	  m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
	  if (!(*m_rmap)) {
		m_log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endmsg;
		return StatusCode::FAILURE;
	  }
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
    bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(m_dumpAllModules) is_bad =true;
    if(is_bad){
      FTKRawHit tmpmodraw;

      tmpmodraw.setHitType(ftk::PIXEL);
      tmpmodraw.setModuleType(ftk::MODULETYPE_PIXEL);
      tmpmodraw.setBarrelEC(m_pixelId->barrel_ec(id));
      tmpmodraw.setLayer(m_pixelId->layer_disk(id));
      tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
      tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
      tmpmodraw.setPhiSide(m_pixelId->phi_index(id));
      tmpmodraw.setEtaStrip(m_pixelId->eta_index(id));
      tmpmodraw.setIdentifierHash(idhash);
      tmpmodraw.normalizeLayerID();

      badmodule_rawlist.push_back(tmpmodraw);
    }
  }

  // take the list of the dead SCT modules
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(m_dumpAllModules) is_bad =true;
    if(is_bad){
      FTKRawHit tmpmodraw;

      tmpmodraw.setHitType(ftk::SCT);
      tmpmodraw.setModuleType(ftk::MODULETYPE_SCT);
      tmpmodraw.setBarrelEC(m_sctId->barrel_ec(id));
      tmpmodraw.setLayer(m_sctId->layer_disk(id));
      tmpmodraw.setPhiModule(m_sctId->phi_module(id));
      tmpmodraw.setEtaModule(m_sctId->eta_module(id));
      tmpmodraw.setPhiSide(m_sctId->side(id));
      tmpmodraw.setEtaStrip(m_sctId->strip(id));
      tmpmodraw.setIdentifierHash(idhash);
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
    bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(m_dumpAllModules) is_bad =true;
    if(is_bad){

      mapfile_ATLAS_BadModuleMap << "B\t"
				 << 1  << '\t' // 1  pixel 0 sct
				 << m_pixelId->barrel_ec(id) << '\t'
				 << m_pixelId->layer_disk(id) << '\t'
				 << m_pixelId->phi_module(id) << '\t'
				 << m_pixelId->eta_module(id) << '\t'
				 << 0 << '\t' //it means m_pixelId don't have side(id)
				 << idhash << '\t'
				 << std::endl;
    }
  }
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(m_dumpAllModules) is_bad =true;
    if(is_bad){
      mapfile_ATLAS_BadModuleMap  << "B\t"
				 << 0  << '\t'  // 1  pixel 0 sct
				 << m_sctId->barrel_ec(id) << '\t'
				 << m_sctId->layer_disk(id) << '\t'
				 << m_sctId->phi_module(id) << '\t'
				 << m_sctId->eta_module(id) << '\t'
				 << m_sctId->side(id) << '\t'
				 << idhash << '\t'
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
          << (*ite_list).getIdentifierHash() << '\t'
          <<  std::endl;
    }
  }
  mapfile_FTK_BadModuleMap.close();

  m_doBadModuleMap = false;
}

void FTKDetectorTool::dumpModuleIDMap()
{
#if 0 // excluded from compilation to avoid warnings
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
  }
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
  }
#endif
}

/* this method loops over all the ID modules, pixel and SCT, with the goal
 * to create the online and offline map used to describe the modules within
 * the FTK system. In particular can create the list of the modules in FTK
 * towers, organized by layer, as used to calcualte the SS
 */
void FTKDetectorTool::dumpGlobalToLocalModuleMap() {
  /* All the modules are collected as FTKRawHit, allowing to interact efficiently
   * with FTK elements as PMAP or RMAP
   */
  list<FTKRawHit> CompleteIDModuleList;

  // To save the SRAM lookup into an output file
  ofstream fout_pix(m_sram_path_pix);
  ofstream fout_sct(m_sram_path_sct);
  Int_t countForSRAM(0);

  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();

    FTKRawHit tmpmodraw;

    tmpmodraw.setHitType(ftk::PIXEL);
    tmpmodraw.setModuleType(ftk::MODULETYPE_PIXEL);
    tmpmodraw.setBarrelEC(m_pixelId->barrel_ec(id));
    tmpmodraw.setLayer(m_pixelId->layer_disk(id));
    tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
    tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
    tmpmodraw.setPhiSide(m_pixelId->phi_index(id));
    tmpmodraw.setEtaStrip(m_pixelId->eta_index(id));
    tmpmodraw.setIdentifierHash(idhash);
    tmpmodraw.normalizeLayerID();

    if (abs(m_pixelId->barrel_ec(id)) != 4) { // remove diamond hits
      fout_pix << m_pixelId->barrel_ec(id) << "\t" << m_pixelId->layer_disk(id) << "\t" << m_pixelId->phi_module(id) << "\t" << m_pixelId->eta_module(id) << "\t" << countForSRAM << endl;
      countForSRAM++;
    }

    CompleteIDModuleList.push_back(tmpmodraw);
  }

  countForSRAM = 0;

  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();

    FTKRawHit tmpmodraw;

    tmpmodraw.setHitType(ftk::SCT);
    tmpmodraw.setModuleType(ftk::MODULETYPE_SCT);
    tmpmodraw.setBarrelEC(m_sctId->barrel_ec(id));
    tmpmodraw.setLayer(m_sctId->layer_disk(id));
    tmpmodraw.setPhiModule(m_sctId->phi_module(id));
    tmpmodraw.setEtaModule(m_sctId->eta_module(id));
    tmpmodraw.setPhiSide(m_sctId->side(id));
    tmpmodraw.setEtaStrip(m_sctId->strip(id));
    tmpmodraw.setIdentifierHash(idhash);
    tmpmodraw.normalizeLayerID();

    fout_sct << m_sctId->barrel_ec(id) << "\t" << m_sctId->layer_disk(id) << "\t" << m_sctId->phi_module(id) << "\t" << m_sctId->eta_module(id) << "\t" << countForSRAM << endl;
    countForSRAM++;

    CompleteIDModuleList.push_back(tmpmodraw);
  }



  /* The modules are store by tower and by logical layer */
  int nregions(m_rmap->getNumRegions()); // get the number of towers
  int nplanes(m_pmap->getNPlanes());
  set<unsigned int> **grouped_modules = new set<unsigned int>*[nregions];
  for (int ireg=0;ireg!=nregions;++ireg) grouped_modules[ireg] = new set<unsigned int>[nplanes];

  unsigned int nskipped(0);
  for (auto curmodrawhit: CompleteIDModuleList) { // loop over the modules, represente as raw hits
      // verify if accoring the current pmap this is module that has to be mapped
      if (m_pmap->getMap(curmodrawhit.getHitType(),curmodrawhit.getBarrelEC()!=0,curmodrawhit.getLayer()).getPlane() == -1) {
          nskipped += 1;
          continue;
      }

      // convert the FTKRawHit representation in FTKHit to interact with the PMAP
      FTKHit curmodhit = curmodrawhit.getFTKHit(m_pmap);

      bool hasOneRegion(false); // it will become true if at least 1 tower is associated with the module
      for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
          if (m_rmap->isHitInRegion(curmodhit,ireg)) {
              hasOneRegion = true;
              // the module is compatible with the current
              grouped_modules[ireg][curmodhit.getPlane()].insert(curmodhit.getIdentifierHash());
          }
      } // end loop over the regions

      if (!hasOneRegion) m_log << MSG::WARNING << "The module with hash " << curmodhit.getIdentifierHash() << " and FTK ID (" << curmodhit.getPlane() << "," << curmodhit.getSector() << ") is not associated to a tower" << endmsg;
  } // end loop over the modules

  // Save the map into the output file and print at screen a small message
  ofstream fout(m_global2local_path.c_str());
  for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
      for (int ip=0;ip!=nplanes;++ip) { // loop over the regions
         //m_log << MSG::INFO << "Region " << ireg << ", layer" << ip << " has " << grouped_modules[ireg][ip].size() << " modules" << endmsg;
          unsigned int modnumber(0);
          for (auto curhash: grouped_modules[ireg][ip]) {
              fout << ireg << '\t' << ip << '\t' << curhash << '\t' << modnumber++ << endl;
          }
      }
  }
  fout.close();

  // Close SRAM lookup tables
  fout_pix.close();
  fout_sct.close();

  // clear the memory
  for (int ireg=0;ireg!=nregions;++ireg) delete [] grouped_modules[ireg];
  delete [] grouped_modules;

}


void FTKDetectorTool::dumpIDMap()
{
  ofstream mapfile("mapfile.txt");

  mapfile << "PXL\tSL\tBEC\tLD\tMPHI\tMETA\tPHIID\tETAID\tHASH\tX\tY\tZ\tSinT\tW\tL\tISBAD" << endl;

  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));

    mapfile << ftk::PIXEL << "\t" << 0 << "\t";
    mapfile << m_pixelId->barrel_ec(id) << "\t";
    mapfile << m_pixelId->layer_disk(id) << "\t";
    mapfile << m_pixelId->phi_module(id) << "\t";
    mapfile << m_pixelId->eta_module(id) << "\t";
    mapfile << m_pixelId->phi_index(id) << "\t";
    mapfile << m_pixelId->eta_index(id) << "\t";
    mapfile << idhash << "\t";
    Amg::Vector3D modcenter = sielement->center();
    mapfile << modcenter[0] << "\t" << modcenter[1] << "\t" << modcenter[2] << "\t";
    mapfile << sielement->sinTilt() << "\t";
    mapfile << sielement->width() << "\t";
    mapfile << sielement->length() << "\t";
    mapfile << is_bad;
    mapfile << endl;
  }

  // take the list of the dead SCT modules
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));

    mapfile << ftk::SCT << "\t" << (sielement->isStereo() ? 1 : 0) << "\t";
    mapfile << m_sctId->barrel_ec(id) << "\t";
    mapfile << m_sctId->layer_disk(id) << "\t";
    mapfile << m_sctId->phi_module(id) << "\t";
    mapfile << m_sctId->eta_module(id) << "\t";
    mapfile << m_sctId->side(id) << "\t";
    mapfile << m_sctId->strip(id) << "\t";
    mapfile << idhash << "\t";
    Amg::Vector3D modcenter = sielement->center();
    mapfile << modcenter[0] << "\t" << modcenter[1] << "\t" << modcenter[2] << "\t";
    mapfile << sielement->sinTilt() << "\t";
    mapfile << sielement->width() << "\t";
    mapfile << sielement->length() << "\t";
    mapfile << is_bad;
    mapfile << endl;
  }

}

void FTKDetectorTool::dumpModulePositions() {
   m_log << MSG::INFO << "dumpModulePositions"<< endmsg; 
   TFile *output=new TFile("FTKmodulePositions.root","recreate");
   TTree *t=new TTree("modulePositions","modulePositions");
   Int_t idhash;
   Int_t isbad,isBLayer,isPixel,barrel_endcap,layer_disk,phi_module,eta_module;
   Int_t side,strip,phi_index,eta_index,hitSector,section,swapPhi,swapEta;
   Float_t phi[2],r[2],z[2];
   Float_t center[3],phiAxis[3],etaAxis[3],width,length,phiPitch,etaPitch;
   Float_t sinTilt;
   t->Branch("id",&idhash,"id/I");
   t->Branch("isPixel",&isPixel,"isPixel/I");
   t->Branch("isBLayer",&isBLayer,"isBLayer/I");
   t->Branch("barrel_endcap",&barrel_endcap,"barrel_endcap/I");
   t->Branch("layer_disk",&layer_disk,"layer_disk/I");
   t->Branch("phi_module",&phi_module,"phi_module/I");
   t->Branch("eta_module",&eta_module,"eta_module/I");
   t->Branch("side",&side,"side/I");
   t->Branch("section",&section,"section/I");
   t->Branch("phi_index",&phi_index,"phi_index/I");
   t->Branch("eta_index",&eta_index,"eta_index/I");
   t->Branch("strip",&strip,"strip/I");
   t->Branch("width",&width,"width/F");
   t->Branch("length",&length,"length/F");
   t->Branch("phiPitch",&phiPitch,"phiPitch/F");
   t->Branch("etaPitch",&etaPitch,"etaPitch/F");
   t->Branch("phiAxis",phiAxis,"phiAxis[3]/F");
   t->Branch("etaAxis",etaAxis,"etaAxis[3]/F");
   t->Branch("swapPhi",&swapPhi,"swapPhi/I");
   t->Branch("swapEta",&swapEta,"swapEta/I");
   t->Branch("sinTilt",&sinTilt,"sinTilt/F");
   t->Branch("center",center,"center[3]/F");
   t->Branch("phi",phi,"phi[2]/F");
   t->Branch("r",r,"r[2]/F");
   t->Branch("z",z,"z[2]/F");
   t->Branch("isbad",&isbad,"isbad/I");
   t->Branch("hitSector",&hitSector,"hitSector/I");
   InDetDD::SiDetectorElementCollection::const_iterator iStart[2],iEnd[2];
   iStart[0]=m_SCT_mgr->getDetectorElementBegin();
   iEnd[0]=m_SCT_mgr->getDetectorElementEnd();
   iStart[1]=m_PIX_mgr->getDetectorElementBegin();
   iEnd[1]=m_PIX_mgr->getDetectorElementEnd();
   for(isPixel=0;isPixel<2;isPixel++) {
      //m_log << MSG::INFO <<"dumpModulePositions() isPixel="<<isPixel<<endmsg;
     for( InDetDD::SiDetectorElementCollection::const_iterator
              i=iStart[isPixel];i!=iEnd[isPixel];i++) {
         const InDetDD::SiDetectorElement* sielement( *i );
         idhash=sielement->identifyHash();
         Identifier id = sielement->identify();
         isBLayer=sielement->isBlayer();
         if(isPixel) {
            barrel_endcap=m_pixelId->barrel_ec(id);
            layer_disk=m_pixelId->layer_disk(id);
            phi_module=m_pixelId->phi_module(id);
            eta_module=m_pixelId->eta_module(id);
            side=-1;
            strip=-1;
            phi_index=m_pixelId->phi_index(id);
            eta_index=m_pixelId->eta_index(id);
            section=m_pmap->getMap(ftk::PIXEL,!(barrel_endcap==0),
                                   layer_disk).getSection();
            // see FTKRawHit
            if ((FTKSetup::getFTKSetup().getIBLMode()==1) &&
                (layer_disk==0) &&
                (barrel_endcap==0)) {
               // IBL module, without 3d sensors
               hitSector = phi_module*1000+eta_module+8;
            } else if ((FTKSetup::getFTKSetup().getIBLMode()==2) &&
                       (layer_disk==0) && (barrel_endcap==0)) {
               // IBL module with 3d sensors, 20 modules in total
               hitSector = phi_module*1000+eta_module+10;
            }  else if(barrel_endcap) {
               hitSector=
                  phi_module*1000+
                  (eta_module+1)*20 + (barrel_endcap == ftk::NEGEC)*10
                  + section;
            } else {
               // is a generic module of the barrel region
               hitSector = phi_module*1000+eta_module+6;
            }
         } else {
            barrel_endcap=m_sctId->barrel_ec(id);
            layer_disk=m_sctId->layer_disk(id);
            phi_module=m_sctId->phi_module(id);
            eta_module=m_sctId->eta_module(id);
            side=m_sctId->side(id);
            strip=m_sctId->strip(id);
            phi_index=-1;
            eta_index=-1;
            section=m_pmap->getMap(ftk::SCT,!(barrel_endcap==0),
                                   layer_disk).getSection();
            // see FTKRawHit
            if(barrel_endcap) {
               hitSector=
                  phi_module*1000+
                  (eta_module+1)*20 + (barrel_endcap == ftk::NEGEC)*10
                  + section;
            } else {
               // is a generic module of the barrel region
               hitSector = phi_module*1000+eta_module+6;
            }
         }
         if(FTKSetup::getFTKSetup().getITkMode() ) {
            // see FTKRawHit
            hitSector = (phi_module*100000) + ((eta_module+60)*100) +
               ((barrel_endcap+2) *10) + section;
         }
         width=sielement->width();
         length=sielement->length();
         phiPitch=sielement->phiPitch();
         etaPitch=sielement->etaPitch();
         //phiDir=sielement->hitPhiDirection();
         //etaDir=sielement->hitEtaDirection();
         swapPhi=sielement->swapPhiReadoutDirection() ? 1 : 0;
         swapEta=sielement->swapEtaReadoutDirection() ? 1 : 0;
         sinTilt=sielement->sinTilt();
         for(int k=0;k<3;k++) {
            center[k]= sielement->center()[k];
            phiAxis[k]= sielement->phiAxis()[k];
            etaAxis[k]= sielement->etaAxis()[k];
         }
         r[0]=sielement->rMin();
         r[1]=sielement->rMax();
         z[0]=sielement->zMin();
         z[1]=sielement->zMax();
         phi[0]=sielement->phiMin();
         phi[1]=sielement->phiMax();
         isbad=m_sctCondSummarySvc->isGood( idhash ) ? 0 : 1;
         t->Fill();
      }
   }
   //m_log << MSG::INFO <<"dumpModulePositions() done"<<endmsg;
   t->Write();
   output->Close();
   delete output;
}
