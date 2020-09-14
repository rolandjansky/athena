/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmNeighbours.h
 Author      : guennadi.pospelov@cern.ch
 Created     : April, 2007
 DESCRIPTION : Initialize and provide access to neighbours accross
               the calorimeter systems for dead material calibration hits.
**************************************************************************** */

#include "CaloCalibHitRec/CaloDmNeighbours.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/getMessageSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArID_Exception.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloDmDetDescr/CaloDmDescrManager.h"


#define MAX_BUFFER_LEN 1024

#define CALOMAP_DETA 0.1
#define CALOMAP_NETA 100
#define CALOMAP_ETA_MIN (-5.0)
#undef DEBUG


/* **************************************************************************
CaloDmNeighbours::CaloDmNeighbours() ctor
************************************************************************** */
CaloDmNeighbours::CaloDmNeighbours(const CaloDmDescrManager *dmMgr)
  : m_caloDM_ID(nullptr),
    m_caloCell_ID(nullptr),
    m_larFcal_ID(nullptr),
    m_larHec_ID(nullptr),
    m_id_helper(nullptr)
{
  m_caloDmDescrManager = dmMgr;
  m_DmNeighboursFileName = "DeadMaterialCaloNeighbours.dat";
  initialize(m_DmNeighboursFileName);
}


CaloDmNeighbours::~CaloDmNeighbours()
{

}



/* **************************************************************************
initialization
************************************************************************** */
int CaloDmNeighbours::initialize(std::string DmNeighboursFileName)
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmNeighbours");

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("DetectorStore", m_detStore);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Unable to get pointer to StoreGate Service" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_caloDM_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve caloDM_ID helper from DetectorStore" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_caloCell_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve caloCell_ID helper from DetectorStore" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_larFcal_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve larFcal_ID helper from DetectorStore" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_larHec_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve larHec_ID helper from DetectorStore" << endmsg;
    return 1;
  }

  sc = m_detStore->retrieve(m_id_helper);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve AtlasDetectorID helper from DetectorStore" << endmsg;
    return 1;
  }

//   semiautomatic generation of DeadMaterialCaloNeighbours.dat
//   generate_neighbour_file();
//   return StatusCode::SUCCESS;

  DmNeighboursFileName.clear();
//  sc = load_neighbours(DmNeighboursFileName);
//  if (sc.isFailure()) {
//    log << MSG::ERROR
//        << "Unable to load neighbours from file " << DmNeighboursFileName << endmsg;
//    return sc;
//  }

  // Filling m_CaloSample2DmRegion[csmp][netabin][n_dm_region] containing list of 
  // DmRegion for given CaloSample at given eta. This map is used to find DM 
  // neighbours for calo cells.
  make_CaloSample2DmRegion_map();

  return 0;
}



/* **************************************************************************
filling vector of dead material hash_identifiers for given CaloCell identifier
************************************************************************** */
int CaloDmNeighbours::getNeighbours_DmHitsForCaloCell(const Identifier &cell_id,  std::vector<IdentifierHash>& neighbourList) const
{
  neighbourList.clear();

  const CaloDetDescrElement* theCDDE = getMgr()->get_element(cell_id);
  if (!theCDDE) return 1;
  float cell_eta = theCDDE->eta();
  int neta=int((cell_eta - CALOMAP_ETA_MIN)*(1./CALOMAP_DETA));
  if(neta>=CALOMAP_NETA || neta <0) {
    std::cout << " ERROR! Wrong neta" << std::endl;
    std::cout << "     " << m_id_helper->show_to_string(cell_id)
        << " csmp: " << m_caloCell_ID->calo_sample(cell_id)
        << " eta: " << theCDDE->eta()
        << " phi: " << theCDDE->phi()
        << " neta_bin: " << neta
        << " map_size: " << m_CaloSample2DmRegion[m_caloCell_ID->calo_sample(cell_id)][neta].size()
        << std::endl;
    return 1;
  }
  float cell_phi = theCDDE->phi();
  int cell_csmp = m_caloCell_ID->calo_sample(cell_id);

  for(unsigned int i=0; i<m_CaloSample2DmRegion[cell_csmp][neta].size(); i++){
    const CaloDmRegion *myRegion = m_CaloSample2DmRegion[cell_csmp][neta][i];
    IdentifierHash dm_hash_id = m_caloDmDescrManager->get_dm_hash(myRegion, cell_eta, cell_phi);
    if(dm_hash_id.is_valid() )  {
      neighbourList.push_back(dm_hash_id);
#ifdef DEBUG
      Identifier id;
      if(dm_hash_id < m_caloDM_ID->lar_zone_hash_max()){
        id=m_caloDM_ID->lar_zone_id(dm_hash_id);
      }else{
        id=m_caloDM_ID->tile_zone_id(dm_hash_id - m_caloDM_ID->lar_zone_hash_max());
      }
      std::cout << "        we got: "
          << " dm_hash_id: " << dm_hash_id
          << " " << m_id_helper->show_to_string(id) << std::endl;
#endif
    }else{
#ifdef DEBUG
      std::cout << "CaloDmNeighbours::getNeighbours_DmHitsForCaloCell ->  Error! Wrong hash "
          << " dm_hash_id: " << dm_hash_id
          << " cell_csmp: " << cell_csmp
          << " cell_eta: " << cell_eta
          << " cell_phi: " << cell_phi
          << " neta_bin: " << neta
          << " key: " << myRegion->m_key
          << " side: " << myRegion->m_side
          << " myRegion->etaMin() : " << myRegion->etaMin()
          << " myRegion->etaMax() : " << myRegion->etaMax()
          << std::endl;
#endif
    }

  }
  return 0;
}



