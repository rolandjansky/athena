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
  msg(MSG::INFO) << "Initializing the MuonBackExtrapolator" << endmsg;
  ATH_CHECK( AthAlgTool::initialize() );
  return StatusCode::SUCCESS;
}


StatusCode
TrigMuonBackExtrapolator::finalize()
{
  msg(MSG::INFO) << "Finalizing the MuonBackExtrapolator" << endmsg;
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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);


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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);

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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);
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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);
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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);


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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);

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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);
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
                                                     m_data_Barrel_Param,
                                                     m_data_Barrel_Sigmas,
                                                     m_data_Endcap_TriggerST_Param,
                                                     m_data_Endcap_TriggerST_Sigmas,
                                                     m_data_Endcap_InnerST_Param,
                                                     m_data_Endcap_InnerST_PhiSigmas,
                                                     m_data_Endcap_InnerST_EtaSigmas);
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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

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
                                                  m_data_Barrel_Param,
                                                  m_data_Barrel_Sigmas,
                                                  m_data_Endcap_TriggerST_Param,
                                                  m_data_Endcap_TriggerST_Sigmas,
                                                  m_data_Endcap_InnerST_Param,
                                                  m_data_Endcap_InnerST_PhiSigmas,
                                                  m_data_Endcap_InnerST_EtaSigmas);

    if (result) return  StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

