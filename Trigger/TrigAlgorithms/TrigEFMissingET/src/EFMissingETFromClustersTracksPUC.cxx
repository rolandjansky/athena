/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClustersTracksPUC.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rui Zou, Ben Carlson, Jonathan Burr
CREATED:  Feb 01, 2018

PURPOSE:  Pile-up fit + tracks

********************************************************************/

#include "TrigEFMissingET/EFMissingETFromClustersTracksPUC.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include <TMatrixD.h>
#include <string>

using namespace std;
namespace {
  // Helper class
  struct Tower{
    xAOD::CaloCluster::State m_clusterstate;
    float sumEt{0.};
    float px{0.};
    float py{0.};
    float pt() { return sqrt(px*px+py*py); }
    float cosPhi() { return pt() > 0 ? px/pt() : 1.; }
    float sinPhi() { return pt() > 0 ? py/pt() : 0.; }
    Tower& operator=(bool m_saveuncalibrated) {
      if(m_saveuncalibrated) m_clusterstate = xAOD::CaloCluster::UNCALIBRATED;
      else m_clusterstate = xAOD::CaloCluster::CALIBRATED;
      return *this;
    }
    Tower& operator+=(const xAOD::CaloCluster& clus) {
      float sinPhi;
      float cosPhi;
      sincosf(clus.phi(m_clusterstate), &sinPhi, &cosPhi);
      float Et = clus.pt(m_clusterstate);
      sumEt += Et;
      px += Et * cosPhi;
      py += Et * sinPhi;
      return *this;
    }
  };
}


EFMissingETFromClustersTracksPUC::EFMissingETFromClustersTracksPUC(const std::string& type, 
    const std::string& name, 
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent),
  m_trackselTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this )
{
  // declare the algorithm configurables here
  // declareProperty("PropertyName", m_property=default_value, "Property Description");
  // (*Do* add a property description...)
  declareProperty("SaveUncalibrated", m_saveuncalibrated = false ,"save uncalibrated topo. clusters");
  declareProperty("TargetTowerWidth", m_targetTowerWidth = 0.7, "The target width of the towers");
  declareProperty("MaxEta", m_maxEta = 5., "The maximum eta");
  declareProperty("ForwardpTCut", m_forward_ptcut = 50.0e3 ,"pT Cut for forward jets");
  declareProperty("TrackpTCut", m_track_ptcut = 0.0 ,"pT Cut for online tracks");
  declareProperty("dRCut", m_dRCut = 0.4, "dR Cut for track - jet association");
  declareProperty("MinJetPtJvt", m_minJetPtJvt = 20.e3, "The minimum pT (in MeV) for jets to consider for JVT");
  declareProperty("MaxJetPtJvt", m_maxJetPtJvt = 60.e3, "The maximum pT (in MeV) for jets to consider for JVT (above this jets automatically pass");
  declareProperty("JetRpTCut", m_jetRpTCut = 0.1, "The JVT RpT cut to apply to jets");//0.1, "The JVT RpT cut to apply to jets");
  declareProperty("CaloResSqrtTerm", m_caloResSqrtTerm = 15.81, "The coefficient of the sqrt term in the calorimeter resolution (in MeV)");
  declareProperty("CaloResFloor", m_caloResFloor = 50, "Floor for calorimeter resolution (in MeV)");
  declareProperty("ConstraintWeight", m_constraintWeight = 1., "The relative weighting applied to the constraints");

  // declare configurables

  m_fextype = FexType::TOPO;
  m_methelperposition = 3;

  // Determine number of phi & eta bins, and number of towers
  m_nPhiBins = TMath::TwoPi() / m_targetTowerWidth;
  m_nEtaBins = 2 * m_maxEta / m_targetTowerWidth;
  m_nTowers = m_nPhiBins * m_nEtaBins;

  m_towerEtaWidth = 2 * m_maxEta / m_nEtaBins;
  m_towerPhiWidth = TMath::TwoPi() / m_nPhiBins;

  m_clusterstate = xAOD::CaloCluster::UNCALIBRATED;
}


EFMissingETFromClustersTracksPUC::~EFMissingETFromClustersTracksPUC()
{
}


StatusCode EFMissingETFromClustersTracksPUC::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::initialize()" );

  // Standard initialize step

  // Set up the timer service.
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  if(m_saveuncalibrated) m_clusterstate = xAOD::CaloCluster::UNCALIBRATED;
  else m_clusterstate = xAOD::CaloCluster::CALIBRATED;

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersTracksPUC::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromClustersTracksPUC::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::finalize()" );

  return StatusCode::SUCCESS;
  
}

