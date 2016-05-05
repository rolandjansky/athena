/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

// ROOT include(s)
#include "TClass.h"
#include "TH1F.h"
#include "TH2F.h"

namespace CscRdoBins {
  void RdoBinLabels(TH1 *h, int m_side) {
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
CscRdoValAlg::CscRdoValAlg(const std::string & type, const std::string & name, 
    const IInterface* parent) : ManagedMonitorToolBase(type, name, parent),
                                m_cscRdoDecoderTool("Muon::CscRDO_Decoder"),
                                cscrdo_oviewEA(0), 
                                cscrdo_oviewEC(0)
{

  declareProperty("CSCRawDataKey", m_cscRdoKey = "CSCRDO");
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
  if(cscrdo_oviewEA) {
    delete cscrdo_oviewEA;
    cscrdo_oviewEA = 0;
  }
  if(cscrdo_oviewEC) {
    delete cscrdo_oviewEC;
    cscrdo_oviewEC = 0;
  }
  ATH_MSG_DEBUG ( " deleting CscRdoValAlg "  );
}


//
// initialize ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::initialize() {

  ATH_MSG_DEBUG ( "CscRdoValAlg : in initialize()"  );
  StatusCode sc;

  sc = detStore()->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure()) { 
    ATH_MSG_ERROR ( "Cannot get CscIdHelper"  ); 
    return sc; 
  }
  ATH_MSG_DEBUG ( " Found the CscIdHelper. "  );

  ManagedMonitorToolBase::initialize().ignore();  // Ignore the checking code;
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

  //if (newEventsBlock){}
  //if (newLumiBlock){}
  //if (newRun) {
  
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

  //}

}

