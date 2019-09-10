/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClustersPufitMT.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Kenji Hamano
CREATED:  April 11, 2019

PURPOSE:  athenaMT migration

 ********************************************************************/
// Framework includes
#include "AthenaMonitoring/Monitored.h"
#include "GaudiKernel/IToolSvc.h"

#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

// TrigEFMissingET includes
#include "EFMissingETFromClustersPufitMT.h"
#include "EFMissingETComponentCopier.h"

#include <TMatrixD.h>

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromClustersPufitMT::EFMissingETFromClustersPufitMT(const std::string& type, 
							       const std::string& name, 
							       const IInterface* parent) :
  base_class(type, name, parent)
{
  // Determine number of phi & eta bins, and number of towers
  m_nphibins = (TMath::TwoPi()/m_towerwidthinput/2)*2;
  m_netabins = 2* m_etarange/m_towerwidthinput;
  m_ntowers = m_nphibins*m_netabins;

  //initialization to make coverity happy:
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
  
}


EFMissingETFromClustersPufitMT::~EFMissingETFromClustersPufitMT()
{
}


StatusCode EFMissingETFromClustersPufitMT::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFromClustersPufitMT::initialize()" );

  if(m_saveuncalibrated) {
    m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
  } else {
    m_clusterstate = xAOD::CaloCluster_v1::CALIBRATED;
  }

  CHECK( m_clustersKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersPufitMT::update(xAOD::TrigMissingET *met,
						  TrigEFMissingEtHelper *metHelper, 
						  const EventContext& ctx ) const
{

  ATH_MSG_DEBUG( "called EFMissingETFromClustersPufitMT::update()" );

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto caloClustersHandle = SG::makeHandle( m_clustersKey, ctx );

  auto loopTimer = Monitored::Timer( "TIME_Loop" );


  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = nullptr;
  metComp = metHelper->GetComponent(TrigEFMissingEtComponent::TCPufit); // fetch Cluster component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch TC PUFit component!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );


  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    return StatusCode::SUCCESS;
  }

  // set status to Processing
  metComp->m_status |= m_maskProcessing;
  

  // Variables
  double MExEta = 0, MEyEta = 0, MEzEta = 0, MExFull = 0, MEyFull = 0, MEzFull = 0, METEta = 0, MET = 0;

  double sumEtEta = 0, sumEtFull = 0, sumEEta = 0, sumEFull = 0;
  vector<double> ExTower(m_ntowers,0), EyTower(m_ntowers,0), EtTower(m_ntowers,0);
  vector<double> ExTower0(m_ntowers,0), EyTower0(m_ntowers,0), EtTower0(m_ntowers,0);
  vector<double> ExTower1(m_ntowers,0), EyTower1(m_ntowers,0), EtTower1(m_ntowers,0);
  vector<double> ExTower2(m_ntowers,0), EyTower2(m_ntowers,0), EtTower2(m_ntowers,0);
  vector<double> ExTower3(m_ntowers,0), EyTower3(m_ntowers,0), EtTower3(m_ntowers,0);
  vector<double> ExInMask, EyInMask, EtInMask, AreaInMask;

  // Calculate initial energy
  for (const auto& clus : *caloClustersHandle) {
      float phi = clus->phi(m_clusterstate), eta = clus->eta(m_clusterstate), Et  = clus->pt(m_clusterstate),
            E = clus->p4(m_clusterstate).E();
      float cosPhi, sinPhi; sincosf(phi, &sinPhi, &cosPhi);
      float Ex = Et*cosPhi, Ey = Et*sinPhi, Ez = Et*sinhf(eta);
      // calculate full granularity Ex, Ey
      MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
      // collect clusters into fixed-position towers    
      if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
        MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
       int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
           binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
           index0 = binEta*m_nphibins + binPhi;
       int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
           binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
           index1 = binEta1*m_nphibins + binPhi,
           index2 = binEta *m_nphibins + binPhi1,
           index3 = binEta1*m_nphibins + binPhi1;
       ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
       ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
       ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
       ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
      }
   } // end topo. loop.   

  int iTmax;
  double EtTowerTrimMean = 0, VarEtTower = 0, ptmin_final;
  

  // find median tower energy and calculate sample variance of tower energy using lower 1/2 of distribution
  EtTower = EtTower0;
  std::sort (EtTower.begin(), EtTower.end()); // sorts in ascending order
  int nTrimmed = m_ntowers*(1-m_trimfactor)/2;
  for (int k=nTrimmed; k<m_ntowers-nTrimmed; k++) EtTowerTrimMean += EtTower[k];
  EtTowerTrimMean *= 1.0/(m_ntowers-2*nTrimmed);
  for (int k=0; k<m_ntowers*(1.0+m_trimfactor)/2; k++) { // ignore highest towers
    double term = (EtTowerTrimMean - EtTower[k])*(EtTowerTrimMean - EtTower[k]);
    if (k < m_ntowers*(1.0-m_trimfactor)/2) term *= 2; // replace highest with lowest in variance
    VarEtTower += term;
  }
  VarEtTower *= 1.0/m_ntowers;
  // Set a minimum value to avoid FPEs
  if (VarEtTower < 1.0e6) VarEtTower = 1.0e6;
  ptmin_final = EtTowerTrimMean + m_nsigma*sqrt(VarEtTower);
  
  // find binning that gives largest sumEt from bins above threshold
  std::vector<double> EtMaxSumT(4,0);
  for(unsigned int k=0; k<ExTower0.size(); k++) {
    if ( EtTower0[k]  > ptmin_final ) { EtMaxSumT[0] += EtTower0[k]; }
    if ( EtTower1[k]  > ptmin_final ) { EtMaxSumT[1] += EtTower1[k]; }
    if ( EtTower2[k]  > ptmin_final ) { EtMaxSumT[2] += EtTower2[k]; }
    if ( EtTower3[k]  > ptmin_final ) { EtMaxSumT[3] += EtTower3[k]; }
  }
  std::vector<double>::iterator itmax;
  itmax = std::max_element(EtMaxSumT.begin(), EtMaxSumT.end());
  iTmax = std::distance(EtMaxSumT.begin(), itmax);
  
  // select Tower bins to correspond to iTmax
  if (iTmax == 0) {
    EtTower = EtTower0; ExTower = ExTower0; EyTower = EyTower0;
  } else if (iTmax == 1) {
    EtTower = EtTower1; ExTower = ExTower1; EyTower = EyTower1;
  } else if (iTmax == 2) {
    EtTower = EtTower2; ExTower = ExTower2; EyTower = EyTower2;
  } else if (iTmax == 3) {
    EtTower = EtTower3; ExTower = ExTower3; EyTower = EyTower3;
  }
  // Missing transverse energy from fixed position tower
  METEta = sqrt(MExEta*MExEta + MEyEta*MEyEta); MET = sqrt(MExFull*MExFull + MEyFull*MEyFull);

  // Perform pile-up fit and subtract contributions
  // --------------------------------------------------------------------------------------
  if (m_subtractpileup) {

   for(unsigned int k=0; k<ExTower.size(); k++) {
     if ( EtTower[k] > ptmin_final ) {
       ExInMask.push_back(ExTower[k]); EyInMask.push_back(EyTower[k]); EtInMask.push_back(EtTower[k]);
       AreaInMask.push_back(2*m_etarange/m_netabins*TMath::TwoPi()/m_nphibins);
     }
   }

   int nummasks = ExInMask.size();
   if(nummasks > 0) {
    // Form sumEtobs and covEtobs from all towers
    double sumEtobs = 0, varsumEtobs = 0;
    TMatrixD Etobs(2,1), covEtobs(2,2);
    for(unsigned int j = 0; j < ExTower.size(); j++) {
      double E1 = sqrt(ExTower[j]*ExTower[j]+EyTower[j]*EyTower[j]);
      if (E1 < 1.0) continue;
      sumEtobs += E1;
      double E1res2 = m_resefloor*m_resefloor + m_rese*m_rese*E1;
      varsumEtobs += E1res2;
      double cosphi1 = ExTower[j]/E1, sinphi1 = EyTower[j]/E1;
      Etobs[0][0] += E1*cosphi1; Etobs[1][0] += E1*sinphi1;
      covEtobs[0][0] += E1res2*cosphi1*cosphi1;
      covEtobs[1][0] += E1res2*cosphi1*sinphi1;
      covEtobs[1][1] += E1res2*sinphi1*sinphi1;
    }
    covEtobs[0][1] = covEtobs[1][0];
    // record masks, remove towers that are masked from observed quantities
    TMatrixD Etmasked(2,1);
    double arealost = 0;
    for(int k = 0; k < nummasks; k++) {
      arealost += AreaInMask[k];
      double E1 = sqrt(ExInMask[k]*ExInMask[k]+EyInMask[k]*EyInMask[k]);
      double E1res2 = m_resefloor*m_resefloor + m_rese*m_rese*E1;
      double cosphi1 = ExInMask[k]/E1, sinphi1 = EyInMask[k]/E1;
      Etmasked[0][0] += E1*cosphi1;  Etmasked[1][0] += E1*sinphi1;
      sumEtobs -= E1; varsumEtobs -= E1res2;
      Etobs[0][0] -= E1*cosphi1;  Etobs[1][0] -= E1*sinphi1;
      covEtobs[0][0] -= E1res2*cosphi1*cosphi1;
      covEtobs[1][0] -= E1res2*sinphi1*cosphi1;
      covEtobs[1][1] -= E1res2*sinphi1*sinphi1;
    }  
    covEtobs[0][1] = covEtobs[1][0];
    TMatrixD covEtobsinv(covEtobs); covEtobsinv.Invert();
    
    TMatrixD dXdEab(nummasks,nummasks), dXdEa(nummasks,1);
    vector<double> varRhoA;
    for (int k1 = 0; k1<nummasks; k1++) {
      varRhoA.push_back(VarEtTower * m_varrhoscale);
      double ET1inv = 1/EtInMask[k1];
      double cosphi1 = ExInMask[k1]*ET1inv;
      double sinphi1 = EyInMask[k1]*ET1inv;
      dXdEa[k1][0] = -(							\
        Etobs[0][0]*(covEtobsinv[0][0]*cosphi1+covEtobsinv[1][0]*sinphi1) + \
        Etobs[1][0]*(covEtobsinv[0][1]*cosphi1+covEtobsinv[1][1]*sinphi1) - \
        EtTowerTrimMean/varRhoA[k1] );
      for (int k2 = 0; k2<nummasks; k2++) {
      	double ET2inv = 1/EtInMask[k2];
      	double cosphi2 = ExInMask[k2]*ET2inv;
      	double sinphi2 = EyInMask[k2]*ET2inv;
      	dXdEab[k1][k2] = (						\
  			  cosphi1*(covEtobsinv[0][0]*cosphi2+covEtobsinv[1][0]*sinphi2) + \
  			  sinphi1*(covEtobsinv[0][1]*cosphi2+covEtobsinv[1][1]*sinphi2) );
      	if (k1 == k2) dXdEab[k1][k2] += 1/varRhoA[k1];
      }
    }
    TMatrixD covFit(dXdEab); covFit.Invert(); TMatrixD Evals(covFit*dXdEa);
    TMatrixD ETobscor(Etmasked), ETfitcor(Etmasked);
    for (int k=0; k<nummasks; k++) {
      double cosphi1 = ExInMask[k]/EtInMask[k], sinphi1 = EyInMask[k]/EtInMask[k];
      ETfitcor[0][0] -= Evals[k][0]*cosphi1;
      ETfitcor[1][0] -= Evals[k][0]*sinphi1;
    }
    double METfitcor = sqrt(ETfitcor[0][0]*ETfitcor[0][0]+ETfitcor[1][0]*ETfitcor[1][0]);
    
    ATH_MSG_DEBUG( " METEta = " << METEta << "\t METfitcor = " << METfitcor << "\t MET= " << MET );
    
    ETobscor[0][0]=MExEta; ETobscor[1][0]=MEyEta;
    
    metComp->m_ex = -(float) ETfitcor[0][0];
    metComp->m_ey = -(float) ETfitcor[1][0];  
    // For the rest we add in the default quantities 
    metComp->m_ez = -MEzEta;
    metComp->m_sumEt = sumEtEta;
    metComp->m_sumE  = sumEEta;
    metComp->m_usedChannels += 1;
  
    metComp = metHelper->GetComponent(TrigEFMissingEtComponent::TCPufitUnc); // fetch first auxiliary component to store uncorrected MET
    
    metComp->m_ex = -(float) ETobscor[0][0];
    metComp->m_ey = -(float) ETobscor[1][0];  
    // For the rest we add in the default quantities 
    metComp->m_ez = -MEzEta;
    metComp->m_sumEt = sumEtEta;
    metComp->m_sumE  = sumEEta;
    metComp->m_usedChannels += 1;     
    
   } else {
     
     // Just store zero energies for the clusters 
     metComp->m_ex = 0.;
     metComp->m_ey = 0.;
     metComp->m_ez = 0.;
     metComp->m_sumEt = 0.;
     metComp->m_sumE  = 0.;
     metComp->m_usedChannels += 1;
       
     metComp = metHelper->GetComponent(TrigEFMissingEtComponent::TCPufitUnc); // fetch first auxiliary component to store uncorrected MET
     
     metComp->m_ex = -MExEta;
     metComp->m_ey = -MEyEta;
     metComp->m_ez = -MEzEta;
     metComp->m_sumEt = sumEtEta;
     metComp->m_sumE  = sumEEta;
     metComp->m_usedChannels += 1;
     
   }
   
   // --------------------------------------------------------------------------------------
   
   
   // move from "processing" to "processed" state
   metComp->m_status ^= m_maskProcessing; // switch off bit
   metComp->m_status |= m_maskProcessed;  // switch on bit
   
  } // end container loop.
  


  // Save MET into final met object
  EFMissingETComponentCopier copier = EFMissingETComponentCopier(met, metHelper);
  const std::vector<std::string> vComp = {"TCPufit", "TCPufitUnc"};
  met->defineComponents( vComp );
  copier.addHelperCompToMET(TrigEFMissingEtComponent::TCPufit);
  copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::TCPufit);
  copier.setMETCompFromHelper(1, TrigEFMissingEtComponent::TCPufitUnc);
  
  return StatusCode::SUCCESS;

}