/* **************************************************************************
filling vector of neighbors
************************************************************************** */
int CaloDmNeighbours::getNeighbours_CaloCellsForDmHit(const IdentifierHash &dmHashID,  std::vector<IdentifierHash>& neighbourList) const
{
  neighbourList.clear();
  std::cout << " Not realised yet " << dmHashID << std::endl;
  return 0;
}



/* **************************************************************************
Parsing
************************************************************************** */
StatusCode CaloDmNeighbours::unfold_neighbours(std::string &MySourceString, std::string &MyTargetString)
{
#ifdef DEBUG
  std::cout << "process_neighbours(), our pairs ->" << MySourceString << " " << MyTargetString << std::endl;
#endif

  Range MySourceRange, MyTargetRange;
  MySourceRange.build(MySourceString);
  MyTargetRange.build(MyTargetString);
//   MySourceRange.show();
//   MyTargetRange.show();
//   if(MySourceRange.fields() != 7) {
//     std::cout << " CaloDmNeighbours::process_neighbours() -> Error. Number of fields in DM Range != 7!" << std::endl;
//     return StatusCode::FAILURE;
//   }
//   std::cout << "fields: " << MySourceRange.fields() << std::endl;
//   for(unsigned int i=0; i<MySourceRange.fields(); i++){
//     std::cout << " i:" << i << " MySourceRange[i].get_indices()" << MySourceRange[i].get_indices() << std::endl;
//     for(unsigned int j=0; j<MySourceRange[i].get_indices(); j++){
//       std::cout << "YYY " << MySourceRange[i].get_value_at(j) << std::endl;
//     }
//   }

  ExpandedIdentifier idExpSource = MySourceRange.minimum();
  int indxSideSource, indxEtaSource, indxPhiSource;
  get_indexes(idExpSource, indxSideSource, indxEtaSource, indxPhiSource);

//   std::cout << (std::string)idExpSource << "   indxSideSource: " << indxSideSource
//       << " tot,min,max:" << MySourceRange[indxSideSource].get_indices() 
//       << "," << MySourceRange[indxSideSource].get_minimum()
//       << "," << MySourceRange[indxSideSource].get_maximum()
//       << "   indxEtaSource: " << indxEtaSource
//       << " tot,min,max:" << MySourceRange[indxEtaSource].get_indices() 
//       << "," << MySourceRange[indxEtaSource].get_minimum()
//       << "," << MySourceRange[indxEtaSource].get_maximum() 
//       << "   indxPhiSource: " << indxPhiSource
//       << " tot,min,max:" << MySourceRange[indxPhiSource].get_indices() 
//       << "," << MySourceRange[indxPhiSource].get_minimum()
//       << "," << MySourceRange[indxPhiSource].get_maximum() 
//       << std::endl;

  ExpandedIdentifier idExpTarget = MyTargetRange.minimum();
  int indxSideTarget, indxEtaTarget, indxPhiTarget;
  get_indexes(idExpTarget, indxSideTarget, indxEtaTarget, indxPhiTarget);

//   std::cout << (std::string)idExpTarget <<  "   indxSideTarget: " << indxSideTarget
//       << " tot,min,max:" << MyTargetRange[indxSideTarget].get_indices() 
//       << "," << MyTargetRange[indxSideTarget].get_minimum()
//       << "," << MyTargetRange[indxSideTarget].get_maximum()
//       << "   indxEtaTarget: " << indxEtaTarget
//       << " tot,min,max:" << MyTargetRange[indxEtaTarget].get_indices() 
//       << "," << MyTargetRange[indxEtaTarget].get_minimum()
//       << "," << MyTargetRange[indxEtaTarget].get_maximum() 
//       << "   indxPhiTarget: " << indxPhiTarget
//       << " tot,min,max:" << MyTargetRange[indxPhiTarget].get_indices() 
//       << "," << MyTargetRange[indxPhiTarget].get_minimum()
//       << "," << MyTargetRange[indxPhiTarget].get_maximum() 
//       << std::endl;

//   Range::identifier_factory first = MySourceRange.factory_begin();
//   Range::identifier_factory last  = MySourceRange.factory_end();
//   for (; first != last; ++first) {
//     ExpandedIdentifier idExpSource = *first;
//   }

    float xEta = ( MyTargetRange[indxEtaTarget].get_maximum() - MyTargetRange[indxEtaTarget].get_minimum() + 1.0) /
        ( MySourceRange[indxEtaSource].get_maximum() - MySourceRange[indxEtaSource].get_minimum() + 1.0);
    float xPhi = ( MyTargetRange[indxPhiTarget].get_maximum() - MyTargetRange[indxPhiTarget].get_minimum() + 1.0) /
        ( MySourceRange[indxPhiSource].get_maximum() - MySourceRange[indxPhiSource].get_minimum() + 1.0);

  if(int(xEta+0.00001)!=1 && int(xEta+0.00001)!=4 && int(xEta+0.00001)!=32) {
    std::cout << "CaloDmNeighbours::unfold_neighbours() -> WARNING! Wrong xEta " << xEta << std::endl;
//    return StatusCode::WARNINGWARNING;
  }
  if(int(xPhi+0.00001)!=1 && int(xPhi+0.00001) !=4) {
    std::cout << "CaloDmNeighbours::unfold_neighbours() -> WARNING! Wrong xPhi " << xPhi << std::endl;
//    return StatusCode::WARNINGWARNING;
  }

//  std::cout << " xEta: " << xEta << " " << int(xEta)
//      << " xPhi: " << xPhi << " " << int(xPhi) << std::endl;
//  std::cout << "XXX" << std::endl;

  const float inv_xPhi = 1. / xPhi;
  const float inv_xEta = 1. / xEta;

  for(Range::identifier_factory it1 = MySourceRange.factory_begin(); it1!=MySourceRange.factory_end(); ++it1){
    ExpandedIdentifier idExpSource = *it1;
    Identifier idSource;
    IdentifierHash idHashSource;
    if(get_id(idExpSource, idSource, idHashSource)==1) continue;
//    std::cout << "------------------------------------------------------" << std::endl;
//    std::cout << " Neighbours for " << (std::string)idExpSource;
    for(Range::identifier_factory it2 = MyTargetRange.factory_begin(); it2!=MyTargetRange.factory_end(); ++it2){
      ExpandedIdentifier idExpTarget = *it2;
      if(copysign(1,idExpSource[indxSideSource]) != idExpTarget[indxSideTarget] ) continue;
      int phiCalc = MySourceRange[indxPhiSource].get_minimum() + int((idExpTarget[indxPhiTarget] - MyTargetRange[indxPhiTarget].get_minimum())*inv_xPhi);
      int etaCalc = MySourceRange[indxEtaSource].get_minimum() + int((idExpTarget[indxEtaTarget] - MyTargetRange[indxEtaTarget].get_minimum())*inv_xEta);
//      std::cout << "Candidate: " << (std::string)idExpTarget << std::endl;
//      std::cout << " phiCalc: " << phiCalc << " etaCalc:" << etaCalc << std::endl;
      if( (phiCalc != idExpSource[indxPhiSource]) || (etaCalc != idExpSource[indxEtaSource]) ) continue;
//      std::cout << "       YES. ITS NEIGHBOUR" << std::endl;
      Identifier idTarget;
      IdentifierHash idHashTarget;
      if(get_id(idExpTarget, idTarget, idHashTarget)==1) continue;
      if(m_caloDM_ID->is_lar(idSource) ) {
        m_DmNeighbourLArVector[idHashSource].push_back(idHashTarget);
      } else if(m_caloDM_ID->is_tile(idSource) ){
        m_DmNeighbourTileVector[idHashSource].push_back(idHashTarget);
      } else {
//        std::cout << "JOPA5" << std::endl;
      }
    } // loop over Target identifiers
//    std::cout << " n_neighbours: " << m_DmNeighbourLArVector[idHashSource].size();
//    for(std::vector<IdentifierHash >::iterator it=m_DmNeighbourLArVector[idHashSource].begin(); it!=m_DmNeighbourLArVector[idHashSource].end(); it++){
//      IdentifierHash hash_id = (*it);
//      Identifier id= m_caloCell_ID->cell_id(hash_id);
//      std::cout << " " << m_id_helper->show_to_string(id);
//    }
//    std::cout << std::endl;
  } // loop over source identifiers

  return StatusCode::SUCCESS;
}



