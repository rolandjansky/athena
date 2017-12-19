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
#include <map>
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
    m_muonSelTool(""), m_elecSelLHTool(""), m_photonSelIsEMTool(""), m_jvtTool("")
  {
    
    declareProperty( "METContainerName", m_metName = "MET_Reference_AntiKt4LCTopo" );
    //declareProperty( "METContainerName", m_metName );
    declareProperty( "InputJets",      m_jetColl   = "AntiKt4LCTopoJets" );
    declareProperty( "InputElectrons", m_eleColl   = "Electrons"         );
    declareProperty( "InputPhotons",   m_gammaColl = "Photons"           );
    declareProperty( "InputTaus",      m_tauColl   = "TauJets"           );
    declareProperty( "InputMuons",     m_muonColl  = "Muons"             );
    declareProperty( "DoTruth", m_doTruth = false );
    declareProperty( "METMapName",     m_mapname   = "METAssoc"          );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty( "MuonSelectionTool",        m_muonSelTool           );
    declareProperty( "ElectronLHSelectionTool",  m_elecSelLHTool         );
    declareProperty( "PhotonIsEMSelectionTool" , m_photonSelIsEMTool     );
    declareProperty( "TauSelectionTool",         m_tauSelTool            );
    declareProperty( "METMakerTopo",         m_metmakerTopo );
    declareProperty( "METMakerPFlow",         m_metmakerPFlow );
    declareProperty( "JVTTool",          m_jvtTool );
  }
  
  // Destructor
  ///////////////
  PhysValMET::~PhysValMET()
  {
    names.clear();
    types.clear();
    terms.clear();
    m_MET_Ref.clear();
    m_MET_Ref_x.clear();
    m_MET_Ref_y.clear();
    m_MET_Ref_phi.clear();
    m_MET_Ref_sum.clear();
    m_MET_Diff_Ref.clear();
    m_MET_Diff_Ref_x.clear();
    m_MET_Diff_Ref_y.clear();
    m_MET_Diff_Ref_phi.clear();
    m_MET_Diff_Ref_sum.clear();
    m_MET_Cumu_Ref.clear();
    m_MET_Resolution_Ref.clear();
    m_MET_Significance_Ref.clear();
    m_MET_dPhi_Ref.clear();
    m_MET_CorrFinalTrk_Ref.clear();
    m_MET_CorrFinalClus_Ref.clear();
    m_MET_Reb.clear();
    m_MET_Reb_x.clear();
    m_MET_Reb_y.clear();
    m_MET_Reb_phi.clear();
    m_MET_Reb_sum.clear();
    m_MET_Diff_Reb.clear();
    m_MET_Diff_Reb_x.clear();
    m_MET_Diff_Reb_y.clear();
    m_MET_Diff_Reb_phi.clear();
    m_MET_Diff_Reb_sum.clear();
    m_MET_Cumu_Reb.clear();
    m_MET_Resolution_Reb.clear();
    m_MET_Significance_Reb.clear();
    m_MET_dPhi_Reb.clear();
    m_MET_CorrFinalTrk_Reb.clear();
    m_MET_CorrFinalClus_Reb.clear();
}
  
  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode PhysValMET::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");    
    ATH_CHECK(ManagedMonitorToolBase::initialize());

    names.clear();
    names["RefEle"] = "Electron term";
    names["RefGamma"] = "Photon term";
    names["RefTau"] = "Tau term";
    names["Muons"] = "Muon term";
    names["RefJet"] = "Jet term";
    names["SoftClus"] = "Cluster-based soft term";
    names["PVSoftTrk"] = "Track-based soft term (PV-matched)";
    names["FinalTrk"] = "Total MET with TST";
    names["FinalClus"] = "Total MET with CST";
    names["Track"] = "Track MET, loose selection";
    names["PVTrack_Nominal"] = "Track MET for highest sum p_{T}^{2} PV";
    names["PVTrack_Pileup"] = "Track MET for each pileup vertex";

    types.clear();
    types.push_back("AntiKt4LCTopo");
    types.push_back("AntiKt4EMTopo");
    types.push_back("AntiKt4EMPFlow");

    terms.clear();
    terms.push_back("RefEle");
    terms.push_back("RefGamma");
    terms.push_back("RefTau");
    terms.push_back("Muons");
    terms.push_back("RefJet");
    terms.push_back("SoftClus");
    terms.push_back("PVSoftTrk");
    terms.push_back("FinalTrk");
    terms.push_back("FinalClus");

    ATH_MSG_INFO("Retrieving tools...");

    if( m_metmakerTopo.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve METMaker tool: " << m_metmakerTopo->name());
      return StatusCode::FAILURE;
    }

    if( m_metmakerPFlow.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve METMaker tool: " << m_metmakerPFlow->name());   
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

    m_MET_Ref.clear();
    m_MET_Ref_x.clear();
    m_MET_Ref_y.clear();
    m_MET_Ref_phi.clear();
    m_MET_Ref_sum.clear();
    m_MET_Diff_Ref.clear();
    m_MET_Diff_Ref_x.clear();
    m_MET_Diff_Ref_y.clear();
    m_MET_Diff_Ref_phi.clear();
    m_MET_Diff_Ref_sum.clear();
    m_MET_Cumu_Ref.clear();
    m_MET_Resolution_Ref.clear();
    m_MET_Significance_Ref.clear();
    m_MET_dPhi_Ref.clear();
    m_MET_CorrFinalTrk_Ref.clear();
    m_MET_CorrFinalClus_Ref.clear();
    m_MET_Reb.clear();
    m_MET_Reb_x.clear();
    m_MET_Reb_y.clear();
    m_MET_Reb_phi.clear();
    m_MET_Reb_sum.clear();
    m_MET_Diff_Reb.clear();
    m_MET_Diff_Reb_x.clear();
    m_MET_Diff_Reb_y.clear();
    m_MET_Diff_Reb_phi.clear();
    m_MET_Diff_Reb_sum.clear();
    m_MET_Cumu_Reb.clear();
    m_MET_Resolution_Reb.clear();
    m_MET_Significance_Reb.clear();
    m_MET_dPhi_Reb.clear();
    m_MET_CorrFinalTrk_Reb.clear();
    m_MET_CorrFinalClus_Reb.clear();
    
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

     for (const auto& type : types){

      	std::string name_met = "MET_Reference_" + type;
      	dir_met.clear();
      	std::vector<TH1D*> v_MET_Ref;
      	std::vector<TH1D*> v_MET_Ref_x;
      	std::vector<TH1D*> v_MET_Ref_y;
      	std::vector<TH1D*> v_MET_Ref_phi;
      	std::vector<TH1D*> v_MET_Ref_sum;
      	std::vector<TH1D*> v_MET_Cumu_Ref;
      	std::vector<TH1D*> v_MET_Resolution_Ref;
      	std::vector<TH1D*> v_MET_Significance_Ref;
      	std::vector<TH1D*> v_MET_dPhi_Ref;
      	std::vector<TH2D*> v_MET_CorrFinalTrk_Ref;
      	std::vector<TH2D*> v_MET_CorrFinalClus_Ref;
      	std::vector<TH1D*> v_MET_Diff_Ref;
      	std::vector<TH1D*> v_MET_Diff_Ref_x;
      	std::vector<TH1D*> v_MET_Diff_Ref_y;
      	std::vector<TH1D*> v_MET_Diff_Ref_phi;
      	std::vector<TH1D*> v_MET_Diff_Ref_sum;

      	for(const auto& term : terms) {
      	  v_MET_Ref.push_back( new  TH1D((name_met + "_" + term).c_str(), (name_met + " " + names[term] + "; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
      	  v_MET_Ref_x.push_back( new  TH1D((name_met + "_" + term +"_x").c_str(), (name_met + " " + names[term] + " x; E_{x}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	  v_MET_Ref_y.push_back( new  TH1D((name_met + "_" + term + "_y").c_str(), (name_met + " " + names[term] + " y; E_{y}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	  v_MET_Ref_phi.push_back( new  TH1D((name_met + "_" + term + "_phi").c_str(), (name_met + " " + names[term] + " phi; #Phi; Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi) );
      	  v_MET_Ref_sum.push_back( new  TH1D((name_met + "_" + term + "_sum").c_str(), (name_met + " " + names[term] + " sum; E_{T}^{sum} [GeV]; Entries / 25 GeV").c_str(), m_nbinE, m_lowET, m_suET) );
      	  dir_met.push_back("MET/" + name_met + "/Terms/" + term + "/");
      	}

      	m_MET_Ref[type] = v_MET_Ref;
      	m_MET_Ref_x[type] = v_MET_Ref_x;
      	m_MET_Ref_y[type] = v_MET_Ref_y;
      	m_MET_Ref_phi[type] = v_MET_Ref_phi;
      	m_MET_Ref_sum[type] = v_MET_Ref_sum;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Ref[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Ref_x[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Ref_y[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Ref_phi[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Ref_sum[type].at(i),dir_met[i],all));
      	}

      	std::string name_sub = name_met + "/Cumulative";
      	v_MET_Cumu_Ref.push_back( new  TH1D((name_met + "_Cumulative_FinalClus").c_str(), (name_met + " CST MET cumulative; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
      	v_MET_Cumu_Ref.push_back( new  TH1D((name_met + "_Cumulative_FinalTrk").c_str(), (name_met + " TST MET cumulative; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
	
      	m_MET_Cumu_Ref[type] = v_MET_Cumu_Ref;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Cumu_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Cumu_Ref[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Residuals";
      	v_MET_Resolution_Ref.push_back(  new TH1D((name_met + "_Resolution_FinalClus_x").c_str(), ("x-Residual of CST MET in " + name_met + "; #Delta(E_{T,CST}^{miss}, E_{T,truth}^{miss})_{x} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Ref.push_back(  new TH1D((name_met + "_Resolution_FinalClus_y").c_str(), ("y-Residual of CST MET in " + name_met + "; #Delta(E_{T,CST}^{miss}, E_{T,truth}^{miss})_{y} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Ref.push_back(  new TH1D((name_met + "_Resolution_FinalTrk_x").c_str(), ("x-Residual of TST MET in " + name_met + "; #Delta(E_{T,TST}^{miss}, E_{T,truth}^{miss})_{x} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Ref.push_back(  new TH1D((name_met + "_Resolution_FinalTrk_y").c_str(), ("y-Residual of TST MET in " + name_met + "; #Delta(E_{T,TST}^{miss}, E_{T,truth}^{miss})_{y} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	
      	m_MET_Resolution_Ref[type] = v_MET_Resolution_Ref;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Resolution_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Resolution_Ref[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Significance";
      	v_MET_Significance_Ref.push_back(  new TH1D((name_met + "_Significance_FinalClus").c_str(), ("MET / sqrt(sumet) for " + name_met + " CST; MET/#sqrt{SET} [#sqrt{GeV}]; Entries / 0.25 #sqrt{GeV}").c_str(), m_nbinp, 0., 25.) );
      	v_MET_Significance_Ref.push_back(  new TH1D((name_met + "_Significance_FinalTrk").c_str(), ("MET / sqrt(sumet) for " + name_met + " TST; MET/#sqrt{SET} [#sqrt{GeV}]; Entries / 0.25 #sqrt{GeV}").c_str(), m_nbinp, 0., 25.) );
	
      	m_MET_Significance_Ref[type] = v_MET_Significance_Ref;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Significance_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Significance_Ref[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/dPhi";
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_leadJetMET_FinalClus").c_str(), ("MET deltaPhi vs leading jet for " + name_met + " CST; #Delta#Phi(leadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_subleadJetMET_FinalClus").c_str(), ("MET deltaPhi vs subleading jet for " + name_met + " CST; #Delta#Phi(subleadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_leadLepMET_FinalClus").c_str(), ("MET deltaPhi vs leading lepton for " + name_met + " CST; #Delta#Phi(leadLep, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_leadJetMET_FinalTrk").c_str(), ("MET deltaPhi vs leading jet for " + name_met + " TST; #Delta#Phi(leadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_subleadJetMET_FinalTrk").c_str(), ("MET deltaPhi vs subleading jet for " + name_met + " TST; #Delta#Phi(subleadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Ref.push_back(  new TH1D((name_met + "_dPhi_leadLepMET_FinalTrk").c_str(), ("MET deltaPhi vs leading lepton for " + name_met + " TST; #Delta#Phi(leadLep, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
	
      	m_MET_dPhi_Ref[type] = v_MET_dPhi_Ref;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_dPhi_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_dPhi_Ref[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Correlations";
      	std::vector<std::string> corrClus_names;
      	corrClus_names.push_back("RefEle");
      	corrClus_names.push_back("RefGamma");
      	corrClus_names.push_back("RefTau");
      	corrClus_names.push_back("Muons");
      	corrClus_names.push_back("RefJet");
      	corrClus_names.push_back("SoftClus");
      	std::vector<std::string> corrTrk_names;
      	corrTrk_names.push_back("RefEle");
      	corrTrk_names.push_back("RefGamma");
      	corrTrk_names.push_back("RefTau");
      	corrTrk_names.push_back("Muons");
      	corrTrk_names.push_back("RefJet");
      	corrTrk_names.push_back("PVSoftTrk");

      	for(const auto& it : corrClus_names) {
      	  v_MET_CorrFinalClus_Ref.push_back( new  TH2D((name_met + "_" + it + "_FinalClus").c_str(), (name_met + " " + names[it] + " vs. CST MET; E_{T," + it + "}^{miss} [GeV]; E_{T,CST}^{miss} [GeV]; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      	}
      	for(const auto& it : corrTrk_names) {
      	  v_MET_CorrFinalTrk_Ref.push_back( new  TH2D((name_met + "_" + it + "_FinalTrk").c_str(), (name_met + " " + names[it] + " vs. TST MET; E_{T," + it + "}^{miss} [GeV]; E_{T,TST}^{miss} [GeV]; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      	}

      	m_MET_CorrFinalClus_Ref[type] = v_MET_CorrFinalClus_Ref;
      	m_MET_CorrFinalTrk_Ref[type] = v_MET_CorrFinalTrk_Ref;

      	for(std::vector<TH2D*>::size_type i = 0; i < v_MET_CorrFinalTrk_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_CorrFinalTrk_Ref[type].at(i),"MET/" + name_sub + "/",all));
	}
      	for(std::vector<TH2D*>::size_type i = 0; i < v_MET_CorrFinalClus_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_CorrFinalClus_Ref[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	std::vector<std::string> sum_names;
      	sum_names.push_back("RefEle");
      	sum_names.push_back("RefGamma");
      	sum_names.push_back("RefTau");
      	sum_names.push_back("Muons");
      	sum_names.push_back("RefJet");

      	dir_met.clear();

      	for(const auto& it : sum_names) {
      	  v_MET_Diff_Ref.push_back( new  TH1D((name_met + "_Diff_" + it).c_str(), ("MET_Diff " + names[it] + " in " + name_met +"; E_{T}^{miss} - #Sigma p_{T} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150));
      	  v_MET_Diff_Ref_x.push_back( new  TH1D((name_met + "_Diff_" + it +"_x").c_str(), ("MET_Diff x " + names[it] + " in " + name_met +"; E_{x}^{miss} - #Sigma p_{x} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150) );
      	  v_MET_Diff_Ref_y.push_back( new  TH1D((name_met + "_Diff_" + it +"_y").c_str(), ("MET_Diff y " + names[it] + " in " + name_met +"; E_{y}^{miss} - #Sigma p_{y} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150) );
      	  v_MET_Diff_Ref_phi.push_back( new  TH1D((name_met + "_Diff_" + it +"_phi").c_str(), ("MET_Diff phi " + names[it] + " in " + name_met +"; #Delta#Phi(E_{T}^{miss},#Sigma p_{T}); Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi) );
      	  v_MET_Diff_Ref_sum.push_back( new  TH1D((name_met + "_Diff_" + it +"_sum").c_str(), ("MET_Diff sumet " + names[it] + " in " + name_met +"; E_{T}^{sum} - #Sigma |p_{T}| [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -250, 250) );
      	  dir_met.push_back("MET/" + name_met + "/Differences/" + it + "/");
      	}

      	m_MET_Diff_Ref[type] = v_MET_Diff_Ref;
      	m_MET_Diff_Ref_x[type] = v_MET_Diff_Ref_x;
      	m_MET_Diff_Ref_y[type] = v_MET_Diff_Ref_y;
      	m_MET_Diff_Ref_phi[type] = v_MET_Diff_Ref_phi;
      	m_MET_Diff_Ref_sum[type] = v_MET_Diff_Ref_sum;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Diff_Ref.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Diff_Ref[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Ref_x[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Ref_y[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Ref_phi[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Ref_sum[type].at(i),dir_met[i],all));
      	}
	

      	//-------------------------------------------------------------------------------------
      	//Now the same for Rebuilt MET

      	name_met = "MET_Rebuilt_" + type;
      	dir_met.clear();
      	std::vector<TH1D*> v_MET_Reb;
      	std::vector<TH1D*> v_MET_Reb_x;
      	std::vector<TH1D*> v_MET_Reb_y;
      	std::vector<TH1D*> v_MET_Reb_phi;
      	std::vector<TH1D*> v_MET_Reb_sum;
      	std::vector<TH1D*> v_MET_Cumu_Reb;
      	std::vector<TH1D*> v_MET_Resolution_Reb;
      	std::vector<TH1D*> v_MET_Significance_Reb;
      	std::vector<TH1D*> v_MET_dPhi_Reb;
      	std::vector<TH2D*> v_MET_CorrFinalTrk_Reb;
      	std::vector<TH2D*> v_MET_CorrFinalClus_Reb;
      	std::vector<TH1D*> v_MET_Diff_Reb;
      	std::vector<TH1D*> v_MET_Diff_Reb_x;
      	std::vector<TH1D*> v_MET_Diff_Reb_y;
      	std::vector<TH1D*> v_MET_Diff_Reb_phi;
      	std::vector<TH1D*> v_MET_Diff_Reb_sum;

      	for(const auto& term : terms) {
      	  v_MET_Reb.push_back( new  TH1D((name_met + "_" + term).c_str(), (name_met + " " + names[term] + "; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
      	  v_MET_Reb_x.push_back( new  TH1D((name_met + "_" + term + "_x").c_str(), (name_met + " " + names[term] + " x; E_{x}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	  v_MET_Reb_y.push_back( new  TH1D((name_met + "_" + term + "_y").c_str(), (name_met + " " + names[term] + " y; E_{y}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	  v_MET_Reb_phi.push_back( new  TH1D((name_met + "_" + term + "_phi").c_str(), (name_met + " " + names[term] + " phi; #Phi; Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi) );
      	  v_MET_Reb_sum.push_back( new  TH1D((name_met + "_" + term + "_sum").c_str(), (name_met + " " + names[term] + " sum; E_{T}^{sum} [GeV]; Entries / 25 GeV").c_str(), m_nbinE, m_lowET, m_suET) );
      	  dir_met.push_back("MET/" + name_met + "/Terms/" + term + "/");
      	}

      	m_MET_Reb[type] = v_MET_Reb;
      	m_MET_Reb_x[type] = v_MET_Reb_x;
      	m_MET_Reb_y[type] = v_MET_Reb_y;
      	m_MET_Reb_phi[type] = v_MET_Reb_phi;
      	m_MET_Reb_sum[type] = v_MET_Reb_sum;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Reb[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Reb_x[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Reb_y[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Reb_phi[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Reb_sum[type].at(i),dir_met[i],all));
      	}

      	name_sub = name_met + "/Cumulative";
      	v_MET_Cumu_Reb.push_back( new  TH1D((name_met + "_Cumulative_FinalClus").c_str(), (name_met + " CST MET cumulative; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
      	v_MET_Cumu_Reb.push_back( new  TH1D((name_met + "_Cumulative_FinalTrk").c_str(), (name_met + " TST MET cumulative; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi) );
	
      	m_MET_Cumu_Reb[type] = v_MET_Cumu_Reb;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Cumu_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Cumu_Reb[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Residuals";
      	v_MET_Resolution_Reb.push_back(  new TH1D((name_met + "_Resolution_FinalClus_x").c_str(), ("x-Residual of CST MET in " + name_met + "; #Delta(E_{T,CST}^{miss}, E_{T,truth}^{miss})_{x} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Reb.push_back(  new TH1D((name_met + "_Resolution_FinalClus_y").c_str(), ("y-Residual of CST MET in " + name_met + "; #Delta(E_{T,CST}^{miss}, E_{T,truth}^{miss})_{y} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Reb.push_back(  new TH1D((name_met + "_Resolution_FinalTrk_x").c_str(), ("x-Residual of TST MET in " + name_met + "; #Delta(E_{T,TST}^{miss}, E_{T,truth}^{miss})_{x} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	v_MET_Resolution_Reb.push_back(  new TH1D((name_met + "_Resolution_FinalTrk_y").c_str(), ("y-Residual of TST MET in " + name_met + "; #Delta(E_{T,TST}^{miss}, E_{T,truth}^{miss})_{y} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
      	m_MET_Resolution_Reb[type] = v_MET_Resolution_Reb;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Resolution_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Resolution_Reb[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Significance";
      	v_MET_Significance_Reb.push_back(  new TH1D((name_met + "_Significance_FinalClus").c_str(), ("MET / sqrt(sumet) for " + name_met + " CST; MET/#sqrt{SET} [#sqrt{GeV}]; Entries / 0.25 #sqrt{GeV}").c_str(), m_nbinp, 0., 25.) );
      	v_MET_Significance_Reb.push_back(  new TH1D((name_met + "_Significance_FinalTrk").c_str(), ("MET / sqrt(sumet) for " + name_met + " TST; MET/sqrt{SET} [#sqrt{GeV}]; Entries / 0.25 #sqrt{GeV}").c_str(), m_nbinp, 0., 25.) );
	
      	m_MET_Significance_Reb[type] = v_MET_Significance_Reb;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Significance_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Significance_Reb[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/dPhi";
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_leadJetMET_FinalClus").c_str(), ("MET deltaPhi vs leading jet for " + name_met + " CST; #Delta#Phi(leadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_subleadJetMET_FinalClus").c_str(), ("MET deltaPhi vs subleading jet for " + name_met + " CST; #Delta#Phi(subleadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_leadLepMET_FinalClus").c_str(), ("MET deltaPhi vs leading lepton for " + name_met + " CST; #Delta#Phi(leadLep, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_leadJetMET_FinalTrk").c_str(), ("MET deltaPhi vs leading jet for " + name_met + " TST; #Delta#Phi(leadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_subleadJetMET_FinalTrk").c_str(), ("MET deltaPhi vs subleading jet for " + name_met + " TST; #Delta#Phi(subleadJet, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
      	v_MET_dPhi_Reb.push_back(  new TH1D((name_met + "_dPhi_leadLepMET_FinalTrk").c_str(), ("MET deltaPhi vs leading lepton for " + name_met + " TST; #Delta#Phi(leadLep, MET); Entries / 0.05").c_str(), m_nbinphi, 0., m_binphi) );
	
      	m_MET_dPhi_Reb[type] = v_MET_dPhi_Reb;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_dPhi_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_dPhi_Reb[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	name_sub = name_met + "/Correlations";
      	for(const auto& it : corrClus_names) {
      	  v_MET_CorrFinalClus_Reb.push_back( new  TH2D((name_met + "_" + it + "_FinalClus").c_str(), (name_met + " " + names[it] + " vs. CST MET; E_{T," + it + "}^{miss} [GeV]; E_{T,CST}^{miss} [GeV]; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      	}
      	for(const auto& it : corrTrk_names) {
      	  v_MET_CorrFinalTrk_Reb.push_back( new  TH2D((name_met + "_" + it + "_FinalTrk").c_str(), (name_met + " " + names[it] + " vs. TST MET; E_{T," + it + "}^{miss} [GeV]; E_{T,TST}^{miss} [GeV]; Entries").c_str(), m_nbinp, 0., m_suptmi, m_nbinp, 0., m_suptmi) );
      	}

      	m_MET_CorrFinalClus_Reb[type] = v_MET_CorrFinalClus_Reb;
      	m_MET_CorrFinalTrk_Reb[type] = v_MET_CorrFinalTrk_Reb;

      	for(std::vector<TH2D*>::size_type i = 0; i < v_MET_CorrFinalTrk_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_CorrFinalTrk_Reb[type].at(i),"MET/" + name_sub + "/",all));
	}
      	for(std::vector<TH2D*>::size_type i = 0; i < v_MET_CorrFinalClus_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_CorrFinalClus_Reb[type].at(i),"MET/" + name_sub + "/",all));
      	}

      	dir_met.clear();

      	for(const auto& it : sum_names) {
      	  v_MET_Diff_Reb.push_back( new  TH1D((name_met + "_Diff_" + it).c_str(), ("MET_Diff " + names[it] + " in " + name_met +"; E_{T}^{miss} - #Sigma p_{T} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150));
      	  v_MET_Diff_Reb_x.push_back( new  TH1D((name_met + "_Diff_" + it + "_x").c_str(), ("MET_Diff x " + names[it] + " in " + name_met +"; E_{x}^{miss} - #Sigma p_{x} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150) );
      	  v_MET_Diff_Reb_y.push_back( new  TH1D((name_met + "_Diff_" + it + "_y").c_str(), ("MET_Diff y " + names[it] + " in " + name_met +"; E_{y}^{miss} - #Sigma p_{y} [GeV]; Entries / 3 GeV").c_str(), m_nbinpxy, -150, 150) );
      	  v_MET_Diff_Reb_phi.push_back( new  TH1D((name_met + "_Diff_" + it + "_phi").c_str(), ("MET_Diff phi " + names[it] + " in " + name_met +"; #Delta#Phi(E_{T}^{miss}, #Sigma p_{T}); Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi) );
      	  v_MET_Diff_Reb_sum.push_back( new  TH1D((name_met + "_Diff_" + it + "_sum").c_str(), ("MET_Diff sumet " + names[it] + " in " + name_met +"; E_{T}^{sum} - #Sigma |p_{T}| [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -250, 250) );
      	  dir_met.push_back("MET/" + name_met + "/Differences/" + it + "/");
      	}

      	m_MET_Diff_Reb[type] = v_MET_Diff_Reb;
      	m_MET_Diff_Reb_x[type] = v_MET_Diff_Reb_x;
      	m_MET_Diff_Reb_y[type] = v_MET_Diff_Reb_y;
      	m_MET_Diff_Reb_phi[type] = v_MET_Diff_Reb_phi;
      	m_MET_Diff_Reb_sum[type] = v_MET_Diff_Reb_sum;

      	for(std::vector<TH1D*>::size_type i = 0; i < v_MET_Diff_Reb.size(); ++i) {
      	  ATH_CHECK(regHist(m_MET_Diff_Reb[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Reb_x[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Reb_y[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Reb_phi[type].at(i),dir_met[i],all));
      	  ATH_CHECK(regHist(m_MET_Diff_Reb_sum[type].at(i),dir_met[i],all));
      	}
     }


	//-------------------------------------------------------------------------------------
	//Now MET_Track

      std::string name_met = "MET_Track";
      std::string dir = "MET/" + name_met + "/";

      std::string sub_dir = dir + "Track/";
      ATH_CHECK(regHist(m_MET_Track = new  TH1D("Track", (name_met + " " + names["Track"] + "; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_Track_x = new  TH1D("Track_x", (name_met + " " + names["Track"] + " x; E_{x}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_Track_y = new  TH1D("Track_y", (name_met + " " + names["Track"] + " y; E_{y}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_Track_phi = new  TH1D("Track_phi", (name_met + " " + names["Track"] + " phi;  #Phi; Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_Track_sum = new  TH1D("Track_sum", (name_met + " " + names["Track"] + " sum; E_{T}^{sum} [GeV]; Entries / 25 GeV").c_str(), m_nbinE, m_lowET, m_suET), sub_dir, all));

      sub_dir = dir + "PVTrack_Nominal/";
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal = new TH1D("PVTrack_Nominal", (name_met + " " + names["PVTrack_Nominal"] + " ; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_x = new  TH1D("PVTrack_Nominal_x", (name_met + " " + names["PVTrack_Nominal"] + " x; E_{x}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_y = new  TH1D("PVTrack_Nominal_y", (name_met + " " + names["PVTrack_Nominal"] + " y; E_{y}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_phi = new  TH1D("PVTrack_Nominal_phi", (name_met + " " + names["PVTrack_Nominal"] + " phi; #Phi; Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Nominal_sum = new  TH1D("PVTrack_Nominal_sum", (name_met + " " + names["PVTrack_Nominal"] + " sum; E_{T}^{sum} [GeV]; Entries / 25 GeV").c_str(), m_nbinE, m_lowET, m_suET), sub_dir, all));

      sub_dir = dir + "PVTrack_Pileup/";
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup = new  TH1D("PVTrack_Pileup", (name_met + " " + names["PVTrack_Pileup"] + "; E_{T}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinp, 0., m_suptmi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_x = new  TH1D("PVTrack_Pileup_x", (name_met + " " + names["PVTrack_Pileup"] + " x; E_{x}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_y = new  TH1D("PVTrack_Pileup_y", (name_met +" " +  names["PVTrack_Pileup"] + " y; E_{y}^{miss} [GeV]; Entries / 5 GeV").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_phi = new  TH1D("PVTrack_Pileup_phi", (name_met + " " + names["PVTrack_Pileup"] + " phi; #Phi; Entries / 0.1").c_str(), m_nbinphi,-m_binphi,m_binphi), sub_dir, all));
      ATH_CHECK(regHist(m_MET_PVTrack_Pileup_sum = new  TH1D("PVTrack_Pileup_sum", (name_met + " " + names["PVTrack_Pileup"] + " sum; E_{T}^{sum} [GeV]; Entries / 25 GeV").c_str(), m_nbinE, m_lowET, m_suET), sub_dir, all));

    }

    return StatusCode::SUCCESS;      
  }

  StatusCode PhysValMET::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "...");

    //Retrieve MET Truth
    const xAOD::MissingETContainer* met_Truth = 0;
    if(m_doTruth) {
      ATH_CHECK( evtStore()->retrieve(met_Truth,"MET_Truth") );
      if (!met_Truth) {
	ATH_MSG_ERROR ( "Failed to retrieve MET_Truth. Exiting." );
	return StatusCode::FAILURE;
      }
    }

    //Retrieve MET Track
    const xAOD::MissingETContainer* met_Track = 0;
    ATH_CHECK( evtStore()->retrieve(met_Track,"MET_Track") );
    if (!met_Track) {
      ATH_MSG_ERROR ( "Failed to retrieve MET_Track. Exiting." );
      return StatusCode::FAILURE;
    }

    //Physics Objects
    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve(muons,m_muonColl) );
    if (!muons) {
      ATH_MSG_ERROR ( "Failed to retrieve Muon container. Exiting." );
      return StatusCode::FAILURE;
    }
    ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
    bool is_muon = 0;
    for(const auto& mu : *muons) {
      if(Accept(mu)) {
	metMuons.push_back(mu);
	is_muon = 1;
      }
    }

    const xAOD::ElectronContainer* electrons = 0;
    ATH_CHECK( evtStore()->retrieve(electrons,m_eleColl) );
    if (!electrons) {
      ATH_MSG_ERROR ( "Failed to retrieve Electron container. Exiting." );
      return StatusCode::FAILURE;
    }
   ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
   bool is_electron = 0;
   for(const auto& el : *electrons) {
     if(Accept(el)) {
       metElectrons.push_back(el);
       is_electron = 1;
     }
   }

    const xAOD::PhotonContainer* photons = 0;
    ATH_CHECK( evtStore()->retrieve(photons,m_gammaColl) );
    if (!electrons) {
      ATH_MSG_ERROR ( "Failed to retrieve Photon container. Exiting." );
      return StatusCode::FAILURE;
    }
    ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
    for(const auto& ph : *photons) {
      if(Accept(ph)) {
	metPhotons.push_back(ph);
      }
    }

    const TauJetContainer* taus = 0;
    ATH_CHECK( evtStore()->retrieve(taus, m_tauColl) );
    if(!taus) {
      ATH_MSG_ERROR("Failed to retrieve TauJet container: " << m_tauColl);
      return StatusCode::SUCCESS;
    }
    ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
    for(const auto& tau : *taus) {
      if(Accept(tau)) {
	metTaus.push_back(tau);
      }
    }

    // Overlap removal

    ConstDataVector<PhotonContainer>::iterator pho_itr;
    ConstDataVector<ElectronContainer>::iterator ele_itr;
    ConstDataVector<TauJetContainer>::iterator taujet_itr;
    ConstDataVector<MuonContainer>::iterator mu_itr;
    ConstDataVector<JetContainer>::iterator jetc_itr;

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

    for (const auto& type : types){
      std::string name_met = "MET_Reference_" + type;
    
      // Retrieve Reference MET
      const xAOD::MissingETContainer* met_Ref = 0;
      ATH_CHECK( evtStore()->retrieve(met_Ref, name_met) );
      if (!met_Ref) {
    	ATH_MSG_ERROR ("Couldn't retrieve " << name_met);
    	return StatusCode::FAILURE;                   
      } 

      // Retrieve Jets
      std::string name_jet = type + "Jets";
      const xAOD::JetContainer* jets = 0;
      ATH_CHECK( evtStore()->retrieve(jets,name_jet) );
      if (!jets) {
    	ATH_MSG_ERROR ( "Failed to retrieve Jet container: " << name_jet << ". Exiting." );
    	return StatusCode::FAILURE;
      }
      for(auto jet : *jets) {
	float newjvt = m_jvtTool->updateJvt(*jet); 
	jet->auxdecor<float>("NewJvt") = newjvt;
      }
      ConstDataVector<JetContainer> metJets(SG::VIEW_ELEMENTS);
      for(const auto& jet : *jets) {
	metJets.push_back(jet);
      }
      //Overlap Removal
      ConstDataVector<JetContainer> metJetsOR(SG::VIEW_ELEMENTS);
      bool is_jet = 0;
      for(jetc_itr = metJets.begin(); jetc_itr != metJets.end(); ++jetc_itr ) {
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
    	  metJetsOR.push_back(*jetc_itr);
    	  is_jet = 1;
    	}
      }

      TLorentzVector jet_tlv;
      double sum_jet = 0;
      for(jetc_itr = metJetsOR.begin(); jetc_itr != metJetsOR.end(); ++jetc_itr ) {
    	jet_tlv += (*jetc_itr)->p4();
    	sum_jet += (*jetc_itr)->pt();
      }

      // Fill MET_Ref
      ATH_MSG_INFO( "  MET_Ref_" << type << ":" );
      for(const auto& it : *met_Ref) {
    	std::string name = it->name();
    	if(name == "RefEle"){
    	  (m_MET_Ref[type]).at(0)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(0)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(0)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(0)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(0)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefGamma"){
    	  (m_MET_Ref[type]).at(1)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(1)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(1)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(1)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(1)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefTau"){
    	  (m_MET_Ref[type]).at(2)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(2)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(2)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(2)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(2)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "Muons"){
    	  (m_MET_Ref[type]).at(3)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(3)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(3)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(3)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(3)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefJet"){
    	  (m_MET_Ref[type]).at(4)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(4)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(4)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(4)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(4)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "SoftClus"){
    	  (m_MET_Ref[type]).at(5)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(5)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(5)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(5)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(5)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "PVSoftTrk"){
    	  (m_MET_Ref[type]).at(6)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(6)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(6)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(6)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(6)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "FinalTrk"){
    	  (m_MET_Ref[type]).at(7)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(7)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(7)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(7)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(7)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "FinalClus"){
    	  (m_MET_Ref[type]).at(8)->Fill((*met_Ref)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Ref_x[type]).at(8)->Fill((*met_Ref)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Ref_y[type]).at(8)->Fill((*met_Ref)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Ref_phi[type]).at(8)->Fill((*met_Ref)[name.c_str()]->phi(), 1.);
    	  (m_MET_Ref_sum[type]).at(8)->Fill((*met_Ref)[name.c_str()]->sumet()/1000., 1.);
    	}
      }

      //Prepare Rebuilding MET
      ATH_MSG_INFO( "  Rebuilding MET_" << type );
      MissingETContainer* met_Reb = new MissingETContainer();
      if( evtStore()->record(met_Reb,("MET_Rebuilt"+type).c_str()).isFailure() ) {
    	ATH_MSG_WARNING("Unable to record MissingETContainer: MET_Rebuilt_" << type);
    	return StatusCode::FAILURE;
      }
      MissingETAuxContainer* met_RebAux = new MissingETAuxContainer();
      if( evtStore()->record(met_RebAux,("MET_Rebuilt"+type+"Aux").c_str()).isFailure() ) {
    	ATH_MSG_WARNING("Unable to record MissingETAuxContainer: MET_Rebuilt" << type);
    	return StatusCode::FAILURE;
      }
      met_Reb->setStore(met_RebAux);

      m_mapname = "METAssoc_"+type;
      m_corename = "MET_Core_"+type;
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

      ATH_MSG_INFO( "  MET_Rebuilt_" << type << ":" );
      //Select and flag objects for final MET building ***************************
      if( type.find("PFlow") != std::string::npos) m_metmaker = m_metmakerPFlow;
      else m_metmaker = m_metmakerTopo;

      // Electrons
      if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, met_Reb, metElectrons.asDataVector(), metMap).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build electron term.");
      }

      // Photons
      if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, met_Reb, metPhotons.asDataVector(), metMap).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build photon term.");
      }

      // Taus
      if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, met_Reb,metTaus.asDataVector(),metMap).isFailure() ){
    	ATH_MSG_WARNING("Failed to build tau term.");
      }

      // Muons
      if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, met_Reb, metMuons.asDataVector(), metMap).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build muon term.");
      }

      // Jets
      if( m_metmaker->rebuildJetMET("RefJet", "SoftClus", "PVSoftTrk", met_Reb, jets, coreMet, metMap, true).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build jet and soft terms.");
      }
      MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
      if((*met_Reb)["PVSoftTrk"]) trksource = (*met_Reb)["PVSoftTrk"]->source();
      if( m_metmaker->buildMETSum("FinalTrk", met_Reb, trksource).isFailure() ){
    	ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
      }
      MissingETBase::Types::bitmask_t clsource;
      if (type == "AntiKt4LCTopo") clsource = MissingETBase::Source::LCTopo;
      else if (type == "AntiKt4EMTopo") clsource = MissingETBase::Source::EMTopo;
      else clsource = MissingETBase::Source::UnknownSignal;
      
      if((*met_Reb)["SoftClus"]) clsource = (*met_Reb)["SoftClus"]->source();
      if( m_metmaker->buildMETSum("FinalClus", met_Reb, clsource).isFailure() ) {
    	ATH_MSG_WARNING("Building MET FinalClus sum failed.");
      }

      // Fill MET_Ref
      for(const auto& it : *met_Reb) {
    	std::string name = it->name();
    	if(name == "RefEle"){
    	  (m_MET_Reb[type]).at(0)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(0)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(0)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(0)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(0)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefGamma"){
    	  (m_MET_Reb[type]).at(1)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(1)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(1)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(1)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(1)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefTau"){
    	  (m_MET_Reb[type]).at(2)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(2)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(2)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(2)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(2)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "Muons"){
    	  (m_MET_Reb[type]).at(3)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(3)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(3)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(3)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(3)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "RefJet"){
    	  (m_MET_Reb[type]).at(4)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(4)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(4)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(4)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(4)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "SoftClus"){
    	  (m_MET_Reb[type]).at(5)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(5)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(5)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(5)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(5)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "PVSoftTrk"){
    	  (m_MET_Reb[type]).at(6)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(6)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(6)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(6)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(6)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "FinalTrk"){
    	  (m_MET_Reb[type]).at(7)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(7)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(7)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(7)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(7)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
    	if(name == "FinalClus"){
    	  (m_MET_Reb[type]).at(8)->Fill((*met_Reb)[name.c_str()]->met()/1000., 1.);
    	  (m_MET_Reb_x[type]).at(8)->Fill((*met_Reb)[name.c_str()]->mpx()/1000., 1.);
    	  (m_MET_Reb_y[type]).at(8)->Fill((*met_Reb)[name.c_str()]->mpy()/1000., 1.);
    	  (m_MET_Reb_phi[type]).at(8)->Fill((*met_Reb)[name.c_str()]->phi(), 1.);
    	  (m_MET_Reb_sum[type]).at(8)->Fill((*met_Reb)[name.c_str()]->sumet()/1000., 1.);
    	}
      }

      //Fill MET Angles
      ATH_MSG_INFO( "  MET_Angles :" );

      double leadPt = 0., subleadPt = 0., leadPhi = 0., subleadPhi = 0.;

      xAOD::JetContainer::const_iterator jet_itr = jets->begin();
      xAOD::JetContainer::const_iterator jet_end = jets->end();

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

      (m_MET_dPhi_Ref[type]).at(0)->Fill( -remainder( leadPhi - (*met_Reb)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Ref[type]).at(1)->Fill( -remainder( subleadPhi - (*met_Reb)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Ref[type]).at(3)->Fill( -remainder( leadPhi - (*met_Reb)["FinalTrk"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Ref[type]).at(4)->Fill( -remainder( subleadPhi - (*met_Reb)["FinalTrk"]->phi(), 2*M_PI ) );
    
      (m_MET_dPhi_Reb[type]).at(0)->Fill( -remainder( leadPhi - (*met_Reb)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Reb[type]).at(1)->Fill( -remainder( subleadPhi - (*met_Reb)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Reb[type]).at(3)->Fill( -remainder( leadPhi - (*met_Reb)["FinalTrk"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Reb[type]).at(4)->Fill( -remainder( subleadPhi - (*met_Reb)["FinalTrk"]->phi(), 2*M_PI ) );
  

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

      (m_MET_dPhi_Ref[type]).at(2)->Fill( -remainder( leadPhi - (*met_Ref)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Ref[type]).at(5)->Fill( -remainder( leadPhi - (*met_Ref)["FinalTrk"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Reb[type]).at(2)->Fill( -remainder( leadPhi - (*met_Reb)["FinalClus"]->phi(), 2*M_PI ) );
      (m_MET_dPhi_Reb[type]).at(5)->Fill( -remainder( leadPhi - (*met_Reb)["FinalTrk"]->phi(), 2*M_PI ) );
    
    
      //Fill Correlation Plots
      //Reference
      for(const auto& it : *met_Ref) {
    	std::string name = it->name();
    	if(name == "RefEle"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(0)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Ref[type]).at(0)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefGamma"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(1)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Ref[type]).at(1)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefTau"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(2)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Ref[type]).at(2)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "Muons"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(3)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Ref[type]).at(3)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefJet"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(4)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Ref[type]).at(4)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "PVSoftTrk"){
    	  (m_MET_CorrFinalTrk_Ref[type]).at(5)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalTrk"]->met()/1000., 1.);
    	}
    	if(name == "SoftClus"){
    	  (m_MET_CorrFinalClus_Ref[type]).at(5)->Fill((*met_Ref)[name.c_str()]->met()/1000.,(*met_Ref)["FinalClus"]->met()/1000., 1.);
    	}
      }

      //Rebuilt
      for(const auto& it : *met_Reb) {
    	std::string name = it->name();
    	if(name == "RefEle"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(0)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Reb[type]).at(0)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefGamma"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(1)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Reb[type]).at(1)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefTau"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(2)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Reb[type]).at(2)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "Muons"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(3)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Reb[type]).at(3)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "RefJet"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(4)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	  (m_MET_CorrFinalClus_Reb[type]).at(4)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
    	if(name == "PVSoftTrk"){
    	  (m_MET_CorrFinalTrk_Reb[type]).at(5)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalTrk"]->met()/1000., 1.);
    	}
    	if(name == "SoftClus"){
    	  (m_MET_CorrFinalClus_Reb[type]).at(5)->Fill((*met_Reb)[name.c_str()]->met()/1000.,(*met_Reb)["FinalClus"]->met()/1000., 1.);
    	}
      }

      // Fill Resolution
      if(m_doTruth)
    	{
    	  ATH_MSG_INFO( "  Resolution:" );
    	  (m_MET_Resolution_Ref[type]).at(0)->Fill(((*met_Ref)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    	  (m_MET_Resolution_Ref[type]).at(1)->Fill(((*met_Ref)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
    	  (m_MET_Resolution_Ref[type]).at(2)->Fill(((*met_Ref)["FinalTrk"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    	  (m_MET_Resolution_Ref[type]).at(3)->Fill(((*met_Ref)["FinalTrk"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
    	  (m_MET_Resolution_Reb[type]).at(0)->Fill(((*met_Reb)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    	  (m_MET_Resolution_Reb[type]).at(1)->Fill(((*met_Reb)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
    	  (m_MET_Resolution_Reb[type]).at(2)->Fill(((*met_Reb)["FinalTrk"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    	  (m_MET_Resolution_Reb[type]).at(3)->Fill(((*met_Reb)["FinalTrk"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);
    	}

      //Fill MET Significance
      ATH_MSG_INFO( "  MET_significance:" );
      if( (*met_Ref)["FinalClus"]->sumet() != 0) (m_MET_Significance_Ref[type]).at(0)->Fill((*met_Ref)["FinalClus"]->met()/sqrt((*met_Ref)["FinalClus"]->sumet()*1000.), 1.);
      if( (*met_Ref)["FinalTrk"]->sumet() != 0) (m_MET_Significance_Ref[type]).at(1)->Fill((*met_Ref)["FinalTrk"]->met()/sqrt((*met_Ref)["FinalTrk"]->sumet()*1000.), 1.);
      if( (*met_Reb)["FinalClus"]->sumet() != 0) (m_MET_Significance_Reb[type]).at(0)->Fill((*met_Reb)["FinalClus"]->met()/sqrt((*met_Reb)["FinalClus"]->sumet()*1000.), 1.);
      if( (*met_Reb)["FinalTrk"]->sumet() != 0) (m_MET_Significance_Reb[type]).at(1)->Fill((*met_Reb)["FinalTrk"]->met()/sqrt((*met_Reb)["FinalTrk"]->sumet()*1000.), 1.);

      //Fill Diff histograms
      TLorentzVector target_tlv;
      for(const auto& it : *met_Ref) {
    	if(it->name() == "RefEle"){
    	  if(is_electron or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Ref[type]).at(0)->Fill((target_tlv.Pt() - el_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Ref_x[type]).at(0)->Fill((target_tlv.Px() - el_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Ref_y[type]).at(0)->Fill((target_tlv.Py() - el_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Ref_phi[type]).at(0)->Fill(el_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Ref_sum[type]).at(0)->Fill((it->sumet() - sum_el)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefGamma"){
    	  if(is_photon or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Ref[type]).at(1)->Fill((target_tlv.Pt() - photon_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Ref_x[type]).at(1)->Fill((target_tlv.Px() - photon_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Ref_y[type]).at(1)->Fill((target_tlv.Py() - photon_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Ref_phi[type]).at(1)->Fill(photon_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Ref_sum[type]).at(1)->Fill((it->sumet() - sum_photon)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefTau"){
    	  if(is_tau or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Ref[type]).at(2)->Fill((target_tlv.Pt() - tau_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Ref_x[type]).at(2)->Fill((target_tlv.Px() - tau_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Ref_y[type]).at(2)->Fill((target_tlv.Py() - tau_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Ref_phi[type]).at(2)->Fill(tau_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Ref_sum[type]).at(2)->Fill((it->sumet() - sum_tau)/1000., 1.);
    	  }
    	}
    	if(it->name() == "Muons"){
    	  if(is_muon or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Ref[type]).at(3)->Fill((target_tlv.Pt() - mu_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Ref_x[type]).at(3)->Fill((target_tlv.Px() - mu_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Ref_y[type]).at(3)->Fill((target_tlv.Py() - mu_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Ref_phi[type]).at(3)->Fill(mu_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Ref_sum[type]).at(3)->Fill((it->sumet() - sum_mu)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefJet"){
    	  if(is_jet or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Ref[type]).at(4)->Fill((target_tlv.Pt() - jet_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Ref_x[type]).at(4)->Fill((target_tlv.Px() - jet_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Ref_y[type]).at(4)->Fill((target_tlv.Py() - jet_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Ref_phi[type]).at(4)->Fill(jet_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Ref_sum[type]).at(4)->Fill((it->sumet() - sum_jet)/1000., 1.);
    	  }
    	}
      }

      // For rebuilt MET add only jets with pT>20e3 and JVT cut
      TLorentzVector jetReb_tlv;
      double sum_jetReb = 0;
      double JvtCut = 0.59;
      if (type == "AntiKt4EMPFlow") JvtCut = 0.2;
      for(const auto jet : metJetsOR) {
    	if(Accept(jet, JvtCut)) {
    	  jetReb_tlv += jet->p4();
    	  sum_jetReb += jet->pt();
    	}
      }

      for(const auto& it : *met_Reb) {
    	if(it->name() == "RefEle"){
    	  if(is_electron or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Reb[type]).at(0)->Fill((target_tlv.Pt() - el_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Reb_x[type]).at(0)->Fill((target_tlv.Px() - el_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Reb_y[type]).at(0)->Fill((target_tlv.Py() - el_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Reb_phi[type]).at(0)->Fill(el_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Reb_sum[type]).at(0)->Fill((it->sumet() - sum_el)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefGamma"){
    	  if(is_photon or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Reb[type]).at(1)->Fill((target_tlv.Pt() - photon_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Reb_x[type]).at(1)->Fill((target_tlv.Px() - photon_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Reb_y[type]).at(1)->Fill((target_tlv.Py() - photon_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Reb_phi[type]).at(1)->Fill(photon_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Reb_sum[type]).at(1)->Fill((it->sumet() - sum_photon)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefTau"){
    	  if(is_tau or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Reb[type]).at(2)->Fill((target_tlv.Pt() - tau_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Reb_x[type]).at(2)->Fill((target_tlv.Px() - tau_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Reb_y[type]).at(2)->Fill((target_tlv.Py() - tau_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Reb_phi[type]).at(2)->Fill(tau_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Reb_sum[type]).at(2)->Fill((it->sumet() - sum_tau)/1000., 1.);
    	  }
    	}
    	if(it->name() == "Muons"){
    	  if(is_muon or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Reb[type]).at(3)->Fill((target_tlv.Pt() - mu_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Reb_x[type]).at(3)->Fill((target_tlv.Px() - mu_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Reb_y[type]).at(3)->Fill((target_tlv.Py() - mu_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Reb_phi[type]).at(3)->Fill(mu_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Reb_sum[type]).at(3)->Fill((it->sumet() - sum_mu)/1000., 1.);
    	  }
    	}
    	if(it->name() == "RefJet"){
    	  if(is_jet or (it->sumet() > 0)){
    	    target_tlv.SetPxPyPzE(-it->mpx(), -it->mpy(), 0, it->met());
    	    (m_MET_Diff_Reb[type]).at(4)->Fill((target_tlv.Pt() - jetReb_tlv.Pt())/1000., 1.);
    	    (m_MET_Diff_Reb_x[type]).at(4)->Fill((target_tlv.Px() - jetReb_tlv.Px())/1000., 1.);
    	    (m_MET_Diff_Reb_y[type]).at(4)->Fill((target_tlv.Py() - jetReb_tlv.Py())/1000., 1.);
    	    (m_MET_Diff_Reb_phi[type]).at(4)->Fill(jetReb_tlv.DeltaPhi(target_tlv), 1.);
    	    (m_MET_Diff_Reb_sum[type]).at(4)->Fill((it->sumet() - sum_jetReb)/1000., 1.);
    	  }
    	}
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

   return StatusCode::SUCCESS;
   //return StatusCode::FAILURE;
  }
  
  StatusCode PhysValMET::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");

    for (const auto& type : types){
      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_Ref[type]).size(); ++i) {
    	(m_MET_Ref[type]).at(i)->Sumw2();
    	(m_MET_Ref_x[type]).at(i)->Sumw2();
    	(m_MET_Ref_y[type]).at(i)->Sumw2();
    	(m_MET_Ref_phi[type]).at(i)->Sumw2();
    	(m_MET_Ref_sum[type]).at(i)->Sumw2();
    	(m_MET_Reb[type]).at(i)->Sumw2();
    	(m_MET_Reb_x[type]).at(i)->Sumw2();
    	(m_MET_Reb_y[type]).at(i)->Sumw2();
    	(m_MET_Reb_phi[type]).at(i)->Sumw2();
    	(m_MET_Reb_sum[type]).at(i)->Sumw2();
      }

      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_Diff_Ref[type]).size(); ++i) {
    	(m_MET_Diff_Ref[type]).at(i)->Sumw2();
    	(m_MET_Diff_Ref_x[type]).at(i)->Sumw2();
    	(m_MET_Diff_Ref_y[type]).at(i)->Sumw2();
    	(m_MET_Diff_Ref_phi[type]).at(i)->Sumw2();
    	(m_MET_Diff_Ref_sum[type]).at(i)->Sumw2();
    	(m_MET_Diff_Reb[type]).at(i)->Sumw2();
    	(m_MET_Diff_Reb_x[type]).at(i)->Sumw2();
    	(m_MET_Diff_Reb_y[type]).at(i)->Sumw2();
    	(m_MET_Diff_Reb_phi[type]).at(i)->Sumw2();
    	(m_MET_Diff_Reb_sum[type]).at(i)->Sumw2();
      }

      for(std::vector<TH2D*>::size_type i = 0; i < (m_MET_CorrFinalTrk_Ref[type]).size(); ++i) {
    	(m_MET_CorrFinalTrk_Ref[type]).at(i)->Sumw2();
    	(m_MET_CorrFinalTrk_Reb[type]).at(i)->Sumw2();
      }

      for(std::vector<TH2D*>::size_type i = 0; i < (m_MET_CorrFinalClus_Ref[type]).size(); ++i) {
    	(m_MET_CorrFinalClus_Ref[type]).at(i)->Sumw2();
    	(m_MET_CorrFinalClus_Reb[type]).at(i)->Sumw2();
      }

      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_Significance_Ref[type]).size(); ++i) {
    	(m_MET_Significance_Ref[type]).at(i)->Sumw2();
    	(m_MET_Significance_Reb[type]).at(i)->Sumw2();
      }

      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_Resolution_Ref[type]).size(); ++i) {
    	(m_MET_Resolution_Ref[type]).at(i)->Sumw2();
    	(m_MET_Resolution_Reb[type]).at(i)->Sumw2();
      }

      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_dPhi_Ref[type]).size(); ++i) {
    	(m_MET_dPhi_Ref[type]).at(i)->Sumw2();
    	(m_MET_dPhi_Reb[type]).at(i)->Sumw2();
      }

      int nBins = (m_MET_Ref[type]).at(7)->GetNbinsX();
      for(int i=1;i<=nBins;i++){
      	double err;
      	(m_MET_Cumu_Ref[type]).at(0)->SetBinContent(i, (m_MET_Ref[type]).at(8)->IntegralAndError(i,nBins+1,err));
      	(m_MET_Cumu_Ref[type]).at(0)->SetBinError(i, err);
      	(m_MET_Cumu_Ref[type]).at(1)->SetBinContent(i, (m_MET_Ref[type]).at(7)->IntegralAndError(i,nBins+1,err));
      	(m_MET_Cumu_Ref[type]).at(1)->SetBinError(i, err);
      	(m_MET_Cumu_Reb[type]).at(0)->SetBinContent(i, (m_MET_Reb[type]).at(8)->IntegralAndError(i,nBins+1,err));
      	(m_MET_Cumu_Reb[type]).at(0)->SetBinError(i, err);
      	(m_MET_Cumu_Reb[type]).at(1)->SetBinContent(i, (m_MET_Reb[type]).at(7)->IntegralAndError(i,nBins+1,err));
      	(m_MET_Cumu_Reb[type]).at(1)->SetBinError(i, err);
      }
      for(std::vector<TH1D*>::size_type i = 0; i < (m_MET_Cumu_Ref[type]).size(); ++i) {
      	m_MET_Cumu_Ref[type].at(i)->Scale(1./(m_MET_Cumu_Ref[type]).at(i)->GetBinContent(1));
      	m_MET_Cumu_Reb[type].at(i)->Scale(1./(m_MET_Cumu_Reb[type]).at(i)->GetBinContent(1));
      }

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
    return m_elecSelLHTool->accept(el);
  }

  bool PhysValMET::Accept(const xAOD::Photon* ph)
  {
    if( !(ph->author()&20) || fabs(ph->eta())>2.47 || ph->pt()<10e3 ) return false;
    return m_photonSelIsEMTool->accept(ph);
  }

  bool PhysValMET::Accept(const xAOD::TauJet* tau)
  { return m_tauSelTool->accept( *tau ); }

  bool PhysValMET::Accept(const xAOD::Jet* jet, double JvtCut)
  {
    if( jet->pt()<20e3) return false;
    return (fabs(jet->eta()) > 2.4 || jet->pt() > 60e3 || m_jvtTool->updateJvt(*jet) > JvtCut);
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
