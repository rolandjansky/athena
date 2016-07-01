/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*   NAME    : CscPrdValAlg.cxx
 *   PACKAGE : MuonRawDataMonitoring/CscRawDataMonitoring
 *   PURPOSE : CSC PrepRawData (PRD) monitoring
 *   AUTHOR  : Ken Johns, Xiaowen Lei (U.Arizona)
 *
 *   MODIFIED: N. Benekos(Illinois)
 *             V. Kaushik(U.Arizona) : 2009-05-12
 *             J. Veatch(U.Arizona)  : 2012-02-08
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Athena include(s)
#include "CscRawDataMonitoring/CscPrdValAlg.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include "CscClusterization/ICscStripFitter.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "xAODEventInfo/EventInfo.h"

// ROOT include(s)
#include "TClass.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace Muon;

namespace CscPrdBins {
  void PrdBinLabels(TH1 *h, int m_side) {
    h->GetXaxis()->SetTitle("");
    h->GetXaxis()->SetLabelSize(0.03);
    if(m_side == -1) {
      for(size_t j=6; j<86; j++) {
        if( j%5 != 0 ) {
          float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
          xmid -= 1.0; xmid *= -1.0;
          int seclay = int(ceil(xmid*10)); // get sector/layer
          int sec = seclay/10;
          int lay = (seclay%10)/2 ;
          lay = (4 - lay) + 1;
          h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
        } // end for
      } // end if
    } else if (m_side == 1) {
      for(size_t j=6; j<86; j++) {
        if( j%5 != 0 ) {
          float xmid = h->GetBinLowEdge(j) + h->GetBinWidth(j);
          int seclay = int(ceil(xmid*10)); // get sector/layer
          int sec = seclay/10;
          int lay = (seclay%10)/2 ;
          h->GetXaxis()->SetBinLabel(j,Form("%c%02d:%d",(sec%2==0?'S':'L'),sec,lay));
        }
      } // end for
    } // end else

  } // BinLabels

} // end namespace CscBins


//
// constructor ----------------------------------------------------------------
//
CscPrdValAlg::CscPrdValAlg(const std::string & type, const std::string & name, 
    const IInterface* parent) : 
  ManagedMonitorToolBase(type, name, parent),
  m_stripFitter(name),
  cscprd_oviewEA(0),
  cscprd_oviewEC(0)
{

  declareProperty("CSCStripFitter", m_stripFitter);
  declareProperty("CSCPrepRawDataPath", m_cscPRDPath = "Muon/MuonRawDataMonitoring/CSC/PRD");
  declareProperty("CSCPrepRawDataKey", m_cscPrdKey = "CSC_Measurements");
  declareProperty("NoiseCutADC", m_cscNoiseCut = 50);
  declareProperty("MapYXandRZ", m_mapxyrz = false);

  m_cscGenPath = m_cscPRDPath.substr(0,m_cscPRDPath.find("CSC"));

  initHistograms();

} // end CscPrdValAlg::CscPrdValAlg constructor


//
// destructor ----------------------------------------------------------------
//
CscPrdValAlg::~CscPrdValAlg() {
  if(cscprd_oviewEA) {
    delete cscprd_oviewEA;
    cscprd_oviewEA = 0;
  }
  if(cscprd_oviewEC) {
    delete cscprd_oviewEC;
    cscprd_oviewEC = 0;
  }
  ATH_MSG_DEBUG( "CscPrdValAlg: in destructor" );
}


//
// initialize ----------------------------------------------------------------
//
StatusCode CscPrdValAlg::initialize() {
  ATH_MSG_INFO( "CscPrdValAlg: in initialize" );

  ATH_MSG_DEBUG( "strip fitter " << m_stripFitter );

  StatusCode sc;

  sc = detStore()->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Cannot get CscIdHelper" );
    return sc; 
  }
  else {
    ATH_MSG_DEBUG( " Found the CscIdHelper. " );
  }

  //Fetch strip fitter
  sc = m_stripFitter.retrieve();
  if ( !m_stripFitter || sc.isFailure() ) {
    ATH_MSG_DEBUG( "CscPrdValAlg " << name() << ": unable to retrieve strip fitter " << m_stripFitter );
    return sc;
  } else {
    ATH_MSG_DEBUG( "CscPrdValAlg " << name() << ": retrieved " << m_stripFitter );
  }

  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc " );
    return sc ;
  }

  ManagedMonitorToolBase::initialize().ignore();  // Ignore the checking code;
  return StatusCode::SUCCESS;

} // end CscPrdValAlg::initialize()