/* **************************************************************************
For given expanded identifier returns position of side, eta and phi subfields
************************************************************************** */
int CaloDmNeighbours::get_indexes(ExpandedIdentifier &idExp, int &indxSide, int &indxEta, int &indxPhi)
{
  if(  (idExp.fields() > 0) && idExp[0]==10 ) {
    indxSide = kDM_SUBDET;
    indxEta = kDM_ETA;
    indxPhi = kDM_PHI;
  } else if ( m_caloCell_ID->is_lar_em(idExp) ) {
    indxSide = kLAR_SIDE;
    indxEta = kLAR_ETA;
    indxPhi = kLAR_PHI;
  } else if ( m_caloCell_ID->is_tile(idExp) ) {
    indxSide = kTILE_SIDE;
    indxEta = kTILE_ETA;
    indxPhi = kTILE_PHI;
  } else if ( m_caloCell_ID->is_lar_fcal(idExp) ) {
    indxSide = kFCAL_SIDE;
    indxEta = kFCAL_ETA;
    indxPhi = kFCAL_PHI;
  } else {
    indxSide = 0;
    indxEta = 0;
    indxPhi = 0;
    std::cout << " CaloDmNeighbours::get_indexes() -> Error! Unknown id ";
    idExp.show();
    std::cout <<std::endl;
  }
  return 0;
}



