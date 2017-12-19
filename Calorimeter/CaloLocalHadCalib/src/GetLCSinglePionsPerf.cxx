/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CheckSinglePionsReco.cxx,v 1.1.1.1 2008/11/04 08:56:11 menke Exp $
//
// Description: see CheckSinglePionsReco.h
//
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"

#include <iterator>
#include <cmath>

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h" 

#include "CaloEvent/CaloCell.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GeneratorObjects/McEventCollection.h"
#include <CLHEP/Vector/LorentzVector.h>
#include "CaloEvent/CaloCompositeKineBase.h"
#include "CaloEvent/CaloRecoStatus.h"
#include "TBEvent/TBEventInfo.h"

#include "xAODCaloEvent/CaloCluster.h"


#include "TProfile.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "TH1F.h"
#include "TVector3.h"
#include "TTree.h"


using CLHEP::HepLorentzVector;
using CLHEP::MeV;


/* ****************************************************************************

**************************************************************************** */
GetLCSinglePionsPerf::GetLCSinglePionsPerf(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_id_helper(0),
    m_calo_dd_man(0),
    m_calo_id(0),
    m_caloDmDescrManager(0),
    m_outputFileName("CheckSinglePionsReco.root"),
    m_outputFile(0),
    m_distance_cut(1.5),
    m_netabin(25), m_etamin(0.0), m_etamax(5.0),
    m_deta(0),
    m_nphibin(1), m_phimin(-M_PI), m_phimax(M_PI),
    m_dphi(0),
    m_nlogenerbin(22), m_logenermin(2.0), m_logenermax(6.4),
    m_nlogenerbin2(3),
    m_dlogener(0),
    m_xcryo_min(-80.), m_xcryo_max(80.),m_ytable_min(-1000.), m_ytable_max(1000.),
    m_nnormtype(3),
    m_ncluscoll(4),
    m_ntagcases(3),
    m_nmoments(0),
    m_nmomsums(6),
    m_truthPiEngFraction(0.1),
    m_isTestbeam(false),
    m_doEngRecOverTruth(true),
    m_doEngTag(true),
    m_doEngRecSpect(true),
    m_doEngNoiseClus(true),
    m_doClusMoments(true),
    m_doRecoEfficiency(true),
    m_useRecoEfficiency(false),
    m_useGoodClus(false),
    m_doCalibHitsValidation(true),
    m_mc_ener(0),
    m_mc_eta(0),
    m_mc_phi(0),
    m_mc_etabin(0),
    m_mc_enerbin(0),
    m_mc_phibin(0)
{
  // collection name where cluster always have moments
  declareProperty("ClusterBasicCollName",m_clusterBasicCollName);

  // collections names after different correction steps
  declareProperty("ClusterCollectionNames",m_clusterCollNames);

  // distance cut for clusters to calcuate engReco
  declareProperty("DistanceCut",m_distance_cut);

  // Name of output file to save histograms in
  declareProperty("OutputFileName",m_outputFileName);

  // Names of CalibrationHitContainers to use
  declareProperty("CalibrationHitContainerNames",m_CalibrationHitContainerNames); 
  declareProperty("DMCalibrationHitContainerNames",m_DMCalibrationHitContainerNames);

  // to create different types of validation histograms
  declareProperty("doEngRecOverTruth",m_doEngRecOverTruth);
  declareProperty("doEngTag",m_doEngTag);
  declareProperty("doEngRecSpect",m_doEngRecSpect);
  declareProperty("doEngNoiseClus",m_doEngNoiseClus);
  declareProperty("doClusMoments",m_doClusMoments);
  declareProperty("doRecoEfficiency",m_doRecoEfficiency);
  declareProperty("useRecoEfficiency",m_useRecoEfficiency);
  declareProperty("useGoodClus",m_useGoodClus);

  declareProperty("etamin",m_etamin);
  declareProperty("etamax",m_etamax);
  declareProperty("netabin",m_netabin);

  declareProperty("phimin",m_phimin);
  declareProperty("phimax",m_phimax);
  declareProperty("nphibin",m_nphibin);

  declareProperty("logenermin",m_logenermin);
  declareProperty("logenermax",m_logenermax);
  declareProperty("nlogenerbin",m_nlogenerbin);

  declareProperty("isTestbeam",m_isTestbeam);

  declareProperty("doCalibHitsValidation",m_doCalibHitsValidation);
//   Only use clusters that contain at least m_truthPiEngFraction % of the true pion energy
  declareProperty("usePionClustersOnly",m_usePionClusters);

  // dead material identifier description manager  
  m_caloDmDescrManager = CaloDmDescrManager::instance();

  m_validMoments.resize(0);
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_TOT"),xAOD::CaloCluster::ENG_CALIB_TOT));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_OUT_L"),xAOD::CaloCluster::ENG_CALIB_OUT_L));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TOT"),xAOD::CaloCluster::ENG_CALIB_DEAD_TOT));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EMB0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILE0"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_TILEG3"),xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_EME0"),xAOD::CaloCluster::ENG_CALIB_DEAD_EME0));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_HEC0"),xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_FCAL"),xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_LEAKAGE"),xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE));
  m_validMoments.push_back(moment_name_pair(std::string("ENG_CALIB_DEAD_UNCLASS"),xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS));

  m_nmoments = m_validMoments.size();

}



/* ****************************************************************************

**************************************************************************** */
GetLCSinglePionsPerf::~GetLCSinglePionsPerf()
{

}



