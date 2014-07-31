/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     egammaMonTool.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaPerformance

AUTHORS: Kamal Benslama University of Regina
CREATED: 02 June 2007

PURPOSE: egamma offline monitoring tool

UPDATED:
        Jan. 14, 2008 Eduardo J. Ortega 
                      This tool monitors electron and photon characteristic
                      variables (energy, spatial end etc.)  
        Nov. 10, 2008 A.Kalinowski 
        Feb. 06, 2009 (FD) better calculation of E/p
********************************************************************/

//#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "egammaPerformance/egammaMonTool.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"

#include "CaloEvent/CaloClusterContainer.h"
//-REMI #include "CaloEvent/CaloSampling.h"
//+REMI
#include "CaloGeoHelpers/CaloSampling.h"


#include <sstream>


using CLHEP::GeV;
using CLHEP::HepLorentzVector;


//constructor
egammaMonTool::
egammaMonTool( const std::string & type, const std::string & name,
	       const IInterface* parent )
  : IEgammaMonTool( type, name, parent )
  , m_nOfCaloSamplings(8)
  , m_h_index(0)
  , m_el_clus_meanEne(0)
  , m_el_clus_meanEta(0)
  , m_el_clus_meanPhi(0)
  , m_ph_clus_meanEne(0)
  , m_ph_clus_meanEta(0)
  , m_ph_clus_meanPhi(0)
  , m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool") 
  , m_storeGate(0)
  , m_IsEMHisto(0)
  , m_el_F2Histo(0)
  , m_ph_F2Histo(0)
  , m_el_HighEtRangesHisto(0)
  , m_el_ClusterPhiHisto(0)
  , m_ph_ClusterPhiHisto(0)
  , m_el_IsEMMedium_ClusterEtHisto(0)
  , m_el_IsEMMedium_ClusterEtaHisto(0)
  , m_el_IsEMMedium_ClusterPhiHisto(0)
  , m_el_IsEMMedium_EtvsEtaHisto(0)
  , m_el_IsEMMedium_EtvsPhiHisto(0)
  , m_el_IsEMMedium_EoverPHisto(0)
  , m_el_IsEMMedium_DeltaEta1Histo(0)
  , m_el_IsEMMedium_DeltaPhi2Histo(0)
  , m_el_IsEMMedium_EthadHisto(0)
  , m_el_IsEMMedium_F1Histo(0)
  , m_el_IsEMMedium_F2Histo(0)
  , m_el_IsEMMedium_F3Histo(0)
  , m_el_IsEMMedium_rE233E237Histo(0)
  , m_el_IsEMMedium_rE237E277Histo(0)
  , m_el_IsEMMedium_HighEtRangesHisto(0)
  , m_ph_IsEMTight_ClusterEtHisto(0)
  , m_ph_IsEMTight_ClusterEtaHisto(0)
  , m_ph_IsEMTight_ClusterPhiHisto(0)
  , m_ph_IsEMTight_F1Histo(0)
  , m_ph_IsEMTight_F2Histo(0)
  , m_ph_IsEMTight_F3Histo(0)
  , m_ph_IsEMTight_EtHadHisto(0)
  , m_ph_IsEMTight_rE233E237Histo(0)
  , m_ph_IsEMTight_rE237E277Histo(0)
  , m_ph_IsEMTight_ECoreHisto(0)
  , m_ph_IsEMTight_convTrkMatchHisto(0)
  , m_ph_IsEMTight_convAngleMatchHisto(0)
  , m_ph_IsEMTight_EtvsEtaHisto(0)
  , m_ph_IsEMTight_EtvsPhiHisto(0)
  , m_el_Electron_EoverPvsEta(0)
  , m_el_Positron_EoverPvsEta(0)
  , m_el_Electron_EoverPvsPhi(0)
  , m_el_Positron_EoverPvsPhi(0)
  , m_el_TightElectron_EoverPvsEta(0)
  , m_el_TightPositron_EoverPvsEta(0)
  , m_el_TightElectron_EoverPvsPhi(0)
  , m_el_TightPositron_EoverPvsPhi(0)
  , m_el_Electron_EoverP(0)
  , m_el_Positron_EoverP(0)
  , m_z_ClusterEt(0)
  , m_z_ClusterEta(0)
  , m_z_ClusterPhi(0)
  , m_z_ClusterEtvsEta(0)
  , m_z_ClusterEtvsPhi(0)
  , m_z_TrackPt(0)
  , m_z_TrackEta(0)
  , m_z_TrackPhi(0)
  , m_z_TrackPtvsEta(0)
  , m_z_TrackPtvsPhi(0)
  , m_z_ClusterWithTrackEt(0)
  , m_z_ClusterWithTrackEta(0)
  , m_z_ClusterWithTrackPhi(0)
  , m_z_ClusterWithTrackEtvsEta(0)
  , m_z_ClusterWithTrackEtvsPhi(0)
  , m_z_CutClusterEt(0)
  , m_z_CutClusterEta(0)
  , m_z_CutClusterPhi(0)
  , m_z_mass(0)
  , m_z_mass_bb(0)
  , m_z_mass_be(0)
  , m_z_mass_bc(0)	
  , m_z_mass_ec(0)	
  , m_z_mass_ee(0)	
  , m_z_ClusterEtEff(0)
  , m_z_ClusterEtaEff(0)
  , m_z_ClusterPhiEff(0)
  , m_z_CutClusterEtEff(0)
  , m_z_CutClusterEtaEff(0)
  , m_z_CutClusterPhiEff(0)
{
  // Name of the electron collection
  declareProperty("ElectronContainer",		
		  m_ElectronCnt = 	"ElectronCollection",
		  "Name of the electron collection");
  // Name of the photon collection
  declareProperty("PhotonContainer",              
		  m_PhotonCnt = 		"PhotonCollection",
		  "Name of the photon collection");
  // Name of the egDetail for cluster-track matching
  declareProperty("EMTrackMatchContainer",        
		  m_EMTrackMatchCnt = 	"egDetailContainer",
		  "Name of the egDetail for cluster-track matching");
  // Name of the egDetail for shower
  declareProperty("EMShowerContainer",            
		  m_EMShowerCnt = 	"egDetailContainer",
		  "Name of the egDetail for shower");
  // Name of the egDetail for conversion
  declareProperty("EMConvertContainer",		
		  m_EMConvertCnt = 	"egDetailContainer",
		  "Name of the egDetail for conversion");
  // Name of the tool for the trigger decision  
  declareProperty("TriggerDecisionTool", 
		  m_trigDec,
		  "Name of the tool for the trigger decision");
  // Items for the electron trigger selection
  declareProperty("Electron_Trigger_Items", 
		  m_el_triggers,
		  "Items for the trigger selection");
  // Items for the photon trigger selection
  declareProperty("Photon_Trigger_Items", 
		  m_ph_triggers,
		  "Items for the photon trigger selection");
  // Cuts on ET
  declareProperty("EtCut",
		  m_EtCut,
		  "Cuts on ET");
  // Cuts on eta
  declareProperty("EtaCuts",
		  m_eta_cuts,
		  "Cuts on eta");
  // Cuts on phi
  declareProperty("PhiCuts",
		  m_phi_cuts,
		  "Cuts on phi");
  // Boolean to use trigger information
  declareProperty("UseTrigger",
		  m_useTrigger = false,
		  "Boolean to use trigger information");
  // Cut on Electron cluster ET for Zmass
  declareProperty("ZmassElectronClusterEtCut",
		  m_ElectronClusterEtCut = 15*GeV,
		  "Cut on Electron cluster ET for Zmass");
  // Lower cut on Z mass
  declareProperty("ZmassLowerCut",
		  m_ZmassLowerCut = 70*GeV,
		  "Lower cut on Z mass");
  // Upper cut on Z mass
  declareProperty("ZmassUpperCut",
		  m_ZmassUpperCut = 110*GeV,
		  "Upper cut on Z mass");
  //Boolean to switch on EventInfo::LAr checking
//  declareProperty("CheckLAr",
//                  m_checkLar,
//                  "Decision to check Lar EventInfo or not");

}

// ===========================================================
//destructor
egammaMonTool::~egammaMonTool()
{
}