/* **************************************************************************
make Identifier and IdentifierHash from ExpandedIdentifier
************************************************************************** */
int CaloDmNeighbours::get_id(ExpandedIdentifier &idExp, Identifier &id, IdentifierHash &hash_id)
{
  CaloCell_ID::SUBCALO subCalo;
  // dead material identifier
  if(  (idExp.fields() == 7) && idExp[0]==10 ) {
    try {
      id = m_caloDM_ID->zone_id(idExp[kDM_SUBDET],idExp[kDM_TYPE],idExp[kDM_SAMPL],idExp[kDM_REGION],idExp[kDM_ETA],idExp[kDM_PHI]);
    }
    catch(CaloID_Exception & except){
      std::cout << " CaloDmNeighbours::get_id -> Error! Exception caught:" 
          << std::endl << (std::string)except << std::endl;
      return 1;
    }
    if(m_caloDM_ID->is_lar(id)) {
      hash_id = m_caloDM_ID->lar_zone_hash(id);
    } else if (m_caloDM_ID->is_tile(id)) {
      hash_id = m_caloDM_ID->tile_zone_hash(id);
    } else {
      std::cout << "CaloDmNeighbours::get_id() -> Error! Wrong expanded dead material identifier " << (std::string)idExp  << std::endl;
      return 1;
    }

  // em barrel/edcap identifier  
  } else if (m_caloCell_ID->is_lar_em(idExp)) {
    subCalo = CaloCell_ID::LAREM;
    try {
      id = m_caloCell_ID->cell_id(subCalo, idExp[kLAR_SIDE], idExp[kLAR_SAMPL], idExp[kLAR_REGION], idExp[kLAR_ETA], idExp[kLAR_PHI]);
    }
    catch(LArID_Exception & except){
      std::cout << " CaloDmNeighbours::get_id -> Error! Exception caught:" 
          << std::endl << (std::string)except << std::endl;
      return 1;
    }
    hash_id =  m_caloCell_ID->calo_cell_hash(id);
  // hec identifier
  } else if ( m_caloCell_ID->is_lar_hec(idExp) ) {
    subCalo = CaloCell_ID::LARHEC;
    try {
      id = m_caloCell_ID->cell_id(subCalo, idExp[kLAR_SIDE], idExp[kLAR_SAMPL], idExp[kLAR_REGION], idExp[kLAR_ETA], idExp[kLAR_PHI]);
    }
    catch(LArID_Exception & except){
      std::cout << " CaloDmNeighbours::get_id -> Error! Exception caught:" 
          << std::endl << (std::string)except << std::endl;
      return 1;
    }

    hash_id =  m_caloCell_ID->calo_cell_hash(id);
  // fcal identifier
  } else if ( m_caloCell_ID->is_lar_fcal(idExp) ) {
    subCalo = CaloCell_ID::LARFCAL;
    try {
      id = m_caloCell_ID->cell_id(subCalo, idExp[kLAR_SIDE], idExp[kLAR_SAMPL], 0, idExp[kFCAL_ETA], idExp[kFCAL_PHI]);
    }
    catch(LArID_Exception & except){
      std::cout << " CaloDmNeighbours::get_id -> Error! Exception caught:" 
          << std::endl << (std::string)except << std::endl;
      return 1;
    }
    hash_id =  m_caloCell_ID->calo_cell_hash(id);
  // tile identifier
  } else if ( m_caloCell_ID->is_tile(idExp) ) {
    subCalo = CaloCell_ID::TILE;
    try {
      id = m_caloCell_ID->cell_id(subCalo, idExp[kTILE_SECTION], idExp[kTILE_SIDE], idExp[kTILE_PHI], idExp[kTILE_ETA], idExp[kTILE_SAMPL]);
    }
    catch(LArID_Exception & except){
      std::cout << " CaloDmNeighbours::get_id -> Error! Exception caught:" 
          << std::endl << (std::string)except << std::endl;
      return 1;
    }
    hash_id =  m_caloCell_ID->calo_cell_hash(id);
  // x.z.
  } else {
    std::cout << "CaloDmNeighbours::get_id() -> Error! Wrong LAr/Tile expanded identifier " << (std::string)idExp << std::endl;
    return 1;
  }
  return 0;
}