/* ****************************************************************************

**************************************************************************** */
StatusCode GetLCSinglePionsPerf::initialize()
{
  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 

  m_calo_id = m_calo_dd_man->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_id_helper) );


  m_deta = (m_etamax - m_etamin)/m_netabin;
  m_dphi = (m_phimax - m_phimin)/m_nphibin;
  m_dlogener = (m_logenermax - m_logenermin)/m_nlogenerbin;

  m_outputFile = new TFile(m_outputFileName.c_str(),"RECREATE");
  m_outputFile->cd();

  // defining histograms
  char hname[256];
  float h_eta_min = -5.0;
  float h_eta_max=5.0;
  int h_nch_eta = 50;
  if(m_etamin != 0.0) {
    h_eta_min = m_etamin;
    h_eta_max = m_etamax;
  }
  const int n_logener_bins = m_nlogenerbin*2;
  double *xbins = new double[n_logener_bins+1];
  const double inv_n_logener_bins = 1. / static_cast<double> (n_logener_bins);
  for(int i_bin=0; i_bin<=n_logener_bins; i_bin++) {
    xbins[i_bin] = pow(10,m_logenermin+i_bin*(m_logenermax-m_logenermin)*inv_n_logener_bins);
  }
  m_ncluscoll = m_clusterCollNames.size();

  /* **************************************************************************
  histograms to check classification performance
  *************************************************************************** */
  if(m_doEngTag) {
    // as a function of pion eta for several energy bins
    m_engTag_vs_eta.resize(m_ntagcases);
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++) {
      m_engTag_vs_eta[i_tag].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        sprintf(hname, "hp_engTag_vs_eta_tag%d_ener%d",i_tag, i_ener);
        TProfile *hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
        hp->GetXaxis()->SetTitle("#eta");
        hp->GetYaxis()->SetTitle("E_{class}/E_{tot}");
        m_engTag_vs_eta[i_tag][i_ener] = hp;
      } // i_ener
    } // i_tag
    // as a function of pion energy for several eta bins
    m_engTag_vs_ebeam.resize(m_ntagcases);
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
      m_engTag_vs_ebeam[i_tag].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++) {
        sprintf(hname, "hp_engTag_vs_ebeam_tag%d_eta%d",i_tag, i_eta);
        TProfile *hp = new TProfile(hname, hname, n_logener_bins, xbins);
        hp->GetXaxis()->SetTitle("E_{#pi}, MeV");
        hp->GetYaxis()->SetTitle("E_{class}/E_{tot}");
        m_engTag_vs_ebeam[i_tag][i_eta] = hp;
      }
    }
  } // m_doEngTag


  /* **************************************************************************
  histograms to check reconstructed energy after each step of local hadronic 
  calibration, reco energy is defined as a sum of clusters energies within certain 
  angle around particle direction
  *************************************************************************** */
  if(m_doEngRecOverTruth) {
    // as a function of pion eta for several energy bins
    m_engRecOverTruth_vs_eta.resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecOverTruth_vs_eta[i_coll].resize(m_nnormtype);
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        m_engRecOverTruth_vs_eta[i_coll][i_norm].resize(m_nlogenerbin, 0);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          double ylow(0), yup(0);
          if(i_norm != kLEVEL_PARTICLE) {
            ylow = 0.0;
            yup  = 100.0;
          }
          sprintf(hname, "hp_engRecOverTruth_vs_eta_coll%d_norm%d_ener%d",i_coll, i_norm, i_ener);
          TProfile *hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max, ylow, yup);
          hp->GetXaxis()->SetTitle("#eta");
          if(i_norm==kLEVEL_PARTICLE) {
            hp->GetYaxis()->SetTitle("E_{reco}/E_{#pi}");
          }else{
            hp->GetYaxis()->SetTitle("E_{reco}/E_{calib}");
          }
          m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener] = hp;
        } // i_ener
      } // i_norm
    } // i_coll
    // as a function of pion energy for several eta bins
    m_engRecOverTruth_vs_ebeam.resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecOverTruth_vs_ebeam[i_coll].resize(m_nnormtype);
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        m_engRecOverTruth_vs_ebeam[i_coll][i_norm].resize(m_netabin, 0);
        for(int i_eta=0; i_eta<m_netabin; i_eta++) {
          double ylow(0), yup(0);
          if(i_norm != kLEVEL_PARTICLE) {
            ylow = 0.0;
            yup  = 100.0;
          }
          sprintf(hname, "hp_m_engRecOverTruth_vs_ebeam_coll%d_norm%d_eta%d",i_coll, i_norm, i_eta);
          TProfile *hp = new TProfile(hname, hname, n_logener_bins, xbins, ylow, yup);
          hp->GetXaxis()->SetTitle("E_{#pi}, MeV");
          if(i_norm==kLEVEL_PARTICLE) {
            hp->GetYaxis()->SetTitle("E_{reco}/E_{#pi}");
          }else{
            hp->GetYaxis()->SetTitle("E_{reco}/E_{calib}");
          }
          m_engRecOverTruth_vs_ebeam[i_coll][i_norm][i_eta] = hp;
        } // i_eta
      } // i_norm
    } // i_coll
  } // m_doEngRecOverTruth


  /* **************************************************************************
  histograms to save reco and noise clusters energy spectras
  *************************************************************************** */
  if(m_doEngRecSpect) {
    m_engRecSpect.resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecSpect[i_coll].resize(m_nlogenerbin);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        m_engRecSpect[i_coll][i_ener].resize(m_netabin, 0);
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          sprintf(hname, "hp_engRecSpect_coll%d_ener%d_eta%d",i_coll, i_ener, i_eta);
          TH1F *h1 = new TH1F(hname, hname, 110, -0.2, 2.0);
          h1->GetXaxis()->SetTitle("E_{reco}/E_{#pi}");
          m_engRecSpect[i_coll][i_ener][i_eta] = h1;
        } // i_eta
      } // i_ener
    } // i_coll
    // energy spectra of incident pions
    m_engPionSpect.resize(m_nlogenerbin);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      m_engPionSpect[i_ener].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "hp_engPionSpect_ener%d_eta%d",i_ener, i_eta);
        double x1 = pow(10,m_logenermin+i_ener*m_dlogener);
        double x2 = pow(10,m_logenermin+(i_ener+1)*m_dlogener);
        TH1F *h1 = new TH1F(hname, hname, 20, x1, x2);
        h1->GetXaxis()->SetTitle("E_{#pi}, MeV");
        m_engPionSpect[i_ener][i_eta] = h1;
      } // i_eta
    } // i_ener
  } // m_doEngRecSpect

  /* **************************************************************************
  histograms to save noise clusters spectras
  *************************************************************************** */
  if(m_doEngNoiseClus) {
    // noise clusters spectras
    m_engNoiseClusSpect.resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engNoiseClusSpect[i_coll].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "hp_engNoiseClusSpect_coll%d_eta%d",i_coll, i_eta);
        TH1F *h1 = new TH1F(hname, hname, 100, -2000., 2000.);
        h1->GetXaxis()->SetTitle("E_{clus}, MeV");
        m_engNoiseClusSpect[i_coll][i_eta] = h1;
      } // i_eta
    } // i_coll
    // average
    m_engNoiseClus_vs_eta.resize(m_ncluscoll, 0);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      sprintf(hname, "hp_engNoiseClus_vs_eta_coll%d",i_coll);
      TProfile *hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
      hp->GetXaxis()->SetTitle("#eta");
      hp->GetYaxis()->SetTitle("E_{clus}, MeV");
      m_engNoiseClus_vs_eta[i_coll] = hp;
      //hp->SetErrorOption("s");
    }
  } // m_doEngNoiseClus

  /* **************************************************************************
  histograms to save cluster moments
  *************************************************************************** */
  if(m_doClusMoments) {
    m_clusMoment_vs_eta.resize(m_nmoments);
    for(int i_mom=0; i_mom<m_nmoments; i_mom++){
      m_clusMoment_vs_eta[i_mom].resize(m_nmomsums);
      for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
        m_clusMoment_vs_eta[i_mom][i_sum].resize(m_nlogenerbin, 0);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          sprintf(hname, "hp_clusMoment_vs_eta_mom%d_sum%d_ener%d",i_mom, i_sum, i_ener);
          TProfile *hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
          hp->GetXaxis()->SetTitle("#eta");
          hp->GetYaxis()->SetTitle("Mom_{cls}/Mom_{sum}");
          hp->SetTitle(m_validMoments[i_mom].first.c_str());
          m_clusMoment_vs_eta[i_mom][i_sum][i_ener] = hp;
        } // i_ener
      } // i_sum
    } // i_mom

    m_clusMoment_vs_ebeam.resize(m_nmoments);
    for(int i_mom=0; i_mom<m_nmoments; i_mom++){
      m_clusMoment_vs_ebeam[i_mom].resize(m_nmomsums);
      for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
        m_clusMoment_vs_ebeam[i_mom][i_sum].resize(m_netabin, 0);
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          sprintf(hname, "hp_clusMoment_vs_ebeam_mom%d_sum%d_eta%d",i_mom, i_sum, i_eta);
          TProfile *hp = new TProfile(hname, hname, n_logener_bins, xbins);
          hp->GetXaxis()->SetTitle("E_{#pi}");
          hp->GetYaxis()->SetTitle("Mom_{cls}/Mom_{sum}");
          hp->SetTitle(m_validMoments[i_mom].first.c_str());
          m_clusMoment_vs_ebeam[i_mom][i_sum][i_eta] = hp;
        } // i_eta
      } // i_sum
    } // i_mom

  } // m_doClusMoments


  /* **************************************************************************
  histograms to save reconstruction efficiency
  *************************************************************************** */
  if(m_doRecoEfficiency) {
    // as a function of eta for several energy bins
    for(int i_eff=0; i_eff<2; i_eff++){ // 0-reconstructed, 1-all events
      m_RecoEfficiency_vs_eta[i_eff].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        sprintf(hname, "hp_m_RecoEfficiency_eff%d_ener%d", i_eff, i_ener);
        TH1F *h1 = new TH1F(hname, hname,  h_nch_eta, h_eta_min, h_eta_max);
        h1->GetXaxis()->SetTitle("#eta");
        h1->GetYaxis()->SetTitle("Efficiency");
        m_RecoEfficiency_vs_eta[i_eff][i_ener] = h1;
      } // i_eta
    } // i_eff
    // as a function of energy for several eta bins
    for(int i_eff=0; i_eff<2; i_eff++){ // 0-reconstructed, 1-all events
      m_RecoEfficiency_vs_ebeam[i_eff].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "hp_m_RecoEfficiency_eff%d_eta%d", i_eff, i_eta);
        TH1F *h1 = new TH1F(hname, hname, n_logener_bins, xbins);
        h1->GetXaxis()->SetTitle("E_{#pi}, MeV");
        h1->GetYaxis()->SetTitle("Efficiency");
        m_RecoEfficiency_vs_ebeam[i_eff][i_eta] = h1;
      } // i_eta
    } // i_eff
  }

  /* **************************************************************************
  histograms to check calibration hits machinery
  *************************************************************************** */
  if(m_doCalibHitsValidation) {
    m_SumCalibHitOverEbeam_vs_eta.resize(m_nlogenerbin2, 0);
    m_DefaultCalculatorOverEbeam_vs_eta.resize(m_nlogenerbin2, 0);
    m_SumCalibHitAssignedOverEbeam_vs_eta.resize(m_nlogenerbin2, 0);
    m_SumCalibHitAssignedOverEbeam_vs_etaphi.resize(m_nlogenerbin2, 0);
    for(int i_ener=0; i_ener<m_nlogenerbin2; i_ener++){
      sprintf(hname, "hp_SumCalibHitOverEbeam_vs_eta_ener%d", i_ener);
      TProfile *hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
      hp->GetXaxis()->SetTitle("#eta");
      hp->GetYaxis()->SetTitle("#sum E_{calib}/E_{#pi}");
      m_SumCalibHitOverEbeam_vs_eta[i_ener] = hp;
      sprintf(hname, "hp_DefaultCalculatorOverEbeam_vs_eta_ener%d", i_ener);
      hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
      hp->GetXaxis()->SetTitle("#eta");
      hp->GetYaxis()->SetTitle("#sum E_{DefCalc}/E_{#pi}");
      m_DefaultCalculatorOverEbeam_vs_eta[i_ener] = hp;
      sprintf(hname, "hp_SumCalibHitAssignedOverEbeam_vs_eta_ener%d", i_ener);
      hp = new TProfile(hname, hname, h_nch_eta, h_eta_min, h_eta_max);
      hp->GetXaxis()->SetTitle("#eta");
      hp->GetYaxis()->SetTitle("#sum E_{assigned}/E_{#pi}");
      m_SumCalibHitAssignedOverEbeam_vs_eta[i_ener] = hp;
      sprintf(hname, "hp_SumCalibHitAssignedOverEbeam_vs_etaphi_ener%d", i_ener);
      TProfile2D *hp2 = new TProfile2D(hname, hname, 25, h_eta_min, h_eta_max, 25, -M_PI, M_PI);
      hp2->GetXaxis()->SetTitle("#eta");
      hp2->GetYaxis()->SetTitle("#phi");
      hp2->GetZaxis()->SetTitle("#sum E_{assigned}/E_{#pi}");
      m_SumCalibHitAssignedOverEbeam_vs_etaphi[i_ener] = hp2;
    } // i_ener

//     m_SumCalibHitOverEbeam_vs_ebeam.resize(m_netabin, 0);
// //     m_DefaultCalculatorOverEbeam_vs_ebeam.resize(m_netabin, 0);
//     for(int i_eta=0; i_eta<m_netabin; i_eta++){
//       sprintf(hname, "hp_SumCalibHitOverEbeam_vs_ebeam_eta%d", i_eta);
//       TProfile *hp = new TProfile(hname, hname,  n_logener_bins, xbins);
//       hp->GetXaxis()->SetTitle("#eta");
//       hp->GetYaxis()->SetTitle("#sum E_{calib}/E_{#pi}");
//       m_SumCalibHitOverEbeam_vs_ebeam[i_eta] = hp;
// //       sprintf(hname, "hp_DefaultCalculatorOverEbeam_vs_ebeam_eta%d", i_eta);
// //       hp = new TProfile(hname, hname,  n_logener_bins, xbins);
// //       hp->GetXaxis()->SetTitle("#eta");
// //       hp->GetYaxis()->SetTitle("#sum E_{DefCalc}/E_{#pi}");
// //       m_DefaultCalculatorOverEbeam_vs_ebeam[i_eta] = hp;
//     } // i_eta
  } // m_doCalibHitsValidation


  delete [] xbins;

  return StatusCode::SUCCESS;
}



