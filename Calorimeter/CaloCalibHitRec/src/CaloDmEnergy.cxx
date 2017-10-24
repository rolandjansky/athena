/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmEnergy.h
 Author      : guennadi.pospelov@cern.ch
 Created     : May, 2007
 DESCRIPTION : calculate Dead Material energy in different DM areas
               (sum over specific groups of DM calibration hits).
 Usage       : see CBNT_CalibrationInfoDM2 algorithm for example
**************************************************************************** */

#include "CaloCalibHitRec/CaloDmEnergy.h"

#include "AthenaKernel/getMessageSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

//#include "CaloEvent/CaloCluster.h"
//#include "CaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloCalibHitRec/CaloDmNeighbours.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>

#undef DEBUG

using CLHEP::HepLorentzVector;


/* ****************************************************************************
CaloDmCell c-tor
It is a special dead material cell based on dead material calibration hit 
Has a list of neighboring clusters
**************************************************************************** */
CaloDmCell::CaloDmCell(const CaloCalibrationHit *hit, const CaloDmDescrElement *element, const CaloDmRegion *region)
{
  m_id = hit->cellID();
  m_energy_total = hit->energyTotal();
  m_element = element;
  m_region = region;
  m_dmarea_n = CaloDmEnergy::DMA_UNCLASS;
  m_sum_cls_energy = 0.0;
}

CaloDmCell::~CaloDmCell()
{
}


void CaloDmCell::Add(CaloCalibrationHit *hit)
{
  m_energy_total += hit->energyTotal();
}



/* ****************************************************************************
CaloDmEnergy constructor/destructor
**************************************************************************** */
CaloDmEnergy::CaloDmEnergy(const CaloDmDescrManager *dmMgr):
  m_dmsum_etotal(0),
  m_dmsum_em(0),
  m_dmsum_nonem(0),
  m_dmsum_invisible(0),
  m_dmsum_escaped(0),
  m_dmcls_sum_etotal(0),
  m_max_dmhit(1000000),
  m_max_cluster(10000), m_apars_alpha(0.5), m_apars_r0(0.2),
  m_apars_clust_min_ener(200.0),m_apars_clust_min_ecalib(10.0), m_apars_cone_cut(0.5),
  m_caloDmNeighbours(0),
  m_caloDetDescrManager(0),
  m_caloDM_ID(0),
  m_caloCell_ID(0),
  m_id_helper(0),
  m_detStore(0)
{
  m_caloDmDescrManager = dmMgr;
  m_CalibrationContainerNamesDM.push_back("LArCalibrationHitDeadMaterial");
  m_CalibrationContainerNamesDM.push_back("TileCalibrationDMHitCnt");
  m_nclusters = 0;
  initialize();
}



CaloDmEnergy::~CaloDmEnergy()
{
  m_dmsum_area_etotal.clear();
  m_dmcls_area_etotal.clear();
  for(std::vector<CaloDmCell *>::iterator it=m_CaloDmCellVector.begin(); it!=m_CaloDmCellVector.end(); it++){
    delete (*it);
  }
  m_CaloDmCellVector.clear();
  m_CaloDmCellContainer.clear();
  if(m_caloDmNeighbours) delete m_caloDmNeighbours;
}



/* ****************************************************************************
CaloDmEnergy::initialize()
**************************************************************************** */
int CaloDmEnergy::initialize()
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmEnergy");

  ISvcLocator* svcLoc = Gaudi::svcLocator();

  StatusCode sc = svcLoc->service("StoreGateSvc", m_storeGate);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Unable to get pointer to StoreGate Service" << endmsg;
    return sc;
  }

  sc = svcLoc->service("DetectorStore", m_detStore);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Unable to get pointer to DetectorStore Service" << endmsg;
    return sc;
  }

  // pointer to detector manager:
  m_caloDetDescrManager  = CaloDetDescrManager::instance(); 

  sc = m_detStore->retrieve(m_caloDM_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve caloDM_ID helper from DetectorStore" << endmsg;
    return sc;
  }

  sc = m_detStore->retrieve(m_caloCell_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve caloCell_ID helper from DetectorStore" << endmsg;
    return sc;
  }

  // retrieve ID helpers from det store
  sc = m_detStore->retrieve(m_id_helper);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve AtlasDetectorID helper from DetectorStore" << endmsg;
    return sc;
  }

  // manager to get DM hash identifiers which are neighbours of given CaloCell
  m_caloDmNeighbours = new CaloDmNeighbours(m_caloDmDescrManager);

  return StatusCode::SUCCESS;
}



