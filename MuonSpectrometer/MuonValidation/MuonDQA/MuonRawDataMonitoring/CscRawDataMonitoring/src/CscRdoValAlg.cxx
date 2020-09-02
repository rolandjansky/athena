/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*   NAME    : CscRdoValAlg.cxx
 *   PACKAGE : MuonRawDataMonitoring/CscRawDataMonitoring
 *   PURPOSE : CSC ROD-cluster (RDO) monitoring
 *   AUTHOR  : Ken Johns, Xiaowen Lei (U.Arizona)
 *   
 *   MODIFIED: N. Benekos(Illinois)
 *             V. Kaushik(U.Arizona) : 2009-05-10
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Athena include(s)
#include "CscRawDataMonitoring/CscRdoValAlg.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"

// ROOT include(s)
#include "TClass.h"
#include "TH1F.h"
#include "TH2F.h"

namespace CscRdoBins {
  void RdoBinLabels(TH1 *h, int side) {
    h->GetXaxis()->SetTitle("");
    h->GetXaxis()->SetLabelSize(0.03);
    if(side == -1) {
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
    } else if (side == 1) {
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
CscRdoValAlg::CscRdoValAlg(const std::string & type, const std::string & name, 
    const IInterface* parent) : ManagedMonitorToolBase(type, name, parent),
                                m_cscRdoDecoderTool("Muon::CscRDO_Decoder"),
                                m_cscrdo_oviewEA(0), 
                                m_cscrdo_oviewEC(0)
{

  declareProperty("CSCRawDataPath", m_cscRDOPath = "Muon/MuonRawDataMonitoring/CSC/RDO");
  declareProperty("CSCRawDataDecoder",m_cscRdoDecoderTool);
  declareProperty("NoiseCutADC", m_cscNoiseCut = 50);

  m_cscGenPath = m_cscRDOPath.substr(0,m_cscRDOPath.find("CSC"));
  
  initHistograms();

}


//
// destructor ----------------------------------------------------------------
//
CscRdoValAlg::~CscRdoValAlg() {
  if(m_cscrdo_oviewEA) {
    delete m_cscrdo_oviewEA;
    m_cscrdo_oviewEA = 0;
  }
  if(m_cscrdo_oviewEC) {
    delete m_cscrdo_oviewEC;
    m_cscrdo_oviewEC = 0;
  }
  ATH_MSG_DEBUG ( " deleting CscRdoValAlg "  );
}


//
// initialize ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::initialize() {
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_MSG_DEBUG ("CscRdoValAlg : in initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());
  ATH_CHECK(m_cscRdoKey.initialize());
  return StatusCode::SUCCESS;
}


//
//// initHistograms ----------------------------------------------------------------
////
void CscRdoValAlg::initHistograms() {

  m_h2csc_rdo_hitmap = 0;
  m_h2csc_rdo_hitmap_signal = 0;
  m_h2csc_rdo_hitmap_noise = 0;

  m_h2csc_rdo_hitmap_norm = 0;
  m_h2csc_rdo_hitmap_norm_signal = 0;
  m_h2csc_rdo_hitmap_norm_noise = 0;

  m_h2csc_rdo_hitmap_signal_EA = 0;
  m_h1csc_rdo_hitmap_signal_EA_count = 0;
  m_h1csc_rdo_hitmap_signal_EA_occupancy = 0;

  m_h2csc_rdo_hitmap_norm_signal_EA = 0;

  m_h2csc_rdo_hitmap_signal_EC = 0;
  m_h1csc_rdo_hitmap_signal_EC_count = 0;
  m_h1csc_rdo_hitmap_signal_EC_occupancy = 0;

  m_h2csc_rdo_hitmap_norm_signal_EC = 0;

  m_h2csc_rdo_phicluswidth = 0;
  m_h2csc_rdo_phicluswidth_signal = 0;
  m_h2csc_rdo_phicluswidth_noise = 0;

  m_h2csc_rdo_phicluswidth_signal_EA = 0;
  m_h1csc_rdo_phicluswidth_signal_EA_count = 0;
  m_h1csc_rdo_phicluswidth_signal_EA_occupancy = 0;

  m_h2csc_rdo_phicluswidth_signal_EC = 0;
  m_h1csc_rdo_phicluswidth_signal_EC_count = 0;
  m_h1csc_rdo_phicluswidth_signal_EC_occupancy = 0;

  m_h2csc_rdo_etacluswidth = 0;
  m_h2csc_rdo_etacluswidth_signal = 0;
  m_h2csc_rdo_etacluswidth_noise = 0;

  m_h2csc_rdo_etacluswidth_signal_EA = 0;
  m_h1csc_rdo_etacluswidth_signal_EA_count = 0;
  m_h1csc_rdo_etacluswidth_signal_EA_occupancy = 0;
  m_h2csc_rdo_etacluswidth_signal_EC = 0;
  m_h1csc_rdo_etacluswidth_signal_EC_count = 0;
  m_h1csc_rdo_etacluswidth_signal_EC_occupancy = 0;

  m_h2csc_rdo_phicluscount = 0;
  m_h2csc_rdo_phicluscount_signal = 0;
  m_h2csc_rdo_phicluscount_noise = 0;

  m_h2csc_rdo_etacluscount = 0;
  m_h2csc_rdo_etacluscount_signal = 0;
  m_h2csc_rdo_etacluscount_noise = 0;


  m_h1csc_rdo_maxdiffamp = 0;

  // Correlation plots
  m_h2csc_rdo_eta_vs_phi_cluscount = 0;
  m_h2csc_rdo_eta_vs_phi_cluscount_signal = 0;
  m_h2csc_rdo_eta_vs_phi_cluscount_noise = 0;

  m_h2csc_rdo_eta_vs_phi_cluswidth = 0;
  m_h2csc_rdo_eta_vs_phi_cluswidth_signal = 0;

}

//
// bookRdoHistograms ----------------------------------------------------------------
//
void  CscRdoValAlg::bookRdoHistograms(){
  
  m_regHShift.clear();   
  m_regHExpert.clear();
  m_regHOviewEA.clear();
  m_regHOviewEC.clear();

  ///******************** DO NOT MODIFY (begin) ***********************************************///
  int nxbins  = 242;  // 192 bins for precision, 48 for transverse strips + 2 extra
  float nxmin = -49.; // -1 -> -48 (for transverse)
  float nxmax = 193.; // 1 -> 192 (for precision)

  int nybins  = 175;  // 32 chambers (16 per side x 5 layers per chamber) + 4 extra
  float nymin = -17.; //
  float nymax = 18.;  //

  int nybinsEA  = 90;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEA =  0.; //
  float nymaxEA = 18.;  //

  int nybinsEC  = 85;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEC = -17.; //
  float nymaxEC = 0.;  //
  ///******************** DO NOT MODIFY (end) ***********************************************///
  
  // book histograms
  m_h1csc_rdo_maxdiffamp = new TH1F("h1csc_rdo_maxdiffamp", "Max Amplitude in ROD Cluster;ADC counts;;",500,0,5000);

  // strip hitmap
  m_h2csc_rdo_hitmap = new TH2F("h2csc_rdo_hitmap", "Hit Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_rdo_hitmap_noise = new TH2F("h2csc_rdo_hitmap_noise", "Noise Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_rdo_hitmap_signal = new TH2F("h2csc_rdo_hitmap_signal", "Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_rdo_hitmap_norm = new TH2F("h2csc_rdo_hitmap_norm", "Hit Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_rdo_hitmap_norm_noise = new TH2F("h2csc_rdo_hitmap_norm_noise", "Noise Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);
  m_h2csc_rdo_hitmap_norm_signal = new TH2F("h2csc_rdo_hitmap_norm_signal", "Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_rdo_hitmap_signal_EA = new TH2F("h2csc_rdo_hitmap_signal_EA", "EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEA,nyminEA,nymaxEA);
  m_h1csc_rdo_hitmap_signal_EA_count = new TH1F("h1csc_rdo_hitmap_signal_EA_count", "EndCap A: Signal Occupancy;channel;entries/channel",
      nxbins,nxmin,nxmax);
  m_h1csc_rdo_hitmap_signal_EA_occupancy = new TH1F("h1csc_rdo_hitmap_signal_EA_occupancy", "EndCap A: Signal Occupancy;[sector] + [0.2 #times layer];entries/layer",
      nybinsEA,nyminEA,nymaxEA);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_hitmap_signal_EA_occupancy,1);

  m_h2csc_rdo_hitmap_norm_signal_EA = new TH2F("h2csc_rdo_hitmap_norm_signal_EA", "EndCap A: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEA,nyminEA,nymaxEA);

  m_h2csc_rdo_hitmap_signal_EC = new TH2F("h2csc_rdo_hitmap_signal_EC", "EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEC,nyminEC,nymaxEC);
  m_h1csc_rdo_hitmap_signal_EC_count = new TH1F("h1csc_rdo_hitmap_signal_EC_count", "EndCap C: Signal Occupancy;channel;entries/channel",
      nxbins,nxmin,nxmax);
  m_h1csc_rdo_hitmap_signal_EC_occupancy = new TH1F("h1csc_rdo_hitmap_signal_EC_occupancy", "EndCap C: Signal Occupancy;[sector] + [0.2 #times layer];entries/layer",
      nybinsEC,nyminEC,nymaxEC);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_hitmap_signal_EC_occupancy,-1);

  m_h2csc_rdo_hitmap_norm_signal_EC = new TH2F("h2csc_rdo_hitmap_norm_signal_EC", "EndCap C: Signal Occupancy;channel;[sector] + [0.2 #times layer]",
      nxbins,nxmin,nxmax,nybinsEC,nyminEC,nymaxEC);

  // ROD eta-cluster width
  m_h2csc_rdo_etacluswidth = new TH2F("h2csc_rdo_etacluswidth",
      "ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_rdo_etacluswidth_noise = new TH2F("h2csc_rdo_etacluswidth_noise",
      "ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_rdo_etacluswidth_signal = new TH2F("h2csc_rdo_etacluswidth_signal",
      "ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_rdo_etacluswidth_signal_EA = new TH2F("h2csc_rdo_etacluswidth_signal_EA",
      "EndCap A: ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybinsEA,nyminEA,nymaxEA);
  m_h1csc_rdo_etacluswidth_signal_EA_count = new TH1F("h1csc_rdo_etacluswidth_signal_EA_count",
      "EndCap A: ROD precision-cluster width;width(no.of strips);entries;",192,0,192);
  m_h1csc_rdo_etacluswidth_signal_EA_occupancy = new TH1F("h1csc_rdo_etacluswidth_signal_EA_occupancy",
      "EndCap A: ROD precision-cluster width;[sector] + [0.2 #times layer];entries/layer",nybinsEA,nyminEA,nymaxEA);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_etacluswidth_signal_EA_occupancy,1);

  m_h2csc_rdo_etacluswidth_signal_EC = new TH2F("h2csc_rdo_etacluswidth_signal_EC",
      "EndCap C: ROD precision-cluster width;no.of strips;[sector] + [0.2 #times layer]",
      192,0,192,nybinsEC,nyminEC,nymaxEC);
  m_h1csc_rdo_etacluswidth_signal_EC_count = new TH1F("h1csc_rdo_etacluswidth_signal_EC_count",
      "EndCap C: ROD precision-cluster width;width(no.of strips);entries",192,0,192);
  m_h1csc_rdo_etacluswidth_signal_EC_occupancy = new TH1F("h1csc_rdo_etacluswidth_signal_EC_occupancy",
      "EndCap C: ROD precision-cluster width;[sector] + [0.2 #times layer];entries/layer",nybinsEC,nyminEC,nymaxEC);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_etacluswidth_signal_EC_occupancy,-1);

  // ROD phi-cluster width
  m_h2csc_rdo_phicluswidth = new TH2F("h2csc_rdo_phicluswidth",
      "ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);
  m_h2csc_rdo_phicluswidth_noise = new TH2F("h2csc_rdo_phicluswidth_noise",
      "ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);
  m_h2csc_rdo_phicluswidth_signal = new TH2F("h2csc_rdo_phicluswidth_signal",
      "ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);

  m_h2csc_rdo_phicluswidth_signal_EA = new TH2F("h2csc_rdo_phicluswidth_signal_EA",
      "EndCap A: ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]",
      48,0,48,nybinsEA,nyminEA,nymaxEA);
  m_h1csc_rdo_phicluswidth_signal_EA_count = new TH1F("h1csc_rdo_phicluswidth_signal_EA_count",
      "EndCap A: ROD transverse-cluster width;width(no.of strips);entries",48,0,48);
  m_h1csc_rdo_phicluswidth_signal_EA_occupancy = new TH1F("h1csc_rdo_phicluswidth_signal_EA_occupancy",
      "EndCap A: ROD transverse-cluster width;[sector] + [0.2 #times layer];entries/layer",nybinsEA,nyminEA,nymaxEA);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_phicluswidth_signal_EA_occupancy,1);

  m_h2csc_rdo_phicluswidth_signal_EC = new TH2F("h2csc_rdo_phicluswidth_signal_EC",
      "EndCap C: ROD transverse-cluster width;width(no.of strips);[sector] + [0.2 #times layer]",
      48,0,48,nybinsEC,nyminEC,nymaxEC);
  m_h1csc_rdo_phicluswidth_signal_EC_count = new TH1F("h1csc_rdo_phicluswidth_signal_EC_count",
      "EndCap C: ROD transverse-cluster width;width(no.of strips);entries",48,0,48);
  m_h1csc_rdo_phicluswidth_signal_EC_occupancy = new TH1F("h1csc_rdo_phicluswidth_signal_EC_occupancy",
      "EndCap C: ROD transverse-cluster width;[sector] + [0.2 #times layer];entries/layer",nybinsEC,nyminEC,nymaxEC);
  CscRdoBins::RdoBinLabels(m_h1csc_rdo_phicluswidth_signal_EC_occupancy,-1);

  // ROD eta-cluster count
  m_h2csc_rdo_etacluscount = new TH2F("h2csc_rdo_etacluscount",
      "ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_rdo_etacluscount_noise = new TH2F("h2csc_rdo_etacluscount_noise",
      "ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_rdo_etacluscount_signal = new TH2F("h2csc_rdo_etacluscount_signal",
      "ROD precision-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);

  // ROD phi-cluster count
  m_h2csc_rdo_phicluscount = new TH2F("h2csc_rdo_phicluscount",
      "ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_rdo_phicluscount_noise = new TH2F("h2csc_rdo_phicluscount_noise",
      "ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_rdo_phicluscount_signal = new TH2F("h2csc_rdo_phicluscount_signal",
      "ROD transverse-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);


  // correlation histograms
  m_h2csc_rdo_eta_vs_phi_cluscount = new TH2F("h2csc_rdo_eta_vs_phi_cluscount", 
      "Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_rdo_eta_vs_phi_cluscount_noise = new TH2F("h2csc_rdo_eta_vs_phi_cluscount_noise", 
      "Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_rdo_eta_vs_phi_cluscount_signal = new TH2F("h2csc_rdo_eta_vs_phi_cluscount_signal", 
      "Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);

  m_h2csc_rdo_eta_vs_phi_cluswidth = new TH2F("h2csc_rdo_eta_vs_phi_cluswidth", 
      "Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width",100,0,100,100,0,100);

  m_regHShift.push_back(m_h1csc_rdo_maxdiffamp);              // shift, overview

  m_regHExpert.push_back(m_h2csc_rdo_hitmap);                 // expert
  m_regHShift.push_back(m_h2csc_rdo_hitmap_signal);           // shift, overview (dq-flag)
  m_regHExpert.push_back(m_h2csc_rdo_hitmap_noise);           // expert
  
  m_regHExpert.push_back(m_h2csc_rdo_hitmap_norm);                 // expert
  m_regHShift.push_back(m_h2csc_rdo_hitmap_norm_signal);           // shift, overview (dq-flag)
  m_regHExpert.push_back(m_h2csc_rdo_hitmap_norm_noise);           // expert

  m_regHOviewEA.push_back(m_h2csc_rdo_hitmap_signal_EA);
  m_regHOviewEA.push_back(m_h1csc_rdo_hitmap_signal_EA_count);
  m_regHOviewEA.push_back(m_h1csc_rdo_hitmap_signal_EA_occupancy);

  m_regHOviewEA.push_back(m_h2csc_rdo_hitmap_norm_signal_EA);

  m_regHOviewEC.push_back(m_h2csc_rdo_hitmap_signal_EC);
  m_regHOviewEC.push_back(m_h1csc_rdo_hitmap_signal_EC_count);
  m_regHOviewEC.push_back(m_h1csc_rdo_hitmap_signal_EC_occupancy);

  m_regHOviewEC.push_back(m_h2csc_rdo_hitmap_norm_signal_EC);

  m_regHExpert.push_back(m_h2csc_rdo_etacluswidth);           // expert
  m_regHShift.push_back(m_h2csc_rdo_etacluswidth_signal);     // shift, overview (dq-flags)
  m_regHExpert.push_back(m_h2csc_rdo_etacluswidth_noise);     // expert

  m_regHOviewEA.push_back(m_h2csc_rdo_etacluswidth_signal_EA);
  m_regHOviewEA.push_back(m_h1csc_rdo_etacluswidth_signal_EA_count);
  m_regHOviewEA.push_back(m_h1csc_rdo_etacluswidth_signal_EA_occupancy);

  m_regHOviewEC.push_back(m_h2csc_rdo_etacluswidth_signal_EC);
  m_regHOviewEC.push_back(m_h1csc_rdo_etacluswidth_signal_EC_count);
  m_regHOviewEC.push_back(m_h1csc_rdo_etacluswidth_signal_EC_occupancy);

  m_regHExpert.push_back(m_h2csc_rdo_phicluswidth);           // expert
  m_regHShift.push_back(m_h2csc_rdo_phicluswidth_signal);     // shift, overview (dq-flags)
  m_regHExpert.push_back(m_h2csc_rdo_phicluswidth_noise);     // expert

  m_regHOviewEA.push_back(m_h2csc_rdo_phicluswidth_signal_EA);
  m_regHOviewEA.push_back(m_h1csc_rdo_phicluswidth_signal_EA_count);
  m_regHOviewEA.push_back(m_h1csc_rdo_phicluswidth_signal_EA_occupancy);

  m_regHOviewEC.push_back(m_h2csc_rdo_phicluswidth_signal_EC);
  m_regHOviewEC.push_back(m_h1csc_rdo_phicluswidth_signal_EC_count);
  m_regHOviewEC.push_back(m_h1csc_rdo_phicluswidth_signal_EC_occupancy);

  m_regHExpert.push_back(m_h2csc_rdo_etacluscount);           // expert
  m_regHShift.push_back(m_h2csc_rdo_etacluscount_signal);     // shift
  m_regHExpert.push_back(m_h2csc_rdo_etacluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_rdo_phicluscount);           // expert
  m_regHShift.push_back(m_h2csc_rdo_phicluscount_signal);     // shift
  m_regHExpert.push_back(m_h2csc_rdo_phicluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_rdo_eta_vs_phi_cluscount);           // expert
  m_regHExpert.push_back(m_h2csc_rdo_eta_vs_phi_cluscount_signal);    // expert
  m_regHExpert.push_back(m_h2csc_rdo_eta_vs_phi_cluscount_noise);     // expert

  m_regHExpert.push_back(m_h2csc_rdo_eta_vs_phi_cluswidth);           // expert

}

//
// bookHistograms ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::bookHistograms(){
  ATH_MSG_DEBUG ( "CSCRdoMon : in bookHistograms()"  );

  StatusCode sc = StatusCode::SUCCESS;

  bookRdoHistograms();

  //declare a group of histograms
  MonGroup cscrdo_shift( this, m_cscRDOPath+"/Shift", run, ATTRIB_MANAGED );
  MonGroup cscrdo_expert( this, m_cscRDOPath+"/Expert", run, ATTRIB_MANAGED );

  // register shift histograms with service
  for(size_t j = 0; j < m_regHShift.size(); j++) {
    sc=cscrdo_shift.regHist(m_regHShift[j]);
    if(sc.isFailure()) {
      ATH_MSG_ERROR ( "Failed to register shift histogram \"" << m_regHShift[j]->GetName() << "\""  );
      return sc;
    } // end if
  } // end for

  // register expert histograms with service
  for(size_t j = 0; j < m_regHExpert.size(); j++) {
    sc=cscrdo_expert.regHist(m_regHExpert[j]);
    if(sc.isFailure()) {
      ATH_MSG_ERROR ( "Failed to register expert histogram \"" << m_regHExpert[j]->GetName() << "\""  );
      return sc;
    } // end if
  } // end for


  // register overview histograms for EA
  std::vector<TH1 *>::iterator iT;
  //MonGroup m_cscrdo_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
  m_cscrdo_oviewEA = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEA/RDO", run, ATTRIB_UNMANAGED );
  iT = m_regHOviewEA.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEA.size() << " CSCEA Overview Histograms " );
  for (; iT != m_regHOviewEA.end(); ++iT) {
    sc = m_cscrdo_oviewEA->regHist(*iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A: " << (*iT)->GetName() );
      return sc;
    }
  }

  // register overview histograms for EC
  //MonGroup m_cscrdo_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
  m_cscrdo_oviewEC = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEC/RDO", run, ATTRIB_UNMANAGED );
  iT = m_regHOviewEC.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEC.size() << " CSCEC Overview Histograms " );
  for (; iT != m_regHOviewEC.end(); ++iT) {
    sc = m_cscrdo_oviewEC->regHist(*iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap C: " << (*iT)->GetName() );
      return sc;
    }
  }

  //} // end if newRun

  return sc;
}


//
// fillHistograms ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::fillHistograms() {

  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG ( "CscRdoValAlg :: in fillHistograms()"  );

  if(m_cscRdoKey.key()==""){  //it seems that protection is needed for this case
    ATH_MSG_WARNING("CSC RDO key is blank, returning");
    return sc;
  }

  SG::ReadHandle<CscRawDataContainer> CscRDO(m_cscRdoKey);

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

  ATH_MSG_DEBUG ( " Size of RDO Container  : " << CscRDO->size()  );

  // Begin Event ==================================================
  ATH_MSG_DEBUG ( " BEGIN  EVENT ========================================== "  );

  for (CscRawDataContainer::const_iterator it = CscRDO->begin(); it != CscRDO->end(); ++it) {
    const CscRawDataCollection * rdo = *it;
    ATH_MSG_DEBUG ( " Number of Samples      : " << rdo->numSamples()  );
    ATH_MSG_DEBUG ( " Size of Collection     : " << rdo->size()  );

    size_t nEtaClusWidthCnt = 0, nPhiClusWidthCnt = 0;    // cluster position in each phi-layer
    Identifier stationId, channelId;

    int clusCount[33][9], sigclusCount[33][9];
    for(size_t kl = 0; kl < 33; kl++ ) {
      for(size_t km = 0; km < 9; km++ ) {
        clusCount[kl][km] = 0;
        sigclusCount[kl][km] = 0;
      }
    }
    // loop over ROD-clusters 
    for (CscRawDataCollection::const_iterator ic   = (*it)->begin(); ic != (*it)->end(); ++ic) {
      const CscRawData * raw = (*ic);
      if( raw ) {
        // Identify side(A/C), sector(1-16)/layer(1-4)
        stationId = m_cscRdoDecoderTool->stationIdentifier(raw);
        channelId = m_cscRdoDecoderTool->channelIdentifier(raw,0);
        int stationName = m_idHelperSvc->cscIdHelper().stationName(channelId);
        std::string stationString = m_idHelperSvc->cscIdHelper().stationNameString(stationName);
        int chamberType = stationString == "CSS" ? 0 : 1;
        int stationEta  = m_idHelperSvc->cscIdHelper().stationEta(channelId);
        int stationPhi  = m_idHelperSvc->cscIdHelper().stationPhi(channelId);
        int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(channelId);
        int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

        // determine the sector number

        int sectorNo  = stationEta * (2 * stationPhi - chamberType);

        // compute the indices to store cluster count
        int ns = sectorNo < 0 ? sectorNo*(-1) : sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
        int nl = (measuresPhi ? wireLayer : wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

        clusCount[ns][nl]++;

        // indices for ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
        // indices for nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
        ATH_MSG_DEBUG(" ns = " << ns << "\tm_nl = " << nl << "\tm_sec = " << sectorNo << "\t m_lay= " << wireLayer << "\tmPhi = " << measuresPhi);


        // y-axis fill value
        // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
        // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
        // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
        // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
        float secLayer = sectorNo + 0.2 * (wireLayer - 1) + 0.1;  
        int xfac = measuresPhi ? -1 : 1;        // -48 / +192

        // this way we get 4 time samples per strip
        ATH_MSG_DEBUG ( " Width of ROD cluster   : " << raw->width()  );
        uint16_t diff_max = 0, diff;
        std::vector<float> xVals ; /*, m_yVals;*/
        // loop over strips in ROD cluster
        size_t raw_clus_width =  raw->width();

        if(!measuresPhi) {
          m_h2csc_rdo_etacluswidth->Fill(raw_clus_width,secLayer);  // fill eta-cluster width
          nEtaClusWidthCnt++;
        } else {
          m_h2csc_rdo_phicluswidth->Fill(raw_clus_width,secLayer);  // fill phi-cluster width
          nPhiClusWidthCnt++;
        }

        // loop over strips
        for (size_t n = 0; n < raw_clus_width; n++) {

          // identify this strip
          Identifier chID = m_cscRdoDecoderTool->channelIdentifier(raw, n);
          int strip = m_idHelperSvc->cscIdHelper().strip(chID);
          float stripId = strip * xfac;         // x-axis fill value

          m_h2csc_rdo_hitmap->Fill(stripId, secLayer);  // fill hitmap 

          // for every strip that has a hit, store the X,Y values
          xVals.push_back(stripId);

          // extract the (four) time samples for this strip
          std::vector<uint16_t> samples;
          bool extractSamples = raw->samples(n, rdo->numSamples(), samples);
          uint16_t n_max = 0, n_min = 9999;

          // if we have the time samples, identify the max/min sampling values i.e., ADC_max and ADC_min
          if(extractSamples) {
            for(size_t np = 0; np < samples.size(); np++) {
              if(samples[np] < n_min) n_min = samples[np];
              if(samples[np] > n_max) n_max = samples[np];
            }
            // the diff between max and min samplings Delta_ADC = (ADC_max - ADC_min)
            diff = n_max - n_min;

            // compute the max difference Max_Delta_ADC
            if(diff > diff_max) diff_max = diff;

            ATH_MSG_DEBUG ( n << " Max = " << n_max << "  Min = " << n_min << " Diff = " << diff  );
          } // end if extractSamples

        } // end for loop over strips in cluster
        ATH_MSG_DEBUG ( " End loop over strips======================" );

        ATH_MSG_DEBUG ( " Max difference                : " << diff_max  );

        m_h1csc_rdo_maxdiffamp->Fill(diff_max);  // fill Max_Delta_ADC

        // determine of the cluster is a noise/signal cluster Max_Delta_ADC > NoiseCut
        bool signal = diff_max > m_cscNoiseCut ? true : false;
        if(signal) sigclusCount[ns][nl]++; 
        if(signal) {
          for(size_t nf = 0; nf < xVals.size(); nf++) {
            m_h2csc_rdo_hitmap_signal->Fill(xVals[nf],secLayer);
            if(stationEta == -1) {
              m_h2csc_rdo_hitmap_signal_EC->Fill(xVals[nf],secLayer);
              m_h1csc_rdo_hitmap_signal_EC_count->Fill(xVals[nf]);
              m_h1csc_rdo_hitmap_signal_EC_occupancy->Fill(secLayer);
            } else {
              m_h2csc_rdo_hitmap_signal_EA->Fill(xVals[nf],secLayer);
              m_h1csc_rdo_hitmap_signal_EA_count->Fill(xVals[nf]);
              m_h1csc_rdo_hitmap_signal_EA_occupancy->Fill(secLayer);
            }
          }
          if(!measuresPhi) {
            m_h2csc_rdo_etacluswidth_signal->Fill(raw_clus_width,secLayer);
            if(stationEta == -1) {
              m_h2csc_rdo_etacluswidth_signal_EC->Fill(raw_clus_width,secLayer);
              m_h1csc_rdo_etacluswidth_signal_EC_count->Fill(raw_clus_width);
              m_h1csc_rdo_etacluswidth_signal_EC_occupancy->Fill(secLayer);
            } else {
              m_h2csc_rdo_etacluswidth_signal_EA->Fill(raw_clus_width,secLayer);
              m_h1csc_rdo_etacluswidth_signal_EA_count->Fill(raw_clus_width);
              m_h1csc_rdo_etacluswidth_signal_EA_occupancy->Fill(secLayer);
            }
          }else {
            m_h2csc_rdo_phicluswidth_signal->Fill(raw_clus_width,secLayer);
            if(stationEta == -1) {
              m_h2csc_rdo_phicluswidth_signal_EC->Fill(raw_clus_width,secLayer);
              m_h1csc_rdo_phicluswidth_signal_EC_count->Fill(raw_clus_width);
              m_h1csc_rdo_phicluswidth_signal_EC_occupancy->Fill(secLayer);
            } else {
              m_h2csc_rdo_phicluswidth_signal_EA->Fill(raw_clus_width,secLayer);
              m_h1csc_rdo_phicluswidth_signal_EA_count->Fill(raw_clus_width);
              m_h1csc_rdo_phicluswidth_signal_EA_occupancy->Fill(secLayer);
            }
          }
        } else {
          for(size_t nf = 0; nf < xVals.size(); nf++) 
            m_h2csc_rdo_hitmap_noise->Fill(xVals[nf],secLayer);
          if(!measuresPhi) {
            m_h2csc_rdo_etacluswidth_noise->Fill(raw_clus_width,secLayer);
          }else {
            m_h2csc_rdo_phicluswidth_noise->Fill(raw_clus_width,secLayer);
          }
        }
      } // if raw (CscRawData)
    } // end for loop over ROD-clusters
    ATH_MSG_DEBUG ( " End loop over clusters======================" );


    m_h2csc_rdo_eta_vs_phi_cluswidth->Fill(nPhiClusWidthCnt,nEtaClusWidthCnt);

    int numeta = 0, numphi = 0;
    int numetasignal = 0, numphisignal = 0;
    for(int kl = 1; kl < 33; kl++ ) {

      //int m_sec = kl < 17 ? kl*(-1) : kl; // [1->16](-side)  [17-32] (+side)
      for(int km = 1; km < 9; km++ ) {

        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";
        int count = clusCount[kl][km];
        int scount = sigclusCount[kl][km];

        if(count) {
          float secLayer = kl-16 + 0.2 * (lay - 1) + 0.1;
          if(mphi) {
            m_h2csc_rdo_phicluscount->Fill(count,secLayer); // all phi-cluster counts
            numphi += count; 
            if(scount) {
              numphisignal += scount; 
              m_h2csc_rdo_phicluscount_signal->Fill(scount,secLayer); // signal phi-cluster count
              m_h2csc_rdo_phicluscount_noise->Fill((count-scount),secLayer); // noise phi-cluster count
            } else {
              m_h2csc_rdo_phicluscount_noise->Fill(count,secLayer); // noise phi-cluster count
            }
          } else {
            m_h2csc_rdo_etacluscount->Fill(count,secLayer);
            numeta += count; 
            if(scount) {
              numetasignal += scount; 
              m_h2csc_rdo_etacluscount_signal->Fill(scount,secLayer); // signal eta-cluster count
              m_h2csc_rdo_etacluscount_noise->Fill((count-scount),secLayer); // noise eta-cluster count
            } else {
              m_h2csc_rdo_etacluscount_noise->Fill(count,secLayer); // noise eta-cluster count
            } 
          }
          ATH_MSG_DEBUG ( wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " << 
              secLayer << "\t = " << count << "\t" << scount);
        }
      }
    }

    m_h2csc_rdo_eta_vs_phi_cluscount->Fill(numphi,numeta);
    m_h2csc_rdo_eta_vs_phi_cluscount_signal->Fill(numphisignal,numetasignal);
    m_h2csc_rdo_eta_vs_phi_cluscount_noise->Fill(numphi-numphisignal, numeta-numetasignal);

  } // end for if (container)
  ATH_MSG_DEBUG ( " END EVENT ============================================ "  );
  // End Event ====================================================

  ATH_MSG_DEBUG ( "done collecting histograms"  );
  ATH_MSG_DEBUG ( "CSCRdoMon::fillHistograms reports success"  );

  return sc;
}

//
// checkHists ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::checkHists(bool /* fromFinalize */) {

  ATH_MSG_DEBUG ( "CscRdoValAlg : in checkHists()"  );

  return StatusCode::SUCCESS;
}