/* **************************************************************************
loading calo neighbour list for DM identifiers 
************************************************************************** */
StatusCode CaloDmNeighbours::load_neighbours(const std::string& DmNeighboursFileName)
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmNeighbours");

  // Find the full path to filename:
  std::string file = PathResolver::find_file (DmNeighboursFileName, "DATAPATH");
  log << MSG::INFO << "Reading file  " << file << endmsg;

  std::ifstream fin(file.c_str());
  if(!fin){
    log << MSG::ERROR << "Can't open file " << file << endmsg;
    return StatusCode::FAILURE;
  }

  char cLine[MAX_BUFFER_LEN];

  // reading text file line by line, parsing neighbours string
  while(fin.getline(cLine,sizeof(cLine)-1)){
    if( strlen(cLine)==0 ||  cLine[0] == '#' || cLine[0] == '\n') continue;
    if( cLine[0] == '@' ) break;
    std::string sLine(cLine);

    std::istringstream neighbours( sLine.c_str() );
    std::string MyDm, MyCalo;
    if(neighbours >> MyDm) {
      while(neighbours >> MyCalo) {
        if(MyCalo.find('#') != std::string::npos ) break;
        if(!MyDm.empty() && !MyCalo.empty()) {
          StatusCode sc = unfold_neighbours(MyDm, MyCalo);
          if (sc.isFailure()) {
            log << MSG::ERROR << "Error processing string '" << sLine <<"'" << std::endl;
            return sc;
          }
        } else {
          log << MSG::ERROR << "Error while parsing string '" << sLine <<"'" << std::endl;
          return StatusCode::FAILURE;
        }
      }
    }
  } // while over lines
  fin.close();

  // some checks of generated neighbours
#ifdef DEBUG
  std::cout << "YYY analyzing." << std::endl;
  char sep ='/';
  for(unsigned int i=0; i<m_caloDM_ID->lar_zone_hash_max(); i++) {
    if( !m_DmNeighbourLArVector[i].size()) continue;
    std::cout << m_id_helper->show_to_string(m_caloDM_ID->lar_zone_id(i),0,sep) << "  ";
    for(unsigned int j=0; j<m_DmNeighbourLArVector[i].size(); j++){
      IdentifierHash hash_id = m_DmNeighbourLArVector[i][j];
      Identifier id = m_caloCell_ID->cell_id(hash_id);
      std::cout << " j:" << j << " hash_id: " << hash_id << " " << m_caloCell_ID->show_to_string(id,0,sep) << std::endl;
    }
  }
#endif
  log << MSG::INFO << "Neighbours loaded from file " << file << endmsg;
  return StatusCode::SUCCESS;
}