/* ****************************************************************************
CaloDmEnergy::assign2clusters()
Assignment of DM cells to clusters. One DM cell can be assigned to many clusters.
Result of procedure is a vector of assigned cluster indexes and they energies for
each DM cell.
Procedure consist of two steps:
I.  DM hits is assigned to cluster if it is a direct neighbour of cluster cells.
    Hits which were no assigned to any cluster are passed to the next step.
II. DM hits are assigned to all cluster within distance < m_apars_r0=1.5,
    weights of individual DM hit for given cluster will be calculated as a
    function of cluster energy and distance to the cluster
**************************************************************************** */
int CaloDmEnergy::assign2clusters(const std::string &clusterContainerName)
{
  StatusCode sc;
  MsgStream log(Athena::getMessageSvc(), "CaloDmEnergy");

  // retreiving DM containers
  std::vector<const CaloCalibrationHitContainer *> v_dmcchc;
  for (std::vector<std::string>::iterator iter=m_CalibrationContainerNamesDM.begin();iter!=m_CalibrationContainerNamesDM.end();iter++) {
    const DataHandle<CaloCalibrationHitContainer> dmcchc;
    sc = m_storeGate->retrieve(dmcchc,*iter);
    if (sc.isFailure() ) {
      log << MSG::WARNING << "Cannot retrieve DM calibration hit container " << *iter << endmsg;
      return 1;
    } else {
      v_dmcchc.push_back(dmcchc);
    }
  }

  // retreiving cluster container
  const DataHandle<xAOD::CaloClusterContainer > theClusters;
  sc = m_storeGate->retrieve(theClusters, clusterContainerName);
  if (sc.isFailure()) {
    log << MSG::WARNING << " Couldn't get cluster container '" << clusterContainerName << "'" << endmsg;
    return 0;
  }

  // the procedure needs calibration energy of clusters
  std::vector<double > v_cluster_calib_energy;
  v_cluster_calib_energy.resize(theClusters->size(), 0.0);
  for(xAOD::CaloClusterContainer::const_iterator ic=theClusters->begin(); ic != theClusters->end(); ic++){
    const xAOD::CaloCluster *cluster =(*ic);
    double x_moment = 0;
    (void)cluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, x_moment);
    v_cluster_calib_energy.push_back(x_moment);
  }

  assign2clusters(v_dmcchc, theClusters, v_cluster_calib_energy);
  return 0;
}



