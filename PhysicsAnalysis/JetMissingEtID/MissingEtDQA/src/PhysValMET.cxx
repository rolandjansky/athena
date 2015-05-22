///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValMET.cxx 
// Implementation file for class PhysValMET
// Author: Daniel Buescher <daniel.buescher@cern.ch>
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
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODMissingET/MissingETComposition.h"

#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

using xAOD::MissingETContainer;
using xAOD::MissingETComposition;
using xAOD::MissingET;

namespace MissingEtDQA {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

  PhysValMET::PhysValMET( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent ) : 
    ManagedMonitorToolBase( type, name, parent ),
    m_Resolution_TruthNonInt_RefFinal_METx(0), m_Resolution_TruthNonInt_RefFinal_METy(0),
    m_dPhi_leadJetMET(0), m_dPhi_subleadJetMET(0), m_dPhi_LepMET(0),
    m_MET_significance(0) 
  {
    
    declareProperty( "METContainerName", m_metName = "MET_Reference_AntiKt4LCTopo" );
    //declareProperty( "METContainerName", m_metName );
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

	// TODO: loop over available MET terms
	//for (MissingETContainer::iterator it = met_Simple->begin(); it != met_Simple-> end(); it++) (*it)->name()

	//std::string name_met = "MET_RefFinal";
	std::string name_met = m_metName;
	const xAOD::MissingETContainer* met_RefFinal = 0;
	ATH_CHECK( evtStore()->retrieve(met_RefFinal, m_metName) );

	for(const auto& it : *met_RefFinal) {
	  m_MET_RefFinal.push_back( new  TH1D((it->name()).c_str(), (name_met + " " + it->name() + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
	  m_MET_RefFinal_x.push_back( new  TH1D((it->name()+"_x").c_str(), (name_met + " " + it->name() + "_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	  m_MET_RefFinal_y.push_back( new  TH1D((it->name()+"_y").c_str(), (name_met + " " + it->name() + "_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
	  m_MET_RefFinal_phi.push_back( new  TH1D((it->name()+"_phi").c_str(), (name_met + " " + it->name() + "_phi; E_{T}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
	  m_MET_RefFinal_sum.push_back( new  TH1D((it->name()+"_sum").c_str(), (name_met + " " + it->name() + "_sum; E_{T}^{miss}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
	  dir_met.push_back("MET/" + name_met + "/" + it->name() + "/");
	}

	for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RefFinal.size(); ++i) {
          ATH_CHECK(regHist(m_MET_RefFinal[i],dir_met[i],all));
	  ATH_CHECK(regHist(m_MET_RefFinal_x[i],dir_met[i],all));
	  ATH_CHECK(regHist(m_MET_RefFinal_y[i],dir_met[i],all));
	  ATH_CHECK(regHist(m_MET_RefFinal_phi[i],dir_met[i],all));
	  ATH_CHECK(regHist(m_MET_RefFinal_sum[i],dir_met[i],all));
        }


	name_met = "MET_Reference_AntiKt4EMTopo";
        const xAOD::MissingETContainer* met_EM = 0;
        ATH_CHECK( evtStore()->retrieve(met_EM, name_met) );

	dir_met.clear();
        for(const auto& it : *met_EM) {
          m_MET_EM.push_back( new  TH1D((it->name()).c_str(), (name_met + " " + it->name() + "; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi) );
          m_MET_EM_x.push_back( new  TH1D((it->name()+"_x").c_str(), (name_met + " " + it->name() + "_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
          m_MET_EM_y.push_back( new  TH1D((it->name()+"_y").c_str(), (name_met + " " + it->name() + "_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy) );
          m_MET_EM_phi.push_back( new  TH1D((it->name()+"_phi").c_str(), (name_met + " " + it->name() + "_phi; E_{T}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi) );
          m_MET_EM_sum.push_back( new  TH1D((it->name()+"_sum").c_str(), (name_met + " " + it->name() + "_sum; E_{T}^{miss}; Entries").c_str(), m_nbinE, m_lowET, m_suET) );
          dir_met.push_back("MET/" + name_met + "/" + it->name() + "/");
        }

	for(std::vector<TH1D*>::size_type i = 0; i < m_MET_EM.size(); ++i) {
          ATH_CHECK(regHist(m_MET_EM[i],dir_met[i],all));
          ATH_CHECK(regHist(m_MET_EM_x[i],dir_met[i],all));
          ATH_CHECK(regHist(m_MET_EM_y[i],dir_met[i],all));
          ATH_CHECK(regHist(m_MET_EM_phi[i],dir_met[i],all));
          ATH_CHECK(regHist(m_MET_EM_sum[i],dir_met[i],all));
        }

		
	name_met = "MET_Track";
	std::string dir = "MET/" + name_met + "/";

	ATH_CHECK(regHist(m_MET_Track = new  TH1D("Track", (name_met + " Track; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
	ATH_CHECK(regHist(m_MET_Track_x = new  TH1D("Track_x", (name_met + " Track_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_Track_y = new  TH1D("Track_y", (name_met + " Track_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_Track_phi = new  TH1D("Track_phi", (name_met + " Track_phi; E_{T}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
        ATH_CHECK(regHist(m_MET_Track_sum = new  TH1D("Track_sum", (name_met + " Track_sum; E_{T}^{miss}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));

	ATH_CHECK(regHist(m_MET_PVTrack_Nominal = new TH1D("PVTrack_Nominal", (name_met + " PVTrack_Nominal; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
	ATH_CHECK(regHist(m_MET_PVTrack_Nominal_x = new  TH1D("PVTrack_Nominal_x", (name_met + " PVTrack_Nominal_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Nominal_y = new  TH1D("PVTrack_Nominal_y", (name_met + " PVTrack_Nominal_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Nominal_phi = new  TH1D("PVTrack_Nominal_phi", (name_met + " PVTrack_Nominal_phi; E_{T}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Nominal_sum = new  TH1D("PVTrack_Nominal_sum", (name_met + " PVTrack_Nominal_sum; E_{T}^{miss}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));

	ATH_CHECK(regHist(m_MET_PVTrack_Pileup = new  TH1D("PVTrack_Pileup", (name_met + " PVTrack_Pileup; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir, all));
	ATH_CHECK(regHist(m_MET_PVTrack_Pileup_x = new  TH1D("PVTrack_Pileup_x", (name_met + " PVTrack_Pileup_x; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Pileup_y = new  TH1D("PVTrack_Pileup_y", (name_met + " PVTrack_Pileup_y; E_{T}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Pileup_phi = new  TH1D("PVTrack_Pileup_phi", (name_met + " PVTrack_Pileup_phi; E_{T}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir, all));
        ATH_CHECK(regHist(m_MET_PVTrack_Pileup_sum = new  TH1D("PVTrack_Pileup_sum", (name_met + " PVTrack_Pileup_sum; E_{T}^{miss}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir, all));


	name_met = "MET_Resolution";
        dir = "MET/" + name_met + "/";
        ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METx = new TH1D("m_Resolution_TruthNonInt_RefFinal_METx", (name_met + " Resolution_TruthNonInt_RefFinal_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));
        ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METy = new TH1D("m_Resolution_TruthNonInt_RefFinal_METy", (name_met + " Resolution_TruthNonInt_RefFinal_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir, all));

	name_met = "MET_Significance";
        dir = "MET/" + name_met + "/";
        ATH_CHECK(regHist(m_MET_significance = new TH1D("MET_Significance", (name_met + " MET_Significance; MET/sqrt(SET); Entries").c_str(), m_nbinp, 0., 200.), dir, all));

        name_met = "MET_dPhi";
        dir = "MET/" + name_met + "/";
        ATH_CHECK(regHist(m_dPhi_leadJetMET = new TH1D("dPhi_leadJetMET", (name_met + " dPhi_leadJetMET; #Delta#Phi(leadJet,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));
        ATH_CHECK(regHist(m_dPhi_subleadJetMET = new TH1D("dPhi_subleadJetMET", (name_met + " dPhi_subleadJetMET; #Delta#Phi(subleadJet,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));
        ATH_CHECK(regHist(m_dPhi_LepMET = new TH1D("dPhi_LepMET", (name_met + " dPhi_LepMET; #Delta#Phi(Lep,MET); Entries").c_str(), m_nbinphi, 0., 3.14), dir, all));


      }

      return StatusCode::SUCCESS;      
  }

  StatusCode PhysValMET::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "...");

    // Retrieve MET container: 
    //m_metName = "MET_Reference_AntiKt4LCTopo";
    
    const xAOD::MissingETContainer* met_RefFinal = 0;
    ATH_CHECK( evtStore()->retrieve(met_RefFinal, m_metName) );
 
   if (!met_RefFinal) {
      ATH_MSG_ERROR ("Couldn't retrieve MET Final");
      return StatusCode::FAILURE;                   
    } 

   const xAOD::MissingETContainer* met_EM = 0;
   ATH_CHECK( evtStore()->retrieve(met_EM,"MET_Reference_AntiKt4EMTopo") );

    const xAOD::MissingETContainer* met_Truth = 0;
    ATH_CHECK( evtStore()->retrieve(met_Truth,"MET_Truth") );

    const xAOD::MissingETContainer* met_Track = 0;
    ATH_CHECK( evtStore()->retrieve(met_Track,"MET_Track") );
       
    
            
    ATH_MSG_INFO( "  MET_RefFinal:" );

    for(std::vector<TH1D*>::size_type i = 0; i < m_MET_RefFinal.size(); ++i) {
      m_MET_RefFinal[i]->Fill((*met_RefFinal)[m_MET_RefFinal[i]->GetName()]->met()/1000., 1.);
      m_MET_RefFinal_x[i]->Fill((*met_RefFinal)[m_MET_RefFinal[i]->GetName()]->mpx()/1000., 1.);
      m_MET_RefFinal_y[i]->Fill((*met_RefFinal)[m_MET_RefFinal[i]->GetName()]->mpy()/1000., 1.);
      m_MET_RefFinal_phi[i]->Fill((*met_RefFinal)[m_MET_RefFinal[i]->GetName()]->phi(), 1.);
      m_MET_RefFinal_sum[i]->Fill((*met_RefFinal)[m_MET_RefFinal[i]->GetName()]->sumet()/1000., 1.);
    }

    m_MET_significance->Fill((*met_RefFinal)["FinalClus"]->met()/sqrt((*met_RefFinal)["FinalClus"]->sumet()/sqrt(1000.)), 1.);

    for(std::vector<TH1D*>::size_type i = 0; i < m_MET_EM.size(); ++i) {
      m_MET_EM[i]->Fill((*met_EM)[m_MET_EM[i]->GetName()]->met()/1000., 1.);
      m_MET_EM_x[i]->Fill((*met_EM)[m_MET_EM[i]->GetName()]->mpx()/1000., 1.);
      m_MET_EM_y[i]->Fill((*met_EM)[m_MET_EM[i]->GetName()]->mpy()/1000., 1.);
      m_MET_EM_phi[i]->Fill((*met_EM)[m_MET_EM[i]->GetName()]->phi(), 1.);
      m_MET_EM_sum[i]->Fill((*met_EM)[m_MET_EM[i]->GetName()]->sumet()/1000., 1.);
    }

       
    ATH_MSG_INFO( "  Resolution:" );

    m_Resolution_TruthNonInt_RefFinal_METx->Fill(((*met_RefFinal)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    m_Resolution_TruthNonInt_RefFinal_METy->Fill(((*met_RefFinal)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);


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
    
    

    ATH_MSG_INFO( "  MET_Angles :" );

    double leadPt = 0., subleadPt = 0., leadPhi = 0., subleadPhi = 0.;

    const xAOD::JetContainer* jets = 0;
    ATH_CHECK( evtStore()->retrieve(jets,"AntiKt4LCTopoJets") );
    if (!jets) {
      ATH_MSG_ERROR ( "Failed to retrieve AntiKt4LCTopoJets container. Exiting." );
      return StatusCode::FAILURE;
    }

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

    m_dPhi_leadJetMET->Fill( -remainder( leadPhi - (*met_RefFinal)["FinalClus"]->phi(), 2*M_PI ) );
    m_dPhi_subleadJetMET->Fill( -remainder( subleadPhi - (*met_RefFinal)["FinalClus"]->phi(), 2*M_PI ) );
    

    leadPt = 0.; leadPhi = 0.;

    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve(muons,"Muons") );
    if (!muons) {
      ATH_MSG_ERROR ( "Failed to retrieve Muons container. Exiting." );
      return StatusCode::FAILURE;
    }

    xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
    xAOD::MuonContainer::const_iterator muon_end = muons->end();

    for( ; muon_itr != muon_end; ++muon_itr ) {
      if((*muon_itr)->pt() > leadPt) {
	leadPt = (*muon_itr)->pt();
	leadPhi = (*muon_itr)->phi();
      }
    }

    const xAOD::ElectronContainer* electrons = 0;
    ATH_CHECK( evtStore()->retrieve(electrons,"Electrons") );
    if (!electrons) {
      ATH_MSG_ERROR ( "Failed to retrieve ElectronCollection container. Exiting." );
      return StatusCode::FAILURE;
    }

    xAOD::ElectronContainer::const_iterator electron_itr = electrons->begin();
    xAOD::ElectronContainer::const_iterator electron_end = electrons->end();

    for( ; electron_itr != electron_end; ++electron_itr ) {
      if((*electron_itr)->pt() > leadPt) {
	leadPt = (*electron_itr)->pt();
	leadPhi = (*electron_itr)->phi();
      }
    }

    m_dPhi_LepMET->Fill( -remainder( leadPhi - (*met_RefFinal)["FinalClus"]->phi(), 2*M_PI ) );

   

    return StatusCode::SUCCESS;
    //return StatusCode::FAILURE;
  }
  
  StatusCode PhysValMET::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    return StatusCode::SUCCESS;
  }
  
/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

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