// ===========================================================
StatusCode egammaMonTool::
bookPhotonHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/,
		  MonGroup egamma_photon, MonGroup egamma_photon_ene, MonGroup egamma_photon_eta,
		  MonGroup egamma_photon_phi, MonGroup egamma_photon_tight)
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookPhotonHistos()"<<endreq;
  StatusCode sc;

  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {
    
    //////////////////////////////////////
    //                                  //
    //      Photon Histograms           //  
    //                                  // 
    //////////////////////////////////////  
    m_ph_histos.clear();
    m_ph_ene_histos.clear();
    m_ph_eta_histos.clear();
    m_ph_phi_histos.clear();
    
    log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tCreating basic photon histos"<<endreq;
    //egamma variables / shift
    std::ostringstream ph_hName0 ;
    std::ostringstream ph_hTitle0;
    ph_hName0  << "ph_ClusterEt" ;
    ph_hTitle0 << "Photon Cluster Et" ;
    TH1* ph_h0 = new TH1F( ph_hName0.str().c_str(), ph_hTitle0.str().c_str(), 100, -1000, 250000 );
    sc = egamma_photon.regHist( ph_h0 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<ph_hName0.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }
    m_ph_histos.push_back( ph_h0 );
    
    std::ostringstream ph_hName1  ;
    std::ostringstream ph_hTitle1 ;
    ph_hName1  << "ph_ClusterEta" ;
    ph_hTitle1 << "Photon Cluster eta" ;
    TH1* ph_h1 = new TH1F( ph_hName1.str().c_str(), ph_hTitle1.str().c_str(), 50, -3.2, 3.2 );
    sc = egamma_photon.regHist( ph_h1 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<ph_hName1.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }
    m_ph_histos.push_back( ph_h1 );
    
    //EMShower variables / shift
    std::ostringstream ph_hName6  ;
    std::ostringstream ph_hTitle6 ;
    ph_hName6  << "ph_f1" ;
    ph_hTitle6 << "Photon fractional energy in 1st sampling" ;
    TH1* ph_h6 = new TH1F( ph_hName6.str().c_str(), ph_hTitle6.str().c_str(), 50, -1., 1.);
    sc = egamma_photon.regHist( ph_h6 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<ph_hName6.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }
    m_ph_histos.push_back( ph_h6 );
    
    std::ostringstream ph_F2HistoName  ;
    std::ostringstream ph_F2HistoTitle;
    ph_F2HistoName << "ph_f2" ;
    ph_F2HistoTitle << "Photon fractional energy in 2nd sampling" ;
    m_ph_F2Histo = new TH1F( ph_F2HistoName.str().c_str(), ph_F2HistoTitle.str().c_str(), 50, -1., 1);
    sc = egamma_photon.regHist( m_ph_F2Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<ph_F2HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }
    
    std::ostringstream ph_hName7  ;
      std::ostringstream ph_hTitle7 ;
      ph_hName7  << "ph_f3" ;
      ph_hTitle7 << "Photon fractional energy in 3rd sampling" ;
      TH1* ph_h7 = new TH1F( ph_hName7.str().c_str(), ph_hTitle7.str().c_str(), 50, -1., 1);
      sc = egamma_photon.regHist( ph_h7 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName7.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h7 );

      std::ostringstream ph_hName8  ;
      std::ostringstream ph_hTitle8 ;
      ph_hName8  << "ph_ehad1" ;
      ph_hTitle8 << "Photon energy leakage in 1st sampling of hadcal" ;
      TH1* ph_h8 = new TH1F( ph_hName8.str().c_str(), ph_hTitle8.str().c_str(), 50, -1000., 10000.);
      sc = egamma_photon.regHist( ph_h8 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName8.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h8 );

      std::ostringstream ph_hName9  ;
      std::ostringstream ph_hTitle9 ;
      ph_hName9  << "ph_Re233e237" ;
      ph_hTitle9 << "Photon uncor. energy fraction in 3x3/3x7 cells in em sampling 2" ;
      TH1* ph_h9 = new TH1F( ph_hName9.str().c_str(), ph_hTitle9.str().c_str(), 50, -4., 4.);
      sc = egamma_photon.regHist( ph_h9 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName9.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h9 );

      std::ostringstream ph_hName10  ;
      std::ostringstream ph_hTitle10 ;
      ph_hName10  << "ph_Re237e277" ;
      ph_hTitle10 << "Photon uncor. energy fraction in 3x7/7x7 cells in em sampling 2" ;
      TH1* ph_h10 = new TH1F( ph_hName10.str().c_str(), ph_hTitle10.str().c_str(), 50, -4., 4. );
      sc = egamma_photon.regHist( ph_h10 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName10.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h10 );

      std::ostringstream ph_hName12  ;
      std::ostringstream ph_hTitle12 ;
      ph_hName12  << "ph_core" ;
      ph_hTitle12 << "Photon core energy in em" ;
      TH1* ph_h12 = new TH1F( ph_hName12.str().c_str(), ph_hTitle12.str().c_str(), 50, -5000., 250000.);
      sc = egamma_photon.regHist( ph_h12 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName12.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h12 );


      std::ostringstream ph_hName19  ;
      std::ostringstream ph_hTitle19 ;
      ph_hName19  << "ph_EtVsEta" ;
      ph_hTitle19 << "Photon Et vs. Eta" ; 
      TH2* ph_h19 = new TH2F( ph_hName19.str().c_str(), ph_hTitle19.str().c_str(), 
			      100, -2.5, 2.5,
			      100, 0, 150000
			      );
      sc = egamma_photon.regHist( ph_h19 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName19.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h19 );

      std::ostringstream ph_hName20  ;
      std::ostringstream ph_hTitle20 ;
      ph_hName20  << "ph_EtVsPhi" ;
      ph_hTitle20 << "Photon Et vs. Phi" ; 
      TH2* ph_h20 = new TH2F( ph_hName20.str().c_str(), ph_hTitle20.str().c_str(), 
			      100, -4., 4.,
			      100, 0, 150000
			      );
      sc = egamma_photon.regHist( ph_h20 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName20.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_histos.push_back( ph_h20 );

      std::ostringstream ph_ClusterPhiName ;
      std::ostringstream ph_ClusterPhiTitle ;
      ph_ClusterPhiName  << "ph_ClusterPhi" ;
      ph_ClusterPhiTitle << "Photon Cluster phi" ;
      m_ph_ClusterPhiHisto = new TH1F( ph_ClusterPhiName.str().c_str(), ph_ClusterPhiTitle.str().c_str(), 50, -4., 4. );
      sc = egamma_photon.regHist( m_ph_ClusterPhiHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_ClusterPhiName.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }


      // Photon passing Tight cut variables
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tCreating Tight selection photon histos"<<endreq;
      //Cluster Et
      std::string ph_Tight_HistoName("ph_ClusterEt");
      std::string ph_Tight_HistoTitle("Photon Cluster Et");
      m_ph_IsEMTight_ClusterEtHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 100, -1000, 250000);
      sc = egamma_photon_tight.regHist( m_ph_IsEMTight_ClusterEtHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      // Cluster Eta
      ph_Tight_HistoName = "ph_ClusterEta";
      ph_Tight_HistoTitle = "Photon Cluster Eta";
      m_ph_IsEMTight_ClusterEtaHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -3.2, 3.2);
      sc = egamma_photon_tight.regHist( m_ph_IsEMTight_ClusterEtaHisto);
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //Cluster Phi
      ph_Tight_HistoName = "ph_Cluster_Phi";
      ph_Tight_HistoTitle = "Photon Cluster Phi";
      m_ph_IsEMTight_ClusterPhiHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -4., 4.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_ClusterPhiHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //F1
      ph_Tight_HistoName = "ph_F1";
      ph_Tight_HistoTitle = "Photon Fractional energy in 1st sampling";
      m_ph_IsEMTight_F1Histo = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -1.,1.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_F1Histo );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //F2
      ph_Tight_HistoName = "ph_F2";
      ph_Tight_HistoTitle = "Photon Fractional energy in 2nd sampling";
      m_ph_IsEMTight_F2Histo = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -1., 1.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_F2Histo );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //F3
      ph_Tight_HistoName = "ph_F3";
      ph_Tight_HistoTitle = "Photon Fractional energy in 3rd sampling";
      m_ph_IsEMTight_F3Histo = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -1., 1.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_F3Histo );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //EtHad
      ph_Tight_HistoName = "ph_EtHad";
      ph_Tight_HistoTitle = "Photon energy leakage in the first sampling of the hadronic calorimeter";
      m_ph_IsEMTight_EtHadHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -1000., 10000.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_EtHadHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //rE233E237
      ph_Tight_HistoName = "ph_rE233E237";
      ph_Tight_HistoTitle = "Photon  uncor. energy fraction in 3x3/3x7 cells in em sampling 2";
      m_ph_IsEMTight_rE233E237Histo = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -4., 4.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_rE233E237Histo );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //rE237E277
      ph_Tight_HistoName = "ph_rE237E277";
      ph_Tight_HistoTitle = "Photon uncor. energy fraction in 3x7/7x7 cells in em sampling 2";
      m_ph_IsEMTight_rE237E277Histo = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -4., 4.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_rE237E277Histo );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //Core Energy
      ph_Tight_HistoName = "ph_ECore";
      ph_Tight_HistoTitle = "Photon core energy in em";
      m_ph_IsEMTight_ECoreHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50, -5000, 250000);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_ECoreHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //convTrkMatch
      ph_Tight_HistoName = "ph_convTrkMatch";
      ph_Tight_HistoTitle = "Photon convTrackMatch flag";
      m_ph_IsEMTight_convTrkMatchHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 6, -3., 3.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_convTrkMatchHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //convAngleMatch
      ph_Tight_HistoName = "ph_convAngleMatch";
      ph_Tight_HistoTitle = "Photon convAnglMatch flag";
      m_ph_IsEMTight_convAngleMatchHisto = new TH1F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 6, -3., 3.);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_convAngleMatchHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //Cluster Et vs Eta
      ph_Tight_HistoName = "ph_EtvsEta";
      ph_Tight_HistoTitle = "Photon Cluster Et vs Cluster Eta";
      m_ph_IsEMTight_EtvsEtaHisto = new TH2F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(),
					     50,-2.5,2.5,
					     100, 0, 150000
					     );
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_EtvsEtaHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      //Cluster Et vs Phi
      ph_Tight_HistoName = "ph_EtvsPhi";
      ph_Tight_HistoTitle = "Photon Cluster Et vs Phi";
      m_ph_IsEMTight_EtvsPhiHisto = new TH2F(ph_Tight_HistoName.c_str(), ph_Tight_HistoTitle.c_str(), 50,-4.,4., 100, 0, 150000);
      sc = egamma_photon_tight.regHist(m_ph_IsEMTight_EtvsPhiHisto );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_Tight_HistoName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      //CaloCluster variables / expert
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tCreating photont CaloCluster histos"<<endreq;
      //cluster energy in different samples
      std::ostringstream ph_hName_ene0  ;
      std::ostringstream ph_hTitle_ene0 ;
      ph_hName_ene0  << "ph_ene_PreSamplerB" ;
      ph_hTitle_ene0 << "Photon cluster energy in CaloSampling::PreSamplerB sampling" ;
      TH1* ph_h_ene0 = new TH1F( ph_hName_ene0.str().c_str(), ph_hTitle_ene0.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene0 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene0.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_ene_histos.push_back( ph_h_ene0 );

      std::ostringstream ph_hName_ene1  ;
      std::ostringstream ph_hTitle_ene1 ;
      ph_hName_ene1  << "ph_ene_EMB1" ;
      ph_hTitle_ene1 << "Photon cluster energy in CaloSampling::EMB1 sampling" ;
      TH1* ph_h_ene1 = new TH1F( ph_hName_ene1.str().c_str(), ph_hTitle_ene1.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene1 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene1.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene1 );

      std::ostringstream ph_hName_ene2  ;
      std::ostringstream ph_hTitle_ene2 ;
      ph_hName_ene2  << "ph_ene_EMB2" ;
      ph_hTitle_ene2 << "Photon cluster energy in CaloSampling::EMB2 sampling" ;
      TH1* ph_h_ene2 = new TH1F( ph_hName_ene2.str().c_str(), ph_hTitle_ene2.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene2 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene2.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene2 );

      std::ostringstream ph_hName_ene3  ;
      std::ostringstream ph_hTitle_ene3 ;
      ph_hName_ene3  << "ph_ene_EMB3" ;
      ph_hTitle_ene3 << "Photon cluster energy in CaloSampling::EMB3 sampling" ;
      TH1* ph_h_ene3 = new TH1F( ph_hName_ene3.str().c_str(), ph_hTitle_ene3.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene3 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene3.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene3 );

      std::ostringstream ph_hName_ene4  ;
      std::ostringstream ph_hTitle_ene4 ;
      ph_hName_ene4  << "ph_ene_PreSamplerE" ;
      ph_hTitle_ene4 << "Photon cluster energy in CaloSampling::PreSamplerE sampling" ;
      TH1* ph_h_ene4 = new TH1F( ph_hName_ene4.str().c_str(), ph_hTitle_ene4.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene4 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene4.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene4 );

      std::ostringstream ph_hName_ene5  ;
      std::ostringstream ph_hTitle_ene5 ;
      ph_hName_ene5  << "ph_ene_EME1" ;
      ph_hTitle_ene5 << "Photon cluster energy in CaloSampling::EME1 sampling" ;
      TH1* ph_h_ene5 = new TH1F( ph_hName_ene5.str().c_str(), ph_hTitle_ene5.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene5 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene5.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene5 );

      std::ostringstream ph_hName_ene6  ;
      std::ostringstream ph_hTitle_ene6 ;
      ph_hName_ene6  << "ph_ene_EME2" ;
      ph_hTitle_ene6 << "Photon cluster energy in CaloSampling::EME2 sampling" ;
      TH1* ph_h_ene6 = new TH1F( ph_hName_ene6.str().c_str(), ph_hTitle_ene6.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene6 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene6.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene6 );

      std::ostringstream ph_hName_ene7  ;
      std::ostringstream ph_hTitle_ene7 ;
      ph_hName_ene7  << "ph_ene_EME3" ;
      ph_hTitle_ene7 << "Photon cluster energy in CaloSampling::EME3 sampling" ;
      TH1* ph_h_ene7 = new TH1F( ph_hName_ene7.str().c_str(), ph_hTitle_ene7.str().c_str(), 50, -5000., 50000.);
      sc = egamma_photon_ene.regHist( ph_h_ene7 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_ene7.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_ene_histos.push_back( ph_h_ene7 );


 
      //cluster eta
      std::ostringstream ph_hName_eta0  ;
      std::ostringstream ph_hTitle_eta0 ;
      ph_hName_eta0  << "ph_eta_PreSamplerB" ;
      ph_hTitle_eta0 << "Photon cluster eta in CaloSampling::PreSamplerB sampling" ;
      TH1* ph_h_eta0 = new TH1F( ph_hName_eta0.str().c_str(), ph_hTitle_eta0.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta0 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta0.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_eta_histos.push_back( ph_h_eta0 );

      std::ostringstream ph_hName_eta1  ;
      std::ostringstream ph_hTitle_eta1 ;
      ph_hName_eta1  << "ph_eta_EMB1" ;
      ph_hTitle_eta1 << "Photon cluster eta in CaloSampling::EMB1 sampling" ;
      TH1* ph_h_eta1 = new TH1F( ph_hName_eta1.str().c_str(), ph_hTitle_eta1.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta1 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta1.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta1 );

      std::ostringstream ph_hName_eta2  ;
      std::ostringstream ph_hTitle_eta2 ;
      ph_hName_eta2  << "ph_eta_EMB2" ;
      ph_hTitle_eta2 << "Photon cluster eta in CaloSampling::EMB2 sampling" ;
      TH1* ph_h_eta2 = new TH1F( ph_hName_eta2.str().c_str(), ph_hTitle_eta2.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta2 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta2.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta2 );

      std::ostringstream ph_hName_eta3  ;
      std::ostringstream ph_hTitle_eta3 ;
      ph_hName_eta3  << "ph_eta_EMB3" ;
      ph_hTitle_eta3 << "Photon cluster eta in CaloSampling::EMB3 sampling" ;
      TH1* ph_h_eta3 = new TH1F( ph_hName_eta3.str().c_str(), ph_hTitle_eta3.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta3 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta3.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta3 );

      std::ostringstream ph_hName_eta4  ;
      std::ostringstream ph_hTitle_eta4 ;
      ph_hName_eta4  << "ph_eta_PreSamplerE" ;
      ph_hTitle_eta4 << "Photon cluster eta in CaloSampling::PreSamplerE sampling" ;
      TH1* ph_h_eta4 = new TH1F( ph_hName_eta4.str().c_str(), ph_hTitle_eta4.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta4 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta4.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta4 );

      std::ostringstream ph_hName_eta5  ;
      std::ostringstream ph_hTitle_eta5 ;
      ph_hName_eta5  << "ph_eta_EME1" ;
      ph_hTitle_eta5 << "Photon cluster eta in CaloSampling::EME1 sampling" ;
      TH1* ph_h_eta5 = new TH1F( ph_hName_eta5.str().c_str(), ph_hTitle_eta5.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta5 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta5.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta5 );

      std::ostringstream ph_hName_eta6  ;
      std::ostringstream ph_hTitle_eta6 ;
      ph_hName_eta6  << "ph_eta_EME2" ;
      ph_hTitle_eta6 << "Photon cluster eta in CaloSampling::EME2 sampling" ;
      TH1* ph_h_eta6 = new TH1F( ph_hName_eta6.str().c_str(), ph_hTitle_eta6.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta6 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta6.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta6 );

      std::ostringstream ph_hName_eta7  ;
      std::ostringstream ph_hTitle_eta7 ;
      ph_hName_eta7  << "ph_eta_EME3" ;
      ph_hTitle_eta7 << "Photon cluster eta in CaloSampling::EME3 sampling" ;
      TH1* ph_h_eta7 = new TH1F( ph_hName_eta7.str().c_str(), ph_hTitle_eta7.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_eta.regHist( ph_h_eta7 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_eta7.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_eta_histos.push_back( ph_h_eta7 );


      //cluster phi
      std::ostringstream ph_hName_phi0  ;
      std::ostringstream ph_hTitle_phi0 ;
      ph_hName_phi0  << "ph_phi_PreSamplerB" ;
      ph_hTitle_phi0 << "Photon cluster phi in CaloSampling::PreSamplerB sampling" ;
      TH1* ph_h_phi0 = new TH1F( ph_hName_phi0.str().c_str(), ph_hTitle_phi0.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi0 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi0.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi0 );

      std::ostringstream ph_hName_phi1  ;
      std::ostringstream ph_hTitle_phi1 ;
      ph_hName_phi1  << "ph_phi_EMB1" ;
      ph_hTitle_phi1 << "Photon cluster phi in CaloSampling::EMB1 sampling" ;
      TH1* ph_h_phi1 = new TH1F( ph_hName_phi1.str().c_str(), ph_hTitle_phi1.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi1 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi1.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi1 );

      std::ostringstream ph_hName_phi2  ;
      std::ostringstream ph_hTitle_phi2 ;
      ph_hName_phi2  << "ph_phi_EMB2" ;
      ph_hTitle_phi2 << "Photon cluster phi in CaloSampling::EMB2 sampling" ;
      TH1* ph_h_phi2 = new TH1F( ph_hName_phi2.str().c_str(), ph_hTitle_phi2.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi2 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi2.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi2 );

      std::ostringstream ph_hName_phi3  ;
      std::ostringstream ph_hTitle_phi3 ;
      ph_hName_phi3  << "ph_phi_EMB3" ;
      ph_hTitle_phi3 << "Photon cluster phi in CaloSampling::EMB3 sampling" ;
      TH1* ph_h_phi3 = new TH1F( ph_hName_phi3.str().c_str(), ph_hTitle_phi3.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi3 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi3.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi3 );

      std::ostringstream ph_hName_phi4  ;
      std::ostringstream ph_hTitle_phi4 ;
      ph_hName_phi4  << "ph_phi_PreSamplerE" ;
      ph_hTitle_phi4 << "Photon cluster phi in CaloSampling::PreSamplerE sampling" ;
      TH1* ph_h_phi4 = new TH1F( ph_hName_phi4.str().c_str(), ph_hTitle_phi4.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi4 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi4.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi4 );

      std::ostringstream ph_hName_phi5  ;
      std::ostringstream ph_hTitle_phi5 ;
      ph_hName_phi5  << "ph_phi_EME1" ;
      ph_hTitle_phi5 << "Photon cluster phi in CaloSampling::EME1 sampling" ;
      TH1* ph_h_phi5 = new TH1F( ph_hName_phi5.str().c_str(), ph_hTitle_phi5.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi5 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi5.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi5 );

      std::ostringstream ph_hName_phi6  ;
      std::ostringstream ph_hTitle_phi6 ;
      ph_hName_phi6  << "ph_phi_EME2" ;
      ph_hTitle_phi6 << "Photon cluster phi in CaloSampling::EME2 sampling" ;
      TH1* ph_h_phi6 = new TH1F( ph_hName_phi6.str().c_str(), ph_hTitle_phi6.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi6 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi6.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi6 );
 
      std::ostringstream ph_hName_phi7  ;
      std::ostringstream ph_hTitle_phi7 ;
      ph_hName_phi7  << "ph_phi_EME3" ;
      ph_hTitle_phi7 << "Photon cluster phi in CaloSampling::EME3 sampling" ;
      TH1* ph_h_phi7 = new TH1F( ph_hName_phi7.str().c_str(), ph_hTitle_phi7.str().c_str(), 50, -4., 4.);
      sc = egamma_photon_phi.regHist( ph_h_phi7 );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<ph_hName_phi7.str().c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_phi_histos.push_back( ph_h_phi7 );



  }
  return StatusCode::SUCCESS;
}

// ==========================================================
StatusCode
egammaMonTool::
bookEoverPElectronHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/,
			  MonGroup egamma_electron_EoverP, MonGroup /*egamma_electron_EoverP_EtaBin*/, MonGroup /*egamma_electron_EoverP_PhiBin*/)
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookEoverPElectronHistos()"<<endreq;
  StatusCode sc;

  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {


      // Histograms for E/P studies
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tCreating E/p studies' histos"<<endreq;
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\t m_eta_cuts.size(): "<<m_eta_cuts.size()<<endreq;
      // E/p vs eta
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs eta electrons "<<endreq;
      std::string eleName = "ElectronEoverPvsEta";
      std::string eleTitle = "E/P vs eta for Electrons";
      m_el_Electron_EoverPvsEta = new TH2F(eleName.c_str(),
					   eleTitle.c_str(),
					   100,0,5,
					   50, m_eta_cuts[0],m_eta_cuts[m_eta_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_Electron_EoverPvsEta);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs eta positrons"<<endreq;
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\t m_eta_cuts.size(): "<<m_eta_cuts.size()<<endreq;
      eleName = "PositronEoverPvsEta";
      eleTitle = "E/P vs eta for Positrons";
      m_el_Positron_EoverPvsEta = new TH2F(eleName.c_str(),
					   eleTitle.c_str(),
					   100,0,5,
					   50, m_eta_cuts[0],m_eta_cuts[m_eta_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_Positron_EoverPvsEta);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      // E/p vs eta (Tight)
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs eta Tight electrons"<<endreq;
      eleName = "TightElectronEoverPvsEta";
      eleTitle = "E/P vs eta for Tight Electrons";
      m_el_TightElectron_EoverPvsEta = new TH2F(eleName.c_str(),
						eleTitle.c_str(),
						100,0,5,
						50, m_eta_cuts[0],m_eta_cuts[m_eta_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_TightElectron_EoverPvsEta);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs eta Tight positrons"<<endreq;
      eleName = "TightPositronEoverPvsEta";
      eleTitle = "E/P vs eta for Tight Positrons";
      m_el_TightPositron_EoverPvsEta = new TH2F(eleName.c_str(),
						eleTitle.c_str(),
						100,0,5,
						50, m_eta_cuts[0],m_eta_cuts[m_eta_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_TightPositron_EoverPvsEta);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      // E/p vs phi
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs phi electrons"<<endreq;
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\t m_phi_cuts.size(): "<<m_phi_cuts.size()<<endreq;
      eleName = "ElectronEoverPvsPhi";
      eleTitle = "E/P vs phi for Electrons";
      m_el_Electron_EoverPvsPhi = new TH2F(eleName.c_str(),
					   eleTitle.c_str(),
					   100,0,5,
					   50, m_phi_cuts[0],m_phi_cuts[m_phi_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_Electron_EoverPvsPhi);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs phi positrons"<<endreq;
      eleName = "PositronEoverPvsPhi";
      eleTitle = "E/P vs eta for Positrons";
      m_el_Positron_EoverPvsPhi = new TH2F(eleName.c_str(),
					   eleTitle.c_str(),
					   100,0,5,
					   50, m_phi_cuts[0],m_phi_cuts[m_phi_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_Positron_EoverPvsPhi);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      // E/p vs phi (Tight)
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs phi Tight electrons"<<endreq;
      eleName = "TightElectronEoverPvsPhi";
      eleTitle = "E/P vs phi for Tight Electrons";
      m_el_TightElectron_EoverPvsPhi = new TH2F(eleName.c_str(),
						eleTitle.c_str(),
						100,0,5,
						50, -m_phi_cuts[m_phi_cuts.size()-1],m_phi_cuts[m_phi_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_TightElectron_EoverPvsPhi);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p vs phi Tight positrons"<<endreq;
      eleName = "TightPositronEoverPvsPhi";
      eleTitle = "E/P vs phi for Tight Positrons";
      m_el_TightPositron_EoverPvsPhi = new TH2F(eleName.c_str(),
						eleTitle.c_str(),
						100,0,5,
						50, m_phi_cuts[0],m_phi_cuts[m_phi_cuts.size()-1]);
      sc = egamma_electron_EoverP.regHist(m_el_TightPositron_EoverPvsPhi);
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
        return StatusCode::FAILURE;
      }     
      // E/p (Electrons)
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p > 2 for electrons"<<endreq;
      eleName = "EoverP_Large_Electrons";
      eleTitle = "E/P for Electrons with E/p > 2";
      m_el_Electron_EoverP = new TH1F(eleName.c_str(),
				      eleTitle.c_str(),
				      100, 2, 5);
      sc = egamma_electron_EoverP.regHist(m_el_Electron_EoverP);
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }
      // E/p (Positrons)
      log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tE/p > 2 for positrons"<<endreq;
      eleName = "EoverP_Large_Positrons";
      eleTitle = "E/P for Positrons with E/p > 2";
      m_el_Positron_EoverP = new TH1F(eleName.c_str(),
				      eleTitle.c_str(),
				      100, 2, 5);
      sc = egamma_electron_EoverP.regHist(m_el_Positron_EoverP);
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<eleName.c_str()<<endreq;
	return StatusCode::FAILURE;
      }


  }

  return StatusCode::SUCCESS;
}


StatusCode
egammaMonTool::
bookZmassHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/)
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookZmassHistos()"<<endreq;
  StatusCode sc;

  //-REMI MonGroup Z ( this, "egamma/Electron/Physics/Zee", expert, run );

  m_z_ClusterEt = new TH1F("z_LeadingClustersEt","Et of leading clusters", 100,
			   0, 150000 );
  //-REMI sc = Z.regHist(m_z_ClusterEt);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClustersEt";
  }
  m_z_ClusterEta = new TH1F("z_LeadingClustersEta","Eta of leading clusters", 100,
			   -2.5, 2.5 );
  //-REMI sc = Z.regHist(m_z_ClusterEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClustersEta";
  }
  m_z_ClusterPhi = new TH1F("z_LeadingClustersPhi","Phi of leading clusters", 100,
			   -4, 4 );
  //-REMI sc = Z.regHist(m_z_ClusterPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClustersPhi";
  }
  m_z_ClusterEtvsEta = new TH2F("z_LeadingClustersEtvsEta","Et vs Eta of leading clusters", 
				100, -2.5, 2.5,
				100, 0, 150000
				);
  //-REMI sc = Z.regHist(m_z_ClusterEtvsEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClustersEtvsEta";
  }
  m_z_ClusterEtvsPhi = new TH2F("z_LeadingClustersEtvsPhi","Et vs Phi of leading clusters", 
				100, -4, 4,
				100, 0, 150000
				);
  //-REMI sc = Z.regHist(m_z_ClusterEtvsPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClustersEtvsPhi";
  }

  m_z_TrackPt = new TH1F("z_LeadingTrackPt","Pt of tracks associated to leading clusters",
			 100, 0, 150000);
  //-REMI sc = Z.regHist(m_z_TrackPt);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingTrackPt";
  }
  m_z_TrackEta = new TH1F("z_LeadingTrackEta", "Eta of tracks associated to leading clusters",
			  100, -2.5, 2.5);
  //-REMI sc = Z.regHist(m_z_TrackEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingTrackEta";
  }
  m_z_TrackPhi = new TH1F("z_LeadingTrackPhi", "Phi of tracks associated to leading clusters",
			  100, -4, 4);
  //-REMI sc = Z.regHist(m_z_TrackPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingTrackPhi";
  }
  m_z_TrackPtvsEta = new TH2F("z_LeadingTrackPtvsEta", "Pt vs Eta of tracks associated to leading clusters",
			      100, -2.5, 2.5,
			      100, 0, 150000
			      );
  //-REMI sc = Z.regHist(m_z_TrackPtvsEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingTrackPtvsEta";
  }
  m_z_TrackPtvsPhi = new TH2F("z_LeadingTrackPtvsPhi", "Pt vs Phi of tracks associated to leading clusters",
			      100, -4, 4,
			      100, 0, 150000
			      );
  //-REMI sc = Z.regHist(m_z_TrackPtvsPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingTrackPtvsPhi";
  }

  m_z_ClusterWithTrackEt = new TH1F("z_LeadingClusterWithTrackEt","Et of clusters with tracks associated to leading clusters",
			 100, 0, 150000);
  //-REMI sc = Z.regHist(m_z_ClusterWithTrackEt);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClusterWithTrackEt";
  }
  m_z_ClusterWithTrackEta = new TH1F("z_LeadingClusterWithTrackEta", "Eta of clusters with tracks associated to leading clusters",
			  100, -2.5, 2.5);
  //-REMI sc = Z.regHist(m_z_ClusterWithTrackEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClusterWithTrackEta";
  }
  m_z_ClusterWithTrackPhi = new TH1F("z_LeadingClusterWithTrackPhi", "Phi of clusters with tracks associated to leading clusters",
			  100, -4, 4);
  //-REMI sc = Z.regHist(m_z_ClusterWithTrackPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClusterWithTrackPhi";
  }
  m_z_ClusterWithTrackEtvsEta = new TH2F("z_LeadingClusterWithTrackEtvsEta", "Et vs Eta of clusters with tracks associated to leading clusters",
			      100, -2.5, 2.5,
			      100, 0, 150000
			      );
  //-REMI sc = Z.regHist(m_z_ClusterWithTrackEtvsEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClusterWithTrackEtvsEta";
  }
  m_z_ClusterWithTrackEtvsPhi = new TH2F("z_LeadingClusterWithTrackEtvsPhi", "Et vs Phi of clusters with tracks associated to leading clusters",
			      100, -4, 4,
			      100, 0, 150000
			      );
  //-REMI sc = Z.regHist(m_z_ClusterWithTrackEtvsPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingClusterWithTrackEtvsPhi";
  }

  m_z_CutClusterEt = new TH1F("z_LeadingCutClustersEt","Et of leading clusters after identification criteria", 100,
			   0, 150000 );
  //-REMI sc = Z.regHist(m_z_CutClusterEt);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingCutClustersEt";
  }
  m_z_CutClusterEta = new TH1F("z_LeadingCutClustersEta","Eta of leading clusters after identification criteria", 100,
			   -2.5, 2.5 );
  //-REMI sc = Z.regHist(m_z_CutClusterEta);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingCutClustersEta";
  }
  m_z_CutClusterPhi = new TH1F("z_LeadingCutClustersPhi","Phi of leading clusters after identification criteria", 100,
			   -4, 4 );
  //-REMI sc = Z.regHist(m_z_CutClusterPhi);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_LeadingCutClustersPhi";
  }

  m_z_mass = new TH1F("z_mass","Invariant mass", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass";
  }
  m_z_mass_bb = new TH1F("z_mass_bb","Invariant mass (Both clusters in barrel)", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass_bb);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass_bb";
  }
  m_z_mass_be = new TH1F("z_mass_be","Invariant mass (One cluster in barrel, one in endcap)", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass_be);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass_be";
  }
  m_z_mass_bc = new TH1F("z_mass_bc","Invariant mass (One cluster in barrel, one in crack)", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass_bc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass_bc";
  }
  m_z_mass_ec = new TH1F("z_mass_ec","Invariant mass (One cluster in endcap, one in crack)", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass_ec);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass_ec";
  }
  m_z_mass_ee = new TH1F("z_mass_ee","Invariant mass (Both clusters in endcaps)", 80, -20000, 20000);
  //-REMI sc = Z.regHist(m_z_mass_ee);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram z_mass_ee";
  }


  // Efficiency profiles
  m_z_ClusterEtEff = new TProfile("m_z_ClusterEtEfficiency","Efficiency vs. Et of the cluster", 100, 0, 150000, 0, 1.1);
  m_z_ClusterEtaEff = new TProfile("m_z_ClusterEtaEfficiency","Efficiency vs. Eta of the cluster", 100, -2.5, 2.5, 0, 1.1);
  m_z_ClusterPhiEff = new TProfile("m_z_ClusterPhiEfficiency","Efficiency vs. Phi of the cluster", 100, -4, 4, 0, 1.1);
  m_z_CutClusterEtEff = new TProfile("m_z_CutClusterEtEfficiency","Efficiency vs. Et of the cluster with identification criteria", 100, 0, 150000, 0, 1.1);
  m_z_CutClusterEtaEff = new TProfile("m_z_CutClusterEtaEfficiency","Efficiency vs. Eta of the cluster with identification criteria", 100, -2.5, 2.5, 0, 1.1);
  m_z_CutClusterPhiEff = new TProfile("m_z_CutClusterPhiEfficiency","Efficiency vs. Phi of the cluster with identification criteria", 100, -4, 4, 0, 1.1);

  //-REMI sc = Z.regHist(m_z_ClusterEtEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_ClusterEtEfficiency";
  }
  //-REMI sc = Z.regHist(m_z_ClusterEtaEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_ClusterEtaEfficiency";
  }
  //-REMI sc = Z.regHist(m_z_ClusterPhiEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_ClusterPhiEfficiency";
  }
  //-REMI sc = Z.regHist(m_z_CutClusterEtEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_CutClusterEtEfficiency";
  }
  //-REMI sc = Z.regHist(m_z_CutClusterEtaEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_CutClusterEtaEfficiency";
  }
  //-REMI sc = Z.regHist(m_z_CutClusterPhiEff);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Unable to register histogram m_z_CutClusterPhiEfficiency";
  }

  return StatusCode::SUCCESS;
}