//
// bookHistograms ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::bookHistograms(){
  ATH_MSG_DEBUG ( "CSCRdoMon : in bookHistograms()"  );

  StatusCode sc = StatusCode::SUCCESS;

  bookRdoHistograms();

  //if (newEventsBlock){}
  //if (newLumiBlock){}
  //if (newRun) {

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
  std::vector<TH1 *>::iterator m_iT;
  //MonGroup cscrdo_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
  cscrdo_oviewEA = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEA/RDO", run, ATTRIB_UNMANAGED );
  m_iT = m_regHOviewEA.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEA.size() << " CSCEA Overview Histograms " );
  for (; m_iT != m_regHOviewEA.end(); ++m_iT) {
    sc = cscrdo_oviewEA->regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A: " << (*m_iT)->GetName() );
      return sc;
    }
  }

  // register overview histograms for EC
  //MonGroup cscrdo_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
  cscrdo_oviewEC = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEC/RDO", run, ATTRIB_UNMANAGED );
  m_iT = m_regHOviewEC.begin();
  ATH_MSG_DEBUG (  "Found " << m_regHOviewEC.size() << " CSCEC Overview Histograms " );
  for (; m_iT != m_regHOviewEC.end(); ++m_iT) {
    sc = cscrdo_oviewEC->regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap C: " << (*m_iT)->GetName() );
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

  const DataHandle<CscRawDataContainer> m_CscRDO(0);

  if(!evtStore()->contains<CscRawDataContainer>(m_cscRdoKey) || m_cscRdoKey == "") {
    ATH_MSG_WARNING (  "RDO container of type Muon::CscRawDataContainer and key \"" << m_cscRdoKey << "\" NOT found in StoreGate" );
    return sc;
  } else {
    sc = evtStore()->retrieve(m_CscRDO, m_cscRdoKey);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING ( "Could not retrieve RDO container of type Muon::CscRawDataContainer and key \"" << m_cscRdoKey << "\"" );
      return sc;
    }
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

  // If we still don't have CSCRDO after retrieval (happens!) return
  // without statuscode without further processing after issuing warning message 
  if( !m_CscRDO ) {
    ATH_MSG_WARNING (  "RDO container of type Muon::CscRawDataContainer and key \"" << m_cscRdoKey << "\" is NULL after retrieval from StoreGate" );
    return sc;
  }

  ATH_MSG_DEBUG ( " Size of RDO Container  : " << m_CscRDO->size()  );

  // Begin Event ==================================================
  ATH_MSG_DEBUG ( " BEGIN  EVENT ========================================== "  );

  for (CscRawDataContainer::const_iterator it = m_CscRDO->begin(); it != m_CscRDO->end(); ++it) {
    const CscRawDataCollection * m_rdo = *it;
    ATH_MSG_DEBUG ( " Number of Samples      : " << m_rdo->numSamples()  );
    ATH_MSG_DEBUG ( " Size of Collection     : " << m_rdo->size()  );

    size_t m_nEtaClusWidthCnt = 0, m_nPhiClusWidthCnt = 0;    // cluster position in each phi-layer
    Identifier m_stationId, m_channelId;

    int m_clusCount[33][9], m_sigclusCount[33][9];
    for(size_t kl = 0; kl < 33; kl++ ) {
      for(size_t km = 0; km < 9; km++ ) {
        m_clusCount[kl][km] = 0;
        m_sigclusCount[kl][km] = 0;
      }
    }
    // loop over ROD-clusters 
    for (CscRawDataCollection::const_iterator ic   = (*it)->begin(); ic != (*it)->end(); ++ic) {
      const CscRawData * m_raw = (*ic);
      if( m_raw ) {
        // Identify side(A/C), sector(1-16)/layer(1-4)
        m_stationId = m_cscRdoDecoderTool->stationIdentifier(m_raw);
        m_channelId = m_cscRdoDecoderTool->channelIdentifier(m_raw,0);
        int m_stationName = m_cscIdHelper->stationName(m_channelId);
        std::string m_stationString = m_cscIdHelper->stationNameString(m_stationName);
        int m_chamberType = m_stationString == "CSS" ? 0 : 1;
        int m_stationEta  = m_cscIdHelper->stationEta(m_channelId);
        int m_stationPhi  = m_cscIdHelper->stationPhi(m_channelId);
        int m_wireLayer = m_cscIdHelper->wireLayer(m_channelId);
        int m_measuresPhi = m_cscIdHelper->measuresPhi(m_channelId);

        // determine the sector number

        int m_sectorNo  = m_stationEta * (2 * m_stationPhi - m_chamberType);

        // check boundaries of sector/layer - redundancy
        //if(!(m_sectorNo+16) < 33) m_sectorNo = 0;
        //if(!(m_wireLayer < 5)) m_wireLayer = 0;

        // compute the indices to store cluster count
        int m_ns = m_sectorNo < 0 ? m_sectorNo*(-1) : m_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
        int m_nl = (m_measuresPhi ? m_wireLayer : m_wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

        m_clusCount[m_ns][m_nl]++;

        // indices for m_ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
        // indices for m_nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
        ATH_MSG_DEBUG(" m_ns = " << m_ns << "\tm_nl = " << m_nl << "\tm_sec = " << m_sectorNo << "\t m_lay= " << m_wireLayer << "\tmPhi = " << m_measuresPhi);


        // y-axis fill value
        // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
        // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
        // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
        // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
        float m_secLayer = m_sectorNo + 0.2 * (m_wireLayer - 1) + 0.1;  
        int xfac = m_measuresPhi ? -1 : 1;        // -48 / +192

        // this way we get 4 time samples per strip
        // no of strips = width
        ATH_MSG_DEBUG ( " Width of ROD cluster   : " << m_raw->width()  );
        uint16_t m_diff_max = 0, m_diff;
        std::vector<float> m_xVals ; /*, m_yVals;*/
        // loop over strips in ROD cluster
        size_t m_raw_clus_width =  m_raw->width();

        if(!m_measuresPhi) {
          m_h2csc_rdo_etacluswidth->Fill(m_raw_clus_width,m_secLayer);  // fill eta-cluster width
          m_nEtaClusWidthCnt++;
        } else {
          m_h2csc_rdo_phicluswidth->Fill(m_raw_clus_width,m_secLayer);  // fill phi-cluster width
          m_nPhiClusWidthCnt++;
        }

        // loop over strips
        for (size_t n = 0; n < m_raw_clus_width; n++) {

          // identify this strip
          Identifier m_chID = m_cscRdoDecoderTool->channelIdentifier(m_raw, n);
          int m_strip = m_cscIdHelper->strip(m_chID);
          float m_stripId = m_strip * xfac;         // x-axis fill value

          m_h2csc_rdo_hitmap->Fill(m_stripId, m_secLayer);  // fill hitmap 

          // for every strip that has a hit, store the X,Y values
          m_xVals.push_back(m_stripId);
          //m_yVals.push_back(m_secLayer);

          // extract the (four) time samples for this strip
          std::vector<uint16_t> samples;
          bool extractSamples = m_raw->samples(n, m_rdo->numSamples(), samples);
          uint16_t n_max = 0, n_min = 9999;

          // if we have the time samples, identify the max/min sampling values i.e., ADC_max and ADC_min
          if(extractSamples) {
            for(size_t np = 0; np < samples.size(); np++) {
              if(samples[np] < n_min) n_min = samples[np];
              if(samples[np] > n_max) n_max = samples[np];
            }
            // the diff between max and min samplings Delta_ADC = (ADC_max - ADC_min)
            m_diff = n_max - n_min;

            // compute the max difference Max_Delta_ADC
            if(m_diff > m_diff_max) m_diff_max = m_diff;

            ATH_MSG_DEBUG ( n << " Max = " << n_max << "  Min = " << n_min << " Diff = " << m_diff  );
          } // end if extractSamples

        } // end for loop over strips in cluster
        ATH_MSG_DEBUG ( " End loop over strips======================" );

        ATH_MSG_DEBUG ( " Max difference                : " << m_diff_max  );

        m_h1csc_rdo_maxdiffamp->Fill(m_diff_max);  // fill Max_Delta_ADC

        // determine of the cluster is a noise/signal cluster Max_Delta_ADC > NoiseCut
        bool m_signal = m_diff_max > m_cscNoiseCut ? true : false;
        if(m_signal) m_sigclusCount[m_ns][m_nl]++; 
        if(m_signal) {
          for(size_t nf = 0; nf < m_xVals.size(); nf++) {
            m_h2csc_rdo_hitmap_signal->Fill(m_xVals[nf],m_secLayer);
            if(m_stationEta == -1) {
              m_h2csc_rdo_hitmap_signal_EC->Fill(m_xVals[nf],m_secLayer);
              m_h1csc_rdo_hitmap_signal_EC_count->Fill(m_xVals[nf]);
              m_h1csc_rdo_hitmap_signal_EC_occupancy->Fill(m_secLayer);
            } else {
              m_h2csc_rdo_hitmap_signal_EA->Fill(m_xVals[nf],m_secLayer);
              m_h1csc_rdo_hitmap_signal_EA_count->Fill(m_xVals[nf]);
              m_h1csc_rdo_hitmap_signal_EA_occupancy->Fill(m_secLayer);
            }
          }
          if(!m_measuresPhi) {
            m_h2csc_rdo_etacluswidth_signal->Fill(m_raw_clus_width,m_secLayer);
            if(m_stationEta == -1) {
              m_h2csc_rdo_etacluswidth_signal_EC->Fill(m_raw_clus_width,m_secLayer);
              m_h1csc_rdo_etacluswidth_signal_EC_count->Fill(m_raw_clus_width);
              m_h1csc_rdo_etacluswidth_signal_EC_occupancy->Fill(m_secLayer);
            } else {
              m_h2csc_rdo_etacluswidth_signal_EA->Fill(m_raw_clus_width,m_secLayer);
              m_h1csc_rdo_etacluswidth_signal_EA_count->Fill(m_raw_clus_width);
              m_h1csc_rdo_etacluswidth_signal_EA_occupancy->Fill(m_secLayer);
            }
          }else {
            m_h2csc_rdo_phicluswidth_signal->Fill(m_raw_clus_width,m_secLayer);
            if(m_stationEta == -1) {
              m_h2csc_rdo_phicluswidth_signal_EC->Fill(m_raw_clus_width,m_secLayer);
              m_h1csc_rdo_phicluswidth_signal_EC_count->Fill(m_raw_clus_width);
              m_h1csc_rdo_phicluswidth_signal_EC_occupancy->Fill(m_secLayer);
            } else {
              m_h2csc_rdo_phicluswidth_signal_EA->Fill(m_raw_clus_width,m_secLayer);
              m_h1csc_rdo_phicluswidth_signal_EA_count->Fill(m_raw_clus_width);
              m_h1csc_rdo_phicluswidth_signal_EA_occupancy->Fill(m_secLayer);
            }
          }
        } else {
          for(size_t nf = 0; nf < m_xVals.size(); nf++) 
            m_h2csc_rdo_hitmap_noise->Fill(m_xVals[nf],m_secLayer);
          if(!m_measuresPhi) {
            m_h2csc_rdo_etacluswidth_noise->Fill(m_raw_clus_width,m_secLayer);
          }else {
            m_h2csc_rdo_phicluswidth_noise->Fill(m_raw_clus_width,m_secLayer);
          }
        }
      } // if m_raw (CscRawData)
    } // end for loop over ROD-clusters
    ATH_MSG_DEBUG ( " End loop over clusters======================" );


    m_h2csc_rdo_eta_vs_phi_cluswidth->Fill(m_nPhiClusWidthCnt,m_nEtaClusWidthCnt);

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
            m_h2csc_rdo_phicluscount->Fill(m_count,m_secLayer); // all phi-cluster counts
            m_numphi += m_count; 
            if(m_scount) {
              m_numphisignal += m_scount; 
              m_h2csc_rdo_phicluscount_signal->Fill(m_scount,m_secLayer); // signal phi-cluster count
              m_h2csc_rdo_phicluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise phi-cluster count
            } else {
              m_h2csc_rdo_phicluscount_noise->Fill(m_count,m_secLayer); // noise phi-cluster count
            }
          } else {
            m_h2csc_rdo_etacluscount->Fill(m_count,m_secLayer);
            m_numeta += m_count; 
            if(m_scount) {
              m_numetasignal += m_scount; 
              m_h2csc_rdo_etacluscount_signal->Fill(m_scount,m_secLayer); // signal eta-cluster count
              m_h2csc_rdo_etacluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise eta-cluster count
            } else {
              m_h2csc_rdo_etacluscount_noise->Fill(m_count,m_secLayer); // noise eta-cluster count
            } 
          }
          ATH_MSG_DEBUG ( m_wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " << 
              m_secLayer << "\t = " << m_count << "\t" << m_scount);
        }
      }
    }

    m_h2csc_rdo_eta_vs_phi_cluscount->Fill(m_numphi,m_numeta);
    m_h2csc_rdo_eta_vs_phi_cluscount_signal->Fill(m_numphisignal,m_numetasignal);
    m_h2csc_rdo_eta_vs_phi_cluscount_noise->Fill(m_numphi-m_numphisignal, m_numeta-m_numetasignal);

  } // end for if (container)
  ATH_MSG_DEBUG ( " END EVENT ============================================ "  );
  // End Event ====================================================

  ATH_MSG_DEBUG ( "done collecting histograms"  );
  ATH_MSG_DEBUG ( "CSCRdoMon::fillHistograms reports success"  );

  return sc;
}