StatusCode EFMissingETFromClustersTracksPUC::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * jetContainer,
    const xAOD::TrackParticleContainer * trackContainer, const xAOD::VertexContainer * vertexContainer,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{
  // The function signature has to be compatible with all of the algorithms used in the package
  // This is unfortunate and liable to change in the future (certainly for athenaMT) but until then just be aware that that's how it goes
  // The main algorithm works out what types the various trigger elements are that have been given to it and then passes them onto these functions

  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::execute()" );

  if(m_timersvc)m_glob_timer->start(); // total time

  // The met Helper is used to create the various components of the output trigger MET
  // Again, this is an old design and could be updated but probably won't be until athenaMT
  // You need to write the met into a position in the helper (and a later tool then compiles this into a met value)
  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = nullptr;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name).substr(0,2)!="TC"){
    ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Clusters component!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );

  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  }
  else { // skip if processed
    if(m_timersvc) m_glob_timer->stop(); // total time
    return StatusCode::SUCCESS;
  }

  // The mask statuses are involved in making sure that you don't try and run the same calculation multiple times
  // To be honest I *thought* this was handled by the steering so it probably is something we should check...
  // set status to Processing
  metComp->m_status |= m_maskProcessing;

  std::vector<const xAOD::Jet*> JetsVec(jetContainer->begin(), jetContainer->end());
  ATH_MSG_DEBUG( "num of jets: " << JetsVec.size() );

  std::vector<const xAOD::TrackParticle*> TracksVec(trackContainer->begin(), trackContainer->end());
  ATH_MSG_DEBUG( "num of tracks: " << TracksVec.size() );

  std::vector<const xAOD::Vertex*> VertexVec(vertexContainer->begin(), vertexContainer->end());
  ATH_MSG_DEBUG( "num of vertices: " << VertexVec.size() );

  std::vector<const xAOD::CaloCluster*> clusters(caloCluster->begin(), caloCluster->end());

  std::vector<const xAOD::Jet*> hardScatterJets(0);
  std::vector<const xAOD::Jet*> pileupJets(0);
  
  const xAOD::Vertex* primaryVertex =  nullptr;
  for (const xAOD::Vertex* vertex : VertexVec) {

    ATH_MSG_DEBUG( "\tx: " << vertex->x() << "\ty: " << vertex->y() << "\tz: " << vertex->z()
                   << "\tntracks: " <<  vertex->nTrackParticles()
                   << "\ttype: " << vertex->vertexType()
                   << "\txAOD::VxType::PriVtx: " << xAOD::VxType::PriVtx );

    // ntracks is not working now in rel21
    // if ( vertex->nTrackParticles() < 2 ) continue;
    if ( vertex->vertexType() == xAOD::VxType::PriVtx ) {
      primaryVertex = vertex;
      break;
    }
  }

  std::vector<double> ptsum_pvsVec(JetsVec.size(), 0.0);
  for (const xAOD::TrackParticle* itrk : TracksVec) {
    bool accept = (itrk->pt()> m_track_ptcut && m_trackselTool->accept(*itrk, primaryVertex));
    if (!accept) continue;
    int whichjet = 0;
    double minkT = -1;
    for (uint i=0; i < JetsVec.size(); i++) {
      const xAOD::Jet* jet = JetsVec.at(i);
      if (primaryVertex) {
	if (fabs(jet->eta() ) < 2.4) {
	  double dphi = fabs(jet->phi()-itrk->phi());
	  if (dphi > M_PI) dphi = 2*M_PI - dphi;
	  double dR = sqrt((jet->rapidity()-itrk->rapidity())*(jet->rapidity()-itrk->rapidity()) + dphi * dphi);
	  if (dR > m_dRCut) continue;
	  double kT = dR/jet->pt();
	  if (kT < minkT || minkT < 0) {
	    minkT = kT;
	    whichjet = i;
	  }
	}
      }
      if (minkT >= 0) ptsum_pvsVec.at(whichjet) += itrk->pt();
	
    }
  } 

  for (uint i=0; i < JetsVec.size(); i++) {
    const xAOD::Jet* jet = JetsVec.at(i);
    if (primaryVertex) {
      if (fabs(jet->eta() ) < 2.4) {
	double RpT = ptsum_pvsVec.at(i)/jet->pt();
	if (jet->pt() > m_minJetPtJvt && (RpT > m_jetRpTCut || jet->pt() > m_maxJetPtJvt)){
	  hardScatterJets.push_back(jet);
	} else {
	  pileupJets.push_back(jet);
	}
      } else if (jet->pt() >= m_forward_ptcut) {
	hardScatterJets.push_back(jet);
      } else {
	pileupJets.push_back(jet);
      }
    } else {
      pileupJets.push_back(jet);
    }
  }  

  if (hardScatterJets.size() == 0) return StatusCode::SUCCESS;

  std::vector<const xAOD::CaloCluster*> pileupClusters(0);
  std::vector<const xAOD::CaloCluster*> hardScatterClusters(0);
  std::set<unsigned int> hardScatterIndices; // The indices of clusters associated with pileup jets

  // Assume all of our jets have the same radius - the rest of the algorithm basically
  // relies on this anyway.
  float jetRadiusParameter = hardScatterJets.front()->getSizeParameter();
  
  for (const xAOD::Jet* ijet : hardScatterJets)
    for (const auto& link : ijet->constituentLinks() )
      hardScatterIndices.insert(link.index() );
  for (unsigned int ii = 0; ii < clusters.size(); ++ii) {
    if (fabs(clusters.at(ii)->eta(m_clusterstate) ) > m_maxEta)
      continue;
    if (hardScatterIndices.count(ii) ){
      hardScatterClusters.push_back(clusters.at(ii) );
    } else {
      pileupClusters.push_back(clusters.at(ii) );
    }
  }

  // Calculate covariance of pileup deposits, collect into towers and calculate pileup 2-vector
  float sigma_xx = 0.;
  float sigma_yy = 0.;
  float sigma_xy = 0.;
  std::vector<Tower> pileupTowers(m_nTowers);
  TVector2 ptPileup;
  for (const xAOD::CaloCluster* iclus : pileupClusters) {
    float sigma2 = m_caloResFloor*m_caloResFloor + m_caloResSqrtTerm*m_caloResSqrtTerm*iclus->pt(m_clusterstate);
    float sinPhi;
    float cosPhi;
    sincosf(iclus->phi(m_clusterstate), &sinPhi, &cosPhi);
    sigma_xx += sigma2 * cosPhi*cosPhi;
    sigma_yy += sigma2 * sinPhi*sinPhi;
    sigma_xy += sigma2 * cosPhi*sinPhi;
    unsigned int etaBin = (iclus->eta(m_clusterstate) + m_maxEta)/(2*m_maxEta) * m_nEtaBins;
    unsigned int phiBin = fmod(iclus->phi(m_clusterstate)/TMath::TwoPi() + 1, 1) * m_nPhiBins;
    pileupTowers.at(etaBin*m_nPhiBins + phiBin) = m_clusterstate;
    pileupTowers.at(etaBin*m_nPhiBins + phiBin) += *iclus;
    ptPileup += TVector2(cosPhi, sinPhi) * iclus->pt(m_clusterstate);
  }
  float sigma_det = sigma_yy*sigma_xx - sigma_xy*sigma_xy;
  double cosPhiPileup = ptPileup.Px()/ptPileup.Mod();
  double sinPhiPileup = ptPileup.Py()/ptPileup.Mod();

  float sumEtPileup = 0.;
  float sumEt2Pileup = 0.;
  unsigned int nNonOverlappingPileupTowers = 0;
  // Now calculate the variance of the towers that don't overlap with jets
  for (unsigned int ii = 0; ii < pileupTowers.size(); ++ii) {
    Tower& tower = pileupTowers.at(ii);
    // Calculate the (eta, phi) centre of the tower from its index
    unsigned int phiBin = ii % m_nPhiBins;
    float phi = (phiBin + 0.5) * m_towerPhiWidth;
    unsigned int etaBin = ii/m_nPhiBins;
    float eta = -m_maxEta + (etaBin + 0.5) * m_towerEtaWidth;
    bool overlaps = false;
    for (const xAOD::Jet* ijet : hardScatterJets) {
      // Taking absolutes exploits the symmetry of the overlap problem
      // We consider the case of overlapping with the right side, the top or the top right corner
      float dPhi = fabs(phi - ijet->phi() );
      float dEta = fabs(eta - ijet->eta() );
      if ( (dPhi < m_towerPhiWidth/2. && dEta < (jetRadiusParameter + m_towerEtaWidth/2.) )
	   || (dEta < m_towerEtaWidth/2. && dPhi < (jetRadiusParameter + m_towerPhiWidth/2.) )
	   || (dEta - m_towerEtaWidth/2.)*(dEta - m_towerEtaWidth/2.) + (dPhi-m_towerPhiWidth/2.)*(dPhi-m_towerPhiWidth/2.) < jetRadiusParameter*jetRadiusParameter) {
        overlaps = true;
        break;
      }
    }
    // Remove overlapping towers from the variance calculation
    if (overlaps)
      continue;
    sumEtPileup += tower.sumEt;
    sumEt2Pileup += tower.sumEt*tower.sumEt;
    ++nNonOverlappingPileupTowers;
  }
  float avgPileupEt = sumEtPileup/nNonOverlappingPileupTowers;
  float varPileupEt = sumEt2Pileup/nNonOverlappingPileupTowers - avgPileupEt*avgPileupEt;

  // Construct matrix to calculate corrections
  TMatrixD Xij(hardScatterJets.size(), hardScatterJets.size() );
  TMatrixD Cj(hardScatterJets.size(), 1);
  for (unsigned int ii = 0; ii < hardScatterJets.size(); ++ii) {
    float sinPhi1;
    float cosPhi1;
    sincosf(hardScatterJets.at(ii)->phi(), &sinPhi1, &cosPhi1);
    Cj[ii][0] = -m_constraintWeight * ptPileup.Mod() / sigma_det * (
        sigma_yy * cosPhi1 * cosPhiPileup
        + sigma_xx * sinPhi1 * sinPhiPileup
        - sigma_xy * (sinPhi1 * cosPhiPileup + cosPhi1 * sinPhiPileup) )
      + avgPileupEt / varPileupEt;
    for (unsigned int jj = ii; jj < hardScatterJets.size(); ++jj) {
      float sinPhi2;
      float cosPhi2;
      sincosf(hardScatterJets.at(jj)->phi(), &sinPhi2, &cosPhi2);
      Xij[ii][jj] = m_constraintWeight / sigma_det * (
          sigma_yy * cosPhi1 * cosPhi2
          + sigma_xx * sinPhi1 * sinPhi2
          - sigma_xy * (cosPhi1 * sinPhi2 + sinPhi1 * cosPhi2) );
      if (ii == jj)
        // Scale the variance of the jet by the ratio of its area to the tower area
        Xij[ii][jj] += m_towerPhiWidth*m_towerEtaWidth / (varPileupEt * hardScatterJets.at(ii)->getAttribute<float>(xAOD::JetAttribute::AttributeID::ActiveArea) );
      else
        Xij[jj][ii] = Xij[ii][jj];
    }
  }
  Xij.Invert();
  TMatrixD corrections(Xij * Cj);

  float ex = 0;
  float ey = 0;
  float ez = 0;
  float sumEt  = 0;
  float sumE = 0;
  for (const xAOD::CaloCluster* iclus : hardScatterClusters) {
    float pt = iclus->pt(m_clusterstate);
    float sinPhi;
    float cosPhi;
    float eta = iclus->eta(m_clusterstate);
    sincosf(iclus->phi(m_clusterstate), &sinPhi, &cosPhi);
    ex -= pt * cosPhi;
    ey -= pt * sinPhi;
    ez += pt * sinhf(eta);
    sumEt  += pt;
    sumE += iclus->p4(m_clusterstate).E();
  }
  // Apply corrections
  for (unsigned int ii = 0; ii < hardScatterJets.size(); ++ii) {
    float sinPhi;
    float cosPhi;
    sincosf(hardScatterJets.at(ii)->phi(), &sinPhi, &cosPhi);
    ex += corrections[ii][0] * cosPhi;
    ey += corrections[ii][0] * sinPhi;
    sumEt  -= corrections[ii][0];
  }
  
  // At the end make sure that you stored your calculated met values in the component
  metComp->m_ex = ex;
  metComp->m_ey = ey;
  metComp->m_ez = ez;
  metComp->m_sumEt = sumEt;
  metComp->m_sumE  = sumE;
  metComp->m_usedChannels += 1;

  // You *can* store bits in other components (either spreading the met over several components or recording extra information that might be useful)
  // However you need to be very sure that the helper is adding up the right bits
  /****************************************************************************************
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET

  metComp->m_ex = -MExEta;
  metComp->m_ey = -MEyEta;
  metComp->m_ez = -MEzEta;
  metComp->m_sumEt = sumEtEta;
  metComp->m_sumE  = sumEEta;
  metComp->m_usedChannels += 1;
  *****************************************************************************************/
  //}
  
  // --------------------------------------------------------------------------------------

  // move from "processing" to "processed" state
  metComp->m_status ^= m_maskProcessing; // switch off bit
  metComp->m_status |= m_maskProcessed;  // switch on bit

 // end container loop.

  if(m_timersvc) m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;

}
