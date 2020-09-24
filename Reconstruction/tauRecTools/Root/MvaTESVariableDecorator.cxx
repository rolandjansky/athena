/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/HelperFunctions.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

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

  ATH_CHECK(m_tauVertexCorrection.retrieve()); 
  ATH_CHECK(m_aveIntPerXKey.initialize());
  ATH_CHECK(m_vertexContainerKey.initialize(SG::AllowEmpty));
  
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::execute(xAOD::TauJet& xTau) const {
  
  // Decorate event info
  // need to check mu can be retrieved via EventInfo for Run3 trigger
  int mu = 0;
  SG::ReadDecorHandle<xAOD::EventInfo, float> eventInfoDecorHandle( m_aveIntPerXKey );
  if (!eventInfoDecorHandle.isPresent()) {
    ATH_MSG_WARNING ( "EventInfo decoration not available! Will set mu=0." );
  }
  else {
    mu = eventInfoDecorHandle(0);
  } 

  int nVtxPU = 0;
  if(!m_vertexContainerKey.empty()) {
    // Get the primary vertex container from StoreGate
    SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexContainerKey );
    if (!vertexInHandle.isValid()) {
      ATH_MSG_WARNING ("Could not retrieve HiveDataObj with key " << vertexInHandle.key() << ", will set nVtxPU=0.");
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
  double center_lambda=0.       , first_eng_dens=0.      , em_probability=0.      , second_lambda=0.      ;
  double mean_center_lambda=0.  , mean_first_eng_dens=0. , mean_em_probability=0. , mean_second_lambda=0. ;
  double mean_presampler_frac=0., lead_cluster_frac = 0. ;
  double clE=0., Etot=0.;

  // approximate Upsilon based on clusters, not PFOs (for online TES)
  TLorentzVector clusters_EM_P4;
  clusters_EM_P4.SetPtEtaPhiM(0,0,0,0);
  TLorentzVector clusters_had_P4;
  clusters_had_P4.SetPtEtaPhiM(0,0,0,0);
 
  if (! xTau.jetLink().isValid()) {
    ATH_MSG_ERROR("Tau jet link is invalid.");
    return StatusCode::FAILURE;
  }
  const xAOD::Jet *jetSeed = xTau.jet();
  
  const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed); 

  const xAOD::Vertex* tauVertex = nullptr;
  if (xTau.vertexLink().isValid()) tauVertex = xTau.vertex();

  TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(xTau);

  // Loop through jets, get links to clusters
  std::vector<const xAOD::CaloCluster*> clusterList;
  ATH_CHECK(tauRecTools::GetJetClusterList(jetSeed, clusterList, m_incShowerSubtr));

  // Loop through clusters and jet constituents
  for (const xAOD::CaloCluster* cluster : clusterList){
    TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(cluster, tauVertex, jetVertex);
  
    if (clusterP4.DeltaR(tauAxis) > 0.2) continue;

    clE = cluster->calE();
    Etot += clE;

    if(clE>lead_cluster_frac) lead_cluster_frac = clE;

    if(cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA,center_lambda))
      mean_center_lambda += clE*center_lambda;
    else ATH_MSG_WARNING("Failed to retrieve moment: CENTER_LAMBDA");

    if(cluster->retrieveMoment(xAOD::CaloCluster::MomentType::FIRST_ENG_DENS,first_eng_dens))
      mean_first_eng_dens += clE*first_eng_dens;
    else ATH_MSG_WARNING("Failed to retrieve moment: FIRST_ENG_DENS");

    if(cluster->retrieveMoment(xAOD::CaloCluster::MomentType::EM_PROBABILITY,em_probability)) {
      mean_em_probability += clE*em_probability;

      if(em_probability>0.5) clusters_EM_P4 += cluster->p4(xAOD::CaloCluster::State::CALIBRATED);      
      else clusters_had_P4 += cluster->p4(xAOD::CaloCluster::State::CALIBRATED);
    }
    else ATH_MSG_WARNING("Failed to retrieve moment: EM_PROBABILITY");

    if(cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA,second_lambda))
      mean_second_lambda += clE*second_lambda;
    else ATH_MSG_WARNING("Failed to retrieve moment: SECOND_LAMBDA");

    mean_presampler_frac += (cluster->eSample(CaloSampling::PreSamplerB) + cluster->eSample(CaloSampling::PreSamplerE));
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
  if(!jetSeed->getAttribute<int>("GhostMuonSegmentCount", nMuSeg)) nMuSeg=0;
  xTau.setDetail(xAOD::TauJetParameters::GhostMuonSegmentCount, nMuSeg);
  
  // ----summing corrected Pi0 PFO energies
  TLorentzVector Pi0_totalP4;
  Pi0_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  for(size_t i=0; i<xTau.nPi0PFOs(); i++){
    Pi0_totalP4 += xTau.pi0PFO(i)->p4();
  }
  
  double Pi0_totalE = Pi0_totalP4.E();
  
  // ----summing charged PFO energies
  TLorentzVector charged_totalP4;
  charged_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  for(size_t i=0; i<xTau.nChargedPFOs(); i++){
    charged_totalP4 += xTau.chargedPFO(i)->p4();
  }
  
  double charged_totalE = charged_totalP4.E();
  
  // ----calculate relative difference and decorate to tau
  double relDiff=0.;
  if(Pi0_totalE+charged_totalE){
    relDiff = (charged_totalE - Pi0_totalE) / (charged_totalE + Pi0_totalE) ;
  }
  xTau.setDetail(xAOD::TauJetParameters::PFOEngRelDiff, (float) relDiff);
  
  return StatusCode::SUCCESS;
}