//
// procHistograms ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::procHistograms() {
  ATH_MSG_DEBUG ( "CscRdoValAlg : in procHistograms()"  );
  StatusCode sc = StatusCode::SUCCESS;
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){}
  /*if(isEndOfRun){
    std::string m_cscGenPath = m_cscRDOPath.substr(0,m_cscRDOPath.find("CSC"));
  //MonGroup cscrdo_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
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
  CscRdoBins::RdoBinLabels(m_hY,-1);
  // register histogram with Overview/CSCEC
  sc = cscrdo_oviewEC->regHist(m_hY);
  if ( sc.isFailure() ) {
  ATH_MSG_ERROR (  "Cannot register histogram " << m_hY->GetName() );
  return sc;
  }
  // Get X-projection (counts)
  TH1D *m_hX = dynamic_cast<TH2F* >(m_h)->ProjectionX(Form("%s_hX",m_hname.c_str()),0,-1,"e");
  sc = cscrdo_oviewEC->regHist(m_hX);
  if ( sc.isFailure() ) {
  ATH_MSG_ERROR (  "Cannot register histogram " << m_hX->GetName() );
  return sc;
  }
  } // end if hist2d
  } // end if m_h
  } // end for

  //MonGroup cscrdo_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
  for(size_t j = 0; j < m_regHOviewEA.size(); j++ ) {
  TH1 *m_h(0);
  m_h = m_regHOviewEA[j];
  if(m_h != NULL) {
  bool m_hist2d = m_h->IsA()->InheritsFrom("TH2");
  if(m_hist2d) {
  std::string m_hname = m_h->GetName();
  // Get Y-projection (sec+0.2*lay)
  TH1D *m_hY = dynamic_cast<TH2F* >(m_h)->ProjectionY(Form("%s_hY",m_hname.c_str()),0,-1,"");
  // set bin labels
  CscRdoBins::RdoBinLabels(m_hY,1);
  // register histogram with Overview/CSCEA
  sc = cscrdo_oviewEA->regHist(m_hY);
  if ( sc.isFailure() ) {
  ATH_MSG_ERROR (  "Cannot register histogram " << m_hY->GetName() );
  return sc;
  }
  // Get X-projection (counts)
  TH1D *m_hX = dynamic_cast<TH2F* >(m_h)->ProjectionX(Form("%s_hX",m_hname.c_str()),0,-1,"e");
  sc = cscrdo_oviewEA->regHist(m_hX);
  if ( sc.isFailure() ) {
  ATH_MSG_ERROR (  "Cannot register histogram " << m_hX->GetName() );
  return sc;
  }
  } // end if hist2d
  } // end if m_h
  } // end for

  } // end isEndOfRun*/

  return sc;
}


//
// checkHists ----------------------------------------------------------------
//
StatusCode CscRdoValAlg::checkHists(bool /* fromFinalize */) {

  ATH_MSG_DEBUG ( "CscRdoValAlg : in checkHists()"  );

  return StatusCode::SUCCESS;
}
