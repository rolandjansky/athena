/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "GaudiKernel/ToolFactory.h"
#include "TrigMuonBackExtrapolator/TrigMuonBackExtrapolator.h"
#include "TrigMuonBackExtrapolator/BackExtrapolator.h"


TrigMuonBackExtrapolator::TrigMuonBackExtrapolator(const std::string& t, 
						   const std::string& n,
						   const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_aligned(true),
  m_dataset(false)
{
  declareInterface <ITrigMuonBackExtrapolator> ( this );
  declareProperty( "Aligned", m_aligned=true);
  declareProperty( "DataSet", m_dataset=false);

  //load LUT
  init_LUT();
}


TrigMuonBackExtrapolator::~TrigMuonBackExtrapolator() 
{

}


StatusCode
TrigMuonBackExtrapolator::initialize()
{
  msg(MSG::INFO) << "Initializing the MuonBackExtrapolator" << endreq;
  ATH_CHECK( AthAlgTool::initialize() );
  return StatusCode::SUCCESS;
}


StatusCode
TrigMuonBackExtrapolator::finalize()
{
  msg(MSG::INFO) << "Finalizing the MuonBackExtrapolator" << endreq;
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}


StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex (const MuonFeature* feature,
     double& extEta,double& sigmaEta,double& extPhi,double& sigmaPhi,double PT)
{
    bool result = MuonBackExtrapolator::give_eta_phi_at_vertex(
                                                     feature->pt(), 
						     feature->eta(), 
						     feature->phi(),
                                                     feature->saddress()!=-1,
						     feature->radius() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);


    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex (double pt,
    const MuonFeature* feature, double& extEta, double& sigmaEta,
    double& extPhi, double& sigmaPhi, double PT)
{
    bool result = MuonBackExtrapolator::give_eta_phi_at_vertex(
                                                     pt, 
						     feature->eta(), 
						     feature->phi(),
                                                     feature->saddress()!=-1,
						     feature->radius() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}


StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex( const MuonFeature* feature,
                               double ZetaID,double& extEta,double& sigmaEta,
                                   double& extPhi,double& sigmaPhi,double PT)
{
     bool result = MuonBackExtrapolator::give_eta_phi_at_tuned_vertex(
                                                     feature->pt(),
						     ZetaID,
						     feature->zeta(),
						     feature->eta(), 
						     feature->phi(),
                                                     feature->saddress()!=-1,
						     feature->radius() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);
     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}


StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex( double pt,
    const MuonFeature* feature, double ZetaID, double& extEta, double& sigmaEta,
    double& extPhi, double& sigmaPhi, double PT)
{
     bool result = MuonBackExtrapolator::give_eta_phi_at_tuned_vertex(
                                                     pt,
						     ZetaID,
						     feature->zeta(),
						     feature->eta(), 
						     feature->phi(),
                                                     feature->saddress()!=-1,
						     feature->radius() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);
     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}

StatusCode 
TrigMuonBackExtrapolator::loose_window_match (const MuonFeature* feature,
                     const TrigInDetTrack* idtrack,double winPT,double weight)
{
     bool result = MuonBackExtrapolator::idTrack_in_loose_window(
                                                  idtrack->param()->pT()/1000.,
						  feature->saddress(),
						  feature->radius(),
						  feature->eta(), 
						  feature->phi(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}			     

StatusCode 
TrigMuonBackExtrapolator::loose_window_match (double pt, 
                     const MuonFeature* feature, const TrigInDetTrack* idtrack,
		     double winPT,double weight)
{
     bool result = MuonBackExtrapolator::idTrack_in_loose_window(
                                                  pt,
						  feature->saddress(),
						  feature->radius(),
						  feature->eta(), 
						  feature->phi(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}			     


StatusCode 
TrigMuonBackExtrapolator::tight_window_match (const MuonFeature* feature,
		     const TrigInDetTrack* idtrack,double winPT,double weight)
{  
    bool result = MuonBackExtrapolator::idTrack_in_tight_window(
                                                  idtrack->param()->pT()/1000.,
						  feature->saddress(),
						  feature->radius(),
						  idtrack->param()->z0(),
						  feature->zeta(), 
						  feature->eta(), 
						  feature->phi(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}
  
  
StatusCode 
TrigMuonBackExtrapolator::tight_window_match (double pt,
                     const MuonFeature* feature, const TrigInDetTrack* idtrack,
		     double winPT,double weight)
{  
    bool result = MuonBackExtrapolator::idTrack_in_tight_window(
                                                  pt,
						  feature->saddress(),
						  feature->radius(),
						  idtrack->param()->z0(),
						  feature->zeta(), 
						  feature->eta(), 
						  feature->phi(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

double 
TrigMuonBackExtrapolator::give_loose_chi2 (const MuonFeature*,const TrigInDetTrack*)
{
    return 0.;
}  
  

double 
TrigMuonBackExtrapolator::give_tight_chi2  (const MuonFeature*,const TrigInDetTrack*)
{
    return 0.;
}

/////////////////////////////////
// Use xAOD EDM

StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex (const xAOD::L2StandAloneMuon* muonSA,
     double& extEta,double& sigmaEta,double& extPhi,double& sigmaPhi,double PT)
{
    bool result = MuonBackExtrapolator::give_eta_phi_at_vertex(
                                                     muonSA->pt(), 
						     muonSA->etaMS(), 
						     muonSA->phiMS(),
                                                     muonSA->sAddress()!=-1,
						     muonSA->rMS() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);


    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex (double pt,
    const xAOD::L2StandAloneMuon* muonSA, double& extEta, double& sigmaEta,
    double& extPhi, double& sigmaPhi, double PT)
{
    bool result = MuonBackExtrapolator::give_eta_phi_at_vertex(
                                                     pt, 
						     muonSA->etaMS(), 
						     muonSA->phiMS(),
                                                     muonSA->sAddress()!=-1,
						     muonSA->rMS() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}


StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex( const xAOD::L2StandAloneMuon* muonSA,
                               double ZetaID,double& extEta,double& sigmaEta,
                                   double& extPhi,double& sigmaPhi,double PT)
{
     bool result = MuonBackExtrapolator::give_eta_phi_at_tuned_vertex(
                                                     muonSA->pt(),
						     ZetaID,
						     muonSA->zMS(),
						     muonSA->etaMS(), 
						     muonSA->phiMS(),
                                                     muonSA->sAddress()!=-1,
						     muonSA->rMS() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);
     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}


StatusCode 
TrigMuonBackExtrapolator::give_eta_phi_at_vertex( double pt,
    const xAOD::L2StandAloneMuon* muonSA, double ZetaID, double& extEta, double& sigmaEta,
    double& extPhi, double& sigmaPhi, double PT)
{
     bool result = MuonBackExtrapolator::give_eta_phi_at_tuned_vertex(
                                                     pt,
						     ZetaID,
						     muonSA->zMS(),
						     muonSA->etaMS(), 
						     muonSA->phiMS(),
                                                     muonSA->sAddress()!=-1,
						     muonSA->rMS() <= 10.,
						     m_aligned,
                                                     m_dataset,
						     extEta,
                                                     sigmaEta,
						     extPhi,
						     sigmaPhi,
						     PT,
                                                     data_Barrel_Param,
                                                     data_Barrel_Sigmas,
                                                     data_Endcap_TriggerST_Param,
                                                     data_Endcap_TriggerST_Sigmas,
                                                     data_Endcap_InnerST_Param,
                                                     data_Endcap_InnerST_PhiSigmas,
                                                     data_Endcap_InnerST_EtaSigmas);
     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}

StatusCode 
TrigMuonBackExtrapolator::loose_window_match (const xAOD::L2StandAloneMuon* muonSA,
                     const TrigInDetTrack* idtrack,double winPT,double weight)
{
     bool result = MuonBackExtrapolator::idTrack_in_loose_window(
                                                  idtrack->param()->pT()/1000.,
						  muonSA->sAddress(),
						  muonSA->rMS(),
						  muonSA->etaMS(), 
						  muonSA->phiMS(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}			     

StatusCode 
TrigMuonBackExtrapolator::loose_window_match (double pt, 
                     const xAOD::L2StandAloneMuon* muonSA, const TrigInDetTrack* idtrack,
		     double winPT,double weight)
{
     bool result = MuonBackExtrapolator::idTrack_in_loose_window(
                                                  pt,
						  muonSA->sAddress(),
						  muonSA->rMS(),
						  muonSA->etaMS(), 
						  muonSA->phiMS(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

     if (result) return  StatusCode::SUCCESS;
     return StatusCode::FAILURE;
}			     


StatusCode 
TrigMuonBackExtrapolator::tight_window_match (const xAOD::L2StandAloneMuon* muonSA,
		     const TrigInDetTrack* idtrack,double winPT,double weight)
{  
    bool result = MuonBackExtrapolator::idTrack_in_tight_window(
                                                  idtrack->param()->pT()/1000.,
						  muonSA->sAddress(),
						  muonSA->rMS(),
						  idtrack->param()->z0(),
						  muonSA->zMS(), 
						  muonSA->etaMS(), 
						  muonSA->phiMS(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}
  
  
StatusCode 
TrigMuonBackExtrapolator::tight_window_match (double pt,
                     const xAOD::L2StandAloneMuon* muonSA, const TrigInDetTrack* idtrack,
		     double winPT,double weight)
{  
    bool result = MuonBackExtrapolator::idTrack_in_tight_window(
                                                  pt,
						  muonSA->sAddress(),
						  muonSA->rMS(),
						  idtrack->param()->z0(),
						  muonSA->zMS(), 
						  muonSA->etaMS(), 
						  muonSA->phiMS(),
						  idtrack->param()->eta(),
						  idtrack->param()->phi0(),
						  m_aligned,
                                                  m_dataset,
						  winPT,
                                                  weight,
                                                  data_Barrel_Param,
                                                  data_Barrel_Sigmas,
                                                  data_Endcap_TriggerST_Param,
                                                  data_Endcap_TriggerST_Sigmas,
                                                  data_Endcap_InnerST_Param,
                                                  data_Endcap_InnerST_PhiSigmas,
                                                  data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

void
TrigMuonBackExtrapolator::init_LUT()
{

  msg(MSG::INFO) << "TrigMuonBackExtrapolator LUT loaded" << endreq;

//Barrel
data_Barrel_Param[0][0][0] = -0.344175;   data_Barrel_Param[0][0][1] = -0.007004; 
data_Barrel_Param[0][1][0] =  0.389134;   data_Barrel_Param[0][1][1] =  0.005503; 
data_Barrel_Param[1][0][0] = -0.394867;   data_Barrel_Param[1][0][1] = -0.005019; 
data_Barrel_Param[1][1][0] =  0.361533;   data_Barrel_Param[1][1][1] =  0.006671; 

data_Barrel_Sigmas[0][0][0][0] =  0.019819;   data_Barrel_Sigmas[0][0][0][1] =  0.009023; 
data_Barrel_Sigmas[0][1][0][0] =  0.019706;   data_Barrel_Sigmas[0][1][0][1] =  0.008985; 
data_Barrel_Sigmas[1][0][0][0] =  0.025500;   data_Barrel_Sigmas[1][0][0][1] =  0.008895; 
data_Barrel_Sigmas[1][1][0][0] =  0.025495;   data_Barrel_Sigmas[1][1][0][1] =  0.008971; 
data_Barrel_Sigmas[0][0][1][0] =  0.157201;  data_Barrel_Sigmas[0][0][1][1] =  0.000833; 
data_Barrel_Sigmas[0][1][1][0] =  0.153530;  data_Barrel_Sigmas[0][1][1][1] =  0.001045; 
data_Barrel_Sigmas[1][0][1][0] =  0.152441;  data_Barrel_Sigmas[1][0][1][1] =  0.001100; 
data_Barrel_Sigmas[1][1][1][0] =  0.183810;  data_Barrel_Sigmas[1][1][1][1] = -0.000077; 

//Endcaps - TriggerST
 data_Endcap_TriggerST_Param[0][0][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][0][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][0][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][0][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][1][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][1][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][1][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][1][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][2][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][2][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][2][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][2][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][3][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][3][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][3][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][3][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][4][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][4][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][4][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][4][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][5][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][5][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][5][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][5][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][6][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][6][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][6][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][6][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][7][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][7][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][7][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][7][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][8][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][8][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][8][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][8][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][9][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][9][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][9][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][9][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][10][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][10][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][10][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][10][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][11][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][11][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][11][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][11][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][0][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][0][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][0][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][0][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][1][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][1][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][1][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][1][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][2][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][2][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][2][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][2][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][3][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][3][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][3][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][3][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][4][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][4][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][4][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][4][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][5][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][5][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][5][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][5][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][6][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][6][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][6][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][6][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][7][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][7][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][7][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][7][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][8][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][8][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][8][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][8][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][9][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][9][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][9][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][9][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][10][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][10][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][10][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][10][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][11][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][11][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][11][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][11][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][0][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][0][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][0][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][0][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][1][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][1][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][1][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][1][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][2][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][2][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][2][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][2][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][3][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][3][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][3][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][3][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][4][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][4][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][4][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][4][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][5][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][5][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][5][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][5][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][6][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][6][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][6][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][6][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][7][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][7][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][7][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][7][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][8][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][8][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][8][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][8][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][9][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][9][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][9][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][9][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][10][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][10][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][10][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][10][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][11][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][11][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][11][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][11][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][0][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][0][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][0][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][0][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][1][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][1][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][1][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][1][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][2][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][2][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][2][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][2][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][3][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][3][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][3][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][3][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][4][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][4][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][4][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][4][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][5][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][5][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][5][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][5][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][6][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][6][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][6][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][6][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][7][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][7][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][7][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][7][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][8][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][8][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][8][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][8][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][9][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][9][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][9][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][9][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][10][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][10][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][10][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][10][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][11][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][11][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][11][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][11][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[1][0][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][0][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][0][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][0][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][1][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][1][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][1][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][1][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][2][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][2][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][2][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][2][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][3][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][3][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][3][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][3][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][4][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][4][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][4][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][4][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][5][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][5][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][5][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][5][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][6][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][6][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][6][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][6][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][7][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][7][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][7][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][7][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][8][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][8][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][8][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][8][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][9][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][9][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][9][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][9][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][10][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][10][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][10][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][10][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][11][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][11][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][11][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][11][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][0][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][0][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][0][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][0][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][1][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][1][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][1][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][1][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][2][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][2][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][2][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][2][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][3][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][3][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][3][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][3][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][4][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][4][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][4][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][4][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][5][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][5][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][5][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][5][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][6][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][6][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][6][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][6][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][7][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][7][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][7][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][7][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][8][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][8][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][8][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][8][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][9][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][9][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][9][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][9][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][10][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][10][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][10][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][10][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][11][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][11][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][11][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][11][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][0][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][0][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][0][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][0][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][1][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][1][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][1][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][1][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][2][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][2][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][2][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][2][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][3][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][3][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][3][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][3][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][4][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][4][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][4][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][4][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][5][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][5][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][5][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][5][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][6][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][6][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][6][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][6][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][7][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][7][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][7][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][7][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][8][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][8][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][8][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][8][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][9][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][9][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][9][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][9][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][10][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][10][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][10][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][10][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][11][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][11][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][11][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][11][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][0][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][0][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][0][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][0][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][1][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][1][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][1][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][1][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][2][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][2][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][2][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][2][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][3][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][3][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][3][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][3][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][4][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][4][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][4][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][4][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][5][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][5][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][5][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][5][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][6][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][6][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][6][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][6][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][7][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][7][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][7][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][7][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][8][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][8][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][8][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][8][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][9][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][9][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][9][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][9][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][10][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][10][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][10][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][10][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][11][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][11][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][11][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][11][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[2][0][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][0][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][0][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][0][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][1][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][1][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][1][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][1][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][2][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][2][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][2][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][2][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][3][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][3][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][3][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][3][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][4][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][4][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][4][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][4][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][5][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][5][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][5][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][5][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][6][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][6][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][6][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][6][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][7][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][7][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][7][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][7][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][8][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][8][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][8][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][8][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][9][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][9][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][9][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][9][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][10][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][10][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][10][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][10][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][11][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][11][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][11][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][11][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][0][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][0][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][0][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][0][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][1][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][1][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][1][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][1][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][2][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][2][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][2][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][2][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][3][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][3][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][3][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][3][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][4][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][4][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][4][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][4][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][5][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][5][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][5][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][5][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][6][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][6][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][6][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][6][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][7][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][7][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][7][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][7][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][8][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][8][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][8][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][8][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][9][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][9][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][9][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][9][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][10][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][10][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][10][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][10][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][11][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][11][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][11][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][11][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][0][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][0][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][0][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][0][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][1][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][1][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][1][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][1][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][2][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][2][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][2][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][2][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][3][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][3][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][3][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][3][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][4][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][4][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][4][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][4][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][5][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][5][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][5][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][5][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][6][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][6][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][6][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][6][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][7][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][7][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][7][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][7][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][8][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][8][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][8][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][8][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][9][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][9][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][9][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][9][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][10][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][10][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][10][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][10][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][11][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][11][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][11][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][11][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][0][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][0][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][0][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][0][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][1][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][1][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][1][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][1][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][2][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][2][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][2][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][2][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][3][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][3][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][3][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][3][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][4][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][4][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][4][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][4][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][5][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][5][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][5][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][5][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][6][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][6][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][6][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][6][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][7][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][7][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][7][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][7][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][8][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][8][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][8][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][8][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][9][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][9][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][9][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][9][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][10][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][10][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][10][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][10][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][11][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][11][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][11][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][11][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[3][0][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][0][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][0][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][0][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][1][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][1][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][1][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][1][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][2][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][2][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][2][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][2][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][3][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][3][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][3][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][3][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][4][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][4][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][4][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][4][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][5][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][5][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][5][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][5][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][6][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][6][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][6][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][6][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][7][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][7][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][7][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][7][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][8][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][8][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][8][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][8][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][9][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][9][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][9][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][9][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][10][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][10][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][10][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][10][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][11][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][11][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][11][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][11][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][0][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][0][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][0][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][0][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][1][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][1][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][1][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][1][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][2][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][2][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][2][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][2][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][3][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][3][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][3][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][3][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][4][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][4][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][4][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][4][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][5][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][5][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][5][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][5][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][6][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][6][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][6][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][6][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][7][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][7][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][7][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][7][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][8][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][8][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][8][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][8][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][9][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][9][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][9][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][9][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][10][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][10][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][10][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][10][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][11][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][11][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][11][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][11][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][0][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][0][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][0][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][0][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][1][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][1][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][1][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][1][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][2][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][2][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][2][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][2][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][3][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][3][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][3][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][3][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][4][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][4][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][4][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][4][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][5][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][5][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][5][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][5][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][6][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][6][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][6][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][6][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][7][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][7][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][7][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][7][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][8][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][8][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][8][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][8][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][9][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][9][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][9][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][9][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][10][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][10][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][10][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][10][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][11][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][11][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][11][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][11][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][0][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][0][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][0][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][0][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][1][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][1][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][1][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][1][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][2][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][2][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][2][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][2][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][3][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][3][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][3][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][3][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][4][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][4][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][4][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][4][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][5][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][5][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][5][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][5][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][6][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][6][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][6][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][6][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][7][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][7][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][7][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][7][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][8][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][8][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][8][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][8][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][9][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][9][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][9][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][9][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][10][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][10][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][10][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][10][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][11][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][11][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][11][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][11][1][1][1][1] =    -0.00164896;

 data_Endcap_TriggerST_Sigmas[0][0][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][0][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][0][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][0][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][1][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][1][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][1][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][1][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][2][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][2][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][2][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][2][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][3][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][3][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][3][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][3][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][4][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][4][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][4][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][4][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][5][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][5][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][5][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][5][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][6][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][6][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][6][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][6][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][7][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][7][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][7][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][7][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][8][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][8][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][8][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][8][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][9][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][9][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][9][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][9][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][10][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][10][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][10][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][10][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][11][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][11][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][11][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][11][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][0][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][0][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][0][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][0][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][1][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][1][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][1][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][1][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][2][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][2][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][2][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][2][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][3][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][3][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][3][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][3][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][4][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][4][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][4][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][4][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][5][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][5][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][5][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][5][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][6][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][6][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][6][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][6][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][7][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][7][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][7][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][7][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][8][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][8][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][8][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][8][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][9][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][9][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][9][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][9][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][10][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][10][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][10][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][10][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][11][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][11][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][11][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][11][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][0][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][0][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][0][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][0][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][1][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][1][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][1][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][1][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][2][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][2][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][2][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][2][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][3][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][3][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][3][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][3][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][4][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][4][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][4][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][4][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][5][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][5][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][5][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][5][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][6][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][6][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][6][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][6][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][7][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][7][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][7][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][7][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][8][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][8][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][8][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][8][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][9][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][9][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][9][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][9][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][10][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][10][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][10][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][10][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][11][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][11][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][11][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][11][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][0][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][0][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][0][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][0][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][1][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][1][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][1][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][1][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][2][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][2][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][2][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][2][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][3][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][3][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][3][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][3][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][4][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][4][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][4][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][4][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][5][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][5][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][5][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][5][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][6][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][6][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][6][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][6][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][7][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][7][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][7][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][7][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][8][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][8][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][8][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][8][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][9][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][9][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][9][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][9][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][10][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][10][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][10][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][10][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][11][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][11][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][11][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][11][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[1][0][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][0][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][0][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][0][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][1][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][1][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][1][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][1][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][2][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][2][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][2][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][2][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][3][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][3][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][3][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][3][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][4][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][4][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][4][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][4][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][5][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][5][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][5][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][5][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][6][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][6][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][6][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][6][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][7][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][7][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][7][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][7][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][8][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][8][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][8][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][8][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][9][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][9][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][9][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][9][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][10][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][10][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][10][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][10][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][11][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][11][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][11][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][11][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][0][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][0][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][0][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][0][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][1][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][1][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][1][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][1][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][2][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][2][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][2][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][2][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][3][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][3][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][3][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][3][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][4][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][4][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][4][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][4][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][5][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][5][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][5][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][5][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][6][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][6][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][6][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][6][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][7][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][7][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][7][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][7][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][8][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][8][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][8][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][8][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][9][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][9][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][9][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][9][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][10][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][10][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][10][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][10][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][11][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][11][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][11][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][11][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][0][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][0][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][0][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][0][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][1][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][1][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][1][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][1][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][2][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][2][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][2][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][2][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][3][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][3][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][3][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][3][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][4][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][4][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][4][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][4][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][5][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][5][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][5][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][5][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][6][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][6][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][6][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][6][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][7][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][7][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][7][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][7][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][8][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][8][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][8][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][8][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][9][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][9][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][9][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][9][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][10][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][10][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][10][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][10][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][11][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][11][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][11][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][11][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][0][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][0][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][0][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][0][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][1][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][1][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][1][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][1][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][2][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][2][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][2][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][2][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][3][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][3][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][3][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][3][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][4][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][4][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][4][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][4][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][5][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][5][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][5][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][5][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][6][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][6][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][6][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][6][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][7][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][7][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][7][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][7][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][8][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][8][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][8][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][8][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][9][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][9][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][9][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][9][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][10][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][10][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][10][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][10][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][11][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][11][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][11][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][11][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[2][0][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][0][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][0][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][0][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][1][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][1][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][1][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][1][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][2][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][2][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][2][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][2][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][3][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][3][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][3][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][3][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][4][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][4][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][4][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][4][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][5][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][5][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][5][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][5][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][6][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][6][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][6][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][6][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][7][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][7][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][7][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][7][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][8][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][8][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][8][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][8][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][9][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][9][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][9][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][9][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][10][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][10][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][10][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][10][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][11][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][11][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][11][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][11][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][0][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][0][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][0][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][0][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][1][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][1][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][1][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][1][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][2][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][2][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][2][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][2][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][3][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][3][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][3][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][3][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][4][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][4][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][4][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][4][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][5][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][5][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][5][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][5][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][6][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][6][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][6][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][6][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][7][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][7][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][7][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][7][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][8][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][8][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][8][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][8][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][9][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][9][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][9][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][9][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][10][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][10][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][10][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][10][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][11][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][11][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][11][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][11][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][0][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][0][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][0][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][0][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][1][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][1][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][1][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][1][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][2][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][2][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][2][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][2][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][3][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][3][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][3][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][3][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][4][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][4][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][4][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][4][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][5][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][5][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][5][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][5][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][6][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][6][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][6][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][6][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][7][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][7][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][7][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][7][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][8][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][8][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][8][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][8][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][9][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][9][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][9][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][9][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][10][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][10][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][10][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][10][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][11][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][11][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][11][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][11][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][0][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][0][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][0][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][0][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][1][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][1][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][1][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][1][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][2][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][2][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][2][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][2][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][3][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][3][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][3][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][3][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][4][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][4][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][4][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][4][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][5][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][5][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][5][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][5][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][6][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][6][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][6][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][6][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][7][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][7][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][7][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][7][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][8][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][8][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][8][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][8][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][9][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][9][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][9][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][9][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][10][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][10][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][10][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][10][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][11][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][11][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][11][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][11][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[3][0][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][0][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][0][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][0][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][1][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][1][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][1][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][1][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][2][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][2][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][2][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][2][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][3][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][3][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][3][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][3][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][4][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][4][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][4][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][4][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][5][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][5][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][5][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][5][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][6][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][6][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][6][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][6][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][7][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][7][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][7][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][7][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][8][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][8][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][8][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][8][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][9][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][9][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][9][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][9][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][10][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][10][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][10][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][10][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][11][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][11][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][11][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][11][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][0][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][0][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][0][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][0][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][1][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][1][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][1][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][1][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][2][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][2][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][2][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][2][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][3][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][3][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][3][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][3][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][4][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][4][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][4][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][4][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][5][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][5][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][5][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][5][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][6][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][6][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][6][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][6][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][7][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][7][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][7][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][7][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][8][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][8][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][8][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][8][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][9][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][9][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][9][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][9][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][10][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][10][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][10][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][10][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][11][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][11][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][11][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][11][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][0][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][0][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][0][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][0][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][1][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][1][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][1][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][1][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][2][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][2][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][2][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][2][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][3][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][3][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][3][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][3][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][4][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][4][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][4][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][4][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][5][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][5][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][5][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][5][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][6][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][6][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][6][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][6][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][7][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][7][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][7][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][7][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][8][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][8][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][8][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][8][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][9][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][9][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][9][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][9][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][10][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][10][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][10][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][10][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][11][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][11][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][11][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][11][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][0][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][0][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][0][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][0][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][1][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][1][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][1][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][1][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][2][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][2][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][2][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][2][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][3][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][3][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][3][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][3][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][4][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][4][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][4][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][4][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][5][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][5][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][5][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][5][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][6][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][6][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][6][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][6][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][7][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][7][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][7][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][7][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][8][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][8][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][8][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][8][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][9][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][9][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][9][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][9][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][10][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][10][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][10][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][10][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][11][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][11][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][11][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][11][1][1][1][1] =    -0.00003769;

//Endcaps - InnerST
 data_Endcap_InnerST_PhiSigmas[0][0][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][0][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][0][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][0][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][0][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][0][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][0][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][0][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][1][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][1][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][1][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][1][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][1][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][1][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][1][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][1][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][2][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][2][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][2][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][2][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][2][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][2][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][2][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][2][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][3][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][3][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][3][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][3][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][3][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][3][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][3][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][3][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][4][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][4][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][4][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][4][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][4][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][4][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][4][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][4][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][5][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][5][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][5][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][5][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][5][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][5][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][5][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][5][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][6][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][6][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][6][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][6][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][6][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][6][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][6][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][6][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][7][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][7][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][7][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][7][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][7][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][7][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][7][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][7][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][8][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][8][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][8][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][8][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][8][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][8][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][8][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][8][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][9][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][9][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][9][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][9][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][9][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][9][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][9][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][9][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][10][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][10][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][10][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][10][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][10][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][10][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][10][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][10][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][11][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][11][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][11][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][11][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][11][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][11][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][11][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][11][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[1][0][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][0][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][0][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][0][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][0][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][0][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][0][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][0][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][1][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][1][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][1][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][1][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][1][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][1][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][1][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][1][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][2][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][2][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][2][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][2][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][2][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][2][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][2][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][2][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][3][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][3][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][3][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][3][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][3][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][3][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][3][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][3][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][4][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][4][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][4][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][4][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][4][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][4][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][4][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][4][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][5][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][5][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][5][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][5][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][5][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][5][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][5][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][5][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][6][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][6][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][6][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][6][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][6][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][6][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][6][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][6][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][7][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][7][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][7][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][7][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][7][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][7][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][7][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][7][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][8][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][8][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][8][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][8][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][8][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][8][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][8][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][8][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][9][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][9][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][9][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][9][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][9][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][9][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][9][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][9][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][10][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][10][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][10][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][10][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][10][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][10][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][10][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][10][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][11][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][11][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][11][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][11][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][11][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][11][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][11][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][11][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[0][0][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][0][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][0][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][0][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][0][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][0][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][0][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][0][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][1][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][1][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][1][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][1][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][1][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][1][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][1][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][1][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][2][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][2][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][2][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][2][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][2][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][2][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][2][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][2][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][3][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][3][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][3][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][3][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][3][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][3][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][3][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][3][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][4][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][4][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][4][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][4][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][4][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][4][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][4][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][4][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][5][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][5][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][5][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][5][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][5][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][5][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][5][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][5][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][6][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][6][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][6][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][6][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][6][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][6][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][6][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][6][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][7][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][7][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][7][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][7][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][7][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][7][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][7][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][7][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][8][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][8][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][8][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][8][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][8][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][8][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][8][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][8][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][9][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][9][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][9][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][9][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][9][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][9][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][9][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][9][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][10][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][10][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][10][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][10][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][10][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][10][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][10][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][10][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][11][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][11][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][11][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][11][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][11][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][11][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][11][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][11][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[1][0][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][0][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][0][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][0][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][0][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][0][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][0][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][0][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][1][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][1][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][1][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][1][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][1][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][1][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][1][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][1][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][2][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][2][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][2][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][2][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][2][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][2][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][2][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][2][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][3][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][3][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][3][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][3][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][3][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][3][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][3][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][3][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][4][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][4][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][4][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][4][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][4][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][4][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][4][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][4][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][5][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][5][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][5][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][5][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][5][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][5][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][5][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][5][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][6][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][6][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][6][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][6][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][6][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][6][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][6][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][6][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][7][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][7][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][7][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][7][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][7][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][7][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][7][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][7][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][8][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][8][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][8][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][8][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][8][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][8][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][8][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][8][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][9][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][9][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][9][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][9][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][9][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][9][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][9][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][9][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][10][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][10][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][10][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][10][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][10][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][10][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][10][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][10][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][11][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][11][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][11][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][11][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][11][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][11][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][11][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][11][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[0][0][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][0][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][0][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][0][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][0][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][0][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][0][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][0][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][1][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][1][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][1][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][1][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][1][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][1][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][1][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][1][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][2][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][2][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][2][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][2][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][2][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][2][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][2][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][2][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][3][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][3][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][3][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][3][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][3][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][3][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][3][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][3][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][4][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][4][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][4][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][4][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][4][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][4][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][4][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][4][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][5][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][5][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][5][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][5][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][5][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][5][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][5][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][5][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][6][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][6][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][6][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][6][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][6][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][6][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][6][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][6][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][7][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][7][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][7][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][7][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][7][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][7][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][7][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][7][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][8][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][8][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][8][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][8][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][8][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][8][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][8][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][8][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][9][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][9][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][9][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][9][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][9][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][9][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][9][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][9][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][10][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][10][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][10][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][10][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][10][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][10][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][10][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][10][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][11][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][11][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][11][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][11][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][11][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][11][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][11][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][11][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[1][0][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][0][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][0][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][0][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][0][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][0][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][0][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][0][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][1][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][1][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][1][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][1][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][1][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][1][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][1][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][1][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][2][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][2][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][2][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][2][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][2][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][2][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][2][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][2][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][3][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][3][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][3][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][3][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][3][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][3][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][3][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][3][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][4][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][4][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][4][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][4][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][4][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][4][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][4][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][4][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][5][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][5][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][5][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][5][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][5][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][5][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][5][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][5][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][6][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][6][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][6][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][6][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][6][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][6][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][6][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][6][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][7][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][7][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][7][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][7][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][7][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][7][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][7][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][7][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][8][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][8][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][8][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][8][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][8][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][8][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][8][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][8][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][9][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][9][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][9][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][9][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][9][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][9][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][9][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][9][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][10][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][10][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][10][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][10][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][10][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][10][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][10][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][10][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][11][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][11][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][11][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][11][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][11][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][11][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][11][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][11][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[0][0][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][0][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][0][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][0][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][0][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][0][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][0][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][0][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][1][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][1][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][1][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][1][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][1][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][1][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][1][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][1][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][2][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][2][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][2][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][2][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][2][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][2][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][2][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][2][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][3][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][3][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][3][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][3][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][3][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][3][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][3][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][3][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][4][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][4][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][4][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][4][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][4][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][4][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][4][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][4][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][5][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][5][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][5][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][5][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][5][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][5][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][5][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][5][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][6][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][6][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][6][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][6][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][6][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][6][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][6][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][6][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][7][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][7][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][7][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][7][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][7][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][7][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][7][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][7][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][8][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][8][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][8][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][8][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][8][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][8][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][8][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][8][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][9][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][9][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][9][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][9][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][9][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][9][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][9][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][9][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][10][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][10][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][10][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][10][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][10][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][10][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][10][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][10][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][11][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][11][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][11][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][11][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][11][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][11][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][11][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][11][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[1][0][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][0][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][0][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][0][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][0][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][0][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][0][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][0][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][1][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][1][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][1][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][1][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][1][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][1][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][1][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][1][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][2][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][2][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][2][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][2][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][2][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][2][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][2][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][2][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][3][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][3][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][3][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][3][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][3][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][3][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][3][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][3][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][4][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][4][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][4][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][4][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][4][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][4][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][4][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][4][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][5][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][5][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][5][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][5][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][5][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][5][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][5][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][5][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][6][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][6][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][6][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][6][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][6][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][6][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][6][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][6][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][7][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][7][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][7][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][7][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][7][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][7][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][7][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][7][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][8][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][8][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][8][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][8][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][8][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][8][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][8][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][8][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][9][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][9][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][9][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][9][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][9][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][9][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][9][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][9][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][10][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][10][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][10][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][10][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][10][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][10][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][10][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][10][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][11][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][11][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][11][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][11][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][11][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][11][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][11][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][11][1][1][1]=0.00096603; 

 data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263;

}
