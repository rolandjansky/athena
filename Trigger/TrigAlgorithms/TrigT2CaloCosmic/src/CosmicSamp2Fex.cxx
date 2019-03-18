/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     CosmicSamp2Fex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCosmic
// 
// AUTHOR:   M.P. Casado
// 
//
// ********************************************************************

#include "CaloIdentifier/LArEM_ID.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloCosmic/CosmicSamp2Fex.h"
#include "TrigT2CaloCommon/Calo_Def.h"


CosmicSamp2Fex::CosmicSamp2Fex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
	declareProperty("THR_1",m_THR_1 = 400.0);
	declareProperty("THR_2",m_THR_2 = 100.0);
	//#ifndef NDEBUG
	// Create Geometry object
        // 0 -> CaloType EM, 2 -> Second Layer
	//        m_geometry[0] = new T2Geometry(0,2);
	//#endif
}

CosmicSamp2Fex::~CosmicSamp2Fex(){
}

StatusCode CosmicSamp2Fex::execute(TrigEMCluster &rtrigEmCluster,
				   const IRoiDescriptor& roi,
				   const CaloDetDescrElement*& /*caloDDE*/) { 
  
        m_lardecoded=false;
	m_tiledecoded=false;
	// Time total AlgTool time 
	if (!m_timersvc.empty()) m_timer[0]->start();      

	if (msgLvl(MSG::DEBUG)) 
	  ATH_MSG_DEBUG("in execute(TrigEMCluster&)");

	// Time to access RegionSelector
	if (!m_timersvc.empty()) m_timer[1]->start();      

	// Region Selector, sampling 2
	int sampling = 2;
	// Get detector offline ID's for Collections
	m_data->RegionSelector(sampling, roi);

	// Finished to access RegionSelector
	if (!m_timersvc.empty()) m_timer[1]->stop();      
	// Time to access Collection (and ByteStreamCnv ROBs)
	if (!m_timersvc.empty()) m_timer[2]->start();      

	if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() )
		return StatusCode::FAILURE;
	m_lardecoded=true;
	if ( m_saveCells ) IAlgToolCalo::storeCells();
	// Finished to access Collection
	if (!m_timersvc.empty()) m_timer[2]->stop();      
	// Algorithmic time
	if (!m_timersvc.empty()) m_timer[3]->start();      

        //double energyEta = 0.;
        //double energyPhi = 0.;
 
        // add these variables to take care of phi wrap-around
        //double energyNegPhi = 0.; //SRA
        //double energyNegPhiConv = 0.; //SRA
        //double energyPosPhi = 0.; //SRA


  // 1. Find seed cell (highest Et in ROI .. layer 2)
  // 2. Find Et weighted eta, phi in 3*7 cell (layer 2) (photon + e id)
  // 3. Find Et in cells of sizes 3*3, 3*7, 7*7 (layer 2 + strips)
  //	    				        (3*7 for photon + e id)
  // 4. Find cluster width in 3*5 cell, layer 2 (photon id, needs
  //		 			         parabolic parametrisation)
  // 5. Find strip energies and eta (2*5 window)
  // 6. Find frac73 (photon id), (E1-E2)/(E1+E2) (e + photon id)
  
  double seedEnergy = 0.; 
  double seedPhi    = 0.;
  double seedEta    = 999.;
  
  //const LArEM_ID*   emID  = m_larMgr->getEM_ID();
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
      if ((*m_it)->energy() > seedEnergy && (*m_it)->energy() > m_THR_1) {
        seedEnergy = (*m_it)->energy();
        seedEta    = (*m_it)->eta();
        seedPhi    = (*m_it)->phi();
      } 
  }

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

    double etaCell = (*m_it)->eta();
    double phiCell = (*m_it)->phi();
    double energyCell = (*m_it)->energy();

    // find position of current cell w.r.t. seed
    float deta = fabs( etaCell - seedEta );
    float dphi = fabs( phiCell - seedPhi );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
    // hadronic measurements: energy sum in 0.1*0.1
    if ( deta <= 0.1  &&
         dphi <= 0.1  && energyCell>m_THR_2){
      //CaloSampling::CaloSample samp = CaloSampling::getSampling(*(*m_it));
      CaloSampling::CaloSample samp = (*m_it)->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    }
  }

  //double totalEnergy = 0;
  //CaloSampling::CaloSample samp;

     
	if (!m_timersvc.empty()) m_timer[3]->stop();      
	// Save EMShowerMinimal time
	if (!m_timersvc.empty()) m_timer[4]->start();      

  
  // Update cluster Variables

  rtrigEmCluster.set_e277(999);
  rtrigEmCluster.set_e2tsts1(999);
  rtrigEmCluster.setRawEnergy( seedEnergy );
  rtrigEmCluster.setEnergy( seedEnergy );
  rtrigEmCluster.setEta(seedEta);
  rtrigEmCluster.setPhi(seedPhi);
  rtrigEmCluster.setRawEta(seedEta);
  rtrigEmCluster.setRawPhi(seedPhi);
        
	// Finished save EMShowerMinimal time
	if (!m_timersvc.empty()) m_timer[4]->stop();      


	// Time total AlgTool time 
	if (!m_timersvc.empty()) m_timer[0]->stop();      

  return StatusCode::SUCCESS;
}

double CosmicSamp2Fex::etaSizeLArEMSamp2(double cellEta, int calo) const{
  double sizeEta;
  if( calo == Calorimeter::EMBAR ){             
      if ( fabs(cellEta) < 1.4 ){
        sizeEta = 0.025;
      }else{
        sizeEta = 0.075;
      }
  } else {             
      if ( fabs(cellEta) < 2.5 ){
	sizeEta = 0.025;
      } else{
	sizeEta = 0.1;
      }		
  }
  return sizeEta;
}

double CosmicSamp2Fex::phiSizeLArEMSamp2(double cellEta, int calo) const{
  double sizePhi;
  if( calo == Calorimeter::EMBAR ){               
    sizePhi = 0.025;
  } else {             
      if ( fabs(cellEta) < 2.5 ){
	sizePhi = 0.025;
      } else {
	sizePhi = 0.1;
      }		
  }
  return sizePhi;
}
