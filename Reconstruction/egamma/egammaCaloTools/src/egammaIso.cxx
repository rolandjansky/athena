/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaIso.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "FourMomUtils/P4Helpers.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath> 

using CLHEP::MeV;


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
egammaIso::egammaIso(const std::string& type,
		     const std::string& name,
		     const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare Interface
  declareInterface<IegammaIso>(this);
  //
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");

}

// ====================================================================
// DESTRUCTOR:
egammaIso::~egammaIso()
{ 
}

// ====================================================================
// INITIALIZE METHOD:
StatusCode egammaIso::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaIso");

  // ---- retrieve the noisetool ----------------
  if(m_noiseTool.retrieve().isFailure()){
    ATH_MSG_WARNING("Unable to find tool for CaloNoise ");
  }    else {
    ATH_MSG_DEBUG("Noise Tool retrieved");
  }
  
  return StatusCode::SUCCESS;
}

// ====================================================================
// FINALIZE METHOD:
StatusCode egammaIso::finalize()
{
  return StatusCode::SUCCESS;
}

// ===============================================================
StatusCode egammaIso::execute(const xAOD::CaloCluster *cluster, 
			      CaloCellList* had, CaloCellList* em, 
			      double R, int neta, int nphi)
{
  
  StatusCode sc = execute( cluster, had);
  if( !sc.isSuccess()){
    ATH_MSG_WARNING(" egammaIso not successful"); 
    return StatusCode::RECOVERABLE;  
  }


  m_etcone.clear();
  m_etcone.push_back(0);
  m_EMCellList = em;

  // all et in a cone 
  CalcEtCone(R,neta,nphi);
  
  return StatusCode::SUCCESS;
}

// NOTE:: First cone has to be the largest
// ===============================================================
StatusCode egammaIso::execute(const xAOD::CaloCluster *cluster, 
			      CaloCellList* had, CaloCellList* em, 
			      const std::vector<double>& Rs, int neta, int nphi)
{

  StatusCode sc = execute( cluster, had);
  if( !sc.isSuccess()){
    ATH_MSG_WARNING(" egammaIso not successful"); 
    return StatusCode::RECOVERABLE;  
  }

  const double eta  = m_cluster->eta(); 
  const double phi  = m_cluster->phi(); 
 
  m_EMCellList = em;

  if (Rs.empty()) {
    ATH_MSG_DEBUG("Rs are empty");
    return StatusCode::SUCCESS;
  }

  // calculate the rectangle that needs to be subtracted
  const double deta = 0.0125*neta; 
  const double dphi = M_PI/256.*nphi;   
  double recEt = 0;

  if (m_EMCellList) {
    m_EMCellList->select(eta,phi,deta,dphi);       
    recEt = m_EMCellList->et();
  }

  // Exclude tile scintillator from the surrounding cone.
  // FIXME: Window size hard-coded to match that in CaloSwGap_v2.
  if (m_HADCellList) {
    m_HADCellList->select(eta,phi,0.2,2*M_PI/64,CaloSampling::TileGap3);
    recEt += m_HADCellList->et();
  }

  m_etcone.clear();
  m_etcone.resize(Rs.size(), - recEt);

  std::vector<double> Rssq;
  for (std::vector<double>::const_iterator rit = Rs.begin();
       rit != Rs.end();
       ++rit) {
    Rssq.push_back(*rit * *rit);
  }

  // Let's determine some values based on the input specs
  double Rmax = 0;
  for (std::vector<double>::const_iterator it = Rs.begin();
       it != Rs.end();
       ++it) {
    if (*it > Rmax) {
      Rmax = *it;
    }
  }
    
  if (m_EMCellList) {
    // get the cells for the first one; by convention, it must
    // be bigger than all the other cones.
    m_EMCellList->select(eta,phi,Rmax);       
    
    for (CaloCellList::list_iterator it = m_EMCellList->begin();
	 it != m_EMCellList->end();
	 ++it) {
      
      const double etacel=(*it)->eta() ;
      const double phicel=(*it)->phi() ;
      
      const double deleta = eta-etacel;
      const double delphi=P4Helpers::deltaPhi(phi, phicel);
      const double drcel2=deleta*deleta+delphi*delphi;
      
      for (unsigned int i = 0; i < Rssq.size(); i++) {
	if (drcel2 < Rssq.at(i)) {
	  m_etcone[i] += (*it)->et();
	}
      }
    }
  }


  if (m_HADCellList) {
    // get the cells for the first one; by convention, it must
    // be bigger than all the other cones.
    m_HADCellList->select(eta,phi,Rmax);       
    
    for (CaloCellList::list_iterator it = m_HADCellList->begin();
	 it != m_HADCellList->end();
	 ++it) {
      
      const double etacel=(*it)->eta() ;
      const double phicel=(*it)->phi() ;
      
      const double deleta = eta-etacel;
      const double delphi=P4Helpers::deltaPhi(phi, phicel);
      const double drcel2=deleta*deleta+delphi*delphi;
      
      for (unsigned int i = 0; i < Rssq.size(); i++) {
	if (drcel2 < Rssq.at(i)) {
	  m_etcone[i] += (*it)->et();
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

//C.A
//Method to calculate just the hadronic leakage
//should be a copy of the relevant lines from the method above
// ===============================================================
StatusCode egammaIso::execute(const xAOD::CaloCluster *cluster, CaloCellList* had)
{
  m_ethad  = 0;
  m_ethad1 = 0;
  m_ehad1  = 0;

  m_cluster = cluster;
  if(!m_cluster) { 
    ATH_MSG_WARNING(" egammaIso: Invalid pointer to cluster"); 
    return StatusCode::RECOVERABLE;
  }
  m_HADCellList = had;
  
  const double eta  = m_cluster->eta(); 
  const double phi  = m_cluster->phi(); 
  double egap=0.;
  double ehad=0.;
  const double size = 0.12;
  
  // increase window size a bit to avoid problems due to 4cm shift.
  // considering that in the tile the granularity is 0.1, the value
  // choosen below is safe.
  // all hadron in 0.2X0.2
  if (m_HADCellList) {
    m_HADCellList->select(eta,phi,size,size);       
    ehad +=m_HADCellList->energy(); 
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap3);       
    egap +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::HEC0);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileBar0);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    // Fix had leakage in crack (TileGap1 and TileGap2 missing before 14.2)
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap1);
    m_ehad1 +=m_HADCellList->energy();
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileGap2);       
    m_ehad1 +=m_HADCellList->energy(); 
    
    m_HADCellList->select(eta,phi,size,size,CaloSampling::TileExt0);       
    m_ehad1 +=m_HADCellList->energy(); 
  }

  const double eta2 = m_cluster->etaBE(2);
  if (eta2==-999.) {
    m_ethad1 = -999;
    m_ethad = -999;
  } else {
    const double cheta2 = cosh(eta2);
    m_ethad1 = cheta2!=0. ? m_ehad1/cheta2 : 0.; 
    // substract scintillator energy from ehad! 
    m_ethad = cheta2!=0. ? (ehad-egap)/cheta2 : 0.;
  }

  return StatusCode::SUCCESS;
}