// ===========================================================
StatusCode
egammaMonTool::
bookCaloClusterElectronHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/,
			  MonGroup egamma_electron_ene, MonGroup egamma_electron_eta,
			  MonGroup egamma_electron_phi)
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookCaloClusterElectronHistos()"<<endreq;
  StatusCode sc;

  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {

      //CaloCluster variables / expert
      log <<MSG::DEBUG<<"egammaMonTool::bookCaloClusterElectronHistos()\tCreating CaloCluster electron histos"<<endreq;
      //cluster energy in different samples
      std::ostringstream hName_ene0  ;
      std::ostringstream hTitle_ene0 ;
      hName_ene0  << "el_ene_PreSamplerB" ;
      hTitle_ene0 << "Electron cluster energy in CaloSampling::PreSamplerB sampling" ;
      TH1* h_ene0 = new TH1F( hName_ene0.str().c_str(), hTitle_ene0.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene0 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene0.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene0 );

      std::ostringstream hName_ene1  ;
      std::ostringstream hTitle_ene1 ;
      hName_ene1  << "el_ene_EMB1" ;
      hTitle_ene1 << "Electron cluster energy in CaloSampling::EMB1 sampling" ;
      TH1* h_ene1 = new TH1F( hName_ene1.str().c_str(), hTitle_ene1.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene1 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene1.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene1 );

      std::ostringstream hName_ene2  ;
      std::ostringstream hTitle_ene2 ;
      hName_ene2  << "el_ene_EMB2" ;
      hTitle_ene2 << "Electron cluster energy in CaloSampling::EMB2 sampling" ;
      TH1* h_ene2 = new TH1F( hName_ene2.str().c_str(), hTitle_ene2.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene2 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene2.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene2 );

      std::ostringstream hName_ene3  ;
      std::ostringstream hTitle_ene3 ;
      hName_ene3  << "el_ene_EMB3" ;
      hTitle_ene3 << "Electron cluster energy in CaloSampling::EMB3 sampling" ;
      TH1* h_ene3 = new TH1F( hName_ene3.str().c_str(), hTitle_ene3.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene3 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene3.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene3 );

      std::ostringstream hName_ene4  ;
      std::ostringstream hTitle_ene4 ;
      hName_ene4  << "el_ene_PreSamplerE" ;
      hTitle_ene4 << "Electron cluster energy in CaloSampling::PreSamplerE sampling" ;
      TH1* h_ene4 = new TH1F( hName_ene4.str().c_str(), hTitle_ene4.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene4 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene4.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene4 );

      std::ostringstream hName_ene5  ;
      std::ostringstream hTitle_ene5 ;
      hName_ene5  << "el_ene_EME1" ;
      hTitle_ene5 << "Electron cluster energy in CaloSampling::EME1 sampling" ;
      TH1* h_ene5 = new TH1F( hName_ene5.str().c_str(), hTitle_ene5.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene5 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene5.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene5 );

      std::ostringstream hName_ene6  ;
      std::ostringstream hTitle_ene6 ;
      hName_ene6  << "el_ene_EME2" ;
      hTitle_ene6 << "Electron cluster energy in CaloSampling::EME2 sampling" ;
      TH1* h_ene6 = new TH1F( hName_ene6.str().c_str(), hTitle_ene6.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene6 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene6.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene6 );

      std::ostringstream hName_ene7  ;
      std::ostringstream hTitle_ene7 ;
      hName_ene7  << "el_ene_EME3" ;
      hTitle_ene7 << "Electron cluster energy in CaloSampling::EME3 sampling" ;
      TH1* h_ene7 = new TH1F( hName_ene7.str().c_str(), hTitle_ene7.str().c_str(), 50, -5000., 50000.);
      sc = egamma_electron_ene.regHist( h_ene7 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_ene7.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_ene_histos.push_back( h_ene7 );

      //cluster eta
      std::ostringstream hName_eta0  ;
      std::ostringstream hTitle_eta0 ;
      hName_eta0  << "el_eta_PreSamplerB" ;
      hTitle_eta0 << "Electron cluster eta in CaloSampling::PreSamplerB sampling" ;
      TH1* h_eta0 = new TH1F( hName_eta0.str().c_str(), hTitle_eta0.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta0 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta0.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta0 );

      std::ostringstream hName_eta1  ;
      std::ostringstream hTitle_eta1 ;
      hName_eta1  << "el_eta_EMB1" ;
      hTitle_eta1 << "Electron cluster eta in CaloSampling::EMB1 sampling" ;
      TH1* h_eta1 = new TH1F( hName_eta1.str().c_str(), hTitle_eta1.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta1 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta1.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta1 );

      std::ostringstream hName_eta2  ;
      std::ostringstream hTitle_eta2 ;
      hName_eta2  << "el_eta_EMB2" ;
      hTitle_eta2 << "Electron cluster eta in CaloSampling::EMB2 sampling" ;
      TH1* h_eta2 = new TH1F( hName_eta2.str().c_str(), hTitle_eta2.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta2 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta2.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta2 );

      std::ostringstream hName_eta3  ;
      std::ostringstream hTitle_eta3 ;
      hName_eta3  << "el_eta_EMB3" ;
      hTitle_eta3 << "Electron cluster eta in CaloSampling::EMB3 sampling" ;
      TH1* h_eta3 = new TH1F( hName_eta3.str().c_str(), hTitle_eta3.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta3 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta3.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta3 );

      std::ostringstream hName_eta4  ;
      std::ostringstream hTitle_eta4 ;
      hName_eta4  << "el_eta_PreSamplerE" ;
      hTitle_eta4 << "Electron cluster eta in CaloSampling::PreSamplerE sampling" ;
      TH1* h_eta4 = new TH1F( hName_eta4.str().c_str(), hTitle_eta4.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta4 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta4.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta4 );

      std::ostringstream hName_eta5  ;
      std::ostringstream hTitle_eta5 ;
      hName_eta5  << "el_eta_EME1" ;
      hTitle_eta5 << "Electron cluster eta in CaloSampling::EME1 sampling" ;
      TH1* h_eta5 = new TH1F( hName_eta5.str().c_str(), hTitle_eta5.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta5 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta5.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta5 );

      std::ostringstream hName_eta6  ;
      std::ostringstream hTitle_eta6 ;
      hName_eta6  << "el_eta_EME2" ;
      hTitle_eta6 << "Electron cluster eta in CaloSampling::EME2 sampling" ;
      TH1* h_eta6 = new TH1F( hName_eta6.str().c_str(), hTitle_eta6.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta6 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta6.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta6 );

      std::ostringstream hName_eta7  ;
      std::ostringstream hTitle_eta7 ;
      hName_eta7  << "el_eta_EME3" ;
      hTitle_eta7 << "Electron cluster eta in CaloSampling::EME3 sampling" ;
      TH1* h_eta7 = new TH1F( hName_eta7.str().c_str(), hTitle_eta7.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_eta.regHist( h_eta7 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_eta7.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_eta_histos.push_back( h_eta7 );


      //cluster phi
      std::ostringstream hName_phi0  ;
      std::ostringstream hTitle_phi0 ;
      hName_phi0  << "el_phi_PreSamplerB" ;
      hTitle_phi0 << "Electron cluster phi in CaloSampling::PreSamplerB sampling" ;
      TH1* h_phi0 = new TH1F( hName_phi0.str().c_str(), hTitle_phi0.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi0 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi0.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi0 );

      std::ostringstream hName_phi1  ;
      std::ostringstream hTitle_phi1 ;
      hName_phi1  << "el_phi_EMB1" ;
      hTitle_phi1 << "Electron cluster phi in CaloSampling::EMB1 sampling" ;
      TH1* h_phi1 = new TH1F( hName_phi1.str().c_str(), hTitle_phi1.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi1 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi1.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi1 );

      std::ostringstream hName_phi2  ;
      std::ostringstream hTitle_phi2 ;
      hName_phi2  << "el_phi_EMB2" ;
      hTitle_phi2 << "Electron cluster phi in CaloSampling::EMB2 sampling" ;
      TH1* h_phi2 = new TH1F( hName_phi2.str().c_str(), hTitle_phi2.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi2 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi2.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi2 );

      std::ostringstream hName_phi3  ;
      std::ostringstream hTitle_phi3 ;
      hName_phi3  << "el_phi_EMB3" ;
      hTitle_phi3 << "Electron cluster phi in CaloSampling::EMB3 sampling" ;
      TH1* h_phi3 = new TH1F( hName_phi3.str().c_str(), hTitle_phi3.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi3 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi3.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi3 );

      std::ostringstream hName_phi4  ;
      std::ostringstream hTitle_phi4 ;
      hName_phi4  << "el_phi_PreSamplerE" ;
      hTitle_phi4 << "Electron cluster phi in CaloSampling::PreSamplerE sampling" ;
      TH1* h_phi4 = new TH1F( hName_phi4.str().c_str(), hTitle_phi4.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi4 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi4.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi4 );

      std::ostringstream hName_phi5  ;
      std::ostringstream hTitle_phi5 ;
      hName_phi5  << "el_phi_EME1" ;
      hTitle_phi5 << "Electron cluster phi in CaloSampling::EME1 sampling" ;
      TH1* h_phi5 = new TH1F( hName_phi5.str().c_str(), hTitle_phi5.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi5 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi5.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi5 );

      std::ostringstream hName_phi6  ;
      std::ostringstream hTitle_phi6 ;
      hName_phi6  << "el_phi_EME2" ;
      hTitle_phi6 << "Electron cluster phi in CaloSampling::EME2 sampling" ;
      TH1* h_phi6 = new TH1F( hName_phi6.str().c_str(), hTitle_phi6.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi6 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi6.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi6 );

      std::ostringstream hName_phi7  ;
      std::ostringstream hTitle_phi7 ;
      hName_phi7  << "el_phi_EME3" ;
      hTitle_phi7 << "Electron cluster phi in CaloSampling::EME3 sampling" ;
      TH1* h_phi7 = new TH1F( hName_phi7.str().c_str(), hTitle_phi7.str().c_str(), 50, -4., 4.);
      sc = egamma_electron_phi.regHist( h_phi7 );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<hName_phi7.str().c_str()<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_phi_histos.push_back( h_phi7 );

  }


  return StatusCode::SUCCESS;
}

// ===========================================================
StatusCode
egammaMonTool::
bookMediumElectronHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/, MonGroup egamma_electron_medium )
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookMediumElectronHistos()"<<endreq;
  StatusCode sc;

  // Book ESD capable histos
  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {

    log <<MSG::DEBUG<<"egammaMonTool::bookMediumElectronHistos()Creating basic electrons histos"<<endreq;
    ////  Electron ID by Medium cut histograms  ////
    // Et
    std::ostringstream el_IsEMMedium_ClusterEtHistoName ;
    std::ostringstream el_IsEMMedium_ClusterEtHistoTitle;
    el_IsEMMedium_ClusterEtHistoName << "el_ClusterEt" ;
    el_IsEMMedium_ClusterEtHistoTitle << "Electron Cluster Et" ;
    m_el_IsEMMedium_ClusterEtHisto = new TH1F( el_IsEMMedium_ClusterEtHistoName.str().c_str(),el_IsEMMedium_ClusterEtHistoTitle.str().c_str(), 100, -1000, 100000 );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_ClusterEtHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_ClusterEtHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // Eta
    std::ostringstream el_IsEMMedium_ClusterEtaHistoName  ;
    std::ostringstream el_IsEMMedium_ClusterEtaHistoTitle ;
    el_IsEMMedium_ClusterEtaHistoName  << "el_ClusterEta" ;
    el_IsEMMedium_ClusterEtaHistoTitle << "Electron Cluster eta" ;
    m_el_IsEMMedium_ClusterEtaHisto = new TH1F( el_IsEMMedium_ClusterEtaHistoName.str().c_str(), el_IsEMMedium_ClusterEtaHistoTitle.str().c_str(), 50, -3.2, 3.2 );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_ClusterEtaHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_ClusterEtaHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // Phi
    std::ostringstream el_IsEMMedium_ClusterPhiName ;
    std::ostringstream el_IsEMMedium_ClusterPhiTitle ;
    el_IsEMMedium_ClusterPhiName  << "el_ClusterPhi" ;
    el_IsEMMedium_ClusterPhiTitle << "Electron Cluster phi" ;
    m_el_IsEMMedium_ClusterPhiHisto = new TH1F( el_IsEMMedium_ClusterPhiName.str().c_str(), el_IsEMMedium_ClusterPhiTitle.str().c_str(), 50, -4., 4. );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_ClusterPhiHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_ClusterPhiName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // EtvsEta
    std::ostringstream el_IsEMMedium_EtvsEtaHistoName  ;
    std::ostringstream el_IsEMMedium_EtvsEtaHistoTitle ;
    el_IsEMMedium_EtvsEtaHistoName << "el_EtVsEta" ;
    el_IsEMMedium_EtvsEtaHistoTitle << "Electron Et vs. Eta" ;
    m_el_IsEMMedium_EtvsEtaHisto = new TH2F( el_IsEMMedium_EtvsEtaHistoName.str().c_str(), el_IsEMMedium_EtvsEtaHistoTitle.str().c_str(),
					     100, -2.5,2.5,
					     50, 0, 100000
					     );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_EtvsEtaHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_EtvsEtaHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // EtvsPhi
    std::ostringstream el_IsEMMedium_EtvsPhiHistoName ;
    std::ostringstream el_IsEMMedium_EtvsPhiHistoTitle ;
    el_IsEMMedium_EtvsPhiHistoName  << "el_EtVsPhi" ;
    el_IsEMMedium_EtvsPhiHistoTitle << "Electron Et vs. Phi" ;
    m_el_IsEMMedium_EtvsPhiHisto = new TH2F( el_IsEMMedium_EtvsPhiHistoName.str().c_str(), el_IsEMMedium_EtvsPhiHistoTitle.str().c_str(),
					     100, -4., 4.,
					     50, 0, 100000
					     );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_EtvsPhiHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_EtvsPhiHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // EoverP
    std::ostringstream  el_IsEMMedium_EoverPHistoName ;
    std::ostringstream  el_IsEMMedium_EoverPHistoTitle ;
    el_IsEMMedium_EoverPHistoName << "el_EoverP" ;
    el_IsEMMedium_EoverPHistoTitle << "Electron track match E/p (1st sampling)" ;
    m_el_IsEMMedium_EoverPHisto = new TH1F( el_IsEMMedium_EoverPHistoName.str().c_str(), el_IsEMMedium_EoverPHistoTitle.str().c_str(), 50,0,3);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_EoverPHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_EoverPHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    //DeltaEta1
    std::ostringstream  el_IsEMMedium_DeltaEta1HistoName ;
    std::ostringstream  el_IsEMMedium_DeltaEta1HistoTitle ;
    el_IsEMMedium_DeltaEta1HistoName << "el_deltaEta1" ;
    el_IsEMMedium_DeltaEta1HistoTitle << "Electron track match delta eta (1st sampling)" ;
    m_el_IsEMMedium_DeltaEta1Histo = new TH1F( el_IsEMMedium_DeltaEta1HistoName.str().c_str(), el_IsEMMedium_DeltaEta1HistoTitle.str().c_str(), 50, -0.05, 0.05 );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_DeltaEta1Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_DeltaEta1HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // DeltaPhi2
    std::ostringstream el_IsEMMedium_DeltaPhi2HistoName ;
    std::ostringstream el_IsEMMedium_DeltaPhi2HistoTitle ;
    el_IsEMMedium_DeltaPhi2HistoName << "el_deltaPhi2" ;
    el_IsEMMedium_DeltaPhi2HistoTitle << "Electron track match delta phi (2nd sampling)" ;
    m_el_IsEMMedium_DeltaPhi2Histo = new TH1F( el_IsEMMedium_DeltaPhi2HistoName.str().c_str(), el_IsEMMedium_DeltaPhi2HistoTitle.str().c_str(), 50, -0.1, 0.1 );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_DeltaPhi2Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_DeltaPhi2HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // EtHad1
    std::ostringstream el_IsEMMedium_EthadHistoName ;
    std::ostringstream el_IsEMMedium_EthadHistoTitle ;
    el_IsEMMedium_EthadHistoName << "el_Ethad" ;
    el_IsEMMedium_EthadHistoTitle << "Electron energy leakage in 1st sampling of hadcal" ;
    m_el_IsEMMedium_EthadHisto = new TH1F( el_IsEMMedium_EthadHistoName.str().c_str(), el_IsEMMedium_EthadHistoTitle.str().c_str(), 50, -0.1, 0.1 );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_EthadHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_EthadHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // F1
    std::ostringstream el_IsEMMedium_F1HistoName;
    std::ostringstream el_IsEMMedium_F1HistoTitle;
    el_IsEMMedium_F1HistoName<<"el_f1";
    el_IsEMMedium_F1HistoTitle<<"Electron fractional energy in 1st sampling";
    m_el_IsEMMedium_F1Histo = new TH1F(el_IsEMMedium_F1HistoName.str().c_str(), el_IsEMMedium_F1HistoTitle.str().c_str(), 50, -1, 1);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_F1Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_F1HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // F2
    std::ostringstream el_IsEMMedium_F2HistoName;
    std::ostringstream el_IsEMMedium_F2HistoTitle;
    el_IsEMMedium_F2HistoName<<"el_f2";
    el_IsEMMedium_F2HistoTitle<<"Electron fractional energy in 2nd sampling";
    m_el_IsEMMedium_F2Histo = new TH1F(el_IsEMMedium_F2HistoName.str().c_str(), el_IsEMMedium_F2HistoTitle.str().c_str(), 50, -1, 1);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_F2Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_F2HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // F3
    std::ostringstream el_IsEMMedium_F3HistoName;
    std::ostringstream el_IsEMMedium_F3HistoTitle;
    el_IsEMMedium_F3HistoName<<"el_f3";
    el_IsEMMedium_F3HistoTitle<<"Electron fractional energy in 3rd sampling";
    m_el_IsEMMedium_F3Histo = new TH1F(el_IsEMMedium_F3HistoName.str().c_str(), el_IsEMMedium_F3HistoTitle.str().c_str(), 50, -1, 1);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_F3Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_F3HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // rE233E237
    std::ostringstream el_IsEMMedium_rE233E237HistoName ;
    std::ostringstream el_IsEMMedium_rE233E237HistoTitle ;
    el_IsEMMedium_rE233E237HistoName << "el_Re233e237" ;
    el_IsEMMedium_rE233E237HistoTitle << "Electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2" ;
    m_el_IsEMMedium_rE233E237Histo = new TH1F( el_IsEMMedium_rE233E237HistoName.str().c_str(), el_IsEMMedium_rE233E237HistoTitle.str().c_str(), 50, -.5, 2.);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_rE233E237Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_rE233E237HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // rE237E277
    std::ostringstream el_IsEMMedium_rE237E277HistoName ;
    std::ostringstream el_IsEMMedium_rE237E277HistoTitle ;
    el_IsEMMedium_rE237E277HistoName << "el_Re237e277" ;
    el_IsEMMedium_rE237E277HistoTitle << "Electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2" ;
    m_el_IsEMMedium_rE237E277Histo = new TH1F( el_IsEMMedium_rE237E277HistoName.str().c_str(), el_IsEMMedium_rE237E277HistoTitle.str().c_str(), 50, -.5, 2. );
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_rE237E277Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_rE237E277HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    // High Et ranges
    std::ostringstream el_IsEMMedium_HighEtRangesHistoName ;
    std::ostringstream el_IsEMMedium_HighEtRangesHistoTitle;
    el_IsEMMedium_HighEtRangesHistoName << "el_HighEtRanges" ;
    el_IsEMMedium_HighEtRangesHistoTitle << "Electrons with Et > 100, 500 and 1000 GeV" ;
    m_el_IsEMMedium_HighEtRangesHisto = new TH1F( el_IsEMMedium_HighEtRangesHistoName.str().c_str(), el_IsEMMedium_HighEtRangesHistoTitle.str().c_str(), 3, 0, 3);
    sc = egamma_electron_medium.regHist( m_el_IsEMMedium_HighEtRangesHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMMedium_HighEtRangesHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

  }

  return StatusCode::SUCCESS;

}


StatusCode
egammaMonTool::
bookBaseElectronHistos( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/, MonGroup egamma_electron )
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookBaseElectronHistos()"<<endreq;
  StatusCode sc;

  // Book ESD capable histos
  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {

    log <<MSG::DEBUG<<"egammaMonTool::bookBaseElectronHistos()Creating basic electrons histos"<<endreq;
    //egamma variables / shift
    std::ostringstream hName0 ;
    std::ostringstream hTitle0;
    hName0  << "el_ClusterEt" ;
    hTitle0 << "Electron Cluster Et" ;
    TH1* h0 = new TH1F( hName0.str().c_str(), hTitle0.str().c_str(), 100, -1000, 250000 );
    sc = egamma_electron.regHist( h0 );	
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName0.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }
    m_el_histos.push_back( h0 );		

    std::ostringstream hName1  ;
    std::ostringstream hTitle1 ;
    hName1  << "el_ClusterEta" ;
    hTitle1 << "Electron Cluster eta" ;
    TH1* h1 = new TH1F( hName1.str().c_str(), hTitle1.str().c_str(), 50, -3.2, 3.2 );
    sc = egamma_electron.regHist( h1 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName1.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h1 );

    //EMTrackMatch variables / shift
//     std::ostringstream hName2  ;
//     std::ostringstream hTitle2 ;
//     hName2  << "el_EtaCorrMag" ;
//     hTitle2 << "Electron match track eta corr. magnitude" ;
//     TH1* h2 = new TH1F( hName2.str().c_str(), hTitle2.str().c_str(), 50, -2., 2. );
//     sc = egamma_electron.regHist( h2 );
//     if (sc == StatusCode::FAILURE){
//       log <<MSG::FATAL <<"Failed to register "<<hName2.str().c_str()<<endreq;
//       return StatusCode::FAILURE;
//     }

//     m_el_histos.push_back( h2 );

    std::ostringstream hName3  ;
    std::ostringstream hTitle3 ;
    hName3  << "el_EoverP" ;
    hTitle3 << "Electron match track E over P" ;
    TH1* h3 = new TH1F( hName3.str().c_str(), hTitle3.str().c_str(), 50, 0., 3. );
    sc = egamma_electron.regHist( h3 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName3.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h3 );

    std::ostringstream hName4  ;
    std::ostringstream hTitle4 ;
    hName4  << "el_deltaEta1" ;
    hTitle4 << "Electron track match delta eta (1st sampling)" ;
    TH1* h4 = new TH1F( hName4.str().c_str(), hTitle4.str().c_str(), 50, -0.05, 0.05 );
    sc = egamma_electron.regHist( h4 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName4.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h4 );

    std::ostringstream hName5  ;
    std::ostringstream hTitle5 ;
    hName5  << "el_deltaPhi2" ;
    hTitle5 << "Electron track match delta phi (2nd sampling)" ;
    TH1* h5 = new TH1F( hName5.str().c_str(), hTitle5.str().c_str(), 50, -0.1, 0.1 );
    sc = egamma_electron.regHist( h5 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName5.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h5 );
	
    //EMShower variables / shift
    std::ostringstream hName6  ;
    std::ostringstream hTitle6 ;
    hName6  << "el_f1" ;
    hTitle6 << "Electron fractional energy in 1st sampling" ;
    TH1* h6 = new TH1F( hName6.str().c_str(), hTitle6.str().c_str(), 50, -1., 1.);
    sc = egamma_electron.regHist( h6 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName6.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h6 );

    std::ostringstream el_F2HistoName  ;
    std::ostringstream el_F2HistoTitle;
    el_F2HistoName << "el_f2" ;
    el_F2HistoTitle << "Electron fractional energy in 2nd sampling" ;
    m_el_F2Histo = new TH1F( el_F2HistoName.str().c_str(), el_F2HistoTitle.str().c_str(), 50, -1., 1);
    sc = egamma_electron.regHist( m_el_F2Histo );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_F2HistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }


    std::ostringstream hName7  ;
    std::ostringstream hTitle7 ;
    hName7  << "el_f3" ;
    hTitle7 << "Electron fractional energy in 3rd sampling" ;
    TH1* h7 = new TH1F( hName7.str().c_str(), hTitle7.str().c_str(), 50, -1., 1);
    sc = egamma_electron.regHist( h7 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName7.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h7 );

    std::ostringstream hName8  ;
    std::ostringstream hTitle8 ;
    hName8  << "el_ehad1" ;
    hTitle8 << "Electron energy leakage in 1st sampling of hadcal" ;
    TH1* h8 = new TH1F( hName8.str().c_str(), hTitle8.str().c_str(), 50, -1000., 10000.);
    sc = egamma_electron.regHist( h8 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName8.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h8 );

    std::ostringstream hName9  ;
    std::ostringstream hTitle9 ;
    hName9  << "el_Re233e237" ;
    hTitle9 << "Electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2" ;
    TH1* h9 = new TH1F( hName9.str().c_str(), hTitle9.str().c_str(), 50, -4., 4.);
    sc = egamma_electron.regHist( h9 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName9.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h9 );

    std::ostringstream hName10  ;
    std::ostringstream hTitle10 ;
    hName10  << "el_Re237e277" ;
    hTitle10 << "Electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2" ;
    TH1* h10 = new TH1F( hName10.str().c_str(), hTitle10.str().c_str(), 50, -4., 4. );
    sc = egamma_electron.regHist( h10 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName10.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h10 );

    //                 std::ostringstream hName11  ;
    //                 std::ostringstream hTitle11 ;
    //                 hName11  << "el_e277" ;
    //                 hTitle11 << "Electron uncor. energy in 7x7 cells in em sampling 2" ;
    //                 TH1* h11 = new TH1F( hName11.str().c_str(), hTitle11.str().c_str(), 50, -5000, 250000.);
    //                 egamma_electron.regHist( h11 );
    //                 m_el_histos.push_back( h11 );

    std::ostringstream hName12  ;
    std::ostringstream hTitle12 ;
    hName12  << "el_core" ;
    hTitle12 << "Electron core energy in em" ;
    TH1* h12 = new TH1F( hName12.str().c_str(), hTitle12.str().c_str(), 50, -5000., 250000.);
    sc = egamma_electron.regHist( h12 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName12.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h12 );

    //EMConvert variables / shift		
    std::ostringstream hName13  ;
    std::ostringstream hTitle13 ;
    hName13  << "el_convTrackMatch" ;
    hTitle13 << "Electron convTrackMatch flag" ; 
    TH1* h13 = new TH1F( hName13.str().c_str(), hTitle13.str().c_str(), 6, -3, -3);
    sc = egamma_electron.regHist( h13 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName13.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h13 );

    std::ostringstream hName14  ;
    std::ostringstream hTitle14 ;
    hName14  << "el_convAngleMatch" ;
    hTitle14 << "Electron convAnglMatch flag" ; 
    TH1* h14 = new TH1F( hName14.str().c_str(), hTitle14.str().c_str(), 6, -3, -3);
    sc = egamma_electron.regHist( h14 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName14.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h14 );

    // ID variables / shift
    std::ostringstream hName15  ;
    std::ostringstream hTitle15 ;
    hName15  << "el_nOfBLayerHits" ;
    hTitle15 << "Electron number of track B-Layer Hits" ; 
    TH1* h15 = new TH1F( hName15.str().c_str(), hTitle15.str().c_str(), 6, 0, 6);
    sc = egamma_electron.regHist( h15 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName15.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h15 );

    std::ostringstream hName16  ;
    std::ostringstream hTitle16 ;
    hName16  << "el_nOfPixelHits" ;
    hTitle16 << "Electron number of track pixel Hits" ; 
    TH1* h16 = new TH1F( hName16.str().c_str(), hTitle16.str().c_str(), 20, 0, 20);
    sc = egamma_electron.regHist( h16 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName16.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h16 );

    std::ostringstream hName17  ;
    std::ostringstream hTitle17 ;
    hName17  << "el_nOfTrackSiHits" ;
    hTitle17 << "Electron number of track precision Hits" ; 
    TH1* h17 = new TH1F( hName17.str().c_str(), hTitle17.str().c_str(), 25, 0, 25);
    sc = egamma_electron.regHist( h17 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName17.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h17 );

    std::ostringstream hName18  ;
    std::ostringstream hTitle18 ;
    hName18  << "el_nOfTRTHits" ;
    hTitle18 << "Electron number of track TRT Hits" ; 
    TH1* h18 = new TH1F( hName18.str().c_str(), hTitle18.str().c_str(), 45, 0, 45);
    sc = egamma_electron.regHist( h18 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName18.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h18 );

    std::ostringstream hName19  ;
    std::ostringstream hTitle19 ;
    hName19  << "el_EtVsEta" ;
    hTitle19 << "Electron Et vs. Eta" ; 
    TH2* h19 = new TH2F( hName19.str().c_str(), hTitle19.str().c_str(),
			 100, -2.5, 2.5,
			 100, 0, 150000
			 );
    sc = egamma_electron.regHist( h19 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName19.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h19 );

    std::ostringstream hName20  ;
    std::ostringstream hTitle20 ;
    hName20  << "el_EtVsPhi" ;
    hTitle20 << "Electron Et vs. Phi" ; 
    TH2* h20 = new TH2F( hName20.str().c_str(), hTitle20.str().c_str(),
			 100, -4., 4.,
			 100, 0, 150000
			 );
    sc = egamma_electron.regHist( h20 );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<hName20.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }

    m_el_histos.push_back( h20 );
  
    std::ostringstream hName21  ;
    std::ostringstream hTitle21 ;
    hName21  << "el_nOfHighTRTHits" ;
    hTitle21 << "Electron number of track high threshold TRT Hits" ; 
    TH1* h21 = new TH1F( hName21.str().c_str(), hTitle21.str().c_str(), 45, 0, 45);
    egamma_electron.regHist( h21 );
    m_el_histos.push_back( h21 );

  
    std::ostringstream el_IsEMHistoName ;
    std::ostringstream el_IsEMHistoTitle;
    el_IsEMHistoName << "el_IsEM" ;
    el_IsEMHistoTitle << "Electron IsEM" ;
    m_IsEMHisto = new TH1F( el_IsEMHistoName.str().c_str(), el_IsEMHistoTitle.str().c_str(), 3, 0, 3);
    sc = egamma_electron.regHist( m_IsEMHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_IsEMHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }


    std::ostringstream el_HighEtRangesHistoName ;
    std::ostringstream el_HighEtRangesHistoTitle;
    el_HighEtRangesHistoName << "el_HighEtRanges" ;
    el_HighEtRangesHistoTitle << "Electrons with Et > 100, 500 and 1000 GeV" ;
    m_el_HighEtRangesHisto = new TH1F( el_HighEtRangesHistoName.str().c_str(), el_HighEtRangesHistoTitle.str().c_str(), 3, 0, 3);
    sc = egamma_electron.regHist( m_el_HighEtRangesHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_HighEtRangesHistoName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }


    std::ostringstream el_ClusterPhiName ;
    std::ostringstream el_ClusterPhiTitle ;
    el_ClusterPhiName  << "el_ClusterPhi" ;
    el_ClusterPhiTitle << "Electron Cluster phi" ;
    m_el_ClusterPhiHisto = new TH1F( el_ClusterPhiName.str().c_str(), el_ClusterPhiTitle.str().c_str(), 50, -4., 4. );
    sc = egamma_electron.regHist( m_el_ClusterPhiHisto );
    if (sc == StatusCode::FAILURE){
      log <<MSG::FATAL <<"Failed to register "<<el_ClusterPhiName.str().c_str()<<endreq;
      return StatusCode::FAILURE;
    }




  }
  return StatusCode::SUCCESS;
}

