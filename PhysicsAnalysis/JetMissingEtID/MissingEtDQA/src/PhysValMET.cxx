///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValMET.cxx 
// Implementation file for class PhysValMET
// Author: Daniel Buescher <daniel.buescher@cern.ch>, Philipp Mogg <philipp.mogg@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "PhysValMET.h"

// STL includes
#include <vector>
#include <cmath>
#include <math.h>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODMissingET/versions/MissingETBase.h" 
#include "xAODMissingET/MissingET.h" 
#include "xAODMissingET/MissingETContainer.h" 
#include "xAODMissingET/MissingETAuxContainer.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

using namespace xAOD;

namespace MissingEtDQA {

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////

  PhysValMET::PhysValMET( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent) : 
    ManagedMonitorToolBase( type, name, parent ),
    m_Resolution_TruthNonInt_RebLC_METx(0), m_Resolution_TruthNonInt_RebLC_METy(0),
    m_Resolution_TruthNonInt_RebEM_METx(0), m_Resolution_TruthNonInt_RebEM_METy(0),
    m_Resolution_TruthNonInt_RebEMPFlow_METx(0), m_Resolution_TruthNonInt_RebEMPFlow_METy(0),
    m_dPhi_leadJetMET(0), m_dPhi_subleadJetMET(0), m_dPhi_LepMET(0),
    m_MET_significance_RebLC(0), m_MET_significance_RebEM(0), m_MET_significance_RebEMPFlow(0),
    m_muonSelTool(""), m_elecSelLHTool(""), m_photonSelIsEMTool(""), m_jvtTool("")
  {
    
    declareProperty( "METContainerName", m_metName = "MET_Reference_AntiKt4LCTopo" );
    //declareProperty( "METContainerName", m_metName );
    declareProperty( "InputJets",      m_jetColl   = "AntiKt4LCTopoJets" );
    declareProperty( "InputElectrons", m_eleColl   = "Electrons"         );
    declareProperty( "InputPhotons",   m_gammaColl = "Photons"           );
    declareProperty( "InputTaus",      m_tauColl   = "TauJets"           );
    declareProperty( "InputMuons",     m_muonColl  = "Muons"             );
    declareProperty( "DoTruth", m_doTruth );
    declareProperty( "METMapName",     m_mapname   = "METAssoc"          );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty( "MuonSelectionTool",        m_muonSelTool           );
    declareProperty( "ElectronLHSelectionTool",  m_elecSelLHTool         );
    declareProperty( "PhotonIsEMSelectionTool" , m_photonSelIsEMTool     );
    declareProperty( "TauSelectionTool",         m_tauSelTool            );
    declareProperty( "METMaker",         m_metmaker );
    declareProperty( "JVTTool",          m_jvtTool );
  }
  
  // Destructor
  ///////////////
  PhysValMET::~PhysValMET()
  {}
  
  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode PhysValMET::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");    
    ATH_CHECK(ManagedMonitorToolBase::initialize());

    ATH_MSG_INFO("Retrieving tools...");

    if( m_metmaker.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve METMaker tool: " << m_metmaker->name());
      return StatusCode::FAILURE;
    }

    if( m_muonSelTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve MuonSelection tool: " << m_muonSelTool->name());
      return StatusCode::FAILURE;
    }

    if( m_elecSelLHTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve ElectronSelection tool: " << m_elecSelLHTool->name());
      return StatusCode::FAILURE;
    }

    if( m_photonSelIsEMTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve PhotonSelection tool: " << m_photonSelIsEMTool->name());
      return StatusCode::FAILURE;
    }

    if( m_tauSelTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve TauSelection tool: " << m_tauSelTool->name());
      return StatusCode::FAILURE;
    }

    if( m_jvtTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve JVT tool: " << m_jvtTool->name());
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysValMET::bookHistograms()
  {
    ATH_MSG_INFO ("Booking hists " << name() << "...");
      
    // Physics validation plots are level 10

    int m_nbinp = 100;
    int m_nbinpxy = 100;
    int m_nbinphi = 63;
    int m_nbinE = 100;
    double m_suptmi = 500.;
    double m_suptmixy = 250.;
    double m_binphi = 3.15;
    double m_lowET = 0.;
    double m_suET = 2500.;

    if (m_detailLevel >= 10) {

      //std::string name_met = "MET_RefLC";
      std::string name_met = m_metName;

      std::vector<std::string> ref_names;
      ref_names.push_back("RefEle");
      ref_names.push_back("RefGamma");
      ref_names.push_back("RefTau");
      ref_names.push_back("Muons");
      ref_names.push_back("RefJet");
      ref_names.push_back("SoftClus");
      ref_names.push_back("PVSoftTrk");
      ref_names.push_back("FinalTrk");
      ref_names.push_back("FinalClus");

      dir_met.clear();
      for(const auto& it : ref_names) {
	m_MET_RefLC.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RefLC_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefLC_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefLC_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RefLC_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RefLC.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RefLC[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefLC_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefLC_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefLC_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefLC_sum[i],dir_met[i],all));
      }


      name_met = "MET_Reference_AntiKt4EMTopo";

      dir_met.clear();
      for(const auto& it : ref_names) {
	m_MET_RefEM.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RefEM_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefEM_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefEM_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RefEM_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RefEM.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RefEM[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEM_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEM_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEM_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEM_sum[i],dir_met[i],all));
      }

      name_met = "MET_Reference_AntiKt4EMPFlow";

      dir_met.clear();
      for(const auto& it : ref_names) {
	m_MET_RefEMPFlow.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RefEMPFlow_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefEMPFlow_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RefEMPFlow_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RefEMPFlow_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RefEMPFlow.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RefEMPFlow[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEMPFlow_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEMPFlow_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEMPFlow_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RefEMPFlow_sum[i],dir_met[i],all));
      }

      //Correlations
      std::vector<std::string> corrTrk_names;
      corrTrk_names.push_back("RefEle");
      corrTrk_names.push_back("RefGamma");
      corrTrk_names.push_back("RefTau");
      corrTrk_names.push_back("Muons");
      corrTrk_names.push_back("RefJet");
      corrTrk_names.push_back("PVSoftTrk");
      std::vector<std::string> corrClus_names;
      corrClus_names.push_back("RefEle");
      corrClus_names.push_back("RefGamma");
      corrClus_names.push_back("RefTau");
      corrClus_names.push_back("Muons");
      corrClus_names.push_back("RefJet");
      corrClus_names.push_back("SoftClus");

