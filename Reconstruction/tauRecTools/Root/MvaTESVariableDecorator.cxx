/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESVariableDecorator.h"

// tools include(s) 
#include "TauAnalysisTools/HelperFunctions.h"

//_____________________________________________________________________________
MvaTESVariableDecorator::MvaTESVariableDecorator(const std::string& name) 
  : TauRecToolBase(name) 
  , m_xEventInfo(0)
  , m_xVertexContainer(0) 
  , m_mu(0)
  , m_nVtx(0)
{
}

//_____________________________________________________________________________
MvaTESVariableDecorator::~MvaTESVariableDecorator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::eventInitialize()
{
  ATH_CHECK(evtStore()->retrieve(m_xEventInfo,"EventInfo"));
  m_mu = m_xEventInfo->averageInteractionsPerCrossing();

  if(evtStore()->contains<xAOD::VertexContainer>("PrimaryVertices")){
    ATH_CHECK(evtStore()->retrieve(m_xVertexContainer, "PrimaryVertices"));
    m_nVtx = (int)m_xVertexContainer->size();
  }
  else {
    ATH_MSG_WARNING("No xAOD::VertexContainer, setting nVtx to 0");
    m_nVtx=0;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::execute(xAOD::TauJet& xTau) {
  
  // Decorate event info
  xTau.auxdata<double>("mu") = m_mu;
  xTau.auxdata<int>("nVtx") = m_nVtx;
 
  // Decorate jet seed variables
  const xAOD::Jet* jet_seed = xTau.jet();
  
  double center_lambda=0.      , first_eng_dens=0.      , em_probability=0.      , second_lambda=0.      ;
  double mean_center_lambda=0. , mean_first_eng_dens=0. , mean_em_probability=0. , mean_second_lambda=0. ;
  double mean_presampler_frac=0.;
  double clE=0., Etot=0.;
  
  TLorentzVector LC_P4;
  LC_P4.SetPtEtaPhiM(xTau.auxdata<float>("LC_TES_precalib"),
                     xTau.auxdata<float>("seedCalo_eta"),
                     xTau.auxdata<float>("seedCalo_phi"),
                     xTau.m());
  
  // ----loop over jet seed constituents
  xAOD::JetConstituentVector vec = jet_seed->getConstituents();
  xAOD::JetConstituentVector::iterator it = vec.begin();
  xAOD::JetConstituentVector::iterator itE = vec.end();
  for( ; it!=itE; it++){
    // ----DeltaR selection
    TLorentzVector cluster_P4;
    cluster_P4.SetPtEtaPhiM(1,(*it)->Eta(),(*it)->Phi(),1);
    if(LC_P4.DeltaR(cluster_P4)>0.2) continue;
    
    // ----retrieve CaloCluster moments
    const xAOD::CaloCluster* cl = dynamic_cast<const xAOD::CaloCluster *>( (*it)->rawConstituent() );
    cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::CENTER_LAMBDA,center_lambda);
    cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::FIRST_ENG_DENS,first_eng_dens);
    cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::EM_PROBABILITY,em_probability);
    cl->retrieveMoment(xAOD::CaloCluster_v1::MomentType::SECOND_LAMBDA,second_lambda);
    
    clE = cl->calE();
    Etot += clE;
    mean_center_lambda += clE*center_lambda;
    mean_first_eng_dens += clE*first_eng_dens;
    mean_em_probability += clE*em_probability;
    mean_second_lambda += clE*second_lambda;
    mean_presampler_frac += (cl->eSample(CaloSampling::PreSamplerB) + cl->eSample(CaloSampling::PreSamplerE));
  }
  
  // ----calculate mean values
  if(Etot>0.) {
    mean_center_lambda /= Etot;
    mean_first_eng_dens /= Etot; 
    mean_em_probability /= Etot; 
    mean_second_lambda /= Etot;
    mean_presampler_frac /= Etot;
    
    mean_first_eng_dens = TMath::Log10(mean_first_eng_dens/Etot);
  }
  
  // ----retrieve Ghost Muon Segment Count (for punch-through studies)
  const int nMuSeg = jet_seed->getAttribute<int>("GhostMuonSegmentCount");
  
  // ----decorating jet seed information to tau
  xTau.auxdecor<double>("center_lambda")   = mean_center_lambda;
  xTau.auxdecor<double>("first_eng_dens")  = mean_first_eng_dens;
  xTau.auxdecor<double>("em_probability")  = mean_em_probability;
  xTau.auxdecor<double>("second_lambda")   = mean_second_lambda;
  xTau.auxdecor<double>("presampler_frac") = mean_presampler_frac;
  xTau.auxdecor<int>("GhostMuonSegmentCount") = nMuSeg;
  
  // calculate PFO energy relative difference
  // ----summing corrected Pi0 PFO energies
  TLorentzVector Pi0_totalP4;
  Pi0_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  std::vector<TLorentzVector> Pi0PFOs;
  TauAnalysisTools::createPi0Vectors(&xTau,Pi0PFOs);
  
  for(size_t i=0; i<Pi0PFOs.size(); i++){
    Pi0_totalP4 += Pi0PFOs.at(i);
  };
  
  double Pi0_totalE = Pi0_totalP4.E();
  int    nPi0PFOs   = (int)Pi0PFOs.size();
  
  // ----summing charged PFO energies
  TLorentzVector charged_totalP4;
  charged_totalP4.SetPtEtaPhiM(0,0,0,0);
  
  for(size_t i=0; i<xTau.nChargedPFOs(); i++){
    charged_totalP4 += (TLorentzVector)xTau.chargedPFO(i)->p4();
  };
  
  double charged_totalE = charged_totalP4.E();
  int    nChargedPFOs   = (int)xTau.nChargedPFOs();
  
  // ----calculate relative difference and decorate to tau
  double relDiff=0.;
  if(Pi0_totalE+charged_totalE){
    relDiff = (charged_totalE - Pi0_totalE) / (charged_totalE + Pi0_totalE) ;
  }
  xTau.auxdecor<int>("nPi0PFOs")     = nPi0PFOs;
  xTau.auxdecor<int>("nChargedPFOs") = nChargedPFOs;
  xTau.auxdecor<double>("PFOEngRelDiff") = relDiff;
  
  // calculate interpolated pT
  double GeV = 1000.;
  double pt_pantau  = xTau.ptPanTauCellBased();
  double pt_LC      = xTau.auxdata<float>("LC_TES_precalib");
  double interpolWeight;
  
  interpolWeight = 0.5 * ( 1. + TMath::TanH( ( pt_LC/GeV - 250. ) / 20. ) );
  double LC_pantau_interpolPt = interpolWeight*pt_LC + (1.-interpolWeight)*pt_pantau;
  
  xTau.auxdecor<double>("LC_pantau_interpolPt") = LC_pantau_interpolPt;

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode MvaTESVariableDecorator::eventFinalize()
{
  return StatusCode::SUCCESS;
}