// ===========================================================
//-REMI append _old
StatusCode
egammaMonTool::
bookHistograms_old( bool isNewEventsBlock, bool isNewLumiBlock, bool /* isNewRun */)
{
  MsgStream log( msgSvc(), name() );
  log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()"<<endreq;
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
      return sc;
    }

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }
	
  log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tDefining Monitoring Groups for electrons"<<endreq;
	
  //electron
  //-REMI MonGroup egamma_electron	(this, "egamma/Electron", 		shift, run );
  //-REMI MonGroup egamma_electron_medium (this, "egamma/Electron/Medium",        shift, run );
  //-REMI MonGroup egamma_electron_ene	(this, "egamma/Electron/Calo/Energy",   expert, run );
  //-REMI MonGroup egamma_electron_phi	(this, "egamma/Electron/Calo/Phi", 	expert, run );
  //-REMI MonGroup egamma_electron_eta	(this, "egamma/Electron/Calo/Eta", 	expert, run );
  //-REMI MonGroup egamma_electron_EoverP (this, "egamma/Electron/EoverP",expert, run);
  log <<MSG::DEBUG<<"egammaMonTool::bookHistograms()\tDefining Monitoring Groups for photons"<<endreq;

  //photon
  //-REMI MonGroup egamma_photon		(this, "egamma/Photon", 		shift,  run );
  //-REMI MonGroup egamma_photon_ene    	(this, "egamma/Photon/Calo/Energy",	expert, run );
  //-REMI MonGroup egamma_photon_phi    	(this, "egamma/Photon/Calo/Phi",   	expert, run );
  //-REMI MonGroup egamma_photon_eta    	(this, "egamma/Photon/Calo/Eta",   	expert, run );
  //-REMI MonGroup egamma_photon_tight   	(this, "egamma/Photon/Tight",   	expert, run );

  if( !m_trigDec.empty() && m_useTrigger )
    {
      sc = m_trigDec.retrieve();
      if( !sc.isSuccess() ) {
	log << MSG::ERROR << "!! Unable to retrieve the TrigDecisionTool !!" << endreq;
	return sc;
      }
      log << MSG::DEBUG << "  --> Found AlgTool \"TrigDecisionTool\"" << endreq;    
    }
  log << MSG::DEBUG << "Size of Electron Trigger vector = "<< m_el_triggers.size() << endreq;
  log << MSG::DEBUG << "Size of Photon Trigger vector = "<< m_ph_triggers.size() << endreq;

  /*-REMI
  if( isNewRun ) { 
    
    //////////////////////////////////////
    //				    //
    //	Electron Histograms	    //	
    //				    //
    //////////////////////////////////////	
    // WARNING
    // These clear() are important. If you don't clear all your vectors
    // of histos, when switching to a second input file it will segfault.
    m_el_histos.clear();	
    m_el_ene_histos.clear();
    m_el_eta_histos.clear();
    m_el_phi_histos.clear();

    sc = this->bookBaseElectronHistos(isNewEventsBlock, isNewLumiBlock, isNewRun, egamma_electron);
    if(sc != StatusCode::SUCCESS ) {
      log << "Error booking base electron histos."<<endreq;
      return StatusCode::FAILURE;
    }
    sc = this->bookMediumElectronHistos(isNewEventsBlock, isNewLumiBlock, isNewRun, egamma_electron_medium);
    if(sc != StatusCode::SUCCESS ) {
      log << "Error booking base electron histos."<<endreq;
      return StatusCode::FAILURE;
    }
    sc = this->bookCaloClusterElectronHistos(isNewEventsBlock, isNewLumiBlock, isNewRun,
					egamma_electron_ene, egamma_electron_eta,
					egamma_electron_phi);
    if(sc != StatusCode::SUCCESS ) {
      log << "Error booking base electron histos."<<endreq;
      return StatusCode::FAILURE;
    }
    sc = this->bookZmassHistos( isNewEventsBlock, isNewLumiBlock, isNewRun );
    if(sc != StatusCode::SUCCESS) {
      log << "Error booking z mass histos."<<endreq;
      return StatusCode::FAILURE;
    }

    if( m_environment == AthenaMonManager::tier0 || 
	m_environment == AthenaMonManager::tier0ESD ) {
     
      m_el_clus_meanEne = new TH1F("el_clus_meanEne","Mean energy of electron cluster in the differenet calo samples", 
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings);

      sc = egamma_electron.regHist( m_el_clus_meanEne );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<"el_clus_meanEne"<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_clus_meanEta = new TH1F("el_clus_meanEta","Mean eta of electron cluster in the differenet calo samples", 
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings);
      sc = egamma_electron.regHist( m_el_clus_meanEta );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<"el_clus_meanEta"<<endreq;
        return StatusCode::FAILURE;
      }

      m_el_clus_meanPhi = new TH1F("el_clus_meanPhi","Mean phi of electron cluster in the differenet calo samples", 
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings );
      sc = egamma_electron.regHist( m_el_clus_meanPhi );
      if (sc == StatusCode::FAILURE){
        log <<MSG::FATAL <<"Failed to register "<<"el_clus_meanPhi"<<endreq;
        return StatusCode::FAILURE;
      }

      sc = this->bookEoverPElectronHistos(isNewEventsBlock, isNewLumiBlock , isNewRun, 
					  egamma_electron_EoverP, egamma_electron_EoverP, 
					  egamma_electron_EoverP);
      if(sc != StatusCode::SUCCESS ) {
	log << "Error booking base electron histos."<<endreq;
	return StatusCode::FAILURE;
      }
      sc = this->bookPhotonHistos(isNewEventsBlock, isNewLumiBlock, isNewRun,
				  egamma_photon, egamma_photon_ene,  
				  egamma_photon_eta, egamma_photon_phi, egamma_photon_tight );
      if(sc != StatusCode::SUCCESS ) {
	log << "Error booking base electron histos."<<endreq;
	return StatusCode::FAILURE;
      }
      m_ph_clus_meanEne = new TH1F("ph_clus_meanEne","Mean energy of photon cluster in the differenet calo samples",
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings);
      sc = egamma_photon.regHist( m_ph_clus_meanEne );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<"ph_clus_meanEne"<<endreq;
	return StatusCode::FAILURE;
      }

      m_ph_clus_meanEta = new TH1F("ph_clus_meanEta","Mean eta of photon cluster in the differenet calo samples",
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings);
      sc = egamma_photon.regHist( m_ph_clus_meanEta );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<"ph_clus_meanEta"<<endreq;
	return StatusCode::FAILURE;
      }


      m_ph_clus_meanPhi = new TH1F("ph_clus_meanPhi","Mean phi of photon cluster in the differenet calo samples",
				   m_nOfCaloSamplings, 0., m_nOfCaloSamplings );
      sc = egamma_photon.regHist( m_ph_clus_meanPhi );
      if (sc == StatusCode::FAILURE){
	log <<MSG::FATAL <<"Failed to register "<<"ph_clus_meanPhi"<<endreq;
	return StatusCode::FAILURE;
      }

    }
  } 
*/	
  if( isNewEventsBlock || isNewLumiBlock ) { }

  
  return StatusCode( StatusCode::SUCCESS );
}