/* ****************************************************************************
writing histograms into the file
**************************************************************************** */
StatusCode GetLCSinglePionsPerf::finalize()
{
  ATH_MSG_INFO( "Writing out histograms"  );

  // ereco over truth after each step of calibration
  if(m_doEngRecOverTruth) {
    m_outputFile->cd();
    m_outputFile->mkdir("engRecOverTruth");
    m_outputFile->cd("engRecOverTruth");
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          if(m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener]) m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener]->Write();
        } // i_ener
      } // i_norm
    } // i_coll
    //
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        for(int i_eta=0; i_eta<m_netabin; i_eta++) {
          if(m_engRecOverTruth_vs_ebeam[i_coll][i_norm][i_eta]) m_engRecOverTruth_vs_ebeam[i_coll][i_norm][i_eta]->Write();
        } // i_eta
      } // i_norm
    } // i_coll
  } // m_doEngRecOverTruth

  // classification performance
  if(m_doEngTag) {
    m_outputFile->cd();
    m_outputFile->mkdir("engTag");
    m_outputFile->cd("engTag");
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++) {
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        if(m_engTag_vs_eta[i_tag][i_ener]) m_engTag_vs_eta[i_tag][i_ener]->Write();
      } // i_ener
    } // i_tag
    // as a function of pion energy for several eta bins
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
      for(int i_eta=0; i_eta<m_netabin; i_eta++) {
        if(m_engTag_vs_ebeam[i_tag][i_eta]) m_engTag_vs_ebeam[i_tag][i_eta]->Write();
      }
    }
  } // m_doEngTag

  // reco energy spactras
  if(m_doEngRecSpect) {
    m_outputFile->cd();
    m_outputFile->mkdir("engRecSpect");
    m_outputFile->cd("engRecSpect");
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          if(m_engRecSpect[i_coll][i_ener][i_eta]) m_engRecSpect[i_coll][i_ener][i_eta]->Write();
        } // i_eta
      } // i_ener
    } // i_coll
    // pione energy spectra
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        if(m_engPionSpect[i_ener][i_eta]) m_engPionSpect[i_ener][i_eta]->Write();
      } // i_eta
    } // i_ener
  } // m_doEngRecSpect

  // noise clusters spectra
  if(m_doEngNoiseClus) {
    m_outputFile->cd();
    m_outputFile->mkdir("engNoiseClus");
    m_outputFile->cd("engNoiseClus");
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        if(m_engNoiseClusSpect[i_coll][i_eta]) m_engNoiseClusSpect[i_coll][i_eta]->Write();
      } // i_eta
    } // i_coll
    // average noise
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      if(m_engNoiseClus_vs_eta[i_coll]) m_engNoiseClus_vs_eta[i_coll] -> Write();
    }
  } // m_doEngRecSpect

  if(m_doClusMoments) {
    m_outputFile->cd();
    m_outputFile->mkdir("clusMoment");
    m_outputFile->cd("clusMoment");
    for(int i_mom=0; i_mom<m_nmoments; i_mom++){
      for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          if(m_clusMoment_vs_eta[i_mom][i_sum][i_ener]) m_clusMoment_vs_eta[i_mom][i_sum][i_ener]->Write();
        } // i_ener
      } // i_sum
    } // i_mom
    for(int i_mom=0; i_mom<m_nmoments; i_mom++){
      for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          if(m_clusMoment_vs_ebeam[i_mom][i_sum][i_eta]) m_clusMoment_vs_ebeam[i_mom][i_sum][i_eta]->Write();
        } // i_eta
      } // i_sum
    } // i_mom

  } // m_doClusMoments

  if(m_doRecoEfficiency) {
    m_outputFile->cd();
    m_outputFile->mkdir("RecoEff");
    m_outputFile->cd("RecoEff");
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      if(m_RecoEfficiency_vs_eta[0][i_ener] && m_RecoEfficiency_vs_eta[1][i_ener]) {
        m_RecoEfficiency_vs_eta[0][i_ener]->Write();
        m_RecoEfficiency_vs_eta[1][i_ener]->Write();
      }
    } // i_eta
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      if(m_RecoEfficiency_vs_ebeam[0][i_eta] && m_RecoEfficiency_vs_ebeam[1][i_eta]) {
        m_RecoEfficiency_vs_ebeam[0][i_eta]->Write();
        m_RecoEfficiency_vs_ebeam[1][i_eta]->Write();
      }
    } // i_eta
  } // m_doRecoEfficiency

  /* **************************************************************************
  histograms to check calibration hits machinery
  *************************************************************************** */
  if(m_doCalibHitsValidation) {
    m_outputFile->cd();
    m_outputFile->mkdir("CalibHitsValidation");
    m_outputFile->cd("CalibHitsValidation");
    for(int i_ener=0; i_ener<m_nlogenerbin2; i_ener++){
      if(m_SumCalibHitOverEbeam_vs_eta[i_ener]) m_SumCalibHitOverEbeam_vs_eta[i_ener]->Write();
      if(m_DefaultCalculatorOverEbeam_vs_eta[i_ener]) m_DefaultCalculatorOverEbeam_vs_eta[i_ener]->Write();
      if(m_SumCalibHitAssignedOverEbeam_vs_eta[i_ener]) m_SumCalibHitAssignedOverEbeam_vs_eta[i_ener]->Write();
      if(m_SumCalibHitAssignedOverEbeam_vs_etaphi[i_ener]) m_SumCalibHitAssignedOverEbeam_vs_etaphi[i_ener]->Write();
    } // i_ener
//     for(int i_eta=0; i_eta<m_netabin; i_eta++){
//       if(m_SumCalibHitOverEbeam_vs_ebeam[i_eta]) m_SumCalibHitOverEbeam_vs_ebeam[i_eta]->Write();
// //       if(m_DefaultCalculatorOverEbeam_vs_ebeam[i_eta]) m_DefaultCalculatorOverEbeam_vs_ebeam[i_eta]->Write();
//     } // i_eta

  } // m_doCalibHitsValidation

  /* ********************************************
  writing tree with histogram parameters
  ******************************************** */
  m_outputFile->cd();
  m_outputFile->mkdir("Parameters");
  m_outputFile->cd("Parameters");
  TTree *tree = new TTree("ParamTree","ParamTree");
  tree->Branch("m_netabin",&m_netabin,"m_netabin/I");
  tree->Branch("m_etamin",&m_etamin,"m_etamin/F");
  tree->Branch("m_etamax",&m_etamax,"m_etamax/F");
  tree->Branch("m_nphibin",&m_nphibin,"m_nphibin/I");
  tree->Branch("m_phimin",&m_phimin,"m_phimin/F");
  tree->Branch("m_phimax",&m_phimax,"m_phimax/F");
  tree->Branch("m_nlogenerbin",&m_nlogenerbin,"m_nlogenerbin/I");
  tree->Branch("m_logenermin",&m_logenermin,"m_logenermin/F");
  tree->Branch("m_logenermax",&m_logenermax,"m_logenermax/F");
  tree->Branch("m_nnormtype",&m_nnormtype,"m_nnormtype/I");
  tree->Branch("m_ncluscoll",&m_ncluscoll,"m_ncluscoll/I");
  tree->Branch("m_ntagcases",&m_ntagcases,"m_ntagcases/I");
  tree->Branch("m_nmoments",&m_nmoments,"m_nmoments/I");
  tree->Branch("m_nmomsums",&m_nmomsums,"m_nmomsums/I");
  tree->Fill();
  tree->Write();

  m_outputFile->Close();

  return StatusCode::SUCCESS;
}