/* **************************************************************************
Subroutine for semi-automatic generation of DeadMaterialCaloNeighbours.dat file
************************************************************************** */
int CaloDmNeighbours::generate_neighbour_file()
{
//   std::cout << "CaloDmNeighbours::generate_neighbour_file() -> Start" << std::endl;
//   CaloDmDescrManager *dmManager = new CaloDmDescrManager();
// 
//   // loop over all LAr DM identifiers
//   for(unsigned int i_hit=0; i_hit<m_caloDM_ID->lar_zone_hash_max(); i_hit++) {
//     //if(i_hit>5000) break;
//     Identifier dmhit_id = m_caloDM_ID->lar_zone_id(i_hit);
//     int dmhit_subdet = m_caloDM_ID->pos_neg_z(dmhit_id); // subdet
//     int dmhit_type = m_caloDM_ID->dmat(dmhit_id);      // type
//     int dmhit_sampling = m_caloDM_ID->sampling(dmhit_id);  // sampling
//     int dmhit_region = m_caloDM_ID->region(dmhit_id);    // region
//     int dmhit_ieta = m_caloDM_ID->eta(dmhit_id);       // eta
//     int dmhit_iphi = m_caloDM_ID->phi(dmhit_id);       // phi           
//     int dmhit_key = abs(dmhit_subdet)*1000+dmhit_type*100+ dmhit_sampling*10+dmhit_region;
//     if(dmhit_key!=4125 ) continue;
//     //if(dmhit_key != 4117 && dmhit_key!=4205 && dmhit_key!=4215) continue;
//     if(dmhit_subdet > 0) continue;
//     if(dmhit_iphi != 63) continue;
//     //if(dmhit_ieta < 7) continue;
//     
//     CaloDmDescrManager::DmRegion *myRegion = dmManager->get_dm_region(dmhit_id);
//     if(!myRegion->m_CaloSampleNeighbours.size()) continue;
// //     std::cout << m_id_helper->show_to_string(dmhit_id);
// //     std::cout << " key: " << myRegion->m_key 
// //         << " eta: " << myRegion->m_eta_min << " " << myRegion->m_eta_max
// //         << " size: " << myRegion->m_CaloSampleNeighbours.size()
// //         << std::endl;
// 
//     
//         
//     int dmhit_sign = int(copysign(1,m_caloDM_ID->pos_neg_z(dmhit_id)));
//     CaloDmDescrElement *dmhit_element = dmManager->get_element(dmhit_id);
//     float dmhit_eta = dmhit_element->eta();
//     float dmhit_phi = dmhit_element->phi();
//     float dmhit_deta = dmhit_element->deta();
//     float dmhit_dphi = dmhit_element->dphi();
//     
// /*      std::cout << m_id_helper->show_to_string(dmhit_id) 
//     << " dm_eta: " << dmhit_eta << " " << dmhit_deta << " dm_phi: " << dmhit_phi << " " << dmhit_dphi
//           << std::endl;
//       continue;    */
//     if(i_hit%1000 == 0) std::cout << "OOO- " << i_hit << std::endl;
//     
//     
//     IdentifierHash caloCellMinMin = m_caloCell_ID->calo_cell_hash_max();
//     IdentifierHash caloCellMaxMax = 0;
//     for(unsigned int i_hreg=0; i_hreg < myRegion->m_CaloSampleNeighbours.size(); i_hreg++) {
//       int subCalo = CaloCell_ID::NOT_VALID;
//       if(myRegion->m_CaloSampleNeighbours[i_hreg] <= CaloCell_ID::EME3) {
//         subCalo = CaloCell_ID::LAREM;
//       } else if(myRegion->m_CaloSampleNeighbours[i_hreg] <= CaloCell_ID::HEC3) {
//         subCalo = CaloCell_ID::LARHEC;
//       } else if(myRegion->m_CaloSampleNeighbours[i_hreg] <= CaloCell_ID::TileExt2) {
//         subCalo = CaloCell_ID::TILE;
//       } else if(myRegion->m_CaloSampleNeighbours[i_hreg] <= CaloCell_ID::FCAL2) {
//         subCalo = CaloCell_ID::LARFCAL;
//       }
//       IdentifierHash caloCellMin, caloCellMax;
//       m_caloCell_ID->calo_cell_hash_range(subCalo, caloCellMin, caloCellMax);
//       if(caloCellMin < caloCellMinMin) caloCellMinMin = caloCellMin;
//       if(caloCellMax > caloCellMaxMax) caloCellMaxMax = caloCellMax;
// /*      std::cout << "i_hreg: " << i_hreg
//           << " myRegion->m_CaloSampleNeighbours[i_hreg]: " << myRegion->m_CaloSampleNeighbours[i_hreg]
//           << " subCalo: " << subCalo
//           << " caloCellMin: " << caloCellMin 
//           << " caloCellMax: " << caloCellMax 
//           << std::endl;*/
//     }
// /*    std::cout << " caloCellMaxMax: " << caloCellMaxMax
//         << " caloCellMinMin: " << caloCellMinMin 
//         << std::endl;*/
//     
//     for(unsigned int calo_hash=caloCellMinMin; calo_hash<caloCellMaxMax; calo_hash++){
//       Identifier calo_id = m_caloCell_ID->cell_id(calo_hash);
//       int calo_csmp = m_caloCell_ID->calo_sample(calo_id);
//       bool good_csmp = false;
//       for(unsigned int i_hreg=0; i_hreg < myRegion->m_CaloSampleNeighbours.size(); i_hreg++) {
//         if(calo_csmp == myRegion->m_CaloSampleNeighbours[i_hreg]) { good_csmp = true; break;}
//       }
//       if(!good_csmp) continue;
// 
//       int calo_calo = m_caloCell_ID->sub_calo(calo_id);
//       int calo_sign;
//       if ( (CaloCell_ID::SUBCALO) calo_calo == CaloCell_ID::TILE) {
//         calo_sign = int(copysign(1,m_caloCell_ID->side(calo_id)));
//       } else {
//         calo_sign = int(copysign(1,m_caloCell_ID->pos_neg(calo_id)));
//       }
//       if(calo_sign != dmhit_sign) continue;
// 
//       const CaloDetDescrElement* theCDDE = m_caloDetDescrManager->get_element(calo_id);
//         
//       float calo_eta = theCDDE->eta();
//       float calo_phi = theCDDE->phi();
//       float calo_deta=0;
//       float calo_dphi=0;
//       if(theCDDE){
//         calo_deta = theCDDE->deta();
//         calo_dphi = theCDDE->dphi();
//       }else{
//         std::cout << "CaloDmNeighbours::generate_neighbour_file() -> Warning, no CDDE" << m_id_helper->show_to_string(calo_id) << std::endl; 
//       }
//     // since deat, dphi for FCAL cells are absent, lets imagine they approximate values
//       if ( (CaloCell_ID::SUBCALO) m_caloCell_ID->sub_calo(calo_id) == CaloCell_ID::LARFCAL && theCDDE) {
//         int nfcal = m_caloCell_ID->sampling(calo_id) - 1;
//         float dsize[3]={7.5,8.0,9.0}; // tube size (x or y) in mm for fcal1-3
//                 // ssize = 2.0*dsize*dsize*sqrt(3.0)/4.0;
//         float ssize[3]={48.713928, 55.425625, 70.148056}; // cell area in mm*mm for fcal1-3
//         float z = fabs(theCDDE->z());
//         float volume = theCDDE->volume();
//         float N = volume/ssize[nfcal]/(450.0); // number of tubes for given FCAL cell, 450. it's fcal length
//         float dr = sqrt(N)*dsize[nfcal]; // cell size
//         float eta=fabs(calo_eta);
//         calo_dphi = (dr/z)*sinh(eta);
//         calo_deta = (dr/z)*cosh(eta);
//       }
//       if(dmhit_key==4117) {
//         if(calo_csmp == CaloCell_ID::EME2) {
//           calo_dphi=0.0;
//           calo_deta *= 1.5;
//         }
//       }else if(dmhit_key==4205 || dmhit_key==4215) {
//         if(calo_csmp == CaloCell_ID::HEC0 || calo_csmp == CaloCell_ID::EMB3) {
//           calo_dphi=0.0;
//           calo_deta *= 1.5;
//         }
//       }else if(dmhit_key==4123 || dmhit_key==4124) {
//         if(calo_csmp >= CaloCell_ID::HEC0 && calo_csmp <= CaloCell_ID::HEC3) {
//           calo_dphi=0.0;
//           calo_deta *= 1.;
//         }
//       } else {
//         calo_dphi=0.0;
//         calo_deta=0.0;       
//       }
//     // neighbours or not
//       double phi_diff = calo_phi - dmhit_phi;
//       if(phi_diff > M_PI) phi_diff=phi_diff-2.*M_PI;
//       if(phi_diff < -M_PI) phi_diff=phi_diff+2.*M_PI;
// /*      std::cout << m_id_helper->show_to_string(dmhit_id) << " " << m_id_helper->show_to_string(calo_id) 
//       << " dm_eta: " << dmhit_eta << " " << dmhit_deta << " dm_phi: " << dmhit_phi << " " << dmhit_dphi
//       << " eta: " << calo_eta << " " << calo_deta << " phi: " << calo_phi << " " << calo_dphi
//       << " d: " << phi_diff << " " << fabs(dmhit_eta-calo_eta) << std::endl;*/
//       if( fabs(phi_diff) < (dmhit_dphi + calo_dphi+0.00001)/2. && fabs(dmhit_eta-calo_eta) < (dmhit_deta+calo_deta+0.00001)/2. ){
//         m_DmNeighbourLArVector[i_hit].push_back(calo_hash);
// //        std::cout << "NEI!" << std::endl;
//       }
// 
//     } // loop over calo hash
//   } // loop over DM hits    
// 
//   delete dmManager;
// 
//   // printing on the screen
//   for(unsigned int i_hit=0; i_hit<m_caloDM_ID->lar_zone_hash_max(); i_hit++) {
//     Identifier dmhit_id = m_caloDM_ID->lar_zone_id(i_hit);
//     //if(m_caloDM_ID->phi(dmhit_id) != 0) continue;
//     if(!m_DmNeighbourLArVector[i_hit].size()) continue;
//     //std::cout << m_id_helper->show_to_string(dmhit_id,0,'/') << "  ";
//     /*    range.build(m_id_helper->show_to_string(dmhit_id,0,'/'));*/
//     Range range;
//     ExpandedIdentifier dmhit_expId(m_id_helper->show_to_string(dmhit_id,0,'/'));
//     for(unsigned int i_field = 0; i_field<dmhit_expId.fields(); i_field++){
//       Range::field myfield;
//       myfield.add_value(dmhit_expId[i_field]);
//       if(i_field == kDM_SUBDET){
//         myfield.add_value(-dmhit_expId[i_field]);      
//       }
//       range.add(myfield);
//     }
// //    int dm_side = 0;
//     // range[kDM_SUBDET].add_value(dm_side);
//     std::cout << (std::string)range << "  ";
//     for(unsigned int i_calo=0; i_calo<m_DmNeighbourLArVector[i_hit].size(); i_calo++){
//       Identifier calo_id = m_caloCell_ID->cell_id(m_DmNeighbourLArVector[i_hit][i_calo]);
// /*      std::cout << " " << m_id_helper->show_to_string(calo_id,0,'/');*/
//       Range range;
//       ExpandedIdentifier calo_expId(m_id_helper->show_to_string(calo_id,0,'/'));
//       for(unsigned int i_field = 0; i_field<dmhit_expId.fields(); i_field++){
//         Range::field myfield;
//         myfield.add_value(calo_expId[i_field]);
//         if(i_field == kLAR_SIDE){
//           myfield.add_value(-calo_expId[i_field]);
//         }
//         if(i_field==6 && (m_caloCell_ID->is_fcal(calo_id) || m_caloCell_ID->is_tile(calo_id))) break;
//         range.add(myfield);
//       }
//       std::cout << " " << (std::string)range;
//     } 
//     std::cout << std::endl;
//   } 
//   std::cout << "CaloDmNeighbours::generate_neighbour_file() -> Done" << std::endl;
  return 0;
}



