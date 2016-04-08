/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaAllFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   D.O. Damazio
// 
//
// ********************************************************************

#include "CaloIdentifier/LArEM_ID.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloEgamma/EgammaAllFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

EgammaAllFex::EgammaAllFex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
		   declareProperty("IncludeHad",  m_includeHad = false );
#ifndef NDEBUG
	// Create Geometry object
        // 0 -> CaloType EM, 2 -> Second Layer
//        m_geometry[0] = new T2Geometry(0,2);
#endif
}

EgammaAllFex::~EgammaAllFex(){
}

StatusCode EgammaAllFex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				 const IRoiDescriptor& roi) { 
  
  // Time total AlgTool time 
  if (!m_timersvc.empty()) {
    m_timer[0]->start();      
    m_timer[1]->start();      
    m_timer[1]->pause();      
    m_timer[2]->start();      
    m_timer[2]->pause();      
    m_timer[3]->start();      
    m_timer[3]->pause();      
  }
  // reset error
  m_error=0x0;
  int ncells = 0;
  // LVL1 positions
  float etaL1=rtrigEmCluster.eta();
  float phiL1=rtrigEmCluster.phi();
  
  double totalEnergy = 0;
  double totalEnergyEM = 0;
  CaloSampling::CaloSample samp;

  ATH_MSG_DEBUG( "in execute(TrigEMCluster&)" );

	// Time to access RegionSelector

     for(DETID dd = TTEM; dd <= TTHEC; dd=(DETID)( ((int)dd)+1) ){
	for(int sampling=0;sampling<4;sampling++) {
	// Get detector offline ID's for Collections
	if (!m_timersvc.empty()) m_timer[1]->resume();
	m_data->RegionSelector(sampling, roi, dd);

	// Finished to access RegionSelector
	if (!m_timersvc.empty()) m_timer[1]->pause();
	// Time to access Collection (and ByteStreamCnv ROBs)
	if (!m_timersvc.empty()) m_timer[2]->resume();
	if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() ){
		if (!m_timersvc.empty()) {
		m_timer[3]->stop();      
		m_timer[2]->stop();      
		m_timer[1]->stop();      
		m_timer[0]->stop();      
		}
		return StatusCode::SUCCESS;
	}
	m_error|=m_data->report_error();
	if ( m_saveCells && !m_error ){
	   m_data->storeCells(m_iBegin,m_iEnd,*m_CaloCellContPoint,m_cellkeepthr);
        }
	// Finished to access Collection
	if (!m_timersvc.empty()) m_timer[2]->pause();      
	// Algorithmic time
	if (!m_timersvc.empty()) m_timer[3]->resume();

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

    const LArCell* larcell = (*m_it);
    double energyCell = larcell->energy();
    totalEnergy += energyCell;
    if( dd==TTEM) totalEnergyEM += energyCell;
    //samp = CaloSampling::getSampling(*larcell);
    samp = larcell->caloDDE()->getSampling();
    rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
    rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    ncells++;

  }  // end of loop over sampling
  if (!m_timersvc.empty()) m_timer[3]->pause();
  } // end of loop over all samples   
  } // end of loop over DetID

// TileCAL
	if (!m_timersvc.empty()) m_timer[1]->resume();      
        m_data->RegionSelector(0, roi, TILE);
        // Finished to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->stop();

        for (unsigned int iR=0;iR<m_data->TileContSize();iR++) {

        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->resume();
        if ( m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure() ){

                if (!m_timersvc.empty()) {
			m_timer[3]->stop();
			m_timer[2]->stop();
			m_timer[0]->stop();
		}
                return StatusCode::SUCCESS;
        }
        m_error|=m_data->report_error();
        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->pause();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->resume();

  for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) {

    const TileCell* tilecell = (*m_itt);
    double energyCell = tilecell->energy();
    totalEnergy += energyCell;
    //samp = CaloSampling::getSampling(*tilecell);
    samp = tilecell->caloDDE()->getSampling();
    rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
    rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    ncells++;

  }  // end of loop over sampling
  if (!m_timersvc.empty()) m_timer[3]->pause();
  } // end of loop over Tile drawers

    
  if (!m_timersvc.empty()) m_timer[2]->stop();      
  if (!m_timersvc.empty()) m_timer[3]->stop();      
  // Save EMShowerMinimal time
  if (!m_timersvc.empty()) m_timer[4]->start();      

  
  // Update cluster Variables

  if ( m_includeHad )
  rtrigEmCluster.setEnergy(totalEnergy);
  else
  rtrigEmCluster.setEnergy(totalEnergyEM);
  rtrigEmCluster.setEta(etaL1);
  rtrigEmCluster.setPhi(phiL1);
  rtrigEmCluster.setNCells(ncells);
        
  // Finished save EMShowerMinimal time
  if (!m_timersvc.empty()) m_timer[4]->stop();      


  // Time total AlgTool time 
  if (!m_timersvc.empty()) m_timer[0]->stop();      

  return StatusCode::SUCCESS;
}