/* ****************************************************************************

**************************************************************************** */
StatusCode GetLCSinglePionsPerf::execute()
{ 
  /* ********************************************
  reading TBEventInfo
  ******************************************** */
  const TBEventInfo* theTBEventInfo = 0;
  if(m_isTestbeam) {
    ATH_CHECK( evtStore()->retrieve(theTBEventInfo,"TBEventInfo") );
  }

  /* ********************************************
  reading particles
  ******************************************** */
  const McEventCollection* truthEvent=0;
  ATH_CHECK( evtStore()->retrieve(truthEvent, "TruthEvent") );
  if( truthEvent->at(0)->particles_empty() ){
    ATH_MSG_ERROR( "No particles in McEventCollection" );
    return StatusCode::FAILURE;
  }
  // primary particle info
  HepMC::GenEvent::particle_const_iterator pit  = truthEvent->at(0)->particles_begin();
  const HepMC::GenParticle *gen = (*pit);
  m_mc_eta = gen->momentum().pseudoRapidity();
  m_mc_phi = gen->momentum().phi();
  m_mc_ener = gen->momentum().e();

  if(m_isTestbeam) {
    if( theTBEventInfo->getTableY() < m_ytable_min || theTBEventInfo->getTableY() > m_ytable_max
     || theTBEventInfo->getCryoX() < m_xcryo_min || theTBEventInfo->getCryoX() >= m_xcryo_max) return StatusCode::SUCCESS;

    double x0 = gen->production_vertex()->position().x();
    double y0 = gen->production_vertex()->position().y();
    //double theta = gen->momentum().theta();
    // to calculate ATKAS-like eta, phi coordinates of incident particle
    as_in_atlas(m_mc_eta, m_mc_phi, x0, y0, (double)theTBEventInfo->getCryoX());

  }

  // energy and eta bins for particle
  m_mc_enerbin = int( (log10(m_mc_ener) - m_logenermin)/m_dlogener);
  m_mc_etabin = int((fabs(m_mc_eta)-m_etamin)/m_deta);
  m_mc_phibin = int((m_mc_phi-m_phimin)/m_dphi);

  if(m_mc_etabin <0 || m_mc_etabin >= m_netabin || m_mc_enerbin <0 || m_mc_enerbin >= m_nlogenerbin || m_mc_phibin !=0) return StatusCode::SUCCESS;

  ATH_CHECK(  evtStore()->retrieve(m_clusColl, m_clusterBasicCollName ) );
  if(m_clusColl->size() == 0)  {
    ATH_MSG_WARNING( "Empty ClusterContainer "  <<  m_clusterBasicCollName  );
    return StatusCode::SUCCESS;
  }

  // check that we have at least one meaningfull cluster
  if(m_doRecoEfficiency) {
    bool PionWasReconstructed = false;
    float engClusCalibThs = 1.0*MeV;
    HepLorentzVector hlv_pion(1,0,0,1);
    hlv_pion.setREtaPhi(1./cosh(m_mc_eta),m_mc_eta,m_mc_phi);
    xAOD::CaloClusterContainer::const_iterator clusIter = m_clusColl->begin();
    xAOD::CaloClusterContainer::const_iterator clusIterEnd = m_clusColl->end();
    for( ;clusIter!=clusIterEnd;clusIter++) {
      const xAOD::CaloCluster * theCluster = (*clusIter); // this collection has cluster moments
// 
      double mx_calib_tot;
      if( !theCluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, mx_calib_tot) ) {
        ATH_MSG_ERROR( "Moment ENG_CALIB_TOT is absent"   );
      }
      double mx_dens, mx_center_lambda;
      if (!theCluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA, mx_center_lambda))
        mx_center_lambda = 0;
      if (!theCluster->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS, mx_dens))
        mx_dens = 0;

      HepLorentzVector hlv_cls(1,0,0,1);
      hlv_cls.setREtaPhi(1./cosh(theCluster->eta()), theCluster->eta(), theCluster->phi());
      double r = hlv_pion.angle(hlv_cls.vect());
      if(r < m_distance_cut*angle_mollier_factor(m_mc_eta) 
        && mx_calib_tot > engClusCalibThs
        && theCluster->size() > 1
        && mx_dens > 0
        && mx_center_lambda != 0.0 ) {
        PionWasReconstructed = true;
        break;
      }
    }
    if(PionWasReconstructed){
      m_RecoEfficiency_vs_ebeam[0][m_mc_etabin]->Fill(m_mc_ener);
      m_RecoEfficiency_vs_eta[0][m_mc_enerbin]->Fill(m_mc_eta);
    }
    m_RecoEfficiency_vs_ebeam[1][m_mc_etabin]->Fill(m_mc_ener);
    m_RecoEfficiency_vs_eta[1][m_mc_enerbin]->Fill(m_mc_eta);
    if(m_useRecoEfficiency && !PionWasReconstructed) return StatusCode::SUCCESS;
  }

  if(m_doEngRecSpect) {
    m_engPionSpect[m_mc_enerbin][m_mc_etabin]->Fill(m_mc_ener);
  }

  if(m_doEngRecOverTruth || m_doEngTag || m_doEngRecSpect || m_doEngNoiseClus){
    fill_reco();
  }

  if(m_doClusMoments) {
    fill_moments();
  }

  if(m_doCalibHitsValidation) {
    fill_calibhits();
  }

  return StatusCode::SUCCESS;
}