/* ****************************************************************************
see comments above
**************************************************************************** */
int CaloDmEnergy::assign2clusters(std::vector<const CaloCalibrationHitContainer *> &v_dmcchc, const xAOD::CaloClusterContainer *theClusters, std::vector<double> &v_cluster_calib_energy)
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmEnergy");

  // making vector of dead material cells
  make_dmcell_vector(v_dmcchc);

  m_nclusters = theClusters->size();
  if(m_nclusters > m_max_cluster) m_nclusters = m_max_cluster;

  // preparing for the data
  m_dmcls_sum_etotal = 0.0;
  m_dmcls_area_etotal.clear();
  m_dmcls_area_etotal.resize(m_nclusters);
  for(int i_cls=0; i_cls<m_nclusters; i_cls++){
    m_dmcls_area_etotal[i_cls].resize(DMA_MAX,0.0);
  }

  if( m_nclusters == 0 || m_CaloDmCellVector.size() == 0) {
    log << MSG::WARNING << " No data to process. Number of clusters: " << m_nclusters 
        << " m_CaloDmCellVector.size(): " << m_CaloDmCellVector.size()
        << std::endl;
    return 1;
  }

  /* ******************************************************
  Step1: assignment of DM hits to clusters direct neighbours cells
  ****************************************************** */
  int nc=0;
  for(xAOD::CaloClusterContainer::const_iterator ic=theClusters->begin(); ic != theClusters->end(); ic++){
    if(nc < m_max_cluster){
      const xAOD::CaloCluster *cluster =(*ic);
      if(cluster->e() > m_apars_clust_min_ener && v_cluster_calib_energy[nc] > m_apars_clust_min_ecalib) {
        int cell_indx=0;
        CaloClusterCellLink::const_iterator it_cell = cluster->cell_begin();
        CaloClusterCellLink::const_iterator it_cell_end = cluster->cell_end();
        for(; it_cell != it_cell_end; it_cell++){
          const CaloCell* cell = (*it_cell);
          //double cell_energy = cell->energy()*cluster->getCellWeight(it_cell);
          double cell_energy = cell->energy()*(it_cell.weight());
          //double cell_energy = cell->energy();
          if(cell_energy < 0) continue;
          Identifier cell_id =  cell->ID();
          /* *********************************************************************
          getting list of DM hash identifiers which are direct neighbours of given cell
          ********************************************************************** */
          std::vector<IdentifierHash > cell_dmnei;
          m_caloDmNeighbours->getNeighbours_DmHitsForCaloCell(cell_id, cell_dmnei);
          // checking CaloDmCell container for these hash_id's
          for(std::vector<IdentifierHash >::iterator it=cell_dmnei.begin(); it!=cell_dmnei.end(); it++){
            IdentifierHash dm_hash_id = (*it);
#ifdef DEBUG
            Identifier dm_id;
            if(dm_hash_id < m_caloDM_ID->lar_zone_hash_max()){
              dm_id=m_caloDM_ID->lar_zone_id(dm_hash_id);
            }else{
              dm_id=m_caloDM_ID->tile_zone_id(dm_hash_id - m_caloDM_ID->lar_zone_hash_max());
            }
            CaloDmDescrElement *el = m_caloDmDescrManager->get_element(dm_id);
            std::cout << "        we got: "
                 << " dm_hash_id: " << dm_hash_id
                 << " " << m_id_helper->show_to_string(dm_id)
                << " eta: " << el->eta()
                << " phi: " << el->phi();
#endif
            CaloDmCell *dmCell = m_CaloDmCellContainer[dm_hash_id];
            if(dmCell){
              // yes, this CaloDmCell exists, lets add current cluster,cell info to this CaloDmCell
              int ipos = dmCell->m_cls_index.size()-1;
              // storing for each DmCell list of clusters and they appropriate sampling energies
              if( ipos==-1 || dmCell->m_cls_index[ipos] != nc) {
                dmCell->m_cls_index.push_back(nc); 
                dmCell->m_cls_energy.push_back(cell_energy);
              } else {
                dmCell->m_cls_energy[ipos] += cell_energy;
              }
              // also storing sum of cluster samplings over all neighboring clusters to calculate weight at the end
              dmCell->m_sum_cls_energy += cell_energy;
            }
          } // loop over CaloDmCell's which are neighbours of current cluster cell
          cell_dmnei.clear();
          cell_indx++;
        }
      } // cluster->energy() > m_apars_clust_min_ener && v_cluster_calib_energy[nc] > m_apars_clust_min_ecalib
    } else {
      break;
    }
    nc++;
  } // loop over clusters


  /* ******************************************************
  Step2: assignment of DM hits to clusters on sampling level
  ****************************************************** */
  for(std::vector<CaloDmCell *>::iterator it=m_CaloDmCellVector.begin(); it!= m_CaloDmCellVector.end(); it++){
    CaloDmCell *dmCell = (*it);
    if( !dmCell->m_cls_index.size() ){ // DM cells assigned to clusters by previous procedure will be skipped
       xAOD::CaloClusterContainer::const_iterator ic;
      nc=0;
      for(ic=theClusters->begin(); ic != theClusters->end(); ic++){
        const xAOD::CaloCluster *cluster = (*ic);
        if(nc < m_max_cluster){
          if(cluster->e() > m_apars_clust_min_ener && v_cluster_calib_energy[nc] > m_apars_clust_min_ecalib) {

            double smp_energy = 0.0, distance = 1000.0;
            get_distance_cluster_dmcell(cluster, dmCell, smp_energy, distance);
            if(smp_energy > 0.0 && distance < m_apars_cone_cut) {
              double dmClsEnerFun = pow(smp_energy,m_apars_alpha)*exp(-distance/m_apars_r0);
              dmCell->m_cls_index.push_back(nc);
              dmCell->m_cls_energy.push_back(dmClsEnerFun);
              dmCell->m_sum_cls_energy += dmClsEnerFun;
            }
          } // cluster->energy() > m_apars_clust_min_ener && v_cluster_calib_energy[nc] > m_apars_clust_min_ecalib
        } else {
          break;
        }
        nc++;
      } // loop over clusters
    } // dmCell->m_cls_index.size() == 0
  } // loop over DM cells


  /* ******************************************************
  saving energy sums in different calo areas for each cluster separately
  ****************************************************** */
  for(std::vector<CaloDmCell *>::iterator it=m_CaloDmCellVector.begin(); it!= m_CaloDmCellVector.end(); it++){
    CaloDmCell *dmCell = (*it);
    if(dmCell->m_cls_index.size()) m_dmcls_sum_etotal += dmCell->energyTotal();
    for(unsigned int i_cls=0; i_cls<dmCell->m_cls_index.size(); i_cls++){
      int cls_indx = dmCell->m_cls_index[i_cls];
      // Since one DM cell could belong to many clusters, weight for given cluster are calculated
      if(dmCell->m_sum_cls_energy <= 0.0) continue;
      float dm_weight = dmCell->m_cls_energy[i_cls]/dmCell->m_sum_cls_energy;
      if(dm_weight > 1.0){
        std::cout << "CaloDmEnergy::assign2cluster() -> Error! Strange weight dm_weight:" << dm_weight
            << "  dmCell->m_sum_cls_energy: " << dmCell->m_sum_cls_energy
            << " dmCell->m_cls_energy[i_cls]: " << dmCell->m_cls_energy[i_cls]
            << " i_cls: " << i_cls
            << " dmCell->energyTotal(): " << dmCell->energyTotal()
            << " v_cluster_calib_energy[i_cls]: " << v_cluster_calib_energy[nc]
            << std::endl;
      }
      m_dmcls_area_etotal[cls_indx][DMA_ALL] += (*it)->energyTotal()*dm_weight;
      m_dmcls_area_etotal[cls_indx][(*it)->dmAreaN()] += (*it)->energyTotal()*dm_weight;
    }
  } // loop over DM cells

  return 0;
}



