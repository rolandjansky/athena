/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESVariableDecorator.h"

// tools include(s) 
//#include "TauAnalysisTools/HelperFunctions.h"

//_____________________________________________________________________________
MvaTESVariableDecorator::MvaTESVariableDecorator(const std::string& name) 
  : TauRecToolBase(name) 
  , m_xEventInfo(0)
  , m_xVertexContainer(0) 
  , m_mu(0)
  , m_nVtxPU(0)
{
}

//_____________________________________________________________________________
MvaTESVariableDecorator::~MvaTESVariableDecorator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::eventInitialize()
{
  if(!inTrigger()) {

    ATH_CHECK(evtStore()->retrieve(m_xEventInfo,"EventInfo"));
    m_mu = m_xEventInfo->averageInteractionsPerCrossing();
    
    m_nVtxPU = 0;
    if(evtStore()->contains<xAOD::VertexContainer>("PrimaryVertices")){
      ATH_CHECK(evtStore()->retrieve(m_xVertexContainer, "PrimaryVertices"));  
      for (auto xVertex : *m_xVertexContainer)
	if (xVertex->vertexType() == xAOD::VxType::PileUp)
	  m_nVtxPU++;
    }
    else {
      if(m_emitVertexWarning) {
	ATH_MSG_WARNING("No xAOD::VertexContainer, setting nVtxPU to 0");
	m_emitVertexWarning=false;
      }
      m_nVtxPU=0;
    }
  }
  else {
    
    // AvgInteractions is stored as double in TrigTauRecMerged, so have to retrieve it as a double...
    double mu_tmp = 0.;
    if( tauEventData()->hasObject("AvgInteractions") && tauEventData()->getObject("AvgInteractions", mu_tmp).isSuccess() ) {
      m_mu = mu_tmp;      
    }
    else {
      m_mu = 0.;
      if(m_emitVertexWarning) ATH_MSG_WARNING("AvgInteractions could not be retrieved, will use mu=0.");      
    }
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::execute(xAOD::TauJet& xTau) {
  
  // Decorate event info
  
  static SG::AuxElement::Accessor<float> acc_mu("mu");
  static SG::AuxElement::Accessor<int> acc_nVtxPU("nVtxPU");

  acc_mu(xTau) = m_mu;
  acc_nVtxPU(xTau) = m_nVtxPU;

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
  LC_P4.SetPtEtaPhiM(xTau.ptDetectorAxis(),
		     xTau.etaDetectorAxis(),
		     xTau.phiDetectorAxis(),
		     xTau.m());
  
  // ----loop over jet seed constituents
  xAOD::JetConstituentVector vec = jet_seed->getConstituents();
  xAOD::JetConstituentVector::iterator it = vec.begin();
  xAOD::JetConstituentVector::iterator itE = vec.end();
  for( ; it!=itE; ++it){
    // ----DeltaR selection
    TLorentzVector cluster_P4;
    cluster_P4.SetPtEtaPhiM(1,(*it)->Eta(),(*it)->Phi(),0);
    if(LC_P4.DeltaR(cluster_P4)>0.2) continue;
    
    // ----retrieve CaloCluster moments
    const xAOD::CaloCluster* cl = dynamic_cast<const xAOD::CaloCluster *>( (*it)->rawConstituent() );        

    clE = cl->calE();
    Etot += clE;

    if(clE>lead_cluster_frac) lead_cluster_frac = clE;

    if(cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::CENTER_LAMBDA,center_lambda))
      mean_center_lambda += clE*center_lambda;
    else ATH_MSG_WARNING("Failed to retrieve moment: CENTER_LAMBDA");
    
    if(cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::FIRST_ENG_DENS,first_eng_dens))
      mean_first_eng_dens += clE*first_eng_dens;
    else ATH_MSG_WARNING("Failed to retrieve moment: FIRST_ENG_DENS");
    
    if(cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::EM_PROBABILITY,em_probability)) {
      mean_em_probability += clE*em_probability;

      if(em_probability>0.5) clusters_EM_P4 += (TLorentzVector) cl->p4(xAOD::CaloCluster::State::CALIBRATED);      
      else clusters_had_P4 += (TLorentzVector) cl->p4(xAOD::CaloCluster::State::CALIBRATED);
    }
    else ATH_MSG_WARNING("Failed to retrieve moment: EM_PROBABILITY");
    
    if(cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::SECOND_LAMBDA,second_lambda))
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
  static SG::AuxElement::Accessor<float> acc_LeadClusterFrac("LeadClusterFrac");
  static SG::AuxElement::Accessor<float> acc_UpsilonCluster("UpsilonCluster");
  acc_LeadClusterFrac(xTau) = (float) lead_cluster_frac;
  acc_UpsilonCluster(xTau) = (float) upsilon_cluster;

  if(!inTrigger()) {

    // ----retrieve Ghost Muon Segment Count (for punch-through studies)
    int nMuSeg=0;
    if(!jet_seed->getAttribute<int>("GhostMuonSegmentCount", nMuSeg)) nMuSeg=0;
    xTau.setDetail(xAOD::TauJetParameters::GhostMuonSegmentCount, nMuSeg);

    // calculate PFO energy relative difference
    // ----summing corrected Pi0 PFO energies
    TLorentzVector Pi0_totalP4;
    Pi0_totalP4.SetPtEtaPhiM(0,0,0,0);
    
    //This should be available in EDM as of TauJet_v3
    //  TauAnalysisTools::createPi0Vectors(&xTau,Pi0PFOs);
    //for( size_t i=0; i !=  xTau.nPi0s(); ++i ) Pi0_totalP4+= xTau.pi0(i)->p4();
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
    double GeV = 1000.;
    double pt_pantau  = xTau.ptPanTauCellBased();
    double pt_LC      = xTau.ptDetectorAxis();
    double interpolWeight;
    
    interpolWeight = 0.5 * ( 1. + TMath::TanH( ( pt_LC/GeV - 250. ) / 20. ) );
    double LC_pantau_interpolPt = interpolWeight*pt_LC + (1.-interpolWeight)*pt_pantau;
    
    xTau.setDetail(xAOD::TauJetParameters::LC_pantau_interpolPt, (float) LC_pantau_interpolPt);
  }

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::eventFinalize()
{
  return StatusCode::SUCCESS;
}