/* ****************************************************************************
+ average reconstructed energy after different steps of calibration, energy spectras
+ classification performance
+ noise clusters
**************************************************************************** */
int GetLCSinglePionsPerf::fill_reco()
{
  /* ********************************************
  reading cluster collections for each calibration level
  ******************************************** */
  std::vector<const xAOD::CaloClusterContainer *> clusCollVector;
  for(std::vector<std::string >::iterator it=m_clusterCollNames.begin(); it!=m_clusterCollNames.end(); it++){    
    const DataHandle<xAOD::CaloClusterContainer> pColl;
    ATH_CHECK( evtStore()->retrieve(pColl, (*it) ), -1 );
    clusCollVector.push_back(pColl);
  }

  HepLorentzVector hlv_pion(1,0,0,1);
  hlv_pion.setREtaPhi(1./cosh(m_mc_eta),m_mc_eta,m_mc_phi);

  std::vector<double > engClusSum; // for 4 collections
  engClusSum.resize(m_ncluscoll, 0.0);
  std::vector<double > engClusSumPresOnly; // for 4 collections
  engClusSumPresOnly.resize(m_ncluscoll, 0.0);

  double  engClusSumCalib(0);
  double  engClusSumCalibPresOnly(0);
  double  engClusSumTrueOOC(0);
  double  engClusSumTrueDM(0);
  std::vector<double> engClusSumCalibTagged;
  engClusSumCalibTagged.resize(m_ntagcases, 0.0);

  int nGoodClus = 0;
  xAOD::CaloClusterContainer::const_iterator clusIter = m_clusColl->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = m_clusColl->end();
  unsigned int iClus = 0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    const xAOD::CaloCluster * theCluster = (*clusIter); // this collection has cluster moments

    double mx_calib_tot;
    if( !theCluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, mx_calib_tot) ) {
      ATH_MSG_ERROR( "Moment ENG_CALIB_TOT is absent"   );
    }
    double mx_calib_emb0 = 0;
    double mx_calib_eme0 = 0;
    double mx_calib_tileg3 = 0;
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EMB0, mx_calib_emb0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EME0, mx_calib_eme0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TILEG3, mx_calib_tileg3)){
      ATH_MSG_ERROR( "One of the moment ENG_CALIB_EMB0, ENG_CALIB_EME0, ENG_CALIB_TILEG3 is absent"   );
    }
    double mx_calib_out = 0;
    double mx_calib_dead_tot = 0;
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_OUT_L, mx_calib_out)
        || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_DEAD_TOT, mx_calib_dead_tot) ){
      ATH_MSG_ERROR( "One of the moment ENG_CALIB_OUT_L, ENG_CALIB_DEAD_TOT is absent"   );
    }

    double mx_dens, mx_center_lambda;
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA, mx_center_lambda))
      mx_center_lambda = 0;
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS, mx_dens))
      mx_dens = 0;
    
    bool clusIsGood(true);//Fabian
    if(m_useGoodClus){ 
      if(theCluster->size() <= 1 || mx_dens <=0 || mx_center_lambda ==0 || ( m_usePionClusters && mx_calib_tot <= m_truthPiEngFraction * m_mc_ener )) {
        clusIsGood = false;
      }
    }

    HepLorentzVector hlv_cls(1,0,0,1);
    hlv_cls.setREtaPhi(1./cosh(theCluster->eta()), theCluster->eta(), theCluster->phi());
    double r = hlv_pion.angle(hlv_cls.vect());

    if((!m_usePionClusters && r < m_distance_cut*angle_mollier_factor(m_mc_eta)) ||
       ( ( m_usePionClusters && r < m_distance_cut*angle_mollier_factor(m_mc_eta)) && clusIsGood ) ) {

      theCluster = clusCollVector[kTOPO]->at(iClus); // this collection has RecoStatus
      CaloRecoStatus recoStatus = theCluster->recoStatus();

      if(recoStatus.checkStatus(CaloRecoStatus::TAGGEDEM) ) {
        engClusSumCalibTagged[kTAGEM] += mx_calib_tot;
      }else if(recoStatus.checkStatus(CaloRecoStatus::TAGGEDHAD) ) {
        engClusSumCalibTagged[kTAGHAD] += mx_calib_tot;
      }else if(recoStatus.checkStatus(CaloRecoStatus::TAGGEDUNKNOWN) ) {
        engClusSumCalibTagged[kTAGUNK] += mx_calib_tot;
      }else{
        //std::cout << "CheckSinglePionsReco::execute() -> Error! Unkown classification status " << recoStatus.getStatusWord()<< std::endl;
      }

      if(clusIsGood){
        nGoodClus++;
        engClusSumCalib += mx_calib_tot;
        engClusSumCalibPresOnly += (mx_calib_emb0+mx_calib_eme0+mx_calib_tileg3);
        engClusSumTrueOOC += mx_calib_out;
        engClusSumTrueDM += mx_calib_dead_tot;
        for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
          theCluster = clusCollVector[i_coll]->at(iClus);
          engClusSum[i_coll] += theCluster->e();
          engClusSumPresOnly[i_coll] += (theCluster->eSample(CaloSampling::PreSamplerB)+theCluster->eSample(CaloSampling::PreSamplerE)+theCluster->eSample(CaloSampling::TileGap3));
        }
      }

    }else{
      if(m_doEngNoiseClus){
        // to save noise clusters
        for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
          theCluster = clusCollVector[i_coll]->at(iClus);
          m_engNoiseClusSpect[i_coll][m_mc_etabin]->Fill(theCluster->e());
          m_engNoiseClus_vs_eta[i_coll]->Fill(theCluster->eta(), theCluster->e());
        }
      } // m_doEngNoiseClus
    } // r
    // xxx
  } // iclus

  if(nGoodClus == 0) return 0;

  /* ********************************************
  filling histograms for ereco over truth after each step of calibration
  ******************************************** */
  if(m_doEngRecOverTruth){
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++) {
      // with respect to the initial pion energy
      int i_norm = 0;
      double enom = engClusSum[i_coll];
      if(i_coll != kTOPO) {
        enom = enom - engClusSumPresOnly[i_coll];
      }
      if(m_mc_ener != 0.0) {
        m_engRecOverTruth_vs_eta[i_coll][i_norm][m_mc_enerbin]->Fill(m_mc_eta, enom/m_mc_ener);
        m_engRecOverTruth_vs_ebeam[i_coll][i_norm][m_mc_etabin]->Fill(m_mc_ener, enom/m_mc_ener);
      }
      // with respect to the calibration level
      i_norm = 1;
      double edenom = engClusSumCalib - engClusSumCalibPresOnly; // for em or had
      if(i_coll == kTOPO_OOC){ // ooc
        edenom += engClusSumTrueOOC;
      }else if(i_coll == kTOPO){ // dm
        edenom += (engClusSumTrueOOC + engClusSumTrueDM);
      }
      if(edenom != 0.0) {
        m_engRecOverTruth_vs_eta[i_coll][i_norm][m_mc_enerbin]->Fill(m_mc_eta, enom/edenom);
        m_engRecOverTruth_vs_ebeam[i_coll][i_norm][m_mc_etabin]->Fill(m_mc_ener, enom/edenom);
      }
      // with respect to the calibration level with "ideal" previous step
      i_norm = 2;
      enom = engClusSum[i_coll] - engClusSumPresOnly[i_coll];
      if(i_coll == kTOPO_OOC){ // ooc
        double engClusSumRecoOOC = engClusSum[kTOPO_OOC] - engClusSum[kTOPO_W];
        enom = engClusSumCalib - engClusSumCalibPresOnly + engClusSumRecoOOC;
      }else if(i_coll == kTOPO){ // dm
        double engClusSumRecoDM = engClusSum[kTOPO] - (engClusSum[kTOPO_OOC]-engClusSumPresOnly[kTOPO_OOC]);
        enom = (engClusSumCalib - engClusSumCalibPresOnly) + engClusSumTrueOOC + engClusSumRecoDM;
      }
      if(edenom != 0.0) {
        m_engRecOverTruth_vs_eta[i_coll][i_norm][m_mc_enerbin]->Fill(m_mc_eta, enom/edenom);
        m_engRecOverTruth_vs_ebeam[i_coll][i_norm][m_mc_etabin]->Fill(m_mc_ener, enom/edenom);
      }
    } // i_coll
  } // m_doEngRecOverTruth

  /* ********************************************
  filling histograms with classification results
  ******************************************** */
  if(m_doEngTag) {
    if(engClusSumCalib!=0.0) {
      const double inv_engClusSumCalib = 1. / engClusSumCalib;
      for(int i_tag=0; i_tag<m_ntagcases; i_tag++) {
        m_engTag_vs_eta[i_tag][m_mc_enerbin]->Fill(m_mc_eta, engClusSumCalibTagged[i_tag]*inv_engClusSumCalib);
        m_engTag_vs_ebeam[i_tag][m_mc_etabin]->Fill(m_mc_ener, engClusSumCalibTagged[i_tag]*inv_engClusSumCalib);
      }
    }
  } // m_doEngTag

  /* ********************************************
  filling histograms for reco energy spectras
  ******************************************** */
  if(m_doEngRecSpect){
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      double enom = engClusSum[i_coll];
      if(i_coll != kTOPO) {
        enom = enom - engClusSumPresOnly[i_coll];
      }
      m_engRecSpect[i_coll][m_mc_enerbin][m_mc_etabin]->Fill(enom/m_mc_ener);
    }
  } // m_doEngRecSpect

  return 0;
}