/* ****************************************************************************
checks that cluster has energy in appropriate samplings for this DM cell
and calculates corresponded cluster energy and distance to cluster
**************************************************************************** */
bool CaloDmEnergy::get_distance_cluster_dmcell(const xAOD::CaloCluster *cluster, const CaloDmCell *dmCell, double &energy, double &distance)
{
  energy = 0.0;
  distance = 0.0;
  float cls_eta = cluster->eta();
  for(unsigned int i=0; i<dmCell->m_region->m_CaloSampleNeighbours.size(); i++){
     xAOD::CaloCluster::CaloSample nsmp = (xAOD::CaloCluster::CaloSample)dmCell->m_region->m_CaloSampleNeighbours[i];
    if( (dmCell->m_region->m_CaloSampleEtaMin[i]-0.2) <= cls_eta  &&
         cls_eta <= (dmCell->m_region->m_CaloSampleEtaMax[i]+0.2) ){
      energy += cluster->eSample(nsmp);
    }
  }

//   double phi_diff=dmCell->phi()-cluster->phi();
//   if(phi_diff <= -M_PI){
//     phi_diff += 2.*M_PI;
//   } else if (phi_diff > M_PI){
//     phi_diff -= 2.*M_PI;
//   }
//   float distance2=sqrt(pow((dmCell->eta()-cls_eta),2)+pow(phi_diff,2));

  HepLorentzVector hlv_cls(1,0,0,1);
  hlv_cls.setREtaPhi(1./cosh(cluster->eta()),cluster->eta(),cluster->phi());
  HepLorentzVector hlv_hit(1,0,0,1);
  hlv_hit.setREtaPhi(1./cosh(dmCell->eta()),dmCell->eta(),dmCell->phi());
  distance = hlv_cls.angle(hlv_hit.vect());

//   std::cout << "cls_eta_phi:" << cluster->eta() << " " << cluster->phi()
//       << " dmhit_eta_phi:" << dmCell->eta() << " " << dmCell->phi() 
//       << " an:" << hlv_cls.angle(hlv_hit.vect()) << " " << hlv_hit.angle(hlv_cls.vect())
//       << " " << distance2
//       << std::endl;

  return true;
}



