/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     CosmicHadEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCosmic
// 
// AUTHOR:   M.P. Casado
// 
// REFERENCES: Based on Trigger/TrigT2CaloCosmic/T2EmHadAlgorithm.cxx
//
// ********************************************************************

#include "TrigCaloEvent/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloCosmic/CosmicHadEnFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"


CosmicHadEnFex::CosmicHadEnFex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent),
		   m_THR_1(400),m_THR_2(100)
		   {
	declareProperty("THR_1",m_THR_1);
        declareProperty("THR_2",m_THR_2);
	//#ifndef NDEBUG
/*
	// 1 -> CaloType HAD, 0 -> First Layer
        m_geometry[0] = new T2Geometry(1,0);
        // 1 -> CaloType HAD, 1 -> Second Layer
        m_geometry[1] = new T2Geometry(1,1);
        // 1 -> CaloType HAD, 2 -> Third Layer
        m_geometry[2] = new T2Geometry(1,2);
*/
//#endif
}

CosmicHadEnFex::~CosmicHadEnFex(){
}

StatusCode CosmicHadEnFex::execute(TrigEMCluster &rtrigEmCluster,double etamin,
		double etamax, double phimin, double phimax){

	m_lardecoded=false;
	m_tiledecoded=false;
        // Time total AlgTool time
        if (!m_timersvc.empty()) m_timer[0]->start();

        // MsgStream log(msgSvc(), name());
	if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("in execute(TrigEMCluster &)");

  double deta = 0.;           // eta difference current cell - seed
  double dphi = 0.;           // phi difference current cell - seed

  CaloSampling::CaloSample samp;  

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  rtrigEmCluster.set_ehad1(0.);

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
  double seedEnergy = 0.;
  double seedPhi    = 0;
  double seedEta    = 999.;
  
   // TILECAL Assuming no knowledge about first sample
  
        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->resume();

        // Region Selector, no sample needed
        // Get detector offline ID's for Collections
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax,TILE);

        if (!m_timersvc.empty()) m_timer[1]->pause();

  for (unsigned int iR=0;iR<m_data->TileContSize();iR++) {

        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();
	// For the first sample you will create the containers
	// For the others no
        if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() )
                return StatusCode::FAILURE;
	m_tiledecoded=true;
	if ( m_saveCells ) IAlgToolCalo::storeCells();
        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();

   for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { //loop over cells
    
     //double etaCell = (*m_itt)->eta();
     //double phiCell = (*m_itt)->phi();
     //double energyCell = (*m_itt)->energy();
    if ((*m_itt)->energy() > seedEnergy && (*m_itt)->energy() > m_THR_1) {
        seedEnergy = (*m_itt)->energy();
        seedEta    = (*m_itt)->eta();
        seedPhi    = (*m_itt)->phi();
    }
    }
    if (!m_timersvc.empty()) m_timer[3]->pause();
    }
  energyEta = seedEta;
  energyPhi = seedPhi;
  rtrigEmCluster.setEta(seedEta);
  rtrigEmCluster.setPhi(seedPhi);
  rtrigEmCluster.setEnergy(seedEnergy);
  rtrigEmCluster.set_ehad1(0.);
  rtrigEmCluster.set_e277(999);
  rtrigEmCluster.set_e2tsts1(999);

  for (unsigned int iR=0;iR<m_data->TileContSize();iR++) {

        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();
	// For the first sample you will create the containers
	// For the others no
        if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() )
                return StatusCode::FAILURE;
        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();

   for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { //loop over cells
    
    double etaCell = (*m_itt)->eta();
    double phiCell = (*m_itt)->phi();
    double energyCell = (*m_itt)->energy();

    // find position of current cell w.r.t. seed
    deta = fabs( etaCell - energyEta );
    dphi = fabs( phiCell - energyPhi );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
    // hadronic measurements: energy sum in 0.1*0.1
    if ( deta <= 0.1  &&
	 dphi <= 0.1  && energyCell>m_THR_2){ 
      //samp = CaloSampling::getSampling(*(*m_itt));
      samp = (*m_itt)->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    }

   } // end of loop over cells 
   // Algorithmic time
   if (!m_timersvc.empty()) m_timer[3]->pause();
   
  } // End of loop over TileCal drawers


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