// =======================================================
void egammaIso::CalcEtCone(double R, int neta, int nphi) 
{
  // ====================================================
  // all energy in a cone - in neta X nphi cells
  // ====================================================
  m_etcone.clear();
  m_etcone.push_back(0);

  // protection against absence of cluster
  if(!m_cluster) return;

  // half size of the rectangular.
  const double deta = 0.0125*neta; 
  const double dphi = M_PI/256.*nphi; 
  
  const double eta= m_cluster->eta(); 
  const double phi= m_cluster->phi(); 
  
  // hadronic side
  if (m_HADCellList) {
    m_HADCellList->select(eta,phi,R);       
    m_etcone[0] +=m_HADCellList->et(); 
  }

  // electromagnetic side 
  if (m_EMCellList) {
    m_EMCellList->select(eta,phi,R);       
    m_etcone[0] +=m_EMCellList->et(); 
    // -et in neta X nphi cells.
    m_EMCellList->select(eta,phi,deta,dphi);       
    m_etcone[0] -=m_EMCellList->et(); 
  }

  // Exclude tile scintillator from the surrounding cone.
  // FIXME: Window size hard-coded to match that in CaloSwGap_v2.
  if (m_HADCellList) {
    m_HADCellList->select(eta,phi,0.2,2*M_PI/64,CaloSampling::TileGap3);
    m_etcone[0] -= m_HADCellList->et();
  }

}