/* ****************************************************************************
to check moments assignment
**************************************************************************** */
int GetLCSinglePionsPerf::fill_moments()
{
  /* ********************************************
  reading calibration containers
  ******************************************** */
  const DataHandle<CaloCalibrationHitContainer> cchc;
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  std::vector<std::string>::iterator iter;
  for (iter=m_CalibrationHitContainerNames.begin(); iter!=m_CalibrationHitContainerNames.end();iter++) {
    ATH_CHECK( evtStore()->retrieve(cchc,*iter), -1 );
    v_cchc.push_back(cchc);
  }

  std::vector<const CaloCalibrationHitContainer *> v_dmcchc;
  for (iter=m_DMCalibrationHitContainerNames.begin();iter!=m_DMCalibrationHitContainerNames.end();iter++) {
    ATH_CHECK(  evtStore()->retrieve(cchc,*iter), -1 );
    v_dmcchc.push_back(cchc);
  }

  // calculate total calibration energy int active+inactive hits
  double engCalibTot = 0.0;
  std::vector<double > engCalibTotSmp;
  engCalibTotSmp.resize(CaloSampling::Unknown, 0.0);
  std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
  for (it=v_cchc.begin();it!=v_cchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    //loop over cells in calibration container
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      if(!myId.is_valid()) {
        std::cout << "Error! Bad identifier " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      if(!m_id_helper->is_lar(myId) && !m_id_helper->is_tile(myId)) {
        std::cout << "Error! Bad identifier (nor lar or tile) " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }

      CaloSampling::CaloSample nsmp = CaloSampling::CaloSample(m_calo_id->calo_sample(myId));
      engCalibTot += (*chIter)->energyTotal();
      engCalibTotSmp[nsmp] += (*chIter)->energyTotal();
    }
  }


  double engCalibDeadTot = 0.0;
  std::vector<double > engCalibDeadTotInArea;
  engCalibDeadTotInArea.resize(CaloDmDescrArea::DMA_MAX,0.0);
  for (it=v_dmcchc.begin();it!=v_dmcchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    //loop over cells in calibration container
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      if(!myId.is_valid()) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Bad dead material identifier " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      if(!m_id_helper->is_lar_dm(myId) && !m_id_helper->is_tile_dm(myId)) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Bad dead material identifier (nor lar_dm or tile_dm) " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      engCalibDeadTot += (*chIter)->energyTotal();
      //
      CaloDmDescrElement* myCDDE(0);
      myCDDE = m_caloDmDescrManager->get_element(myId);
      if( !myCDDE ) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Absent DM description element!" << std::endl;
        continue;
      }
      int nDmArea = m_caloDmDescrManager->get_dm_area(myId);
      engCalibDeadTotInArea[nDmArea] += (*chIter)->energyTotal();
      engCalibDeadTotInArea[CaloDmDescrArea::DMA_ALL] += (*chIter)->energyTotal();
    }
  }