// ===========================================================
StatusCode
egammaMonTool::
fillHistograms()
{
  MsgStream log( msgSvc(), name() );

//check whether Lar signalled event bad
//if (this->hasBadLar() && m_checkLar) return StatusCode::SUCCESS;


  // ------  Monitor electrons  -------
  const egammaContainer* electron_container;
  StatusCode sc = m_storeGate->retrieve(electron_container, m_ElectronCnt);
  if(sc.isFailure()  ||  !electron_container)
    {
      log << MSG::DEBUG 
	  << "no electron container found in TDS"
	  << endreq; 
      return sc;
    }

  // Check if passes electron triggers
  bool useTrigger = ( m_el_triggers.size() != 0 ) && ( ! m_trigDec.empty() );
  bool passesTrigger = false;
  if( useTrigger )
    {
      for (unsigned int ii = 0; ii < m_el_triggers.size(); ii++ )
	{
	  // 		log << MSG::WARNING<<"Checking trigger "<<m_triggers[ii]<<endreq;
	  if( m_trigDec->isPassed(m_el_triggers[ii]))
	    {
	      passesTrigger = true;
	      // 		    log << MSG::WARNING<<"Fired trigger "<<m_triggers[ii]<<endreq;
	      break;
	    }
	}
    }
  if(!m_useTrigger) passesTrigger = true;
  if(passesTrigger)
    {
      // Do electron histos filling

      egammaContainer::const_iterator eg_iter    = electron_container->begin();
      egammaContainer::const_iterator eg_iterEnd = electron_container->end();

      int highestClusterEtElectronIndex = -1;
      int secondHighestClusterEtElectronIndex = -1;
      float highestClusterEt = -999;
      float secondHighestClusterEt = -999;
      int containerPosition = -1;
      for (; eg_iter!=eg_iterEnd; eg_iter++)  
	{
	  m_h_index = 0;
	  containerPosition++;
	  const CaloCluster * clus = (*eg_iter)->cluster();
	  if(clus)
	    {

	      if( m_environment == AthenaMonManager::tier0 || 
		  m_environment == AthenaMonManager::tier0ESD ) {
		if(m_el_histos[0])        m_el_histos[0]->Fill(clus->et());               m_h_index++;
		if(m_el_histos[1])        m_el_histos[1]->Fill(clus->eta());      m_h_index++;
	      }
	      if(highestClusterEt < clus->et()) {
		// make current highest be new 2nd highest     
		secondHighestClusterEtElectronIndex = highestClusterEtElectronIndex;
		secondHighestClusterEt = highestClusterEt;
		// keep new highest
		highestClusterEt = clus->et();
		highestClusterEtElectronIndex = containerPosition;
		log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tGot leading cluster with"
		    <<" Et: "<<highestClusterEt<<" index: "<<highestClusterEtElectronIndex<<endreq;
	      }
	      else {
		if(secondHighestClusterEt < clus->et() ) {
		  secondHighestClusterEt = clus->et();
		  secondHighestClusterEtElectronIndex = containerPosition;
		  log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tGot second leading cluster"
		      <<" Et: "<<secondHighestClusterEt<<" index: "<<secondHighestClusterEtElectronIndex<<endreq;
		  
		}
	      }
	      float et = clus->et();
	      float eta = clus->eta();
	      float phi = clus->phi();
	      float e = et*cosh(eta);
	      float f2 = clus->energyBE(2) / e;
	      if( m_environment == AthenaMonManager::tier0 || m_environment ==
		  AthenaMonManager::tier0ESD ) {
		m_el_F2Histo->Fill(f2);
		m_el_ClusterPhiHisto->Fill(phi);
	      }
	      int HighEtRange=-1;
	      if( et > 100000 ) HighEtRange=0;
	      if( et > 500000 ) HighEtRange=1;
	      if( et > 1000000 ) HighEtRange=2;
	      if( HighEtRange != -1 ) {
		if( m_environment == AthenaMonManager::tier0 ||
		    m_environment ==  AthenaMonManager::tier0ESD ) {
		  // book or fill histogram that can be made either on Raw or ESD
		  m_el_HighEtRangesHisto->Fill(HighEtRange);
		}
	      }
	    }

	  std::string ParticleName = "Electron" ;
	  fillEMTrackMatchHistos	((*eg_iter), ParticleName);
	  fillEMShowerHistos	((*eg_iter), ParticleName);
	  fillEMConvertHistos	((*eg_iter), ParticleName);
	  fillCaloClusterHistos	((*eg_iter), ParticleName);
	  fillIDHistos	((*eg_iter), ParticleName);
	  fillRegionHistos ( (*eg_iter), ParticleName );
	  fillIsEMHisto( (*eg_iter) );
	  fillIsEMMediumCutHistos( (*eg_iter), ParticleName);
	  fillEoverPHistos( (*eg_iter) );
	} // end for over electronItr


      if(highestClusterEtElectronIndex != -1 &&
	 secondHighestClusterEtElectronIndex != -1) {
	log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tHave two high energy clusters"<<endreq;

	const egamma * highestClusterEtElectron = (*electron_container)[highestClusterEtElectronIndex];
	const egamma * secondHighestClusterEtElectron = (*electron_container)[secondHighestClusterEtElectronIndex];
	
	double Zmass =  this->InvMass(highestClusterEtElectron->cluster(), 
				     secondHighestClusterEtElectron->cluster() );

	log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tClusters invariant mass: "
	    <<Zmass<<" mass cuts: lower: "<<m_ZmassLowerCut<<" upper: "<<m_ZmassUpperCut
	    <<" Et1: "<<highestClusterEtElectron->cluster()->et()<<" cut1: "<<m_ElectronClusterEtCut
	    <<" Et2: "<<secondHighestClusterEtElectron->cluster()->et()<<" cut2: "<<m_ElectronClusterEtCut<<endreq;

	if(highestClusterEtElectron->cluster()->et() > m_ElectronClusterEtCut &&
	   secondHighestClusterEtElectron->cluster()->et() > m_ElectronClusterEtCut &&
	   Zmass > m_ZmassLowerCut && Zmass < m_ZmassUpperCut ) {
	  log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tClusters and Invariant mass pass"<<endreq;
	  // Fill Et, Eta, Phi, Et vs Eta, Et vs Phi histos
	  m_z_ClusterEt->Fill(highestClusterEtElectron->cluster()->et());
	  m_z_ClusterEt->Fill(secondHighestClusterEtElectron->cluster()->et());

	  m_z_ClusterEta->Fill(highestClusterEtElectron->cluster()->eta());
	  m_z_ClusterEta->Fill(secondHighestClusterEtElectron->cluster()->eta());

	  m_z_ClusterPhi->Fill(highestClusterEtElectron->cluster()->phi());
	  m_z_ClusterPhi->Fill(secondHighestClusterEtElectron->cluster()->phi());

	  m_z_ClusterEtvsEta->Fill(highestClusterEtElectron->cluster()->eta(),
				   highestClusterEtElectron->cluster()->et() );
	  m_z_ClusterEtvsEta->Fill(secondHighestClusterEtElectron->cluster()->eta(), 
				   secondHighestClusterEtElectron->cluster()->et() );

	  m_z_ClusterEtvsPhi->Fill(highestClusterEtElectron->cluster()->phi(),
				   highestClusterEtElectron->cluster()->et() );
	  m_z_ClusterEtvsPhi->Fill(secondHighestClusterEtElectron->cluster()->phi(), 
				   secondHighestClusterEtElectron->cluster()->et() );


	  const Rec::TrackParticle * highestClusterEtTrack = 
	    highestClusterEtElectron->trackParticle();
	  const Rec::TrackParticle * secondHighestClusterEtTrack = 
	    secondHighestClusterEtElectron->trackParticle();

	  if(highestClusterEtTrack && secondHighestClusterEtTrack) {
	    // Fill Pt, Eta,, Phi, Pt vs Eta, Pt vs Phi histos
	    log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tClusters have tracks"<<endreq;
	    m_z_TrackPt->Fill(highestClusterEtTrack->pt());
	    m_z_TrackPt->Fill(secondHighestClusterEtTrack->pt());
	    
	    m_z_TrackEta->Fill(highestClusterEtTrack->eta());
	    m_z_TrackEta->Fill(secondHighestClusterEtTrack->eta());
	    
	    m_z_TrackPhi->Fill(highestClusterEtTrack->phi());
	    m_z_TrackPhi->Fill(secondHighestClusterEtTrack->phi());
	    
	    m_z_TrackPtvsEta->Fill(highestClusterEtTrack->eta(),
				   highestClusterEtTrack->pt() );
	    m_z_TrackPtvsEta->Fill(secondHighestClusterEtTrack->eta(), 
				   secondHighestClusterEtTrack->pt() );
	    
	    m_z_TrackPtvsPhi->Fill(highestClusterEtTrack->phi(),
				   highestClusterEtTrack->pt() );
	    m_z_TrackPtvsPhi->Fill(secondHighestClusterEtTrack->phi(), 
				   secondHighestClusterEtTrack->pt() );  

	    // Fill Et, Eta,, Phi, Et vs Eta, Et vs Phi histos for cluster with track
	    m_z_ClusterWithTrackEt->Fill(highestClusterEtElectron->cluster()->et());
	    m_z_ClusterWithTrackEt->Fill(secondHighestClusterEtElectron->cluster()->et());
	    
	    m_z_ClusterWithTrackEta->Fill(highestClusterEtElectron->cluster()->eta());
	    m_z_ClusterWithTrackEta->Fill(secondHighestClusterEtElectron->cluster()->eta());
	    
	    m_z_ClusterWithTrackPhi->Fill(highestClusterEtElectron->cluster()->phi());
	    m_z_ClusterWithTrackPhi->Fill(secondHighestClusterEtElectron->cluster()->phi());
	    
	    m_z_ClusterWithTrackEtvsEta->Fill(highestClusterEtElectron->cluster()->eta(),
				   highestClusterEtElectron->cluster()->et() );
	    m_z_ClusterWithTrackEtvsEta->Fill(secondHighestClusterEtElectron->cluster()->eta(), 
				   secondHighestClusterEtElectron->cluster()->et() );
	    
	    m_z_ClusterWithTrackEtvsPhi->Fill(highestClusterEtElectron->cluster()->phi(),
				   highestClusterEtElectron->cluster()->et() );
	    m_z_ClusterWithTrackEtvsPhi->Fill(secondHighestClusterEtElectron->cluster()->phi(), 
				   secondHighestClusterEtElectron->cluster()->et() );  


	  }
	  
	  // If one passes Loose, the other one passes Tight
	  if( (this->passesIsEMCut(egammaPID::ElectronTightPP,
				   const_cast<egamma*> (highestClusterEtElectron)) &&
	       this->passesIsEMCut(egammaPID::ElectronLoosePP,
				   const_cast<egamma*> (secondHighestClusterEtElectron)) ) ||
	      (this->passesIsEMCut(egammaPID::ElectronTightPP,
				   const_cast<egamma*> (secondHighestClusterEtElectron)) &&
	       this->passesIsEMCut(egammaPID::ElectronLoosePP,
				   const_cast<egamma*> (highestClusterEtElectron)) ) ) {
	    // Fill Z mass histo
            Zmass-=91188; //PDG value for test
	    log << MSG::DEBUG <<"egammaMonTool::fillHistograms()\tCandidates pass ID cuts"<<endreq;
	    m_z_mass->Fill(Zmass);

	    m_z_CutClusterEt->Fill(highestClusterEtElectron->cluster()->et());
	    m_z_CutClusterEta->Fill(highestClusterEtElectron->cluster()->eta());
	    m_z_CutClusterPhi->Fill(highestClusterEtElectron->cluster()->phi());

	    m_z_CutClusterEt->Fill(secondHighestClusterEtElectron->cluster()->et());
	    m_z_CutClusterEta->Fill(secondHighestClusterEtElectron->cluster()->eta());
	    m_z_CutClusterPhi->Fill(secondHighestClusterEtElectron->cluster()->phi());


	    if(this->inBarrel(highestClusterEtElectron->cluster()) &&
	       this->inBarrel(secondHighestClusterEtElectron->cluster() )) {
	      // Fill both on barrel Z mass histo
	      m_z_mass_bb->Fill(Zmass);
	    }
	    if(this->inEndcap(highestClusterEtElectron->cluster()) &&
	       this->inEndcap(secondHighestClusterEtElectron->cluster() )) {
	      // Fill both on endcap Z mass histo
	      m_z_mass_ee->Fill(Zmass);
	    }
	    if( ( this->inBarrel(highestClusterEtElectron->cluster()) &&
		  this->inEndcap(secondHighestClusterEtElectron->cluster()) ) ||
		( this->inEndcap(highestClusterEtElectron->cluster()) &&
		  this->inBarrel(secondHighestClusterEtElectron->cluster()) ) )  {
		// Fill one on endcap, one on barrel Z mass histo
	      m_z_mass_be->Fill(Zmass);
	      }
	    if( ( this->inBarrel(highestClusterEtElectron->cluster()) &&
		  this->inCrack(secondHighestClusterEtElectron->cluster()) ) ||
		( this->inCrack(highestClusterEtElectron->cluster()) &&
		  this->inBarrel(secondHighestClusterEtElectron->cluster()) ) )  {
		// Fill one on crack, one on barrel Z mass histo
	      m_z_mass_bc->Fill(Zmass);
	      }
	    if( ( this->inCrack(highestClusterEtElectron->cluster()) &&
		  this->inEndcap(secondHighestClusterEtElectron->cluster()) ) ||
		( this->inEndcap(highestClusterEtElectron->cluster()) &&
		  this->inCrack(secondHighestClusterEtElectron->cluster()) ) )  {
		// Fill one on endcap, one on crack Z mass histo
	      m_z_mass_ec->Fill(Zmass);
	      }
	  }

	}
	   
      }
    } //  end if passestrigger



  // Check if passes photon triggers
  useTrigger = ( m_ph_triggers.size() != 0 ) && ( ! m_trigDec.empty() );
  passesTrigger = false;
  if( useTrigger )
    {
      for (unsigned int ii = 0; ii < m_ph_triggers.size(); ii++ )
	{
	  // 		log << MSG::WARNING<<"Checking trigger "<<m_triggers[ii]<<endreq;
	  if( m_trigDec->isPassed(m_ph_triggers[ii]))
	    {
	      passesTrigger = true;
	      // 		    log << MSG::WARNING<<"Fired trigger "<<m_triggers[ii]<<endreq;
	      break;
	    }
	}
    }
  if(!m_useTrigger) passesTrigger = true;
  if(passesTrigger)
    {
      // Do photon histos filling
      const egammaContainer* photon_container;
      sc = m_storeGate->retrieve(photon_container, m_PhotonCnt);
      if(sc.isFailure()  ||  !photon_container)
	{
	  log << MSG::DEBUG
	      << "no photon container found in TDS"
	      << endreq;
	  return sc;
	}

      egammaContainer::const_iterator eg_iter    = photon_container->begin();
      egammaContainer::const_iterator eg_iterEnd = photon_container->end();

      for (; eg_iter!=eg_iterEnd; eg_iter++)  
	{
	  m_h_index = 0;

	  const CaloCluster * clus = (*eg_iter)->cluster();
	  if(clus)
	    {
	      if( m_environment == AthenaMonManager::tier0 || m_environment ==
		  AthenaMonManager::tier0ESD ) {
		if(m_ph_histos[0])        m_ph_histos[0]->Fill(clus->et());               m_h_index++;
		if(m_ph_histos[1])        m_ph_histos[1]->Fill(clus->eta());      m_h_index++;
	      }
	      float et = clus->et();
	      float eta = clus->eta();
	      float phi = clus->phi();
	      float e = et*cosh(eta);
	      float f2 = clus->energyBE(2) / e;
	      if( m_environment == AthenaMonManager::tier0 || m_environment ==
		  AthenaMonManager::tier0ESD ) {
		// book or fill histogram that can be made either on Raw or ESD
		m_ph_F2Histo->Fill(f2);
		m_ph_ClusterPhiHisto->Fill(phi); 
	      }
	    }

	  std::string ParticleName = "Photon" ;				
	  fillEMTrackMatchHistos  ((*eg_iter), ParticleName);
	  fillEMShowerHistos      ((*eg_iter), ParticleName);
	  //	  fillEMConvertHistos	((*eg_iter), ParticleName);
	  fillCaloClusterHistos   ((*eg_iter), ParticleName);
	  //		fillIDHistos	((*eg_iter), ParticleName);
	  fillRegionHistos ( (*eg_iter), ParticleName );
	  fillIsEMTightCutPhotonHistos((*eg_iter), ParticleName);
	}
    } //  end if passestrigger

  return StatusCode( StatusCode::SUCCESS );
}

