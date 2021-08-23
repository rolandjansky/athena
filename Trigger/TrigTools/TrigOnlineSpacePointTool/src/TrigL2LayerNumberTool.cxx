/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"

#include "TrigL2LayerNumberTool.h"
#include "InDetReadoutGeometry/SiNumerology.h"

TrigL2LayerNumberTool::TrigL2LayerNumberTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AthAlgTool(t,n,p),
								     m_useNewScheme(false),
								     m_MaxSiliconLayerNum(-1),
								     m_OffsetEndcapPixels(-1),
								     m_OffsetBarrelSCT(-1),
								     m_OffsetEndcapSCT(-1)
{
  declareInterface< ITrigL2LayerNumberTool >( this );
  declareProperty( "UseNewLayerScheme",      m_useNewScheme = false );
}

StatusCode TrigL2LayerNumberTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO("In initialize...");

  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper"); 
    return sc;
  } 

  sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper"); 
    return sc;
  } 
 
  sc = detStore()->retrieve(m_pixelManager);  
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve Pixel DetectorManager from detStore."); 
    return sc;
  } 

  sc = detStore()->retrieve(m_sctManager);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve SCT DetectorManager from detStore.");
    return sc;
  } 

  //calculate the numbers

  if(!m_useNewScheme) {

    const InDetDD::SiNumerology& pixSiNum = m_pixelManager->numerology(); 
    const InDetDD::SiNumerology& sctSiNum = m_sctManager->numerology(); 

    m_MaxSiliconLayerNum = pixSiNum.numLayers()+pixSiNum.numDisks()+sctSiNum.numLayers()+sctSiNum.numDisks();
    m_OffsetBarrelSCT = pixSiNum.numLayers();
    m_OffsetEndcapPixels = pixSiNum.numLayers()+sctSiNum.numLayers();
    m_LastBarrelLayer = m_OffsetEndcapPixels - 1;
    m_OffsetEndcapSCT = pixSiNum.numLayers()+sctSiNum.numLayers()+pixSiNum.numDisks();
  }
  else {
    createModuleHashMap(m_hashMap);

    ATH_MSG_INFO("Total number of unique silicon layers = "<<m_hashMap.size());

    m_MaxSiliconLayerNum = (int)m_hashMap.size();
    m_OffsetEndcapPixels = m_LastBarrelLayer;
  }

  
  ATH_MSG_INFO("TrigL2LayerNumberTool initialized ");

  report();

  return sc;
}

StatusCode TrigL2LayerNumberTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

void TrigL2LayerNumberTool::report() const {

  ATH_MSG_INFO("TrigL2 Layer numbering scheme:");
  ATH_MSG_INFO("Total number of layers = "<<maxSiliconLayerNum());
  ATH_MSG_INFO("OffsetEndcapPixels     = "<<offsetEndcapPixels());
  ATH_MSG_INFO("OffsetBarrelSCT        = "<<offsetBarrelSCT());
  ATH_MSG_INFO("OffsetEndcapSCT        = "<<offsetEndcapSCT());
}

