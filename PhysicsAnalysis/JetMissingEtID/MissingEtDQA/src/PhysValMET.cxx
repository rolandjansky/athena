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

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODMissingET/MissingET.h" 
#include "xAODMissingET/MissingETContainer.h" 
//#include "MissingET/MissingET.h" 
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODMissingET/MissingETComposition.h"

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
    m_MET_RefFinal_phi(0)
  {
    
    declareProperty( "METContainerName", m_metName = "MET_RefFinal" ); 
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

	std::string dir_met("MET/" + m_metName + "/");
	ATH_CHECK(regHist(m_MET_RefFinal = new TH1D("MET_RefFinal", (m_metName + " MET_RefFinal; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_x = new TH1D("MET_RefFinal_x", (m_metName + " MET_RefFinal_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_y = new TH1D("MET_RefFinal_y", (m_metName + " MET_RefFinal_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_phi = new TH1D("MET_RefFinal_phi", (m_metName + " MET_RefFinal_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefFinal = new TH1D("SET_RefFinal", (m_metName + " SET_RefFinal; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	m_metName = "MET_RefEle"; 
	dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_RefEle = new TH1D("MET_RefEle", (m_metName + " MET_RefEle; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_x = new TH1D("MET_RefEle_x", (m_metName + " MET_RefEle_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_y = new TH1D("MET_RefEle_y", (m_metName + " MET_RefEle_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_phi = new TH1D("MET_RefEle_phi", (m_metName + " MET_RefEle_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefEle = new TH1D("SET_RefEle", (m_metName + " SET_RefEle; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	m_metName = "MET_RefGamma";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_RefGamma = new TH1D("MET_RefGamma", (m_metName + " MET_RefGamma; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_x = new TH1D("MET_RefGamma_x", (m_metName + " MET_RefGamma_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_y = new TH1D("MET_RefGamma_y", (m_metName + " MET_RefGamma_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_phi = new TH1D("MET_RefGamma_phi", (m_metName + " MET_RefGamma_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefGamma = new TH1D("SET_RefGamma", (m_metName + " SET_RefGamma; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	m_metName = "MET_RefTau";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_RefTau = new TH1D("MET_RefTau", (m_metName + " MET_RefTau; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_x = new TH1D("MET_RefTau_x", (m_metName + " MET_RefTau_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_y = new TH1D("MET_RefTau_y", (m_metName + " MET_RefTau_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_phi = new TH1D("MET_RefTau_phi", (m_metName + " MET_RefTau_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefTau = new TH1D("SET_RefTau", (m_metName + " SET_RefTau; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	m_metName = "MET_RefJet";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_RefJet = new TH1D("MET_RefJet", (m_metName + " MET_RefJet; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_x = new TH1D("MET_RefJet_x", (m_metName + " MET_RefJet_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_y = new TH1D("MET_RefJet_y", (m_metName + " MET_RefJet_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_phi = new TH1D("MET_RefJet_phi", (m_metName + " MET_RefJet_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefJet = new TH1D("SET_RefJet", (m_metName + " SET_RefJet; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	m_metName = "MET_Muons";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_Muons = new TH1D("MET_Muons", (m_metName + " MET_Muons; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_x = new TH1D("MET_Muons_x", (m_metName + " MET_Muons_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_y = new TH1D("MET_Muons_y", (m_metName + " MET_Muons_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_phi = new TH1D("MET_Muons_phi", (m_metName + " MET_Muons_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_Muons = new TH1D("SET_Muons", (m_metName + " SET_Muons; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	m_metName = "MET_SoftClus";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_SoftClus = new TH1D("MET_SoftClus", (m_metName + " MET_SoftClus; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_x = new TH1D("MET_SoftClus_x", (m_metName + " MET_SoftClus_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_y = new TH1D("MET_SoftClus_y", (m_metName + " MET_SoftClus_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_phi = new TH1D("MET_SoftClus_phi", (m_metName + " MET_SoftClus_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_SoftClus = new TH1D("SET_SoftClus", (m_metName + " SET_SoftClus; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/2.), dir_met, all));

	m_metName = "MET_PVSoftTrk";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_PVSoftTrk = new TH1D("MET_PVSoftTrk", (m_metName + " MET_PVSoftTrk; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_x = new TH1D("MET_PVSoftTrk_x", (m_metName + " MET_PVSoftTrk_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_y = new TH1D("MET_PVSoftTrk_y", (m_metName + " MET_PVSoftTrk_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_phi = new TH1D("MET_PVSoftTrk_phi", (m_metName + " MET_PVSoftTrk_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_PVSoftTrk = new TH1D("SET_PVSoftTrk", (m_metName + " SET_PVSoftTrk; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/2.), dir_met, all));

	m_metName = "MET_RefFinal_TST";
	dir_met = "MET/" + m_metName + "/";
        ATH_CHECK(regHist(m_MET_RefFinal_TST = new TH1D("MET_RefFinal_TST", (m_metName + " MET_RefFinal_TST; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
        ATH_CHECK(regHist(m_MET_RefFinal_TST_x = new TH1D("MET_RefFinal_TST_x", (m_metName + " MET_RefFinal_TST_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_RefFinal_TST_y = new TH1D("MET_RefFinal_TST_y", (m_metName + " MET_RefFinal_TST_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        //ATH_CHECK(regHist(m_MET_RefFinal_TST_phi = new TH1D("MET_RefFinal_TST_phi", (m_metName + " MET_RefFinal_TST_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
        //ATH_CHECK(regHist(m_SET_RefFinal_TST = new TH1D("SET_RefFinal_TST", (m_metName + " SET_RefFinal_TST; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	m_metName = "MET_Resolution";
	dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METx = new TH1D("m_Resolution_TruthNonInt_RefFinal_METx", (m_metName + " Resolution_TruthNonInt_RefFinal_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METy = new TH1D("m_Resolution_TruthNonInt_RefFinal_METy", (m_metName + " Resolution_TruthNonInt_RefFinal_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir_met, all));

	m_metName = "MET_Track";
        dir_met = "MET/" + m_metName + "/";
	ATH_CHECK(regHist(m_MET_Track = new TH1D("MET_Track", (m_metName + " MET_Track; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_x = new TH1D("MET_Track_x", (m_metName + " MET_Track_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_y = new TH1D("MET_Track_y", (m_metName + " MET_Track_y; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_phi = new TH1D("MET_Track_phi", (m_metName + " MET_Track_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_Track = new TH1D("SET_Track", (m_metName + " SET_Track; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	m_metName = "MET_TrackPV";
        dir_met = "MET/" + m_metName + "/";
        ATH_CHECK(regHist(m_MET_TrackPV = new TH1D("MET_TrackPV", (m_metName + " MET_TrackPV; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_x = new TH1D("MET_TrackPV_x", (m_metName + " MET_TrackPV_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_y = new TH1D("MET_TrackPV_y", (m_metName + " MET_TrackPV_y; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_phi = new TH1D("MET_TrackPV_phi", (m_metName + " MET_TrackPV_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
        ATH_CHECK(regHist(m_SET_TrackPV = new TH1D("SET_TrackPV", (m_metName + " SET_TrackPV; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

      }

      return StatusCode::SUCCESS;      
  }

  StatusCode PhysValMET::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "...");

    // Retrieve MET container: 

    const xAOD::MissingETContainer* met_RefFinal = 0;
    ATH_CHECK( evtStore()->retrieve(met_RefFinal,"MET_RefFinal") );

    const xAOD::MissingETContainer* met_Truth = 0;
    ATH_CHECK( evtStore()->retrieve(met_Truth,"MET_Truth") );

    const xAOD::MissingETContainer* met_Track = 0;
    ATH_CHECK( evtStore()->retrieve(met_Track,"MET_Track") );
       
    // if (!met_RefFinal) {
    //   ATH_MSG_ERROR ("Couldn't retrieve MET Final");
    //   return StatusCode::FAILURE;                   
    // } 

        
    ATH_MSG_INFO( "  MET magnitude:" );


    //const MissingET* metFinal = (*met_RefFinal)["Final"];


    //m_MET_RefFinal_phi->Fill((*met_RefFinal)["Final"]->phi(), 1.);

    m_MET_RefFinal->Fill((*met_RefFinal)["Final"]->met()/1000., 1.);
    m_MET_RefEle->Fill((*met_RefFinal)["RefEle"]->met()/1000., 1.);
    m_MET_RefGamma->Fill((*met_RefFinal)["RefGamma"]->met()/1000., 1.);
    m_MET_RefTau->Fill((*met_RefFinal)["RefTau"]->met()/1000., 1.);
    m_MET_RefJet->Fill((*met_RefFinal)["RefJet"]->met()/1000., 1.);
    m_MET_Muons->Fill((*met_RefFinal)["Muons"]->met()/1000., 1.);
    m_MET_SoftClus->Fill((*met_RefFinal)["SoftClus"]->met()/1000., 1.);
    m_MET_PVSoftTrk->Fill((*met_RefFinal)["PVSoftTrk"]->met()/1000., 1.);
    

    ATH_MSG_INFO( "  MET x-component:" );

    m_MET_RefFinal_x->Fill((*met_RefFinal)["Final"]->mpx()/1000., 1.);
    m_MET_RefEle_x->Fill((*met_RefFinal)["RefEle"]->mpx()/1000., 1.);
    m_MET_RefGamma_x->Fill((*met_RefFinal)["RefGamma"]->mpx()/1000., 1.);
    m_MET_RefTau_x->Fill((*met_RefFinal)["RefTau"]->mpx()/1000., 1.);
    m_MET_RefJet_x->Fill((*met_RefFinal)["RefJet"]->mpx()/1000., 1.);
    m_MET_Muons_x->Fill((*met_RefFinal)["Muons"]->mpx()/1000., 1.);
    m_MET_SoftClus_x->Fill((*met_RefFinal)["SoftClus"]->mpx()/1000., 1.);
    m_MET_PVSoftTrk_x->Fill((*met_RefFinal)["PVSoftTrk"]->mpx()/1000., 1.);
    

    ATH_MSG_INFO( "  MET y-component:" );

    m_MET_RefFinal_y->Fill((*met_RefFinal)["Final"]->mpy()/1000., 1.);
    m_MET_RefEle_y->Fill((*met_RefFinal)["RefEle"]->mpy()/1000., 1.);
    m_MET_RefGamma_y->Fill((*met_RefFinal)["RefGamma"]->mpy()/1000., 1.);
    m_MET_RefTau_y->Fill((*met_RefFinal)["RefTau"]->mpy()/1000., 1.);
    m_MET_RefJet_y->Fill((*met_RefFinal)["RefJet"]->mpy()/1000., 1.);
    m_MET_Muons_y->Fill((*met_RefFinal)["Muons"]->mpy()/1000., 1.);
    m_MET_SoftClus_y->Fill((*met_RefFinal)["SoftClus"]->mpy()/1000., 1.);
    m_MET_PVSoftTrk_y->Fill((*met_RefFinal)["PVSoftTrk"]->mpy()/1000., 1.);


    ATH_MSG_INFO( "  MET phi-component:" );

    m_MET_RefFinal_phi->Fill((*met_RefFinal)["Final"]->phi(), 1.);
    m_MET_RefEle_phi->Fill((*met_RefFinal)["RefEle"]->phi(), 1.);
    m_MET_RefGamma_phi->Fill((*met_RefFinal)["RefGamma"]->phi(), 1.);
    m_MET_RefTau_phi->Fill((*met_RefFinal)["RefTau"]->phi(), 1.);
    m_MET_RefJet_phi->Fill((*met_RefFinal)["RefJet"]->phi(), 1.);
    m_MET_Muons_phi->Fill((*met_RefFinal)["Muons"]->phi(), 1.);
    m_MET_SoftClus_phi->Fill((*met_RefFinal)["SoftClus"]->phi(), 1.);
    m_MET_PVSoftTrk_phi->Fill((*met_RefFinal)["PVSoftTrk"]->phi(), 1.);


    ATH_MSG_INFO( "  SumET magnitude:" );

    m_SET_RefFinal->Fill((*met_RefFinal)["Final"]->sumet()/1000., 1.);
    m_SET_RefEle->Fill((*met_RefFinal)["RefEle"]->sumet()/1000., 1.);
    m_SET_RefGamma->Fill((*met_RefFinal)["RefGamma"]->sumet()/1000., 1.);
    m_SET_RefTau->Fill((*met_RefFinal)["RefTau"]->sumet()/1000., 1.);
    m_SET_RefJet->Fill((*met_RefFinal)["RefJet"]->sumet()/1000., 1.);
    m_SET_Muons->Fill((*met_RefFinal)["Muons"]->sumet()/1000., 1.);
    m_SET_SoftClus->Fill((*met_RefFinal)["SoftClus"]->sumet()/1000., 1.);
    m_SET_PVSoftTrk->Fill((*met_RefFinal)["PVSoftTrk"]->sumet()/1000., 1.);
    

    ATH_MSG_INFO( "  MET_RefFinal_TST :" );

    double MET_TST_x, MET_TST_y;
    MET_TST_x = (*met_RefFinal)["RefEle"]->mpx() + (*met_RefFinal)["RefGamma"]->mpx()
               + (*met_RefFinal)["RefTau"]->mpx() + (*met_RefFinal)["RefJet"]->mpx()
	       + (*met_RefFinal)["Muons"]->mpx() + (*met_RefFinal)["PVSoftTrk"]->mpx();
    MET_TST_y = (*met_RefFinal)["RefEle"]->mpy() + (*met_RefFinal)["RefGamma"]->mpy()
               + (*met_RefFinal)["RefTau"]->mpy() + (*met_RefFinal)["RefJet"]->mpy()
               + (*met_RefFinal)["Muons"]->mpy() + (*met_RefFinal)["PVSoftTrk"]->mpy();

    m_MET_RefFinal_TST_x->Fill(MET_TST_x/1000.);
    m_MET_RefFinal_TST_y->Fill(MET_TST_y/1000.);
    m_MET_RefFinal_TST->Fill(std::sqrt(MET_TST_x*MET_TST_x + MET_TST_y*MET_TST_y)/1000.);

    
    ATH_MSG_INFO( "  Resolution:" );

    m_Resolution_TruthNonInt_RefFinal_METx->Fill(((*met_RefFinal)["Final"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    m_Resolution_TruthNonInt_RefFinal_METy->Fill(((*met_RefFinal)["Final"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);


    ATH_MSG_INFO( "  MET magnitude:" );

    m_MET_Track->Fill((*met_Track)["Track"]->met()/1000., 1.);
    m_MET_Track_x->Fill((*met_Track)["Track"]->mpx()/1000., 1.);
    m_MET_Track_y->Fill((*met_Track)["Track"]->mpy()/1000., 1.);
    m_MET_Track_phi->Fill((*met_Track)["Track"]->phi(), 1.);
    m_SET_Track->Fill((*met_Track)["Track"]->sumet()/1000., 1.);
    
    //m_MET_TrackPV->Fill((*met_Track)["TrackPV"]->met()/1000., 1.);
    //m_MET_TrackPV_x->Fill((*met_Track)["TrackPV"]->mpx()/1000., 1.);
    //m_MET_TrackPV_y->Fill((*met_Track)["TrackPV"]->mpy()/1000., 1.);
    //m_MET_TrackPV_phi->Fill((*met_Track)["TrackPV"]->phi(), 1.);
    //m_SET_TrackPV->Fill((*met_Track)["TrackPV"]->sumet()/1000., 1.);
    
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
