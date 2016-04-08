/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaEmEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloEgamma/EgammaEmEnFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloEgamma/T2CalibrationEgamma.h"


EgammaEmEnFex::EgammaEmEnFex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
  declareProperty( "QlCorrectionLimit",
		    m_limit );
  declareProperty( "QlCorrectionDimension",
		    m_dimension );
  declareProperty( "QlCorrection",
		    m_correction );
	// Calibration object
	m_calib = new T2CalibrationEgamma();
}

EgammaEmEnFex::~EgammaEmEnFex(){
	delete m_calib;
}

StatusCode EgammaEmEnFex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				  const IRoiDescriptor& roi ){
 
        // Time total AlgTool time
        if (!m_timersvc.empty()) m_timer[0]->start();
        // reset error
        m_error=0x0;
        bool cluster_in_barrel = true;
        if ( m_caloDDE )
          cluster_in_barrel = m_caloDDE->is_lar_em_barrel();

        // MsgStream log(msgSvc(), name());
        ATH_MSG_DEBUG( "in execute(TrigEMCluster &)" );

        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->start();

        // Region Selector, sampling 0
        int sampling = 0;
        // Get detector offline ID's for Collections
        m_data->RegionSelector(sampling, roi );

        // Finished to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->pause(); 
        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->start();

        if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() ){
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
        if (!m_timersvc.empty()) m_timer[3]->start();
  
  double deta = 0.;           // eta difference current cell - seed
  double dphi = 0.;           // phi difference current cell - seed

  double totalEnergy = 0;
  CaloSampling::CaloSample samp;  

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  if ( m_caloDDE ){
        energyEta = m_caloDDE->eta();
        energyPhi = m_caloDDE->phi();
  }

  int ncells = 0;
  
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {                                       // Should be revised for London scheme
    ncells++;

    const LArCell* larcell = (*m_it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

      // find the standard em cluster energy (3*7 cell, now sampling 0)
      // Find position of current cell w.r.t. seed
      deta = fabs( etaCell - energyEta );
      dphi = fabs( phiCell - energyPhi );

      if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
       bool condition37 = cluster_in_barrel && ( (deta <= 0.0375+0.0005) && (dphi <= 0.0875+0.0005) );
       bool condition55 = (!cluster_in_barrel) && ( (deta <= 0.0625+0.0005) && (dphi <= 0.0625+0.0005) );

       if ( condition37 || condition55 ) {

	totalEnergy += energyCell;
	//samp = CaloSampling::getSampling(*larcell);
	samp = larcell->caloDDE()->getSampling();
	rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
	rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);

      }      
    
  } // end of loop over sampling 0

        // Pause algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->pause();
        // Save EMShowerMinimal time
        if (!m_timersvc.empty()) m_timer[4]->start();

  // update stored variables


#ifndef NDEBUG
	// This will internaly define normal, narrow and large clusters
  if ( msgLvl(MSG::DEBUG) ) {
        if ( m_geometryTool->EtaPhiRange(0,0,energyEta, energyPhi))
          ATH_MSG_ERROR( "problems with EtaPhiRange" );
        PrintCluster(totalEnergy,0,0,CaloSampling::PreSamplerB
		,CaloSampling::PreSamplerE);
  }
#endif
        // Pause save EMShowerMinimal time
        if (!m_timersvc.empty()) m_timer[4]->pause();
        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->resume();

        // Region Selector, sampling 3
        sampling = 3;
        // Get detector offline ID's for Collections
        m_data->RegionSelector( sampling, roi );

        // Finished to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->stop();
        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();

	// One does not want to prepare the collection again
	// That is why LoadCollections is called with false
        if ( m_data->LoadCollections(m_iBegin,m_iEnd,sampling,false)
			.isFailure() ){
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
        if (!m_timersvc.empty()) m_timer[2]->stop();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();


  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {                                       // Should be revised for London scheme
    ncells++;
    const LArCell* larcell = (*m_it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

      // find the standard em cluster energy (3*7 cell, now sampling 0)
      // Find position of current cell w.r.t. seed
      deta = fabs( etaCell - energyEta );
      dphi = fabs( phiCell - energyPhi );
      if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
      
       bool condition37 = cluster_in_barrel && ( (deta <= 0.0375+0.001) && (dphi <= 0.0875+0.001) );
       bool condition55 = (!cluster_in_barrel) && ( (deta <= 0.0625+0.001) && (dphi <= 0.0625+0.001) );

       if ( condition37 || condition55 ) {

	totalEnergy += energyCell;
	//samp = CaloSampling::getSampling(*larcell);
	samp = larcell->caloDDE()->getSampling();
	rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
	rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);

      }      
    
  } // end of loop over sampling 3

        // Update Cluster Variables
        rtrigEmCluster.setNCells(ncells+rtrigEmCluster.nCells());
        rtrigEmCluster.setRawEnergy( rtrigEmCluster.rawEnergy() + totalEnergy );
	// The dependency with energy is not defined yet
	rtrigEmCluster.setEnergy( rtrigEmCluster.rawEnergy()*
			m_calib->Calib(energyEta,rtrigEmCluster.energy()) );

	// Algorithmic time
	if (!m_timersvc.empty()) m_timer[3]->stop();
	// Save EMShowerMinimal
	if (!m_timersvc.empty()) m_timer[4]->resume();
        rtrigEmCluster.setEt ( rtrigEmCluster.energy()/cosh(energyEta) );
        rtrigEmCluster.setRawEt ( rtrigEmCluster.rawEnergy()/cosh(energyEta) );

#ifndef NDEBUG
        // This will internaly define normal, narrow and large clusters
  if ( msgLvl(MSG::DEBUG) ) {
        if ( m_geometryTool->EtaPhiRange(0,3,energyEta, energyPhi))
          ATH_MSG_ERROR( "problems with EtaPhiRange" );

        PrintCluster(totalEnergy,0,3,CaloSampling::EMB3,CaloSampling::EME3);
  }
#endif
	// Save EMShowerMinimal
	if (!m_timersvc.empty()) m_timer[4]->stop();

  // Time total AlgTool time 
  if (!m_timersvc.empty()) m_timer[0]->stop();      

  return StatusCode::SUCCESS;
}

