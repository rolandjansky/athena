/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/HelperFunctions.h"

#include "AsgDataHandles/ReadHandle.h"

#define GeV 1000

//_____________________________________________________________________________
MvaTESVariableDecorator::MvaTESVariableDecorator(const std::string& name) 
  : TauRecToolBase(name) {
  declareProperty("IncShowerSubtr", m_incShowerSubtr = true, "use shower subtracted clusters in calo calculations");
}

//_____________________________________________________________________________
MvaTESVariableDecorator::~MvaTESVariableDecorator() {

}

StatusCode MvaTESVariableDecorator::initialize() {

  ATH_CHECK( m_eventInfo.initialize() );

  ATH_CHECK( m_vertexInputContainer.initialize(!m_vertexInputContainer.key().empty()) );
  
  return StatusCode::SUCCESS;
}

StatusCode MvaTESVariableDecorator::finalize() {
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::execute(xAOD::TauJet& xTau) const {
  
  // Decorate event info
  // need to check mu can be retrieved via EventInfo for Run3 trigger
  int mu = 0;
  SG::ReadHandle<xAOD::EventInfo> eventinfoInHandle( m_eventInfo );
  if (!eventinfoInHandle.isValid()) {
    ATH_MSG_ERROR ( "Could not retrieve HiveDataObj with key " << eventinfoInHandle.key() << ", will set mu=0.");
    mu = 0.;
  }
  else {
    const xAOD::EventInfo* eventInfo = eventinfoInHandle.cptr();    
    mu = eventInfo->averageInteractionsPerCrossing();
  } 

  int nVtxPU = 0;
  if(!m_vertexInputContainer.key().empty()) {
    // Get the primary vertex container from StoreGate
    SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexInputContainer );
    if (!vertexInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << vertexInHandle.key() << ", will set nVtxPU=0.");
    }
    else {
      const xAOD::VertexContainer* vertexContainer = vertexInHandle.cptr();
      for (auto xVertex : *vertexContainer){
	if (xVertex->vertexType() == xAOD::VxType::PileUp)
	  ++nVtxPU;
      }
    }
  }
  
  SG::AuxElement::Accessor<float> acc_mu("mu");
  SG::AuxElement::Accessor<int> acc_nVtxPU("nVtxPU");
  
  acc_mu(xTau) = mu;
  acc_nVtxPU(xTau) = nVtxPU;

  // Decorate jet seed variables
  const xAOD::Jet* jet_seed = xTau.jet();
  
  double center_lambda=0.       , first_eng_dens=0.      , em_probability=0.      , second_lambda=0.      ;
  double mean_center_lambda=0.  , mean_first_eng_dens=0. , mean_em_probability=0. , mean_second_lambda=0. ;
  double mean_presampler_frac=0., lead_cluster_frac = 0. ;
  double clE=0., Etot=0.;

  // approximate Upsilon based on clusters, not PFOs (for online TES)
  TLorentzVector clusters_EM_P4;
  clusters_EM_P4.SetPtEtaPhiM(0,0,0,0);
  TLorentzVector clusters_had_P4;
  clusters_had_P4.SetPtEtaPhiM(0,0,0,0);

  TLorentzVector LC_P4;
  LC_P4.SetPtEtaPhiM(xTau.ptDetectorAxis(), xTau.etaDetectorAxis(), xTau.phiDetectorAxis(), xTau.m());

  // Loop through jets, get links to clusters
  std::vector<const xAOD::CaloCluster*> clusterList;
  ATH_CHECK(tauRecTools::GetJetClusterList(jet_seed, clusterList, m_incShowerSubtr, LC_P4, 0.2));

  // Loop through clusters and jet constituents
  for (auto cl : clusterList){

    clE = cl->calE();
    Etot += clE;

    if(clE>lead_cluster_frac) lead_cluster_frac = clE;

    if(cl->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA,center_lambda))
      mean_center_lambda += clE*center_lambda;
    else ATH_MSG_WARNING("Failed to retrieve moment: CENTER_LAMBDA");

    if(cl->retrieveMoment(xAOD::CaloCluster::MomentType::FIRST_ENG_DENS,first_eng_dens))
      mean_first_eng_dens += clE*first_eng_dens;
    else ATH_MSG_WARNING("Failed to retrieve moment: FIRST_ENG_DENS");

    if(cl->retrieveMoment(xAOD::CaloCluster::MomentType::EM_PROBABILITY,em_probability)) {
      mean_em_probability += clE*em_probability;

      if(em_probability>0.5) clusters_EM_P4 += (TLorentzVector) cl->p4(xAOD::CaloCluster::State::CALIBRATED);      
      else clusters_had_P4 += (TLorentzVector) cl->p4(xAOD::CaloCluster::State::CALIBRATED);
    }
    else ATH_MSG_WARNING("Failed to retrieve moment: EM_PROBABILITY");

    if(cl->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA,second_lambda))
      mean_second_lambda += clE*second_lambda;
    else ATH_MSG_WARNING("Failed to retrieve moment: SECOND_LAMBDA");

    mean_presampler_frac += (cl->eSample(CaloSampling::PreSamplerB) + cl->eSample(CaloSampling::PreSamplerE));
  }
  
  // ----calculate mean values
  if(Etot>0.) {
    mean_center_lambda /= Etot;
    mean_first_eng_dens /= Etot; 
    mean_em_probability /= Etot; 
    mean_second_lambda /= Etot;
    mean_presampler_frac /= Etot;
    lead_cluster_frac /= Etot;

    mean_first_eng_dens = TMath::Log10(mean_first_eng_dens/Etot);
  }
  
  // cluster-based upsilon, ranges from -1 to 1
  double upsilon_cluster = -2.;
  if(clusters_had_P4.E()+clusters_EM_P4.E()!=0.)
    upsilon_cluster = (clusters_had_P4.E()-clusters_EM_P4.E())/(clusters_had_P4.E()+clusters_EM_P4.E());
  
  // ----decorating jet seed information to tau
  xTau.setDetail(xAOD::TauJetParameters::ClustersMeanCenterLambda, (float) mean_center_lambda);
  xTau.setDetail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, (float) mean_first_eng_dens);
  xTau.setDetail(xAOD::TauJetParameters::ClustersMeanEMProbability, (float) mean_em_probability);
  xTau.setDetail(xAOD::TauJetParameters::ClustersMeanSecondLambda, (float) mean_second_lambda);
  xTau.setDetail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, (float) mean_presampler_frac);

  // online-specific, not defined in TauDefs enum
  SG::AuxElement::Accessor<float> acc_LeadClusterFrac("LeadClusterFrac");
  SG::AuxElement::Accessor<float> acc_UpsilonCluster("UpsilonCluster");
  acc_LeadClusterFrac(xTau) = (float) lead_cluster_frac;
  acc_UpsilonCluster(xTau) = (float) upsilon_cluster;

  if(inTrigger()) {
    return StatusCode::SUCCESS;
  }

  // ----retrieve Ghost Muon Segment Count (for punch-through studies)
  int nMuSeg=0;
  if(!jet_seed->getAttribute<int>("GhostMuonSegmentCount", nMuSeg)) nMuSeg=0;
  xTau.setDetail(xAOD::TauJetParameters::GhostMuonSegmentCount, nMuSeg);
  
  // ----summing corrected Pi0 PFO energies
  TLorentzVector Pi0_totalP4;
  Pi0_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  for(size_t i=0; i<xTau.nPi0PFOs(); i++){
    Pi0_totalP4 += (TLorentzVector)xTau.pi0PFO(i)->p4();
  };
  
  double Pi0_totalE = Pi0_totalP4.E();
  
  // ----summing charged PFO energies
  TLorentzVector charged_totalP4;
  charged_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  for(size_t i=0; i<xTau.nChargedPFOs(); i++){
    charged_totalP4 += (TLorentzVector)xTau.chargedPFO(i)->p4();
  };
  
  double charged_totalE = charged_totalP4.E();
  
  // ----calculate relative difference and decorate to tau
  double relDiff=0.;
  if(Pi0_totalE+charged_totalE){
    relDiff = (charged_totalE - Pi0_totalE) / (charged_totalE + Pi0_totalE) ;
  }
  xTau.setDetail(xAOD::TauJetParameters::PFOEngRelDiff, (float) relDiff);
  
  // calculate interpolated pT
  double pt_pantau  = xTau.ptPanTauCellBased();
  double pt_LC      = xTau.ptDetectorAxis();  
  double interpolWeight = 0.5 * ( 1. + TMath::TanH( ( pt_LC/GeV - 250. ) / 20. ) );
  double LC_pantau_interpolPt = interpolWeight*pt_LC + (1.-interpolWeight)*pt_pantau;
  
  xTau.setDetail(xAOD::TauJetParameters::LC_pantau_interpolPt, (float) LC_pantau_interpolPt);

  return StatusCode::SUCCESS;
}
