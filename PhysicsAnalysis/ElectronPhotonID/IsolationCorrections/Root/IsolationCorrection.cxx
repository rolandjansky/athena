/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for the ROOT tool of this package 
#include "IsolationCorrections/IsolationCorrection.h"
#include "TFile.h"
// xAOD
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODPrimitives/IsolationHelpers.h"

#include "PathResolver/PathResolver.h"

namespace{
const float GeV(1000);
}

namespace CP { 

  IsolationCorrection::IsolationCorrection()
    : m_tool_ver(REL18), 
      m_nBinsEtaFine(10), 
      m_nBinsEtaCoarse(5), 
      m_corr_file(""),
      m_is_mc(false),
      m_set_mc(false),
      m_shower(0)
  {
  }

  void IsolationCorrection::SetCorrectionFile(std::string corr_file){
    m_corr_file = PathResolverFindCalibFile(corr_file);
  }

  StatusCode IsolationCorrection::initialize() {
    setEtaBins();
    setIsolCorr();
    m_shower = new CP::ShowerDepthTool();
    m_shower->initialize();

    if(!m_set_mc){
      m_is_mc = true;
      std::cerr<<"Data or MC not specified, assuming MC ...\n";
    }

    return StatusCode::SUCCESS;
  }

  void IsolationCorrection::SetToolVer(CP::IsolationCorrection::Version ver){
    m_tool_ver = ver;
  }

  StatusCode IsolationCorrection::finalize() {
    return StatusCode::SUCCESS;
  }

  float IsolationCorrection::GetPtCorrectedIsolation(const xAOD::Egamma& input, const xAOD::Iso::IsolationType isol){
    float corrected_isolation = input.isolationValue(isol) - GetPtCorrection(input, isol);
    return corrected_isolation;
  }


  float IsolationCorrection::GetPtCorrection(const xAOD::Egamma& input, const xAOD::Iso::IsolationType isol){
    /*
      - nPV: number of primary vertices (corrections were derived requiring >= 2 tracks/vertex in 2011 data)
    
      - Etcone40: (ph_Etcone40) *** in MeV!
      - Etcone40_ED_corrected: ED corrected version of Etcone40 (ph_Etcone40_ED_corrected) *** in Mev!
    
      - energy: cluster energy (ph_cl_E) *** in MeV!
      - etaS2: eta in the 2nd calo sampling (ph_etas2)
      - etaPointing: pointing eta -> from 1st and 2nd calo samplings (ph_etap)
      - etaCluster: cluster eta (ph_cl_eta)
      - radius: radius of the input EtconeXX variable.  Can be given as, e.g., .20 or 20
      - is_mc: set to true if running on MC
      - Etcone_value: value of uncorrected EtconeXX variable (ph_EtconeXX) *** in MeV!
      - isConversion: photons only: conversion flag (ph_isConv)
      - parttype: ELECTRON or PHOTON, enum defined below
      - version: REL18, REL17_2, REL17 or REL16, enum defined below
    */
    float isolation_ptcorrection = 0;
    float energy = input.e();
    bool is_mc = m_is_mc;
    ParticleType part_type = ( (input.type() == xAOD::Type::Electron) ?  
			       IsolationCorrection::ELECTRON : IsolationCorrection::PHOTON);    
    bool convFlag = false;
    int convFlag_int = 0;
    if(part_type == IsolationCorrection::PHOTON) convFlag_int = xAOD::EgammaHelpers::conversionType((const xAOD::Photon_v1*) &input);
    if(convFlag_int != 0) convFlag = true;
    
    float etaS1 = input.caloCluster()->etaBE(1);
    float etaS2 = input.caloCluster()->etaBE(2);
    float etaCluster = input.caloCluster()->eta();
    float phiCluster = input.caloCluster()->phi();
    
    float etaPointing = m_shower->getCaloPointingEta(etaS1, etaS2, phiCluster, is_mc);
    
    float radius = xAOD::Iso::coneSize(isol);
    bool is_topo = xAOD::Iso::isolationFlavour(isol) == xAOD::Iso::topoetcone ? true : false;

    if(is_topo){
      isolation_ptcorrection = GetPtCorrectionTopo(  energy,
						     etaS2,
						     etaPointing,
						     etaCluster,
						     radius,
						     is_mc,
						     convFlag,
						     part_type,
						     m_tool_ver);
    }else{
      isolation_ptcorrection = GetPtCorrection(  energy,
						 etaS2,
						 etaPointing,
						 etaCluster,
						 radius,
						 is_mc,
						 convFlag,
						 part_type,
						 m_tool_ver);
    }
    return isolation_ptcorrection;
  }

  float IsolationCorrection::GetEtaPointing(const xAOD::Egamma* input){
    float etaS1 = input->caloCluster()->etaBE(1);
    float etaS2 = input->caloCluster()->etaBE(2);
    float phiCluster = input->caloCluster()->phi();
 
    float etaPointing = m_shower->getCaloPointingEta(etaS1, etaS2, phiCluster);
    return etaPointing;
  }

  void IsolationCorrection::SetDataMC(bool is_mc){
    m_is_mc = is_mc;
    m_set_mc = true;
  }

  void IsolationCorrection::SetDataMC(const xAOD::EventInfo* event_info){
    SetDataMC(event_info->eventType(xAOD::EventInfo::IS_SIMULATION));
  }