// ===========================================================
double egammaMonTool::InvMass(const CaloCluster* EM1, const CaloCluster* EM2) 
{
  //
  //
  //

  if (EM1 == 0 || EM2 == 0) return -99.;

  double invmass = 0.;
  if (EM1->pt() != 0 && EM2->pt() != 0.) {
    TLorentzVector particle1;
    TLorentzVector particle2;
    particle1.SetPtEtaPhiE(EM1->pt(),EM1->eta(),EM1->phi(),EM1->e());
    particle2.SetPtEtaPhiE(EM2->pt(),EM2->eta(),EM2->phi(),EM2->e());
    invmass = (particle1+particle2).Mag();
  }
  return invmass;
}

// ===========================================================
void egammaMonTool::makeEffHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff) 
{
  //
  //
  //

  int Nbins = h_num->GetNbinsX();
  for (int bin=0; bin!=Nbins; ++bin) {
    int Npass = int(h_num->GetBinContent(bin+1));
    int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;
    double x = h_denom->GetBinCenter(bin+1);
    for (int pass=0; pass!=Npass; ++pass) {
      h_eff->Fill(x,1.);
    }
    for (int fail=0; fail!=Nfail; ++fail) {
      h_eff->Fill(x,0.);
    }
  }
}

// ===========================================================
//-REMI append _old
StatusCode
egammaMonTool::
procHistograms_old( bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool isEndOfRun )
{
  MsgStream log( msgSvc(), name() );
  log<<MSG::DEBUG<<"egammaMonTool::procHistograms()"<<endreq;

  if( m_environment == AthenaMonManager::tier0 || 
      m_environment == AthenaMonManager::tier0ESD ) {
    //Fill histograms with average+-error values for electron & photon 
    //energy, eta and phi in each CaloSampling
    for( int i = 0; i < m_nOfCaloSamplings; i++ ) {	
      //Electron energy, eta & phi averaged in each CaloSampling
      if(m_el_clus_meanEne)      m_el_clus_meanEne->SetBinContent(	i+1, m_el_ene_histos[i]->GetMean() 	);
      if(m_el_clus_meanEne)      m_el_clus_meanEne->SetBinError(	i+1, m_el_ene_histos[i]->GetMeanError() );
      if(m_el_clus_meanEta)      m_el_clus_meanEta->SetBinContent(	i+1, m_el_eta_histos[i]->GetMean() 	);
      if(m_el_clus_meanEta)      m_el_clus_meanEta->SetBinError( 	i+1, m_el_eta_histos[i]->GetMeanError() );
      if(m_el_clus_meanPhi)      m_el_clus_meanPhi->SetBinContent(	i+1, m_el_phi_histos[i]->GetMean() 	);
      if(m_el_clus_meanPhi)      m_el_clus_meanPhi->SetBinError( 	i+1, m_el_phi_histos[i]->GetMeanError() );

      //Electron energy, eta & phi averaged in each CaloSampling
      if(m_ph_clus_meanEne)      m_ph_clus_meanEne->SetBinContent(    i+1, m_ph_ene_histos[i]->GetMean()      );
      if(m_ph_clus_meanEne)      m_ph_clus_meanEne->SetBinError(      i+1, m_ph_ene_histos[i]->GetMeanError() );
      if(m_ph_clus_meanEta)      m_ph_clus_meanEta->SetBinContent(    i+1, m_ph_eta_histos[i]->GetMean()      );
      if(m_ph_clus_meanEta)      m_ph_clus_meanEta->SetBinError(      i+1, m_ph_eta_histos[i]->GetMeanError() );
      if(m_ph_clus_meanPhi)      m_ph_clus_meanPhi->SetBinContent(    i+1, m_ph_phi_histos[i]->GetMean()      );
      if(m_ph_clus_meanPhi)      m_ph_clus_meanPhi->SetBinError(      i+1, m_ph_phi_histos[i]->GetMeanError() );
    }

    /*
    // Normalize Histograms
    for(unsigned int ii = 0; ii < m_el_histos.size(); ii++)
      {
	if( (int) m_el_histos[ii]->GetEntries() != 0 )
	  m_el_histos[ii]->Scale( 1. / (m_el_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_el_ene_histos.size(); ii++)
      {
	if( (int) m_el_ene_histos[ii]->GetEntries() != 0 )
	  m_el_ene_histos[ii]->Scale( 1. / (m_el_ene_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_el_eta_histos.size(); ii++)
      {
	if( (int) m_el_eta_histos[ii]->GetEntries() != 0 )	    
	  m_el_eta_histos[ii]->Scale( 1. / (m_el_eta_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_el_phi_histos.size(); ii++)
      {
	if( (int) m_el_phi_histos[ii]->GetEntries() != 0 )
	  m_el_phi_histos[ii]->Scale( 1. / (m_el_phi_histos[ii]->GetEntries()) );
      }
    if( (int) m_el_clus_meanEne->GetEntries() != 0 )
      m_el_clus_meanEne->Scale( 1. / m_el_clus_meanEne->GetEntries() );
    if( (int) m_el_clus_meanEta->GetEntries() != 0 )
      m_el_clus_meanEta->Scale( 1. / m_el_clus_meanEta->GetEntries() );
    if( (int) m_el_clus_meanPhi->GetEntries() != 0 )
      m_el_clus_meanPhi->Scale( 1. / m_el_clus_meanPhi->GetEntries() );

    for(unsigned int ii = 0; ii < m_ph_histos.size(); ii++)
      {
	if( (int) m_ph_histos[ii]->GetEntries() != 0 )
	  m_ph_histos[ii]->Scale( 1. / (m_ph_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_ph_ene_histos.size(); ii++)
      {
	if( (int) m_ph_ene_histos[ii]->GetEntries() != 0 )
	  m_ph_ene_histos[ii]->Scale( 1. / (m_ph_ene_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_ph_eta_histos.size(); ii++)
      {
	if( (int) m_ph_eta_histos[ii]->GetEntries() != 0 )
	  m_ph_eta_histos[ii]->Scale( 1. / (m_ph_eta_histos[ii]->GetEntries()) );
      }
    for(unsigned int ii = 0; ii < m_ph_phi_histos.size(); ii++)
      {
	if( (int) m_ph_phi_histos[ii]->GetEntries() != 0 )
	  m_ph_phi_histos[ii]->Scale( 1. / (m_ph_phi_histos[ii]->GetEntries()) );
      }
    if( (int) m_ph_clus_meanEne->GetEntries() != 0 )
      m_ph_clus_meanEne->Scale( 1. / m_ph_clus_meanEne->GetEntries() );
    if( (int) m_ph_clus_meanEta->GetEntries() != 0 )
      m_ph_clus_meanEta->Scale( 1. / m_ph_clus_meanEta->GetEntries() );
    if( (int) m_ph_clus_meanPhi->GetEntries() != 0 )
      m_ph_clus_meanPhi->Scale( 1. / m_ph_clus_meanPhi->GetEntries() );
	
    if( (int) m_IsEMHisto->GetEntries() != 0 )
      m_IsEMHisto->Scale( 1. / (int)m_IsEMHisto->GetEntries() );

    if( (int) m_el_F2Histo->GetEntries() != 0 )
      m_el_F2Histo->Scale( 1. / (int)m_el_F2Histo->GetEntries() );

    if( (int) m_ph_F2Histo->GetEntries() != 0 )
      m_ph_F2Histo->Scale( 1. / (int)m_ph_F2Histo->GetEntries() );

    if( (int) m_el_HighEtRangesHisto->GetEntries() != 0 )
      m_el_HighEtRangesHisto->Scale( 1. / (int)m_el_HighEtRangesHisto->GetEntries() );

    if( (int) m_el_ClusterPhiHisto->GetEntries() != 0 )
      m_el_ClusterPhiHisto->Scale( 1. / (int)m_el_ClusterPhiHisto->GetEntries() );

    if( (int) m_ph_ClusterPhiHisto->GetEntries() != 0 )
      m_ph_ClusterPhiHisto->Scale( 1. / (int)m_ph_ClusterPhiHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_ClusterEtHisto->GetEntries() != 0 )
      m_el_IsEMMedium_ClusterEtHisto->Scale( 1. / (int)m_el_IsEMMedium_ClusterEtHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_ClusterEtaHisto->GetEntries() != 0 )
      m_el_IsEMMedium_ClusterEtaHisto->Scale( 1. / (int)m_el_IsEMMedium_ClusterEtaHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_ClusterPhiHisto->GetEntries() != 0 )
      m_el_IsEMMedium_ClusterPhiHisto->Scale( 1. / (int)m_el_IsEMMedium_ClusterPhiHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_EtvsEtaHisto->GetEntries() != 0 )
      m_el_IsEMMedium_EtvsEtaHisto->Scale( 1. / (int)m_el_IsEMMedium_EtvsEtaHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_EtvsPhiHisto->GetEntries() != 0 )
      m_el_IsEMMedium_EtvsPhiHisto->Scale( 1. / (int)m_el_IsEMMedium_EtvsPhiHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_EoverPHisto->GetEntries() != 0 )
      m_el_IsEMMedium_EoverPHisto->Scale( 1. / (int)m_el_IsEMMedium_EoverPHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_DeltaEta1Histo->GetEntries() != 0 )
      m_el_IsEMMedium_DeltaEta1Histo->Scale( 1. / (int)m_el_IsEMMedium_DeltaEta1Histo->GetEntries() );

    if( (int) m_el_IsEMMedium_DeltaPhi2Histo->GetEntries() != 0 )
      m_el_IsEMMedium_DeltaPhi2Histo->Scale( 1. / (int)m_el_IsEMMedium_DeltaPhi2Histo->GetEntries() );

    if( (int)  m_el_IsEMMedium_EthadHisto->GetEntries() != 0 )
      m_el_IsEMMedium_EthadHisto->Scale( 1. / (int)m_el_IsEMMedium_EthadHisto->GetEntries() );

    if( (int) m_el_IsEMMedium_F1Histo->GetEntries() != 0 )
      m_el_IsEMMedium_F1Histo->Scale( 1. / (int)m_el_IsEMMedium_F1Histo->GetEntries() );

    if( (int)  m_el_IsEMMedium_F2Histo->GetEntries() != 0 )
      m_el_IsEMMedium_F2Histo->Scale( 1. / (int)m_el_IsEMMedium_F2Histo->GetEntries() );

    if( (int) m_el_IsEMMedium_F3Histo->GetEntries() != 0 )
      m_el_IsEMMedium_F3Histo->Scale( 1. / (int)m_el_IsEMMedium_F3Histo->GetEntries() );

    if( (int) m_el_IsEMMedium_rE233E237Histo->GetEntries() != 0 )
      m_el_IsEMMedium_rE233E237Histo->Scale( 1. / (int)m_el_IsEMMedium_rE233E237Histo->GetEntries() );

    if( (int) m_el_IsEMMedium_rE237E277Histo->GetEntries() != 0 )
      m_el_IsEMMedium_rE237E277Histo->Scale( 1. / (int)m_el_IsEMMedium_rE237E277Histo->GetEntries() );

    if( (int) m_el_IsEMMedium_HighEtRangesHisto->GetEntries() != 0 )
    m_el_IsEMMedium_HighEtRangesHisto->Scale( 1. / (int)m_el_IsEMMedium_HighEtRangesHisto->GetEntries() );


    int entries;
    // E/P studies
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P studies' histos"<<endreq;
    // E/p vs eta
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Electron Et vs eta"<<endreq;
    entries = (int) m_el_Electron_EoverPvsEta->GetEntries();
    if( entries != 0 )
      {
	m_el_Electron_EoverPvsEta->Scale(1./entries);
      }
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Positron Et vs eta"<<endreq;
    entries = (int) m_el_Positron_EoverPvsEta->GetEntries();
    if( entries != 0 )
      {
	m_el_Positron_EoverPvsEta->Scale(1./entries);
      }
    // E/p vs eta (tight)
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Tight Electron Et vs eta"<<endreq;
    entries = (int) m_el_TightElectron_EoverPvsEta->GetEntries();
    if( entries != 0 )
      {
	m_el_TightElectron_EoverPvsEta->Scale(1./entries);
      }
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Tight Positron Et vs eta"<<endreq;
    entries = (int) m_el_TightPositron_EoverPvsEta->GetEntries();
    if( entries != 0 )
      {
	m_el_TightPositron_EoverPvsEta->Scale(1./entries);
      }
    // E/p vs phi
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Electron Et vs phi"<<endreq;
    entries = (int) m_el_Electron_EoverPvsPhi->GetEntries();
    if( entries != 0 )
      {
	m_el_Electron_EoverPvsPhi->Scale(1./entries);
      }
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Positron Et vs phi"<<endreq;
    entries = (int) m_el_Positron_EoverPvsPhi->GetEntries();
    if( entries != 0 )
      {
	m_el_Positron_EoverPvsPhi->Scale(1./entries);
      }
    // E/p vs phi (tight)
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Tight Electron Et vs phi"<<endreq;
    entries = (int) m_el_TightElectron_EoverPvsPhi->GetEntries();
    if( entries != 0 )
      {
	m_el_TightElectron_EoverPvsPhi->Scale(1./entries);
      }
    log<<MSG::DEBUG<<"egammaMonTool::procHistograms()\tProcessing E/P Tight Positron Et vs phi"<<endreq;
    entries = (int) m_el_TightPositron_EoverPvsPhi->GetEntries();
    if( entries != 0 )
      {
	m_el_TightPositron_EoverPvsPhi->Scale(1./entries);
      }
   
    // E/p (Electrons)
    entries = (int) m_el_Electron_EoverP->GetEntries();
    if(entries != 0)
      m_el_Electron_EoverP->Scale(1./entries);

    // E/p (Positrons)
    entries = (int) m_el_Positron_EoverP->GetEntries();
    if(entries != 0)
      m_el_Positron_EoverP->Scale(1./entries);


    // Photons

    entries = (int) m_ph_IsEMTight_ClusterEtHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_ClusterEtHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_ClusterEtaHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_ClusterEtaHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_ClusterPhiHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_ClusterPhiHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_F1Histo->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_F1Histo->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_F2Histo->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_F2Histo->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_F3Histo->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_F3Histo->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_EtHadHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_EtHadHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_rE233E237Histo->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_rE233E237Histo->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_rE237E277Histo->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_rE237E277Histo->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_ECoreHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_ECoreHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_convTrkMatchHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_convTrkMatchHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_convAngleMatchHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_convAngleMatchHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_EtvsEtaHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_EtvsEtaHisto->Scale(1./entries);
      }
    entries = (int) m_ph_IsEMTight_EtvsPhiHisto->GetEntries();
    if( entries != 0 )
      {
	m_ph_IsEMTight_EtvsPhiHisto->Scale(1./entries);
      }
    // Z mass related histos
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tProcessing Z mass cluster related histos"
       <<endreq;log.flush();
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_ClusterEt"
       <<endreq;log.flush();    
    entries = (int) m_z_ClusterEt->GetEntries();
    if(entries != 0) 
      m_z_ClusterEt->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_ClusterEta"
       <<endreq;log.flush();    
    entries = (int) m_z_ClusterEta->GetEntries();
    if(entries != 0) 
      m_z_ClusterEta->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_ClusterPhi"
       <<endreq;log.flush();    
    entries = (int) m_z_ClusterPhi->GetEntries();
    if(entries != 0) 
      m_z_ClusterPhi->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_ClusterEtvsEta"
       <<endreq;log.flush();    
    entries = (int) m_z_ClusterEtvsEta->GetEntries();
    if(entries != 0) 
      m_z_ClusterEtvsEta->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_ClusterEtvsPhi"
       <<endreq;log.flush();    
    entries = (int) m_z_ClusterEtvsPhi->GetEntries();
    if(entries != 0) 
      m_z_ClusterEtvsPhi->Scale(1./entries);

    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tProcessing Z mass track related histos"
       <<endreq;log.flush();
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_TrackPt"
       <<endreq;log.flush();    
    entries = (int) m_z_TrackPt->GetEntries();
    if(entries != 0) 
      m_z_TrackPt->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_TrackEta"
       <<endreq;log.flush();    
    entries = (int) m_z_TrackEta->GetEntries();
    if(entries != 0) 
      m_z_TrackEta->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_TrackPhi"
       <<endreq;log.flush();    
    entries = (int) m_z_TrackPhi->GetEntries();
    if(entries != 0) 
      m_z_TrackPhi->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_TrackPtvsEta"
       <<endreq;log.flush();    
    entries = (int) m_z_TrackPtvsEta->GetEntries();
    if(entries != 0) 
      m_z_TrackPtvsEta->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_TrackPtvsPhi"
       <<endreq;log.flush();    
    entries = (int) m_z_TrackPtvsPhi->GetEntries();
    if(entries != 0) 
      m_z_TrackPtvsPhi->Scale(1./entries);

    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tProcessing Z mass cluster related histos (with ID)"
       <<endreq;log.flush();
    entries = (int) m_z_CutClusterEt->GetEntries();
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_CutClusterEt"
       <<endreq;log.flush();    
    if(entries != 0 )
      m_z_CutClusterEt->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_CutClusterEta"
       <<endreq;log.flush();    
    entries = (int) m_z_CutClusterEta->GetEntries();
    if(entries != 0 )
      m_z_CutClusterEta->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_CutClusterPhi"
       <<endreq;log.flush();    
    entries = (int) m_z_CutClusterPhi->GetEntries();
    if(entries != 0 )
      m_z_CutClusterPhi->Scale(1./entries);

    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tProcessing Z mass histos"
       <<endreq;log.flush();
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass"
       <<endreq;log.flush();    
    entries = (int) m_z_mass->GetEntries();
    if(entries != 0)
      m_z_mass->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass_bb"
       <<endreq;log.flush();    
    entries = (int) m_z_mass_bb->GetEntries();
    if(entries != 0)
      m_z_mass_bb->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass_be"
       <<endreq;log.flush();    
    entries = (int) m_z_mass_be->GetEntries();
    if(entries != 0)
      m_z_mass_be->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass_bc"
       <<endreq;log.flush();    
    entries = (int) m_z_mass_bc->GetEntries();
    if(entries != 0)
      m_z_mass_bc->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass_ec"
       <<endreq;log.flush();    
    entries = (int) m_z_mass_ec->GetEntries();
    if(entries != 0)
      m_z_mass_ec->Scale(1./entries);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tm_z_mass_ee"
       <<endreq;log.flush();    
    entries = (int) m_z_mass_ee->GetEntries();
    if(entries != 0)
      m_z_mass_ee->Scale(1./entries);
*/

    /*
    // Efficiency profiles
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tCreating efficiency histos"
       <<endreq;log.flush();
    this->makeEffHisto(m_z_ClusterEt, m_z_ClusterWithTrackEt, m_z_ClusterEtEff);
    this->makeEffHisto(m_z_ClusterEta, m_z_ClusterWithTrackEta, m_z_ClusterEtaEff);
    this->makeEffHisto(m_z_ClusterPhi, m_z_ClusterWithTrackPhi, m_z_ClusterPhiEff);
    log<< MSG::DEBUG <<
      "egammaMonTool::procHistograms()\tCreating efficiency histos (with ID)"
       <<endreq;log.flush();
    this->makeEffHisto(m_z_ClusterEt, m_z_CutClusterEt, m_z_CutClusterEtEff);
    this->makeEffHisto(m_z_ClusterEta, m_z_CutClusterEta, m_z_CutClusterEtEff);
    this->makeEffHisto(m_z_ClusterPhi, m_z_CutClusterPhi, m_z_CutClusterEtEff);
    */
  }	
  if( isEndOfRun ) { }
  
  return StatusCode( StatusCode::SUCCESS );
}





