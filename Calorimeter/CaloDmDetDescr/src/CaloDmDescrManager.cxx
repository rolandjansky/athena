/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmDescrManager.cxx
 Author      : guennadi.pospelov@cern.ch
 Created     : March, 2007
 DESCRIPTION : dead material identifiers description package
 Usage:
 const CaloDmDescrManager *mgr = CaloDmDescrManager::instance();
 CaloDmDescrElement *element = mgr->get_element(dm_identifier);
 Enjoy with element->eta(), element->phi().
**************************************************************************** */
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "AthenaKernel/getMessageSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#define MAX_BUFFER_LEN 1024
#undef DEBUG

/* **************************************************************************

************************************************************************** */
CaloDmDescrManager::CaloDmDescrManager()
  : m_caloDM_ID (nullptr),
    m_id_helper (nullptr)
{
  initialize();
}



/* **************************************************************************

************************************************************************** */
CaloDmDescrManager::~CaloDmDescrManager()
{
  for(unsigned int i = 0; i<m_DmRegionVector.size(); i++) delete m_DmRegionVector[i];
  m_DmRegionVector.clear();

  for(unsigned int i = 0; i<m_DmElementVector.size(); i++) delete m_DmElementVector[i];
  m_DmElementVector.clear();

}



/* **************************************************************************

************************************************************************** */
const CaloDmDescrManager* CaloDmDescrManager::instance()
{
  static const CaloDmDescrManager s_instance;
  return &s_instance;
}


/* **************************************************************************

************************************************************************** */
int CaloDmDescrManager::initialize()
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmDescrManager");

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("DetectorStore", m_detStore);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Unable to get pointer to StoreGate Service" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_caloDM_ID);
  if (sc.isFailure() || !m_caloDM_ID) {
    log << MSG::ERROR
        << "Unable to retrieve caloDM_ID helper from DetectorStore" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_id_helper);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve AtlasDetectorID helper from DetectorStore" << endmsg;
    return 1;
  }

  //  m_caloDM_ID->set_do_checks(true);
  m_DmRegionFileName = "DeadMaterialRegionDescription_1.10c.txt";
  sc = load_regions(m_DmRegionFileName);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to load DM regions from '" << m_DmRegionFileName << "' file."<< endmsg;
    return 1;
  }

  build_element_vector();

  return 0;
}



/* **************************************************************************

************************************************************************** */
void CaloDmDescrManager::show_element(const Identifier& cellId) const
{
  const CaloDmDescrElement *element = get_element(cellId);
  std::cout << "CaloDmDescrManager::check_element() -> Your identifier " << m_id_helper->show_to_string(cellId);
  element->print();
}



/* **************************************************************************
Get description element for dead material id.
************************************************************************** */
CaloDmDescrElement* CaloDmDescrManager::get_element(const Identifier& cellId) const
{
  IdentifierHash hash;
  if( m_caloDM_ID->is_lar(cellId) ) {
    hash = m_caloDM_ID->lar_zone_hash(cellId);
  } else if ( m_caloDM_ID->is_tile(cellId) ) {
    hash = m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash(cellId);
  } else {
    std::cout <<  "CaloDmDescrManager::get_element-> Error! Wrong dead material identifier! " << m_id_helper->show_to_string(cellId) << std::endl;
    return nullptr;
  }
  if(hash < m_DmElementVector.size()) {
    return m_DmElementVector[hash];
  } else {
    std::cout << "CaloDmDescrManager::get_element-> Error! Bad hash id for dead material identifier " << hash << std::endl;
    return nullptr; 
  }
}



/* **************************************************************************
Calculate dead material region 'key' number from identifier iof the dead
material calibration hit
************************************************************************** */
int CaloDmDescrManager::get_dm_key(const Identifier& cellId) const
{
  int subdet = m_caloDM_ID->pos_neg_z(cellId);
  int type = m_caloDM_ID->dmat(cellId);
  int sampling = m_caloDM_ID->sampling(cellId);
  int region = m_caloDM_ID->region(cellId);
  return abs(subdet)*1000+type*100+sampling*10+region;
}