      name_met = "MET_Corr_AntiKtLCTopo";
      for(const auto& it : corrTrk_names) {
	m_MET_LC_CorrFinalTrk.push_back( new  TH2D(("AntiKtLCTopo_"+it+"_FinalTrk").c_str(), (name_met + " " + it + "vs. FinalTrk; E_{T," + it + "}^{miss}; E_{T,FinalTrk}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }
      for(const auto& it : corrClus_names) {
	m_MET_LC_CorrFinalClus.push_back( new  TH2D(("AntiKtLCTopo_"+it+"_FinalClus").c_str(), (name_met + " " + it + "vs. FinalClus; E_{T," + it + "}^{miss}; E_{T,FinalClus}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }

      for(std::vector<TH2D*>::size_type i = 0; i < m_MET_LC_CorrFinalTrk.size(); ++i) {
	ATH_CHECK(regHist(m_MET_LC_CorrFinalTrk[i],"MET/MET_Corr_AntiKtLCTopo/",all));
	ATH_CHECK(regHist(m_MET_LC_CorrFinalClus[i],"MET/MET_Corr_AntiKtLCTopo/",all));
      }

      name_met = "MET_Corr_AntiKtEMTopo";
      for(const auto& it : corrTrk_names) {
	m_MET_EM_CorrFinalTrk.push_back( new  TH2D(("AntiKtEMTopo_"+it+"_FinalTrk").c_str(), (name_met + " " + it + "vs. FinalTrk; E_{T," + it + "}^{miss}; E_{T,FinalTrk}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }
      for(const auto& it : corrClus_names) {
	m_MET_EM_CorrFinalClus.push_back( new  TH2D(("AntiKtEMTopo_"+it+"_FinalClus").c_str(), (name_met + " " + it + "vs. FinalClus; E_{T," + it + "}^{miss}; E_{T,FinalClus}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }

      for(std::vector<TH2D*>::size_type i = 0; i < m_MET_EM_CorrFinalTrk.size(); ++i) {
	ATH_CHECK(regHist(m_MET_EM_CorrFinalTrk[i],"MET/MET_Corr_AntiKtEMTopo/",all));
	ATH_CHECK(regHist(m_MET_EM_CorrFinalClus[i],"MET/MET_Corr_AntiKtEMTopo/",all));
      }

      name_met = "MET_Corr_AntiKtEMPFlow";
      for(const auto& it : corrTrk_names) {
	m_MET_EMPFlow_CorrFinalTrk.push_back( new  TH2D(("AntiKtEMPFlow_"+it+"_FinalTrk").c_str(), (name_met + " " + it + "vs. FinalTrk; E_{T," + it + "}^{miss}; E_{T,FinalTrk}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }
      for(const auto& it : corrClus_names) {
	m_MET_EMPFlow_CorrFinalClus.push_back( new  TH2D(("AntiKtEMPFlow_"+it+"_FinalClus").c_str(), (name_met + " " + it + "vs. FinalClus; E_{T," + it + "}^{miss}; E_{T,FinalClus}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      }

      for(std::vector<TH2D*>::size_type i = 0; i < m_MET_EMPFlow_CorrFinalTrk.size(); ++i) {
	ATH_CHECK(regHist(m_MET_EMPFlow_CorrFinalTrk[i],"MET/MET_Corr_AntiKtEMPFlow/",all));
	ATH_CHECK(regHist(m_MET_EMPFlow_CorrFinalClus[i],"MET/MET_Corr_AntiKtEMPFlow/",all));
      }

      name_met = "MET_Cumulative";
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4LCTopo_FinalTrk", (name_met + " AntiKt4LCTopo_FinalTrk; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4LCTopo_FinalClus", (name_met + " AntiKt4LCTopo_FinalClus; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4EMTopo_FinalTrk", (name_met + " AntiKt4EMTopo_FinalTrk; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4EMTopo_FinalClus", (name_met + " AntiKt4EMTopo_FinalClus; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4EMPFlow_FinalTrk", (name_met + " AntiKt4EMPFlow_FinalTrk; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
      m_MET_Cumu.push_back( new  TH1D("MET_Cumulative_AntiKt4EMPFlow_FinalClus", (name_met + " AntiKt4EMPFlow_FinalClus; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_Cumu.size(); ++i) {
	ATH_CHECK(regHist(m_MET_Cumu[i],"MET/MET_Cumulative/",all));
      }

      name_met = "MET_Track";
      std::string dir = "MET/" + name_met + "/";

      ATH_CHECK(regHist(m_MET_Track = new  TH1D("Track", (name_met + " Track; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
      ATH_CHECK(regHist(m_MET_Track_x = new  TH1D("Track_x", (name_met + " Track_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_Track_y = new  TH1D("Track_y", (name_met + " Track_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_Track_phi = new  TH1D("Track_phi", (name_met + " Track_phi;  #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
      ATH_CHECK(regHist(m_MET_Track_sum = new  TH1D("Track_sum", (name_met + " Track_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));

      ATH_CHECK(regHist(m_MET_PVTrack_Nominal = new TH1D("PVTrack_Nominal", (name_met + " PVTrack_Nominal; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_x = new  TH1D("PVTrack_Nominal_x", (name_met + " PVTrack_Nominal_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_y = new  TH1D("PVTrack_Nominal_y", (name_met + " PVTrack_Nominal_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_phi = new  TH1D("PVTrack_Nominal_phi", (name_met + " PVTrack_Nominal_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_sum = new  TH1D("PVTrack_Nominal_sum", (name_met + " PVTrack_Nominal_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));

      ATH_CHECK(regHist(m_MET_PVTrack_Pileup = new  TH1D("PVTrack_Pileup", (name_met + " PVTrack_Pileup; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_x = new  TH1D("PVTrack_Pileup_x", (name_met + " PVTrack_Pileup_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_y = new  TH1D("PVTrack_Pileup_y", (name_met + " PVTrack_Pileup_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_phi = new  TH1D("PVTrack_Pileup_phi", (name_met + " PVTrack_Pileup_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_sum = new  TH1D("PVTrack_Pileup_sum", (name_met + " PVTrack_Pileup_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));


      name_met = "MET_Resolution";
      dir = "MET/" + name_met + "/";
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebLC_METx = new TH1D("m_Resolution_TruthNonInt_RebLC_METx", (name_met + " Resolution_TruthNonInt_RebLC_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebLC_METy = new TH1D("m_Resolution_TruthNonInt_RebLC_METy", (name_met + " Resolution_TruthNonInt_RebLC_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebEM_METx = new TH1D("m_Resolution_TruthNonInt_RebEM_METx", (name_met + " Resolution_TruthNonInt_RebEM_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebEM_METy = new TH1D("m_Resolution_TruthNonInt_RebEM_METy", (name_met + " Resolution_TruthNonInt_RebEM_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebEMPFlow_METx = new TH1D("m_Resolution_TruthNonInt_RebEMPFlow_METx", (name_met + " Resolution_TruthNonInt_RebEMPFlow_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
      ATH_CHECK(regHist(m_Resolution_TruthNonInt_RebEMPFlow_METy = new TH1D("m_Resolution_TruthNonInt_RebEMPFlow_METy", (name_met + " Resolution_TruthNonInt_RebEMPFlow_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));

      dir = "MET/MET_Significance/";
      ATH_CHECK(regHist(m_MET_significance_RebLC = new TH1D("MET_Significance_RebLC", (name_met + " MET_Significance_AntiKt4LCTopo; MET/sqrt(SET); Entries").c_str(), m_nbinp, 0., 25.), dir, all));
      ATH_CHECK(regHist(m_MET_significance_RebEM = new TH1D("MET_Significance_RebEM", (name_met + " MET_Significance_AntiKt4EMTopo; MET/sqrt(SET); Entries").c_str(), m_nbinp, 0., 25.), dir, all));
      ATH_CHECK(regHist(m_MET_significance_RebEMPFlow = new TH1D("MET_Significance_RebEMPFlow", (name_met + " MET_Significance_AntiKt4EMPFlow; MET/sqrt(SET); Entries").c_str(), m_nbinp, 0., 25.), dir, all));

      name_met = "MET_dPhi";
      dir = "MET/" + name_met + "/";
      ATH_CHECK(regHist(m_dPhi_leadJetMET = new TH1D("dPhi_leadJetMET", (name_met + " dPhi_leadJetMET; #Delta#Phi(leadJet,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));
      ATH_CHECK(regHist(m_dPhi_subleadJetMET = new TH1D("dPhi_subleadJetMET", (name_met + " dPhi_subleadJetMET; #Delta#Phi(subleadJet,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));
      ATH_CHECK(regHist(m_dPhi_LepMET = new TH1D("dPhi_LepMET", (name_met + " dPhi_LepMET; #Delta#Phi(Lep,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));

      std::vector<std::string> reb_names;
      reb_names.push_back("RefEle");
      reb_names.push_back("RefGamma");
      reb_names.push_back("RefTau");
      reb_names.push_back("Muons");
      reb_names.push_back("RefJet");
      reb_names.push_back("SoftClus");
      reb_names.push_back("PVSoftTrk");
      reb_names.push_back("FinalTrk");
      reb_names.push_back("FinalClus");

      name_met = "MET_Rebuilt_AntiKt4LCTopo";
      dir = "MET/" + name_met + "/";
      dir_met.clear();
      for(const auto& it : reb_names) {
	m_MET_RebLC.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RebLC_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebLC_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebLC_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RebLC_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RebLC.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RebLC[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebLC_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebLC_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebLC_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebLC_sum[i],dir_met[i],all));
      }

      name_met = "MET_Rebuilt_AntiKt4EMTopo";
      dir = "MET/" + name_met + "/";
      dir_met.clear();
      for(const auto& it : reb_names) {
	m_MET_RebEM.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RebEM_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebEM_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebEM_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RebEM_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RebEM.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RebEM[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEM_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEM_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEM_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEM_sum[i],dir_met[i],all));
      }

      name_met = "MET_Rebuilt_AntiKt4EMPFlow";
      dir = "MET/" + name_met + "/";
      dir_met.clear();
      for(const auto& it : reb_names) {
	m_MET_RebEMPFlow.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	m_MET_RebEMPFlow_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebEMPFlow_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	m_MET_RebEMPFlow_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_RebEMPFlow_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RebEMPFlow.size(); ++i) {
	ATH_CHECK(regHist(m_MET_RebEMPFlow[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEMPFlow_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEMPFlow_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEMPFlow_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_RebEMPFlow_sum[i],dir_met[i],all));
      }

      std::vector<std::string> sum_names;
      sum_names.push_back("Ele");
      sum_names.push_back("Gamma");
      sum_names.push_back("Tau");
      sum_names.push_back("Muons");
      sum_names.push_back("AntiKt4LCTopoJet");
      sum_names.push_back("AntiKt4EMTopoJet");
      sum_names.push_back("AntiKt4EMPFlowJet");

      name_met = "MET_Diff_Reference";
      dir = "MET/" + name_met + "/";
      dir_met.clear();

      for(const auto& it : sum_names) {
	m_MET_DiffRef.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss} - #Sigma p_{T}; Entries").c_str(), m_nbinpxy, -150, 150));
	m_MET_DiffRef_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss} - #Sigma p_{x}; Entries").c_str(), m_nbinpxy, -150, 150) );
	m_MET_DiffRef_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss} - #Sigma p_{y}; Entries").c_str(), m_nbinpxy, -150, 150) );
	m_MET_DiffRef_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Delta #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_DiffRef_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum} - #Sigma #abs{p_{T}}; Entries").c_str(), m_nbinpxy, -250, 250) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_DiffRef.size(); ++i) {
	ATH_CHECK(regHist(m_MET_DiffRef[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffRef_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffRef_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffRef_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffRef_sum[i],dir_met[i],all));
      }

      name_met = "MET_Diff_Rebuilt";
      dir = "MET/" + name_met + "/";
      dir_met.clear();

      for(const auto& it : sum_names) {
	m_MET_DiffReb.push_back( new  TH1D(it.c_str(), (name_met + " " + it + "; E_{T}^{miss} - #Sigma p_{T}; Entries").c_str(), m_nbinpxy, -150, 150));
	m_MET_DiffReb_x.push_back( new  TH1D((it+"_x").c_str(), (name_met + " " + it + "_x; E_{x}^{miss} - #Sigma p_{x}; Entries").c_str(), m_nbinpxy, -150, 150) );
	m_MET_DiffReb_y.push_back( new  TH1D((it+"_y").c_str(), (name_met + " " + it + "_y; E_{y}^{miss} - #Sigma p_{y}; Entries").c_str(), m_nbinpxy, -150, 150) );
	m_MET_DiffReb_phi.push_back( new  TH1D((it+"_phi").c_str(), (name_met + " " + it + "_phi; #Delta #Phi; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	m_MET_DiffReb_sum.push_back( new  TH1D((it+"_sum").c_str(), (name_met + " " + it + "_sum; E_{T}^{sum} - #Sigma #abs{p_{T}}; Entries").c_str(), m_nbinpxy, -250, 250) );
	dir_met.push_back("MET/" + name_met + "/" + it + "/");
      }

      for(std::vector<TH1D*>::size_type i = 0; i < m_MET_DiffReb.size(); ++i) {
	ATH_CHECK(regHist(m_MET_DiffReb[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffReb_x[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffReb_y[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffReb_phi[i],dir_met[i],all));
	ATH_CHECK(regHist(m_MET_DiffReb_sum[i],dir_met[i],all));
      }
    }

    return StatusCode::SUCCESS;      
  }

  StatusCode PhysValMET::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "...");

    //Retrieve Containers
    
    const xAOD::MissingETContainer* met_RefLC = 0;
    ATH_CHECK( evtStore()->retrieve(met_RefLC, m_metName) );
    if (!met_RefLC) {
      ATH_MSG_ERROR ("Couldn't retrieve MET Final");
      return StatusCode::FAILURE;                   
    } 

    const xAOD::MissingETContainer* met_RefEM = 0;
    ATH_CHECK( evtStore()->retrieve(met_RefEM,"MET_Reference_AntiKt4EMTopo") );
    if (!met_RefEM) {
      ATH_MSG_ERROR ( "Failed to retrieve MET_Reference_AntiKt4EMTopo. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::MissingETContainer* met_RefEMPFlow = 0;
    ATH_CHECK( evtStore()->retrieve(met_RefEMPFlow,"MET_Reference_AntiKt4EMPFlow") );
    if (!met_RefEMPFlow) {
      ATH_MSG_ERROR ( "Failed to retrieve MET_Reference_AntiKt4EMPFlow. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::MissingETContainer* met_Truth = 0;
    if(m_doTruth) ATH_CHECK( evtStore()->retrieve(met_Truth,"MET_Truth") );

    const xAOD::MissingETContainer* met_Track = 0;
    ATH_CHECK( evtStore()->retrieve(met_Track,"MET_Track") );
    if (!met_Track) {
      ATH_MSG_ERROR ( "Failed to retrieve MET_Track. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::JetContainer* LCjets = 0;
    ATH_CHECK( evtStore()->retrieve(LCjets,m_jetColl) );
    if (!LCjets) {
      ATH_MSG_ERROR ( "Failed to retrieve Jet container: " << m_jetColl << ". Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::JetContainer* EMjets = 0;
    ATH_CHECK( evtStore()->retrieve(EMjets, "AntiKt4EMTopoJets") );
    if(!EMjets){
      ATH_MSG_ERROR ( "Failed to retrieve AntiKt4EMTopoJets container. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::JetContainer* EMPFlowjets = 0;
    ATH_CHECK( evtStore()->retrieve(EMPFlowjets, "AntiKt4EMPFlowJets") );
    if(!EMPFlowjets){
      ATH_MSG_ERROR ( "Failed to retrieve AntiKt4EMPFlowJets container. Exiting." );
      return StatusCode::FAILURE;
    }
       
    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve(muons,m_muonColl) );
    if (!muons) {
      ATH_MSG_ERROR ( "Failed to retrieve Muon container. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::ElectronContainer* electrons = 0;
    ATH_CHECK( evtStore()->retrieve(electrons,m_eleColl) );
    if (!electrons) {
      ATH_MSG_ERROR ( "Failed to retrieve Electron container. Exiting." );
      return StatusCode::FAILURE;
    }

    const xAOD::PhotonContainer* photons = 0;
    ATH_CHECK( evtStore()->retrieve(photons,m_gammaColl) );
    if (!electrons) {
      ATH_MSG_ERROR ( "Failed to retrieve Photon container. Exiting." );
      return StatusCode::FAILURE;
    }

    const TauJetContainer* taus = 0;
    ATH_CHECK( evtStore()->retrieve(taus, m_tauColl) );
    if(!taus) {
      ATH_MSG_ERROR("Failed to retrieve TauJet container: " << m_tauColl);
      return StatusCode::SUCCESS;
    }

    // Fill RefLC
    ATH_MSG_INFO( "  MET_RefLC:" );
    for(const auto& it : *met_RefLC) {
      std::string name = it->name();
      if(name == "RefEle"){
	m_MET_RefLC[0]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[0]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[0]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[0]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[0]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefGamma"){
	m_MET_RefLC[1]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[1]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[1]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[1]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[1]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefTau"){
	m_MET_RefLC[2]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[2]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[2]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[2]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[2]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "Muons"){
	m_MET_RefLC[3]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[3]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[3]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[3]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[3]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefJet"){
	m_MET_RefLC[4]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[4]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[4]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[4]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[4]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "SoftClus"){
	m_MET_RefLC[5]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[5]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[5]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[5]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[5]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "PVSoftTrk"){
	m_MET_RefLC[6]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[6]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[6]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[6]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[6]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalTrk"){
	m_MET_RefLC[7]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[7]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[7]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[7]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[7]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalClus"){
	m_MET_RefLC[8]->Fill((*met_RefLC)[name.c_str()]->met()/1000., 1.);
	m_MET_RefLC_x[8]->Fill((*met_RefLC)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefLC_y[8]->Fill((*met_RefLC)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefLC_phi[8]->Fill((*met_RefLC)[name.c_str()]->phi(), 1.);
	m_MET_RefLC_sum[8]->Fill((*met_RefLC)[name.c_str()]->sumet()/1000., 1.);
      }
    }
    
    //Fill AntiKt4EMTopo Ref
    ATH_MSG_INFO( "  MET_AntiKt4EMTopo:" );
    for(const auto& it : *met_RefEM) {
      std::string name = it->name();
      if(name == "RefEle"){
	m_MET_RefEM[0]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[0]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[0]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[0]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[0]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefGamma"){
	m_MET_RefEM[1]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[1]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[1]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[1]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[1]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefTau"){
	m_MET_RefEM[2]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[2]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[2]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[2]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[2]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "Muons"){
	m_MET_RefEM[3]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[3]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[3]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[3]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[3]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefJet"){
	m_MET_RefEM[4]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[4]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[4]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[4]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[4]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "SoftClus"){
	m_MET_RefEM[5]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[5]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[5]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[5]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[5]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "PVSoftTrk"){
	m_MET_RefEM[6]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[6]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[6]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[6]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[6]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalTrk"){
	m_MET_RefEM[7]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[7]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[7]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[7]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[7]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalClus"){
	m_MET_RefEM[8]->Fill((*met_RefEM)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEM_x[8]->Fill((*met_RefEM)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEM_y[8]->Fill((*met_RefEM)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEM_phi[8]->Fill((*met_RefEM)[name.c_str()]->phi(), 1.);
	m_MET_RefEM_sum[8]->Fill((*met_RefEM)[name.c_str()]->sumet()/1000., 1.);
      }
    }

    //Fill AntiKt4EMPFlow Ref
    ATH_MSG_INFO( "  MET_AntiKt4EMPFlow:" );
    for(const auto& it : *met_RefEMPFlow) {
      std::string name = it->name();
      if(name == "RefEle"){
	m_MET_RefEMPFlow[0]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[0]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[0]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[0]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[0]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefGamma"){
	m_MET_RefEMPFlow[1]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[1]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[1]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[1]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[1]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefTau"){
	m_MET_RefEMPFlow[2]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[2]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[2]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[2]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[2]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "Muons"){
	m_MET_RefEMPFlow[3]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[3]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[3]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[3]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[3]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "RefJet"){
	m_MET_RefEMPFlow[4]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[4]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[4]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[4]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[4]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "SoftClus"){
	m_MET_RefEMPFlow[5]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[5]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[5]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[5]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[5]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "PVSoftTrk"){
	m_MET_RefEMPFlow[6]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[6]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[6]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[6]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[6]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalTrk"){
	m_MET_RefEMPFlow[7]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[7]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[7]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[7]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[7]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
      if(name == "FinalClus"){
	m_MET_RefEMPFlow[8]->Fill((*met_RefEMPFlow)[name.c_str()]->met()/1000., 1.);
	m_MET_RefEMPFlow_x[8]->Fill((*met_RefEMPFlow)[name.c_str()]->mpx()/1000., 1.);
	m_MET_RefEMPFlow_y[8]->Fill((*met_RefEMPFlow)[name.c_str()]->mpy()/1000., 1.);
	m_MET_RefEMPFlow_phi[8]->Fill((*met_RefEMPFlow)[name.c_str()]->phi(), 1.);
	m_MET_RefEMPFlow_sum[8]->Fill((*met_RefEMPFlow)[name.c_str()]->sumet()/1000., 1.);
      }
    }

   // Fill MET Track
   ATH_MSG_INFO( "  MET_Track:" );

   m_MET_Track->Fill((*met_Track)["Track"]->met()/1000., 1.);
   m_MET_Track_x->Fill((*met_Track)["Track"]->mpx()/1000., 1.);
   m_MET_Track_y->Fill((*met_Track)["Track"]->mpy()/1000., 1.);
   m_MET_Track_phi->Fill((*met_Track)["Track"]->phi(), 1.);
   m_MET_Track_sum->Fill((*met_Track)["Track"]->sumet()/1000., 1.);
    
   const xAOD::VertexContainer *vxCont = 0;
   ATH_CHECK( evtStore()->retrieve(vxCont, "PrimaryVertices") );
   for(const auto& vx : *vxCont) {
     int N = vx->index();
     const std::string name = "PVTrack_vx"+std::to_string(N);
     if(vx->vertexType()!=xAOD::VxType::NoVtx) {
       if(vx->vertexType()==xAOD::VxType::PriVtx) {
	 m_MET_PVTrack_Nominal->Fill((*met_Track)[name]->met()/1000., 1.);
	 m_MET_PVTrack_Nominal_x->Fill((*met_Track)[name]->mpx()/1000., 1.);
	 m_MET_PVTrack_Nominal_y->Fill((*met_Track)[name]->mpy()/1000., 1.);
	 m_MET_PVTrack_Nominal_phi->Fill((*met_Track)[name]->phi(), 1.);
	 m_MET_PVTrack_Nominal_sum->Fill((*met_Track)[name]->sumet()/1000., 1.);
       } else { 
	 m_MET_PVTrack_Pileup->Fill((*met_Track)[name]->met()/1000., 1.);
	 m_MET_PVTrack_Pileup_x->Fill((*met_Track)[name]->mpx()/1000., 1.);
	 m_MET_PVTrack_Pileup_y->Fill((*met_Track)[name]->mpy()/1000., 1.);
	 m_MET_PVTrack_Pileup_phi->Fill((*met_Track)[name]->phi(), 1.);
	 m_MET_PVTrack_Pileup_sum->Fill((*met_Track)[name]->sumet()/1000., 1.);
       }
     }
   }
    
    
   //Fill MET Angles
   ATH_MSG_INFO( "  MET_Angles :" );

   double leadPt = 0., subleadPt = 0., leadPhi = 0., subleadPhi = 0.;

   xAOD::JetContainer::const_iterator jet_itr = LCjets->begin();
   xAOD::JetContainer::const_iterator jet_end = LCjets->end();

   for( ; jet_itr != jet_end; ++jet_itr ) {
     if((*jet_itr)->pt() > subleadPt) {
       subleadPt = (*jet_itr)->pt();
       subleadPhi = (*jet_itr)->phi();
     }
     if((*jet_itr)->pt() > leadPt) {
       subleadPt = leadPt;
       subleadPhi = leadPhi;
       leadPt = (*jet_itr)->pt();
       leadPhi = (*jet_itr)->phi();
     }
   }

   m_dPhi_leadJetMET->Fill( -remainder( leadPhi - (*met_RefLC)["FinalClus"]->phi(), 2*M_PI ) );
   m_dPhi_subleadJetMET->Fill( -remainder( subleadPhi - (*met_RefLC)["FinalClus"]->phi(), 2*M_PI ) );
    

   leadPt = 0.; leadPhi = 0.;

   xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
   xAOD::MuonContainer::const_iterator muon_end = muons->end();

   for( ; muon_itr != muon_end; ++muon_itr ) {
     if((*muon_itr)->pt() > leadPt) {
       leadPt = (*muon_itr)->pt();
       leadPhi = (*muon_itr)->phi();
     }
   }

   xAOD::ElectronContainer::const_iterator electron_itr = electrons->begin();
   xAOD::ElectronContainer::const_iterator electron_end = electrons->end();

   for( ; electron_itr != electron_end; ++electron_itr ) {
     if((*electron_itr)->pt() > leadPt) {
       leadPt = (*electron_itr)->pt();
       leadPhi = (*electron_itr)->phi();
     }
   }

   m_dPhi_LepMET->Fill( -remainder( leadPhi - (*met_RefLC)["FinalClus"]->phi(), 2*M_PI ) );

   //Prepare Rebuilding MET
   ATH_MSG_INFO( "  Rebuilding MET" );
   MissingETContainer* met_RebLC = new MissingETContainer();
   if( evtStore()->record(met_RebLC,"MET_Rebuilt_AntiKt4LCTopo").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETContainer: MET_Rebuilt_AntiKt4LCTopo");
     return StatusCode::FAILURE;
   }
   MissingETAuxContainer* met_RebLCAux = new MissingETAuxContainer();
   if( evtStore()->record(met_RebLCAux,"MET_Rebuilt_AntiKt4LCTopoAux").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETAuxContainer: MET_Rebuilt_AntiKt4LCTopoAux");
     return StatusCode::FAILURE;
   }
   met_RebLC->setStore(met_RebLCAux);

   m_mapname = "METAssoc_AntiKt4LCTopo";
   m_corename = "MET_Core_AntiKt4LCTopo";
   const MissingETAssociationMap* metMap = 0;
   if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
     return StatusCode::SUCCESS;
   }
   metMap->resetObjSelectionFlags();
   const MissingETContainer* coreMet(0);
   if( evtStore()->retrieve(coreMet, m_corename).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_corename);
     return StatusCode::SUCCESS;
   }

   ATH_MSG_INFO( "  MET_Rebuilt_AntiKt4LCTopo:" );
   //Select and flag objects for final MET building ***************************
   // Electrons
   ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
   bool is_electron = 0;
   for(const auto& el : *electrons) {
     if(Accept(el)) {
       metElectrons.push_back(el);
       is_electron = 1;
     }
   }

   if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, met_RebLC, metElectrons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build electron term.");
   }

   // Photons
   ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
   for(const auto& ph : *photons) {
     if(Accept(ph)) {
       metPhotons.push_back(ph);
     }
   }
   if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, met_RebLC, metPhotons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build photon term.");
   }

   // Taus
   ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
   for(const auto& tau : *taus) {
     if(Accept(tau)) {
       metTaus.push_back(tau);
     }
   }
   if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, met_RebLC,metTaus.asDataVector(),metMap).isFailure() ){
     ATH_MSG_WARNING("Failed to build tau term.");
   }


   // Muons
   ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
   bool is_muon = 0;
   for(const auto& mu : *muons) {
     if(Accept(mu)) {
       metMuons.push_back(mu);
       is_muon = 1;
     }
   }
   if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, met_RebLC, metMuons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build muon term.");
   }

   // Jets
   ConstDataVector<JetContainer> metJetsLC(SG::VIEW_ELEMENTS);
   for(const auto& jet : *LCjets) {
     if(Accept(jet)) {
       metJetsLC.push_back(jet);
     }
   }
   if( m_metmaker->rebuildJetMET("RefJet", "SoftClus", "PVSoftTrk", met_RebLC, LCjets, coreMet, metMap, false).isFailure() ) {
     ATH_MSG_WARNING("Failed to build jet and soft terms.");
   }
   MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
   if((*met_RebLC)["FinalTrk"]) trksource = (*met_RebLC)["FinalTrk"]->source();
   if( m_metmaker->buildMETSum("FinalTrk", met_RebLC, trksource).isFailure() ){
     ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
   }
   MissingETBase::Types::bitmask_t clsource = MissingETBase::Source::LCTopo;
   if((*met_RebLC)["FinalClus"]) clsource = (*met_RebLC)["FinalClus"]->source();
   if( m_metmaker->buildMETSum("FinalClus", met_RebLC, clsource).isFailure() ) {
     ATH_MSG_WARNING("Building MET FinalClus sum failed.");
   }

   //Fill histograms
   for(const auto& it : *met_RebLC) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_RebLC[0]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[0]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[0]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[0]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[0]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_RebLC[1]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[1]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[1]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[1]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[1]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_RebLC[2]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[2]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[2]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[2]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[2]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_RebLC[3]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[3]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[3]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[3]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[3]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_RebLC[4]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[4]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[4]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[4]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[4]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_RebLC[5]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[5]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[5]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[5]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[5]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_RebLC[6]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[6]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[6]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[6]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[6]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalTrk"){
       m_MET_RebLC[7]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[7]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[7]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[7]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[7]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalClus"){
       m_MET_RebLC[8]->Fill((*met_RebLC)[name.c_str()]->met()/1000., 1.);
       m_MET_RebLC_x[8]->Fill((*met_RebLC)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebLC_y[8]->Fill((*met_RebLC)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebLC_phi[8]->Fill((*met_RebLC)[name.c_str()]->phi(), 1.);
       m_MET_RebLC_sum[8]->Fill((*met_RebLC)[name.c_str()]->sumet()/1000., 1.);
     }
   }

   //Now AntiKt4EMTopo
   ATH_MSG_INFO( "  MET_Rebuilt_AntiKt4EMTopo:" );
   MissingETContainer* met_RebEM = new MissingETContainer();
   if( evtStore()->record(met_RebEM,"MET_Rebuilt_AntiKt4EMTopo").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETContainer: MET_Rebuilt_AntiKt4EMTopo");
     return StatusCode::FAILURE;
   }
   MissingETAuxContainer* met_RebEMAux = new MissingETAuxContainer();
   if( evtStore()->record(met_RebEMAux,"MET_Rebuilt_AntiKt4EMTopoAux").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETAuxContainer: MET_Rebuilt_AntiKt4EMTopoAux");
     return StatusCode::FAILURE;
   }
   met_RebEM->setStore(met_RebEMAux);

   m_mapname = "METAssoc_AntiKt4EMTopo";
   m_corename = "MET_Core_AntiKt4EMTopo";
   //const MissingETAssociationMap* metMap = 0;
   if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
     return StatusCode::SUCCESS;
   }
   metMap->resetObjSelectionFlags();
   //const MissingETContainer* coreMet(0);
   if( evtStore()->retrieve(coreMet, m_corename).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_corename);
     return StatusCode::SUCCESS;
   }

   // Electrons
   if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, met_RebEM, metElectrons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build electron term.");
   }

   // Photons
   if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, met_RebEM, metPhotons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build photon term.");
   }

   // Taus
   if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, met_RebEM,metTaus.asDataVector(),metMap).isFailure() ){
     ATH_MSG_WARNING("Failed to build tau term.");
   }

   // Muons
   if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, met_RebEM, metMuons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build muon term.");
   }

   //Jets
   ConstDataVector<JetContainer> metJetsEM(SG::VIEW_ELEMENTS);
   for(const auto& jet : *EMjets) {
     if(Accept(jet)) {
       metJetsEM.push_back(jet);
     }
   }
   if( m_metmaker->rebuildJetMET("RefJet", "SoftClus", "PVSoftTrk", met_RebEM, EMjets, coreMet, metMap, false).isFailure() ) {
     ATH_MSG_WARNING("Failed to build jet and soft terms.");
   }
   if((*met_RebEM)["FinalTrk"]) trksource = (*met_RebEM)["FinalTrk"]->source();
   if( m_metmaker->buildMETSum("FinalTrk", met_RebEM, trksource).isFailure() ){
     ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
   }
   if((*met_RebEM)["FinalClus"]) clsource = (*met_RebEM)["FinalClus"]->source();
   if( m_metmaker->buildMETSum("FinalClus", met_RebEM, clsource).isFailure() ) {
     ATH_MSG_WARNING("Building MET FinalClus sum failed.");
   }

   //Fill histograms
   for(const auto& it : *met_RebEM) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_RebEM[0]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[0]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[0]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[0]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[0]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_RebEM[1]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[1]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[1]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[1]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[1]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_RebEM[2]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[2]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[2]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[2]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[2]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_RebEM[3]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[3]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[3]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[3]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[3]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_RebEM[4]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[4]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[4]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[4]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[4]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_RebEM[5]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[5]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[5]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[5]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[5]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_RebEM[6]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[6]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[6]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[6]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[6]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalTrk"){
       m_MET_RebEM[7]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[7]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[7]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[7]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[7]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalClus"){
       m_MET_RebEM[8]->Fill((*met_RebEM)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEM_x[8]->Fill((*met_RebEM)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEM_y[8]->Fill((*met_RebEM)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEM_phi[8]->Fill((*met_RebEM)[name.c_str()]->phi(), 1.);
       m_MET_RebEM_sum[8]->Fill((*met_RebEM)[name.c_str()]->sumet()/1000., 1.);
     }
   }

   //Now AntiKt4EMPFlow
   ATH_MSG_INFO( "  MET_Rebuilt_AntiKt4EMPFlow:" );
   MissingETContainer* met_RebEMPFlow = new MissingETContainer();
   if( evtStore()->record(met_RebEMPFlow,"MET_Rebuilt_AntiKt4EMPFlow").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETContainer: MET_Rebuilt_AntiKt4EMPFlow");
     return StatusCode::FAILURE;
   }
   MissingETAuxContainer* met_RebEMPFlowAux = new MissingETAuxContainer();
   if( evtStore()->record(met_RebEMPFlowAux,"MET_Rebuilt_AntiKt4EMPFlowAux").isFailure() ) {
     ATH_MSG_WARNING("Unable to record MissingETAuxContainer: MET_Rebuilt_AntiKt4EMPFlowAux");
     return StatusCode::FAILURE;
   }
   met_RebEMPFlow->setStore(met_RebEMPFlowAux);

   m_mapname = "METAssoc_AntiKt4EMPFlow";
   m_corename = "MET_Core_AntiKt4EMPFlow";
   //const MissingETAssociationMap* metMap = 0;
   if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
     return StatusCode::SUCCESS;
   }
   metMap->resetObjSelectionFlags();
   //const MissingETContainer* coreMet(0);
   if( evtStore()->retrieve(coreMet, m_corename).isFailure() ) {
     ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_corename);
     return StatusCode::SUCCESS;
   }

   // Electrons
   if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, met_RebEMPFlow, metElectrons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build electron term.");
   }

   // Photons
   if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, met_RebEMPFlow, metPhotons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build photon term.");
   }

   // Taus
   if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, met_RebEMPFlow,metTaus.asDataVector(),metMap).isFailure() ){
     ATH_MSG_WARNING("Failed to build tau term.");
   }

   // Muons
   if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, met_RebEMPFlow, metMuons.asDataVector(), metMap).isFailure() ) {
     ATH_MSG_WARNING("Failed to build muon term.");
   }

   //Jets
   ConstDataVector<JetContainer> metJetsEMPFlow(SG::VIEW_ELEMENTS);
   for(const auto& jet : *EMPFlowjets) {
     if(Accept(jet)) {
       metJetsEMPFlow.push_back(jet);
     }
   }
   if( m_metmaker->rebuildJetMET("RefJet", "SoftClus", "PVSoftTrk", met_RebEMPFlow, EMPFlowjets, coreMet, metMap, false).isFailure() ) {
     ATH_MSG_WARNING("Failed to build jet and soft terms.");
   }
   if((*met_RebEMPFlow)["FinalTrk"]) trksource = (*met_RebEMPFlow)["FinalTrk"]->source();
   if( m_metmaker->buildMETSum("FinalTrk", met_RebEMPFlow, trksource).isFailure() ){
     ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
   }
   if((*met_RebEMPFlow)["FinalClus"]) clsource = (*met_RebEMPFlow)["FinalClus"]->source();
   if( m_metmaker->buildMETSum("FinalClus", met_RebEMPFlow, clsource).isFailure() ) {
     ATH_MSG_WARNING("Building MET FinalClus sum failed.");
   }

   //Fill histograms
   for(const auto& it : *met_RebEMPFlow) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_RebEMPFlow[0]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[0]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[0]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[0]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[0]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_RebEMPFlow[1]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[1]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[1]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[1]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[1]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_RebEMPFlow[2]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[2]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[2]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[2]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[2]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_RebEMPFlow[3]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[3]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[3]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[3]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[3]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_RebEMPFlow[4]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[4]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[4]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[4]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[4]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_RebEMPFlow[5]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[5]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[5]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[5]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[5]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_RebEMPFlow[6]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[6]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[6]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[6]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[6]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalTrk"){
       m_MET_RebEMPFlow[7]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[7]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[7]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[7]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[7]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
     if(name == "FinalClus"){
       m_MET_RebEMPFlow[8]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000., 1.);
       m_MET_RebEMPFlow_x[8]->Fill((*met_RebEMPFlow)[name.c_str()]->mpx()/1000., 1.);
       m_MET_RebEMPFlow_y[8]->Fill((*met_RebEMPFlow)[name.c_str()]->mpy()/1000., 1.);
       m_MET_RebEMPFlow_phi[8]->Fill((*met_RebEMPFlow)[name.c_str()]->phi(), 1.);
       m_MET_RebEMPFlow_sum[8]->Fill((*met_RebEMPFlow)[name.c_str()]->sumet()/1000., 1.);
     }
   }

   //Fill Correlation Plots
   //AntiKtLCTopo
   for(const auto& it : *met_RebLC) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_LC_CorrFinalTrk[0]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
       m_MET_LC_CorrFinalClus[0]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_LC_CorrFinalTrk[1]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
       m_MET_LC_CorrFinalClus[1]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_LC_CorrFinalTrk[2]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
       m_MET_LC_CorrFinalClus[2]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_LC_CorrFinalTrk[3]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
       m_MET_LC_CorrFinalClus[3]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_LC_CorrFinalTrk[4]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
       m_MET_LC_CorrFinalClus[4]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_LC_CorrFinalTrk[5]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalTrk"]->met()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_LC_CorrFinalClus[5]->Fill((*met_RebLC)[name.c_str()]->met()/1000.,(*met_RebLC)["FinalClus"]->met()/1000., 1.);
     }
   }

   //AntiKtEMTopo
   for(const auto& it : *met_RebEM) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_EM_CorrFinalTrk[0]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
       m_MET_EM_CorrFinalClus[0]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_EM_CorrFinalTrk[1]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
       m_MET_EM_CorrFinalClus[1]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_EM_CorrFinalTrk[2]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
       m_MET_EM_CorrFinalClus[2]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_EM_CorrFinalTrk[3]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
       m_MET_EM_CorrFinalClus[3]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_EM_CorrFinalTrk[4]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
       m_MET_EM_CorrFinalClus[4]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_EM_CorrFinalTrk[5]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalTrk"]->met()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_EM_CorrFinalClus[5]->Fill((*met_RebEM)[name.c_str()]->met()/1000.,(*met_RebEM)["FinalClus"]->met()/1000., 1.);
     }
   }

   //AntiKtEMPFlow
   for(const auto& it : *met_RebEMPFlow) {
     std::string name = it->name();
     if(name == "RefEle"){
       m_MET_EMPFlow_CorrFinalTrk[0]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
       m_MET_EMPFlow_CorrFinalClus[0]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefGamma"){
       m_MET_EMPFlow_CorrFinalTrk[1]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
       m_MET_EMPFlow_CorrFinalClus[1]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefTau"){
       m_MET_EMPFlow_CorrFinalTrk[2]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
       m_MET_EMPFlow_CorrFinalClus[2]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "Muons"){
       m_MET_EMPFlow_CorrFinalTrk[3]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
       m_MET_EMPFlow_CorrFinalClus[3]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "RefJet"){
       m_MET_EMPFlow_CorrFinalTrk[4]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
       m_MET_EMPFlow_CorrFinalClus[4]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
     if(name == "PVSoftTrk"){
       m_MET_EMPFlow_CorrFinalTrk[5]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalTrk"]->met()/1000., 1.);
     }
     if(name == "SoftClus"){
       m_MET_EMPFlow_CorrFinalClus[5]->Fill((*met_RebEMPFlow)[name.c_str()]->met()/1000.,(*met_RebEMPFlow)["FinalClus"]->met()/1000., 1.);
     }
   }

   // Fill Resolution
   if(m_doTruth)
     {
       ATH_MSG_INFO( "  Resolution:" );
       m_Resolution_TruthNonInt_RebLC_METx->Fill(((*met_RebLC)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
       m_Resolution_TruthNonInt_RebLC_METy->Fill(((*met_RebLC)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
       m_Resolution_TruthNonInt_RebEM_METx->Fill(((*met_RebEM)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
       m_Resolution_TruthNonInt_RebEM_METy->Fill(((*met_RebEM)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
       m_Resolution_TruthNonInt_RebEMPFlow_METx->Fill(((*met_RebEMPFlow)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
       m_Resolution_TruthNonInt_RebEMPFlow_METy->Fill(((*met_RebEMPFlow)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
     }

    //Fill MET significance
    ATH_MSG_INFO( "  MET_significance:" );
    if( (*met_RebLC)["FinalClus"]->sumet() != 0) m_MET_significance_RebLC->Fill((*met_RebLC)["FinalClus"]->met()/sqrt((*met_RebLC)["FinalClus"]->sumet()*1000.), 1.);
    if( (*met_RebEM)["FinalClus"]->sumet() != 0) m_MET_significance_RebEM->Fill((*met_RebEM)["FinalClus"]->met()/sqrt((*met_RebEM)["FinalClus"]->sumet()*1000.), 1.);
    if( (*met_RebEMPFlow)["FinalClus"]->sumet() != 0) m_MET_significance_RebEMPFlow->Fill((*met_RebEMPFlow)["FinalClus"]->met()/sqrt((*met_RebEMPFlow)["FinalClus"]->sumet()*1000.), 1.);

   // Overlap removal

   ConstDataVector<PhotonContainer>::iterator pho_itr;
   ConstDataVector<ElectronContainer>::iterator ele_itr;
   ConstDataVector<TauJetContainer>::iterator taujet_itr;
   ConstDataVector<JetContainer>::iterator jetc_itr;
   ConstDataVector<MuonContainer>::iterator mu_itr;

   //Photons
   bool is_photon = 0;
   ConstDataVector<PhotonContainer> metPhotonsOR(SG::VIEW_ELEMENTS);
   for(pho_itr = metPhotons.begin(); pho_itr != metPhotons.end(); ++pho_itr ) {
     TLorentzVector phtlv = (*pho_itr)->p4();
     bool passOR = 1;
     for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr) {
       if(phtlv.DeltaR((*ele_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     if(passOR){
       metPhotonsOR.push_back(*pho_itr);
       is_photon = 1;
     }
   }
      
   //TauJets
   ConstDataVector<TauJetContainer> metTausOR(SG::VIEW_ELEMENTS);
   bool is_tau = 0;
   for(taujet_itr = metTaus.begin(); taujet_itr != metTaus.end(); ++taujet_itr ) {
     TLorentzVector tautlv = (*taujet_itr)->p4();
     bool passOR = 1;
     for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr) {
       if(tautlv.DeltaR((*ele_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(pho_itr = metPhotonsOR.begin(); pho_itr != metPhotonsOR.end(); ++pho_itr) {
       if(tautlv.DeltaR((*pho_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     if(passOR){
       metTausOR.push_back(*taujet_itr);
       is_tau = 1;
     }
   }

   // //AntiKt4LCJets
   ConstDataVector<JetContainer> metJetsLCOR(SG::VIEW_ELEMENTS);
   bool is_jetLC = 0;
   for(jetc_itr = metJetsLC.begin(); jetc_itr != metJetsLC.end(); ++jetc_itr ) {
     TLorentzVector jettlv = (*jetc_itr)->p4();
     bool passOR = 1;
     for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr) {
       if(jettlv.DeltaR((*ele_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(pho_itr = metPhotonsOR.begin(); pho_itr != metPhotonsOR.end(); ++pho_itr) {
       if(jettlv.DeltaR((*pho_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(taujet_itr = metTausOR.begin(); taujet_itr != metTausOR.end(); ++taujet_itr) {
       if(jettlv.DeltaR((*taujet_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     if(passOR){
       metJetsLCOR.push_back(*jetc_itr);
       is_jetLC = 1;
     }
   }
    
   //AntiKt4EMJets
   ConstDataVector<JetContainer> metJetsEMOR(SG::VIEW_ELEMENTS);
   bool is_jetEM = 0;
   for(jetc_itr = metJetsEM.begin(); jetc_itr != metJetsEM.end(); ++jetc_itr ) {
     TLorentzVector jettlv = (*jetc_itr)->p4();
     bool passOR = 1;
     for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr) {
       if(jettlv.DeltaR((*ele_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(pho_itr = metPhotonsOR.begin(); pho_itr != metPhotonsOR.end(); ++pho_itr) {
       if(jettlv.DeltaR((*pho_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(taujet_itr = metTausOR.begin(); taujet_itr != metTausOR.end(); ++taujet_itr) {
       if(jettlv.DeltaR((*taujet_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     if(passOR) {
       metJetsEMOR.push_back(*jetc_itr);
       is_jetEM = 1;
     }
   }

   //AntiKt4EMPFlowJets
   ConstDataVector<JetContainer> metJetsEMPFlowOR(SG::VIEW_ELEMENTS);
   bool is_jetEMPFlow = 0;
   for(jetc_itr = metJetsEMPFlow.begin(); jetc_itr != metJetsEMPFlow.end(); ++jetc_itr ) {
     TLorentzVector jettlv = (*jetc_itr)->p4();
     bool passOR = 1;
     for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr) {
       if(jettlv.DeltaR((*ele_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(pho_itr = metPhotonsOR.begin(); pho_itr != metPhotonsOR.end(); ++pho_itr) {
       if(jettlv.DeltaR((*pho_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     for(taujet_itr = metTausOR.begin(); taujet_itr != metTausOR.end(); ++taujet_itr) {
       if(jettlv.DeltaR((*taujet_itr)->p4()) < 0.2) {
	 passOR = 0;
	 break;
       }	
     }
     if(passOR) {
       metJetsEMPFlowOR.push_back(*jetc_itr);
       is_jetEMPFlow = 1;
     }
   }

   //Sum up the pT's of the objects
   TLorentzVector el_tlv;
   double sum_el = 0;
   for(ele_itr = metElectrons.begin(); ele_itr != metElectrons.end(); ++ele_itr ) {
     el_tlv += (*ele_itr)->p4();
     sum_el += (*ele_itr)->pt();
   }

   TLorentzVector mu_tlv;
   double sum_mu = 0;
   for(mu_itr = metMuons.begin(); mu_itr != metMuons.end(); ++mu_itr ) {
     mu_tlv += (*mu_itr)->p4();
     sum_mu += (*mu_itr)->pt();
   }

   TLorentzVector tau_tlv;
   double sum_tau = 0;
   for(taujet_itr = metTausOR.begin(); taujet_itr != metTausOR.end(); ++taujet_itr ) {
     tau_tlv += (*taujet_itr)->p4();
     sum_tau += (*taujet_itr)->pt();
   }

   TLorentzVector photon_tlv;
   double sum_photon = 0;
   for(pho_itr = metPhotonsOR.begin(); pho_itr != metPhotonsOR.end(); ++pho_itr ) {
     photon_tlv += (*pho_itr)->p4();
     sum_photon += (*pho_itr)->pt();
   }

   TLorentzVector jetLC_tlv;
   double sum_LC = 0;
   for(jetc_itr = metJetsLCOR.begin(); jetc_itr != metJetsLCOR.end(); ++jetc_itr ) {
     jetLC_tlv += (*jetc_itr)->p4();
     sum_LC += (*jetc_itr)->pt();
   }

   TLorentzVector jetEM_tlv;
   double sum_EM = 0;
   for(jetc_itr = metJetsEMOR.begin(); jetc_itr != metJetsEMOR.end(); ++jetc_itr ) {
     jetEM_tlv += (*jetc_itr)->p4();
     sum_EM += (*jetc_itr)->pt();
   }

   TLorentzVector jetEMPFlow_tlv;
   double sum_EMPFlow = 0;
   for(jetc_itr = metJetsEMPFlowOR.begin(); jetc_itr != metJetsEMPFlowOR.end(); ++jetc_itr ) {
     jetEMPFlow_tlv += (*jetc_itr)->p4();
     sum_EMPFlow += (*jetc_itr)->pt();
   }

   //Fill histograms
   TLorentzVector target_tlv;
   for(const auto& it : *met_RefLC) {
     if(it->name() == "RefEle"){
       if(is_electron or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[0]->Fill((target_tlv.Pt() - el_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[0]->Fill((target_tlv.Px() - el_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[0]->Fill((target_tlv.Py() - el_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[0]->Fill(el_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[0]->Fill((it->sumet() - sum_el)/1000., 1.);
       }
     }
     if(it->name() == "RefGamma"){
       if(is_photon or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[1]->Fill((target_tlv.Pt() - photon_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[1]->Fill((target_tlv.Px() - photon_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[1]->Fill((target_tlv.Py() - photon_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[1]->Fill(photon_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[1]->Fill((it->sumet() - sum_photon)/1000., 1.);
       }
     }
     if(it->name() == "RefTau"){
       if(is_tau or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[2]->Fill((it->met() - tau_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[2]->Fill((target_tlv.Px() - tau_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[2]->Fill((target_tlv.Py() - tau_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[2]->Fill(tau_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[2]->Fill((it->sumet() - sum_tau)/1000., 1.);
       }
     }
     if(it->name() == "Muons"){
       if(is_muon or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[3]->Fill((target_tlv.Pt() - mu_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[3]->Fill((target_tlv.Px() - mu_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[3]->Fill((target_tlv.Py() - mu_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[3]->Fill(mu_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[3]->Fill((it->sumet() - sum_mu)/1000., 1.);
       }
     }
     if(it->name() == "RefJet"){
       if(is_jetLC or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[4]->Fill((target_tlv.Pt() - jetLC_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[4]->Fill((target_tlv.Px() - jetLC_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[4]->Fill((target_tlv.Py() - jetLC_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[4]->Fill(jetLC_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[4]->Fill((it->sumet() - sum_LC)/1000., 1.);
       }
     }
   }

   for(const auto& it : *met_RefEM) {
     if(it->name() == "RefJet"){
       if(is_jetEM or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[5]->Fill((target_tlv.Pt() - jetEM_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[5]->Fill((target_tlv.Px() - jetEM_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[5]->Fill((target_tlv.Py() - jetEM_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[5]->Fill(jetEM_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[5]->Fill((it->sumet() - sum_EM)/1000., 1.);
       }
       break;
     }
   }

   for(const auto& it : *met_RefEMPFlow) {
     if(it->name() == "RefJet"){
       if(is_jetEMPFlow or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffRef[6]->Fill((target_tlv.Pt() - jetEMPFlow_tlv.Pt())/1000., 1.);
	 m_MET_DiffRef_x[6]->Fill((target_tlv.Px() - jetEMPFlow_tlv.Px())/1000., 1.);
	 m_MET_DiffRef_y[6]->Fill((target_tlv.Py() - jetEMPFlow_tlv.Py())/1000., 1.);
	 m_MET_DiffRef_phi[6]->Fill(jetEMPFlow_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffRef_sum[6]->Fill((it->sumet() - sum_EMPFlow)/1000., 1.);
       }
       break;
     }
   }

   // For rebuilt MET add only jets with pT>20e3
   TLorentzVector jetLCreb_tlv;
   double sum_LCreb = 0;
   for(jetc_itr = metJetsLCOR.begin(); jetc_itr != metJetsLCOR.end(); ++jetc_itr ) {
     if((*jetc_itr)->pt() > 20e3) {
       jetLCreb_tlv += (*jetc_itr)->p4();
       sum_LCreb += (*jetc_itr)->pt();
     }
   }

   TLorentzVector jetEMreb_tlv;
   double sum_EMreb = 0;
   for(jetc_itr = metJetsEMOR.begin(); jetc_itr != metJetsEMOR.end(); ++jetc_itr ) {
     if((*jetc_itr)->pt() > 20e3) {
       jetEMreb_tlv += (*jetc_itr)->p4();
       sum_EMreb += (*jetc_itr)->pt();
     }
   }

   TLorentzVector jetEMPFlowreb_tlv;
   double sum_EMPFlowreb = 0;
   for(jetc_itr = metJetsEMPFlowOR.begin(); jetc_itr != metJetsEMPFlowOR.end(); ++jetc_itr ) {
     if((*jetc_itr)->pt() > 20e3) {
       jetEMPFlowreb_tlv += (*jetc_itr)->p4();
       sum_EMPFlowreb += (*jetc_itr)->pt();
     }
   }

   for(const auto& it : *met_RebLC) {
     if(it->name() == "RefEle"){
       if(is_electron or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[0]->Fill((target_tlv.Pt() - el_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[0]->Fill((target_tlv.Px() - el_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[0]->Fill((target_tlv.Py() - el_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[0]->Fill(el_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[0]->Fill((it->sumet() - sum_el)/1000., 1.);
       }
     }
     if(it->name() == "RefGamma"){
       if(is_photon or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[1]->Fill((target_tlv.Pt() - photon_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[1]->Fill((target_tlv.Px() - photon_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[1]->Fill((target_tlv.Py() - photon_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[1]->Fill(photon_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[1]->Fill((it->sumet() - sum_photon)/1000., 1.);
       }
     }
     if(it->name() == "RefTau"){
       if(is_tau or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[2]->Fill((it->met() - tau_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[2]->Fill((target_tlv.Px() - tau_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[2]->Fill((target_tlv.Py() - tau_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[2]->Fill(tau_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[2]->Fill((it->sumet() - sum_tau)/1000., 1.);
       }
     }
     if(it->name() == "Muons"){
       if(is_muon or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[3]->Fill((target_tlv.Pt() - mu_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[3]->Fill((target_tlv.Px() - mu_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[3]->Fill((target_tlv.Py() - mu_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[3]->Fill(mu_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[3]->Fill((it->sumet() - sum_mu)/1000., 1.);
       }
     }
     if(it->name() == "RefJet"){
       if(is_jetLC or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[4]->Fill((target_tlv.Pt() - jetLCreb_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[4]->Fill((target_tlv.Px() - jetLCreb_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[4]->Fill((target_tlv.Py() - jetLCreb_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[4]->Fill(jetLCreb_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[4]->Fill((it->sumet() - sum_LCreb)/1000., 1.);
       }
     }
   }

   for(const auto& it : *met_RebEM) {
     if(it->name() == "RefJet"){
       if(is_jetEM or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[5]->Fill((target_tlv.Pt() - jetEMreb_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[5]->Fill((target_tlv.Px() - jetEMreb_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[5]->Fill((target_tlv.Py() - jetEMreb_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[5]->Fill(jetEMreb_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[5]->Fill((it->sumet() - sum_EMreb)/1000., 1.);
       }
       break;
     }
   }

   for(const auto& it : *met_RebEMPFlow) {
     if(it->name() == "RefJet"){
       if(is_jetEMPFlow or (it->sumet() > 0)){
	 target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
	 m_MET_DiffReb[6]->Fill((target_tlv.Pt() - jetEMPFlowreb_tlv.Pt())/1000., 1.);
	 m_MET_DiffReb_x[6]->Fill((target_tlv.Px() - jetEMPFlowreb_tlv.Px())/1000., 1.);
	 m_MET_DiffReb_y[6]->Fill((target_tlv.Py() - jetEMPFlowreb_tlv.Py())/1000., 1.);
	 m_MET_DiffReb_phi[6]->Fill(jetEMPFlowreb_tlv.DeltaPhi(target_tlv), 1.);
	 m_MET_DiffReb_sum[6]->Fill((it->sumet() - sum_EMPFlowreb)/1000., 1.);
       }
       break;
     }
   }

   return StatusCode::SUCCESS;
   //return StatusCode::FAILURE;
  }
  
  StatusCode PhysValMET::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");

    for(int i =0; i < 9; i++){
      m_MET_RefLC[i]->Sumw2();
      m_MET_RefLC_x[i]->Sumw2();
      m_MET_RefLC_y[i]->Sumw2();
      m_MET_RefLC_phi[i]->Sumw2();
      m_MET_RefLC_sum[i]->Sumw2();
      m_MET_RefEM[i]->Sumw2();
      m_MET_RefEM_x[i]->Sumw2();
      m_MET_RefEM_y[i]->Sumw2();
      m_MET_RefEM_phi[i]->Sumw2();
      m_MET_RefEM_sum[i]->Sumw2();
      m_MET_RefEMPFlow[i]->Sumw2();
      m_MET_RefEMPFlow_x[i]->Sumw2();
      m_MET_RefEMPFlow_y[i]->Sumw2();
      m_MET_RefEMPFlow_phi[i]->Sumw2();
      m_MET_RefEMPFlow_sum[i]->Sumw2();
      m_MET_RebLC[i]->Sumw2();
      m_MET_RebLC_x[i]->Sumw2();
      m_MET_RebLC_y[i]->Sumw2();
      m_MET_RebLC_phi[i]->Sumw2();
      m_MET_RebLC_sum[i]->Sumw2();
      m_MET_RebEM[i]->Sumw2();
      m_MET_RebEM_x[i]->Sumw2();
      m_MET_RebEM_y[i]->Sumw2();
      m_MET_RebEM_phi[i]->Sumw2();
      m_MET_RebEM_sum[i]->Sumw2();
      m_MET_RebEMPFlow[i]->Sumw2();
      m_MET_RebEMPFlow_x[i]->Sumw2();
      m_MET_RebEMPFlow_y[i]->Sumw2();
      m_MET_RebEMPFlow_phi[i]->Sumw2();
      m_MET_RebEMPFlow_sum[i]->Sumw2();
    }
    for(int i =0; i < 7; i++){
      m_MET_DiffRef[i]->Sumw2();
      m_MET_DiffRef_x[i]->Sumw2();
      m_MET_DiffRef_y[i]->Sumw2();
      m_MET_DiffRef_phi[i]->Sumw2();
      m_MET_DiffRef_sum[i]->Sumw2();
      m_MET_DiffReb[i]->Sumw2();
      m_MET_DiffReb_x[i]->Sumw2();
      m_MET_DiffReb_y[i]->Sumw2();
      m_MET_DiffReb_phi[i]->Sumw2();
      m_MET_DiffReb_sum[i]->Sumw2();
    }
    for(int i =0; i < 6; i++){
      m_MET_LC_CorrFinalTrk[i]->Sumw2();
      m_MET_EM_CorrFinalTrk[i]->Sumw2();
      m_MET_EMPFlow_CorrFinalTrk[i]->Sumw2();
      m_MET_LC_CorrFinalClus[i]->Sumw2();
      m_MET_EM_CorrFinalClus[i]->Sumw2();
      m_MET_EMPFlow_CorrFinalClus[i]->Sumw2();
    }
    m_MET_Track->Sumw2();
    m_MET_Track_x->Sumw2();
    m_MET_Track_y->Sumw2();
    m_MET_Track_phi->Sumw2();
    m_MET_Track_sum->Sumw2();
    m_MET_PVTrack_Nominal->Sumw2();
    m_MET_PVTrack_Nominal_x->Sumw2();
    m_MET_PVTrack_Nominal_y->Sumw2();
    m_MET_PVTrack_Nominal_phi->Sumw2();
    m_MET_PVTrack_Nominal_sum->Sumw2();
    m_MET_PVTrack_Pileup->Sumw2();
    m_MET_PVTrack_Pileup_x->Sumw2();
    m_MET_PVTrack_Pileup_y->Sumw2();
    m_MET_PVTrack_Pileup_phi->Sumw2();
    m_MET_PVTrack_Pileup_sum->Sumw2();
    m_Resolution_TruthNonInt_RebLC_METx->Sumw2();
    m_Resolution_TruthNonInt_RebLC_METy->Sumw2();
    m_Resolution_TruthNonInt_RebEM_METx->Sumw2();
    m_Resolution_TruthNonInt_RebEM_METy->Sumw2();
    m_Resolution_TruthNonInt_RebEMPFlow_METx->Sumw2();
    m_Resolution_TruthNonInt_RebEMPFlow_METy->Sumw2();
    m_dPhi_leadJetMET->Sumw2();
    m_dPhi_subleadJetMET->Sumw2();
    m_dPhi_LepMET->Sumw2();
    m_MET_significance_RebLC->Sumw2();
    m_MET_significance_RebEM->Sumw2();
    m_MET_significance_RebEMPFlow->Sumw2();

    int nBins = m_MET_RebLC[7]->GetNbinsX();
    for(int i=1;i<=nBins;i++){
      double err;
      m_MET_Cumu[0]->SetBinContent(i, m_MET_RebLC[7]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[0]->SetBinError(i, err);
      m_MET_Cumu[1]->SetBinContent(i, m_MET_RebLC[8]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[1]->SetBinError(i, err);
      m_MET_Cumu[2]->SetBinContent(i, m_MET_RebEM[7]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[2]->SetBinError(i, err);
      m_MET_Cumu[3]->SetBinContent(i, m_MET_RebEM[8]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[3]->SetBinError(i, err);
      m_MET_Cumu[4]->SetBinContent(i, m_MET_RebEMPFlow[7]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[4]->SetBinError(i, err);
      m_MET_Cumu[5]->SetBinContent(i, m_MET_RebEMPFlow[8]->IntegralAndError(i,nBins+1,err));
      m_MET_Cumu[5]->SetBinError(i, err);
    }
    for(int i=0;i<6;i++) m_MET_Cumu[i]->Scale(1./m_MET_Cumu[i]->GetBinContent(1));

    return StatusCode::SUCCESS;
  }
  
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  bool PhysValMET::Accept(const xAOD::Muon* mu)
  {
    if( mu->pt()<2.5e3 || mu->pt()/cosh(mu->eta())<4e3 ) return false;
    return m_muonSelTool->accept(*mu);
  }

  bool PhysValMET::Accept(const xAOD::Electron* el)
  {
    if( fabs(el->eta())>2.47 || el->pt()<10e3 ) return false;
    return m_elecSelLHTool->accept(*el);
  }

  bool PhysValMET::Accept(const xAOD::Photon* ph)
  {
    if( !(ph->author()&20) || fabs(ph->eta())>2.47 || ph->pt()<10e3 ) return false;
    return m_photonSelIsEMTool->accept(ph);
  }

  bool PhysValMET::Accept(const xAOD::TauJet* tau)
  { return m_tauSelTool->accept( *tau ); }

  bool PhysValMET::Accept(const xAOD::Jet* jet)
  {
    if( jet->pt()<0e3) return false;
    return (jet->eta() < 2.4 || jet->pt() > 50e3 || m_jvtTool->updateJvt(*jet) > 0.64);
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


}

//  LocalWords:  str 