//
// initHistograms ----------------------------------------------------------------
//
void CscPrdValAlg::initHistograms() {

  m_h2csc_prd_hitmap = 0;
  m_h2csc_prd_hitmap_signal = 0;
  m_h2csc_prd_hitmap_noise = 0;

  m_h2csc_prd_hitmap_norm = 0;
  m_h2csc_prd_hitmap_norm_signal = 0;
  m_h2csc_prd_hitmap_norm_noise = 0;

  m_h2csc_prd_hitmap_signal_EA = 0;
  m_h1csc_prd_hitmap_signal_EA_count = 0;
  m_h1csc_prd_hitmap_signal_EA_occupancy = 0;

  m_h2csc_prd_hitmap_norm_signal_EA = 0;

  m_h2csc_prd_hitmap_signal_EC = 0;
  m_h1csc_prd_hitmap_signal_EC_count = 0;
  m_h1csc_prd_hitmap_signal_EC_occupancy = 0;

  m_h2csc_prd_hitmap_norm_signal_EC = 0;

  m_h2csc_prd_occvslb_EA = 0;
  m_h2csc_prd_occvslb_EC = 0;

  m_h2csc_prd_eta_vs_phi_hitmap = 0;
  m_h2csc_prd_r_vs_z_hitmap = 0;
  m_h2csc_prd_y_vs_x_hitmap = 0;

  m_h2csc_prd_phicluswidth = 0;
  m_h2csc_prd_phicluswidth_signal = 0;
  m_h2csc_prd_phicluswidth_noise = 0;

  m_h2csc_prd_etacluswidth = 0;
  m_h2csc_prd_etacluswidth_signal = 0;
  m_h2csc_prd_etacluswidth_noise = 0;

  m_h2csc_prd_phicluscount = 0;
  m_h2csc_prd_phicluscount_signal = 0;
  m_h2csc_prd_phicluscount_noise = 0;

  m_h2csc_prd_etacluscount = 0;
  m_h2csc_prd_etacluscount_signal = 0;
  m_h2csc_prd_etacluscount_noise = 0;


  m_h1csc_prd_maxdiffamp = 0;

  // Correlation plots
  m_h2csc_prd_eta_vs_phi_cluscount = 0;
  m_h2csc_prd_eta_vs_phi_cluscount_signal = 0;
  m_h2csc_prd_eta_vs_phi_cluscount_noise = 0;

  m_h2csc_prd_eta_vs_phi_cluswidth = 0;
  m_h2csc_prd_eta_vs_phi_cluswidth_signal = 0;
  m_h2csc_prd_eta_vs_phi_cluswidth_noise = 0;

}