  void IsolationCorrection::setEtaBins(){
    m_eta_bins_fine.reserve(m_nBinsEtaFine);
    m_eta_bins_coarse.reserve(m_nBinsEtaCoarse);
    m_eta_bins_fine = {0.0, 0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47};
    m_eta_bins_coarse = {0.0, 0.60, 1.37, 1.52, 1.81, 2.47};
  }

  void IsolationCorrection::setIsolCorr(){
    if (m_tool_ver == REL17)        set2011Corr();
    else if (m_tool_ver == REL17_2) { set2011Corr(); set2012Corr(); }
    else if (m_tool_ver == REL18)   set2014Corr();
  }

  void IsolationCorrection::set2011Corr(){
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) leakage corrections --------------------------------------
    // CURRENT isolation corrections: fine grained in eta, derived from MC11
    mc_rel17_leakage_correction_slopes_electron_15 = { 0.01466, 0.01421, 0.01427, 0.01872, 0.02008, 0.02181, 0.02141, 0.02636, 0.03285, 0.03564 };
    mc_rel17_leakage_correction_slopes_electron_20 = { 0.01616, 0.01572, 0.01596, 0.02139, 0.02339, 0.03350, 0.02499, 0.03038, 0.03870, 0.04337 };
    mc_rel17_leakage_correction_slopes_electron_25 = { 0.01663, 0.01616, 0.01667, 0.02223, 0.02445, 0.03852, 0.02611, 0.03174, 0.04093, 0.04692 };
    mc_rel17_leakage_correction_slopes_electron_30 = { 0.01689, 0.01635, 0.01684, 0.02256, 0.02485, 0.04223, 0.02660, 0.03232, 0.04182, 0.04846 };
    mc_rel17_leakage_correction_slopes_electron_35 = { 0.01695, 0.01642, 0.01693, 0.02268, 0.02501, 0.04403, 0.02685, 0.03254, 0.04223, 0.04928 };
    mc_rel17_leakage_correction_slopes_electron_40 = { 0.01701, 0.01646, 0.01702, 0.02272, 0.02517, 0.04550, 0.02698, 0.03267, 0.04242, 0.04964 };
    mc_rel17_leakage_correction_slopes_electron_tp20 = { 0.01460, 0.01410, 0.01440, 0.02070, 0.02390, 0.04630, 0.02590, 0.03010, 0.03970, 0.04850 };
    mc_rel17_leakage_correction_slopes_electron_tp30 = { 0.01470, 0.01410, 0.01450, 0.02080, 0.02400, 0.04610, 0.02610, 0.03020, 0.03980, 0.04850 };
    mc_rel17_leakage_correction_slopes_electron_tp40 = { 0.01470, 0.01420, 0.01450, 0.02080, 0.02400, 0.04660, 0.02630, 0.03020, 0.03980, 0.04850 };
    
    mc_rel17_leakage_correction_offsets_electron_15 = { 21.71, 36.00, 132.56, 191.64, 263.46, 619.58, 288.75, 121.92, 102.35, 175.02 };
    mc_rel17_leakage_correction_offsets_electron_20 = { 76.67, 85.35, 184.90, 276.72, 384.97, 595.60, 657.99, 231.88, 170.48, 312.30 };
    mc_rel17_leakage_correction_offsets_electron_25 = { 90.44, 105.02, 267.95, 420.38, 555.09, 1014.50, 765.83, 283.50, 224.18, 357.30 };
    mc_rel17_leakage_correction_offsets_electron_30 = { 119.55, 127.09, 279.48, 430.96, 571.81, 846.86, 968.01, 354.46, 263.11, 455.21 };
    mc_rel17_leakage_correction_offsets_electron_35 = { 138.79, 161.87, 371.47, 572.08, 754.17, 1249.38, 1000.44, 389.22, 295.72, 464.28 };
    mc_rel17_leakage_correction_offsets_electron_40 = { 180.08, 187.89, 363.31, 553.46, 707.60, 1006.20, 1105.73, 434.21, 312.78, 535.90 };
    mc_rel17_leakage_correction_offsets_electron_tp20 = { -368.60, -334.90, -281.10, -331.10, -251.10, -24.20, -441.40, -472.90, -543.40, -500.10 };
    mc_rel17_leakage_correction_offsets_electron_tp30 = { -371.70, -338.40, -283.80, -326.10, -242.30, 52.00, -439.10, -479.20, -540.80, -491.50 };
    mc_rel17_leakage_correction_offsets_electron_tp40 = { -371.90, -338.90, -284.00, -322.10, -238.80, 53.20, -437.70, -477.50, -538.10, -491.90 };
    