/* ****************************************************************************
Makes vector of dead material "cells" from DM calibration hit container.
One DM cell contains:
- pointers to DM region, DM description element
- Energies summed over all DM calibration hit with same Identifier
**************************************************************************** */
int CaloDmEnergy::make_dmcell_vector(std::vector<const CaloCalibrationHitContainer *> &v_dmcchc)
{
  MsgStream log(Athena::getMessageSvc(), "CaloDmEnergy");

  m_dmsum_etotal = 0.0;
  m_dmsum_em = 0.0;
  m_dmsum_nonem = 0.0;
  m_dmsum_invisible = 0.0;
  m_dmsum_escaped = 0.0;
  
  m_dmsum_area_etotal.clear();
  m_dmsum_area_etotal.resize(DMA_MAX,0.0);

  for(std::vector<CaloDmCell *>::iterator it=m_CaloDmCellVector.begin(); it!=m_CaloDmCellVector.end(); it++) {
    delete (*it);
  }
  m_CaloDmCellVector.clear();
  m_CaloDmCellContainer.clear();
  m_CaloDmCellContainer.resize(m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash_max(), 0);

  int dmhit_n = 0;
  int dmhit_ntotal = 0;
  int dmcell_n = 0;
  for (std::vector<const CaloCalibrationHitContainer * >::const_iterator itCont=v_dmcchc.begin();itCont!=v_dmcchc.end();itCont++) {
    const CaloCalibrationHitContainer* hitContainer= (*itCont);
    CaloCalibrationHitContainer::const_iterator it;
    for(it = hitContainer->begin(); it!=hitContainer->end(); it++) {
      m_dmsum_etotal += (*it)->energyTotal();
      m_dmsum_em += (*it)->energyEM();
      m_dmsum_nonem += (*it)->energyNonEM();
      m_dmsum_invisible += (*it)->energyInvisible();
      m_dmsum_escaped += (*it)->energyEscaped();
      if(dmhit_n < m_max_dmhit) {
        Identifier id  = (*it)->cellID();
        // Check ID for validity
        if (m_id_helper->is_lar_dm(id) || m_id_helper->is_tile_dm(id)) {
          IdentifierHash id_hash;
          // calculation of global hash index of DM cell
          if(m_id_helper->is_lar_dm(id)) {
            id_hash = m_caloDM_ID->lar_zone_hash(id);
          } else if(m_id_helper->is_tile_dm(id)){
            id_hash = m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash(id);
          }
          int narea = get_area(id, m_caloDmDescrManager->get_element(id)->eta());
          // saving sums of DM energy in different areas
          m_dmsum_area_etotal[narea] += (*it)->energyTotal();
          m_dmsum_area_etotal[DMA_ALL] += (*it)->energyTotal();

          // DmCell creation: put cell into container if it doesn't exist
          if( !m_CaloDmCellContainer[id_hash] ){
            CaloDmCell *cell = new CaloDmCell((*it), m_caloDmDescrManager->get_element(id), m_caloDmDescrManager->get_dm_region(id));
            cell->SetAreaN(narea);
            m_CaloDmCellContainer[id_hash] = cell;
            dmcell_n++;
            // otherwise add energies into existing hits
          } else{
            CaloDmCell *cell = m_CaloDmCellContainer[id_hash];
            cell->Add( (*it) );
          }
          dmhit_n++;
        } else {
          log << MSG::WARNING<<"CaloDmEnergy::process() -> Alien identifier "
              << m_id_helper->show_to_string(id)
              << " in container '" << (*itCont)->Name()
              << "'." << endmsg ;
        } // is lar_id, tile_id
      }// dmhit_n < m_max_dmhit
      if(dmhit_ntotal == m_max_dmhit) {
        log << MSG::WARNING<<"CaloDmEnergy::process() -> Number of DM calibration hits exceed " << m_max_dmhit << endmsg;
      }
      dmhit_ntotal++;
    } // loop over hits 
  } // loop over containers

  for(std::vector<CaloDmCell *>::iterator it=m_CaloDmCellContainer.begin(); it!=m_CaloDmCellContainer.end(); it++){
    if( (*it) != 0) m_CaloDmCellVector.push_back((*it));
  }
  return 0;
}



