/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaSamp2Fex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   M.P. Casado
// 
//
// ********************************************************************

#include "CaloIdentifier/LArEM_ID.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloEgamma/EgammaSamp2Fex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

#include "IRegionSelector/IRoiDescriptor.h"

EgammaSamp2Fex::EgammaSamp2Fex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
#ifndef NDEBUG
	// Create Geometry object
        // 0 -> CaloType EM, 2 -> Second Layer
//        m_geometry[0] = new T2Geometry(0,2);
#endif
	// By default no restriction is applied
	declareProperty("MaxDetaHotCell",m_maxHotCellDeta=1.0);
	declareProperty("MaxDphiHotCell",m_maxHotCellDphi=1.0);
}

EgammaSamp2Fex::~EgammaSamp2Fex(){
}

StatusCode EgammaSamp2Fex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				   const IRoiDescriptor& roi ) { 
  
	// Time total AlgTool time 
	if (!m_timersvc.empty()) m_timer[0]->start();      
	// reset error
	m_error=0x0;

        ATH_MSG_DEBUG( "in execute(TrigEMCluster&)" );

	// Time to access RegionSelector
	if (!m_timersvc.empty()) m_timer[1]->start();      

	// Region Selector, sampling 2
	int sampling = 2;
	// Get detector offline ID's for Collections
	m_data->RegionSelector( sampling, roi );

	// Finished to access RegionSelector
	if (!m_timersvc.empty()) m_timer[1]->stop();      
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
	if ( m_saveCells && !m_error ){
	   m_data->storeCells(m_iBegin,m_iEnd,*m_CaloCellContPoint,m_cellkeepthr);
        }
	// Finished to access Collection
	if (!m_timersvc.empty()) m_timer[2]->stop();      
	// Algorithmic time
	if (!m_timersvc.empty()) m_timer[3]->start();      

  double energyEta = 0.;
  double energyPhi = 0.;
 
  // add these variables to take care of phi wrap-around
  double energyNegPhi = 0.; //SRA
  double energyNegPhiConv = 0.; //SRA
  double energyPosPhi = 0.; //SRA


  // 1. Find seed cell (highest Et in ROI .. layer 2)
  // 2. Find Et weighted eta, phi in 3*7 cell (layer 2) (photon + e id)
  // 3. Find Et in cells of sizes 3*3, 3*7, 7*7 (layer 2 + strips)
  //	    				        (3*7 for photon + e id)
  // 4. Find cluster width in 3*5 cell, layer 2 (photon id, needs
  //		 			         parabolic parametrisation)
  // 5. Find strip energies and eta (2*5 window)
  // 6. Find frac73 (photon id), (E1-E2)/(E1+E2) (e + photon id)
  
  double seedEnergy = 0.;
  double seedPhi    = 999.;
  double seedEta    = 999.;
  double hotPhi    = 999.;
  double hotEta    = 999.;
  int ncells = 0;
  // LVL1 positions
  float etaL1=rtrigEmCluster.eta();
  float phiL1=rtrigEmCluster.phi();
  
  const LArEM_ID*   emID  = m_larMgr->getEM_ID();
  const LArCell* larcell;
  const LArCell* seedCell = NULL;
  const LArCell* hotCell = NULL;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
      larcell=(*m_it);
      if (larcell->energy() > seedEnergy) { // Hottest cell seach
	float deta=fabsf(etaL1-larcell->eta());
	if ( deta < m_maxHotCellDeta ){ // Eta check is faster. Do it First
	   float dphi=fabsf(phiL1-larcell->phi());
	   dphi = fabsf ( M_PI - dphi );
	   dphi = fabsf ( M_PI - dphi );
	   if ( dphi < m_maxHotCellDphi ) {
        	seedEnergy = larcell->energy();
		seedCell=larcell;
	   } // End of dphi check
	} // End of deta check
      } // End of if energy 
      ncells++;
  }
  if (seedCell!=NULL){
  seedEta    = seedCell->eta();
  seedPhi    = seedCell->phi();
  // For the S-shape correction, we store the caloDDE of the hottest cell
  setCaloDetDescrElement(seedCell->caloDDE());
  hotCell = seedCell;
  hotEta    = hotCell->eta();
  hotPhi    = hotCell->phi();
  } else { 
	m_error|=0x80000000;
	if (!m_timersvc.empty()) m_timer[3]->stop();      
	return StatusCode::SUCCESS;
  }

    std::map<const LArCell*,float> windows;
    for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
      larcell=(*m_it);
        float deta=fabsf(seedEta-larcell->eta());
        if ( deta < 0.025+0.002 ){ // Eta check is faster. Do it First
           float dphi=fabsf(seedPhi-larcell->phi());
           dphi = fabsf ( M_PI - dphi );
           dphi = fabsf ( M_PI - dphi );
           if ( dphi < 0.025+0.002 ) {
		if ( windows.find(larcell) == windows.end() ){
			windows[larcell]=0.0;
		} // end of windows check
           } // End of dphi check
        } // End of deta check
      ncells++;
  }
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

    const LArCell* larcell = (*m_it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->et();

    // Find position of current cell w.r.t. seed
    float deta = fabs( etaCell - seedEta );
    if ( deta > 0.05+0.002 ) continue;
    for(std::map<const LArCell*,float>::iterator witer=
		windows.begin(); witer!=windows.end(); ++witer){
    float deta1 = fabs( etaCell - witer->first->eta() );
    float dphi = fabs( phiCell - witer->first->phi() );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28


    // Electromagnetic measurements, done by layer
    // layer 2: Et weighted eta, phi, summed Et in 3*7,
    //          cluster width in 3*5 cell
    // emEnergy in 3*7 cell is also calculated, all samplings

    double cellSizeEta;
    double cellSizePhi;
    if (emID->is_em_barrel( larcell->ID() ) ) {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
    } else {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
    }

    // 3. do the 3*5 stuff
    int nCellsEta = 3;
    int nCellsPhi = 5;
    if ( deta1 <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
         dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01  ){
		witer->second+= energyCell;
    } // End of do the 3*5 stuff
    } // End of windows map loop
  } // end of the loop

  float max = -999;
  for(std::map<const LArCell*,float>::const_iterator witer=
		windows.begin(); witer!=windows.end(); ++witer){
        if ( witer->second > max ) {
		max = witer->second;
		seedCell = witer->first;
	}
  }

  if (seedCell!=NULL){
  seedEta    = seedCell->eta();
  seedPhi    = seedCell->phi();
  // For the S-shape correction, we store the caloDDE of the hottest cell
  setCaloDetDescrElement(seedCell->caloDDE());
  } else {
        m_error|=0x80000000;
        if (!m_timersvc.empty()) m_timer[3]->stop();
        return StatusCode::SUCCESS;
  }





  bool cluster_in_barrel = true;
  if ( seedCell && seedCell->caloDDE() )
	cluster_in_barrel = seedCell->caloDDE()->is_lar_em_barrel();

  // Build desired quantities using the values previously obtained.

  double energy37Lay2  = 0.;  // sum Et in 3*7 cell layer 2
  double energy37Lay2NegPhi = 0.; //SRA for phi wrap-around
  double energy37Lay2PosPhi = 0.; //SRA for phi wrap-around
  double energy77Lay2  = 0.;  // sum Et in 7*7 cell layer 2
  double energy35Lay2  = 0.;  // sum Et in 3*5 cell layer 2
  double energy33Lay2  = 0.;  // sum Et in 3*3 cell layer 2
  double weightEta     = 0.;  // temporary variable for cluster width
  double weightEta2    = 0.;  // ditto
  double clusterWidth35= 0.;  // cluster width in eta, 3*5 cell, layer 2
  int nCellsEta = 0;          // size of cell array in eta
  int nCellsPhi = 0;          // size of cell array in phi
  double deta = 0.;           // eta difference current cell - seed
  double dphi = 0.;           // phi difference current cell - seed
  double detaH = 0.;           // eta difference current cell - seed
  double dphiH = 0.;           // phi difference current cell - seed

  double totalEnergy = 0;
  CaloSampling::CaloSample samp;

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

    const LArCell* larcell = (*m_it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // Find position of current cell w.r.t. seed
    deta = fabs( etaCell - seedEta );
    dphi = fabs( phiCell - seedPhi );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28
 
    detaH = fabs( etaCell - hotEta );
    dphiH = fabs( phiCell - hotPhi );
    if ( dphiH > M_PI ) dphiH = 2.* M_PI - dphiH;   // wrap 0 -> 6.28
 
    // Electromagnetic measurements, done by layer
    // layer 2: Et weighted eta, phi, summed Et in 3*7,
    //          cluster width in 3*5 cell
    // emEnergy in 3*7 cell is also calculated, all samplings
          
    double cellSizeEta;
    double cellSizePhi;
    if (emID->is_em_barrel( larcell->ID() ) ) {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
    } else {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
    }

    // hot cell stuff first
    // 1. do the 7*7 stuff
    nCellsEta = 7;
    nCellsPhi = 7;
    if ( detaH <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
         dphiH <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01  ){
    // 2. do the 3*7 stuff
    nCellsEta = 3;
    nCellsPhi = 7;
    if ( detaH <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 &&
         dphiH <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01  ){
      energy37Lay2 += energyCell;
    } // End of do the 3*7 stuff
      energy77Lay2 += energyCell;
    } // End of do the 7*7 stuff
                         
    // 1. do the 7*7 stuff
    nCellsEta = 7;
    nCellsPhi = 7;
    if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.005 && 
	 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.005  ){
    // 2. do the 3*7 stuff
    nCellsEta = 3;
    nCellsPhi = 7;
    if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.002 && 
	 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.002  ){
      energyEta += energyCell * etaCell;
      //energy37Lay2 += energyCell;

      if ( cluster_in_barrel ){
      //totalEnergy += energyCell;
      //samp = CaloSampling::getSampling(*larcell);
	samp = larcell->caloDDE()->getSampling();
      //rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      //rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
      }

      if ( phiCell > 0.) {  // SRA phi wrap-around
	energyPosPhi += double(energyCell) * phiCell;
	energy37Lay2PosPhi += energyCell;
      } else {
	energyNegPhi += double(energyCell) * phiCell;
	energyNegPhiConv += double(energyCell) * (phiCell+2.0*M_PI);
	energy37Lay2NegPhi += energyCell;
      }
    // 3. do the 3*5 stuff 
    nCellsEta = 3;
    nCellsPhi = 5;
    if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 && 
	 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01  ){
      weightEta += energyCell * etaCell; 
      weightEta2 += energyCell * etaCell * etaCell; 
      energy35Lay2 += energyCell;
      // 3a. do the 3*3 stuff 
      //nCellsEta = 3;
      nCellsPhi = 3;
      if ( deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01 && 
	 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01  ){
         energy33Lay2 += energyCell;
      } // End of do the 3*3 stuff
    } // End of do the 3*5 stuff

    } // End of do the 3x7 stuff
    // 4. do the 5*5 stuff
    nCellsEta = 5;
    nCellsPhi = 5;
    if ( (!cluster_in_barrel) && deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.005 && 
	 dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.005  ){
       //totalEnergy += energyCell;
       //samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
       //rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
       //rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    } // End of do the 5*5 stuff
      //energy77Lay2 += energyCell;
    } // End of do the 7*7 stuff

  }  // end of loop over sampling 2
    
  // Normalise energy weighted angles and calculate values  

  // SRA phi wrap-around
  double AvgNegPhi = 0.;
  double AvgPosPhi = 0.;

  if ( (energy37Lay2PosPhi + energy37Lay2NegPhi) > 0. ){           // dont divide by zero
    energyEta /= (energy37Lay2PosPhi + energy37Lay2NegPhi) ;

    if (energy37Lay2NegPhi > 0. ){
       AvgNegPhi = energyNegPhi / energy37Lay2NegPhi;
    } else {
      AvgNegPhi = -999.0;
    }

    if (energy37Lay2PosPhi > 0. ){
      AvgPosPhi = energyPosPhi / energy37Lay2PosPhi;    
    } else {
      AvgPosPhi = -999.0;
    }

    if (AvgPosPhi==-999.0) {
      if (AvgNegPhi != -999.0) {
	energyPhi = AvgNegPhi;
      }
    }
    
    if (AvgNegPhi==-999.0) {
      if (AvgPosPhi != -999.0) {
	energyPhi = AvgPosPhi;
      }
    }
    
    if (AvgNegPhi != -999.0 && AvgPosPhi != -999.0) {
      if ( (AvgNegPhi > (-M_PI/2.0)) && (AvgPosPhi < (M_PI/2.0)) ) {
	energyPhi = (energyNegPhi + energyPosPhi)/(energy37Lay2NegPhi + energy37Lay2PosPhi);
      } else {
	if ((AvgNegPhi < (-M_PI/2.0)) && (AvgPosPhi > (M_PI/2.0))) {
	  energyPhi = (energyNegPhiConv + energyPosPhi)/(energy37Lay2NegPhi + energy37Lay2PosPhi);
	  if (energyPhi > M_PI) {
	    energyPhi = energyPhi - 2*M_PI;
	  }
	}
      }
    }

  } else {
    energyEta = 99. ;
    energyPhi = 0. ;
    m_error|=0x40000000;
  }

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {

    const LArCell* larcell = (*m_it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // Find position of current cell w.r.t. seed
    deta = fabs( etaCell - energyEta );
    dphi = fabs( phiCell - energyPhi );
    if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28

    double cellSizeEta;
    double cellSizePhi;
    if (emID->is_em_barrel( larcell->ID() ) ) {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
    } else {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
    }

    nCellsEta = 3;
    nCellsPhi = 7;
    if ( (deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01) &&
         (dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01)  ){

      if ( cluster_in_barrel ){
      totalEnergy += energyCell;
      //samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
      }
    } // End of do the 3x7 stuff

    nCellsEta = 5;
    nCellsPhi = 5;
    if ( (!cluster_in_barrel) && (deta <= 0.5*double(nCellsEta-1)*cellSizeEta+0.01) &&
         (dphi <= 0.5*double(nCellsPhi-1)*cellSizePhi+0.01)  ){
       totalEnergy += energyCell;
       //samp = CaloSampling::getSampling(*larcell);
       samp = larcell->caloDDE()->getSampling();
       rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
       rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
    } // End of do the 5*5 stuff

  }  // end of loop over sampling 2
	
  // calculate cluster width

  if ( energy35Lay2 > 0. ){ 
    const double inv_energy35Lay2 = 1. / energy35Lay2;
    clusterWidth35 = (weightEta2*inv_energy35Lay2) -
      (weightEta*inv_energy35Lay2)*(weightEta*inv_energy35Lay2);
    clusterWidth35 > 0.? clusterWidth35 = sqrt(clusterWidth35) :
      clusterWidth35 = 99.;				
  } else {
    clusterWidth35 = 99.;
  }
  
	// Finished Algorithmic time
//	m_timer[0]->propVal((int)floorf(energyEta*100));
	if (!m_timersvc.empty()) m_timer[3]->stop();      
	// Save EMShowerMinimal time
	if (!m_timersvc.empty()) m_timer[4]->start();      

  
  // Update cluster Variables

  // update stored variables
  rtrigEmCluster.setE233(energy33Lay2);
  rtrigEmCluster.setE237(energy37Lay2);
  rtrigEmCluster.setE277(energy77Lay2);
  rtrigEmCluster.setWeta2(clusterWidth35);
  rtrigEmCluster.setRawEnergy( rtrigEmCluster.rawEnergy() + totalEnergy);
  rtrigEmCluster.setEta(energyEta);
  rtrigEmCluster.setPhi(energyPhi);
  rtrigEmCluster.setRawEta(energyEta);
  rtrigEmCluster.setRawPhi(energyPhi);
  rtrigEmCluster.setNCells(ncells);
        
  // Finished save EMShowerMinimal time
  if (!m_timersvc.empty()) m_timer[4]->stop();      

#ifndef NDEBUG
  // This will internaly define normal, narrow and large clusters
  if ( msgLvl(MSG::DEBUG) ) {
    if ( m_geometryTool->EtaPhiRange(0,2,energyEta, energyPhi))
      ATH_MSG_ERROR( "problems with EtaPhiRange" );
    ATH_MSG_DEBUG( "totalEnergy" << totalEnergy );
    PrintCluster(totalEnergy,0,2, CaloSampling::EMB2,CaloSampling::EME2);
  }
#endif

  // Time total AlgTool time 
  if (!m_timersvc.empty()) m_timer[0]->stop();      

  return StatusCode::SUCCESS;
}