/* **************************************************************************
return dead material area number, where area is a collection of dm hits in
some detector regions
************************************************************************** */
int CaloDmDescrManager::get_dm_area(const Identifier& cellId) const
{
  int key = get_dm_key(cellId);
  CaloDmDescrElement *el = get_element(cellId);
  return CaloDmDescrArea::getArea(key, el->eta());
}



/* **************************************************************************
Get DM region for DM hit identifier
************************************************************************** */
CaloDmRegion* CaloDmDescrManager::get_dm_region(const Identifier& cellId) const
{
  Identifier id_reg = m_caloDM_ID->region_id(cellId);
  IdentifierHash hash_reg;
  if( m_caloDM_ID->is_lar(cellId) ) {
    hash_reg = m_caloDM_ID->lar_region_hash(id_reg);
  } else if ( m_caloDM_ID->is_tile(cellId) ) {
    hash_reg = m_caloDM_ID->lar_region_hash_max() + m_caloDM_ID->tile_region_hash(id_reg);
  } else {
    std::cout <<  "CaloDmDescrManager::get_dm_region-> Error! Wrong dead material identifier! " << m_id_helper->show_to_string(cellId) << std::endl;
    return nullptr;
  }
  if(hash_reg < m_DmRegionVector.size()) {
    return m_DmRegionVector[hash_reg];
  } else {
    std::cout << "CaloDmDescrManager::get_dm_region-> Error! Bad region hash id!" << m_id_helper->show_to_string(cellId) << std::endl;
    return nullptr; 
  }
}



/* **************************************************************************
For given eta, phi coordinate and CaloDmRegion returns hash identifier of 
appropriate dead material calibration hit
************************************************************************** */
IdentifierHash CaloDmDescrManager::get_dm_hash(const CaloDmRegion *myRegion, float &eta, float &phi) const
{
  IdentifierHash id_hash;

  if(eta < myRegion->etaMin() || eta >= myRegion->etaMax()) {
#ifdef DEBUG
    std::cout << "              input eta out of range"
        << " eta: " << eta
        << " myRegion->m_key: " << myRegion->m_key
        << " myRegion->etaMin(): " << myRegion->etaMin()
        << " myRegion->etaMax(): " << myRegion->etaMax()
        << std::endl;
#endif
    return 0xFFFFFFFF;
  }
  int neta;
  neta=int((eta-myRegion->etaMin())/myRegion->deta());
  if(myRegion->m_side < 0) {
    neta = myRegion->netaMax() - neta - 1;
  }
  float xphi=phi;
  if(xphi<0.0) xphi=2*M_PI+xphi;
  int nphi= int(xphi/myRegion->dphi());
#ifdef DEBUG
  std::cout << "              key: " << myRegion->m_key
      << " side: " << myRegion->m_side
      << " etaMin: " << myRegion->etaMin()
      << " phiMax: " << myRegion->etaMax()
      << " neta: " << neta
      << " nphi: " << nphi
      << " myRegion->netaMax(): " << myRegion->netaMax()
      << " myRegion->nphiMax(): " << myRegion->nphiMax()
      << std::endl;
#endif
  if( (neta >=0 && neta < myRegion->netaMax()) && (nphi >=0 && nphi < myRegion->nphiMax()) ) {
    Identifier id = m_caloDM_ID->zone_id(myRegion->region_id(), neta, nphi);
    if( m_caloDM_ID->is_lar(id) ) {
      id_hash = m_caloDM_ID->lar_zone_hash(id);
    } else if ( m_caloDM_ID->is_tile(id) ) {
      id_hash = m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash(id);
    }else{
      std::cout <<  "CaloDmDescrManager::get_dm_hash() -> ERROR! p1." << std::endl;
      return 0xFFFFFFFF;
    }
  } else{
    std::cout <<  "CaloDmDescrManager::get_dm_hash() -> ERROR! p2."
        << " key: " << myRegion->m_key
        << " side: " << myRegion->m_side
        << " myRegion->etaMin(): " << myRegion->etaMin()
        << " myRegion->etaMax(): " << myRegion->etaMax()
        << " myRegion->deta(): " << myRegion->deta()
        << " eta: " << eta
        << " phi: " << phi
        << " neta: " << neta
        << " nphi: " << nphi
        << " myRegion->netaMax(): " << myRegion->netaMax()
        << " myRegion->nphiMax(): " << myRegion->nphiMax()
        << std::endl;
  }
  return id_hash;
}