/* ****************************************************************************
Return key number of DM cell
**************************************************************************** */
int CaloDmEnergy::get_key(Identifier id) const
{
  int subdet = m_caloDM_ID->pos_neg_z(id);
  int type = m_caloDM_ID->dmat(id);
  int sampling = m_caloDM_ID->sampling(id);
  int region = m_caloDM_ID->region(id);
  return abs(subdet)*1000+type*100+sampling*10+region;
}



/* ****************************************************************************
Return number of DM area for given DM cell
**************************************************************************** */
int CaloDmEnergy::get_area(Identifier id, float eta_in) const
{
  int nsmp=DMA_UNCLASS;
  int key=get_key(id);
  float eta = fabs(eta_in);

  //   if(key == 4202 || key == 4203) nsmp=DMA_HEC0_AZIMUTH;
  //   if(key == 4212 || key == 4213) nsmp=DMA_HEC1_AZIMUTH;
  //   if(key == 4222 || key == 4223) nsmp=DMA_HEC2_AZIMUTH;
  //   if(key == 4232 || key == 4233) nsmp=DMA_HEC3_AZIMUTH;
  //   if(key == 4204 || key == 4214) nsmp=DMA_HEC1_FCAL;
  //   if(key==4224) nsmp=DMA_HEC2_FCAL;
  //   if(key==4234) nsmp=DMA_HEC3_FCAL;
  //   if(key == 4117) nsmp = DMA_FCAL01;
  //   if(key == 4205) nsmp = DMA_FCAL02;
  //   if(key == 4215) nsmp = DMA_FCAL03;

  if(key >=4101 && key <=4105) { // inner detector
    if(eta <1.5) {
      nsmp = DMA_EMB0;
    } else if (eta>=1.5 && eta<1.6){
      nsmp = DMA_SCN;
    } else if (eta>=1.6 && eta<1.8){
      nsmp = DMA_EME0;
    } else if (eta>=1.8 && eta<2.5){
      nsmp = DMA_EME13;
    } else if (eta>=2.5 && eta<3.2){
      nsmp = DMA_EME14;
    } else if (eta>=3.2) {
      // nsmp = DMA_FCAL01;
      nsmp = DMA_FCAL0;
    }
  } else if(key==4110 || key==4111 || key==4112) {
    nsmp = DMA_EMB0;
  } else if(key==4113) {
    nsmp = DMA_EMB1;
  }else if(key==4114 || key==4122 || key==4121) {
    nsmp = DMA_SCN;
  } else if(key==4115) {
    nsmp = DMA_EME0;
  } else if(key==4116) {
    if(eta<1.5){
      nsmp = DMA_EME11;
    }else if(eta<1.8){
      nsmp = DMA_EME12;
    }else if(eta<2.5){
      nsmp = DMA_EME13;
    }else if(eta<3.2){
      nsmp = DMA_EME14;
    }
  } else if(key==4120) {
    nsmp = DMA_EMB3_TILE0;
  } else if(key==4123) {
    nsmp = DMA_EME3_HEC0;
  } else if(key==4125) {
    nsmp = DMA_EME_EXT;
  } else if((key >=4130 && key<=4132) || key==5130) {
    nsmp = DMA_LEAK;
  } else if(key == 4202 || key == 4203 || key == 4212 || key == 4213 
     || key == 4222 || key == 4223 || key == 4232 || key == 4233 ) {
    nsmp=DMA_HEC_AZIMUTH;
  } else if(key == 4204 || key == 4214 || key==4224 || key == 4234) {
    nsmp=DMA_HEC_FCAL;
  } else if(key == 4117 || key==4205 || key==4215 ) {
    nsmp = DMA_FCAL0;
  } else if(key == 4225) {
    nsmp = DMA_FCAL0_FCAL1;
  } else if(key == 4235) {
    nsmp = DMA_FCAL1_FCAL2;
  }else if(key == 4100) {
    nsmp = DMA_DEFCALC;
  } else if(key == 4124) {
    nsmp = DMA_HECWHEEL;
  }else if(key == 4200 || key == 4210|| key == 4220|| key == 4230) {
    nsmp = DMA_EMB_HALVES;
  }else if(key == 4201 || key == 4211|| key == 4221|| key == 4231) {
    nsmp = DMA_EME_INNER_OUTER;
  } else if(key == 5120 || key == 5121|| key == 5122) {
    nsmp = DMA_LEAK;
  } else if(key == 5200) {
    nsmp = DMA_LEAK; // actually it's TILE default calculator, let it be inside DMA_LEAK
  }

  if(nsmp == DMA_UNCLASS ) {
    std::cout << " CaloDmEnergy::get_area() -> Warning! Can not classify dead material hit: " 
        << " " << m_id_helper->show_to_string(id)
        << " key:" << key
        << std::endl;
  }

  return nsmp;
}