//
// bookPrdHistograms ----------------------------------------------------------------
//
void CscPrdValAlg::bookPrdHistograms() {

  m_regHShift.clear();
  m_regHExpert.clear();
  m_regHOviewEA.clear();
  m_regHOviewEC.clear();

  /// ******************** DO NOT MODIFY (begin) *********************************************** ///
  int nxbins  = 242;  // 192 bins for precision, 48 for transverse strips + 2 extra
  float nxmin = -49.; // -1 -> -48 (for transverse)
  float nxmax = 193.; // 1 -> 192 (for precision)

  int nybins  = 175;  // 32 chambers (16 per side x 5 layers per chamber) + 1 set extra
  float nymin = -17.; //
  float nymax = 18.;  //

  int nybinsEA  = 90;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEA = 0.;  //
  float nymaxEA = 18.; //

  int nybinsEC  = 85;   // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEC = -17.; //
  float nymaxEC = 0.;   //

  int nxbinsLB  = 2510; // 2500 LB (x 16 chambers x 5 layers per chamber) + 10 extra
  float nxminLB = -10.;   //
  float nxmaxLB = 2500.;  //
  /// ******************** DO NOT MODIFY (end) *********************************************** ///

  //if (newEventsBlock){}
  //if (newLumiBlock){}
  //if (newRun) {

  // book histograms
  //m_h1csc_prd_maxdiffamp = new TH1F("h1csc_prd_maxdiffamp", "Max Amplitude in ROD Cluster;ADC counts;;",500,0,5000);

  // strip hitmap
  m_h2csc_prd_hitmap = new TH2F("h2csc_prd_hitmap", "Hit Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_prd_hitmap_noise = new TH2F("h2csc_prd_hitmap_noise", "Noise Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_prd_hitmap_signal = new TH2F("h2csc_prd_hitmap_signal", "Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_prd_hitmap_norm = new TH2F("h2csc_prd_hitmap_norm", "Normalized Hit Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_prd_hitmap_norm_noise = new TH2F("h2csc_prd_hitmap_norm_noise", "Normalized Noise Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_prd_hitmap_norm_signal = new TH2F("h2csc_prd_hitmap_norm_signal", "Normalized Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_prd_hitmap_signal_EA = new TH2F("h2csc_prd_hitmap_signal_EA", "EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEA,nyminEA,nymaxEA);
  m_h1csc_prd_hitmap_signal_EA_count = new TH1F("h1csc_prd_hitmap_signal_EA_count", "EndCap A: Signal Occupancy;channel;entries/channel;",
      nxbins,nxmin,nxmax);
  m_h1csc_prd_hitmap_signal_EA_occupancy = new TH1F("h1csc_prd_hitmap_signal_EA_occupancy", "EndCap A: Signal Occupancy;[sector] + [0.2 #times layer];entries/layer",
      nybinsEA,nyminEA,nymaxEA);
  CscPrdBins::PrdBinLabels(m_h1csc_prd_hitmap_signal_EA_occupancy,1);

  m_h2csc_prd_hitmap_norm_signal_EA = new TH2F("h2csc_prd_hitmap_norm_signal_EA", "EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEA,nyminEA,nymaxEA);

  m_h2csc_prd_hitmap_signal_EC = new TH2F("h2csc_prd_hitmap_signal_EC", "EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEC,nyminEC,nymaxEC);
  m_h1csc_prd_hitmap_signal_EC_count = new TH1F("h1csc_prd_hitmap_signal_EC_count", "EndCap C: Signal Occupancy;channel;entries/channel;",
      nxbins,nxmin,nxmax);
  m_h1csc_prd_hitmap_signal_EC_occupancy = new TH1F("h1csc_prd_hitmap_signal_EC_occupancy", "EndCap C: Signal Occupancy;[sector] + [0.2 #times layer];entries/layer",
      nybinsEC,nyminEC,nymaxEC);
  CscPrdBins::PrdBinLabels(m_h1csc_prd_hitmap_signal_EC_occupancy,-1);

  m_h2csc_prd_hitmap_norm_signal_EC = new TH2F("h2csc_prd_hitmap_norm_signal_EC", "EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEC,nyminEC,nymaxEC);

  m_h2csc_prd_occvslb_EA = new TH2F("h2csc_prd_occvslb_EA", "EndCap A: Layer Signal Occupancy Per LB;LB;[sector] + [0.2 #times layer]",
      nxbinsLB,nxminLB,nxmaxLB,nybinsEA,nyminEA,nymaxEA);
  m_h2csc_prd_occvslb_EC = new TH2F("h2csc_prd_occvslb_EC", "EndCap C: Layer Signal Occupancy Per LB;LB;[sector] + [0.2 #times layer]",
      nxbinsLB,nxminLB,nxmaxLB,nybinsEC,nyminEC,nymaxEC);

  // PRD eta-cluster width
  m_h2csc_prd_etacluswidth = new TH2F("h2csc_prd_etacluswidth",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_prd_etacluswidth_noise = new TH2F("h2csc_prd_etacluswidth_noise",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_prd_etacluswidth_signal = new TH2F("h2csc_prd_etacluswidth_signal",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);

  // PRD phi-cluster width
  m_h2csc_prd_phicluswidth = new TH2F("h2csc_prd_phicluswidth",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);
  m_h2csc_prd_phicluswidth_noise = new TH2F("h2csc_prd_phicluswidth_noise",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);
  m_h2csc_prd_phicluswidth_signal = new TH2F("h2csc_prd_phicluswidth_signal",
      "PRD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);

  // PRD eta-cluster count
  m_h2csc_prd_etacluscount = new TH2F("h2csc_prd_etacluscount",
      "PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_prd_etacluscount_noise = new TH2F("h2csc_prd_etacluscount_noise",
      "PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_prd_etacluscount_signal = new TH2F("h2csc_prd_etacluscount_signal",
      "PRD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);

  // PRD phi-cluster count
  m_h2csc_prd_phicluscount = new TH2F("h2csc_prd_phicluscount",
      "PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_prd_phicluscount_noise = new TH2F("h2csc_prd_phicluscount_noise",
      "PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_prd_phicluscount_signal = new TH2F("h2csc_prd_phicluscount_signal",
      "PRD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);

  // correlation histograms
  m_h2csc_prd_eta_vs_phi_cluscount = new TH2F("h2csc_prd_eta_vs_phi_cluscount",
      "Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_prd_eta_vs_phi_cluscount_noise = new TH2F("h2csc_prd_eta_vs_phi_cluscount_noise",
      "Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_prd_eta_vs_phi_cluscount_signal = new TH2F("h2csc_prd_eta_vs_phi_cluscount_signal",
      "Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);

  m_h2csc_prd_eta_vs_phi_cluswidth = new TH2F("h2csc_prd_eta_vs_phi_cluswidth",
      "Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width",100,0,100,100,0,100);

  if(m_mapxyrz) {
    m_h2csc_prd_r_vs_z_hitmap = new TH2F("h2csc_prd_r_vs_z_hitmap",
        "R vs. Z Cluster hitmap;z(mm);R(mm)",200, -10000., 10000., 40, 0., 4000);

    m_h2csc_prd_y_vs_x_hitmap = new TH2F("h2csc_prd_y_vs_x_hitmap",
        "Y vs. X Cluster hitmap;x(mm);y(mm)",100, -5000., 5000., 100, -5000., 5000);
  }

  //m_regHShift.push_back(m_h1csc_prd_maxdiffamp);            // shift
  m_regHExpert.push_back(m_h2csc_prd_hitmap);                 // expert
  m_regHShift.push_back(m_h2csc_prd_hitmap_signal);           // shift, overview (dq-flag)
  m_regHExpert.push_back(m_h2csc_prd_hitmap_noise);           // expert

  m_regHExpert.push_back(m_h2csc_prd_hitmap_norm);                 // expert
  m_regHShift.push_back(m_h2csc_prd_hitmap_norm_signal);           // shift, overview (dq-flag)
  m_regHExpert.push_back(m_h2csc_prd_hitmap_norm_noise);           // expert

  m_regHOviewEA.push_back(m_h2csc_prd_hitmap_signal_EA);
  m_regHOviewEA.push_back(m_h1csc_prd_hitmap_signal_EA_count);
  m_regHOviewEA.push_back(m_h1csc_prd_hitmap_signal_EA_occupancy);

  m_regHOviewEA.push_back(m_h2csc_prd_hitmap_norm_signal_EA);

  m_regHOviewEC.push_back(m_h2csc_prd_hitmap_signal_EC);
  m_regHOviewEC.push_back(m_h1csc_prd_hitmap_signal_EC_count);
  m_regHOviewEC.push_back(m_h1csc_prd_hitmap_signal_EC_occupancy);

  m_regHOviewEC.push_back(m_h2csc_prd_hitmap_norm_signal_EC);

  m_regHOviewEA.push_back(m_h2csc_prd_occvslb_EA);
  m_regHOviewEC.push_back(m_h2csc_prd_occvslb_EC);

  m_regHExpert.push_back(m_h2csc_prd_etacluswidth);           // expert
  m_regHExpert.push_back(m_h2csc_prd_etacluswidth_signal);    // expert
  m_regHExpert.push_back(m_h2csc_prd_etacluswidth_noise);     // expert

  m_regHExpert.push_back(m_h2csc_prd_phicluswidth);           // expert
  m_regHExpert.push_back(m_h2csc_prd_phicluswidth_signal);    // expert
  m_regHExpert.push_back(m_h2csc_prd_phicluswidth_noise);     // expert

  m_regHExpert.push_back(m_h2csc_prd_etacluscount);           // expert
  m_regHShift.push_back(m_h2csc_prd_etacluscount_signal);     // shift
  m_regHExpert.push_back(m_h2csc_prd_etacluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_prd_phicluscount);           // expert
  m_regHShift.push_back(m_h2csc_prd_phicluscount_signal);     // shift
  m_regHExpert.push_back(m_h2csc_prd_phicluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_prd_eta_vs_phi_cluscount);           // expert
  m_regHExpert.push_back(m_h2csc_prd_eta_vs_phi_cluscount_signal);    // expert
  m_regHExpert.push_back(m_h2csc_prd_eta_vs_phi_cluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_prd_eta_vs_phi_cluswidth);       // expert
  //m_regHShift.push_back(m_h2csc_prd_eta_vs_phi_hitmap);         // shift
  if(m_mapxyrz) {
    m_regHShift.push_back(m_h2csc_prd_r_vs_z_hitmap);               // shift
    m_regHShift.push_back(m_h2csc_prd_y_vs_x_hitmap);               // shift
  }

  //}

}

//
// bookHistograms ----------------------------------------------------------------
//
StatusCode CscPrdValAlg::bookHistograms() {
  ATH_MSG_DEBUG( "CscPrdValAlg: in bookHistograms" );

  StatusCode sc = StatusCode::SUCCESS;

  bookPrdHistograms();

  //if (newEventsBlock){}
  //if (newLumiBlock){}
  //if (newRun) {

  //declare a group of histograms
  MonGroup cscprd_shift( this, m_cscPRDPath+"/Shift", run, ATTRIB_MANAGED );
  MonGroup cscprd_expert( this, m_cscPRDPath+"/Expert", run, ATTRIB_MANAGED );

  // register shift histograms with service
  for(size_t j = 0; j < m_regHShift.size(); j++) {
    sc=cscprd_shift.regHist(m_regHShift[j]);
    if(sc.isFailure()) {
      ATH_MSG_ERROR ( "Failed to register shift histogram \"" << m_regHShift[j]->GetName() << "\""  );
      return sc;
    } // end if
  } // end for shift

  // register expert histograms with service
  for(size_t j = 0; j < m_regHExpert.size(); j++) {
    sc=cscprd_expert.regHist(m_regHExpert[j]);
    if(sc.isFailure()) {
      ATH_MSG_ERROR ( "Failed to register expert histogram \"" << m_regHExpert[j]->GetName() << "\""  );
      return sc;
    } // end if
  } // end for expert

  // register overview histograms for EA
  std::vector<TH1 *>::iterator m_iT;
  cscprd_oviewEA = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEA/PRD", run, ATTRIB_MANAGED );
  m_iT = m_regHOviewEA.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEA.size() << " CSCEA Overview Histograms " );
  for (; m_iT != m_regHOviewEA.end(); ++m_iT) {
    ATH_MSG_DEBUG ( "Registering CSCEA Overview Histogram: " << (*m_iT)->GetName() );
    sc = cscprd_oviewEA->regHist(*m_iT);
    ATH_MSG_DEBUG ( "Successfully Registered CSCEA Overview Histogram: " << (*m_iT)->GetName() );
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A: " << (*m_iT)->GetName() );
      return sc;
    }
  }

  // register overview histograms for EC
  cscprd_oviewEC = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEC/PRD", run, ATTRIB_MANAGED );
  m_iT = m_regHOviewEC.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEC.size() << " CSCEC Overview Histograms " );
  for (; m_iT != m_regHOviewEC.end(); ++m_iT) {
    ATH_MSG_DEBUG ( "Registering CSCEC Overview Histogram: " << (*m_iT)->GetName() );
    sc = cscprd_oviewEC->regHist(*m_iT);
    ATH_MSG_DEBUG ( "Successfully Registered CSCEC Overview Histogram: " << (*m_iT)->GetName() );
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap C: " << (*m_iT)->GetName() );
      return sc;
    }
  }


  //} // end if isNewRun

  return sc;
} // End CscPrdValAlg::bookHistograms

//
// fillHistograms ----------------------------------------------------------------
//
StatusCode CscPrdValAlg::fillHistograms()  {

  StatusCode sc = StatusCode::SUCCESS;

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_DEBUG( "CscPrdValAlg: in fillHistograms" );

  const DataHandle<CscStripPrepDataContainer> m_CscPRD(0);

  sc = evtStore()->contains<CscStripPrepDataContainer>(m_cscPrdKey);
  if(sc.isFailure() || m_cscPrdKey == "") {
    ATH_MSG_WARNING (  "PRD container of type Muon::CscStripPrepDataContainer and key \"" << m_cscPrdKey << "\" NOT found in StoreGate" );
    return sc;
  } else {
    sc = evtStore()->retrieve(m_CscPRD, m_cscPrdKey);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING ( "Could not retrieve PRD container of type Muon::CscStripPrepDataContainer and key \"" << m_cscPrdKey << "\"" );
      return sc;
    }
  }

  //Get lumiblock info
  sc = fillLumiBlock();
  if( sc.isFailure() ){
    ATH_MSG_ERROR( "Could Not Get LumiBlock Info" );
    return sc;
  }

  // ==============================================================================
  // Field           Range               Notes
  // ==============================================================================
  // StationName     unsigned integer    maps to "CSS", "CSL", etc.
  // StationEta      [-1,1]              -1 for backward, 1 for forward endcap
  // StationPhi      [1,8]               increases with Phi
  // Technology      [1]                 maps to "CSC"
  // ChamberLayer    [1,2]               increases with |Z|
  // WireLayer       [1,4]               increases with |Z|
  // MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
  // Strip           [1,n]               increases with R   for MeasuresPhi=0
  //                                     increases with Phi for MeasuresPhi=1
  // ==============================================================================



  // Begin Event ==================================================
  ATH_MSG_DEBUG ( " BEGIN  EVENT ========================================== "  );
  ATH_MSG_DEBUG(" Size of PRD Container  : " << m_CscPRD->size());

  for (CscStripPrepDataContainer::const_iterator it = m_CscPRD->begin(); it != m_CscPRD->end(); ++it) {
    const CscStripPrepDataCollection *m_prd = *it;
    ATH_MSG_DEBUG ( " Size of Collection     : " << m_prd->size()  );
    size_t m_noStrips = m_prd->size();  // no. of strips in this cluster = m_stripIds.size()
    size_t m_nEtaClusWidthCnt[5], m_nPhiClusWidthCnt[5];    // cluster position in each phi-layer
    int m_clusCount[33][9], m_sigclusCount[33][9];
    for(size_t kl = 0; kl < 33; kl++ ) {
      for(size_t km = 0; km < 9; km++ ) {
        if(kl == 0 && km < 5) {
          m_nEtaClusWidthCnt[km] = 0;
          m_nPhiClusWidthCnt[km] = 0;
        }
        m_clusCount[kl][km] = 0;
        m_sigclusCount[kl][km] = 0;
      } // end loop over km
    } // end loop over kl

    // loop over PRD-clusters
    // Loop over strip id's vector -- this is just one strip even though its a vector of ID's
    ATH_MSG_DEBUG ( " BEGIN Loop over Strips ========================================== "  );
    for (CscStripPrepDataCollection::const_iterator ic = (*it)->begin(); ic != (*it)->end(); ++ic) { // for-loop over PRD collection
      const CscStripPrepData& m_praw = **ic;

      // Identify the PRD cluster
      Identifier m_prawId = m_praw.identify();
      int m_stationName = m_cscIdHelper->stationName(m_prawId);
      std::string m_stationString = m_cscIdHelper->stationNameString(m_stationName);
      int m_chamberType = m_stationString == "CSS" ? 0 : 1;
      int m_stationEta  = m_cscIdHelper->stationEta(m_prawId);
      int m_stationPhi  = m_cscIdHelper->stationPhi(m_prawId);
      int m_wireLayer   = m_cscIdHelper->wireLayer(m_prawId);
      int m_measuresPhi = m_cscIdHelper->measuresPhi(m_prawId);
      int m_stripId     = m_cscIdHelper->strip(m_prawId);

      int m_sectorNo  = m_stationEta * (2 * m_stationPhi - m_chamberType);

      // compute the indices to store cluster count
      int m_ns = m_sectorNo < 0 ? m_sectorNo*(-1) : m_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
      int m_nl = (m_measuresPhi ? m_wireLayer : m_wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

      m_clusCount[m_ns][m_nl]++;

      // indices for m_ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
      // indices for m_nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
      ATH_MSG_DEBUG(" m_ns = " << m_ns << "\tm_nl = " << m_nl << "\tm_sec = " << m_sectorNo << "\t m_lay= " 
          << m_wireLayer << "\t strip = " << m_stripId << "\tmPhi = " << m_measuresPhi);


      // y-axis fill value
      // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
      // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
      // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
      // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
      float m_secLayer = m_sectorNo + 0.2 * (m_wireLayer - 1) + 0.1;
      int xfac = m_measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]

      // x-axis fill value
      float m_spid = m_stripId * xfac;  
      m_h2csc_prd_hitmap->Fill(m_spid, m_secLayer);
      m_measuresPhi ? m_h2csc_prd_phicluswidth->Fill(m_noStrips,m_secLayer) : m_h2csc_prd_etacluswidth->Fill(m_noStrips,m_secLayer);

      if(m_mapxyrz) {
        float m_x = m_praw.globalPosition().x();
        float m_y = m_praw.globalPosition().y();
        float m_z = m_praw.globalPosition().z();
        float m_r = sqrt(m_x*m_x + m_y*m_y);
        ATH_MSG_DEBUG(" prd m_x = " << m_x << "\t m_y = " << m_y << "\t m_z = " << m_z );
        m_h2csc_prd_r_vs_z_hitmap->Fill(m_z,m_r);
        m_h2csc_prd_y_vs_x_hitmap->Fill(m_y,m_x);
      } // end if(m_mapxyrz)

      // Fit this strip and get Charge (in units of: # of electrons)
      ICscStripFitter::Result m_res;
      m_res = m_stripFitter->fit(m_praw);

      ATH_MSG_DEBUG ( "Strip q +- dq = " << m_res.charge  << " +- " << m_res.dcharge << "\t t +- dt = "
          << m_res.time << " +- " <<  m_res.dtime << "\t w +- dw = " << m_res.width << " +- "
          << m_res.dwidth << "\t status= " << m_res.status << "\t chisq= " << m_res.chsq);


      // determine of the cluster is a noise/signal cluster Max_Delta_ADC > NoiseCut
      float m_kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)
      float m_qstripADC = m_res.charge * m_kiloele;

      // By default m_res.status = -1
      // if strip fit is success m_res.status = 0
      // If fit fails use the peak sample. In this case m_res.status = 1

      bool m_signal = ((m_qstripADC > m_cscNoiseCut) && (m_res.status >= 0)) ? true : false;

      // increment the signal-cluster count
      if(m_signal) {
        m_sigclusCount[m_ns][m_nl]++;
        m_measuresPhi ? m_nPhiClusWidthCnt[m_wireLayer]++ : m_nEtaClusWidthCnt[m_wireLayer]++ ;
        m_h2csc_prd_hitmap_signal->Fill(m_spid,m_secLayer);
        if(m_stationEta == -1) {
          m_h2csc_prd_hitmap_signal_EC->Fill(m_spid,m_secLayer);
          m_h1csc_prd_hitmap_signal_EC_count->Fill(m_spid);
          m_h1csc_prd_hitmap_signal_EC_occupancy->Fill(m_secLayer);
          m_h2csc_prd_occvslb_EC->Fill(m_lumiblock,m_secLayer);
        } // end if(m_stationEta == -1)
        else {
          m_h2csc_prd_hitmap_signal_EA->Fill(m_spid,m_secLayer);
          m_h1csc_prd_hitmap_signal_EA_count->Fill(m_spid);
          m_h1csc_prd_hitmap_signal_EA_occupancy->Fill(m_secLayer);
          m_h2csc_prd_occvslb_EA->Fill(m_lumiblock,m_secLayer);
        } // end else if(m_stationEta == -1)
        if(!m_measuresPhi) {
          m_h2csc_prd_etacluswidth_signal->Fill(m_noStrips,m_secLayer);
        } // end if(!m_measuresPhi)
        else {
          m_h2csc_prd_phicluswidth_signal->Fill(m_noStrips,m_secLayer);
        } // end else if(!m_measuresPhi)
      } // end if(m_signal)
      else {
        m_h2csc_prd_hitmap_noise->Fill(m_spid,m_secLayer);
        if(!m_measuresPhi) {
          m_h2csc_prd_etacluswidth_noise->Fill(m_noStrips,m_secLayer);
        } // end if(!m_measuresPhi)
        else {
          m_h2csc_prd_phicluswidth_noise->Fill(m_noStrips,m_secLayer);
        } // end else if(!m_measuresPhi)
      } // end else if(m_signal)


    } // end for-loop over PRD collection
    ATH_MSG_DEBUG ( " End loop over PRD collection======================" );

    for(size_t lcnt = 1; lcnt < 5; lcnt++ ) {
      m_h2csc_prd_eta_vs_phi_cluswidth->Fill(m_nPhiClusWidthCnt[lcnt],m_nEtaClusWidthCnt[lcnt]);
    } // end loop over lcnt

    int m_numeta = 0, m_numphi = 0;
    int m_numetasignal = 0, m_numphisignal = 0;
    for(int kl = 1; kl < 33; kl++ ) {

      //int m_sec = kl < 17 ? kl*(-1) : kl; // [1->16](-side)  [17-32] (+side)
      for(int km = 1; km < 9; km++ ) {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string m_wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";

        int m_count = m_clusCount[kl][km];
        int m_scount = m_sigclusCount[kl][km];


        if(m_count) {
          float m_secLayer = kl-16 + 0.2 * (lay - 1) + 0.1;
          if(mphi) {
            m_h2csc_prd_phicluscount->Fill(m_count,m_secLayer); // all phi-cluster counts
            m_numphi += m_count;
            if(m_scount) {
              m_numphisignal += m_scount;
              m_h2csc_prd_phicluscount_signal->Fill(m_scount,m_secLayer); // signal phi-cluster count
              m_h2csc_prd_phicluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise phi-cluster count
            } // end if(m_scount) 
            else {
              m_h2csc_prd_phicluscount_noise->Fill(m_count,m_secLayer); // noise phi-cluster count
            } // end else if(m_scount)
          } // end if(mphi)
          else {
            m_h2csc_prd_etacluscount->Fill(m_count,m_secLayer);
            m_numeta += m_count;
            if(m_scount) {
              m_numetasignal += m_scount;
              m_h2csc_prd_etacluscount_signal->Fill(m_scount,m_secLayer); // signal eta-cluster count
              m_h2csc_prd_etacluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise eta-cluster count
            } // end if(m_scount)
            else {
              m_h2csc_prd_etacluscount_noise->Fill(m_count,m_secLayer); // noise eta-cluster count
            } // end else if(m_scount)
          } // end else if(mphi)
          ATH_MSG_DEBUG ( m_wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " <<
              m_secLayer << "\t = " << m_count << "\t" << m_scount);
        } // end if(m_count)
      } // end for km
    } // end for kl

    m_h2csc_prd_eta_vs_phi_cluscount->Fill(m_numphi,m_numeta);
    m_h2csc_prd_eta_vs_phi_cluscount_signal->Fill(m_numphisignal,m_numetasignal);
    m_h2csc_prd_eta_vs_phi_cluscount_noise->Fill(m_numphi-m_numphisignal, m_numeta-m_numetasignal);

  } // end for-loop over container
  ATH_MSG_DEBUG ( " End EVENT======================" );

  ATH_MSG_DEBUG( "CscPrdValAlg: fillHistograms reports success" );

  return sc;
} // end CscPrdValAlg::fillHistograms()

//
// procHistograms ----------------------------------------------------------------
//
StatusCode CscPrdValAlg::procHistograms() {
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG( "CscPrdValAlg: in procHistograms" );
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){
    /*
       std::string m_cscGenPath = m_cscPRDPath.substr(0,m_cscPRDPath.find("CSC"));
    //MonGroup cscprd_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
    for(size_t j = 0; j < m_regHOviewEC.size(); j++ ) {
    TH1 *m_h(0);
    m_h = m_regHOviewEC[j];
    if(m_h != NULL) {
    bool m_hist2d = m_h->IsA()->InheritsFrom("TH2");
    if(m_hist2d) {
    std::string m_hname = m_h->GetName();
    // Get Y-projection (sec+0.2*lay)
    TH1D *m_hY = dynamic_cast<TH2F* >(m_h)->ProjectionY(Form("%s_hY",m_hname.c_str()),0,-1,"");
    // set bin labels
    CscPrdBins::PrdBinLabels(m_hY,-1);
    // register histogram with Overview/CSCEC
    sc = cscprd_oviewEC->regHist(m_hY);
    if ( sc.isFailure() ) {
    ATH_MSG_ERROR (  "Cannot register histogram " << m_hY->GetName() );
    return sc;
    }
    // Get X-projection (counts)
    TH1D *m_hX = dynamic_cast<TH2F* >(m_h)->ProjectionX(Form("%s_hX",m_hname.c_str()),0,-1,"e");
    sc = cscprd_oviewEC->regHist(m_hX);
    if ( sc.isFailure() ) {
    ATH_MSG_ERROR (  "Cannot register histogram " << m_hX->GetName() );
    return sc;
    }
    } // end if hist2d
    } // end if m_h
    } // end for

    //MonGroup cscprd_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
    for(size_t j = 0; j < m_regHOviewEA.size(); j++ ) {
    TH1 *m_h(0);
    m_h = m_regHOviewEA[j];
    if(m_h != NULL) {
    */
  } // end isEndOfRun

  return sc;
  }

  //
  // checkHists ----------------------------------------------------------------
  //
  StatusCode CscPrdValAlg::checkHists(bool /* fromFinalize */) {

    ATH_MSG_DEBUG( "CscPrdValAlg: in checkHists" );

    return StatusCode::SUCCESS;
  }

  //
  // fillLumiBlock ----------------------------------------------------------------
  //
  StatusCode CscPrdValAlg::fillLumiBlock(){

    m_lumiblock = -1;

    const xAOD::EventInfo* evt(0);

    StatusCode sc = StatusCode::SUCCESS;
    sc = (*m_activeStore)->retrieve(evt);
    if (sc.isFailure() || evt==0){
      ATH_MSG_ERROR("Could not find Event Info");
      return sc;
    }

    m_lumiblock = evt->lumiBlock();

    return sc;

  }