/* **************************************************************************
Build vector of elements (eta, phi, deta, dphi) for all dead material id's
Total size of vector will be 
m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash_max()
************************************************************************** */
void CaloDmDescrManager::build_element_vector()
{
  if(!m_DmElementVector.empty()) {
    std::cout << "CaloDmDescrManager::build_element_list() -> Warning! Non empty list."
        << " m_DmElementVector.size(): " << m_DmElementVector.size()
        << std::endl;
    return;
  }

  m_DmElementVector.resize(m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash_max());

  // loop over all DM LAr identifiers
  for(unsigned int i=0; i<m_caloDM_ID->lar_zone_hash_max(); i++) {
    Identifier id = m_caloDM_ID->lar_zone_id(i);
    Identifier id_reg = m_caloDM_ID->region_id(id);
    IdentifierHash hash_reg = m_caloDM_ID->lar_region_hash(id_reg);
    m_DmElementVector[i] = build_element(id, m_DmRegionVector[hash_reg]);
  }
  // loop over tile identifier
  for(unsigned int i=0; i<m_caloDM_ID->tile_zone_hash_max(); i++) {
    Identifier id = m_caloDM_ID->tile_zone_id(i);
    Identifier id_reg = m_caloDM_ID->region_id(id);
    IdentifierHash hash_reg = m_caloDM_ID->lar_region_hash_max() + m_caloDM_ID->tile_region_hash(id_reg);
    m_DmElementVector[m_caloDM_ID->lar_zone_hash_max() + i] = build_element(id, m_DmRegionVector[hash_reg]);
  }
}



/* **************************************************************************
Create element corresponded to given DM id.
************************************************************************** */
CaloDmDescrElement* CaloDmDescrManager::build_element(const Identifier& id, const CaloDmRegion *myRegion)
{
  int neta = m_caloDM_ID->eta(id);
  int nphi = m_caloDM_ID->phi(id);
  int subdet = m_caloDM_ID->pos_neg_z(id);
  float eta;
  if(subdet>0){
    eta = myRegion->etaMin() + (0.5+neta)*myRegion->deta();
  } else {
    eta = myRegion->etaMax() - (0.5+neta)*myRegion->deta();
  }
  float phi = myRegion->m_dphi*(0.5+nphi);
  // !!! temporary hack FIX ME (used only for 14.1.0 simulation)
//   if( (myRegion->m_key == 4123 || myRegion->m_key == 4124) && fabs(eta)<2.5) {
//     phi = phi*2.0;
//     if(phi > 2*M_PI) phi -= 2*M_PI;
//   }
  // eof FIX ME
  if(phi > M_PI) phi -= 2*M_PI; // we need to have phi=-M_PI,M_PI
  float theta = 2.*atan(exp(-eta));
  float rho, z;
  if(myRegion->m_isBarrel) {
    rho=myRegion->m_distance*0.1;
    z=rho/fabs(tan(theta));
  }else{
    z=myRegion->m_distance*0.1;
    rho=z*fabs(tan(theta));
  }
  // building new element which will containt real eta, phi values for this identifier
  CaloDmDescrElement *element = new CaloDmDescrElement(eta, myRegion->m_deta, phi, myRegion->m_dphi, rho, z); 
  return element;
}