// =======================================================
void egammaIso::CalcEtRingNoise(double R1, double R2,int flagCell, int flagNoise, int NsigNoise ) 
{
  // ====================================================
  // all energy in a ring : (R1<DR<R2) with energy > Nsigma above noise 
  // flagCell  = 1 EM, 2 EM+HAD
  // flagNoise = 1 elec, 2 total
  // NsigNoise = Nbumber of sigma above noise
  // ====================================================
  m_etcone.clear();
  m_etcone.push_back(0);

  const double R1sq = R1*R1;
  const double R2sq = R2*R2;

  // protection against absence of cluster
  if(!m_cluster) return; 

  const double eta= m_cluster->eta(); 
  const double phi= m_cluster->phi(); 
  
  
  // flagCell==1 --> EMccl seul
  // flagCell==2 --> EMccl + HADccl
  if (flagCell==1|| flagCell==2) {
    m_EMCellList->select(eta,phi,R2);       
    CaloCellList::list_iterator begloop=m_EMCellList->begin();
    CaloCellList::list_iterator endloop=m_EMCellList->end();
    for (; begloop != endloop; ++begloop) {
      const double etacel=(*begloop)->eta() ;
      const double phicel=(*begloop)->phi() ;
      
      const double deta = eta-etacel;
      const double dphi=P4Helpers::deltaPhi(phi, phicel);
      const double drcel2=deta*deta+dphi*dphi;
      
      double noisecel1=-10000000;
      if (flagNoise==1) {
	noisecel1=m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
      }
      else if (flagNoise==2) {
	noisecel1=m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
      }
      
      const double ecel=(*begloop)->e();
      
      if (R2sq>drcel2 && drcel2>R1sq  && ecel> NsigNoise*noisecel1) {
	m_etcone[0]+=(*begloop)->et();
      }
    }
    
    if (flagCell==2) {
      m_HADCellList->select(eta,phi,R2);       
      CaloCellList::list_iterator begloopH=m_HADCellList->begin();
      CaloCellList::list_iterator endloopH=m_HADCellList->end();
      for (; begloopH != endloopH; ++begloopH) {
	const double etacel=(*begloopH)->eta() ;
	const double phicel=(*begloopH)->phi() ;
	 
	const double deta = eta-etacel;
	const double dphi = P4Helpers::deltaPhi(phi, phicel);
	const double drcel2 = deta*deta+dphi*dphi;
	 
	double noisecel1=-10000000;
	if (flagNoise==1) {
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
	}
	else if (flagNoise==2) {
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
	}
	const double ecel=(*begloopH)->e();
	 
	if (R2sq>drcel2 && drcel2 > R1sq && ecel> NsigNoise*noisecel1) {
	  m_etcone[0]+=(*begloopH)->et();
	}
      }
    }
  } 
}

