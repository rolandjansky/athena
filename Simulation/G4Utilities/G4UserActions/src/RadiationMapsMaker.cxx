/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RadiationMapsMaker.h"
#include <iostream>
#include <string>
#include <cmath>
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4PionZero.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4KaonZeroShort.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonZero.hh"
#include "G4AntiKaonZero.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Neutron.hh"
#include "G4AntiNeutron.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Lambda.hh"
#include "G4AntiLambda.hh"
#include "G4SigmaPlus.hh"
#include "G4AntiSigmaPlus.hh"
#include "G4SigmaMinus.hh"
#include "G4AntiSigmaMinus.hh"
#include "G4SigmaZero.hh"
#include "G4AntiSigmaZero.hh"
#include "G4AntiXiMinus.hh"
#include "G4XiZero.hh"
#include "G4AntiXiZero.hh"
#include "G4XiMinus.hh"
#include "G4AntiXiMinus.hh"
#include "G4OmegaMinus.hh"
#include "G4AntiOmegaMinus.hh"
#include "G4Gamma.hh"
#include "G4Alpha.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Geantino.hh"
#include "TGraph.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VSensitiveDetector.hh"
#include "PathResolver/PathResolver.h"

namespace G4UA{

  
  //----------------------------------------------------------------------------
  // Constructor
  //----------------------------------------------------------------------------
  RadiationMapsMaker::RadiationMapsMaker(const Config& config)
    : m_config(config)
  {
  }

  //---------------------------------------------------------------------------
  // Merge results
  //---------------------------------------------------------------------------
  void RadiationMapsMaker::Report::merge(const RadiationMapsMaker::Report& maps)
  {

    // all 2d vectors have same size 
    for(unsigned int i=0;i<maps.m_rz_tid.size();i++) {
      // zoom 2d
      m_rz_tid [i] += maps.m_rz_tid [i];
      m_rz_eion[i] += maps.m_rz_eion[i];
      m_rz_niel[i] += maps.m_rz_niel[i];
      m_rz_h20 [i] += maps.m_rz_h20 [i];
      m_rz_neut[i] += maps.m_rz_neut[i];
      m_rz_chad[i] += maps.m_rz_chad[i];

      // full 2d
      m_full_rz_tid [i] += maps.m_full_rz_tid [i];
      m_full_rz_eion[i] += maps.m_full_rz_eion[i];
      m_full_rz_niel[i] += maps.m_full_rz_niel[i];
      m_full_rz_h20 [i] += maps.m_full_rz_h20 [i];
      m_full_rz_neut[i] += maps.m_full_rz_neut[i];
      m_full_rz_chad[i] += maps.m_full_rz_chad[i];
    }

    for(unsigned int i=0;i<maps.m_rz_vol.size();i++) {
      // need volume fraction only if particular material is selected
      m_rz_vol [i] += maps.m_rz_vol [i];
      m_rz_norm[i] += maps.m_rz_norm[i];
      m_full_rz_vol [i] += maps.m_full_rz_vol [i];
      m_full_rz_norm[i] += maps.m_full_rz_norm[i];
    }

    // all zoom 3d vectors have same size 
    for(unsigned int i=0;i<maps.m_3d_tid.size();i++) {
      // zoom 3d
      m_3d_tid [i] += maps.m_3d_tid [i];
      m_3d_eion[i] += maps.m_3d_eion[i];
      m_3d_niel[i] += maps.m_3d_niel[i];
      m_3d_h20 [i] += maps.m_3d_h20 [i];
      m_3d_neut[i] += maps.m_3d_neut[i];
      m_3d_chad[i] += maps.m_3d_chad[i];
    }

    // all time 2d vectors have same size 
    for(unsigned int i=0;i<maps.m_rz_tid_time.size();i++) {
      // zoom 2d
      m_rz_tid_time [i] += maps.m_rz_tid_time [i];

      // full 2d
      m_full_rz_tid_time [i] += maps.m_full_rz_tid_time [i];
    }
    
    // all element fraction 2d vectors have same size
    for(unsigned int i=0;i<maps.m_rz_element.size();i++) {
      m_rz_element     [i] += maps.m_rz_element     [i];
      m_full_rz_element[i] += maps.m_full_rz_element[i];
    }

    for(unsigned int i=0;i<maps.m_3d_vol.size();i++) {
      // need volume fraction only if particular material is selected
      m_3d_vol [i] += maps.m_3d_vol [i];
      m_3d_norm[i] += maps.m_3d_norm[i];
    }

    // neutron spectra have different size from all other particle's spectra
    for(unsigned int i=0;i<maps.m_rz_neut_spec.size();i++) {
      m_rz_neut_spec     [i] += maps.m_rz_neut_spec     [i];
      m_full_rz_neut_spec[i] += maps.m_full_rz_neut_spec[i];
    }
    // x theta bins
    for(unsigned int i=0;i<maps.m_theta_full_rz_neut_spec.size();i++) {
      m_theta_full_rz_neut_spec[i] += maps.m_theta_full_rz_neut_spec[i];
    }
    // all other particle's spectra have same size
    for(unsigned int i=0;i<maps.m_rz_gamm_spec.size();i++) {
      m_rz_gamm_spec     [i] += maps.m_rz_gamm_spec     [i];
      m_full_rz_gamm_spec[i] += maps.m_full_rz_gamm_spec[i];
      m_rz_elec_spec     [i] += maps.m_rz_elec_spec     [i];
      m_full_rz_elec_spec[i] += maps.m_full_rz_elec_spec[i];
      m_rz_muon_spec     [i] += maps.m_rz_muon_spec     [i];
      m_full_rz_muon_spec[i] += maps.m_full_rz_muon_spec[i];
      m_rz_pion_spec     [i] += maps.m_rz_pion_spec     [i];
      m_full_rz_pion_spec[i] += maps.m_full_rz_pion_spec[i];
      m_rz_prot_spec     [i] += maps.m_rz_prot_spec     [i];
      m_full_rz_prot_spec[i] += maps.m_full_rz_prot_spec[i];
      m_rz_rest_spec     [i] += maps.m_rz_rest_spec     [i];
      m_full_rz_rest_spec[i] += maps.m_full_rz_rest_spec[i];
    }
    // x theta bins
    for(unsigned int i=0;i<maps.m_theta_full_rz_gamm_spec.size();i++) {
      m_theta_full_rz_gamm_spec[i]  += maps.m_theta_full_rz_gamm_spec[i];
      m_theta_full_rz_elec_spec[i]  += maps.m_theta_full_rz_elec_spec[i];
      m_theta_full_rz_muon_spec[i]  += maps.m_theta_full_rz_muon_spec[i];
      m_theta_full_rz_pion_spec[i]  += maps.m_theta_full_rz_pion_spec[i];
      m_theta_full_rz_prot_spec[i]  += maps.m_theta_full_rz_prot_spec[i];
      m_theta_full_rz_rchgd_spec[i] += maps.m_theta_full_rz_rchgd_spec[i];
      m_theta_full_rz_rneut_spec[i] += maps.m_theta_full_rz_rneut_spec[i];
    }
  }