///////////////////////////////////////////
//
// Helper methods for histogram filling
//
///////////////////////////////////////////

void
egammaMonTool::
fillEMTrackMatchHistos(const egamma* eg, std::string& ParticleName)
{
  MsgStream log( msgSvc(), name() );

  int index = m_h_index;
  //std::vector<TH1*>* histos;

  if (ParticleName == "Electron" )        ;//histos = &m_el_histos;
  else if (ParticleName == "Photon" )     return; //histos = &m_ph_histos;
  //else                                    histos = 0;

  const EMTrackMatch* trackMatch = eg->detail<EMTrackMatch>(m_EMTrackMatchCnt);
  if(trackMatch) {
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
    
      // E/p
      //double ep = trackMatch->EoverP();
      double ep = 0.;
      if ( eg->trackParticle() ){	    
	double p = eg->trackParticle()->pt()*
	  cosh(eg->trackParticle()->eta());
	double e = eg->cluster()->et()*cosh(eg->cluster()->eta());
	ep = p>0. ? e/p : 0.;
      }
      
      // if(m_el_histos[index])      m_el_histos[index]->Fill(float(trackMatch->EtaCorrMag()));	index++;
      // if(m_el_histos[index])      m_el_histos[index]->Fill(float(trackMatch->EoverP()));	index++;	
      if(m_el_histos[index])      m_el_histos[index]->Fill(float(ep));	index++;	
      if(m_el_histos[index])      m_el_histos[index]->Fill(float(trackMatch->deltaEta(1)));	index++;
      if(m_el_histos[index])      m_el_histos[index]->Fill(float(trackMatch->deltaPhi(2)));	index++;
    }    
    m_h_index = index;
  }
  else {
    if (ParticleName == "Electron" )
      log << MSG::WARNING << "Can't retrive EMTrackMatch container with the name: " << m_EMTrackMatchCnt <<endreq;
  }
}



void
egammaMonTool::
fillEMShowerHistos(const egamma* eg, std::string& ParticleName) 
{
  MsgStream log( msgSvc(), name() );

  int index = m_h_index;
  std::vector<TH1*>* histos;			

  if (ParticleName == "Electron" )   	histos = &m_el_histos;
  else if (ParticleName == "Photon" )   	histos = &m_ph_histos;
  else 					histos = 0;

  const EMShower* shower = eg->detail<EMShower>(m_EMShowerCnt) ;
  if(shower) {
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
    
      if((*histos)[index])     (*histos)[index]->Fill(float(shower->f1()));		index++;
      if((*histos)[index])     (*histos)[index]->Fill(float(shower->f3()));		index++;
      if((*histos)[index])     (*histos)[index]->Fill(float(shower->ehad1()));	index++;
      if((*histos)[index])     (*histos)[index]->Fill(float( (shower->e233()) / (shower->e237()) ) );		index++;
      if((*histos)[index])     (*histos)[index]->Fill(float( (shower->e237()) / (shower->e277()) ) );		index++;
      //                if((*histos)[index])     (*histos)[index]->Fill(float(shower->e277()));		index++;
      if((*histos)[index])     (*histos)[index]->Fill(float(shower->ecore()));	index++;
    }
    m_h_index = index;	
  }
  else {
    log << MSG::WARNING << "Can't retrive EMShower container with the name: " << m_EMShowerCnt <<endreq;
  }
}



void
egammaMonTool::
fillEMConvertHistos(const egamma* eg, std::string& ParticleName)
{
  MsgStream log( msgSvc(), name() );

  int index = m_h_index;
  std::vector<TH1*>* histos;
  log << MSG::DEBUG << "egammaMonTool::fillEMConvertHistos()\tRunning with particle "+ParticleName<<endreq;
  if (ParticleName == "Electron" )        histos = &m_el_histos;
  else if (ParticleName == "Photon" )     histos = &m_ph_histos;
  else                                    histos = 0;
  log<< MSG::DEBUG << "egammaMonTool::fillEMConvertHistos()\tindex starts as "
     << index<<endreq;
  const EMConvert* conv = eg->detail<EMConvert>(m_EMConvertCnt);
  double convTrkMatch = -999;
  double convAngleMatch = -999;
  if(conv) {
    convTrkMatch = conv->convTrackMatch();
    convAngleMatch = conv->convAngleMatch();	
  }
  else {
    log << MSG::WARNING << "Can't retrive EMConvert container with the name: " << m_EMConvertCnt <<endreq;
  }
  if( m_environment == AthenaMonManager::tier0 || m_environment ==
      AthenaMonManager::tier0ESD ) {
    // book or fill histogram that can be made either on Raw or ESD
    if((*histos)[index])     (*histos)[index]->Fill(convTrkMatch);
    index++;
    if((*histos)[index])     (*histos)[index]->Fill(convAngleMatch);
    index++;
  }
    /**
       HACK. Since we have issues with conversion detail container, previous
       fillings are not happening, hence index is never increased as it should.
       For the time being, we fix the index by hand increasing it by 2 when we 
       it hasn't been  before.
    */
  //  if(index== m_h_index) index+=2; // Remove me if EMConver stuff is fixed.
  log<< MSG::DEBUG << "egammaMonTool::fillEMConvertHistos()\tindex ends as "
     << index<<endreq;
  m_h_index = index;
  
}



