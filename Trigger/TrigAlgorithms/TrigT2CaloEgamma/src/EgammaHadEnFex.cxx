/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaHadEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   M.P. Casado
// 
// REFERENCES: Based on Trigger/TrigT2CaloEgamma/T2EmHadAlgorithm.cxx
//
// ********************************************************************

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloEgamma/EgammaHadEnFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include <math.h>

inline double check_tilemin(const double x){
const double dphi=0.09817477;
const double oneoverdphi=1.0/0.09817477;
if(x>=0)
        return (dphi*ceilf(x*oneoverdphi))+0.01;
else
        return (-dphi*floorf(-x*oneoverdphi))+0.01;
}
inline double check_tilemax(const double x){
const double dphi=0.09817477;
const double oneoverdphi=1.0/0.09817477;
if(x>=0)
        return (dphi*floorf(x*oneoverdphi))-0.01;
else
        return (-dphi*ceilf(-x*oneoverdphi))-0.01;
}

EgammaHadEnFex::EgammaHadEnFex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
}

EgammaHadEnFex::~EgammaHadEnFex(){
}

StatusCode EgammaHadEnFex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				   const IRoiDescriptor& roi ) { 
        // Time total AlgTool time
        if (!m_timersvc.empty()) m_timer[0]->start();
	m_error=0x0;

#ifndef NDEBUG
  if ( msg().level() <= MSG::DEBUG ) 
        msg() << MSG::INFO << "in execute(TrigEMCluster &)" << endreq;
#endif

  double deta = 0.;           // eta difference current cell - seed
  double dphi = 0.;           // phi difference current cell - seed

  CaloSampling::CaloSample samp;  

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  rtrigEmCluster.setEhad1(0.);

  // Dump eta-phi strip ranges into arrays
  // Define variables here so that their scope covers both the Tile and HEC loops
  // Use arrays of length 3 since there is no samp 0 in the HAD calorimeters 
  // this avoids wasting memory but makes the code slightly less clear

  // Time Region Selector 
  if (!m_timersvc.empty()) {
	m_timer[1]->start();      
	m_timer[1]->pause();      
	m_timer[2]->start();      
	m_timer[2]->pause();      
	m_timer[3]->start();      
	m_timer[3]->pause();      
	m_timer[4]->start();      
	m_timer[4]->pause();      
  }

  int ncells=0;

  for (unsigned int sampling = 0;sampling<3;sampling++) {                  

        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->resume();

        // Region Selector
        // Get detector offline ID's for Collections
	//        m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax,TTHEC);
        m_data->RegionSelector(sampling,roi,TTHEC);

        if (!m_timersvc.empty()) m_timer[1]->pause();
        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();

	// For the first sample you will create the containers
	// For the others no
        if ( m_data->LoadCollections(m_iBegin,m_iEnd,sampling
			,!sampling).isFailure() ){
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
	}
        m_error|=m_data->report_error();
/*
        if ( m_error ) {
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
        }
*/
        if ( m_saveCells ){
           m_data->storeCells(m_iBegin,m_iEnd,*m_CaloCellContPoint,m_cellkeepthr);
        }
        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();
	
    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

      ncells++;
      const LArCell* larcell = (*m_it);
      double etaCell = larcell->eta();
      double phiCell = larcell->phi();
      double energyCell = larcell->energy();

      // find position of current cell w.r.t. seed
      deta = fabs( etaCell - energyEta );
      dphi = fabs( phiCell - energyPhi );
      if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
      // hadronic measurements: energy sum in 0.1*0.1
      if ( deta <= 0.12  &&
	   dphi <= 0.12 ){ 
        //samp = CaloSampling::getSampling(*larcell);
        samp = larcell->caloDDE()->getSampling();
	rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
	rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
	if (CaloSampling::HEC0 == samp) {
	  rtrigEmCluster.setEhad1(rtrigEmCluster.ehad1() + energyCell );
	}
      }
	
    } // end of loop over cells 
    // End of Algorithmic time
    if (!m_timersvc.empty()) m_timer[3]->pause();
 
  } // End sampling loop     
#ifndef NDEBUG
  if ( msg().level() <= MSG::DEBUG ) {
	for (int sampling=0; sampling<3;sampling++)
          if (m_geometryTool->EtaPhiRange(1,sampling,energyEta, energyPhi))
                msg() << MSG::ERROR << "problems with EtaPhiRange" << endreq;
        PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC0),1,0
		,CaloSampling::HEC0,CaloSampling::HEC0);
        PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC1),1,1
		,CaloSampling::HEC1,CaloSampling::HEC1);
        PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC2),1,2
		,CaloSampling::HEC2,CaloSampling::HEC2);
  }
#endif

 // Next TILECAL
	// Needs some fix for RS
	
  
  // MS       phimin=check_tilemin(phimin);
  // MS       phimax=check_tilemax(phimax);
	
  
        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->resume();

        // Region Selector, no sample needed
        // Get detector offline ID's for Collections
	// MS m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);
	m_data->RegionSelector(0,roi,TILE);

        if (!m_timersvc.empty()) m_timer[1]->pause();

  for (unsigned int iR=0;iR<m_data->TileContSize();iR++) {

        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();
	// For the first sample you will create the containers
	// For the others no
        if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ){
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
	}
        m_error|=m_data->report_error();
/*
        if ( m_error ) {
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
        }
*/
	if ( m_saveCells ){
           m_data->storeCells(m_itBegin,m_itEnd,*m_CaloCellContPoint,m_cellkeepthr);
        }

        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();

   for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { //loop over cells
    
    ncells++;
    const TileCell* tilecell = (*m_itt);
    double etaCell = tilecell->eta();
    double phiCell = tilecell->phi();
    double energyCell = tilecell->energy();

    // find position of current cell w.r.t. seed
    deta = fabs( etaCell - energyEta );
    dphi = fabs( phiCell - energyPhi );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
    // hadronic measurements: energy sum in 0.1*0.1
    if ( deta <= 0.12  &&
	 dphi <= 0.12 ){ 
      //samp = CaloSampling::getSampling(*tilecell);
      samp = tilecell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
      if( (CaloSampling::TileBar0 == samp) || (CaloSampling::TileExt0 == samp) || (CaloSampling::TileGap1 == samp) || (CaloSampling::TileGap2 == samp) ) {
        rtrigEmCluster.setEhad1(rtrigEmCluster.ehad1() + energyCell);
      }
    }

   } // end of loop over cells 
   // Algorithmic time
   if (!m_timersvc.empty()) m_timer[3]->pause();
   
  } // End of loop over TileCal drawers
  rtrigEmCluster.setNCells(ncells+rtrigEmCluster.nCells() );
  m_timer[0]->propVal(rtrigEmCluster.nCells() );

#ifndef NDEBUG
  if ( msg().level() <= MSG::DEBUG ) {
        PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar0)
		+rtrigEmCluster.energy(CaloSampling::TileExt0),1,0
		,CaloSampling::TileBar0,CaloSampling::TileExt0);
        PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar1)
		+rtrigEmCluster.energy(CaloSampling::TileExt1),1,1
		,CaloSampling::TileBar1,CaloSampling::TileExt1);
        PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar2)
		+rtrigEmCluster.energy(CaloSampling::TileExt2),1,2
		,CaloSampling::TileBar2,CaloSampling::TileExt2);
  }
#endif
	// Stop all timers
        if (!m_timersvc.empty()){ 
		m_timer[4]->stop();
		m_timer[3]->stop();
		m_timer[2]->stop();
		m_timer[1]->stop();
		m_timer[0]->stop();
	}

  return StatusCode::SUCCESS;
}