    // photons: last eta bin isn't used
    mc_rel17_leakage_correction_slopes_photon_converted_15 = { 0.01450, 0.01410, 0.01410, 0.01860, 0.01990, 0.0, 0.02120, 0.02610, 0.03260, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_20 = { 0.01600, 0.01560, 0.01580, 0.02130, 0.02320, 0.0, 0.02450, 0.03000, 0.03840, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_25 = { 0.01630, 0.01600, 0.01620, 0.02210, 0.02420, 0.0, 0.02560, 0.03140, 0.04060, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_30 = { 0.01630, 0.01600, 0.01630, 0.02240, 0.02460, 0.0, 0.02610, 0.03190, 0.04150, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_35 = { 0.01660, 0.01600, 0.01630, 0.02240, 0.02470, 0.0, 0.02640, 0.03210, 0.04190, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_40 = { 0.01610, 0.01590, 0.01620, 0.02250, 0.02480, 0.0, 0.02650, 0.03220, 0.04210, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_tp20 = { 0.01480, 0.01430, 0.01450, 0.02090, 0.02370, 0.0, 0.02550, 0.02960, 0.03920, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_tp30 = { 0.01490, 0.01430, 0.01460, 0.02090, 0.02370, 0.0, 0.02580, 0.02970, 0.03930, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_converted_tp40 = { 0.01490, 0.01430, 0.01450, 0.02090, 0.02370, 0.0, 0.02590, 0.02970, 0.03930, 0.0 };
    
    mc_rel17_leakage_correction_offsets_photon_converted_15 = { 36.50, 61.80, 176.90, 206.40, 300.70, 0.0, 277.40, 91.70, 126.60, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_20 = { 93.30, 101.40, 270.60, 369.10, 514.70, 0.0, 586.10, 160.80, 193.80, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_25 = { 195.80, 166.20, 386.50, 472.30, 637.30, 0.0, 739.40, 207.60, 240.60, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_30 = { 286.60, 241.60, 501.60, 570.70, 739.50, 0.0, 860.00, 264.50, 270.40, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_35 = { 329.90, 314.70, 585.60, 655.60, 835.70, 0.0, 934.30, 291.50, 291.90, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_40 = { 478.70, 383.80, 679.20, 725.70, 938.70, 0.0, 999.30, 322.80, 316.20, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_tp20 = { -421.90, -362.20, -291.50, -312.20, -170.80, 0.0, -440.70, -464.60, -479.90, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_tp30 = { -424.40, -368.70, -286.60, -301.50, -150.20, 0.0, -451.00, -468.10, -476.70, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_converted_tp40 = { -432.10, -368.20, -284.10, -294.40, -141.20, 0.0, -455.70, -470.10, -474.10, 0.0 };
    
    mc_rel17_leakage_correction_slopes_photon_unconverted_15 = { 0.01480, 0.01410, 0.01400, 0.01820, 0.01950, 0.0, 0.02140, 0.02660, 0.03250, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_20 = { 0.01630, 0.01560, 0.01560, 0.02060, 0.02240, 0.0, 0.02480, 0.03060, 0.03830, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_25 = { 0.01670, 0.01610, 0.01620, 0.02140, 0.02330, 0.0, 0.02590, 0.03200, 0.04050, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_30 = { 0.01690, 0.01630, 0.01640, 0.02170, 0.02360, 0.0, 0.02630, 0.03260, 0.04140, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_35 = { 0.01700, 0.01640, 0.01650, 0.02180, 0.02380, 0.0, 0.02650, 0.03280, 0.04180, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_40 = { 0.01710, 0.01640, 0.01650, 0.02190, 0.02390, 0.0, 0.02660, 0.03290, 0.04200, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp20 = { 0.01470, 0.01400, 0.01400, 0.01930, 0.02220, 0.0, 0.02500, 0.02980, 0.03850, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp30 = { 0.01470, 0.01410, 0.01400, 0.01940, 0.02230, 0.0, 0.02530, 0.02980, 0.03860, 0.0 };
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp40 = { 0.01470, 0.01410, 0.01410, 0.01940, 0.02230, 0.0, 0.02550, 0.02990, 0.03860, 0.0 };
    
    mc_rel17_leakage_correction_offsets_photon_unconverted_15 = { -27.80, 3.80, 67.50, 80.90, 114.90, 0.0, 82.60, 2.10, 39.80, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_20 = { -17.70, 12.60, 97.80, 126.50, 186.20, 0.0, 200.80, 24.00, 62.30, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_25 = { -8.60, 20.30, 118.20, 161.80, 244.30, 0.0, 271.80, 39.80, 79.10, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_30 = { 5.40, 33.80, 141.60, 199.50, 295.40, 0.0, 336.50, 64.80, 90.40, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_35 = { 9.60, 47.80, 154.10, 231.10, 346.10, 0.0, 384.60, 77.80, 96.90, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_40 = { 13.30, 62.00, 177.00, 267.10, 406.20, 0.0, 419.80, 89.40, 105.90, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp20 = { -371.60, -344.40, -306.50, -370.80, -395.70, 0.0, -441.10, -464.50, -492.40, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp30 = { -373.30, -346.80, -307.70, -372.40, -393.90, 0.0, -454.40, -467.20, -494.30, 0.0 };
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp40 = { -373.70, -347.40, -308.10, -372.90, -394.30, 0.0, -461.00, -469.00, -493.30, 0.0 };
    // ------------------------------------------------------------------------------------------
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) pileup corrections --------------------------------------
    // all corrections use >= 2 tracks/vertex
    
    // linear corrections from Zee rel 17 full 2011 data
    // - not used in favor of quadratic corrections below
    //     rel17_npv_correction_slopes_20 = {066., 075., 0., 065., 075.};
    //     rel17_npv_correction_slopes_30 = {163., 191., 0., 173., 196.};
    //     rel17_npv_correction_slopes_40 = {315., 357., 0., 333., 366.};
 
    // quadratic corrections from rel 17 full 2011 data    
    rel17_npv_correction_slopes_20 = { 42., 52., 0.0, 58., 45. };
    rel17_npv_correction_slopes_30 = { 140., 137., 0.0, 152., 119. };
    rel17_npv_correction_slopes_40 = { 253., 248., 0.0, 287., 227. };

    rel17_npv_correction_quads_20 = { 1.65, 1.66, 0.0, 0.467, 2.19 };
    rel17_npv_correction_quads_30 = { 1.74, 3.63, 0.0, 1.61, 5.39 };
    rel17_npv_correction_quads_40 = { 4.26, 7.05, 0.0, 3.50, 9.55 };

    // -------------------------------------------------------------------------------------------
  }

  void IsolationCorrection::set2012Corr() {
    if( m_corr_file != ""){
      load2012Corr();
    }else{
      std::cerr<<"Correction file for 2012 data/mc not specified, tool not initialized for 2012 corrections\n";
    }
  }

  void IsolationCorrection::load2012Corr() {
    TFile* file_ptleakagecorr = new TFile( m_corr_file.c_str(), "read" );
    if(!file_ptleakagecorr){
      std::cerr<<"Correction file for 2012 data/mc not found, tool not initialized for 2012 corrections\n";
      m_corr_file = "";
    }else{
    
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin0_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin1_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin2_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin3_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin4_extrap") );
      graph_cone40_photon_unconverted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin6_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin7_extrap") );
      graph_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_unconverted_etabin8_extrap") );
      graph_cone40_photon_unconverted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin0_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin1_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin2_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin3_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin4_extrap") );
      graph_cone40_photon_converted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin6_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin7_extrap") );
      graph_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_photon_converted_etabin8_extrap") );
      graph_cone40_photon_converted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin0_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin1_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin2_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin3_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin4_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin5_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin6_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin7_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin8_extrap") );
      graph_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone40_electron_etabin9_extrap") );                                                

      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin0_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin1_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin2_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin3_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin4_extrap") );
      graph_cone30_photon_unconverted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin6_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin7_extrap") );
      graph_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_unconverted_etabin8_extrap") );
      graph_cone30_photon_unconverted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin0_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin1_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin2_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin3_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin4_extrap") );
      graph_cone30_photon_converted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin6_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin7_extrap") );
      graph_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_photon_converted_etabin8_extrap") );
      graph_cone30_photon_converted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin0_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin1_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin2_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin3_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin4_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin5_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin6_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin7_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin8_extrap") );
      graph_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone30_electron_etabin9_extrap") );

      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin0_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin1_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin2_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin3_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin4_extrap") );
      graph_cone20_photon_unconverted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin6_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin7_extrap") );
      graph_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_unconverted_etabin8_extrap") );
      graph_cone20_photon_unconverted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin0_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin1_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin2_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin3_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin4_extrap") );
      graph_cone20_photon_converted.push_back( new TGraph() );//No corrections for photons in the crack region 
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin6_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin7_extrap") );
      graph_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_photon_converted_etabin8_extrap") );
      graph_cone20_photon_converted.push_back( new TGraph() );//No corrections for photons with |eta|>2.37
        
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin0_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin1_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin2_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin3_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin4_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin5_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin6_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin7_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin8_extrap") );
      graph_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("graph_cone20_electron_etabin9_extrap") );
    }
  }

  void IsolationCorrection::set2014Corr() {
    if( m_corr_file != ""){
      load2014Corr();
    }else{
      std::cerr<<"Correction file for 2014 data/mc not specified, tool not initialized for 2014 corrections\n";
    }    
  }

  void IsolationCorrection::load2014Corr() {
    TFile* file_ptleakagecorr = new TFile( m_corr_file.c_str(), "read" );
    
    if(!file_ptleakagecorr){
      std::cerr<<"Correction file for 2014 data/mc not found, tool not initialized for 2014 corrections\n";
      m_corr_file = "";
    }else{
      // Photon corrections
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.0_0.1_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.1_0.6_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.6_0.8_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.8_1.15_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.15_1.37_unconv") );
      graph_2014_cone40_photon_unconverted.push_back(  new TGraph() );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.52_1.81_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.81_2.01_unconv") );
      graph_2014_cone40_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_2.01_2.37_unconv") );
      graph_2014_cone40_photon_unconverted.push_back(  new TGraph() );
        
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.0_0.1_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.1_0.6_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.6_0.8_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.8_1.15_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.15_1.37_conv") );
      graph_2014_cone40_photon_converted.push_back(  new TGraph() );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.52_1.81_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.81_2.01_conv") );
      graph_2014_cone40_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_2.01_2.37_conv") );
      graph_2014_cone40_photon_converted.push_back(  new TGraph() );
        
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.0_0.1_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.1_0.6_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.6_0.8_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.8_1.15_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.15_1.37_unconv") );
      graph_2014_cone30_photon_unconverted.push_back(  new TGraph() );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.52_1.81_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.81_2.01_unconv") );
      graph_2014_cone30_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_2.01_2.37_unconv") );
      graph_2014_cone30_photon_unconverted.push_back(  new TGraph() );
        
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.0_0.1_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.1_0.6_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.6_0.8_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.8_1.15_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.15_1.37_conv") );
      graph_2014_cone30_photon_converted.push_back(  new TGraph() );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.52_1.81_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.81_2.01_conv") );
      graph_2014_cone30_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_2.01_2.37_conv") );
      graph_2014_cone30_photon_converted.push_back(  new TGraph() );
        
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.0_0.1_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.1_0.6_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.6_0.8_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.8_1.15_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.15_1.37_unconv") );
      graph_2014_cone20_photon_unconverted.push_back(  new TGraph() );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.52_1.81_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.81_2.01_unconv") );
      graph_2014_cone20_photon_unconverted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_2.01_2.37_unconv") );
      graph_2014_cone20_photon_unconverted.push_back(  new TGraph() );
        
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.0_0.1_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.1_0.6_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.6_0.8_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.8_1.15_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.15_1.37_conv") );
      graph_2014_cone20_photon_converted.push_back(  new TGraph() );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.52_1.81_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.81_2.01_conv") );
      graph_2014_cone20_photon_converted.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_2.01_2.37_conv") );
      graph_2014_cone20_photon_converted.push_back(  new TGraph() );

      // Electron corrections        
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.0_0.1_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.1_0.6_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.6_0.8_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_0.8_1.15_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.15_1.37_electron") );
      graph_2014_cone40_electron.push_back(  new TGraph() );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.52_1.81_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_1.81_2.01_electron") );
      graph_2014_cone40_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone40_eta_2.01_2.37_electron") );
      graph_2014_cone40_electron.push_back(  new TGraph() );
        
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.0_0.1_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.1_0.6_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.6_0.8_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_0.8_1.15_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.15_1.37_electron") );
      graph_2014_cone30_electron.push_back(  new TGraph() );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.52_1.81_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_1.81_2.01_electron") );
      graph_2014_cone30_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone30_eta_2.01_2.37_electron") );
      graph_2014_cone30_electron.push_back(  new TGraph() );
        
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.0_0.1_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.1_0.6_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.6_0.8_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_0.8_1.15_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.15_1.37_electron") );
      graph_2014_cone20_electron.push_back(  new TGraph() );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.52_1.81_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_1.81_2.01_electron") );
      graph_2014_cone20_electron.push_back( (TGraph*) file_ptleakagecorr->Get("mean_f_topoetcone20_eta_2.01_2.37_electron") );
      graph_2014_cone20_electron.push_back(  new TGraph() );
        
    }
  }

  template <class T> void IsolationCorrection::FreeClear( T & cntr ) {
    for ( typename T::iterator it = cntr.begin(); 
	  it != cntr.end(); ++it ) {
      delete * it;
    }
    cntr.clear();
  }

  IsolationCorrection::~IsolationCorrection() {
    m_eta_bins_fine.clear();
    m_eta_bins_coarse.clear();

    if (m_tool_ver == REL17_2) {
      FreeClear( graph_cone40_photon_unconverted );
      FreeClear( graph_cone30_photon_unconverted );
      FreeClear( graph_cone20_photon_unconverted );
      
      FreeClear( graph_cone40_photon_converted );
      FreeClear( graph_cone30_photon_converted );
      FreeClear( graph_cone20_photon_converted );
      
      FreeClear( graph_cone40_electron );
      FreeClear( graph_cone30_electron );
      FreeClear( graph_cone20_electron );
    } else if (m_tool_ver == REL18) {
      FreeClear( graph_2014_cone40_photon_unconverted );
      FreeClear( graph_2014_cone40_photon_converted );
      FreeClear( graph_2014_cone40_electron );
      
      FreeClear( graph_2014_cone30_photon_unconverted );
      FreeClear( graph_2014_cone30_photon_converted );
      FreeClear( graph_2014_cone30_electron );
      
      FreeClear( graph_2014_cone20_photon_unconverted );
      FreeClear( graph_2014_cone20_photon_converted );
      FreeClear( graph_2014_cone20_electron );
    }    
    
    // -------------------------------------------------------------------------------------------
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) leakage corrections --------------------------------------
    // CURRENT isolation corrections: fine grained in eta, derived from MC11
    mc_rel17_leakage_correction_slopes_electron_15.clear();
    mc_rel17_leakage_correction_slopes_electron_20.clear();
    mc_rel17_leakage_correction_slopes_electron_25.clear();
    mc_rel17_leakage_correction_slopes_electron_30.clear();
    mc_rel17_leakage_correction_slopes_electron_35.clear();
    mc_rel17_leakage_correction_slopes_electron_40.clear();
    mc_rel17_leakage_correction_slopes_electron_tp20.clear();
    mc_rel17_leakage_correction_slopes_electron_tp30.clear();
    mc_rel17_leakage_correction_slopes_electron_tp40.clear();
    
    mc_rel17_leakage_correction_offsets_electron_15.clear();
    mc_rel17_leakage_correction_offsets_electron_20.clear();
    mc_rel17_leakage_correction_offsets_electron_25.clear();
    mc_rel17_leakage_correction_offsets_electron_30.clear();
    mc_rel17_leakage_correction_offsets_electron_35.clear();
    mc_rel17_leakage_correction_offsets_electron_40.clear();
    mc_rel17_leakage_correction_offsets_electron_tp20.clear();
    mc_rel17_leakage_correction_offsets_electron_tp30.clear();
    mc_rel17_leakage_correction_offsets_electron_tp40.clear();
    
    // photons: last eta bin isn't used
    mc_rel17_leakage_correction_slopes_photon_converted_15.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_20.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_25.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_30.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_35.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_40.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_tp20.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_tp30.clear();
    mc_rel17_leakage_correction_slopes_photon_converted_tp40.clear();
    
    mc_rel17_leakage_correction_offsets_photon_converted_15.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_20.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_25.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_30.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_35.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_40.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_tp20.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_tp30.clear();
    mc_rel17_leakage_correction_offsets_photon_converted_tp40.clear();
    
    mc_rel17_leakage_correction_slopes_photon_unconverted_15.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_20.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_25.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_30.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_35.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_40.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp20.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp30.clear();
    mc_rel17_leakage_correction_slopes_photon_unconverted_tp40.clear();
    
    mc_rel17_leakage_correction_offsets_photon_unconverted_15.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_20.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_25.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_30.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_35.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_40.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp20.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp30.clear();
    mc_rel17_leakage_correction_offsets_photon_unconverted_tp40.clear();
    // ------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) pileup corrections --------------------------------------
    // all corrections use >= 2 tracks/vertex
    
    // linear corrections from Zee rel 17 full 2011 data
    // - not used in favor of quadratic corrections below
    //     rel17_npv_correction_slopes_20.clear();
    //     rel17_npv_correction_slopes_30.clear();
    //     rel17_npv_correction_slopes_40.clear();
 
    // quadratic corrections from rel 17 full 2011 data    
    rel17_npv_correction_slopes_20.clear();
    rel17_npv_correction_slopes_30.clear();
    rel17_npv_correction_slopes_40.clear();

    rel17_npv_correction_quads_20.clear();
    rel17_npv_correction_quads_30.clear();
    rel17_npv_correction_quads_40.clear();

    // -------------------------------------------------------------------------------------------
  }

  //-----------------------------------------------------------------------
  // User function
  // Returns the pt leakage corrected isolation
  //
  float IsolationCorrection::GetPtCorrectedIsolation(float energy, 
						     float etaS2, 
						     float etaPointing, 
						     float etaCluster, 
						     float radius, 
						     bool is_mc, 
						     float Etcone_value,
						     bool isConversion,
						     ParticleType parttype,
						     Version ver){
    
    float pt_correction = GetPtCorrection(energy, etaS2, etaPointing, etaCluster, radius, is_mc, isConversion, parttype, ver);
    return Etcone_value - pt_correction;
  }
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // User function
  // Returns the pt leakage corrected topo isolation
  //
  float IsolationCorrection::GetPtCorrectedTopoIsolation(float energy, 
							 float etaS2, 
							 float etaPointing, 
							 float etaCluster, 
							 float radius, 
							 bool is_mc, 
							 float Etcone_value,
							 bool isConversion,
							 ParticleType parttype,
							 Version ver){
    
    float pt_correction = GetPtCorrectionTopo(energy, etaS2, etaPointing, etaCluster, radius, is_mc, isConversion, parttype, ver);
    return Etcone_value - pt_correction;
  }
  //-----------------------------------------------------------------------


  // ***************************************************************************************************************************
  // ***************************************************************************************************************************
  // Internal Functions Below.
  // ***************************************************************************************************************************
  // ***************************************************************************************************************************

  //-----------------------------------------------------------------------
  // Internal function
  // Gets the pt correction factor
  //
  float IsolationCorrection::GetPtCorrection(  float energy, 
					       float etaS2, float etaPointing, float etaCluster, 
					       float radius, 
					       bool is_mc, 
					       bool isConversion, ParticleType parttype, Version ver){
                                                                                         
    int newrad = GetRadius(radius);
    std::vector<float> mc_correction_slopes_ptr;
    std::vector<float> mc_correction_offsets_ptr;
    std::vector<float> data_correction_slopes_ptr;
    
    //TODO: when implementing ptcorr for etconeXX change this
    if(ver == REL18) ver = REL17_2;
    
    switch(newrad){
    case 20: 
      if (parttype == PHOTON) {
	// photons
	if(isConversion) {
	  // converted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_20; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_20;
	} else {
	  // unconverted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_20; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_20;
	}
      } else {
	// electrons
	mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_20;
	mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_20;
      }
      break;
    case 30: 
      if (parttype == PHOTON) {
	// photons
	if(isConversion) {
	  // converted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_30; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_30;
	} else {
	  // unconverted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_30; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_30;
	}
      } else {
	// electrons
	mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_30;
	mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_30;
      }
      break;
    case 40: 
      if (parttype == PHOTON) {
	// photons
	if(isConversion) {
	  // converted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_40; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_40;
	} else {
	  // unconverted
	  mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_40; 
	  mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_40;
	} 
      } else {
	// electrons
	mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_40;
	mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_40;
      }
      break;
    default:
      std::cerr << "Unable to retrieve leakage correction for cone with radius = " << radius << "." << std::endl
		<< "--- Radii must be one of {.20, .30, .40} OR {20, 30, 40}." << std::endl;
      return 0.;
    }

    float scale_factor = GetPtCorrectionFactor(etaS2, mc_correction_slopes_ptr, data_correction_slopes_ptr);
    float offset = 0.;
    if (!mc_correction_offsets_ptr.empty()) offset = GetPtCorrectionFactor(etaS2, mc_correction_offsets_ptr);
    
    //avoid warnings
    if(is_mc) is_mc = true;
    return offset + GetPtCorrectionValue(energy, etaPointing, etaCluster, scale_factor);

  }
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // Internal function
  // Gets the pt correction factor for topoIso
  //
  float IsolationCorrection::GetPtCorrectionTopo(float energy, 
						 float etaS2, float etaPointing, float etaCluster, 
						 float radius, 
						 bool is_mc, 
						 bool isConversion, ParticleType parttype, Version ver){
    double correction_value = 0.;
    if( ver == REL17 ){
      int newrad = GetRadius(radius);
      std::vector<float> mc_correction_slopes_ptr;
      std::vector<float> mc_correction_offsets_ptr;
      switch(newrad){
      case 20: 
	if (parttype == PHOTON) {
	  // photons
	  if(isConversion) {
	    // converted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_tp20; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_tp20;
	    } 
	  } else {
	    // unconverted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_tp20; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_tp20;
	    } 
	  }
	} else {
	  // electrons
	  if (ver == REL17) {
	    mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_tp20;
	    mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_tp20;
	  } 
                
	}
	break;
      case 30: 
	if (parttype == PHOTON) {
	  // photons
	  if(isConversion) {
	    // converted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_tp30; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_tp30;
	    } 
	  } else {
	    // unconverted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_tp30; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_tp30;
	    } 
	  }
	} else {
	  // electrons
	  if (ver == REL17) {
	    mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_tp30;
	    mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_tp30;
	  } 
                
	}
	break;
      case 40: 
	if (parttype == PHOTON) {
	  // photons
	  if(isConversion) {
	    // converted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_converted_tp40; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_converted_tp40;
	    } 
	  } else {
	    // unconverted
	    if (ver == REL17) {
	      mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_photon_unconverted_tp40; 
	      mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_photon_unconverted_tp40;
	    } 
	  }
	} else {
	  // electrons
	  if (ver == REL17) {
	    mc_correction_slopes_ptr = mc_rel17_leakage_correction_slopes_electron_tp40;
	    mc_correction_offsets_ptr = mc_rel17_leakage_correction_offsets_electron_tp40;
	  } 
                
	}
	break;
      default:
	std::cerr << "Unable to retrieve leakage correction for topoIso cone with radius = " << radius << "." << std::endl
		  << "--- Radii must be one of {.20, .30, .40} OR {20, 30, 40}." << std::endl;
	return 0.;
      }
      float scale_factor = GetPtCorrectionFactor(etaS2, mc_correction_slopes_ptr);
      float offset = 0.;
      if (!mc_correction_offsets_ptr.empty()) offset = GetPtCorrectionFactor(etaS2, mc_correction_offsets_ptr);
      correction_value = offset + GetPtCorrectionValue(energy, etaPointing, etaCluster, scale_factor);
    } else if (ver== REL17_2) {
      correction_value = GetPtCorrection_FromGraph(energy,etaS2,etaPointing,etaCluster,radius,isConversion,parttype);
    } else if (ver== REL18){
      correction_value = GetPtCorrection_FromGraph_2014(energy,etaS2,etaPointing,etaCluster,radius,isConversion,parttype);
    }
    
    //avoid warnings
    if(is_mc) is_mc = true;
    return correction_value;
  }
  //-----------------------------------------------------------------------


  //-----------------------------------------------------------------------
  // Internal function
  // Used to retrieve the correct radius
  int IsolationCorrection::GetRadius(float radius){
    int newrad = 0;
    // avoid roundoff errors by adding 0.1
    if(radius < 1)    newrad = (int)(radius * 100 + 0.1); 
    else                        newrad = (int)radius;
    return newrad;
  }
  //-----------------------------------------------------------------------


  //-----------------------------------------------------------------------
  // Internal function
  // Used to retrieve the correct (fine) eta bin number
  //
  int IsolationCorrection::GetEtaBinFine(float eta){
    int eta_bin=-1;
    float fabs_eta = fabs(eta);
    for (unsigned int i=0; i < m_nBinsEtaFine; ++i) {
      if ((fabs_eta >= m_eta_bins_fine[i]) && (fabs_eta < m_eta_bins_fine[i+1])) {
	eta_bin = i;
	break;
      }
    }
    return eta_bin;
  }
  //-----------------------------------------------------------------------


  //-----------------------------------------------------------------------
  // Internal function
  // Used to retrieve the correct (coarse) eta bin number
  //
  int IsolationCorrection::GetEtaBinCoarse(float eta){
    int eta_bin=-1;
    float fabs_eta = fabs(eta);
    for (unsigned int i=0; i < m_nBinsEtaCoarse; ++i) {
      if ((fabs_eta >= m_eta_bins_coarse[i]) && (fabs_eta < m_eta_bins_coarse[i+1])) {
	eta_bin = i;
	break;
      }
    }
    return eta_bin;
  }
  //-----------------------------------------------------------------------


  //-----------------------------------------------------------------------
  // Internal function
  // Returns the appropriate corrections value
  //
  float IsolationCorrection::GetPtCorrectionFactor(float eta,    
						   std::vector<float> mc_leakage_corrections_ptr, 
						   std::vector<float> data_leakage_corrections_ptr){
    if(mc_leakage_corrections_ptr.empty() && data_leakage_corrections_ptr.empty()) 
      return 0.;

    int eta_bin_fine = GetEtaBinFine(eta);
    int eta_bin_coarse = GetEtaBinCoarse(eta);
    
    float correction = 0.;
    if (!mc_leakage_corrections_ptr.empty() && (eta_bin_fine >= 0)) correction += mc_leakage_corrections_ptr[eta_bin_fine];
    if (!data_leakage_corrections_ptr.empty() && (eta_bin_coarse >= 0)) correction += data_leakage_corrections_ptr[eta_bin_coarse];

    return correction;
  }
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // Internal function
  // Does the final pt scaling
  float IsolationCorrection::GetPtCorrectionValue(float energy, float etaPointing, float etaCluster, float scale_factor){
    // apply the correction to et
    double etaForPt = ((fabs(etaPointing - etaCluster) < 0.15) ? etaPointing : etaCluster);
    double et = (fabs(etaForPt)<99.) ? energy/cosh(etaForPt) : 0.;
    
    return scale_factor * et;
  }
  //-----------------------------------------------------------------------


  //-----------------------------------------------------------------------
  // Internal function
  // Does the correction for REL17_2 from TGraph stored into isolation_leakage_corrections.root file
  // Returns the correction value in MeV
  float IsolationCorrection::GetPtCorrection_FromGraph(float energy,float etaS2,float etaPointing,float etaCluster,float radius,bool isConversion,ParticleType parttype)
  {
    int newrad = GetRadius(radius);
    double etaForPt = ((fabs(etaPointing - etaCluster) < 0.15) ? etaPointing : etaCluster);
    double et = (fabs(etaForPt)<99.) ? energy/cosh(etaForPt) : 0.;
    int etabin = GetEtaBinFine(etaS2);
    if( m_corr_file == "" ){
      std::cerr << "In IsolationCorrection::GetPtCorrection_FromGraph: the file containing the isolation leakage corrections is not initialized." << std::endl;
      std::cerr << "No correction is applied." << std::endl;
      return 0;
    }
    if (etabin < 0) return 0; // must have eta in fiducial region

    if( (etabin == 9 || etabin == 5) && parttype == PHOTON ){ 
      //No correction for photons with |etaS2|>2.37 or 1.37<|etaS2|<1.52
      return 0.;
    }
    double correction_value_mev = -1e6;
    switch(newrad){
    case 20:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_cone20_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = GeV*graph_cone20_photon_converted[etabin]->Eval(et/GeV);
	else correction_value_mev = GeV*graph_cone20_photon_unconverted[etabin]->Eval(et/GeV);
      break;
    case 30:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_cone30_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = GeV*graph_cone30_photon_converted[etabin]->Eval(et/GeV);
	else correction_value_mev = GeV*graph_cone30_photon_unconverted[etabin]->Eval(et/GeV);
      break;
    case 40:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_cone40_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = GeV*graph_cone40_photon_converted[etabin]->Eval(et/GeV);
	else correction_value_mev = GeV*graph_cone40_photon_unconverted[etabin]->Eval(et/GeV);
      break;
    default: 
      std::cerr << "Unable to retrieve leakage corrections with radius = " << radius << "." << std::endl
		<< "--- Radii must be one of {.20, .30, .40} OR {20, 30, 40}." << std::endl;
      return 0.;
    }
    return correction_value_mev;

  }

  //-----------------------------------------------------------------------
  // Internal function
  // Does the correction for REL18 from TGraph stored into isolation_ptcorrections.root file
  // Returns the correction value in MeV
  float IsolationCorrection::GetPtCorrection_FromGraph_2014(float energy,float etaS2,float etaPointing,float etaCluster,float radius,bool isConversion, ParticleType parttype)
  {
    int newrad = GetRadius(radius);
    double etaForPt = ((fabs(etaPointing - etaCluster) < 0.15) ? etaPointing : etaCluster);
    double et = (fabs(etaForPt)<99.) ? energy/cosh(etaForPt) : 0.;
    int etabin = GetEtaBinFine(etaS2);
    if( m_corr_file == "" ){
      std::cerr << "In IsolationCorrection::GetPtCorrection_FromGraph_2014: the file containing the isolation leakage corrections is not initialized." << std::endl;
      std::cerr << "No correction is applied." << std::endl;
      return 0;
    }
    if (etabin < 0) return 0; // must have eta in fiducial region

    if( (etabin == 9 || etabin == 5) && parttype == PHOTON ){ 
      //No correction for photons with |etaS2|>2.37 or 1.37<|etaS2|<1.52
      return 0.;
    }
    
    double et_2 = energy/cosh(etaCluster);
    /*
      int eta_bin = 6;
      double abs_etaCluster = abs(etaCluster);
      if(abs_etaCluster > 0.0 && abs_etaCluster < 0.3) eta_bin = 0;
      else if(abs_etaCluster > 0.3 && abs_etaCluster < 0.6) eta_bin = 1;
      else if(abs_etaCluster > 0.6 && abs_etaCluster < 0.9) eta_bin = 2;
      else if(abs_etaCluster > 0.9 && abs_etaCluster < 1.3) eta_bin = 3;
      else if(abs_etaCluster > 1.6 && abs_etaCluster < 1.9) eta_bin = 4;
      else if(abs_etaCluster > 1.9 && abs_etaCluster < 2.3) eta_bin = 5;
      if(eta_bin == 6) return 0;
    */
    
    double correction_value_mev = -1e6;
    switch(newrad){
    case 20:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_2014_cone20_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = graph_2014_cone20_photon_converted[etabin]->Eval(et_2);
	else correction_value_mev = graph_2014_cone20_photon_unconverted[etabin]->Eval(et_2);
      break;
    case 30:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_2014_cone30_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = graph_2014_cone30_photon_converted[etabin]->Eval(et_2);
	else correction_value_mev = graph_2014_cone30_photon_unconverted[etabin]->Eval(et_2);
      break;
    case 40:
      if(parttype == ELECTRON) 
	correction_value_mev = GeV*graph_2014_cone40_electron[etabin]->Eval(et/GeV);
      else
	if(isConversion) correction_value_mev = graph_2014_cone40_photon_converted[etabin]->Eval(et_2);
	else correction_value_mev = graph_2014_cone40_photon_unconverted[etabin]->Eval(et_2);
      break;
    default: 
      std::cerr << "Unable to retrieve leakage corrections with radius = " << radius << "." << std::endl
		<< "--- Radii must be one of {.20, .30, .40} OR {20, 30, 40}." << std::endl;
      return 0.;
    }
    return correction_value_mev;

  }

}