/* **************************************************************************
Load private Dead Material Regions definition from text file
************************************************************************** */
StatusCode CaloDmDescrManager::load_regions(const std::string& DmRegionFileName)
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmDescrManager");

  char cLine[MAX_BUFFER_LEN];

  if(!m_DmRegionVector.empty()) {
    log << MSG::WARNING << "CaloDmDescrManager::build_element_list() -> Warning! Non empty list."
        << " m_DmRegionVector.size(): " << m_DmRegionVector.size()
        << endmsg;
    return StatusCode::FAILURE;
  }

  // Find the full path to filename:
  std::string file = PathResolver::find_file (DmRegionFileName, "DATAPATH");
  log << MSG::INFO << "Reading file  " << file << endmsg;

  std::ifstream fin(file.c_str());
  if(!fin){
    log << MSG::ERROR << "Can't open file " << file << endmsg;
    return StatusCode::FAILURE;
  }

  m_DmRegionVector.resize(m_caloDM_ID->lar_region_hash_max() + m_caloDM_ID->tile_region_hash_max(), nullptr);
  std::string sLine;
  while(fin.getline(cLine,sizeof(cLine)-1)){
    if( strlen(cLine)==0 ||  cLine[0] == '#' || cLine[0] == '\n') continue;
    sLine = std::string(cLine);
    std::istringstream mypars( sLine.c_str() );

    int key, distance, isBarrel;
    float etamin, etamax, deta, dphi;
    if(mypars >> key >> etamin >> etamax >> deta >> dphi >> distance >> isBarrel) {
      fin.getline(cLine,sizeof(cLine)-1);
      sLine = std::string(cLine);
      std::istringstream mynei( sLine.c_str() );
      std::string sKey;
      std::vector<short> neighbours;
      std::vector<float> CaloEtaMinVector;
      std::vector<float> CaloEtaMaxVector;
      while (mynei >> sKey) {
        if(sKey.find("CaloSampleNeighbors") != std::string::npos) continue;
        if(sKey.find("Unknown") != std::string::npos) continue;
        float caloEtaMin, caloEtaMax;
        CaloCell_ID::CaloSample nsmp = get_calo_sample(sKey, caloEtaMin, caloEtaMax);
        if(nsmp != CaloCell_ID::Unknown) {
          neighbours.push_back((int)nsmp);
          CaloEtaMinVector.push_back(caloEtaMin);
          CaloEtaMaxVector.push_back(caloEtaMax);
        } else {
          std::cout << "CaloDmDescrManager::load_regions() -> Fromat ERROR in line '" << sLine << "', key '" << sKey << "'" << std::endl;
          return StatusCode::FAILURE;
        }
      } // while over sKey

      int subdet=key/1000;
      int type=(key-subdet*1000)/100;
      int sampling=(key-subdet*1000-type*100)/10;
      int region=(key-subdet*1000-type*100-sampling*10)/1;
      if(subdet != 4 && subdet !=5) {
        log << MSG::ERROR << " Error! Wrong subdet " << subdet << ". Format error in line '" << sLine << "', key '" << sKey << "'" << std::endl;
        return StatusCode::FAILURE;
      }

      Identifier id_side_neg = m_caloDM_ID->region_id(-subdet, type, sampling, region);
      Identifier id_side_pos = m_caloDM_ID->region_id(subdet, type, sampling, region);

      IdentifierHash hash_side_neg;
      IdentifierHash hash_side_pos;
      if(subdet == 4) {
        // lar region
        hash_side_neg = m_caloDM_ID->lar_zone_hash(id_side_neg);
        hash_side_pos  = m_caloDM_ID->lar_zone_hash(id_side_pos);
      } else if (subdet == 5){
        // tile region has shifted position in our vectors
        hash_side_neg = m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash(id_side_neg);
        hash_side_pos  = m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash(id_side_pos);
      }

      // Making DM region for positive side
      CaloDmRegion *myRegionPos = new CaloDmRegion();
      myRegionPos->m_key=key;
      myRegionPos->m_side=1;
      myRegionPos->m_eta_min=etamin;
      myRegionPos->m_eta_max=etamax;
      myRegionPos->m_deta=deta;
      myRegionPos->m_neta_max=int((etamax-etamin)/deta+0.00001);
      myRegionPos->m_dphi=dphi*(M_PI/3.2);
      myRegionPos->m_nphi_max=int(M_PI*2./myRegionPos->m_dphi+0.00001);
      myRegionPos->m_distance=distance;
      myRegionPos->m_isBarrel=true;
      if(!isBarrel) myRegionPos->m_isBarrel=false;
      // assigning of information over neighboring calo samplings for this DM region
      myRegionPos->m_CaloSampleNeighbours.clear();
      myRegionPos->m_CaloSampleNeighbours = neighbours;
      myRegionPos->m_CaloSampleEtaMin.clear();
      myRegionPos->m_CaloSampleEtaMin = CaloEtaMinVector;
      myRegionPos->m_CaloSampleEtaMax.clear();
      myRegionPos->m_CaloSampleEtaMax = CaloEtaMaxVector;
      // hash index of first DM identifier for this DM region
      myRegionPos->m_region_id = id_side_pos;
      myRegionPos->m_region_hash = hash_side_pos;

      // Now making DM region for negative side
      CaloDmRegion *myRegionNeg = new CaloDmRegion(*myRegionPos);
      myRegionNeg->m_side = -1 * myRegionPos->m_side;
      myRegionNeg->m_eta_min = -1 * myRegionPos->m_eta_max;
      myRegionNeg->m_eta_max = -1 * myRegionPos->m_eta_min;
      myRegionNeg->m_region_id = id_side_neg;
      myRegionNeg->m_region_hash = hash_side_neg;
      myRegionNeg->m_CaloSampleEtaMin.clear();
      myRegionNeg->m_CaloSampleEtaMax.clear();
      for(unsigned int i=0; i<myRegionPos->m_CaloSampleNeighbours.size(); i++){
        myRegionNeg->m_CaloSampleEtaMin.push_back( -1.0 * myRegionPos->m_CaloSampleEtaMax[i] );
        myRegionNeg->m_CaloSampleEtaMax.push_back( -1.0 * myRegionPos->m_CaloSampleEtaMin[i] );
      }

      // filling vectors 
      if(subdet == 4) {
        m_DmRegionVector[m_caloDM_ID->lar_region_hash(id_side_neg)] = myRegionNeg;
        m_DmRegionVector[m_caloDM_ID->lar_region_hash(id_side_pos)] = myRegionPos;
      } else if (subdet == 5){
        int shift_reg = m_caloDM_ID->lar_region_hash_max();
        m_DmRegionVector[shift_reg + m_caloDM_ID->tile_region_hash(id_side_neg)] = myRegionNeg;
        m_DmRegionVector[shift_reg + m_caloDM_ID->tile_region_hash(id_side_pos)] = myRegionPos;
      }
      // skipping region description
      while(fin.getline(cLine,sizeof(cLine)-1)) {
        if(cLine[0] == '}') break; 
      }
    } else {
      std::cout << "CaloDmDescrManager::load_regions() -> Fromat ERROR in line '" << sLine << "'" << std::endl;
      return StatusCode::FAILURE;
    }
  } // while over lines in file
  fin.close();

  log << MSG::INFO << "Loaded " << m_DmRegionVector.size()  << " LAr+Tile dead material regions from " << file << endmsg;