//   double engCalibDeadTotWithPres = engCalibDeadTot + engCalibTotSmp[CaloSampling::PreSamplerB]
//         + engCalibTotSmp[CaloSampling::PreSamplerE]
//         + engCalibTotSmp[CaloSampling::TileGap3];

  /* ********************************************
  reading clusters moments
  ******************************************** */
  double  engClusSumCalib(0);
  double  engClusSumCalibPresOnly(0);
  std::vector<std::vector<double > > clsMoments; // [iClus][m_nmoments]
  clsMoments.resize(m_clusColl->size());
  std::vector<double > clsMomentsSum; // [m_nmoments]
  clsMomentsSum.resize(m_nmoments,0.0);

  // retrieving cluster moments
  xAOD::CaloClusterContainer::const_iterator clusIter = m_clusColl->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = m_clusColl->end();
  unsigned int iClus = 0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    const xAOD::CaloCluster * theCluster = (*clusIter);
    clsMoments[iClus].resize(m_validMoments.size(),0.0);
    int iMoment=0;
    for(moment_name_vector::const_iterator im=m_validMoments.begin(); im!=m_validMoments.end(); im++, iMoment++){
      double mx;
      if( theCluster->retrieveMoment((*im).second, mx) ) {
        clsMoments[iClus][iMoment] = mx;
      }else{
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Can't retrieve moment " << (*im).first << " " << (*im).second << std::endl;
      }
      clsMomentsSum[iMoment] += clsMoments[iClus][iMoment];
    }
    double mx;
    if(theCluster->retrieveMoment( xAOD::CaloCluster::ENG_CALIB_TOT, mx ) ){
      engClusSumCalib += mx;
    }else{
      std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Can't retrieve moment xAOD::CaloCluster::ENG_CALIB_TOT " << std::endl;
    }
    double mx_calib_emb0, mx_calib_eme0, mx_calib_tileg3;
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EMB0, mx_calib_emb0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EME0, mx_calib_eme0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TILEG3, mx_calib_tileg3)){
      std::cout << "One of the moment ENG_CALIB_EMB0, ENG_CALIB_EME0, ENG_CALIB_TILEG3 is absent"  << std::endl;
    }else{
      engClusSumCalibPresOnly += (mx_calib_emb0+mx_calib_eme0+mx_calib_tileg3);
    }
  } // iClus

  double engCalibDeadTotWithPres = engCalibDeadTot + engClusSumCalibPresOnly;

  // DM energy before barrel presampler, inside it, and between presampler and strips
  double eng_calib_dead_emb0 = engCalibDeadTotInArea[CaloDmDescrArea::DMA_EMB0]
        + engCalibDeadTotInArea[CaloDmDescrArea::DMA_EMB1]
        + engCalibTotSmp[CaloSampling::PreSamplerB];
  // DM energy between barrel and tile
  double eng_calib_dead_tile0 = engCalibDeadTotInArea[CaloDmDescrArea::DMA_EMB3_TILE0];
  // DM energy before scintillator and inside scintillator
  double eng_calib_dead_tileg3 = engCalibDeadTotInArea[CaloDmDescrArea::DMA_SCN]
        + engCalibTotSmp[CaloSampling::TileGap3];
  // DM energy beforee endcap presampler, inside it and between presampler and strips
  double eng_calib_dead_eme0 = engCalibDeadTotInArea[CaloDmDescrArea::DMA_EME0]
        + engCalibDeadTotInArea[CaloDmDescrArea::DMA_EME12]
        + engCalibTotSmp[CaloSampling::PreSamplerE];
  // DM energy between emec and hec
  double eng_calib_dead_hec0 = engCalibDeadTotInArea[CaloDmDescrArea::DMA_EME3_HEC0];
  // DM energy before FCAL and between HEC and FCAL
  double eng_calib_dead_fcal = engCalibDeadTotInArea[CaloDmDescrArea::DMA_FCAL0]
        + engCalibDeadTotInArea[CaloDmDescrArea::DMA_HEC_FCAL];
  // DM leakage behind the calorimeter
  double eng_calib_dead_leakage = engCalibDeadTotInArea[CaloDmDescrArea::DMA_LEAK];
  // the rest of DM energy which remains unclassified
  double eng_calib_dead_unclass = engCalibDeadTotWithPres - eng_calib_dead_emb0 - eng_calib_dead_tile0
        - eng_calib_dead_tileg3 - eng_calib_dead_eme0 - eng_calib_dead_hec0 - eng_calib_dead_fcal
        - eng_calib_dead_leakage;

  // filling histograms
  int iMoment=0;
  for(moment_name_vector::const_iterator im=m_validMoments.begin(); im!=m_validMoments.end(); im++, iMoment++){
    double xnorm = 0.0;
    switch ( (*im).second ) {
      case xAOD::CaloCluster::ENG_CALIB_TOT:
        xnorm = clsMomentsSum[iMoment];
        break;
      case xAOD::CaloCluster::ENG_CALIB_OUT_T:
        xnorm = engCalibTot - engClusSumCalib;
        break;
      case xAOD::CaloCluster::ENG_CALIB_OUT_L:
        xnorm = engCalibTot - engClusSumCalib;
        break;
      case xAOD::CaloCluster::ENG_CALIB_OUT_M:
        xnorm = engCalibTot - engClusSumCalib;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_TOT:
        xnorm = engCalibDeadTotWithPres;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0:
        xnorm = eng_calib_dead_emb0;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0:
        xnorm = eng_calib_dead_tile0;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3:
        xnorm = eng_calib_dead_tileg3;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_EME0:
        xnorm = eng_calib_dead_eme0;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0:
        xnorm = eng_calib_dead_hec0;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL:
        xnorm = eng_calib_dead_fcal;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE:
        xnorm = eng_calib_dead_leakage;
        break;
      case xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS:
        xnorm = eng_calib_dead_unclass;
        break;
      default:
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Not implemented for " << (*im).first << " " << (*im).second << std::endl;
        break;
    }
    if(m_doClusMoments && xnorm > m_mc_ener*0.0001) {
      // moments assigned to first 3 maximum clusters
      const double inv_xnorm = 1. / xnorm;
      for(unsigned int i_cls=0; i_cls<m_clusColl->size(); i_cls++){
        m_clusMoment_vs_eta[iMoment][i_cls][m_mc_enerbin]->Fill(m_mc_eta, clsMoments[i_cls][iMoment]*inv_xnorm);
        m_clusMoment_vs_ebeam[iMoment][i_cls][m_mc_etabin]->Fill(m_mc_ener, clsMoments[i_cls][iMoment]*inv_xnorm);
      if(i_cls>=2) break;
      }
      // sum of moments assigned to clusters wrt total sum of moment
      m_clusMoment_vs_eta[iMoment][kMOM_SUMCLS][m_mc_enerbin]->Fill(m_mc_eta, clsMomentsSum[iMoment]/xnorm);
      m_clusMoment_vs_ebeam[iMoment][kMOM_SUMCLS][m_mc_etabin]->Fill(m_mc_ener, clsMomentsSum[iMoment]/xnorm);
      // unassigned energy wrt pion energy
      m_clusMoment_vs_eta[iMoment][kMOM_MISS][m_mc_enerbin]->Fill(m_mc_eta, (xnorm-clsMomentsSum[iMoment])/m_mc_ener);
      m_clusMoment_vs_ebeam[iMoment][kMOM_MISS][m_mc_etabin]->Fill(m_mc_ener, (xnorm-clsMomentsSum[iMoment])/m_mc_ener);
      // unassigned energy wrt pion energy
      m_clusMoment_vs_eta[iMoment][kMOM_MCENER][m_mc_enerbin]->Fill(m_mc_eta, xnorm/m_mc_ener);
      m_clusMoment_vs_ebeam[iMoment][kMOM_MCENER][m_mc_etabin]->Fill(m_mc_ener, xnorm/m_mc_ener);
    }
  } // i_mom

  return 0;
}