/* ****************************************************************************
Filling m_CaloSample2DmRegion[csmp][netabin][n_dm_region] containing list of 
DmRegion for given CaloSample at given eta. This map is used to find DM 
neighbours for calo cells.
***************************************************************************** */
int CaloDmNeighbours::make_CaloSample2DmRegion_map()
{
  m_CaloSample2DmRegion.resize(CaloCell_ID::Unknown);
  for(int i=0; i<(int)CaloCell_ID::Unknown; i++){
    m_CaloSample2DmRegion[i].resize(CALOMAP_NETA);
  }
  for(std::vector<CaloDmRegion *>::const_iterator it=m_caloDmDescrManager->reg_begin();  it!=m_caloDmDescrManager->reg_end(); it++){
    const CaloDmRegion *myRegion = (*it);
    for(unsigned int i=0; i<myRegion->m_CaloSampleNeighbours.size(); i++){
      int csmp = myRegion->m_CaloSampleNeighbours[i];
      int neta_min = int((myRegion->m_CaloSampleEtaMin[i] - CALOMAP_ETA_MIN)*(1./CALOMAP_DETA)+0.00001);
      int neta_max = int((myRegion->m_CaloSampleEtaMax[i] - CALOMAP_ETA_MIN)*(1./CALOMAP_DETA)+0.00001);
      if(neta_max< 0 || neta_max >CALOMAP_NETA || neta_min < 0 || neta_min >CALOMAP_NETA || neta_min >=neta_max ) {
        std::cout << " CaloDmNeighbours::make_CaloSample2DmRegion_map() -> ERROR! Wrong parameters "
            << " etaMin: " << myRegion->m_CaloSampleEtaMin[i]
            << " etaMax: " << myRegion->m_CaloSampleEtaMax[i]
            << " neta_min: " << neta_min
            << " neta_max: " << neta_max
            << std::endl;
        continue;
      }
      for(int i_eta=neta_min; i_eta<neta_max; i_eta++){
        m_CaloSample2DmRegion[csmp][i_eta].push_back(myRegion);
      }
    }
  }

#ifdef DEBUG
  std::cout << " Map done!" << std::endl;
  for(int i_csmp=0; i_csmp<(int)CaloCell_ID::Unknown; i_csmp++){
    std::cout << " i_csmp: " << i_csmp << std::endl;
    for(int i_eta=0; i_eta<CALOMAP_NETA; i_eta++){
      std::cout << i_eta;
      std::cout << " i_eta: " << i_eta << " size: " << m_CaloSample2DmRegion[i_csmp][i_eta].size() << std::endl;
      for(unsigned int i=0; i<m_CaloSample2DmRegion[i_csmp][i_eta].size(); i++){
        IdentifierHash id_hash = m_CaloSample2DmRegion[i_csmp][i_eta][i]->region_hash();
        Identifier id;
        if(id_hash < m_caloDM_ID->lar_zone_hash_max()) {
          id = m_caloDM_ID->lar_zone_id(id_hash);
        } else {
          id_hash -= m_caloDM_ID->lar_zone_hash_max();
          id = m_caloDM_ID->tile_zone_id(id_hash);
        }
        std::cout << m_id_helper->show_to_string(id) << " ";
      }
      std::cout << std::endl;
    }
  }
#endif
  return 0;
}


const CaloDetDescrManager* CaloDmNeighbours::getMgr() const
{
  const CaloDetDescrManager* p = m_caloDetDescrManager.get();
  if (!p) {
    if (m_detStore->retrieve (p, "CaloMgr").isFailure()) {
      return nullptr;
    }
    m_caloDetDescrManager.set (p);
  }
  return p;
}
