/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwGap_v3.cxx
 * Gap corrections using rel17 single particle MC
 * - crack definition matches the one used by the calibration hits outside the crack: 
 * eta from 2nd sampling in calo frame should be between 1.425 and 1.55
 * - then, eta from the cluster is used to compute the TG3 energy and apply the correction
 */

#include "CaloSwGap_v3.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandle.h"
#include <cmath>
#include <iostream>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;
using CLHEP::pi;
using CLHEP::twopi;


namespace {


const double deta = 0.2;
const double dphi = twopi / 64. ;


} // anonymous namespace


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloSwGap_v3::initialize()
{
  ATH_CHECK( CaloClusterCorrectionCommon::initialize() );
  ATH_CHECK( m_cells_name.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Virtual function for the correction-specific code.
 * @param myctx   ToolWithConstants context.
 * @param cluster The cluster to correct.
 *                It is updated in place.
 * @param elt     The detector description element corresponding
 *                to the cluster location.
 * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
 * @param adj_eta The @f$\eta@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
 * @param adj_phi The @f$\phi@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param samp    The calorimeter sampling we're examining.
 *                This is a sampling code as defined by
 *                @c CaloSampling::CaloSample; i.e., it has both
 *                the calorimeter region and sampling encoded.
 */
void CaloSwGap_v3::makeTheCorrection (const Context& myctx,
                                      CaloCluster* cluster,
                                      const CaloDetDescrElement*/*elt*/,
                                      float eta,
                                      float adj_eta,
                                      float /*phi*/,
                                      float /*adj_phi*/,
                                      CaloSampling::CaloSample /*samp*/) const
{
#if 0
  float the_aeta;
  float the_phi;
  if (m_use_raw_eta){
    the_aeta = std::abs (adj_eta);
    the_phi = adj_phi;
  }else{
    the_aeta = std::abs (eta);
    the_phi = phi;
  }
#endif
  float the_aeta_boundaries;
  if (m_use_raw_eta_boundaries(myctx))
    the_aeta_boundaries = std::abs (adj_eta);
  else
    the_aeta_boundaries = std::abs (eta);

  if (the_aeta_boundaries < m_etamin_crack(myctx) ||
      the_aeta_boundaries > m_etamax_crack(myctx))
  {
    return; // no correction required
  }

  // use cluster positions from now on
  float eta_clus = cluster->eta();
  float phi_clus = cluster->phi();

  SG::ReadHandle<CaloCellContainer> cc (m_cells_name, myctx.ctx());

  // Add up the tile scintillator energy in the region around the cluster.
  double eh_scint = 0;
  if(cc.isValid())
  {
    CaloCellContainer::const_iterator f_cell =
      cc->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator l_cell =
      cc->endConstCalo(CaloCell_ID::TILE);

    for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ; 

      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
        double phic = cell->phi();
        double etac = cell->eta(); 

	float diffeta = etac-eta_clus;
	float diffphi = phic-phi_clus;
	if (diffphi < -pi) diffphi += twopi;
	if (diffphi > pi)  diffphi -= twopi;    
	
	if(fabs(diffeta)<deta && fabs(diffphi)<dphi){
	  eh_scint += cell->e();
	}
      }
    }
  }
  
  
  // Find the correction weights: depends on phi position
  float a = 0.;
  float alpha = 0.;
  float offset = 0.;

  if(isGoodPhi(eta_clus,phi_clus)){
    const int degree = m_degree (myctx);
    const CxxUtils::Array<2> correctionGoodPhi = m_correctionGoodPhi(myctx);
    a = interpolate (correctionGoodPhi, fabs(eta_clus), degree, 1);
    alpha = interpolate (correctionGoodPhi, fabs(eta_clus), degree, 2);
    offset = interpolate (correctionGoodPhi, fabs(eta_clus), degree, 3);
  }
  else{
    const int degree = m_degree (myctx);
    const CxxUtils::Array<2> correctionBadPhi = m_correctionBadPhi(myctx);
    a = interpolate (correctionBadPhi, fabs(eta_clus), degree, 1);
    alpha = interpolate (correctionBadPhi, fabs(eta_clus), degree, 2);
    offset = interpolate (correctionBadPhi, fabs(eta_clus), degree, 3);
  }


  // The correction is a weighted sum of calorimeter and scintillator energies.
  float ec = cluster->e();

  // Sampling energies don't include scintillator contribution.
  setenergy (cluster, a*(ec + offset));
  cluster->setE (a*(ec + alpha*eh_scint + offset));
}


/**
 * @brief Change the name of the CaloCellContainer used by this tool.
 * @param name The new container name.
 */
StatusCode
CaloSwGap_v3::setCaloCellContainerName
  (const std::string& name)
{
  return this->setProperty (StringProperty ("cells_name", name));
}


bool CaloSwGap_v3::isGoodPhi(float eta, float phi) const{

  // get position of missing TG3 cells
  const int NBad = 8;
  int emptyTGEtaPosind[NBad] = {3,12,23,30,35,44,53,60};
  float emptyTGEtaPos[NBad];
  int emptyTGEtaNegind[NBad] = {4,13,20,28,37,45,54,61};
  float emptyTGEtaNeg[NBad];

  int Nmodules = 64; 
  float modSize = twopi / Nmodules; 
  float offset = modSize/2.; 
 
  for(int i=0;i<NBad;i++){ 
    float shift = (emptyTGEtaPosind[i]-1)*modSize; 
    if(shift>pi) shift-=twopi; 
    emptyTGEtaPos[i] = offset + shift; 
    
    shift = (emptyTGEtaNegind[i]-1)*modSize; 
    if(shift>pi) shift-=twopi; 
    emptyTGEtaNeg[i] = offset + shift; 
  } 
   
  // now test whether the current position matches
  // the bad TG3 cells position

  if(eta>0){
    for(int ipos=0;ipos<NBad;ipos++){
      float min = emptyTGEtaPos[ipos]-0.05;
      float max = emptyTGEtaPos[ipos]+0.05;
      if(phi>min && phi<max) {
	return false;
      }
    }
  }
  if(eta<0){
    for(int ipos=0;ipos<NBad;ipos++){
      float min = emptyTGEtaNeg[ipos]-0.05;
      float max = emptyTGEtaNeg[ipos]+0.05;
      if(phi>min && phi<max) {
	return false;
	}
    }
  }
  
  return true;

}