// ====================================================================
void egammaIso::CalcEtConeClusterNoise(double R2, int NsigNoise, int flagCell, int flagNoise, bool symmetric, int neta, int nphi) 
{
  // ====================================================
  // all energy in a cone : (DR<R2) with energy > Nsigma above noise 
  // and cluster subtracted
  // NsigNoise = Number of sigma above noise, if -1 no cut on noise
  // flagCell  = 1 EM, 2 EM+HAD
  // flagNoise = 1 elec, 2 total
  // symmetric : symmetric cut is a cut on abs(e)>meannoise instead of e>meannoise
  // neta, nphi = size of the cluster to subtract
  // ====================================================

  double resEtconeEM=0., resEtconeHAD=0.;

  // half size of the rectangular.
  const double deta = 0.0125*neta; 
  const double dphi = M_PI/256.*nphi; 

  const double R2sq = R2 * R2;

  // protection against bad clusters
  if (!m_cluster) return;

  const double eta= m_cluster->eta(); 
  const double phi= m_cluster->phi(); 

  // flagCell==1 --> EMccl alone
  // flagCell==2 --> EMccl + HADccl
  if (flagCell==1|| flagCell==2) {
    double countedWholeConeEM=0.;

    m_EMCellList->select(eta,phi,R2);       
    CaloCellList::list_iterator begloop=m_EMCellList->begin();
    CaloCellList::list_iterator endloop=m_EMCellList->end();
  
    for (; begloop != endloop; ++begloop) { //We loop over the cells in EM
      //Position of the cell
      const double etacel=(*begloop)->eta() ;
      const double phicel=(*begloop)->phi() ;
      
      //We retrieve the mean noise for the cell     
      double noisecel1=-10000000;
      if (flagNoise==1) { //Retrieving the electronig noise
	noisecel1=m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
      }
      else if (flagNoise==2) { //Retrieving the total noise
	noisecel1 = m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
      }
      
      //Computing the deltaR with the cluster position
      const double deta = eta-etacel;
      const double dphi = P4Helpers::deltaPhi(phi, phicel);
      const double drcel2 = deta*deta+dphi*dphi;
      if (drcel2<R2sq) { //Selecting only the cells in the cone of size R2
	const double ecel=(*begloop)->e(); //Energy of the cell
	
	//Depending on "symmetric", we compare fabs(energy_cell) or energy_cell with the mean noise for the cell (if NsigNoise==-1, we don´t cut out the cell in any case)
	if ( ((symmetric)?fabs(ecel):ecel)>(NsigNoise*noisecel1) || NsigNoise==-1) 
	  countedWholeConeEM +=(*begloop)->et()/MeV; //If the cell is selected, we add its transverse energy to resEtconeEM
      }
    }
    
    resEtconeEM = countedWholeConeEM;
    
    //Subtraction of the cluster et (only for the cells which were added)
    m_EMCellList->select(eta,phi,deta,dphi); //Selecting cells inside the cluster neta x nphi
    double countedInsideClusterEM = 0;
    begloop=m_EMCellList->begin();
    endloop=m_EMCellList->end();
    for (; begloop != endloop; ++begloop) { //Looping over the cells inside the cluster neta x nphi
      double noisecel1=-10000000;
      if (flagNoise==1) { //Retrieving the electronig noise
	noisecel1=m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
      }
      else if (flagNoise==2) { //Retrieving the total noise
	noisecel1 = m_noiseTool->getNoise(*begloop,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
      }
      const double ecel=(*begloop)->e(); //Energy of the cell
      //Depending on "symmetric", we compare fabs(energy_cell) or energy_cell with the mean noise for the cell (if NsigNoise==-1, we don´t cut out the cell in any case)
      if (((symmetric)?fabs(ecel):ecel)>(NsigNoise*noisecel1) || NsigNoise==-1) 
	countedInsideClusterEM += (*begloop)->et()/MeV; //If the cell is selected, we count its transverse energy for removal
    }
    resEtconeEM -= countedInsideClusterEM; //We remove the transverse energy counted inside the cluster from the one counted in the whole cone
    
    
    if (flagCell==2) { //If asked, we loop over the cells in HAD
      m_HADCellList->select(eta,phi,R2);       
      CaloCellList::list_iterator begloopH=m_HADCellList->begin();
      CaloCellList::list_iterator endloopH=m_HADCellList->end();

      for (; begloopH != endloopH; ++begloopH) {
	//Position of the cell
	const double etacel=(*begloopH)->eta() ;
	const double phicel=(*begloopH)->phi() ;
	
	//We retrieve the mean noise for the cell     
	double noisecel1=-10000000;
	if (flagNoise==1) { //Retrieving the electronig noise
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
	}
	else if (flagNoise==2) { //Retrieving the total noise
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
	}
	
	//Computing the deltaR with the cluster position
	const double deta = eta-etacel;
	const double dphi = P4Helpers::deltaPhi(phi, phicel);
	const double drcel2 = deta*deta+dphi*dphi;
	if (drcel2<R2sq) { //Selecting only the cells in the cone of size R2
	  const double ecel=(*begloopH)->e();
	  //Depending on "symmetric", we compare fabs(energy_cell) or energy_cell with the mean noise for the cell (if NsigNoise==-1, we don´t cut out the cell in any case)
	  if (((symmetric)?fabs(ecel):ecel)>(NsigNoise*noisecel1) || NsigNoise==-1)
	    resEtconeHAD+=(*begloopH)->et()/MeV; //If the cell is selected, we add its transverse energy to resEtconeHAD
	}
      }
      
      // Exclude tile scintillator from the surrounding cone if it had been counted
      m_HADCellList->select(eta,phi,0.2,2*M_PI/64,CaloSampling::TileGap3);
      begloopH=m_HADCellList->begin();
      endloopH=m_HADCellList->end();
      for (; begloopH != endloopH; ++begloopH) {
	//We retrieve the mean noise for the cell     
	double noisecel1=-10000000;
	if (flagNoise==1) { //Retrieving the electronig noise
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);
	}
	else if (flagNoise==2) { //Retrieving the total noise
	  noisecel1=m_noiseTool->getNoise(*begloopH,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);
	}
	double ecel=(*begloopH)->e();
	//Depending on "symmetric", we compare fabs(energy_cell) or energy_cell with the mean noise for the cell (if NsigNoise==-1, we don´t cut out the cell in any case)
	if (((symmetric)?fabs(ecel):ecel)>(NsigNoise*noisecel1) || NsigNoise==-1)
	  resEtconeHAD-=(*begloopH)->et()/MeV; //If the cell had been selected and counted, we remove its transverse energy from resEtconeHAD
      }
      
    }
  }

  //Finally the result value is the sum of the EM and HAD components
  m_etcone.clear();
  m_etcone.push_back(resEtconeEM + resEtconeHAD);
}



