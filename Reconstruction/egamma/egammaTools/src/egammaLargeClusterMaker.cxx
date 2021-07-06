/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaLargeClusterMaker.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
//#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

egammaLargeClusterMaker::egammaLargeClusterMaker(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<CaloClusterCollectionProcessor>(this);
}

StatusCode
egammaLargeClusterMaker::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(m_inputClusterCollection.initialize());
  ATH_CHECK(m_cellsKey.initialize());
  ATH_MSG_DEBUG("Initialization successful");
  return StatusCode::SUCCESS;
}

StatusCode
egammaLargeClusterMaker::execute(const EventContext& ctx,
                                 xAOD::CaloClusterContainer* collection) const
{

  if (!collection) {
    ATH_MSG_ERROR("A null collection was passed, which should never happen");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::CaloClusterContainer> inputClusters(
    m_inputClusterCollection, ctx);
  // retrieve the cell containers
  SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey, ctx);

  const CaloDetDescrManager* dd_man = nullptr;
  ATH_CHECK(detStore()->retrieve(dd_man, "CaloMgr"));
  
  // The main loop over clusters
  for (const auto *cluster : *inputClusters) {
    
    if (!m_isFWD) {
      // find the center of the cluster, copying the logic of
      // egammaMiddleShape.cxx
      
      // check if cluster is in barrel or in the end-cap
      if (!cluster->inBarrel() && !cluster->inEndcap()) {
	ATH_MSG_DEBUG(" Cluster is neither in Barrel nor in Endcap; skipping ");
	continue;
      }
      
      // check if cluster is in barrel or end-cap
      bool in_barrel = egammaEnergyPositionAllSamples::inBarrel(*cluster, 2);
      CaloSampling::CaloSample sam = CaloSampling::EMB2;
      if (in_barrel) {
	sam = CaloSampling::EMB2;
      } else {
	sam = CaloSampling::EME2;
      }
      
      // granularity in (eta,phi) in the pre sampler
      // CaloCellList needs both enums: subCalo and CaloSample
      auto eta = cluster->etaSample(sam);
      auto phi = cluster->phiSample(sam);
      
      if ((eta == 0. && phi == 0.) || fabs(eta) > 100) {
	return StatusCode::SUCCESS;
      }
      
      // Should get overritten
      bool barrel = false;
      CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LAREM;
      int sampling_or_module = 0;
      
      CaloDetDescrManager::decode_sample(
      subcalo, barrel, sampling_or_module, (CaloCell_ID::CaloSample)sam);
      
      // Get the corresponding grannularities : needs to know where you are
      //                  the easiest is to look for the CaloDetDescrElement
      
      const CaloDetDescrElement* dde =
	dd_man->get_element(CaloCell_ID::LAREM, 0, barrel, eta, phi);
      
      // if object does not exist then return
      if (!dde) {
	return StatusCode::SUCCESS;
      }
      
      // local granularity
      auto deta = dde->deta();
      auto dphi = dde->dphi();
      
      // search the hottest cell around the (eta,phi)
      // (eta,phi) are defined as etaSample() and phiSample
      // around this position a hot cell is searched for in a window
      // (m_neta*m_deta,m_nphi*m_dphi), by default (m_neta,m_nphi)=(7,7)
      CaloLayerCalculator calc;
      StatusCode sc = calc.fill(*dd_man,
				cellcoll.ptr(),
				cluster->etaSample(sam),
				cluster->phiSample(sam),
				m_neta * deta,
				m_nphi * dphi,
				(CaloSampling::CaloSample)sam);
      if (sc.isFailure()) {
	ATH_MSG_WARNING("CaloLayerCalculator failed  fill ");
      }
      double etamax = calc.etarmax();
      double phimax = calc.phirmax();
      
      // create the new cluster
      xAOD::CaloCluster* newCluster =
	CaloClusterStoreHelper::makeCluster(collection, cellcoll.ptr());
      newCluster->setEta0(etamax);
      newCluster->setPhi0(phimax);
      newCluster->setClusterSize(xAOD::CaloCluster::SW_7_11);
      
    } else { 
      // FWD cluster collection
      
      // check if cluster is in EMEC or FCAL 
      if (cluster->inBarrel()) {
	ATH_MSG_DEBUG(" Cluster is not in FWD; skip ");
	continue;
      } else {
	ATH_MSG_DEBUG (" CLuster is FWD Cluster ");
      }
      
      // check if cluster is in FCAL or EMEC
      bool in_EMEC = false;
      if (cluster->eSample(CaloSampling::EME2) > cluster->eSample(CaloSampling::FCAL0) )
	in_EMEC = true;
      
      CaloSampling::CaloSample sam = CaloSampling::FCAL0;
      if (in_EMEC) {
	sam = CaloSampling::EME2;
      } else {
	sam = CaloSampling::FCAL0;
      }
      
      // granularity in (eta,phi) in the pre sampler
      // CaloCellList needs both enums: subCalo and CaloSample
      auto eta = cluster->etaSample(sam);
      auto phi = cluster->phiSample(sam);
       
      if ((eta == 0. && phi == 0.) || fabs(eta) > 100) {
	return StatusCode::SUCCESS;
      }
      
      // Should get overritten
      bool emec = false;
      CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LARFCAL;
      int sampling_or_module = 0;
      
      CaloDetDescrManager::decode_sample(subcalo, emec, sampling_or_module, (CaloCell_ID::CaloSample)sam);
      
      // Get the corresponding grannularities : needs to know where you are
      //                  the easiest is to look for the CaloDetDescrElement
      //    const CaloDetDescrElement* get_element_FCAL (const CaloDetDescriptor* reg, double eta, double phi) const;
      const CaloDetDescrElement* dde =
	dd_man->get_element(subcalo, sampling_or_module, emec, eta, phi);
      
      // if object does not exist then return
      if (!dde) {
	return StatusCode::SUCCESS;
      }
      
      // local granularity
      auto deta = dde->deta();
      auto dphi = dde->dphi();
      
      // search the hottest cell around the (eta,phi)
      // (eta,phi) are defined as etaSample() and phiSample
      // around this position a hot cell is searched for in a window
      // (m_neta*m_deta,m_nphi*m_dphi), by default (m_neta,m_nphi)=(6,6) 
      // for EMEC-OUTER FWD much bigger cell size 
      
      // create the new cluster
      xAOD::CaloCluster* newCluster =
	CaloClusterStoreHelper::makeCluster(collection, cellcoll.ptr());
      
      // if In EMEC
      CaloLayerCalculator calc;
      StatusCode sc = calc.fill(*dd_man,
				cellcoll.ptr(),
				cluster->etaSample(sam),
				cluster->phiSample(sam),
				m_netaFWD * deta,
				m_nphiFWD * dphi,
				(CaloSampling::CaloSample)sam, in_EMEC ? newCluster : nullptr);
      
      if (sc.isFailure()) {
	ATH_MSG_WARNING("CaloLayerCalculator failed  fill for FWD cluster");
      }
      double etamax = calc.etarmax();
      double phimax = calc.phirmax();
      
      newCluster->setEta0(etamax);
      newCluster->setPhi0(phimax);
      
      if (!in_EMEC) {
	// If FCAL need to add cell to cluster in a cone
	std::vector<const CaloCell*> cells;
	cells.reserve(300);
	CaloCellList myList(cellcoll.ptr());
	myList.select(cluster->etaSample(sam), cluster->phiSample(sam), m_drFWD,(CaloSampling::CaloSample)sam);
	//  myList.select(dde,newCluster->eta0(), newCluster->phi0(),m_drFWD,(CaloSampling::CaloSample)sam); 
	cells.insert(cells.end(), myList.begin(), myList.end());
	
	for ( const auto *cell : cells ) {
	  if( !cell || !cell->caloDDE() ) continue;
	  int index = cellcoll.ptr()->findIndex(cell->caloDDE()->calo_hash());
	  if( index == -1 ) continue;
	  newCluster->addCell(index,1.);
	}
      }
    }// end isFWD
  }// main loop over clusters
  return StatusCode::SUCCESS;
}