//  std::vector<CaloDmRegion *>::const_iterator it = reg_begin();
//  int i_reg = 0;
//  for(; it!=reg_end(); it++){
//    const CaloDmRegion *myRegion = (*it);
//    std::cout << i_reg << "  key: " << myRegion->m_key
//        << "  side: " << myRegion->m_side
//        << "  neta X nphi: " << myRegion->netaMax() << " X " << myRegion->nphiMax()
//        << "  eta:min/max/deta: " << myRegion->etaMin() << " / " << myRegion->etaMax() << " / " << myRegion->deta()
//        << "  hash_id: " << myRegion->region_hash()
//        << " " << m_id_helper->show_to_string(myRegion->region_id())
//        << std::endl;
//    for(unsigned int i_smp=0; i_smp<myRegion->m_CaloSampleNeighbours.size(); i_smp++){
//      std::cout << "     i_smp: " << i_smp << " smp: " << myRegion->m_CaloSampleNeighbours[i_smp] << " smp_eta_min_max: " << myRegion->m_CaloSampleEtaMin[i_smp] << " " << myRegion->m_CaloSampleEtaMax[i_smp] << std::endl;
//    }
//  }

  return StatusCode::SUCCESS;
}



/* **************************************************************************
To get calo_sample number from calo_sample name
************************************************************************** */
CaloCell_ID::CaloSample CaloDmDescrManager::get_calo_sample(const std::string &SamplingName)
{
  if ( SamplingName == "PreSamplerB" ) {
    return CaloCell_ID::PreSamplerB;
  } else if ( SamplingName == "EMB1" ) {
    return CaloCell_ID::EMB1;
  } else if ( SamplingName == "EMB2" ) {
    return CaloCell_ID::EMB2;
  } else if ( SamplingName == "EMB3" ) {
    return CaloCell_ID::EMB3;
  } else if ( SamplingName == "PreSamplerE" ) {
    return CaloCell_ID::PreSamplerE;
  } else if ( SamplingName == "EME1" ) {
    return CaloCell_ID::EME1;
  } else if ( SamplingName == "EME2" ) {
    return CaloCell_ID::EME2;
  } else if ( SamplingName == "EME3" ) {
    return CaloCell_ID::EME3;
  } else if ( SamplingName == "HEC0" ) {
    return CaloCell_ID::HEC0;
  }else if ( SamplingName == "HEC1" ) {
    return CaloCell_ID::HEC1;
  }else if ( SamplingName == "HEC2" ) {
    return CaloCell_ID::HEC2;
  }else if ( SamplingName == "HEC3" ) {
    return CaloCell_ID::HEC3;
  }else if ( SamplingName == "TileBar0" ) { 
    return CaloCell_ID::TileBar0;
  }else if ( SamplingName == "TileBar1" ) {
    return CaloCell_ID::TileBar1;
  }else if ( SamplingName == "TileBar2" ) {
    return CaloCell_ID::TileBar2;
  }else if ( SamplingName == "TileGap1" ) {
    return CaloCell_ID::TileGap1;
  }else if ( SamplingName == "TileGap2" ) {
    return CaloCell_ID::TileGap2;
  }else if ( SamplingName == "TileGap3" ) {
    return CaloCell_ID::TileGap3;
  }else if ( SamplingName == "TileExt0" ) {
    return CaloCell_ID::TileExt0;
  }else if ( SamplingName == "TileExt1" ) {
    return CaloCell_ID::TileExt1;
  }else if ( SamplingName == "TileExt2" ) {
    return CaloCell_ID::TileExt2;
  }else if ( SamplingName == "FCAL0" ) {
     return CaloCell_ID::FCAL0;
  }else if ( SamplingName == "FCAL1" ) {
    return CaloCell_ID::FCAL1;
  }else if ( SamplingName == "FCAL2" ) {
    return CaloCell_ID::FCAL2;
  }
  std::cout << "Calorimeter sampling '" << SamplingName
      << "' is not a valid Calorimeter sampling name and will be ignored! " << std::endl;
  return CaloCell_ID::Unknown;
}



