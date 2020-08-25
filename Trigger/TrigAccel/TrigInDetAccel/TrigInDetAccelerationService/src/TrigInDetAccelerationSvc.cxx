/*   
     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiNumerology.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "TrigInDetAccelerationSvc.h"
#include "TrigAccelEvent/TrigInDetAccelCodes.h"
#include "TrigAccelEvent/TrigInDetAccelEDM.h"

#include <dlfcn.h>

////////// 
/// Constructor 
///// 

TrigInDetAccelerationSvc::TrigInDetAccelerationSvc( const std::string& name, ISvcLocator* pSvcLocator) : 
  base_class( name, pSvcLocator ),
  m_nDCs(12),
  m_moduleName("libTrigInDetCUDA.so"),
  m_libHandle(0),
  m_pWF(0),   
  m_detStore("DetectorStore", name),
  m_evtStore("StoreGateSvc",name), 
  m_factoryConfigured(false) {

  declareProperty( "NumberOfDCs", m_nDCs = 8 );
  declareProperty( "ModuleName", m_moduleName = "libTrigInDetCUDA.so");
} 


////////// 
/// Initialize 
///// 

StatusCode TrigInDetAccelerationSvc::initialize() {   

  ATH_MSG_INFO("TrigInDetAccelerationSvc: initialize");
  
  ATH_CHECK (m_evtStore.retrieve() );
  ATH_CHECK (m_detStore.retrieve() );

  //load the OffloadFactory library

  m_libHandle = dlopen(m_moduleName.c_str(), RTLD_LAZY);
  
  if(!m_libHandle) {
    ATH_MSG_INFO("TrigInDetAccelerationSvc: cannot load the factory library, error:"<<dlerror());
    return StatusCode::SUCCESS;
  }

  dlerror();

  //declare library interface methods 

  TrigAccel::WorkFactory* (*getFactory)();

  getFactory   = (TrigAccel::WorkFactory* (*)()) dlsym(m_libHandle, "getFactory");

  m_pWF = getFactory();
  
  bool cfgResult = m_pWF->configure();

  if(!cfgResult) {
    ATH_MSG_INFO("OffloadFactory config failed");
    m_factoryConfigured = false;
    return StatusCode::SUCCESS;
  }

  m_factoryConfigured = true;
  
  for(int i=0;i<3;i++) m_layerInfo[i].clear();

  ATH_MSG_INFO("TrigInDetAccelerationSvc: created OffloadFactory, factory id = "<<std::hex<<m_pWF->getFactoryId()<<std::dec);
  
  /*    
   * Ask to be informed at the beginning of a new run so that we    
   * can collect geometry, conditions, etc. and copy them to on-GPU data structures   
   */   

  IIncidentSvc* incsvc;   
  StatusCode sc = service("IncidentSvc", incsvc);   
  int priority = 100;   
  if( sc.isSuccess() ) {     
    incsvc->addListener( this, "BeginRun", priority);   
  }
  
  return StatusCode::SUCCESS;
} 

///////// 
/// Finalize 
///// 
StatusCode TrigInDetAccelerationSvc::finalize() {

  void (*deleteFactory)(TrigAccel::WorkFactory*);
  deleteFactory = (void (*)(TrigAccel::WorkFactory*)) dlsym(m_libHandle, "deleteFactory");

  deleteFactory(m_pWF);

  dlclose(m_libHandle);

  return StatusCode::SUCCESS; 
}


/////////  
/// OnBeginRun data gathering and export 
/////

void TrigInDetAccelerationSvc::handle(const Incident&) {    

  ATH_MSG_INFO("OnBeginRun ");

  std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> > hashMap;
  
  if(!extractGeometryInformation(hashMap)) {
    ATH_MSG_INFO("Geometry extraction failed");
    return;
  }
  
  if(!exportGeometryInformation(hashMap)) {
    ATH_MSG_INFO("Geometry export failed");
  }

  return; 
}

TrigAccel::Work* TrigInDetAccelerationSvc::createWork(unsigned int jobCode, std::shared_ptr<TrigAccel::OffloadBuffer> pB) const {
  
  if(!m_factoryConfigured) return 0;

  return m_pWF->createWork(jobCode, pB);

}


const std::vector<short>& TrigInDetAccelerationSvc::getLayerInformation(int i) const {
  if(i<0 || i>2) i = 0;
  return m_layerInfo[i];
}