void TrigL2LayerNumberTool::createModuleHashMap(std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >& hashMap) {

  short subdetid = 1;

  for(int hash = 0; hash<(int)m_pixelId->wafer_hash_max(); hash++) {

    Identifier offlineId = m_pixelId->wafer_id(hash);

    if(offlineId==0) continue;
 
    short barrel_ec = m_pixelId->barrel_ec(offlineId);
    if(abs(barrel_ec)>2) continue;//no DBM needed
    short layer_disk = m_pixelId->layer_disk(offlineId);
    short phi_index = m_pixelId->phi_module(offlineId);
    short eta_index = m_pixelId->eta_module(offlineId);
    //auto t = std::make_tuple(subdetid, barrel_ec, layer_disk); 

    auto t = std::make_tuple(barrel_ec==0 ? -100 : barrel_ec, subdetid, layer_disk); 

    std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.find(t);
    if(it==hashMap.end())
      hashMap.insert(std::pair<std::tuple<short,short,short>,std::vector<PhiEtaHash> >(t,std::vector<PhiEtaHash>(1, PhiEtaHash(phi_index, eta_index, hash) )));
    else (*it).second.push_back(PhiEtaHash(phi_index, eta_index, hash));
  }
  subdetid = 2;
  for(int hash = 0; hash<(int)m_sctId->wafer_hash_max(); hash++) {

    Identifier offlineId = m_sctId->wafer_id(hash);

    if(offlineId==0) continue;
 
    short barrel_ec = m_sctId->barrel_ec(offlineId);
    short layer_disk = m_sctId->layer_disk(offlineId);
    short phi_index = m_sctId->phi_module(offlineId);
    short eta_index = m_sctId->eta_module(offlineId);

    //    auto t = std::make_tuple(subdetid, barrel_ec, layer_disk); 
    
    auto t = std::make_tuple(barrel_ec==0 ? -100 : barrel_ec, subdetid, layer_disk); 

    std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.find(t);
    if(it==hashMap.end())
      hashMap.insert(std::pair<std::tuple<short,short,short>,std::vector<PhiEtaHash> >(t,std::vector<PhiEtaHash>(1, PhiEtaHash(phi_index, eta_index, hash))));
    else (*it).second.push_back(PhiEtaHash(phi_index, eta_index, hash));
  }

  m_pixelLayers.clear();
  m_sctLayers.clear();

  m_pixelLayers.resize(m_pixelId->wafer_hash_max(), -100);
  m_sctLayers.resize(m_sctId->wafer_hash_max(), -100);
  m_layerGeometry.resize(hashMap.size());

  int layerId=0;
  m_LastBarrelLayer=0;
  for(std::map<std::tuple<short,short,short>,std::vector<PhiEtaHash> >::iterator it = hashMap.begin();it!=hashMap.end();++it, layerId++) {

    short subdetId = std::get<1>((*it).first);
    short barrel_ec = std::get<0>((*it).first);
    if(barrel_ec==-100) barrel_ec = 0;
    //short layer_disc = std::get<2>((*it).first);

    if(barrel_ec == 0) m_LastBarrelLayer++;

    m_layerGeometry[layerId].m_type = barrel_ec;
    m_layerGeometry[layerId].m_subdet = subdetId;

    float rc=0.0;
    float minBound = 100000.0;
    float maxBound =-100000.0;
    int nModules = 0;

    for(std::vector<PhiEtaHash>::iterator hIt = (*it).second.begin();hIt != (*it).second.end();++hIt) {
   
      const InDetDD::SiDetectorElement *p{nullptr};

      if(subdetId == 1) {//pixel
	m_pixelLayers[(*hIt).m_hash] = layerId;
	p = m_pixelManager->getDetectorElement((*hIt).m_hash);
      }
      if(subdetId == 2) {//SCT
	m_sctLayers[(*hIt).m_hash] = layerId;
	p = m_sctManager->getDetectorElement((*hIt).m_hash);
      }
    
      if (p==nullptr) {
        ATH_MSG_ERROR("nullptr SiDetectorElement with idHash " << (*hIt).m_hash);
        continue;
      }
      const Amg::Vector3D& C = p->center();
      if(barrel_ec == 0) {
	rc += sqrt(C(0)*C(0)+C(1)*C(1));
	if(p->zMin() < minBound) minBound = p->zMin();
	if(p->zMax() > maxBound) maxBound = p->zMax();
      }
      else {
	rc += C(2);
	if(p->rMin() < minBound) minBound = p->rMin();
	if(p->rMax() > maxBound) maxBound = p->rMax();	
      }
      nModules++;
    }
    m_layerGeometry[layerId].m_refCoord = rc/nModules;
    m_layerGeometry[layerId].m_minBound = minBound;
    m_layerGeometry[layerId].m_maxBound = maxBound;
  }

  int M = (layerId-m_LastBarrelLayer)/2;

  ATH_MSG_INFO("List of unique layers in Pixel and SCT :");
  for(int l=0;l<layerId;l++) {

    int oldL = l;

    if(l>m_LastBarrelLayer-1) {
      oldL = l-m_LastBarrelLayer;
      oldL = oldL < M ? oldL : oldL - M;
      oldL += m_LastBarrelLayer;
    }

    if(m_layerGeometry[l].m_subdet==1) {
      ATH_MSG_INFO("Layer "<<l<<" ("<<oldL<<") : PIX, reference coordinate ="<< m_layerGeometry[l].m_refCoord<<" boundaries: "<<m_layerGeometry[l].m_minBound<<
		   " "<<m_layerGeometry[l].m_maxBound);
    }
    if(m_layerGeometry[l].m_subdet==2) {
      ATH_MSG_INFO("Layer "<<l<<" ("<<oldL<<") : SCT, reference coordinate ="<< m_layerGeometry[l].m_refCoord<<" boundaries: "<<m_layerGeometry[l].m_minBound<<
		   " "<<m_layerGeometry[l].m_maxBound);
    }
  }
}