/* **************************************************************************
To get calo_sample number and eta values from calo_sample string
 - "PreSamplerB(0.0,1.5)" -> CaloCell_ID::PreSamplerB, 0.0, 1.5
************************************************************************** */
CaloCell_ID::CaloSample CaloDmDescrManager::get_calo_sample(const std::string &SamplingName, float &etaMin, float &etaMax)
{
  CaloCell_ID::CaloSample nsmp = CaloCell_ID::Unknown;
  if ( SamplingName.find("PreSamplerB") != std::string::npos ) {
    nsmp = CaloCell_ID::PreSamplerB;
  } else if ( SamplingName.find("EMB1") != std::string::npos ) {
    nsmp = CaloCell_ID::EMB1;
  } else if ( SamplingName.find("EMB2") != std::string::npos ) {
    nsmp = CaloCell_ID::EMB2;
  } else if ( SamplingName.find("EMB3") != std::string::npos ) {
    nsmp = CaloCell_ID::EMB3;
  } else if ( SamplingName.find("PreSamplerE") != std::string::npos ) {
    nsmp = CaloCell_ID::PreSamplerE;
  } else if ( SamplingName.find("EME1") != std::string::npos ) {
    nsmp = CaloCell_ID::EME1;
  } else if ( SamplingName.find("EME2") != std::string::npos ) {
    nsmp = CaloCell_ID::EME2;
  } else if ( SamplingName.find("EME3") != std::string::npos ) {
    nsmp = CaloCell_ID::EME3;
  } else if ( SamplingName.find("HEC0") != std::string::npos ) {
    nsmp = CaloCell_ID::HEC0;
  }else if ( SamplingName.find("HEC1") != std::string::npos ) {
    nsmp = CaloCell_ID::HEC1;
  }else if ( SamplingName.find("HEC2") != std::string::npos ) {
    nsmp = CaloCell_ID::HEC2;
  }else if ( SamplingName.find("HEC3") != std::string::npos ) {
    nsmp = CaloCell_ID::HEC3;
  }else if ( SamplingName.find("TileBar0") != std::string::npos ) { 
    nsmp = CaloCell_ID::TileBar0;
  }else if ( SamplingName.find("TileBar1") != std::string::npos ) {
    nsmp = CaloCell_ID::TileBar1;
  }else if ( SamplingName.find("TileBar2") != std::string::npos ) {
    nsmp = CaloCell_ID::TileBar2;
  }else if ( SamplingName.find("TileGap1") != std::string::npos ) {
    nsmp = CaloCell_ID::TileGap1;
  }else if ( SamplingName.find("TileGap2") != std::string::npos ) {
    nsmp = CaloCell_ID::TileGap2;
  }else if ( SamplingName.find("TileGap3") != std::string::npos ) {
    nsmp = CaloCell_ID::TileGap3;
  }else if ( SamplingName.find("TileExt0") != std::string::npos ) {
    nsmp = CaloCell_ID::TileExt0;
  }else if ( SamplingName.find("TileExt1") != std::string::npos ) {
    nsmp = CaloCell_ID::TileExt1;
  }else if ( SamplingName.find("TileExt2") != std::string::npos ) {
    nsmp = CaloCell_ID::TileExt2;
  }else if ( SamplingName.find("FCAL0") != std::string::npos ) {
    nsmp = CaloCell_ID::FCAL0;
  }else if ( SamplingName.find("FCAL1") != std::string::npos ) {
    nsmp = CaloCell_ID::FCAL1;
  }else if ( SamplingName.find("FCAL2") != std::string::npos ) {
    nsmp = CaloCell_ID::FCAL2;
  }else {
    std::cout << "CaloDmDescrManager::get_calo_sample() -> Calorimeter sampling '" << SamplingName
        << "' is not a valid Calorimeter sampling name and will be ignored! " << std::endl;
    return CaloCell_ID::Unknown;
  }
  char dummy;
  if(SamplingName.find('(') == std::string::npos || SamplingName.find(')') != SamplingName.size()-1 ){
    std::cout << "CaloDmDescrManager::get_calo_sample() -> Can't parse calorimeter sampling '" << SamplingName << "' string! Something with brackets..." << std::endl;
    return CaloCell_ID::Unknown;
  }
  std::string sEtaRange(SamplingName,SamplingName.find('(')+1);
  std::istringstream iEtaRange( sEtaRange.c_str() );
  if(iEtaRange >> etaMin >> dummy >> etaMax) {
    return nsmp;
  }else{
    std::cout << "CaloDmDescrManager::get_calo_sample() -> Can't parse calorimeter sampling '" << SamplingName << "' string!" << std::endl;
    return CaloCell_ID::Unknown;
  }
}
