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
#include "xAODMissingET/versions/MissingETBase.h" 
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
    m_MET_RefFinal(0), m_MET_RefEle(0), m_MET_RefGamma(0), m_MET_RefTau(0), m_MET_RefJet(0), m_MET_Muons(0), m_MET_SoftClus(0), m_MET_PVSoftTrk(0),
    m_MET_RefFinal_x(0), m_MET_RefEle_x(0), m_MET_RefGamma_x(0), m_MET_RefTau_x(0), m_MET_RefJet_x(0), m_MET_Muons_x(0), m_MET_SoftClus_x(0), m_MET_PVSoftTrk_x(0),
    m_MET_RefFinal_y(0), m_MET_RefEle_y(0), m_MET_RefGamma_y(0), m_MET_RefTau_y(0), m_MET_RefJet_y(0), m_MET_Muons_y(0), m_MET_SoftClus_y(0), m_MET_PVSoftTrk_y(0),
    m_MET_RefFinal_phi(0), m_MET_RefEle_phi(0), m_MET_RefGamma_phi(0), m_MET_RefTau_phi(0), m_MET_RefJet_phi(0), m_MET_Muons_phi(0), m_MET_SoftClus_phi(0), m_MET_PVSoftTrk_phi(0),
    m_SET_RefFinal(0), m_SET_RefEle(0), m_SET_RefGamma(0), m_SET_RefTau(0), m_SET_RefJet(0), m_SET_Muons(0), m_SET_SoftClus(0), m_SET_PVSoftTrk(0),
    m_Resolution_TruthNonInt_RefFinal_METx(0), m_Resolution_TruthNonInt_RefFinal_METy(0),
    m_MET_Track(0), m_MET_TrackPV(0), m_MET_Track_x(0), m_MET_TrackPV_x(0), m_MET_Track_y(0), m_MET_TrackPV_y(0), m_MET_Track_phi(0), m_MET_TrackPV_phi(0), m_SET_Track(0), m_SET_TrackPV(0),
    m_MET_RefFinal_TST(0), m_MET_RefFinal_TST_x(0), m_MET_RefFinal_TST_y(0)
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

	std::string name_met = "MET_RefFinal"; 
	std::string dir_met("MET/" + name_met + "/");
	ATH_CHECK(regHist(m_MET_RefFinal = new TH1D("MET_RefFinal", (name_met + " MET_RefFinal; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_x = new TH1D("MET_RefFinal_x", (name_met + " MET_RefFinal_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_y = new TH1D("MET_RefFinal_y", (name_met + " MET_RefFinal_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefFinal_phi = new TH1D("MET_RefFinal_phi", (name_met + " MET_RefFinal_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefFinal = new TH1D("SET_RefFinal", (name_met + " SET_RefFinal; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	name_met = "MET_RefEle"; 
	dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_RefEle = new TH1D("MET_RefEle", (name_met + " MET_RefEle; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_x = new TH1D("MET_RefEle_x", (name_met + " MET_RefEle_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_y = new TH1D("MET_RefEle_y", (name_met + " MET_RefEle_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefEle_phi = new TH1D("MET_RefEle_phi", (name_met + " MET_RefEle_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefEle = new TH1D("SET_RefEle", (name_met + " SET_RefEle; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	name_met = "MET_RefGamma";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_RefGamma = new TH1D("MET_RefGamma", (name_met + " MET_RefGamma; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_x = new TH1D("MET_RefGamma_x", (name_met + " MET_RefGamma_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_y = new TH1D("MET_RefGamma_y", (name_met + " MET_RefGamma_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefGamma_phi = new TH1D("MET_RefGamma_phi", (name_met + " MET_RefGamma_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefGamma = new TH1D("SET_RefGamma", (name_met + " SET_RefGamma; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	name_met = "MET_RefTau";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_RefTau = new TH1D("MET_RefTau", (name_met + " MET_RefTau; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_x = new TH1D("MET_RefTau_x", (name_met + " MET_RefTau_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_y = new TH1D("MET_RefTau_y", (name_met + " MET_RefTau_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefTau_phi = new TH1D("MET_RefTau_phi", (name_met + " MET_RefTau_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefTau = new TH1D("SET_RefTau", (name_met + " SET_RefTau; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	name_met = "MET_RefJet";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_RefJet = new TH1D("MET_RefJet", (name_met + " MET_RefJet; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_x = new TH1D("MET_RefJet_x", (name_met + " MET_RefJet_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_y = new TH1D("MET_RefJet_y", (name_met + " MET_RefJet_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_RefJet_phi = new TH1D("MET_RefJet_phi", (name_met + " MET_RefJet_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_RefJet = new TH1D("SET_RefJet", (name_met + " SET_RefJet; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	name_met = "MET_Muons";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_Muons = new TH1D("MET_Muons", (name_met + " MET_Muons; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0, m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_x = new TH1D("MET_Muons_x", (name_met + " MET_Muons_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_y = new TH1D("MET_Muons_y", (name_met + " MET_Muons_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Muons_phi = new TH1D("MET_Muons_phi", (name_met + " MET_Muons_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_Muons = new TH1D("SET_Muons", (name_met + " SET_Muons; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/10.), dir_met, all));

	name_met = "MET_SoftClus";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_SoftClus = new TH1D("MET_SoftClus", (name_met + " MET_SoftClus; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_x = new TH1D("MET_SoftClus_x", (name_met + " MET_SoftClus_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_y = new TH1D("MET_SoftClus_y", (name_met + " MET_SoftClus_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_SoftClus_phi = new TH1D("MET_SoftClus_phi", (name_met + " MET_SoftClus_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_SoftClus = new TH1D("SET_SoftClus", (name_met + " SET_SoftClus; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/2.), dir_met, all));

	name_met = "MET_PVSoftTrk";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_PVSoftTrk = new TH1D("MET_PVSoftTrk", (name_met + " MET_PVSoftTrk; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi/2.), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_x = new TH1D("MET_PVSoftTrk_x", (name_met + " MET_PVSoftTrk_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_y = new TH1D("MET_PVSoftTrk_y", (name_met + " MET_PVSoftTrk_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_PVSoftTrk_phi = new TH1D("MET_PVSoftTrk_phi", (name_met + " MET_PVSoftTrk_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_PVSoftTrk = new TH1D("SET_PVSoftTrk", (name_met + " SET_PVSoftTrk; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET/2.), dir_met, all));

	name_met = "MET_RefFinal_TST";
	dir_met = "MET/" + name_met + "/";
        ATH_CHECK(regHist(m_MET_RefFinal_TST = new TH1D("MET_RefFinal_TST", (name_met + " MET_RefFinal_TST; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
        ATH_CHECK(regHist(m_MET_RefFinal_TST_x = new TH1D("MET_RefFinal_TST_x", (name_met + " MET_RefFinal_TST_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_RefFinal_TST_y = new TH1D("MET_RefFinal_TST_y", (name_met + " MET_RefFinal_TST_y; E_{Ty}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        //ATH_CHECK(regHist(m_MET_RefFinal_TST_phi = new TH1D("MET_RefFinal_TST_phi", (name_met + " MET_RefFinal_TST_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
        //ATH_CHECK(regHist(m_SET_RefFinal_TST = new TH1D("SET_RefFinal_TST", (name_met + " SET_RefFinal_TST; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	name_met = "MET_Resolution";
	dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METx = new TH1D("m_Resolution_TruthNonInt_RefFinal_METx", (name_met + " Resolution_TruthNonInt_RefFinal_METx; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_Resolution_TruthNonInt_RefFinal_METy = new TH1D("m_Resolution_TruthNonInt_RefFinal_METy", (name_met + " Resolution_TruthNonInt_RefFinal_METy; Resolution; Entries").c_str(), 1.5*m_nbinpxy, -1.5*m_suptmixy, 1.5*m_suptmixy), dir_met, all));

	name_met = "MET_Track";
        dir_met = "MET/" + name_met + "/";
	ATH_CHECK(regHist(m_MET_Track = new TH1D("MET_Track", (name_met + " MET_Track; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_x = new TH1D("MET_Track_x", (name_met + " MET_Track_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_y = new TH1D("MET_Track_y", (name_met + " MET_Track_y; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
	ATH_CHECK(regHist(m_MET_Track_phi = new TH1D("MET_Track_phi", (name_met + " MET_Track_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
	ATH_CHECK(regHist(m_SET_Track = new TH1D("SET_Track", (name_met + " SET_Track; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

	name_met = "MET_TrackPV";
        dir_met = "MET/" + name_met + "/";
        ATH_CHECK(regHist(m_MET_TrackPV = new TH1D("MET_TrackPV", (name_met + " MET_TrackPV; E_{T}^{miss}; Entries").c_str(), m_nbinp, 0., m_suptmi), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_x = new TH1D("MET_TrackPV_x", (name_met + " MET_TrackPV_x; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_y = new TH1D("MET_TrackPV_y", (name_met + " MET_TrackPV_y; E_{Tx}^{miss}; Entries").c_str(), m_nbinpxy, -m_suptmixy, m_suptmixy), dir_met, all));
        ATH_CHECK(regHist(m_MET_TrackPV_phi = new TH1D("MET_TrackPV_phi", (name_met + " MET_TrackPV_phi; E_{Tphi}^{miss}; Entries").c_str(), m_nbinphi,-m_binphi,m_binphi), dir_met, all));
        ATH_CHECK(regHist(m_SET_TrackPV = new TH1D("SET_TrackPV", (name_met + " SET_TrackPV; E_{T}^{sum}; Entries").c_str(), m_nbinE, m_lowET, m_suET), dir_met, all));

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
    //ATH_CHECK( evtStore()->retrieve(met_RefFinal, "MET_RefFinal") );
 
   if (!met_RefFinal) {
      ATH_MSG_ERROR ("Couldn't retrieve MET Final");
      return StatusCode::FAILURE;                   
    } 

    const xAOD::MissingETContainer* met_Truth = 0;
    ATH_CHECK( evtStore()->retrieve(met_Truth,"MET_Truth") );

    const xAOD::MissingETContainer* met_Track = 0;
    ATH_CHECK( evtStore()->retrieve(met_Track,"MET_Track") );
       

        
    ATH_MSG_INFO( "  MET magnitude:" );


    //m_MET_RefFinal_phi->Fill((*met_RefFinal)["Final"]->phi(), 1.);  Types::bitmask_t total

    //m_MET_RefFinal->Fill((*met_RefFinal)[MissingETBase::Source::total()]->met()/1000., 1.);
    m_MET_RefFinal->Fill((*met_RefFinal)["FinalClus"]->met()/1000., 1.);
    m_MET_RefEle->Fill((*met_RefFinal)["RefEle"]->met()/1000., 1.);
    m_MET_RefGamma->Fill((*met_RefFinal)["RefGamma"]->met()/1000., 1.);
    m_MET_RefTau->Fill((*met_RefFinal)["RefTau"]->met()/1000., 1.);
    m_MET_RefJet->Fill((*met_RefFinal)["RefJet"]->met()/1000., 1.);
    m_MET_Muons->Fill((*met_RefFinal)["Muons"]->met()/1000., 1.);
    m_MET_SoftClus->Fill((*met_RefFinal)["SoftClus"]->met()/1000., 1.);
    m_MET_PVSoftTrk->Fill((*met_RefFinal)["PVSoftTrk"]->met()/1000., 1.);
    

    ATH_MSG_INFO( "  MET x-component:" );

    m_MET_RefFinal_x->Fill((*met_RefFinal)["FinalClus"]->mpx()/1000., 1.);
    m_MET_RefEle_x->Fill((*met_RefFinal)["RefEle"]->mpx()/1000., 1.);
    m_MET_RefGamma_x->Fill((*met_RefFinal)["RefGamma"]->mpx()/1000., 1.);
    m_MET_RefTau_x->Fill((*met_RefFinal)["RefTau"]->mpx()/1000., 1.);
    m_MET_RefJet_x->Fill((*met_RefFinal)["RefJet"]->mpx()/1000., 1.);
    m_MET_Muons_x->Fill((*met_RefFinal)["Muons"]->mpx()/1000., 1.);
    m_MET_SoftClus_x->Fill((*met_RefFinal)["SoftClus"]->mpx()/1000., 1.);
    m_MET_PVSoftTrk_x->Fill((*met_RefFinal)["PVSoftTrk"]->mpx()/1000., 1.);
    

    ATH_MSG_INFO( "  MET y-component:" );

    m_MET_RefFinal_y->Fill((*met_RefFinal)["FinalClus"]->mpy()/1000., 1.);
    m_MET_RefEle_y->Fill((*met_RefFinal)["RefEle"]->mpy()/1000., 1.);
    m_MET_RefGamma_y->Fill((*met_RefFinal)["RefGamma"]->mpy()/1000., 1.);
    m_MET_RefTau_y->Fill((*met_RefFinal)["RefTau"]->mpy()/1000., 1.);
    m_MET_RefJet_y->Fill((*met_RefFinal)["RefJet"]->mpy()/1000., 1.);
    m_MET_Muons_y->Fill((*met_RefFinal)["Muons"]->mpy()/1000., 1.);
    m_MET_SoftClus_y->Fill((*met_RefFinal)["SoftClus"]->mpy()/1000., 1.);
    m_MET_PVSoftTrk_y->Fill((*met_RefFinal)["PVSoftTrk"]->mpy()/1000., 1.);


    ATH_MSG_INFO( "  MET phi-component:" );

    m_MET_RefFinal_phi->Fill((*met_RefFinal)["FinalClus"]->phi(), 1.);
    m_MET_RefEle_phi->Fill((*met_RefFinal)["RefEle"]->phi(), 1.);
    m_MET_RefGamma_phi->Fill((*met_RefFinal)["RefGamma"]->phi(), 1.);
    m_MET_RefTau_phi->Fill((*met_RefFinal)["RefTau"]->phi(), 1.);
    m_MET_RefJet_phi->Fill((*met_RefFinal)["RefJet"]->phi(), 1.);
    m_MET_Muons_phi->Fill((*met_RefFinal)["Muons"]->phi(), 1.);
    m_MET_SoftClus_phi->Fill((*met_RefFinal)["SoftClus"]->phi(), 1.);
    m_MET_PVSoftTrk_phi->Fill((*met_RefFinal)["PVSoftTrk"]->phi(), 1.);


    ATH_MSG_INFO( "  SumET magnitude:" );

    m_SET_RefFinal->Fill((*met_RefFinal)["FinalClus"]->sumet()/1000., 1.);
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

    m_Resolution_TruthNonInt_RefFinal_METx->Fill(((*met_RefFinal)["FinalClus"]->mpx()-(*met_Truth)["NonInt"]->mpx())/1000., 1.);
    m_Resolution_TruthNonInt_RefFinal_METy->Fill(((*met_RefFinal)["FinalClus"]->mpy()-(*met_Truth)["NonInt"]->mpy())/1000., 1.);


    ATH_MSG_INFO( "  MET_Track:" );

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