void
egammaMonTool::
fillCaloClusterHistos(const egamma* eg, std::string& ParticleName)
{
  MsgStream log( msgSvc(), name() );

  std::vector<TH1*>* histos;
  const CaloCluster* clus = eg->cluster();

  if(clus) {
    if (ParticleName == "Electron" )        histos = &m_el_ene_histos;
    else if (ParticleName == "Photon" )     histos = &m_ph_ene_histos;
    else                                    histos = 0;

    // uncorrected energies in different calo samplings at EM scale
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
    
      if((*histos)[0])       (*histos)[0]->Fill(float((clus)->eSample(CaloSampling::PreSamplerB))) ;
      if((*histos)[1])       (*histos)[1]->Fill(float((clus)->eSample(CaloSampling::EMB1))) ;
      if((*histos)[2])       (*histos)[2]->Fill(float((clus)->eSample(CaloSampling::EMB2))) ;
      if((*histos)[3])       (*histos)[3]->Fill(float((clus)->eSample(CaloSampling::EMB3))) ;
      if((*histos)[4])       (*histos)[4]->Fill(float((clus)->eSample(CaloSampling::PreSamplerE))) ;
      if((*histos)[5])       (*histos)[5]->Fill(float((clus)->eSample(CaloSampling::EME1))) ;
      if((*histos)[6])       (*histos)[6]->Fill(float((clus)->eSample(CaloSampling::EME2))) ;
      if((*histos)[7])       (*histos)[7]->Fill(float((clus)->eSample(CaloSampling::EME3))) ;
    }


    if (ParticleName == "Electron" )        histos = &m_el_eta_histos;
    else if (ParticleName == "Photon" )     histos = &m_ph_eta_histos;
    else                                    histos = 0;

    // uncorrected eta positions in the different calo samplings
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
    
      if((*histos)[0])       (*histos)[0]->Fill(float((clus)->etaSample(CaloSampling::PreSamplerB))) ;
      if((*histos)[1])       (*histos)[1]->Fill(float((clus)->etaSample(CaloSampling::EMB1))) ;
      if((*histos)[2])       (*histos)[2]->Fill(float((clus)->etaSample(CaloSampling::EMB2))) ;
      if((*histos)[3])       (*histos)[3]->Fill(float((clus)->etaSample(CaloSampling::EMB3))) ;
      if((*histos)[4])       (*histos)[4]->Fill(float((clus)->etaSample(CaloSampling::PreSamplerE))) ;
      if((*histos)[5])       (*histos)[5]->Fill(float((clus)->etaSample(CaloSampling::EME1))) ;
      if((*histos)[6])       (*histos)[6]->Fill(float((clus)->etaSample(CaloSampling::EME2))) ;
      if((*histos)[7])       (*histos)[7]->Fill(float((clus)->etaSample(CaloSampling::EME3))) ;

    }

    if (ParticleName == "Electron" )        histos = &m_el_phi_histos;
    else if (ParticleName == "Photon" )     histos = &m_ph_phi_histos;
    else                                    histos = 0;

    // uncorrected phi positions in the different calo samplings
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
    
      if((*histos)[0])       (*histos)[0]->Fill(float((clus)->phiSample(CaloSampling::PreSamplerB))) ;
      if((*histos)[1])       (*histos)[1]->Fill(float((clus)->phiSample(CaloSampling::EMB1))) ;
      if((*histos)[2])       (*histos)[2]->Fill(float((clus)->phiSample(CaloSampling::EMB2))) ;
      if((*histos)[3])       (*histos)[3]->Fill(float((clus)->phiSample(CaloSampling::EMB3))) ;
      if((*histos)[4])       (*histos)[4]->Fill(float((clus)->phiSample(CaloSampling::PreSamplerE))) ;
      if((*histos)[5])       (*histos)[5]->Fill(float((clus)->phiSample(CaloSampling::EME1))) ;
      if((*histos)[6])       (*histos)[6]->Fill(float((clus)->phiSample(CaloSampling::EME2))) ;
      if((*histos)[7])       (*histos)[7]->Fill(float((clus)->phiSample(CaloSampling::EME3))) ;
    }
  }
  else {
    log << MSG::WARNING << "Can't retrive  CaloCluster object from egamma " <<endreq;
  }
}

void
egammaMonTool::
fillIDHistos(const egamma* eg, std::string& ParticleName)
{
  MsgStream log( msgSvc(), name() );

  int index = m_h_index;
  std::vector<TH1*>* histos;

  if (ParticleName == "Electron" )        histos = &m_el_histos;
  else if (ParticleName == "Photon" )     histos = &m_ph_histos;
  else                                    histos = 0;

  // No TrackParticle for photons, so return
  if ( ParticleName == "Photon" ) return;

  const Rec::TrackParticle* aTrackParticle = eg->trackParticle();
  if( ( ! aTrackParticle ) 
      && ParticleName == "Electron" )
    {
      log << MSG::WARNING << "Error attempting to retrieve trackParticle"<<endreq;
      return;
    }
  const Trk::TrackSummary * trksummary = aTrackParticle->trackSummary();
  if( ( ! trksummary ) 
      && ParticleName == "Electron" )
    {
      log << MSG::WARNING << "Error attempting to retrieve trackSummary"<<endreq;
      return;
    }
  if(trksummary) {
    if( m_environment == AthenaMonManager::tier0 || m_environment ==
	AthenaMonManager::tier0ESD ) {
      // book or fill histogram that can be made either on Raw or ESD
      log << MSG::DEBUG << "egammaMonTool::fillIDHistos()\tFilling ID histos for particle = "+ParticleName<<endreq;
      if((*histos)[index])     (*histos)[index]->Fill( trksummary->get(Trk::numberOfBLayerHits) );	index++;
      if((*histos)[index])     (*histos)[index]->Fill( trksummary->get(Trk::numberOfPixelHits) );	index++;
      if((*histos)[index])     (*histos)[index]->Fill( trksummary->get(Trk::numberOfPixelHits) +
						       trksummary->get(Trk::numberOfSCTHits) );	index++;
      if((*histos)[index])     (*histos)[index]->Fill( trksummary->get(Trk::numberOfTRTHits) );	index++;
      //if((*histos)[index])     (*histos)[index]->Fill( trksummary->get(Trk::numberOfTRTHighThresholdHits) );	index++;


      m_h_index = index;
    }
  }
  else {

    log << MSG::WARNING << "Can't retrive TrackSummary container with the name: " << m_EMConvertCnt <<endreq;
  }
}


void
egammaMonTool::
fillRegionHistos(const egamma* eg, std::string& ParticleName)
{
  MsgStream log( msgSvc(), name() );
  log<<MSG::DEBUG<<"egammaMonTool::fillRegionHistos()\trunning"<<endreq;
  int index = m_h_index;
  std::vector<TH1*>* histos;

  if (ParticleName == "Electron" )        histos = &m_el_histos;
  else if (ParticleName == "Photon" )     histos = &m_ph_histos;
  else                                    histos = 0;
  // 	std::cout << "ParticleName = "<<ParticleName
  // 		  << " Histos = "<< histos
  // 		  << " Hisots size = " << histos->size()
  // 		  << std::endl;
  const CaloCluster * cluster = eg->cluster();
  if(!cluster)
    {
      log <<MSG::WARNING<<"Unable to retrieve cluster from egamma."<<endreq;
      return;
    }
  //	std::cout<<"Retrieved cluster "<<cluster<<std::endl;

  if(cluster)
    {
      float et = cluster->et();
      float eta = cluster->eta();
      float phi = cluster->phi();
      //  	    std::cout<<" et = "<<et
      //  		     <<" eta = "<<eta
      //  		     <<" phi = "<<phi
      //  		     <<std::endl;
      //	    std::cout<<"Fill histograms on index = "<<index<<std::endl;
      if( m_environment == AthenaMonManager::tier0 || m_environment ==
	  AthenaMonManager::tier0ESD ) {
	// book or fill histogram that can be made either on Raw or ESD      	       
	if((*histos)[index])
	  {
	    if(ParticleName=="Electron") {
	      log << MSG::DEBUG << "Et Eta Region histo name = "
		  << (*histos)[index]->GetName() << endreq;
	    }
	    (*histos)[index]->Fill(  eta,et ) ;
	    index++;
	  }
	//	    std::cout<<"Fill histograms on index = "<<index<<std::endl;
	if((*histos)[index])
	  {
	    if(ParticleName=="Electron") {
	      log << MSG::DEBUG << "Et Phi Region histo name = "
		  << (*histos)[index]->GetName() << endreq;
	    }
	    (*histos)[index]->Fill( phi,et ) ;
	    index++;
	  }
      }
    }
  m_h_index = index;

  //	std::cout<<"Ended fillRegionHistos()\n";
}

void
egammaMonTool::
fillIsEMHisto(const egamma* eg)
{
  MsgStream log( msgSvc(), name() );

  //std::cout << "ParticleName = "<<ParticleName
  //                << " Histos = "<< histos
  //                << " Hisots size = " << histos->size()
  //                << std::endl;

  // Determine value(s) to fill with
  if( m_environment == AthenaMonManager::tier0 || m_environment ==
      AthenaMonManager::tier0ESD ) {
    // book or fill histogram that can be made either on Raw or ESD
    if( passesIsEMCut(egammaPID::ElectronLoosePP, eg) )
      m_IsEMHisto->Fill(Loose);
    if( passesIsEMCut(egammaPID::ElectronMediumPP, eg ) )
      m_IsEMHisto->Fill(Medium);
    if( passesIsEMCut(egammaPID::ElectronTightPP, eg ) )
      m_IsEMHisto->Fill(Tight);
  }
}


bool egammaMonTool::inBarrel(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) < 1.3 ) {
    return true;
  }
  return false;
}
bool egammaMonTool::inEndcap(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.5 ) {
    return true;
  }
  return false;
}
bool egammaMonTool::inCrack(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.3 &&
      fabs( cluster->eta() ) < 1.5 ) {
    return true;
  }
  return false;
}

void
egammaMonTool::
fillIsEMMediumCutHistos(const egamma* eg, std::string& /*ParticleName*/)
{
  MsgStream log( msgSvc(), name() );

  //std::cout << "ParticleName = "<<ParticleName
  //                << " Histos = "<< histos
  //                << " Hisots size = " << histos->size()
  //                << std::endl;

  // Determine value(s) to fill with
  if( passesIsEMCut(egammaPID::ElectronMediumPP, eg ) )
    {
      //      std::cout<<"getting clus\n";
      const CaloCluster* clus = eg->cluster();
      //      std::cout<<"getting trackmatch\n";
      const EMTrackMatch* trackMatch = eg->detail<EMTrackMatch>(m_EMTrackMatchCnt);
      //      std::cout<<"getting shower\n";
      const EMShower* shower = eg->detail<EMShower>(m_EMShowerCnt) ;
      float et ;
      float eta ;
      float phi;
      if(clus)
	{
	  //	  std::cout<<"getting et\n";
	  et = clus->et();
	  //	  std::cout<<"getting eta\n";
	  eta = clus->eta();
	  phi = clus->phi();
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	  		
	    //	  std::cout<<"Filling et\n";
	    m_el_IsEMMedium_ClusterEtHisto->Fill( et );
	    //	  std::cout<<"Filling eta\n";
	    m_el_IsEMMedium_ClusterEtaHisto->Fill( eta );
	    m_el_IsEMMedium_ClusterPhiHisto->Fill( phi );
	    //	  std::cout<<"Filling et, eta\n";
	    m_el_IsEMMedium_EtvsEtaHisto->Fill(eta,et);
	    //	  std::cout<<"Filling et, phi\n";
	    m_el_IsEMMedium_EtvsPhiHisto->Fill( clus->phi(),et);
	    int HighEtRange=-1;
	    if( et > 100000 ) HighEtRange=0;
	    else
	      if( et > 500000 ) HighEtRange=1;
	      else
		if( et > 1000000 ) HighEtRange=2;
	    if( HighEtRange != -1 )
	      m_el_IsEMMedium_HighEtRangesHisto->Fill(HighEtRange);
	    float e = et*cosh(eta);
	    //	  std::cout<<"getting f1\n";
	    float f1 = clus->energyBE(1) / e;
	    //	  std::cout<<"getting f2\n";
	    float f2 = clus->energyBE(2) / e;
	    //	  std::cout<<"getting f3\n";
	    float f3 = clus->energyBE(3) / e;

	    //	  std::cout<<"filling f1\n";
	    m_el_IsEMMedium_F1Histo->Fill(f1);
	    //	  std::cout<<"filling f2\n";
	    m_el_IsEMMedium_F2Histo->Fill(f2);
	    //	  std::cout<<"filling f3\n";
	    m_el_IsEMMedium_F3Histo->Fill(f3);
	  }
	}
      // E/p
      //double ep = trackMatch->EoverP();
      double ep = 0.;
      if ( eg->trackParticle() ){	    
	double p = eg->trackParticle()->pt()*
	  cosh(eg->trackParticle()->eta());
	double e = eg->cluster()->et()*cosh(eg->cluster()->eta());
	ep = p>0. ? e/p : 0.;
      }
      if( m_environment == AthenaMonManager::tier0 || m_environment ==
	  AthenaMonManager::tier0ESD ) {
	m_el_IsEMMedium_EoverPHisto->Fill(ep);
      }
      if(trackMatch)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	    //	  std::cout<<"fill eoverp\n";
	    //m_el_IsEMMedium_EoverPHisto->Fill(trackMatch->EoverP());
	    //	  std::cout<<"fill deltaeta\n";
	    m_el_IsEMMedium_DeltaEta1Histo->Fill(trackMatch->deltaEta(1));
	    //	  std::cout<<"fill deltaphi\n";
	    m_el_IsEMMedium_DeltaPhi2Histo->Fill(trackMatch->deltaPhi(2));
	  }
	}
      if(shower)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	    //	  std::cout<<"fill ethad\n";
	    m_el_IsEMMedium_EthadHisto->Fill(shower->ehad1());
	    //	  std::cout<<"fill 233/237\n";
	    m_el_IsEMMedium_rE233E237Histo->Fill( shower->e233()/shower->e237() );
	    //	  std::cout<<"fill 237/277\n";
	    m_el_IsEMMedium_rE237E277Histo->Fill( shower->e237()/shower->e277() );
	  }
	}
    }

  //std::cout<<"Ended fillRegionHistos()\n";
}
bool
egammaMonTool::
passesIsEMCut(const unsigned int Cut , const egamma * eg)
{
  const unsigned int fullmask = egammaPID::ElectronTightPP;
  const unsigned int mask = eg->isem(fullmask);

  if( ( mask & Cut ) == 0 ) return true;
  return false;
}

void
egammaMonTool::
fillIsEMTightCutPhotonHistos(const egamma* eg, std::string& /*ParticleName*/)
{
  MsgStream log( msgSvc(), name() );

  if( passesIsEMCut(egammaPID::ElectronTightPP, eg ) ) // requesting electrontight is equivalent to request eg->isem() == 0 since isem() by default used ElectronTight (All)
    {
      const CaloCluster* clus = eg->cluster();
      const EMShower* shower = eg->detail<EMShower>(m_EMShowerCnt) ;
      const EMConvert* conv = eg->detail<EMConvert>(m_EMConvertCnt);
      if(clus)
	{
	  float Et = clus->et();
	  float eta = clus->eta();
	  float phi = clus->phi();
	  float e =  Et*cosh(eta);
	  float f1 = clus->energyBE(1) / e;
	  float f2 = clus->energyBE(2) / e;
	  float f3 = clus->energyBE(3) / e;
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	    m_ph_IsEMTight_ClusterEtHisto->Fill(Et);
	    m_ph_IsEMTight_ClusterEtaHisto->Fill(eta);
	    m_ph_IsEMTight_ClusterPhiHisto->Fill(phi);
	    m_ph_IsEMTight_F1Histo->Fill(f1);
	    m_ph_IsEMTight_F2Histo->Fill(f2);
	    m_ph_IsEMTight_F3Histo->Fill(f3);
	    m_ph_IsEMTight_EtvsEtaHisto->Fill(eta,Et);
	    m_ph_IsEMTight_EtvsPhiHisto->Fill(phi,Et);
	  }
	}

      if(shower)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	    m_ph_IsEMTight_EtHadHisto->Fill(shower->ehad1());
	    m_ph_IsEMTight_rE233E237Histo->Fill(shower->e233()/shower->e237());
	    m_ph_IsEMTight_rE237E277Histo->Fill(shower->e237()/shower->e277());
	    m_ph_IsEMTight_ECoreHisto->Fill(shower->ecore());
	  }
	}
      if(conv)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	  
	    m_ph_IsEMTight_convTrkMatchHisto->Fill(float(conv->convTrackMatch()));
	    m_ph_IsEMTight_convAngleMatchHisto->Fill(float(conv->convAngleMatch()));
	  }	
	}
    }

}


void
egammaMonTool::
fillEoverPHistos(const egamma* eg)
{
  MsgStream log( msgSvc(), name() );
  log<<MSG::DEBUG<<"egammaMonTool::fillEoverPHistos()\t"<<endreq;
  // Calculate relevant variables
  double Et;
  double eta;
  double phi;
  int etabin=-1;
  int phibin=-1;
  double EoverP;
  int charge;
  // Cluster Et, eta, phi
  if(eg->cluster())
    {
      Et=eg->cluster()->et();
      eta=eg->cluster()->eta();
      phi=eg->cluster()->phi();
    }
  else return;

  // E/p
  //double ep = trackMatch->EoverP();
  //double ep = 0.;
  if ( eg->trackParticle() ){	    
    double p = eg->trackParticle()->pt()*
      cosh(eg->trackParticle()->eta());
    double e = eg->cluster()->et()*cosh(eg->cluster()->eta());
    EoverP = p>0. ? e/p : 0.;
  } 
  else return;
  //  if(trackMatch) 
  //{
  //  EoverP = trackMatch->EoverP();
  //}
  //else return;
  // charge
  charge = (int) eg->charge();

  // passes isem tight
  bool PassesIsEM = this->passesIsEMCut(egammaPID::ElectronTightPP, eg );
  // apply kinematic cuts
  if( ( Et < m_EtCut) ||
      ( etabin == -1 ) ||
      ( phibin == -1 ) ) return;
  // Fill histos
  switch(charge)
    {
    case -1:
      if( m_environment == AthenaMonManager::tier0 || m_environment ==
	  AthenaMonManager::tier0ESD ) {
	// book or fill histogram that can be made either on Raw or ESD
	m_el_Electron_EoverPvsEta->Fill(EoverP,eta);
	m_el_Electron_EoverPvsPhi->Fill(EoverP,phi);      
      }
      if(PassesIsEM )
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	  
	    m_el_TightElectron_EoverPvsEta->Fill(EoverP,eta);
	    m_el_TightElectron_EoverPvsPhi->Fill(EoverP,phi);
	    	   
	  }
	}
      if(EoverP > 2)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	    	  
	    m_el_Electron_EoverP->Fill(EoverP);
	  }
	}
      break;
    case 1:
      if( m_environment == AthenaMonManager::tier0 || m_environment ==
	  AthenaMonManager::tier0ESD ) {
	// book or fill histogram that can be made either on Raw or ESD
      
	m_el_Positron_EoverPvsEta->Fill(EoverP,eta);
	m_el_Positron_EoverPvsPhi->Fill(EoverP,phi);
      }
      if(PassesIsEM )
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	  
	    m_el_TightPositron_EoverPvsEta->Fill(EoverP,eta);
	    m_el_TightPositron_EoverPvsPhi->Fill(EoverP,phi);	  
	  }
	}
      if(EoverP > 2)
	{
	  if( m_environment == AthenaMonManager::tier0 || m_environment ==
	      AthenaMonManager::tier0ESD ) {
	    // book or fill histogram that can be made either on Raw or ESD
	  	  
	    m_el_Positron_EoverP->Fill(EoverP);
	  }      
	}
      break;
    default:
      log<<MSG::WARNING<<"Electron candidate has charge = "<<charge<<endreq;
    }


}