void
TrigMuonBackExtrapolator::init_LUT()
{

  msg(MSG::INFO) << "TrigMuonBackExtrapolator LUT loaded" << endmsg;

//Barrel
m_data_Barrel_Param[0][0][0] = -0.344175;   m_data_Barrel_Param[0][0][1] = -0.007004; 
m_data_Barrel_Param[0][1][0] =  0.389134;   m_data_Barrel_Param[0][1][1] =  0.005503; 
m_data_Barrel_Param[1][0][0] = -0.394867;   m_data_Barrel_Param[1][0][1] = -0.005019; 
m_data_Barrel_Param[1][1][0] =  0.361533;   m_data_Barrel_Param[1][1][1] =  0.006671; 

m_data_Barrel_Sigmas[0][0][0][0] =  0.019819;   m_data_Barrel_Sigmas[0][0][0][1] =  0.009023; 
m_data_Barrel_Sigmas[0][1][0][0] =  0.019706;   m_data_Barrel_Sigmas[0][1][0][1] =  0.008985; 
m_data_Barrel_Sigmas[1][0][0][0] =  0.025500;   m_data_Barrel_Sigmas[1][0][0][1] =  0.008895; 
m_data_Barrel_Sigmas[1][1][0][0] =  0.025495;   m_data_Barrel_Sigmas[1][1][0][1] =  0.008971; 
m_data_Barrel_Sigmas[0][0][1][0] =  0.157201;  m_data_Barrel_Sigmas[0][0][1][1] =  0.000833; 
m_data_Barrel_Sigmas[0][1][1][0] =  0.153530;  m_data_Barrel_Sigmas[0][1][1][1] =  0.001045; 
m_data_Barrel_Sigmas[1][0][1][0] =  0.152441;  m_data_Barrel_Sigmas[1][0][1][1] =  0.001100; 
m_data_Barrel_Sigmas[1][1][1][0] =  0.183810;  m_data_Barrel_Sigmas[1][1][1][1] = -0.000077; 

//Endcaps - TriggerST
 m_data_Endcap_TriggerST_Param[0][0][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][0][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][0][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][0][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][1][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][1][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][1][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][1][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][2][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][2][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][2][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][2][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][3][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][3][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][3][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][3][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][4][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][4][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][4][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][4][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][5][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][5][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][5][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][5][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][6][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][6][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][6][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][6][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][7][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][7][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][7][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][7][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][8][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][8][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][8][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][8][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][9][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][9][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][9][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][9][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][10][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][10][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][10][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][10][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][11][0][0][0][0] =     0.39008528; m_data_Endcap_TriggerST_Param[0][11][0][0][0][1] =    -0.00008796;  m_data_Endcap_TriggerST_Param[0][11][0][0][1][0] =    -0.23731600; m_data_Endcap_TriggerST_Param[0][11][0][0][1][1] =    -0.00697019;
 m_data_Endcap_TriggerST_Param[0][0][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][0][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][0][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][0][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][1][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][1][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][1][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][1][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][2][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][2][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][2][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][2][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][3][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][3][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][3][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][3][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][4][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][4][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][4][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][4][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][5][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][5][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][5][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][5][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][6][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][6][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][6][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][6][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][7][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][7][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][7][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][7][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][8][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][8][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][8][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][8][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][9][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][9][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][9][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][9][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][10][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][10][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][10][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][10][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][11][0][1][0][0] =    -0.41645430; m_data_Endcap_TriggerST_Param[0][11][0][1][0][1] =     0.00196946;  m_data_Endcap_TriggerST_Param[0][11][0][1][1][0] =     0.24404074; m_data_Endcap_TriggerST_Param[0][11][0][1][1][1] =     0.00364261;
 m_data_Endcap_TriggerST_Param[0][0][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][0][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][0][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][0][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][1][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][1][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][1][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][1][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][2][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][2][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][2][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][2][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][3][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][3][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][3][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][3][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][4][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][4][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][4][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][4][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][5][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][5][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][5][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][5][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][6][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][6][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][6][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][6][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][7][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][7][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][7][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][7][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][8][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][8][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][8][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][8][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][9][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][9][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][9][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][9][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][10][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][10][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][10][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][10][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][11][1][0][0][0] =     0.38674998; m_data_Endcap_TriggerST_Param[0][11][1][0][0][1] =    -0.00083805;  m_data_Endcap_TriggerST_Param[0][11][1][0][1][0] =    -0.24427115; m_data_Endcap_TriggerST_Param[0][11][1][0][1][1] =    -0.00569835;
 m_data_Endcap_TriggerST_Param[0][0][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][0][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][0][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][0][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][1][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][1][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][1][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][1][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][2][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][2][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][2][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][2][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][3][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][3][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][3][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][3][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][4][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][4][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][4][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][4][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][5][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][5][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][5][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][5][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][6][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][6][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][6][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][6][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][7][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][7][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][7][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][7][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][8][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][8][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][8][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][8][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][9][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][9][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][9][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][9][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][10][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][10][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][10][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][10][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[0][11][1][1][0][0] =    -0.44981688; m_data_Endcap_TriggerST_Param[0][11][1][1][0][1] =     0.00247323;  m_data_Endcap_TriggerST_Param[0][11][1][1][1][0] =     0.36769134; m_data_Endcap_TriggerST_Param[0][11][1][1][1][1] =     0.00051971;
 m_data_Endcap_TriggerST_Param[1][0][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][0][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][0][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][0][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][1][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][1][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][1][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][1][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][2][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][2][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][2][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][2][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][3][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][3][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][3][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][3][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][4][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][4][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][4][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][4][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][5][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][5][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][5][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][5][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][6][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][6][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][6][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][6][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][7][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][7][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][7][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][7][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][8][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][8][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][8][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][8][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][9][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][9][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][9][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][9][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][10][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][10][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][10][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][10][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][11][0][0][0][0] =     0.12434421; m_data_Endcap_TriggerST_Param[1][11][0][0][0][1] =     0.00301339;  m_data_Endcap_TriggerST_Param[1][11][0][0][1][0] =    -0.08611453; m_data_Endcap_TriggerST_Param[1][11][0][0][1][1] =    -0.00994466;
 m_data_Endcap_TriggerST_Param[1][0][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][0][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][0][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][0][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][1][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][1][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][1][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][1][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][2][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][2][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][2][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][2][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][3][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][3][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][3][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][3][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][4][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][4][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][4][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][4][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][5][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][5][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][5][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][5][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][6][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][6][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][6][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][6][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][7][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][7][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][7][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][7][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][8][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][8][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][8][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][8][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][9][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][9][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][9][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][9][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][10][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][10][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][10][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][10][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][11][0][1][0][0] =    -0.15175075; m_data_Endcap_TriggerST_Param[1][11][0][1][0][1] =    -0.00214425;  m_data_Endcap_TriggerST_Param[1][11][0][1][1][0] =     0.04845307; m_data_Endcap_TriggerST_Param[1][11][0][1][1][1] =     0.00765097;
 m_data_Endcap_TriggerST_Param[1][0][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][0][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][0][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][0][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][1][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][1][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][1][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][1][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][2][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][2][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][2][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][2][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][3][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][3][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][3][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][3][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][4][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][4][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][4][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][4][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][5][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][5][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][5][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][5][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][6][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][6][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][6][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][6][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][7][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][7][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][7][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][7][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][8][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][8][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][8][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][8][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][9][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][9][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][9][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][9][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][10][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][10][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][10][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][10][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][11][1][0][0][0] =     0.24444920; m_data_Endcap_TriggerST_Param[1][11][1][0][0][1] =     0.00041680;  m_data_Endcap_TriggerST_Param[1][11][1][0][1][0] =    -0.14596133; m_data_Endcap_TriggerST_Param[1][11][1][0][1][1] =    -0.00769274;
 m_data_Endcap_TriggerST_Param[1][0][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][0][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][0][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][0][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][1][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][1][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][1][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][1][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][2][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][2][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][2][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][2][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][3][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][3][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][3][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][3][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][4][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][4][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][4][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][4][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][5][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][5][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][5][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][5][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][6][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][6][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][6][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][6][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][7][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][7][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][7][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][7][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][8][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][8][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][8][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][8][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][9][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][9][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][9][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][9][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][10][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][10][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][10][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][10][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[1][11][1][1][0][0] =    -0.27022220; m_data_Endcap_TriggerST_Param[1][11][1][1][0][1] =     0.00099873;  m_data_Endcap_TriggerST_Param[1][11][1][1][1][0] =     0.32536867; m_data_Endcap_TriggerST_Param[1][11][1][1][1][1] =     0.00131456;
 m_data_Endcap_TriggerST_Param[2][0][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][0][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][0][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][0][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][1][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][1][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][1][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][1][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][2][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][2][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][2][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][2][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][3][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][3][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][3][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][3][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][4][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][4][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][4][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][4][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][5][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][5][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][5][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][5][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][6][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][6][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][6][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][6][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][7][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][7][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][7][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][7][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][8][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][8][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][8][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][8][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][9][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][9][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][9][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][9][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][10][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][10][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][10][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][10][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][11][0][0][0][0] =     0.34728334; m_data_Endcap_TriggerST_Param[2][11][0][0][0][1] =    -0.00001011;  m_data_Endcap_TriggerST_Param[2][11][0][0][1][0] =    -0.22728005; m_data_Endcap_TriggerST_Param[2][11][0][0][1][1] =    -0.00112712;
 m_data_Endcap_TriggerST_Param[2][0][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][0][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][0][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][0][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][1][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][1][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][1][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][1][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][2][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][2][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][2][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][2][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][3][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][3][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][3][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][3][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][4][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][4][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][4][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][4][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][5][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][5][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][5][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][5][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][6][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][6][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][6][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][6][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][7][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][7][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][7][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][7][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][8][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][8][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][8][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][8][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][9][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][9][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][9][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][9][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][10][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][10][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][10][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][10][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][11][0][1][0][0] =    -0.32401417; m_data_Endcap_TriggerST_Param[2][11][0][1][0][1] =    -0.00136715;  m_data_Endcap_TriggerST_Param[2][11][0][1][1][0] =     0.14903215; m_data_Endcap_TriggerST_Param[2][11][0][1][1][1] =     0.00117954;
 m_data_Endcap_TriggerST_Param[2][0][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][0][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][0][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][0][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][1][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][1][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][1][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][1][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][2][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][2][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][2][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][2][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][3][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][3][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][3][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][3][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][4][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][4][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][4][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][4][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][5][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][5][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][5][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][5][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][6][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][6][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][6][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][6][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][7][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][7][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][7][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][7][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][8][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][8][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][8][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][8][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][9][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][9][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][9][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][9][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][10][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][10][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][10][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][10][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][11][1][0][0][0] =     0.45776655; m_data_Endcap_TriggerST_Param[2][11][1][0][0][1] =    -0.00218530;  m_data_Endcap_TriggerST_Param[2][11][1][0][1][0] =    -0.18954463; m_data_Endcap_TriggerST_Param[2][11][1][0][1][1] =    -0.00189962;
 m_data_Endcap_TriggerST_Param[2][0][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][0][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][0][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][0][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][1][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][1][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][1][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][1][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][2][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][2][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][2][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][2][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][3][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][3][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][3][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][3][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][4][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][4][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][4][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][4][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][5][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][5][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][5][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][5][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][6][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][6][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][6][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][6][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][7][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][7][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][7][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][7][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][8][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][8][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][8][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][8][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][9][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][9][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][9][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][9][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][10][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][10][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][10][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][10][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[2][11][1][1][0][0] =    -0.39464600; m_data_Endcap_TriggerST_Param[2][11][1][1][0][1] =     0.00224427;  m_data_Endcap_TriggerST_Param[2][11][1][1][1][0] =     0.26039947; m_data_Endcap_TriggerST_Param[2][11][1][1][1][1] =    -0.00294014;
 m_data_Endcap_TriggerST_Param[3][0][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][0][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][0][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][0][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][1][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][1][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][1][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][1][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][2][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][2][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][2][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][2][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][3][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][3][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][3][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][3][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][4][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][4][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][4][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][4][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][5][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][5][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][5][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][5][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][6][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][6][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][6][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][6][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][7][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][7][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][7][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][7][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][8][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][8][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][8][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][8][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][9][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][9][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][9][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][9][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][10][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][10][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][10][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][10][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][11][0][0][0][0] =     0.57829530; m_data_Endcap_TriggerST_Param[3][11][0][0][0][1] =    -0.00135664;  m_data_Endcap_TriggerST_Param[3][11][0][0][1][0] =    -0.20128576; m_data_Endcap_TriggerST_Param[3][11][0][0][1][1] =    -0.00019486;
 m_data_Endcap_TriggerST_Param[3][0][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][0][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][0][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][0][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][1][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][1][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][1][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][1][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][2][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][2][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][2][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][2][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][3][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][3][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][3][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][3][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][4][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][4][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][4][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][4][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][5][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][5][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][5][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][5][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][6][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][6][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][6][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][6][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][7][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][7][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][7][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][7][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][8][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][8][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][8][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][8][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][9][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][9][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][9][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][9][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][10][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][10][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][10][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][10][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][11][0][1][0][0] =    -0.55326371; m_data_Endcap_TriggerST_Param[3][11][0][1][0][1] =     0.00203642;  m_data_Endcap_TriggerST_Param[3][11][0][1][1][0] =     0.16990056; m_data_Endcap_TriggerST_Param[3][11][0][1][1][1] =    -0.00111816;
 m_data_Endcap_TriggerST_Param[3][0][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][0][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][0][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][0][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][1][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][1][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][1][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][1][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][2][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][2][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][2][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][2][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][3][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][3][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][3][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][3][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][4][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][4][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][4][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][4][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][5][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][5][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][5][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][5][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][6][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][6][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][6][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][6][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][7][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][7][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][7][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][7][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][8][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][8][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][8][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][8][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][9][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][9][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][9][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][9][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][10][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][10][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][10][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][10][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][11][1][0][0][0] =     0.52335774; m_data_Endcap_TriggerST_Param[3][11][1][0][0][1] =    -0.00018286;  m_data_Endcap_TriggerST_Param[3][11][1][0][1][0] =    -0.17730148; m_data_Endcap_TriggerST_Param[3][11][1][0][1][1] =    -0.00056622;
 m_data_Endcap_TriggerST_Param[3][0][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][0][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][0][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][0][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][1][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][1][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][1][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][1][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][2][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][2][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][2][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][2][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][3][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][3][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][3][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][3][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][4][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][4][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][4][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][4][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][5][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][5][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][5][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][5][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][6][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][6][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][6][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][6][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][7][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][7][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][7][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][7][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][8][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][8][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][8][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][8][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][9][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][9][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][9][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][9][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][10][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][10][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][10][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][10][1][1][1][1] =    -0.00164896;
 m_data_Endcap_TriggerST_Param[3][11][1][1][0][0] =    -0.60069544; m_data_Endcap_TriggerST_Param[3][11][1][1][0][1] =     0.00269249;  m_data_Endcap_TriggerST_Param[3][11][1][1][1][0] =     0.19942259; m_data_Endcap_TriggerST_Param[3][11][1][1][1][1] =    -0.00164896;

 m_data_Endcap_TriggerST_Sigmas[0][0][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][0][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][0][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][0][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][1][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][1][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][1][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][1][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][2][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][2][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][2][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][2][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][3][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][3][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][3][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][3][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][4][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][4][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][4][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][4][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][5][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][5][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][5][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][5][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][6][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][6][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][6][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][6][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][7][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][7][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][7][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][7][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][8][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][8][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][8][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][8][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][9][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][9][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][9][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][9][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][10][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][10][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][10][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][10][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][11][0][0][0][0] =     0.13281234; m_data_Endcap_TriggerST_Sigmas[0][11][0][0][0][1] =     0.00198762;  m_data_Endcap_TriggerST_Sigmas[0][11][0][0][1][0] =     0.33323734; m_data_Endcap_TriggerST_Sigmas[0][11][0][0][1][1] =    -0.00040997;
 m_data_Endcap_TriggerST_Sigmas[0][0][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][0][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][0][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][0][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][1][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][1][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][1][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][1][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][2][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][2][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][2][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][2][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][3][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][3][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][3][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][3][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][4][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][4][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][4][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][4][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][5][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][5][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][5][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][5][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][6][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][6][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][6][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][6][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][7][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][7][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][7][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][7][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][8][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][8][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][8][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][8][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][9][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][9][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][9][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][9][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][10][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][10][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][10][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][10][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][11][0][1][0][0] =     0.10881599; m_data_Endcap_TriggerST_Sigmas[0][11][0][1][0][1] =     0.00246415;  m_data_Endcap_TriggerST_Sigmas[0][11][0][1][1][0] =     0.36239266; m_data_Endcap_TriggerST_Sigmas[0][11][0][1][1][1] =    -0.00039195;
 m_data_Endcap_TriggerST_Sigmas[0][0][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][0][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][0][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][0][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][1][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][1][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][1][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][1][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][2][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][2][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][2][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][2][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][3][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][3][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][3][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][3][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][4][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][4][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][4][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][4][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][5][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][5][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][5][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][5][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][6][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][6][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][6][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][6][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][7][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][7][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][7][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][7][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][8][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][8][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][8][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][8][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][9][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][9][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][9][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][9][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][10][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][10][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][10][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][10][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][11][1][0][0][0] =     0.12531138; m_data_Endcap_TriggerST_Sigmas[0][11][1][0][0][1] =     0.00201682;  m_data_Endcap_TriggerST_Sigmas[0][11][1][0][1][0] =     0.37857238; m_data_Endcap_TriggerST_Sigmas[0][11][1][0][1][1] =    -0.00118625;
 m_data_Endcap_TriggerST_Sigmas[0][0][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][0][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][0][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][0][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][1][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][1][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][1][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][1][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][2][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][2][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][2][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][2][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][3][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][3][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][3][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][3][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][4][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][4][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][4][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][4][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][5][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][5][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][5][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][5][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][6][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][6][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][6][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][6][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][7][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][7][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][7][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][7][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][8][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][8][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][8][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][8][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][9][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][9][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][9][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][9][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][10][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][10][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][10][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][10][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[0][11][1][1][0][0] =     0.11425225; m_data_Endcap_TriggerST_Sigmas[0][11][1][1][0][1] =     0.00245888;  m_data_Endcap_TriggerST_Sigmas[0][11][1][1][1][0] =     0.33168570; m_data_Endcap_TriggerST_Sigmas[0][11][1][1][1][1] =     0.00033995;
 m_data_Endcap_TriggerST_Sigmas[1][0][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][0][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][0][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][0][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][1][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][1][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][1][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][1][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][2][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][2][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][2][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][2][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][3][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][3][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][3][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][3][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][4][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][4][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][4][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][4][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][5][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][5][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][5][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][5][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][6][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][6][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][6][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][6][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][7][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][7][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][7][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][7][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][8][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][8][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][8][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][8][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][9][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][9][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][9][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][9][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][10][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][10][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][10][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][10][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][11][0][0][0][0] =     0.12856069; m_data_Endcap_TriggerST_Sigmas[1][11][0][0][0][1] =     0.00119065;  m_data_Endcap_TriggerST_Sigmas[1][11][0][0][1][0] =     0.23895755; m_data_Endcap_TriggerST_Sigmas[1][11][0][0][1][1] =     0.00183274;
 m_data_Endcap_TriggerST_Sigmas[1][0][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][0][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][0][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][0][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][1][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][1][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][1][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][1][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][2][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][2][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][2][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][2][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][3][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][3][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][3][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][3][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][4][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][4][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][4][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][4][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][5][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][5][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][5][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][5][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][6][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][6][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][6][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][6][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][7][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][7][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][7][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][7][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][8][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][8][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][8][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][8][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][9][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][9][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][9][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][9][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][10][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][10][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][10][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][10][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][11][0][1][0][0] =     0.15032808; m_data_Endcap_TriggerST_Sigmas[1][11][0][1][0][1] =     0.00148648;  m_data_Endcap_TriggerST_Sigmas[1][11][0][1][1][0] =     0.22652846; m_data_Endcap_TriggerST_Sigmas[1][11][0][1][1][1] =     0.00305960;
 m_data_Endcap_TriggerST_Sigmas[1][0][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][0][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][0][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][0][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][1][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][1][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][1][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][1][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][2][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][2][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][2][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][2][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][3][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][3][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][3][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][3][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][4][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][4][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][4][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][4][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][5][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][5][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][5][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][5][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][6][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][6][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][6][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][6][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][7][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][7][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][7][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][7][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][8][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][8][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][8][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][8][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][9][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][9][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][9][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][9][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][10][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][10][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][10][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][10][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][11][1][0][0][0] =     0.13699295; m_data_Endcap_TriggerST_Sigmas[1][11][1][0][0][1] =     0.00169010;  m_data_Endcap_TriggerST_Sigmas[1][11][1][0][1][0] =     0.36554785; m_data_Endcap_TriggerST_Sigmas[1][11][1][0][1][1] =    -0.00035728;
 m_data_Endcap_TriggerST_Sigmas[1][0][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][0][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][0][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][0][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][1][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][1][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][1][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][1][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][2][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][2][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][2][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][2][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][3][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][3][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][3][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][3][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][4][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][4][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][4][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][4][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][5][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][5][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][5][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][5][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][6][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][6][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][6][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][6][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][7][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][7][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][7][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][7][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][8][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][8][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][8][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][8][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][9][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][9][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][9][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][9][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][10][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][10][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][10][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][10][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[1][11][1][1][0][0] =     0.15657617; m_data_Endcap_TriggerST_Sigmas[1][11][1][1][0][1] =     0.00057384;  m_data_Endcap_TriggerST_Sigmas[1][11][1][1][1][0] =     0.36567692; m_data_Endcap_TriggerST_Sigmas[1][11][1][1][1][1] =    -0.00075383;
 m_data_Endcap_TriggerST_Sigmas[2][0][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][0][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][0][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][0][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][1][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][1][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][1][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][1][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][2][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][2][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][2][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][2][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][3][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][3][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][3][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][3][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][4][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][4][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][4][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][4][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][5][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][5][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][5][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][5][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][6][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][6][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][6][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][6][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][7][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][7][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][7][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][7][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][8][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][8][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][8][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][8][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][9][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][9][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][9][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][9][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][10][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][10][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][10][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][10][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][11][0][0][0][0] =     0.08280142; m_data_Endcap_TriggerST_Sigmas[2][11][0][0][0][1] =     0.00379700;  m_data_Endcap_TriggerST_Sigmas[2][11][0][0][1][0] =     0.12491069; m_data_Endcap_TriggerST_Sigmas[2][11][0][0][1][1] =     0.00149252;
 m_data_Endcap_TriggerST_Sigmas[2][0][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][0][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][0][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][0][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][1][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][1][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][1][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][1][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][2][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][2][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][2][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][2][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][3][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][3][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][3][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][3][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][4][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][4][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][4][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][4][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][5][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][5][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][5][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][5][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][6][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][6][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][6][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][6][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][7][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][7][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][7][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][7][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][8][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][8][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][8][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][8][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][9][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][9][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][9][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][9][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][10][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][10][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][10][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][10][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][11][0][1][0][0] =     0.13244076; m_data_Endcap_TriggerST_Sigmas[2][11][0][1][0][1] =     0.00212945;  m_data_Endcap_TriggerST_Sigmas[2][11][0][1][1][0] =     0.09367422; m_data_Endcap_TriggerST_Sigmas[2][11][0][1][1][1] =     0.00172546;
 m_data_Endcap_TriggerST_Sigmas[2][0][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][0][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][0][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][0][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][1][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][1][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][1][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][1][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][2][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][2][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][2][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][2][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][3][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][3][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][3][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][3][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][4][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][4][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][4][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][4][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][5][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][5][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][5][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][5][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][6][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][6][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][6][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][6][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][7][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][7][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][7][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][7][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][8][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][8][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][8][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][8][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][9][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][9][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][9][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][9][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][10][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][10][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][10][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][10][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][11][1][0][0][0] =     0.14640195; m_data_Endcap_TriggerST_Sigmas[2][11][1][0][0][1] =     0.00135310;  m_data_Endcap_TriggerST_Sigmas[2][11][1][0][1][0] =     0.13339166; m_data_Endcap_TriggerST_Sigmas[2][11][1][0][1][1] =     0.00046171;
 m_data_Endcap_TriggerST_Sigmas[2][0][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][0][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][0][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][0][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][1][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][1][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][1][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][1][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][2][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][2][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][2][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][2][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][3][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][3][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][3][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][3][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][4][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][4][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][4][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][4][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][5][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][5][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][5][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][5][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][6][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][6][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][6][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][6][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][7][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][7][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][7][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][7][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][8][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][8][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][8][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][8][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][9][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][9][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][9][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][9][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][10][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][10][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][10][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][10][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[2][11][1][1][0][0] =    -0.01163394; m_data_Endcap_TriggerST_Sigmas[2][11][1][1][0][1] =     0.00739889;  m_data_Endcap_TriggerST_Sigmas[2][11][1][1][1][0] =     0.15848626; m_data_Endcap_TriggerST_Sigmas[2][11][1][1][1][1] =     0.00146797;
 m_data_Endcap_TriggerST_Sigmas[3][0][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][0][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][0][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][0][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][1][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][1][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][1][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][1][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][2][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][2][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][2][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][2][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][3][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][3][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][3][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][3][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][4][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][4][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][4][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][4][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][5][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][5][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][5][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][5][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][6][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][6][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][6][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][6][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][7][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][7][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][7][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][7][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][8][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][8][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][8][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][8][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][9][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][9][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][9][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][9][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][10][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][10][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][10][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][10][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][11][0][0][0][0] =     0.10413306; m_data_Endcap_TriggerST_Sigmas[3][11][0][0][0][1] =     0.00334968;  m_data_Endcap_TriggerST_Sigmas[3][11][0][0][1][0] =     0.15075245; m_data_Endcap_TriggerST_Sigmas[3][11][0][0][1][1] =     0.00026428;
 m_data_Endcap_TriggerST_Sigmas[3][0][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][0][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][0][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][0][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][1][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][1][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][1][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][1][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][2][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][2][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][2][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][2][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][3][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][3][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][3][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][3][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][4][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][4][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][4][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][4][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][5][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][5][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][5][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][5][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][6][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][6][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][6][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][6][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][7][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][7][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][7][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][7][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][8][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][8][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][8][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][8][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][9][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][9][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][9][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][9][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][10][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][10][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][10][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][10][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][11][0][1][0][0] =     0.08034411; m_data_Endcap_TriggerST_Sigmas[3][11][0][1][0][1] =     0.00396750;  m_data_Endcap_TriggerST_Sigmas[3][11][0][1][1][0] =     0.15655036; m_data_Endcap_TriggerST_Sigmas[3][11][0][1][1][1] =     0.00000012;
 m_data_Endcap_TriggerST_Sigmas[3][0][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][0][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][0][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][0][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][1][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][1][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][1][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][1][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][2][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][2][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][2][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][2][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][3][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][3][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][3][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][3][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][4][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][4][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][4][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][4][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][5][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][5][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][5][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][5][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][6][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][6][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][6][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][6][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][7][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][7][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][7][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][7][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][8][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][8][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][8][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][8][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][9][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][9][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][9][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][9][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][10][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][10][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][10][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][10][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][11][1][0][0][0] =     0.09289853; m_data_Endcap_TriggerST_Sigmas[3][11][1][0][0][1] =     0.00358122;  m_data_Endcap_TriggerST_Sigmas[3][11][1][0][1][0] =     0.13419320; m_data_Endcap_TriggerST_Sigmas[3][11][1][0][1][1] =     0.00075152;
 m_data_Endcap_TriggerST_Sigmas[3][0][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][0][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][0][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][0][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][1][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][1][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][1][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][1][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][2][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][2][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][2][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][2][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][3][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][3][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][3][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][3][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][4][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][4][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][4][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][4][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][5][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][5][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][5][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][5][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][6][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][6][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][6][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][6][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][7][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][7][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][7][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][7][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][8][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][8][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][8][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][8][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][9][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][9][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][9][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][9][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][10][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][10][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][10][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][10][1][1][1][1] =    -0.00003769;
 m_data_Endcap_TriggerST_Sigmas[3][11][1][1][0][0] =     0.10216539; m_data_Endcap_TriggerST_Sigmas[3][11][1][1][0][1] =     0.00344601;  m_data_Endcap_TriggerST_Sigmas[3][11][1][1][1][0] =     0.15786379; m_data_Endcap_TriggerST_Sigmas[3][11][1][1][1][1] =    -0.00003769;

//Endcaps - InnerST
 m_data_Endcap_InnerST_PhiSigmas[0][0][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][0][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][0][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][0][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][0][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][0][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][0][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][0][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][1][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][1][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][1][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][1][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][1][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][1][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][1][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][1][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][2][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][2][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][2][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][2][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][2][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][2][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][2][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][2][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][3][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][3][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][3][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][3][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][3][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][3][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][3][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][3][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][4][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][4][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][4][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][4][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][4][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][4][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][4][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][4][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][5][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][5][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][5][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][5][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][5][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][5][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][5][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][5][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][6][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][6][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][6][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][6][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][6][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][6][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][6][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][6][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][7][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][7][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][7][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][7][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][7][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][7][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][7][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][7][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][8][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][8][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][8][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][8][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][8][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][8][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][8][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][8][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][9][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][9][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][9][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][9][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][9][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][9][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][9][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][9][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][10][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][10][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][10][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][10][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][10][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][10][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][10][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][10][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[0][11][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[0][11][0][0][1]=0.00549303; 
 m_data_Endcap_InnerST_PhiSigmas[2][11][0][0][0]=0.07225676; m_data_Endcap_InnerST_PhiSigmas[2][11][0][0][1]=0.00549303; 
  m_data_Endcap_InnerST_Param[0][11][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[0][11][0][0][1]=-0.00504776; 
 m_data_Endcap_InnerST_Param[2][11][0][0][0]=-0.24234227; m_data_Endcap_InnerST_Param[2][11][0][0][1]=-0.00504776; 
  m_data_Endcap_InnerST_PhiSigmas[1][0][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][0][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][0][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][0][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][0][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][0][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][0][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][0][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][1][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][1][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][1][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][1][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][1][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][1][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][1][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][1][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][2][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][2][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][2][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][2][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][2][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][2][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][2][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][2][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][3][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][3][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][3][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][3][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][3][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][3][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][3][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][3][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][4][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][4][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][4][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][4][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][4][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][4][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][4][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][4][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][5][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][5][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][5][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][5][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][5][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][5][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][5][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][5][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][6][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][6][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][6][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][6][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][6][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][6][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][6][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][6][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][7][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][7][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][7][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][7][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][7][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][7][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][7][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][7][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][8][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][8][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][8][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][8][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][8][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][8][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][8][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][8][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][9][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][9][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][9][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][9][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][9][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][9][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][9][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][9][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][10][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][10][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][10][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][10][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][10][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][10][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][10][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][10][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[1][11][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[1][11][0][0][1]=0.00766950; 
 m_data_Endcap_InnerST_PhiSigmas[3][11][0][0][0]=0.04284341; m_data_Endcap_InnerST_PhiSigmas[3][11][0][0][1]=0.00766950; 
  m_data_Endcap_InnerST_Param[1][11][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[1][11][0][0][1]=-0.00276037; 
 m_data_Endcap_InnerST_Param[3][11][0][0][0]=-0.20331495; m_data_Endcap_InnerST_Param[3][11][0][0][1]=-0.00276037; 
  m_data_Endcap_InnerST_PhiSigmas[0][0][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][0][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][0][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][0][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][0][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][0][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][0][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][0][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][1][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][1][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][1][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][1][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][1][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][1][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][1][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][1][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][2][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][2][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][2][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][2][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][2][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][2][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][2][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][2][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][3][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][3][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][3][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][3][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][3][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][3][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][3][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][3][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][4][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][4][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][4][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][4][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][4][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][4][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][4][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][4][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][5][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][5][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][5][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][5][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][5][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][5][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][5][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][5][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][6][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][6][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][6][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][6][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][6][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][6][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][6][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][6][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][7][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][7][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][7][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][7][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][7][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][7][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][7][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][7][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][8][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][8][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][8][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][8][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][8][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][8][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][8][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][8][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][9][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][9][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][9][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][9][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][9][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][9][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][9][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][9][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][10][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][10][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][10][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][10][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][10][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][10][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][10][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][10][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[0][11][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[0][11][0][1][1]=0.00433204; 
 m_data_Endcap_InnerST_PhiSigmas[2][11][0][1][0]=0.10706679; m_data_Endcap_InnerST_PhiSigmas[2][11][0][1][1]=0.00433204; 
  m_data_Endcap_InnerST_Param[0][11][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[0][11][0][1][1]=0.00264936; 
 m_data_Endcap_InnerST_Param[2][11][0][1][0]=0.25237369; m_data_Endcap_InnerST_Param[2][11][0][1][1]=0.00264936; 
  m_data_Endcap_InnerST_PhiSigmas[1][0][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][0][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][0][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][0][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][0][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][0][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][0][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][0][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][1][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][1][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][1][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][1][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][1][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][1][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][1][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][1][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][2][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][2][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][2][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][2][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][2][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][2][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][2][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][2][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][3][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][3][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][3][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][3][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][3][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][3][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][3][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][3][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][4][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][4][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][4][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][4][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][4][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][4][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][4][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][4][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][5][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][5][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][5][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][5][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][5][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][5][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][5][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][5][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][6][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][6][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][6][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][6][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][6][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][6][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][6][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][6][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][7][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][7][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][7][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][7][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][7][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][7][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][7][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][7][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][8][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][8][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][8][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][8][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][8][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][8][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][8][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][8][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][9][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][9][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][9][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][9][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][9][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][9][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][9][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][9][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][10][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][10][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][10][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][10][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][10][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][10][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][10][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][10][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[1][11][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[1][11][0][1][1]=0.00753565; 
 m_data_Endcap_InnerST_PhiSigmas[3][11][0][1][0]=0.03874343; m_data_Endcap_InnerST_PhiSigmas[3][11][0][1][1]=0.00753565; 
  m_data_Endcap_InnerST_Param[1][11][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[1][11][0][1][1]=-0.00015519; 
 m_data_Endcap_InnerST_Param[3][11][0][1][0]=0.22437839; m_data_Endcap_InnerST_Param[3][11][0][1][1]=-0.00015519; 
  m_data_Endcap_InnerST_PhiSigmas[0][0][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][0][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][0][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][0][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][0][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][0][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][0][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][0][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][1][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][1][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][1][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][1][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][1][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][1][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][1][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][1][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][2][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][2][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][2][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][2][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][2][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][2][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][2][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][2][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][3][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][3][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][3][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][3][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][3][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][3][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][3][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][3][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][4][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][4][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][4][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][4][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][4][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][4][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][4][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][4][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][5][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][5][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][5][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][5][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][5][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][5][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][5][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][5][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][6][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][6][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][6][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][6][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][6][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][6][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][6][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][6][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][7][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][7][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][7][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][7][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][7][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][7][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][7][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][7][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][8][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][8][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][8][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][8][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][8][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][8][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][8][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][8][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][9][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][9][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][9][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][9][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][9][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][9][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][9][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][9][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][10][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][10][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][10][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][10][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][10][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][10][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][10][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][10][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[0][11][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[0][11][1][0][1]=0.00468037; 
 m_data_Endcap_InnerST_PhiSigmas[2][11][1][0][0]=0.09298708; m_data_Endcap_InnerST_PhiSigmas[2][11][1][0][1]=0.00468037; 
  m_data_Endcap_InnerST_Param[0][11][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[0][11][1][0][1]=-0.00447655; 
 m_data_Endcap_InnerST_Param[2][11][1][0][0]=-0.23349855; m_data_Endcap_InnerST_Param[2][11][1][0][1]=-0.00447655; 
  m_data_Endcap_InnerST_PhiSigmas[1][0][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][0][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][0][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][0][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][0][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][0][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][0][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][0][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][1][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][1][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][1][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][1][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][1][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][1][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][1][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][1][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][2][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][2][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][2][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][2][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][2][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][2][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][2][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][2][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][3][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][3][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][3][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][3][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][3][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][3][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][3][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][3][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][4][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][4][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][4][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][4][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][4][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][4][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][4][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][4][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][5][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][5][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][5][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][5][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][5][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][5][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][5][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][5][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][6][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][6][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][6][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][6][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][6][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][6][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][6][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][6][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][7][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][7][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][7][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][7][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][7][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][7][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][7][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][7][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][8][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][8][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][8][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][8][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][8][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][8][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][8][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][8][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][9][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][9][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][9][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][9][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][9][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][9][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][9][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][9][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][10][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][10][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][10][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][10][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][10][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][10][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][10][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][10][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[1][11][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[1][11][1][0][1]=0.00666912; 
 m_data_Endcap_InnerST_PhiSigmas[3][11][1][0][0]=0.04069992; m_data_Endcap_InnerST_PhiSigmas[3][11][1][0][1]=0.00666912; 
  m_data_Endcap_InnerST_Param[1][11][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[1][11][1][0][1]=-0.00089065; 
 m_data_Endcap_InnerST_Param[3][11][1][0][0]=-0.20728864; m_data_Endcap_InnerST_Param[3][11][1][0][1]=-0.00089065; 
  m_data_Endcap_InnerST_PhiSigmas[0][0][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][0][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][0][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][0][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][0][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][0][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][0][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][0][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][1][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][1][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][1][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][1][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][1][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][1][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][1][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][1][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][2][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][2][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][2][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][2][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][2][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][2][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][2][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][2][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][3][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][3][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][3][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][3][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][3][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][3][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][3][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][3][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][4][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][4][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][4][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][4][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][4][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][4][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][4][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][4][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][5][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][5][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][5][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][5][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][5][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][5][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][5][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][5][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][6][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][6][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][6][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][6][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][6][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][6][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][6][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][6][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][7][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][7][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][7][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][7][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][7][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][7][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][7][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][7][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][8][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][8][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][8][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][8][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][8][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][8][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][8][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][8][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][9][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][9][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][9][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][9][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][9][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][9][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][9][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][9][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][10][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][10][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][10][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][10][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][10][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][10][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][10][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][10][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[0][11][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[0][11][1][1][1]=0.00470356; 
 m_data_Endcap_InnerST_PhiSigmas[2][11][1][1][0]=0.09291424; m_data_Endcap_InnerST_PhiSigmas[2][11][1][1][1]=0.00470356; 
  m_data_Endcap_InnerST_Param[0][11][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[0][11][1][1][1]=0.00204247; 
 m_data_Endcap_InnerST_Param[2][11][1][1][0]=0.25992437; m_data_Endcap_InnerST_Param[2][11][1][1][1]=0.00204247; 
  m_data_Endcap_InnerST_PhiSigmas[1][0][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][0][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][0][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][0][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][0][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][0][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][0][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][0][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][1][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][1][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][1][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][1][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][1][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][1][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][1][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][1][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][2][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][2][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][2][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][2][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][2][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][2][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][2][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][2][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][3][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][3][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][3][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][3][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][3][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][3][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][3][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][3][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][4][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][4][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][4][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][4][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][4][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][4][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][4][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][4][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][5][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][5][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][5][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][5][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][5][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][5][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][5][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][5][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][6][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][6][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][6][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][6][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][6][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][6][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][6][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][6][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][7][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][7][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][7][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][7][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][7][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][7][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][7][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][7][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][8][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][8][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][8][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][8][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][8][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][8][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][8][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][8][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][9][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][9][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][9][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][9][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][9][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][9][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][9][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][9][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][10][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][10][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][10][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][10][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][10][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][10][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][10][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][10][1][1][1]=0.00096603; 
  m_data_Endcap_InnerST_PhiSigmas[1][11][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[1][11][1][1][1]=0.00709018; 
 m_data_Endcap_InnerST_PhiSigmas[3][11][1][1][0]=0.03121386; m_data_Endcap_InnerST_PhiSigmas[3][11][1][1][1]=0.00709018; 
  m_data_Endcap_InnerST_Param[1][11][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[1][11][1][1][1]=0.00096603; 
 m_data_Endcap_InnerST_Param[3][11][1][1][0]=0.21324215; m_data_Endcap_InnerST_Param[3][11][1][1][1]=0.00096603; 

 m_data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; m_data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; m_data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; m_data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; m_data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; m_data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; m_data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; m_data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; m_data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; m_data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; m_data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; m_data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; m_data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; m_data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; m_data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; m_data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; m_data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; m_data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; m_data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; m_data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; m_data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; m_data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; m_data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; m_data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; m_data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; m_data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; m_data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; m_data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; m_data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263; 
  m_data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; m_data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 m_data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; m_data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 m_data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; m_data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 m_data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; m_data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263;

}