bool TrigInDetAccelerationSvc::exportGeometryInformation(const std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >& hashMap) const {
  
  const InDetDD::SCT_DetectorManager *  sct_mgr;
  const InDetDD::PixelDetectorManager * pix_mgr;

  if (m_detStore->retrieve(sct_mgr, "SCT").isFailure()) {
    ATH_MSG_WARNING("failed to get SCT Manager");
    return false;
    
  } 
  if (m_detStore->retrieve(pix_mgr,"Pixel").isFailure()) {
    ATH_MSG_WARNING("failed to get SCT Manager");
    return false;
  } 

  //export layer structure

  size_t dataTypeSize = sizeof(TrigAccel::DETECTOR_MODEL);
  const size_t bufferOffset = 256; 
  size_t totalSize = bufferOffset + dataTypeSize;

  TrigAccel::DATA_EXPORT_BUFFER* pBG = new TrigAccel::DATA_EXPORT_BUFFER(5000);
  
  if(!pBG->fit(totalSize)) pBG->reallocate(totalSize);

  TrigAccel::DETECTOR_MODEL* pArray = reinterpret_cast<TrigAccel::DETECTOR_MODEL*>(pBG->m_buffer + bufferOffset);

  memset(pArray,0,dataTypeSize);
  
  int nLayers = (int)hashMap.size();
  pArray->m_nLayers = nLayers;
  pArray->m_nModules=0;
  
  int layerIdx=0;

  for(std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::const_iterator it = hashMap.begin();it!=hashMap.end();++it, layerIdx++) {
    short subdetid = std::get<0>((*it).first);
    short barrel_ec = std::get<1>((*it).first);
    
    pArray->m_layers[layerIdx].m_nElements = 0;
    pArray->m_layers[layerIdx].m_subdet = subdetid;
    pArray->m_layers[layerIdx].m_type = barrel_ec;
    
    std::vector<std::vector<PhiEtaHash>::const_iterator> vStops;
    vStops.push_back((*it).second.begin());
    std::vector<PhiEtaHash>::const_iterator firstIt = (*it).second.begin();
    std::vector<PhiEtaHash>::const_iterator nextIt = (*it).second.begin();
    ++nextIt;

    int nPhiSlices=0;

    for(; nextIt!=(*it).second.end();++nextIt, ++firstIt) {
      if((*nextIt).m_phiIndex!=(*firstIt).m_phiIndex) {
        vStops.push_back(nextIt);
        nPhiSlices++;
      }
    }
    
    nPhiSlices++;
    
    vStops.push_back((*it).second.end());

    float rc=0.0;
    float minBound = 100000.0;
    float maxBound =-100000.0;

    pArray->m_layers[layerIdx].m_nPhiSlices = nPhiSlices;
    
    //3. iterating over phi sectors

    for(unsigned int iStops = 1; iStops<vStops.size();iStops++) {

      int nPhiModules = 0;

      bool first = true;

      for(std::vector<PhiEtaHash>::const_iterator hIt = vStops[iStops-1];hIt!=vStops[iStops];++hIt, nPhiModules++) {

        pArray->m_hashArray[pArray->m_nModules] = (*hIt).m_hash;

        const InDetDD::SiDetectorElement *p = (subdetid==1) ? pix_mgr->getDetectorElement((*hIt).m_hash) : sct_mgr->getDetectorElement((*hIt).m_hash);

        if(first) {
          first = false;
        }

        pArray->m_layers[layerIdx].m_nElements++;

        const Amg::Vector3D& C = p->center();
        if(barrel_ec == 0) {
          rc += sqrt(C(0)*C(0)+C(1)*C(1));
          if(p->zMin() < minBound) minBound = p->zMin();
          if(p->zMax() > maxBound) maxBound = p->zMax();
          pArray->m_minRZ[pArray->m_nModules] = p->zMin();
          pArray->m_maxRZ[pArray->m_nModules] = p->zMax();
        }
        else {
          rc += C(2);
          if(p->rMin() < minBound) minBound = p->rMin();
          if(p->rMax() > maxBound) maxBound = p->rMax();        
          pArray->m_minRZ[pArray->m_nModules] = p->rMin();
          pArray->m_maxRZ[pArray->m_nModules] = p->rMax();
        }

	pArray->m_nModules++;
      }

    }
    pArray->m_layers[layerIdx].m_refCoord = rc/pArray->m_layers[layerIdx].m_nElements;
    pArray->m_layers[layerIdx].m_minBound = minBound;
    pArray->m_layers[layerIdx].m_maxBound = maxBound;
  }

  std::shared_ptr<TrigAccel::OffloadBuffer> pDMBuff = std::make_shared<TrigAccel::OffloadBuffer>(pBG);

  delete pBG;

  ATH_MSG_INFO("Creating Work item for task "<<TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT);

  TrigAccel::Work* pW = createWork(TrigAccel::InDetJobControlCode::SIL_LAYERS_EXPORT, pDMBuff);

  return pW == 0;//request is actioned immediately, no actual WorkItem is created

}