  void RadiationMapsMaker::BeginOfRunAction(const G4Run*){

    // first make sure the vectors are empty

    m_maps.m_rz_tid .resize(0);
    m_maps.m_rz_eion.resize(0);
    m_maps.m_rz_niel.resize(0);
    m_maps.m_rz_h20 .resize(0);
    m_maps.m_rz_neut.resize(0);
    m_maps.m_rz_chad.resize(0);

    m_maps.m_full_rz_tid .resize(0);
    m_maps.m_full_rz_eion.resize(0);
    m_maps.m_full_rz_niel.resize(0);
    m_maps.m_full_rz_h20 .resize(0);
    m_maps.m_full_rz_neut.resize(0);
    m_maps.m_full_rz_chad.resize(0);

    m_maps.m_3d_tid .resize(0);
    m_maps.m_3d_eion.resize(0);
    m_maps.m_3d_niel.resize(0);
    m_maps.m_3d_h20 .resize(0);
    m_maps.m_3d_neut.resize(0);
    m_maps.m_3d_chad.resize(0);

    m_maps.m_rz_tid_time      .resize(0);
    m_maps.m_full_rz_tid_time .resize(0);

    m_maps.m_rz_element       .resize(0);
    m_maps.m_full_rz_element  .resize(0);

    m_maps.m_rz_neut_spec     .resize(0);
    m_maps.m_full_rz_neut_spec.resize(0);
    m_maps.m_rz_gamm_spec     .resize(0);
    m_maps.m_full_rz_gamm_spec.resize(0);
    m_maps.m_rz_elec_spec     .resize(0);
    m_maps.m_full_rz_elec_spec.resize(0);
    m_maps.m_rz_muon_spec     .resize(0);
    m_maps.m_full_rz_muon_spec.resize(0);
    m_maps.m_rz_pion_spec     .resize(0);
    m_maps.m_full_rz_pion_spec.resize(0);
    m_maps.m_rz_prot_spec     .resize(0);
    m_maps.m_full_rz_prot_spec.resize(0);
    m_maps.m_rz_rest_spec     .resize(0);
    m_maps.m_full_rz_rest_spec.resize(0);

    m_maps.m_theta_full_rz_neut_spec .resize(0);
    m_maps.m_theta_full_rz_gamm_spec .resize(0);
    m_maps.m_theta_full_rz_elec_spec .resize(0);
    m_maps.m_theta_full_rz_muon_spec .resize(0);
    m_maps.m_theta_full_rz_pion_spec .resize(0);
    m_maps.m_theta_full_rz_prot_spec .resize(0);
    m_maps.m_theta_full_rz_rchgd_spec.resize(0);
    m_maps.m_theta_full_rz_rneut_spec.resize(0);
    
    if (!m_config.material.empty()) {
      // need volume fraction only if particular material is selected
      m_maps.m_rz_vol .resize(0);
      m_maps.m_rz_norm.resize(0);
      m_maps.m_full_rz_vol .resize(0);
      m_maps.m_full_rz_norm.resize(0);
      m_maps.m_3d_vol .resize(0);
      m_maps.m_3d_norm.resize(0);
    }

    // then resize to proper size and initialize with 0's 

    m_maps.m_rz_tid .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_rz_eion.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_rz_niel.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_rz_h20 .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_rz_neut.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_rz_chad.resize(m_config.nBinsz*m_config.nBinsr,0.0);

    m_maps.m_full_rz_tid .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_full_rz_eion.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_full_rz_niel.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_full_rz_h20 .resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_full_rz_neut.resize(m_config.nBinsz*m_config.nBinsr,0.0);
    m_maps.m_full_rz_chad.resize(m_config.nBinsz*m_config.nBinsr,0.0);

    m_maps.m_3d_tid .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    m_maps.m_3d_eion.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    m_maps.m_3d_niel.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    m_maps.m_3d_h20 .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    m_maps.m_3d_neut.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    m_maps.m_3d_chad.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);