/* ****************************************************************************
calibration hits validation
**************************************************************************** */
int GetLCSinglePionsPerf::fill_calibhits()
{
  /* ********************************************
  reading calibration containers
  ******************************************** */
  const DataHandle<CaloCalibrationHitContainer> cchc;
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  std::vector<std::string>::iterator iter;
  for (iter=m_CalibrationHitContainerNames.begin(); iter!=m_CalibrationHitContainerNames.end();iter++) {
    ATH_CHECK( evtStore()->retrieve(cchc,*iter), -1 );
    v_cchc.push_back(cchc);
  }

  std::vector<const CaloCalibrationHitContainer *> v_dmcchc;
  for (iter=m_DMCalibrationHitContainerNames.begin();iter!=m_DMCalibrationHitContainerNames.end();iter++) {
    ATH_CHECK( evtStore()->retrieve(cchc,*iter), -1 );
    v_dmcchc.push_back(cchc);
  }

  // calculate total calibration energy int active+inactive hits
  double engCalibTot = 0.0;
  std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
  for (it=v_cchc.begin();it!=v_cchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    //loop over cells in calibration container
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      if(!myId.is_valid()) {
        std::cout << "Error! Bad identifier " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      if(!m_id_helper->is_lar(myId) && !m_id_helper->is_tile(myId)) {
        std::cout << "Error! Bad identifier (nor lar or tile) " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      engCalibTot += (*chIter)->energyTotal();
    }
  }

  // calibration energy in dead material
  double engCalibDeadTot = 0.0;
  double engDefaultCalculator = 0.0;
  for (it=v_dmcchc.begin();it!=v_dmcchc.end();it++) {
    CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
    CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
    //loop over cells in calibration container
    for(;chIter!=chIterE;chIter++)  {
      Identifier myId = (*chIter)->cellID();
      if(!myId.is_valid()) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Bad dead material identifier " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      if(!m_id_helper->is_lar_dm(myId) && !m_id_helper->is_tile_dm(myId)) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Bad dead material identifier (nor lar_dm or tile_dm) " << myId << " in container '" << (*it)->Name() << "',"
        << " AtlasDetectorID says '" << m_id_helper->show_to_string(myId) << "'" << std::endl;
        continue;
      }
      engCalibDeadTot += (*chIter)->energyTotal();
      //
      CaloDmDescrElement* myCDDE(0);
      myCDDE = m_caloDmDescrManager->get_element(myId);
      if( !myCDDE ) {
        std::cout << "GetLCSinglePionsPerf::fill_moments() -> Error! Absent DM description element!" << std::endl;
        continue;
      }
      int nDmArea = m_caloDmDescrManager->get_dm_area(myId);
      if(nDmArea == CaloDmDescrArea::DMA_DEFCALC)  engDefaultCalculator += (*chIter)->energyTotal();
    }
  }

  // calibration energy assigned to one or another cluster
  // retrieving cluster moments
  xAOD::CaloClusterContainer::const_iterator clusIter = m_clusColl->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = m_clusColl->end();
  double engCalibAssigned = 0.0;
  double engClusSumCalibPresOnly = 0.0;
  unsigned int iClus = 0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    const xAOD::CaloCluster * theCluster = (*clusIter);

    double mx_calib_tot, mx_calib_ooc, mx_calib_dm;
//     if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT, mx_calib_tot)
//          || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_OUT_L, mx_calib_ooc)
//          || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_DEAD_TOT mx_calib_dm)){
//       std::cout << "One of the moment ENG_CALIB_TOT, ENG_CALIB_OUT_L, ENG_CALIB_DEAD_TOT is absent"  << std::endl;
//     }
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT, mx_calib_tot) ){
      std::cout << "Moment ENG_CALIB_TOT is absent"  << std::endl;
    }
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_OUT_L, mx_calib_ooc) ){
      std::cout << "Moment ENG_CALIB_OUT_L is absent"  << std::endl;
    }
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_DEAD_TOT, mx_calib_dm) ){
      std::cout << "Moment ENG_CALIB_DEAD_TOT is absent"  << std::endl;
    }
    engCalibAssigned += (mx_calib_tot + mx_calib_ooc + mx_calib_dm);

    double mx_calib_emb0, mx_calib_eme0, mx_calib_tileg3;
    if( !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EMB0, mx_calib_emb0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_EME0, mx_calib_eme0)
         || !theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TILEG3, mx_calib_tileg3)){
      std::cout << "One of the moment ENG_CALIB_EMB0, ENG_CALIB_EME0, ENG_CALIB_TILEG3 is absent"  << std::endl;
    }else{
      engClusSumCalibPresOnly += (mx_calib_emb0+mx_calib_eme0+mx_calib_tileg3);
    }

  } // iClus
  // energy in presamplers is included already in the dead material
  engCalibAssigned -= engClusSumCalibPresOnly;

  int n_enerbin = int( (log10(m_mc_ener) - m_logenermin)/ ((m_logenermax - m_logenermin)/m_nlogenerbin2));
  // sum of moments assigned to clusters wrt total sum of moment
  m_SumCalibHitOverEbeam_vs_eta[n_enerbin]->Fill(m_mc_eta, (engCalibTot+engCalibDeadTot)/m_mc_ener);
//   m_SumCalibHitOverEbeam_vs_ebeam[m_mc_etabin]->Fill(m_mc_ener, (engCalibTot+engCalibDeadTot)/m_mc_ener);
  m_SumCalibHitAssignedOverEbeam_vs_eta[n_enerbin]->Fill(m_mc_eta, engCalibAssigned/m_mc_ener);
  m_SumCalibHitAssignedOverEbeam_vs_etaphi[n_enerbin]->Fill(m_mc_eta, m_mc_phi, engCalibAssigned/m_mc_ener);

  m_DefaultCalculatorOverEbeam_vs_eta[n_enerbin]->Fill(m_mc_eta, engDefaultCalculator/m_mc_ener);
//   m_DefaultCalculatorOverEbeam_vs_ebeam[m_mc_etabin]->Fill(m_mc_ener, engDefaultCalculator/m_mc_ener);

  return 0;
}



/* ****************************************************************************

**************************************************************************** */
double GetLCSinglePionsPerf::angle_mollier_factor(double x)
{
  double eta = fabs(x);
  double ff;
  if(eta<1.6){
    ff = atan(5.0*1.7/(200.0*cosh(eta)));
  }else if(eta<3.2){
    ff = atan(5.0*1.6/(420./tanh(eta)));
  }else{
    ff = atan(5.0*0.95/(505./tanh(eta)));
  }
  return ff*(1./atan(5.0*1.7/200.0));
}



/* ****************************************************************************
convert incident particle eta,phi from testbeam to ATLAS-like values
**************************************************************************** */
void GetLCSinglePionsPerf::as_in_atlas ( double &eta,  double &phi, 
                    double X0,  double Y0,  double xCryo, int iCalo)
{
  static const int Ncalo = 3;
  static double Zcalo[Ncalo] = { 3691.0, 4329.5, 4668.5 };
  static double Z0 = -21600.;
  static double Alpha = 4.668*M_PI/180.; //  [degree]
  static double PhiSh = 45.*M_PI/180.;
  static double Eta0 = 2.8;
  static double z0emec = 11115;
  static double Yrun2 = 70;

  //  Get transformation parameters
  double Beta = 2*atan( exp(-Eta0) );
  double Zemec = Zcalo[0];
  double b = Zemec * tan(Beta);
  double Yrun1 = b*cos(Alpha) - Zemec*sin(Alpha);
  double z0 = z0emec + Yrun2*tan(Alpha) - b*sin(Alpha) - Zemec*cos(Alpha);
  double y0 = Yrun2 - Yrun1;
  double z0calo = z0emec + (Zcalo[iCalo]-Zcalo[0]) / cos (Alpha);

  //  Get interception point: track with calorimeter front face
  double ctga = 1./ tan(Alpha);
  //double tgth = tan(Theta);
  double tgth = tan(2*atan(exp(-eta)))*sin(phi);
  double zx = (Y0 - Z0*tgth + z0calo*ctga) / (ctga - tgth);
  double yx = (zx - z0calo) * ctga;
  double xx = X0 + xCryo;

  TVector3 vs(0,y0,z0);
  TVector3 vtb(xx,yx,zx);

  TVector3 vat = vtb - vs;
  vat.RotateX(-Alpha);

  eta = vat.PseudoRapidity();
  phi = vat.Phi() + PhiSh;
  return;
}