bool TrigInDetAccelerationSvc::extractGeometryInformation(std::map<std::tuple<short,short,short>, std::vector<PhiEtaHash> >& hashMap) {

  const PixelID* pixelId;
  const SCT_ID* sctId;

  if (m_detStore->retrieve(pixelId, "PixelID").isFailure()) {
    ATH_MSG_WARNING("Could not get Pixel ID helper");
    return false;
  }
  
  if (m_detStore->retrieve(sctId, "SCT_ID").isFailure()) { 
    ATH_MSG_WARNING("Could not get Pixel ID helper");
    return false;
  }
  
  short subdetid = 1;

  for(int hash = 0; hash<(int)pixelId->wafer_hash_max(); hash++) {

    Identifier offlineId = pixelId->wafer_id(hash);

    if(offlineId==0) continue;
 
    short barrel_ec = pixelId->barrel_ec(offlineId);
    if(abs(barrel_ec)>2) continue;//no DBM needed
    short layer_disk = pixelId->layer_disk(offlineId);
    short phi_index = pixelId->phi_module(offlineId);
    short eta_index = pixelId->eta_module(offlineId);
    auto t = std::make_tuple(subdetid, barrel_ec, layer_disk); 
    std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.find(t);
    if(it==hashMap.end())
      hashMap.insert(std::pair<std::tuple<short,short,short>,std::vector<PhiEtaHash> >(t,std::vector<PhiEtaHash>(1, PhiEtaHash(phi_index, eta_index, hash) )));
    else (*it).second.push_back(PhiEtaHash(phi_index, eta_index, hash));
  }
  subdetid = 2;
  for(int hash = 0; hash<(int)sctId->wafer_hash_max(); hash++) {

    Identifier offlineId = sctId->wafer_id(hash);

    if(offlineId==0) continue;
 
    short barrel_ec = sctId->barrel_ec(offlineId);
    short layer_disk = sctId->layer_disk(offlineId);
    short phi_index = sctId->phi_module(offlineId);
    short eta_index = sctId->eta_module(offlineId);

    auto t = std::make_tuple(subdetid, barrel_ec, layer_disk); 
    std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.find(t);
    if(it==hashMap.end())
      hashMap.insert(std::pair<std::tuple<short,short,short>,std::vector<PhiEtaHash> >(t,std::vector<PhiEtaHash>(1, PhiEtaHash(phi_index, eta_index, hash))));
    else (*it).second.push_back(PhiEtaHash(phi_index, eta_index, hash));
  }

  m_layerInfo[1].resize(pixelId->wafer_hash_max(), -1);
  m_layerInfo[2].resize(sctId->wafer_hash_max(), -1);

  int layerId=0;

  for(std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.begin();it!=hashMap.end();++it, layerId++) {

    short subdetId = std::get<0>((*it).first);
    short barrel_ec = std::get<1>((*it).first);

    m_layerInfo[0].push_back(barrel_ec);

    if(subdetId == 1) {//pixel
      for(std::vector<PhiEtaHash>::iterator hIt = (*it).second.begin();hIt != (*it).second.end();++hIt) {
        m_layerInfo[subdetId][(*hIt).m_hash] = layerId;
      }
    }
    if(subdetId == 2) {//SCT
      for(std::vector<PhiEtaHash>::iterator hIt = (*it).second.begin();hIt != (*it).second.end();++hIt) {
        m_layerInfo[subdetId][(*hIt).m_hash] = layerId;
      }
    }
  }
  
  
  for(std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.begin();it!=hashMap.end();++it) {

    //sorting along phi first, then along eta

    std::sort((*it).second.begin(), (*it).second.end(), PhiEtaHash::compare());

  }
  return true;
}