    m_maps.m_rz_tid_time      .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogT,0.0);
    m_maps.m_full_rz_tid_time .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogT,0.0);

    m_maps.m_rz_element       .resize(m_config.nBinsz*m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1),0.0);
    m_maps.m_full_rz_element  .resize(m_config.nBinsz*m_config.nBinsr*(m_config.elemZMax-m_config.elemZMin+1),0.0);

    m_maps.m_rz_neut_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEn,0.0);
    m_maps.m_full_rz_neut_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEn,0.0);
    m_maps.m_rz_gamm_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_gamm_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_rz_elec_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_elec_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_rz_muon_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_muon_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_rz_pion_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_pion_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_rz_prot_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_prot_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_rz_rest_spec     .resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_full_rz_rest_spec.resize(m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);

    m_maps.m_theta_full_rz_neut_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEn,0.0);
    m_maps.m_theta_full_rz_gamm_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_elec_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_muon_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_pion_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_prot_spec .resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_rchgd_spec.resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);
    m_maps.m_theta_full_rz_rneut_spec.resize(m_config.nBinsdphi*m_config.nBinstheta*m_config.nBinsz*m_config.nBinsr*m_config.nBinslogEo,0.0);

    if (!m_config.material.empty()) {
      // need volume fraction only if particular material is selected
      // 2d zoom
      m_maps.m_rz_vol .resize(m_config.nBinsz*m_config.nBinsr,0.0);
      m_maps.m_rz_norm.resize(m_config.nBinsz*m_config.nBinsr,0.0);
      // 2d full
      m_maps.m_full_rz_vol .resize(m_config.nBinsz*m_config.nBinsr,0.0);
      m_maps.m_full_rz_norm.resize(m_config.nBinsz*m_config.nBinsr,0.0);
      // 3d
      m_maps.m_3d_vol .resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
      m_maps.m_3d_norm.resize(m_config.nBinsz3d*m_config.nBinsr3d*m_config.nBinsphi3d,0.0);
    }

    /// data files for NIEL damage factors in Si
    ///
    /// The data resides in the share directory of the package and should not be
    /// changed. To prevent modification of the damage factors by accident
    /// the file names are not configurable
    ///
    std::string fpSiA = PathResolver::find_file("protons_Si_Gunnar_Summers.dat"   ,"DATAPATH");//E_kin < 15   MeV
    std::string fpSiB = PathResolver::find_file("protons_Si_Gunnar_Huhtinen.dat"  ,"DATAPATH");//E_kin > 15   MeV
    std::string fnSiA = PathResolver::find_file("neutrons_Si_Gunnar_Griffin.dat"  ,"DATAPATH");//E_kin < 20   MeV
    std::string fnSiB = PathResolver::find_file("neutrons_Si_Gunnar_Konobeyev.dat","DATAPATH");//E_kin > 20   MeV
    std::string fpiSi = PathResolver::find_file("pions_Si_Gunnar_Huhtinen.dat"    ,"DATAPATH");//E_kin > 15   MeV
    std::string feSi  = PathResolver::find_file("electrons_Si_Summers.dat"        ,"DATAPATH");//E_kin >  0.1 MeV 

    if ( !m_tgpSiA ) 
      m_tgpSiA = new TGraph(fpSiA.c_str()); //E_kin < 15   MeV
    if ( !m_tgpSiB ) 
      m_tgpSiB = new TGraph(fpSiB.c_str()); //E_kin > 15   MeV
    if ( !m_tgnSiA ) 
      m_tgnSiA = new TGraph(fnSiA.c_str()); //E_kin < 20   MeV
    if ( !m_tgnSiB ) 
      m_tgnSiB = new TGraph(fnSiB.c_str()); //E_kin > 20   MeV
    if ( !m_tgpiSi ) 
      m_tgpiSi = new TGraph(fpiSi.c_str()); //E_kin > 15   MeV
    if ( !m_tgeSi ) 
      m_tgeSi = new TGraph(feSi.c_str());   //E_kin >  0.1 MeV
  }
  
  void RadiationMapsMaker::UserSteppingAction(const G4Step* aStep){
    int pdgid = 10;
    if (aStep->GetTrack()->GetDefinition()==G4Gamma::Definition()){
      pdgid=0;
    } else if (aStep->GetTrack()->GetDefinition()==G4Proton::Definition()){
      pdgid=1;
    } else if (aStep->GetTrack()->GetDefinition()==G4PionPlus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4PionMinus::Definition()){
      pdgid=2;
    } else if(aStep->GetTrack()->GetDefinition()==G4MuonPlus::Definition() ||
	      aStep->GetTrack()->GetDefinition()==G4MuonMinus::Definition()){
      pdgid=3;
    } else if(aStep->GetTrack()->GetDefinition()==G4Electron::Definition() ||
	      aStep->GetTrack()->GetDefinition()==G4Positron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>0.5){
	pdgid=4; 
      } else {
	pdgid=5;
      }
    } else if(aStep->GetTrack()->GetDefinition()==G4Neutron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>10.){
	pdgid=6;
      } else {
	pdgid=7;
      }
    } else if (aStep->GetTrack()->GetDefinition()==G4KaonPlus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4KaonMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4KaonZeroShort::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4KaonZeroLong::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4KaonZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiKaonZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4PionZero::Definition()){
      pdgid=8; // particles not charged pions treated as charged pions for NIEL 

    } else if (aStep->GetTrack()->GetDefinition()==G4AntiProton::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiNeutron::Definition() || // note n-bar is treated as proton like in FLUKA ... o.k. for > 10 MeV ...
	       aStep->GetTrack()->GetDefinition()==G4Lambda::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiLambda::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4SigmaPlus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiSigmaPlus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4SigmaMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiSigmaMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4SigmaZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiSigmaZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4XiMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiXiMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4XiZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiXiZero::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4OmegaMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4AntiOmegaMinus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4Alpha::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4Deuteron::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4Triton::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4He3::Definition()){
      pdgid=9; // particles not protons treated as protons for NIEL 

    } else if (aStep->GetTrack()->GetDefinition()==G4Geantino::Definition()) {
      pdgid = 999; // geantinos are used for volume calculation
    } else if (!aStep->GetTrack()->GetDefinition()->GetPDGCharge()) return; // Not one of those and not a primary?

    if ( pdgid == 10 && aStep->GetTrack()->GetKineticEnergy() > 10 ) {
      // check Z for heavy ions with more than 10 MeV to also treat those for NIEL
      if ( aStep->GetTrack()->GetDefinition()->GetAtomicNumber() > 1 ) {
	pdgid = 9;
      }
    }

    // process spectra, NIEL, h20, Edep, NEUT and CHAD particles only

    if ( pdgid == 0 || pdgid == 3 || /* spectra */ 
	 pdgid == 1 || pdgid == 2 || pdgid == 4 || pdgid == 5 || pdgid == 6 || pdgid == 7 || pdgid == 8 || pdgid == 9 || /* NIEL & h20*/
	 aStep->GetTotalEnergyDeposit() > 0 || pdgid == 999) {
      
      double absq = fabs(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
    
      double rho = aStep->GetTrack()->GetMaterial()->GetDensity()/CLHEP::g*CLHEP::cm3; 

      const G4Material * theMaterial = aStep->GetTrack()->GetMaterial();

      // get time of step as average between pre- and post-step point
      G4StepPoint* pre_step_point = aStep->GetPreStepPoint();
      G4StepPoint* post_step_point = aStep->GetPostStepPoint();
      const G4ThreeVector& startPoint = pre_step_point->GetPosition();
      const G4ThreeVector& endPoint   = post_step_point->GetPosition();
      G4ThreeVector p = (startPoint + endPoint) * 0.5;

      // process upper hemisphere only in case PositiveYOnly is true
      if ( m_config.posYOnly && p.y() < 0 ) return;

      double timeOfFlight = (pre_step_point->GetGlobalTime() +
			     post_step_point->GetGlobalTime()) * 0.5;

      // then compute time difference to a particle traveling with speed of light until this position
      double deltatime = (timeOfFlight - p.mag()/CLHEP::c_light)/CLHEP::s;

      // and use the log10 of that time difference in seconds to fill time-dependent histograms
      // note that the upper bin boundary is the relevant cut. The first bin contains thus all times even shorter than the first lower bin boundary
      double logt = (deltatime<0?m_config.logTMin-1:log10(deltatime));
      
      bool goodMaterial(false);

      if (m_config.material.empty()) {
	// if no material is specified maps are made for all materials
	goodMaterial = true;
      } 
      else {
	// if a material is specified maps are made just for that one.
	// Note that still all steps need to be treated to calculate the
	// volume fraction of the target material in each bin!
	goodMaterial = (m_config.material.compare(aStep->GetTrack()->GetMaterial()->GetName()) == 0);
      } 

      // fluence dN/da = dl/dV 
      double x0 = aStep->GetPreStepPoint()->GetPosition().x()*0.1;
      double x1 = aStep->GetPostStepPoint()->GetPosition().x()*0.1;
      double y0 = aStep->GetPreStepPoint()->GetPosition().y()*0.1;
      double y1 = aStep->GetPostStepPoint()->GetPosition().y()*0.1;
      double z0 = aStep->GetPreStepPoint()->GetPosition().z()*0.1;
      double z1 = aStep->GetPostStepPoint()->GetPosition().z()*0.1;

      double l = sqrt(pow(x1-x0,2)+pow(y1-y0,2)+pow(z1-z0,2));
      // make 1 mm steps but at least 1 step
      double dl0 = 0.1;
      unsigned int nStep = l/dl0+1;
      double dx = (x1-x0)/nStep;
      double dy = (y1-y0)/nStep;
      double dz = (z1-z0)/nStep;
      double dl = l/nStep;
      
      double weight = 0; // weight for NIEL
      double eKin = aStep->GetTrack()->GetKineticEnergy();
      double theta = aStep->GetTrack()->GetMomentumDirection().theta()*180./M_PI;
      // if theta range in configuration is 0 - 90 assume that 180-theta should
      // be used for theta > 90; otherwise leave theta unchanged
      theta = (theta > 90&&m_config.thetaMin==0&&m_config.thetaMax==90?180-theta:theta);
      double dphi = (aStep->GetTrack()->GetMomentumDirection().phi()-p.phi())*180./M_PI;
      while ( dphi > 360 ) dphi -= 360.;
      while ( dphi <   0 ) dphi += 360.;
      
      double logEKin = (eKin > 0?log10(eKin):(m_config.logEMinn<m_config.logEMino?m_config.logEMinn:m_config.logEMino)-1);

      if ( pdgid == 1 || pdgid == 9 ) {
	if ( eKin < 15 ) {
	  if ( eKin > 10 ) {
	    weight = m_tgpSiA->Eval(eKin);
	  }
	}
	else {
	  weight = m_tgpSiB->Eval(eKin);
	}
      }
      else if ( pdgid == 2 || pdgid == 8 ) {
	if ( eKin > 15 ) {
	  weight = m_tgpiSi->Eval(eKin);
	}
      }
      else if ( pdgid == 6 || pdgid == 7 ) {
	if ( eKin < 20 ) {
	  weight = m_tgnSiA->Eval(eKin);
	}
	else {
	  weight = m_tgnSiB->Eval(eKin);
	}
      }
      else if ( pdgid == 4 || pdgid == 5 ) {
	if ( eKin > 0.1 ) {
	  weight = m_tgeSi->Eval(eKin);
	}
      }
      
      double dE_TOT = aStep->GetTotalEnergyDeposit()/nStep;
      double dE_NIEL = aStep->GetNonIonizingEnergyDeposit()/nStep;
      double dE_ION = dE_TOT-dE_NIEL;

      for(unsigned int i=0;i<nStep;i++) {
	double abszorz = z0+dz*(i+0.5);
	// if |z| instead of z take abs value
	if ( m_config.zMinFull >= 0 ) abszorz = fabs(abszorz);

	double rr = sqrt(pow(x0+dx*(i+0.5),2)+
			 pow(y0+dy*(i+0.5),2));
	double pphi = atan2(y0+dy*(i+0.5),x0+dx*(i+0.5))*180/M_PI;

	int vBinZoom      = -1;
	int vBinFull      = -1;
	int vBin3d        = -1;
	int vBinZoomSpecn = -1;
	int vBinFullSpecn = -1;
	int vBinZoomSpeco = -1;
	int vBinFullSpeco = -1;
	int vBinZoomTime  = -1;
	int vBinFullTime  = -1;
	int vBinThetaFullSpecn = -1;
	int vBinThetaFullSpeco = -1;
	
	// zoom 2d
	if ( m_config.zMinZoom < abszorz && 
	     m_config.zMaxZoom > abszorz ) {
	  int iz = (abszorz-m_config.zMinZoom)/(m_config.zMaxZoom-m_config.zMinZoom)*m_config.nBinsz;
	  if ( m_config.rMinZoom < rr && 
	       m_config.rMaxZoom > rr ) {
	    int ir = (rr-m_config.rMinZoom)/(m_config.rMaxZoom-m_config.rMinZoom)*m_config.nBinsr;
	    vBinZoom = m_config.nBinsr*iz+ir;
	    if ( m_config.logTMax > logt ){
	      int ilt = (logt < m_config.logTMin?0:(logt-m_config.logTMin)/(m_config.logTMax-m_config.logTMin)*m_config.nBinslogT);
	      vBinZoomTime = m_config.nBinsr*m_config.nBinslogT*iz+ir*m_config.nBinslogT+ilt;
	    }
	    if ( m_config.logEMinn < logEKin && 
		 m_config.logEMaxn > logEKin &&
		 (pdgid == 6 || pdgid == 7)) {
	      int ile = (logEKin-m_config.logEMinn)/(m_config.logEMaxn-m_config.logEMinn)*m_config.nBinslogEn;
	      vBinZoomSpecn = m_config.nBinsr*m_config.nBinslogEn*iz+ir*m_config.nBinslogEn+ile;
	    }
	    if ( m_config.logEMino < logEKin && 
		 m_config.logEMaxo > logEKin &&
		 pdgid != 6 && pdgid != 7) {
	      int ile = (logEKin-m_config.logEMino)/(m_config.logEMaxo-m_config.logEMino)*m_config.nBinslogEo;
	      vBinZoomSpeco = m_config.nBinsr*m_config.nBinslogEo*iz+ir*m_config.nBinslogEo+ile;
	    }
	  }
	}
	
	// full 2d
	if ( m_config.zMinFull < abszorz && 
	     m_config.zMaxFull > abszorz ) {
	  int iz = (abszorz-m_config.zMinFull)/(m_config.zMaxFull-m_config.zMinFull)*m_config.nBinsz;
	  if ( m_config.rMinFull < rr && 
	       m_config.rMaxFull > rr ) {
	    int ir = (rr-m_config.rMinFull)/(m_config.rMaxFull-m_config.rMinFull)*m_config.nBinsr;
	    vBinFull = m_config.nBinsr*iz+ir;
	    if ( m_config.logTMax > logt ){
	      int ilt = (logt < m_config.logTMin?0:(logt-m_config.logTMin)/(m_config.logTMax-m_config.logTMin)*m_config.nBinslogT);
	      vBinFullTime = m_config.nBinsr*m_config.nBinslogT*iz+ir*m_config.nBinslogT+ilt;
	    }
	    if ( m_config.logEMinn < logEKin && 
		 m_config.logEMaxn > logEKin &&
		 (pdgid == 6 || pdgid == 7)) {
	      int ile = (logEKin-m_config.logEMinn)/(m_config.logEMaxn-m_config.logEMinn)*m_config.nBinslogEn;
	      vBinFullSpecn = m_config.nBinsr*m_config.nBinslogEn*iz+ir*m_config.nBinslogEn+ile;
	      if ( m_config.thetaMin < theta && 
		   m_config.thetaMax > theta ) {
		int ith = (theta-m_config.thetaMin)/(m_config.thetaMax-m_config.thetaMin)*m_config.nBinstheta;
		int idph = dphi/360.*m_config.nBinsdphi;
		ith = ith*m_config.nBinsdphi+idph;
		vBinThetaFullSpecn = m_config.nBinsr*m_config.nBinslogEn*m_config.nBinsdphi*m_config.nBinstheta*iz+ir*m_config.nBinslogEn*m_config.nBinsdphi*m_config.nBinstheta+ile*m_config.nBinsdphi*m_config.nBinstheta+ith;
	      }
	    }
	    if ( m_config.logEMino < logEKin && 
		 m_config.logEMaxo > logEKin &&
		 pdgid != 6 && pdgid != 7) {
	      int ile = (logEKin-m_config.logEMino)/(m_config.logEMaxo-m_config.logEMino)*m_config.nBinslogEo;
	      vBinFullSpeco = m_config.nBinsr*m_config.nBinslogEo*iz+ir*m_config.nBinslogEo+ile;
	      if ( m_config.thetaMin < theta && 
		   m_config.thetaMax > theta ) {
		int ith = (theta-m_config.thetaMin)/(m_config.thetaMax-m_config.thetaMin)*m_config.nBinstheta;
		int idph = dphi/360.*m_config.nBinsdphi;
		ith = ith*m_config.nBinsdphi+idph;
		vBinThetaFullSpeco = m_config.nBinsr*m_config.nBinslogEo*m_config.nBinsdphi*m_config.nBinstheta*iz+ir*m_config.nBinslogEo*m_config.nBinsdphi*m_config.nBinstheta+ile*m_config.nBinsdphi*m_config.nBinstheta+ith;
	      }
	    }
	  }
	}
	
	// zoom 3d
	if ( m_config.zMinZoom < abszorz && 
	     m_config.zMaxZoom > abszorz ) {
	  int iz = (abszorz-m_config.zMinZoom)/(m_config.zMaxZoom-m_config.zMinZoom)*m_config.nBinsz3d;
	  if ( m_config.rMinZoom < rr && 
	       m_config.rMaxZoom > rr ) {
	    int ir = (rr-m_config.rMinZoom)/(m_config.rMaxZoom-m_config.rMinZoom)*m_config.nBinsr3d;
	    if ( m_config.phiMinZoom == 0) {
	      // assume that all phi should be mapped to the selected phi range
	      double phi_mapped = pphi;
	      // first use phi range from 0 - 360 degrees
	      if (phi_mapped < 0) {
		phi_mapped = 360 + phi_mapped;
	      }
	      // then map to selected phi-range
	      int iphi = phi_mapped/m_config.phiMaxZoom;
	      phi_mapped -= iphi*m_config.phiMaxZoom;
	      iphi = phi_mapped/m_config.phiMaxZoom*m_config.nBinsphi3d;
	      vBin3d = m_config.nBinsr3d*m_config.nBinsphi3d*iz+m_config.nBinsphi3d*ir+iphi;
	    }
	    else if ( m_config.phiMinZoom < pphi && 
		      m_config.phiMaxZoom > pphi ) {
	      int iphi = (pphi-m_config.phiMinZoom)/(m_config.phiMaxZoom-m_config.phiMinZoom)*m_config.nBinsphi3d;
	      vBin3d = m_config.nBinsr3d*m_config.nBinsphi3d*iz+m_config.nBinsphi3d*ir+iphi;
	    }
	  }
	}
	// TID & EION
	if ( goodMaterial && vBinZoom >=0 ) {
	  if ( pdgid == 999 ) {
	    m_maps.m_rz_tid [vBinZoom] += dl;
	    m_maps.m_rz_eion[vBinZoom] += rho*dl;
	    for (size_t ie=0;ie<theMaterial->GetNumberOfElements();ie++ ) {
	      const G4Element * theElement = theMaterial->GetElement (ie);
	      const double mFrac = theMaterial->GetFractionVector()[ie];
	      const int zElem = theElement->GetZ();
	      if ( zElem >= m_config.elemZMin && 
		   zElem <= m_config.elemZMax) {
		m_maps.m_rz_element[vBinZoom*(m_config.elemZMax-m_config.elemZMin+1)+zElem-m_config.elemZMin] += rho*dl*mFrac;
	      }
	    }
	  }
	  else {
	    m_maps.m_rz_tid [vBinZoom] += dE_ION/rho;
	    m_maps.m_rz_eion[vBinZoom] += dE_ION;
	    for (size_t ie=0;ie<theMaterial->GetNumberOfElements();ie++ ) {
	      const G4Element * theElement = theMaterial->GetElement (ie);
	      const double mFrac = theMaterial->GetFractionVector()[ie];
	      const int zElem = theElement->GetZ();
	      if ( zElem >= m_config.elemZMin && 
		   zElem <= m_config.elemZMax) {
		m_maps.m_rz_element[vBinZoom*(m_config.elemZMax-m_config.elemZMin+1)+zElem-m_config.elemZMin] += dE_ION*mFrac;
	      }
	    }
	  }
	}
	if ( goodMaterial && vBinZoomTime >=0 ) {
	    m_maps.m_rz_tid_time[vBinZoomTime] += dE_ION/rho;
	}
	if ( goodMaterial && vBinFull >=0 ) {
	  if ( pdgid == 999 ) {
	    m_maps.m_full_rz_tid [vBinFull] += dl;
	    m_maps.m_full_rz_eion[vBinFull] += rho*dl;
	    for (size_t ie=0;ie<theMaterial->GetNumberOfElements();ie++ ) {
	      const G4Element * theElement = theMaterial->GetElement (ie);
	      const double mFrac = theMaterial->GetFractionVector()[ie];
	      const int zElem = theElement->GetZ();
	      if ( zElem >= m_config.elemZMin && 
		   zElem <= m_config.elemZMax) {
		m_maps.m_full_rz_element[vBinFull*(m_config.elemZMax-m_config.elemZMin+1)+zElem-m_config.elemZMin] += rho*dl*mFrac;
	      }
	    }
	  }
	  else {
	    m_maps.m_full_rz_tid [vBinFull] += dE_ION/rho;
	    m_maps.m_full_rz_eion[vBinFull] += dE_ION;
	    for (size_t ie=0;ie<theMaterial->GetNumberOfElements();ie++ ) {
	      const G4Element * theElement = theMaterial->GetElement (ie);
	      const double mFrac = theMaterial->GetFractionVector()[ie];
	      const int zElem = theElement->GetZ();
	      if ( zElem >= m_config.elemZMin && 
		   zElem <= m_config.elemZMax) {
		m_maps.m_full_rz_element[vBinFull*(m_config.elemZMax-m_config.elemZMin+1)+zElem-m_config.elemZMin] += dE_ION*mFrac;
	      }
	    }
	  }
	}
	if ( goodMaterial && vBinFullTime >=0 ) {
	    m_maps.m_full_rz_tid_time[vBinFullTime] += dE_ION/rho;
	}
	if ( goodMaterial && vBin3d >=0 ) {
	  if ( pdgid == 999 ) {
	    m_maps.m_3d_tid [vBin3d] += dl;
	    m_maps.m_3d_eion[vBin3d] += rho*dl;
	  }
	  else {
	    m_maps.m_3d_tid [vBin3d] += dE_ION/rho;
	    m_maps.m_3d_eion[vBin3d] += dE_ION;
	  }
	}
	
	if ( goodMaterial && (pdgid == 1 || pdgid == 2 || pdgid == 4 || pdgid == 5 || pdgid == 6 || pdgid == 7 || pdgid == 8 || pdgid == 9 )) {
	  // NIEL
	  if ( weight > 0 ) {
	    if ( vBinZoom >=0 ) {
	      m_maps.m_rz_niel [vBinZoom] += weight*dl;
	    }
	    if ( vBinFull >=0 ) {
	      m_maps.m_full_rz_niel [vBinFull] += weight*dl;
	    }
	    if ( vBin3d >=0 ) {
	      m_maps.m_3d_niel [vBin3d] += weight*dl;
	    }
	  }
	  // SEE
	  if ( eKin > 20 && (pdgid == 1 || pdgid == 2 || pdgid == 6 || pdgid == 7 || pdgid == 8 || pdgid == 9) ) {
	    if ( vBinZoom >=0 ) {
	      m_maps.m_rz_h20 [vBinZoom] += dl;
	    }
	    if ( vBinFull >=0 ) {
	      m_maps.m_full_rz_h20 [vBinFull] += dl;
	    }
	    if ( vBin3d >=0 ) {
	      m_maps.m_3d_h20 [vBin3d] += dl;
	    }
	  }
	  // NEUT
	  if ( eKin > 0.1 && (pdgid == 6 || pdgid == 7 ) ) {
	    if ( vBinZoom >=0 ) {
	      m_maps.m_rz_neut [vBinZoom] += dl;
	    }
	    if ( vBinFull >=0 ) {
	      m_maps.m_full_rz_neut [vBinFull] += dl;
	    }
	    if ( vBin3d >=0 ) {
	      m_maps.m_3d_neut [vBin3d] += dl;
	    }
	  }
	  // CHAD
	  if ( absq > 0 && (pdgid == 1 || pdgid == 2 || pdgid == 8 || pdgid == 9 ) ) {
	    if ( vBinZoom >=0 ) {
	      m_maps.m_rz_chad [vBinZoom] += dl;
	    }
	    if ( vBinFull >=0 ) {
	      m_maps.m_full_rz_chad [vBinFull] += dl;
	    }
	    if ( vBin3d >=0 ) {
	      m_maps.m_3d_chad [vBin3d] += dl;
	    }
	  }
	  // Neutron Energy Spectra
	  if ( pdgid == 6 || pdgid == 7 ) {
	    if ( vBinZoomSpecn >=0 ) {
	      m_maps.m_rz_neut_spec [vBinZoomSpecn] += dl;
	    }
	    if ( vBinFullSpecn >=0 ) {
	      m_maps.m_full_rz_neut_spec [vBinFullSpecn] += dl;
	    }
	    if ( vBinThetaFullSpecn >=0 ) {
	      m_maps.m_theta_full_rz_neut_spec [vBinThetaFullSpecn] += dl;
	    }
	  }
	}
	
	if ( goodMaterial && (pdgid < 6 || pdgid > 7 )){
	  // Other particle Energy Spectra
	  if ( vBinZoomSpeco >=0 ) {
	    if ( pdgid == 0 ) {
	      m_maps.m_rz_gamm_spec [vBinZoomSpeco] += dl;
	    }
	    else if ( pdgid == 1 ) {
	      m_maps.m_rz_prot_spec [vBinZoomSpeco] += dl;
	    }
	    else if ( pdgid == 2 ) {
	      m_maps.m_rz_pion_spec [vBinZoomSpeco] += dl;
	    }
	    else if ( pdgid == 3 ) {
	      m_maps.m_rz_muon_spec [vBinZoomSpeco] += dl;
	    }
	    else if ( pdgid == 4 || pdgid == 5 ) {
	      m_maps.m_rz_elec_spec [vBinZoomSpeco] += dl;
	    }
	    else {
	      m_maps.m_rz_rest_spec [vBinZoomSpeco] += dl;
	    }
	  }
	  if ( vBinFullSpeco >=0 ) {
	    if ( pdgid == 0 ) {
	      m_maps.m_full_rz_gamm_spec [vBinFullSpeco] += dl;
	    }
	    else if ( pdgid == 1 ) {
	      m_maps.m_full_rz_prot_spec [vBinFullSpeco] += dl;
	    }
	    else if ( pdgid == 2 ) {
	      m_maps.m_full_rz_pion_spec [vBinFullSpeco] += dl;
	    }
	    else if ( pdgid == 3 ) {
	      m_maps.m_full_rz_muon_spec [vBinFullSpeco] += dl;
	    }
	    else if ( pdgid == 4 || pdgid == 5 ) {
	      m_maps.m_full_rz_elec_spec [vBinFullSpeco] += dl;
	    }
	    else {
	      m_maps.m_full_rz_rest_spec [vBinFullSpeco] += dl;
	    }
	  }
	  if ( vBinThetaFullSpeco >=0 ) {
	    if ( pdgid == 0 ) {
	      m_maps.m_theta_full_rz_gamm_spec [vBinThetaFullSpeco] += dl;
	    }
	    else if ( pdgid == 1 ) {
	      m_maps.m_theta_full_rz_prot_spec [vBinThetaFullSpeco] += dl;
	    }
	    else if ( pdgid == 2 ) {
	      m_maps.m_theta_full_rz_pion_spec [vBinThetaFullSpeco] += dl;
	    }
	    else if ( pdgid == 3 ) {
	      m_maps.m_theta_full_rz_muon_spec [vBinThetaFullSpeco] += dl;
	    }
	    else if ( pdgid == 4 || pdgid == 5 ) {
	      m_maps.m_theta_full_rz_elec_spec [vBinThetaFullSpeco] += dl;
	    }
	    else {
	      if ( absq > 0 ) {
		m_maps.m_theta_full_rz_rchgd_spec [vBinThetaFullSpeco] += dl;
	      }
	      else {
		m_maps.m_theta_full_rz_rneut_spec [vBinThetaFullSpeco] += dl;
	      }
	    }
	  }
	}
	
	if (!m_config.material.empty()) {
	  // need volume fraction only if particular material is selected
	  if ( (eKin > 1 && (pdgid == 6 || pdgid == 7)) || pdgid == 999) {
	    // count all neutron > 1 MeV track lengths weighted by r
	    // to get norm for volume per bin. High energetic
	    // neutrons are used because they travel far enough to
	    // map entire bins and are not bent by magnetic fields.
	    // dl is a measure of length inside the current bin.
	    // The multiplication by r accounts for the larger
	    // volume corresponding to larger r assuming that the
	    // neutron flux is locally mainly from inside to
	    // outside. In regions where the neutron flux differs
	    // substantially from this cylindrical assumption a
	    // cylindrical Geantino scan (vertex: flat in z, x=y=0;
	    // momentum: pz=0, flat in phi) should be used to get
	    // the correct volume fraction.
	    if ( vBinZoom >=0 ) {
	      m_maps.m_rz_norm[vBinZoom] += rr*dl;
	    }
	    if ( vBinFull >=0 ) {
	      m_maps.m_full_rz_norm[vBinFull] += rr*dl;
	    }
	    if ( vBin3d >=0 ) {
	      m_maps.m_3d_norm[vBin3d] += rr*dl;
	    }
	    if ( goodMaterial ) {
	      // same but only inside the material of interest. 
	      // The ratio vol/norm gives the volume fraction of the desired
	      // material inside the current bin
	      if ( vBinZoom >=0 ) {
		m_maps.m_rz_vol [vBinZoom] += rr*dl;
	      }
	      if ( vBinFull >=0 ) {
		m_maps.m_full_rz_vol [vBinFull] += rr*dl;
	      }
	      if ( vBin3d >=0 ) {
		m_maps.m_3d_vol [vBin3d] += rr*dl;
	      }
	    }
	  }
	}
      }
    }
  }
} // namespace G4UA 
  