/* **************************************************************************
Get eta and phi size of calorimeter cells. If cell belongs to FCAL, 
then deta, dphi size will be calculated, since DetectorDescription has
deta=0 and dphi=0 for FCAL cells.
************************************************************************** */
int CaloDmEnergy::get_calo_deta_dphi(Identifier &id_cel, double &deta_cel, double &dphi_cel)
{
  dphi_cel=0;
  deta_cel=0;
  const CaloDetDescrElement* theCDDE = m_caloDetDescrManager->get_element(id_cel);
  if(!theCDDE){
    std::cout << "CaloDmEnergy::get_calo_deta_dphi() -> WARNING. Absent CDDE for " << m_id_helper->show_to_string(id_cel) << std::endl;
    return 1;
  }
  deta_cel = theCDDE->deta();
  dphi_cel = theCDDE->dphi();
  float eta_cel = fabs(theCDDE->eta());
  int subcalo = m_caloCell_ID->sub_calo(id_cel);
  if ( (CaloCell_ID::SUBCALO) subcalo  == CaloCell_ID::LARFCAL) {
    int nfcal = m_caloCell_ID->sampling(id_cel) - 1;
    float dsize[3]={7.5,8.0,9.0}; // tube size (x or y) in mm for fcal1-3
                  // ssize = 2.0*dsize*dsize*sqrt(3.0)/4.0;
    float ssize[3]={48.713928, 55.425625, 70.148056}; // cell area in mm*mm for fcal1-3
    float z = fabs(theCDDE->z());
    float volume = theCDDE->volume();
    float N = volume/(ssize[nfcal]*450.0); // number of tubes for given FCAL cell, 450. it's fcal length
    float dr = sqrt(N)*dsize[nfcal]; // cell size
    float dr_z = dr/z;
    dphi_cel = dr_z*sinh(eta_cel);
    deta_cel = dr_z*cosh(eta_cel);
    if( eta_cel < 3.3) deta_cel *= 1.5;
  } else if ( (CaloCell_ID::SUBCALO) subcalo == CaloCell_ID::LARHEC && eta_cel > 3.1) {
    deta_cel *= 1.5;
  } else if ( (CaloCell_ID::SUBCALO) subcalo == CaloCell_ID::LAREM && eta_cel > 3.1) {
    deta_cel *= 1.5;
  }
  return 0;
}

