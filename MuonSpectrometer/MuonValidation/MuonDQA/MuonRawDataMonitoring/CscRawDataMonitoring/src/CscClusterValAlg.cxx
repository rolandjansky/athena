/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*   NAME    : CscClusterValAlg.cxx
 *   PACKAGE : MuonRawDataMonitoring/CscRawDataMonitoring
 *   PURPOSE : CSC cluster monitoring
 *   AUTHOR  : Venkatesh Kaushik <venkat.kaushik@cern.ch> (2009-04-27)
 *   
 *   MODIFIED: 
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Athena include(s)
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "CscRawDataMonitoring/CscClusterValAlg.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscCalibTools/ICscCalibTool.h"

// ROOT include(s)
#include "TH1F.h"
#include "TH2F.h"
#include "TClass.h"

// STL include(s)
#include <bitset>
#include <math.h>

using namespace Muon;

namespace CscBins {
  void BinLabels(TH1 *h, int m_side) {
    h->GetXaxis()->SetTitle("");
    h->GetXaxis()->SetLabelSize(0.03);
    if(m_side == -1) {
      for(unsigned int j=6; j<86; j++) {
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
      for(unsigned int j=6; j<86; j++) {
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
CscClusterValAlg::CscClusterValAlg(const std::string & type, 
    const std::string & name, const IInterface* parent) :
  ManagedMonitorToolBase( type, name, parent ), 
  m_cscIdHelper(0),
  m_stripFitter(name),
  m_cscCalibTool(name),
  m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
  cscclus_oviewEA(0),
  cscclus_oviewEC(0)
{

  declareProperty("CSCClusterKey", m_cscClusterKey = "CSC_Clusters");
  declareProperty("CSCClusterPath", m_cscClusterPath = "Muon/MuonRawDataMonitoring/CSC/Clusters");
  declareProperty("CSCPrepRawDataKey", m_cscPRDKey = "CSC_Measurements");
  declareProperty("CSCQmaxCutADC", m_qmaxADCCut = 100);
  declareProperty("CSCStripFitter", m_stripFitter);
  declareProperty("CSCCalibTool", m_cscCalibTool);

  //trigger aware monitoring
  declareProperty("CSCTrigDecisionTool", m_trigDec );
  declareProperty("CSCDoEventSelection",   m_doEvtSel = false );
  declareProperty("CSCEventSelTriggers", m_sampSelTriggers );

  m_cscGenPath = m_cscClusterPath.substr(0,m_cscClusterPath.find("CSC"));

  initHistograms();

}

//
// destructor ----------------------------------------------------------------
//
CscClusterValAlg::~CscClusterValAlg() {
  if(cscclus_oviewEA) {
    delete cscclus_oviewEA;
    cscclus_oviewEA = 0;
  }
  if(cscclus_oviewEC) {
    delete cscclus_oviewEC;
    cscclus_oviewEC = 0;
  }

  ATH_MSG_DEBUG( "CscClusterValAlg: in destructor" );
}


//
// initialize ----------------------------------------------------------------
//
StatusCode CscClusterValAlg::initialize(){

  ATH_MSG_INFO ( "Initializing        : " << name() );
  ATH_MSG_INFO ( "CSCClusterKey       : " << m_cscClusterKey );
  ATH_MSG_INFO ( "CSCPrepRawDataKey   : " << m_cscPRDKey );

  StatusCode sc;
  sc = detStore()->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Cannot get CscIdHelper"  );
    return sc;
  } else {
    ATH_MSG_DEBUG ("CSCIdHelper         : " << "Using CscIdhelper " );
  }

  sc = m_stripFitter.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING ( "CscClusterValAlg: Unable to retrieve strip fitter");
    return sc;
  } else {
    ATH_MSG_INFO ( "CSCStripFitter      : " << "Using Fitter with name \"" << m_stripFitter->name() << "\"" );
  }

  if( m_doEvtSel ) {
    sc = m_trigDec.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING ( "CscClusterValAlg: Unable to retrieve trigger decision tool");
      m_doEvtSel = false;
      //return sc;
    } else {
      ATH_MSG_INFO ( "TrigDecisionTool    : " << "Using TDT \"" << m_trigDec->name() << "\"" );
    }
    //m_doEvtSel = false;
  }

  sc = m_cscCalibTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING ( "CscClusterValAlg: Unable to retrieve cluster fitter ");
    return sc;
  } else {
    ATH_MSG_INFO ( "CSCCalibTool        : " << "Using calib tool with name \"" << m_cscCalibTool->name() << "\"" );
  }

  if(m_sampSelTriggers.empty() && m_doEvtSel) {
    ATH_MSG_WARNING("Event selection triggers not specified. Switching off trigger-aware monitoring");
    m_doEvtSel = false;
  }

  ManagedMonitorToolBase::initialize().ignore();
  return StatusCode::SUCCESS;
}


//
// initHistograms  ----------------------------------------------------------------
//
void CscClusterValAlg::initHistograms() {

  // hitmaps
  m_h2csc_clus_hitmap = 0;
  m_h2csc_clus_hitmap_noise = 0;
  m_h2csc_clus_hitmap_signal = 0;
  m_h2csc_clus_segmap_signal = 0;

  m_h2csc_clus_eta_vs_phi_hitmap = 0;
  m_h2csc_clus_r_vs_z_hitmap = 0;
  m_h2csc_clus_y_vs_x_hitmap = 0;

  // layer occupancy
  m_h1csc_clus_occupancy_signal_EA = 0;
  m_h1csc_clus_occupancy_signal_EC = 0;

  // q_max of cluster
  m_h2csc_clus_qmax = 0;
  m_h2csc_clus_qmax_noise = 0;
  m_h2csc_clus_qmax_signal = 0;

  m_h2csc_clus_qmax_signal_EA = 0;
  m_h1csc_clus_qmax_signal_EA_count = 0;
  //m_h1csc_clus_qmax_signal_EA_occupancy = 0;

  m_h2csc_clus_qmax_signal_EC = 0;
  m_h1csc_clus_qmax_signal_EC_count = 0;
  //m_h1csc_clus_qmax_signal_EC_occupancy = 0;

  // q_sum = q_max + q_left + q_right of cluster
  m_h2csc_clus_qsum = 0;
  m_h2csc_clus_qsum_noise = 0;
  m_h2csc_clus_qsum_signal = 0;

  m_h2csc_clus_qsum_signal_EA = 0;
  m_h1csc_clus_qsum_signal_EA_count = 0;
  //m_h1csc_clus_qsum_signal_EA_occupancy = 0;
  m_h1csc_clus_qsum_signal_EA_lfitmean = 0;

  m_h2csc_clus_qsum_signal_EC = 0;
  m_h1csc_clus_qsum_signal_EC_count = 0;
  //m_h1csc_clus_qsum_signal_EC_occupancy = 0;
  m_h1csc_clus_qsum_signal_EC_lfitmean = 0;

  // sampling time - eta cluster
  m_h1csc_clus_precision_time = 0;
  m_h1csc_clus_precision_time_noise = 0;
  m_h1csc_clus_precision_time_signal = 0;
  m_h1csc_clus_precision_time_signal_EA = 0;
  m_h1csc_clus_precision_time_signal_EC = 0;

  // sampling time - phi cluster
  m_h1csc_clus_transverse_time = 0;
  m_h1csc_clus_transverse_time_noise = 0;
  m_h1csc_clus_transverse_time_signal = 0;

  // sampling time - phi cluster
  m_h1csc_clus_transverse_time = 0;
  m_h1csc_clus_transverse_time_noise = 0;
  m_h1csc_clus_transverse_time_signal = 0;

  // cluster charge - eta cluster
  m_h1csc_clus_precision_charge = 0;
  m_h1csc_clus_precision_charge_noise = 0;
  m_h1csc_clus_precision_charge_signal = 0;

  // cluster charge - phi cluster
  m_h1csc_clus_transverse_charge = 0;
  m_h1csc_clus_transverse_charge_noise = 0;
  m_h1csc_clus_transverse_charge_signal = 0;

  // cluster count - phi layer // 
  m_h2csc_clus_phicluscount = 0;
  m_h2csc_clus_phicluscount_signal = 0;
  m_h2csc_clus_phicluscount_noise = 0;

  // cluster count - eta layer // 
  m_h2csc_clus_etacluscount = 0;
  m_h2csc_clus_etacluscount_signal = 0;
  m_h2csc_clus_etacluscount_noise = 0;

  m_h1csc_clus_count = 0;
  m_h1csc_clus_count_signal = 0;
  m_h1csc_clus_count_noise = 0;

  m_h1csc_clus_count_perlayer = 0;

  // cluster width - eta layer
  m_h2csc_clus_etacluswidth = 0;
  m_h2csc_clus_etacluswidth_signal = 0;
  m_h2csc_clus_etacluswidth_noise = 0;

  // cluster width - phi layer
  m_h2csc_clus_phicluswidth = 0;
  m_h2csc_clus_phicluswidth_signal = 0;
  m_h2csc_clus_phicluswidth_noise = 0;

  //total cluster width
  m_h1csc_clus_totalWidth_EA = 0;
  m_h1csc_clus_totalWidth_EC = 0;

  // correlation plots
  m_h2csc_clus_eta_vs_phi_charge = 0;
  m_h2csc_clus_eta_vs_phi_charge_noise = 0;
  m_h2csc_clus_eta_vs_phi_charge_signal = 0;

  // hit correlation b/w eta/phi clusters
  m_h2csc_clus_eta_vs_phi_cluscount = 0;
  m_h2csc_clus_eta_vs_phi_cluscount_noise = 0;
  m_h2csc_clus_eta_vs_phi_cluscount_signal = 0;

  // width correlation b/w eta/phi clusters
  m_h2csc_clus_eta_vs_phi_cluswidth = 0;
  m_h2csc_clus_eta_vs_phi_cluswidth_signal = 0;
  m_h2csc_clus_eta_vs_phi_cluswidth_noise = 0;

}


//
// bookClusterHistograms  ----------------------------------------------------------------
//
void CscClusterValAlg::bookClusterHistograms() {

  m_cscClusExpert.clear();
  m_cscClusShift.clear();
  m_cscClusOviewEA.clear();
  m_cscClusOviewEC.clear();

  ///******************** DO NOT MODIFY (begin) ***********************************************///
  int nxbins  = 242;  // 192 bins for precision, 48 for transverse strips + 1 set extra
  float nxmin = -49.; // -1 -> -48 (for transverse)
  float nxmax = 193.; // 1 -> 192 (for precision)

  int nybins  = 175;  // 32 chambers (16 per side x 5 layers per chamber) + 1 set extra
  float nymin = -17.; //
  float nymax = 18.;  //

  int nybinsEA  = 90;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEA =  0.; //
  float nymaxEA = 18.;  //

  int nybinsEC  = 85;  // 16 chambers (x 5 layers per chamber) + 1 set extra
  float nyminEC = -17.; //
  float nymaxEC = 0.;  //

  // kilo electrons
  int nqbins = 400;
  float nqmin = 0.;
  float nqmax = 8000;

  // time
  int ntbins = 260;
  float ntmin = -60;
  float ntmax = 200;

  //total cluster width
  int Nxbins  = 15360;  //16 chambers * [(192 eta-strips * 4 layers) + (48 phi-strips * 4 layers)] = 15360 channels
  float Nxmin = 1.; 
  float Nxmax = 15361.;
  ///******************** DO NOT MODIFY (end) ***********************************************///


  // cluster hitmap
  m_h2csc_clus_hitmap = new TH2F("h2csc_clus_hitmap", 
      "Cluster occupancy ;channel;[sector] + [0.2 #times layer]",nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_clus_hitmap_noise = new TH2F("h2csc_clus_hitmap_noise", 
      Form("Cluster occupancy, Qmax #leq %4u counts;channel;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_clus_hitmap_signal = new TH2F("h2csc_clus_hitmap_signal", 
      Form("Cluster occupancy, Qmax > %4u counts;channel;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nxbins,nxmin,nxmax,nybins,nymin,nymax);

  m_h2csc_clus_segmap_signal = new TH2F("h2csc_clus_segmap_signal", 
      Form("Segment occupancy, Qmax > %4u counts;segment;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      16,-0.5,15.5,nybins,nymin,nymax);

  // layer occupancy
  m_h1csc_clus_occupancy_signal_EA = new TH1F("h1csc_clus_occupancy_signal_EA", 
      Form("EndCap A: Layer occupancy, Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEA,nyminEA,nymaxEA);
  CscBins::BinLabels(m_h1csc_clus_occupancy_signal_EA,1);

  m_h1csc_clus_occupancy_signal_EC = new TH1F("h1csc_clus_occupancy_signal_EC", 
      Form("EndCap C: Layer occupancy, Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEC,nyminEC,nymaxEC);
  CscBins::BinLabels(m_h1csc_clus_occupancy_signal_EC,1);

  // cluster qsum
  m_h2csc_clus_qsum = new TH2F("h2csc_clus_qsum", "Cluster charge (Qsum);counts;[sector] + [0.2 #times layer]",
      nqbins,nqmin,nqmax,nybins,nymin,nymax);
  m_h2csc_clus_qsum_noise = new TH2F("h2csc_clus_qsum_noise", 
      Form("Cluster charge(Qsum), Qmax #leq %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybins,nymin,nymax);

  m_h2csc_clus_qsum_signal = new TH2F("h2csc_clus_qsum_signal", 
      Form("Cluster charge(Qsum), Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybins,nymin,nymax);

  m_h2csc_clus_qsum_signal_EA = new TH2F("h2csc_clus_qsum_signal_EA", 
      Form("EndCap A: Cluster charge(Qsum), Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA);

  m_h1csc_clus_qsum_signal_EA_count = new TH1F("h1csc_clus_qsum_signal_EA_count", 
      Form("EndCap A: Cluster charge(Qsum), Qmax > %4u counts;counts;entries/20 counts;",m_qmaxADCCut),nqbins,nqmin,nqmax);

  /*
  m_h1csc_clus_qsum_signal_EA_occupancy = new TH1F("h1csc_clus_qsum_signal_EA_occupancy", 
      Form("EndCap A: Cluster charge(Qsum), Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEA,nyminEA,nymaxEA);
  CscBins::BinLabels(m_h1csc_clus_qsum_signal_EA_occupancy,1);
  */

  m_h1csc_clus_qsum_signal_EA_lfitmean = new TH1F("h1csc_clus_qsum_signal_EA_lfitmean", 
      Form("EndCap A: MPV of Landau fit to Cluster charge(Qsum);[sector] + [0.2 #times layer];counts/layer"),nybinsEA,nyminEA,nymaxEA);
  CscBins::BinLabels(m_h1csc_clus_qsum_signal_EA_lfitmean,1);

  m_h2csc_clus_qsum_signal_EC = new TH2F("h2csc_clus_qsum_signal_EC", 
      Form("EndCap C: Cluster charge(Qsum), Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC);

  m_h1csc_clus_qsum_signal_EC_count = new TH1F("h1csc_clus_qsum_signal_EC_count", 
      Form("EndCap C: Cluster charge(Qsum), Qmax > %4u counts;counts;entries/20 counts;",m_qmaxADCCut),nqbins,nqmin,nqmax);

  /*
  m_h1csc_clus_qsum_signal_EC_occupancy = new TH1F("h1csc_clus_qsum_signal_EC_occupancy", 
      Form("EndCap C: Cluster charge(Qsum), Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEC,nyminEC,nymaxEC);
  CscBins::BinLabels(m_h1csc_clus_qsum_signal_EC_occupancy,-1);
  */

  m_h1csc_clus_qsum_signal_EC_lfitmean = new TH1F("h1csc_clus_qsum_signal_EC_lfitmean", 
      Form("EndCap C: MPV of Landau fit to Cluster charge(Qsum);[sector] + [0.2 #times layer];counts/layer"),nybinsEC,nyminEC,nymaxEC);
  CscBins::BinLabels(m_h1csc_clus_qsum_signal_EC_lfitmean,-1);

  // cluster qmax
  m_h2csc_clus_qmax = new TH2F("h2csc_clus_qmax", "Cluster peak-strip charge, Qmax;counts;[sector] + [0.2 #times layer]",
      nqbins,nqmin,nqmax,nybins,nymin,nymax);
  m_h2csc_clus_qmax_noise = new TH2F("h2csc_clus_qmax_noise", 
      Form("Cluster peak-strip charge, Qmax #leq %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybins,nymin,nymax);
  m_h2csc_clus_qmax_signal = new TH2F("h2csc_clus_qmax_signal", 
      Form("Cluster peak-strip charge, Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybins,nymin,nymax);

  m_h2csc_clus_qmax_signal_EA = new TH2F("h2csc_clus_qmax_signal_EA", 
      Form("EndCap A: Cluster peak-strip charge, Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybinsEA,nyminEA,nymaxEA);

  m_h1csc_clus_qmax_signal_EA_count = new TH1F("h1csc_clus_qmax_signal_EA_count", 
      Form("EndCap A: Cluster peak-strip charge, Qmax > %4u counts;counts;entries/20 counts;",m_qmaxADCCut),nqbins,nqmin,nqmax);

  /*
  m_h1csc_clus_qmax_signal_EA_occupancy = new TH1F("h1csc_clus_qmax_signal_EA_occupancy", 
      Form("EndCap A: Cluster peak-strip charge, Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEA,nyminEA,nymaxEA);
  CscBins::BinLabels(m_h1csc_clus_qmax_signal_EA_occupancy,1);
  */

  m_h2csc_clus_qmax_signal_EC = new TH2F("h2csc_clus_qmax_signal_EC", 
      Form("EndCap C: Cluster peak-strip charge, Qmax > %4u counts;counts;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      nqbins,nqmin,nqmax,nybinsEC,nyminEC,nymaxEC);

  m_h1csc_clus_qmax_signal_EC_count = new TH1F("h1csc_clus_qmax_signal_EC_count", 
      Form("EndCap C: Cluster peak-strip charge, Qmax > %4u counts;counts;entries/20 counts;",m_qmaxADCCut),nqbins,nqmin,nqmax);

  /*
  m_h1csc_clus_qmax_signal_EC_occupancy = new TH1F("h1csc_clus_qmax_signal_EC_occupancy", 
      Form("EndCap C: Cluster peak-strip charge, Qmax > %4u counts;[sector] + [0.2 #times layer];entries/layer",m_qmaxADCCut),nybinsEC,nyminEC,nymaxEC);
  CscBins::BinLabels(m_h1csc_clus_qmax_signal_EC_occupancy,-1);
  */

  // eta-cluster sampling time
  m_h1csc_clus_precision_time = new TH1F("h1csc_clus_precision_time", 
      "#eta-cluster sampling time;ns;entries/ns",ntbins,ntmin,ntmax );
  m_h1csc_clus_precision_time_noise = new TH1F("h1csc_clus_precision_time_noise", 
      Form("#eta-cluster sampling time, Qmax #leq %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );
  m_h1csc_clus_precision_time_signal = new TH1F("h1csc_clus_precision_time_signal", 
      Form("#eta-cluster sampling time, Qmax > %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );

  m_h1csc_clus_precision_time_signal_EA = new TH1F("h1csc_clus_precision_time_signal_EA", 
      Form("EndCap A: #eta-cluster sampling time, Qmax > %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );
  m_h1csc_clus_precision_time_signal_EC = new TH1F("h1csc_clus_precision_time_signal_EC", 
      Form("EndCap C: #eta-cluster sampling time, Qmax > %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );

  // eta-cluster charge
  m_h1csc_clus_precision_charge = new TH1F("h1csc_clus_precision_charge", 
      "#eta-cluster charge;counts;entries/count",nqbins,nqmin,nqmax );
  m_h1csc_clus_precision_charge_noise = new TH1F("h1csc_clus_precision_charge_noise", 
      Form("#eta-cluster charge, Qmax #leq %4u counts;counts;entries/count",m_qmaxADCCut),nqbins,nqmin,nqmax );
  m_h1csc_clus_precision_charge_signal = new TH1F("h1csc_clus_precision_charge_signal", 
      Form("#eta-cluster charge, Qmax > %4u counts;counts;entries/count",m_qmaxADCCut),nqbins,nqmin,nqmax );

  // phi-cluster sampling time
  m_h1csc_clus_transverse_time = new TH1F("h1csc_clus_transverse_time", 
      "#phi-cluster sampling time;ns;entries/ns",ntbins,ntmin,ntmax );
  m_h1csc_clus_transverse_time_noise = new TH1F("h1csc_clus_transverse_time_noise", 
      Form("#phi-cluster sampling time, Qmax #leq %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );
  m_h1csc_clus_transverse_time_signal = new TH1F("h1csc_clus_transverse_time_signal", 
      Form("#phi-cluster sampling time, Qmax > %4u counts;ns;entries/ns",m_qmaxADCCut),ntbins,ntmin,ntmax );

  // phi-cluster charge
  m_h1csc_clus_transverse_charge = new TH1F("h1csc_clus_transverse_charge", 
      "#phi-cluster charge;counts;entries/count",nqbins,nqmin,nqmax );
  m_h1csc_clus_transverse_charge_noise = new TH1F("h1csc_clus_transverse_charge_noise", 
      Form("#phi-cluster charge, Qmax #leq %4u counts;counts;entries/count",m_qmaxADCCut),nqbins,nqmin,nqmax );
  m_h1csc_clus_transverse_charge_signal = new TH1F("h1csc_clus_transverse_charge_signal", 
      Form("#phi-cluster charge, Qmax > %4u counts;counts;entries/count",m_qmaxADCCut),nqbins,nqmin,nqmax );


  // eta-cluster width
  m_h2csc_clus_etacluswidth = new TH2F("h2csc_clus_etacluswidth",
      "#eta-cluster width;# strips;[sector] + [0.2 #times layer]",
      192,0,192,nybins,nymin,nymax);
  m_h2csc_clus_etacluswidth_noise = new TH2F("h2csc_clus_etacluswidth_noise",
      Form("#eta-cluster width, Qmax #leq %4u counts;# strips;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      192,0,192,nybins,nymin,nymax);
  m_h2csc_clus_etacluswidth_signal = new TH2F("h2csc_clus_etacluswidth_signal",
      Form("#eta-cluster width, Qmax > %4u counts;# strips;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      192,0,192,nybins,nymin,nymax);

  // phi-cluster width
  m_h2csc_clus_phicluswidth = new TH2F("h2csc_clus_phicluswidth",
      "#phi-cluster width;# strips;[sector] + [0.2 #times layer]",
      48,0,48,nybins,nymin,nymax);
  m_h2csc_clus_phicluswidth_noise = new TH2F("h2csc_clus_phicluswidth_noise",
      Form("#phi-cluster width, Qmax #leq %4u counts;# strips;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      48,0,48,nybins,nymin,nymax);
  m_h2csc_clus_phicluswidth_signal = new TH2F("h2csc_clus_phicluswidth_signal",
      Form("#phi-cluster width, Qmax > %4u counts;# strips;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      48,0,48,nybins,nymin,nymax);

  //total cluster width
  m_h1csc_clus_totalWidth_EA = new TH1F("h1csc_clus_totalWidth_EA","EndCap A: Cluster hits in all EA eta(#eta) & phi(#phi) strips;strips;cluster hits",Nxbins,Nxmin,Nxmax);
 
  m_h1csc_clus_totalWidth_EC = new TH1F("h1csc_clus_totalWidth_EC","EndCap C: Cluster hits in all EC eta(#eta) & phi(#phi) strips;strips;cluster hits",Nxbins,Nxmin,Nxmax);

  // eta-cluster count
  m_h2csc_clus_etacluscount = new TH2F("h2csc_clus_etacluscount",
      "#eta-cluster count;no.of clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_clus_etacluscount_noise = new TH2F("h2csc_clus_etacluscount_noise",
      Form("#eta-cluster count, Qmax #leq %4u counts;# clusters;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      20,0,20,nybins,nymin,nymax);
  m_h2csc_clus_etacluscount_signal = new TH2F("h2csc_clus_etacluscount_signal",
      Form("#eta-cluster count, Qmax > %4u counts;# clusters;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      20,0,20,nybins,nymin,nymax);

  // phi-cluster count
  m_h2csc_clus_phicluscount = new TH2F("h2csc_clus_phicluscount",
      "#phi-cluster count;# clusters;[sector] + [0.2 #times layer]",
      20,0,20,nybins,nymin,nymax);
  m_h2csc_clus_phicluscount_noise = new TH2F("h2csc_clus_phicluscount_noise",
      Form("#phi-cluster count, Qmax #leq %4u counts;# clusters;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      20,0,20,nybins,nymin,nymax);
  m_h2csc_clus_phicluscount_signal = new TH2F("h2csc_clus_phicluscount_signal",
      Form("#phi-cluster count, Qmax > %4u counts;# clusters;[sector] + [0.2 #times layer]",m_qmaxADCCut),
      20,0,20,nybins,nymin,nymax);

  m_h1csc_clus_count = new TH1F("h1csc_clus_count", "Clusters per event;no.of clusters;entries",26,-1,25);
  m_h1csc_clus_count_noise = new TH1F("h1csc_clus_count_noise", 
      Form("Clusters per event, Qmax #leq %4u counts;no.of clusters;entries",m_qmaxADCCut),26,-1,25);
  m_h1csc_clus_count_signal = new TH1F("h1csc_clus_count_signal", 
      Form("Clusters per event, Qmax > %4u counts;no.of clusters;entries",m_qmaxADCCut),26,-1,25);

  //m_h1csc_clus_count_perlayer = new TH1F("h1csc_clus_count_perlayer", 
  //    Form("Clusters per layer, Qmax > %4u counts;[sector] + [0.2 #times layer];# clusters",m_qmaxADCCut),170,-17,17);

  // correlation histograms
  m_h2csc_clus_eta_vs_phi_cluscount = new TH2F("h2csc_clus_eta_vs_phi_cluscount",
      "Eta vs. Phi Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_clus_eta_vs_phi_cluscount_noise = new TH2F("h2csc_clus_eta_vs_phi_cluscount_noise",
      "Eta vs. Phi Noise-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);
  m_h2csc_clus_eta_vs_phi_cluscount_signal = new TH2F("h2csc_clus_eta_vs_phi_cluscount_signal",
      "Eta vs. Phi Signal-Cluster count correlation;#varphi-cluster count;#eta-cluster count",100,0,100,100,0,100);

  m_h2csc_clus_eta_vs_phi_cluswidth = new TH2F("h2csc_clus_eta_vs_phi_cluswidth",
      "Eta vs. Phi Cluster width correlation;#varphi-cluster width;#eta-cluster width",100,0,100,100,0,100);

  //m_h2csc_clus_eta_vs_phi_hitmap = new TH2F("h2csc_clus_eta_vs_phi_hitmap",
  //    "Eta vs. Phi Cluster hitmap;#varphi-cluster position;#eta-cluster position",68,-3.4,3.4,68,-3.4,3.4); // 2.0 < |eta-csc| < 2.8

  m_h2csc_clus_r_vs_z_hitmap = new TH2F("h2csc_clus_r_vs_z_hitmap",
      "R vs. Z Cluster hitmap;z(mm);R(mm)",200, -10000., 10000., 40, 0., 4000);

  m_h2csc_clus_y_vs_x_hitmap = new TH2F("h2csc_clus_y_vs_x_hitmap",
      "Y vs. X Cluster hitmap;x(mm);y(mm)",100, -5000., 5000., 100, -5000., 5000);


  // hitmaps
  m_cscClusExpert.push_back(m_h2csc_clus_hitmap);               // expert
  m_cscClusShift.push_back(m_h2csc_clus_hitmap_signal);         // shift
  m_cscClusExpert.push_back(m_h2csc_clus_segmap_signal);        // expert
  m_cscClusExpert.push_back(m_h2csc_clus_hitmap_noise);         // expert

  //m_cscClusShift.push_back(m_h2csc_clus_eta_vs_phi_hitmap);
  m_cscClusShift.push_back(m_h2csc_clus_r_vs_z_hitmap);         // shift
  m_cscClusShift.push_back(m_h2csc_clus_y_vs_x_hitmap);         // shift

  // layer occupancy
  m_cscClusOviewEA.push_back(m_h1csc_clus_occupancy_signal_EA);
  m_cscClusOviewEC.push_back(m_h1csc_clus_occupancy_signal_EC);

  // qsum
  m_cscClusExpert.push_back(m_h2csc_clus_qsum);                 // expert
  m_cscClusShift.push_back(m_h2csc_clus_qsum_signal);           // shift, overview, dq-flag(Q3 is landau)
  m_cscClusExpert.push_back(m_h2csc_clus_qsum_noise);           // expert

  m_cscClusOviewEA.push_back(m_h2csc_clus_qsum_signal_EA);           
  m_cscClusOviewEA.push_back(m_h1csc_clus_qsum_signal_EA_count);           
  //m_cscClusOviewEA.push_back(m_h1csc_clus_qsum_signal_EA_occupancy);           
  m_cscClusOviewEA.push_back(m_h1csc_clus_qsum_signal_EA_lfitmean);           

  m_cscClusOviewEC.push_back(m_h2csc_clus_qsum_signal_EC);           
  m_cscClusOviewEC.push_back(m_h1csc_clus_qsum_signal_EC_count);           
  //m_cscClusOviewEC.push_back(m_h1csc_clus_qsum_signal_EC_occupancy);           
  m_cscClusOviewEC.push_back(m_h1csc_clus_qsum_signal_EC_lfitmean);           

  // qmax
  m_cscClusExpert.push_back(m_h2csc_clus_qmax);                 // expert
  m_cscClusShift.push_back(m_h2csc_clus_qmax_signal);           // shift, overview, dq-flag(#of clusters per layer)
  m_cscClusExpert.push_back(m_h2csc_clus_qmax_noise);           // expert

  m_cscClusOviewEA.push_back(m_h2csc_clus_qmax_signal_EA);
  m_cscClusOviewEA.push_back(m_h1csc_clus_qmax_signal_EA_count);           
  //m_cscClusOviewEA.push_back(m_h1csc_clus_qmax_signal_EA_occupancy);           

  m_cscClusOviewEC.push_back(m_h2csc_clus_qmax_signal_EC);        
  m_cscClusOviewEC.push_back(m_h1csc_clus_qmax_signal_EC_count);           
  //m_cscClusOviewEC.push_back(m_h1csc_clus_qmax_signal_EC_occupancy);           

  // phi time
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_time);          // expert
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_time_signal);   // expert
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_time_noise);    // expert 

  // eta time
  m_cscClusExpert.push_back(m_h1csc_clus_precision_time);           // expert
  m_cscClusShift.push_back(m_h1csc_clus_precision_time_signal);     // shift, dq-flag(timing window -50, +150 ns)
  m_cscClusExpert.push_back(m_h1csc_clus_precision_time_noise);     // expert

  m_cscClusOviewEA.push_back(m_h1csc_clus_precision_time_signal_EA);
  m_cscClusOviewEC.push_back(m_h1csc_clus_precision_time_signal_EC);

  // phi charge
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_charge);            // expert
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_charge_signal);     // expert
  m_cscClusExpert.push_back(m_h1csc_clus_transverse_charge_noise);      // expert

  // eta charge
  m_cscClusExpert.push_back(m_h1csc_clus_precision_charge);             // expert
  m_cscClusExpert.push_back(m_h1csc_clus_precision_charge_signal);      // expert
  m_cscClusExpert.push_back(m_h1csc_clus_precision_charge_noise);       // expert

  // eta width (# of strips/cluster)
  m_cscClusExpert.push_back(m_h2csc_clus_etacluswidth);             // expert
  m_cscClusExpert.push_back(m_h2csc_clus_etacluswidth_signal);      // expert
  m_cscClusExpert.push_back(m_h2csc_clus_etacluswidth_noise);       // expert

  // phi width (# of strips/cluster)
  m_cscClusExpert.push_back(m_h2csc_clus_phicluswidth);             // expert
  m_cscClusExpert.push_back(m_h2csc_clus_phicluswidth_signal);      // expert
  m_cscClusExpert.push_back(m_h2csc_clus_phicluswidth_noise);       // expert

  //total cluster width
  m_cscClusOviewEA.push_back(m_h1csc_clus_totalWidth_EA);           // overview
  m_cscClusOviewEC.push_back(m_h1csc_clus_totalWidth_EC);           // overview

  // eta count (# of clusters)
  m_cscClusExpert.push_back(m_h2csc_clus_etacluscount);             // expert
  m_cscClusExpert.push_back(m_h2csc_clus_etacluscount_signal);      // shift
  m_cscClusExpert.push_back(m_h2csc_clus_etacluscount_noise);       // expert

  // phi count (# of clusters)
  m_cscClusExpert.push_back(m_h2csc_clus_phicluscount);             // expert
  m_cscClusExpert.push_back(m_h2csc_clus_phicluscount_signal);      // expert
  m_cscClusExpert.push_back(m_h2csc_clus_phicluscount_noise);       // expert

  m_cscClusExpert.push_back(m_h1csc_clus_count);                // expert
  m_cscClusExpert.push_back(m_h1csc_clus_count_signal);         // expert
  m_cscClusExpert.push_back(m_h1csc_clus_count_noise);          // expert

  //m_cscClusShift.push_back(m_h1csc_clus_count_perlayer);

  // correlation plots
  // eta vs. phi count (# of clusters)
  m_cscClusExpert.push_back(m_h2csc_clus_eta_vs_phi_cluscount);             // expert
  m_cscClusExpert.push_back(m_h2csc_clus_eta_vs_phi_cluscount_signal);      // expert
  m_cscClusExpert.push_back(m_h2csc_clus_eta_vs_phi_cluscount_noise);       // expert


  // eta vs. phi width (# of strips/cluster)
  m_cscClusExpert.push_back(m_h2csc_clus_eta_vs_phi_cluswidth);             // expert

}


//
// bookHistograms  ----------------------------------------------------------------
//
StatusCode CscClusterValAlg::bookHistograms(){

  ATH_MSG_DEBUG (  "Booking CSC cluster histograms." );
  StatusCode sc = StatusCode::SUCCESS;

  //if(newRun) {
  bookClusterHistograms();
  // register shift histograms
  MonGroup cscclus_shift( this, m_cscClusterPath+"/Shift", run, ATTRIB_MANAGED );
  std::vector<TH1 *>::iterator m_iT = m_cscClusShift.begin();
  ATH_MSG_DEBUG (  "Found " << m_cscClusShift.size() << " shift Histograms " );
  for (; m_iT != m_cscClusShift.end(); ++m_iT) {
    sc = cscclus_shift.regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register histogram " << (*m_iT)->GetName() );
      return sc;
    }
  }

  // register expert histograms
  MonGroup cscclus_expert( this, m_cscClusterPath+"/Expert", run, ATTRIB_MANAGED );
  m_iT = m_cscClusExpert.begin();
  ATH_MSG_DEBUG (  "Found " << m_cscClusExpert.size() << " expert Histograms " );
  for (; m_iT != m_cscClusExpert.end(); ++m_iT) {
    sc = cscclus_expert.regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register histogram " << (*m_iT)->GetName() );
      return sc;
    }
  }

  // register overview histograms for EA
  //MonGroup cscclus_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
  cscclus_oviewEA = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEA/Cluster", run, ATTRIB_MANAGED );
  m_iT = m_cscClusOviewEA.begin();
  ATH_MSG_DEBUG (  "Found " << m_cscClusOviewEA.size() << " CSCEA Overview Histograms " );
  for (; m_iT != m_cscClusOviewEA.end(); ++m_iT) {
    sc = cscclus_oviewEA->regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap A: " << (*m_iT)->GetName() );
      return sc;
    }
  }

  // register overview histograms for EC
  //MonGroup cscclus_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
  cscclus_oviewEC = new MonGroup( this, m_cscGenPath+"CSC/Overview/CSCEC/Cluster", run, ATTRIB_MANAGED );
  m_iT = m_cscClusOviewEC.begin();
  ATH_MSG_DEBUG (  "Found " << m_cscClusOviewEC.size() << " CSCEC Overview Histograms " );
  for (; m_iT != m_cscClusOviewEC.end(); ++m_iT) {
    sc = cscclus_oviewEC->regHist(*m_iT);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR (  "Cannot register overview histogram for Endcap C: " << (*m_iT)->GetName() );
      return sc;
    }
  }
  //} 
  //if(newEventsBlock){}
  //if(newLumiBlock){}

  // if we are here return success
  return sc;

}

//
// fillHistograms  ----------------------------------------------------------------
//
StatusCode CscClusterValAlg::fillHistograms() {

  StatusCode sc = StatusCode::SUCCESS;

  // check if event passed sample-selection triggers
  if(m_doEvtSel) { if(!evtSelTriggersPassed()) return sc; }

  // retrieve cluster / strip collection
  const DataHandle<CscPrepDataContainer> m_cscCluster(0);
  const DataHandle<CscStripPrepDataContainer> m_cscStrip(0);

  sc = evtStore()->contains<Muon::CscPrepDataContainer>(m_cscClusterKey);
  if(sc.isFailure() || m_cscClusterKey == "") {
    ATH_MSG_WARNING (  "Cluster container of type Muon::CscPrepDataContainer and key \"" << m_cscClusterKey << "\" NOT found in StoreGate" );
    return sc;
  } else {
    sc = evtStore()->retrieve(m_cscCluster, m_cscClusterKey);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING ( "Could not retrieve cluster container of type Muon::CscPrepDataContainer and key \"" << m_cscClusterKey << "\"" );
      return sc;
    }
  }

  sc = evtStore()->contains<Muon::CscStripPrepDataContainer>(m_cscPRDKey);
  if(sc.isFailure() || m_cscPRDKey == "") {
    ATH_MSG_WARNING (  "PRD container of type Muon::CscStripPrepDataContainer and key \"" << m_cscPRDKey << "\" NOT found in StoreGate" );
    return sc;
  } else {
    sc = evtStore()->retrieve(m_cscStrip, m_cscPRDKey);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING ( "Could not retrieve strip container of type Muon::CscStripPrepDataContainer and key \"" << m_cscPRDKey << "\"" );
      return sc;
    }
  }

  // we can do (some) monitoring plots with just the cluster
  // ideally we need both the cluster and the strips that make up that cluster  
  if( sc.isSuccess()) { 
    FillCSCClusters(*m_cscCluster, *m_cscStrip);
  } else {
    ATH_MSG_WARNING ( name() << " : cannot fill monitoring histograms " );
    return StatusCode::SUCCESS;
  }

  return sc; 
}

//
// FillCSCClusters ----------------------------------------------------------------
//
void  CscClusterValAlg::FillCSCClusters( const CscPrepDataContainer& m_cols, const CscStripPrepDataContainer& m_strips ) {


  ATH_MSG_DEBUG ( " Size of Cluster Collection : " << m_cols.size() );
  ATH_MSG_DEBUG ( " Size of Strip Collection : " << m_strips.size() );

  for ( CscPrepDataContainer::const_iterator m_Icol = m_cols.begin();
      m_Icol != m_cols.end(); ++m_Icol ) {
    const CscPrepDataCollection& m_clus = **m_Icol;
    //Identifier coll_id = m_clus.identify();


    // arrays to hold cluster-count  
    // 32 chambers and 8 layers (each has one extra - index '0' is not counted)
    int m_clusCount[33][9], m_sigclusCount[33][9];
    unsigned int m_nEtaClusWidthCnt = 0, m_nPhiClusWidthCnt = 0;
    for(unsigned int kl = 0; kl < 33; kl++ ) {
      for(unsigned int km = 0; km < 9; km++ ) {
        m_clusCount[kl][km] = 0;
        m_sigclusCount[kl][km] = 0;
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


    stripsSum_EA = 0.;
    stripsSum_EAtest = -50.;
    stripsSum_EC = 0.;
    stripsSum_ECtest = -50.;


    ATH_MSG_DEBUG ( " Begin loop over clusters ============================");
    for ( CscPrepDataCollection::const_iterator m_Itclu = m_clus.begin();
        m_Itclu != m_clus.end(); ++m_Itclu ) {
      CscPrepData& m_iClus = **m_Itclu;
      const std::vector<Identifier>& m_stripIds = m_iClus.rdoList();    
      float m_clu_charge = m_iClus.charge();
      float m_clu_time = m_iClus.time();

      ATH_MSG_DEBUG(" cluster charge = " << m_clu_charge << "\t cluster time = " << m_clu_time );

      unsigned int m_noStrips = m_stripIds.size();  // no. of strips in this cluster = m_stripIds.size()
      Identifier m_clusId = m_iClus.identify();

      // get the cluster coordinates
      int m_stationName = m_cscIdHelper->stationName(m_clusId);
      std::string m_stationString = m_cscIdHelper->stationNameString(m_stationName);
      int m_chamberType = m_stationString == "CSS" ? 0 : 1;
      int m_stationEta  = m_cscIdHelper->stationEta(m_clusId);
      int m_stationPhi  = m_cscIdHelper->stationPhi(m_clusId);
      int m_wireLayer = m_cscIdHelper->wireLayer(m_clusId);
      int m_measuresPhi = m_cscIdHelper->measuresPhi(m_clusId);


      float m_x = m_iClus.globalPosition().x();
      float m_y = m_iClus.globalPosition().y();
      float m_z = m_iClus.globalPosition().z();
      float m_r = sqrt(m_x*m_x + m_y*m_y);
      //TVector3 m_clu(m_x,m_y,m_z);
      //ATH_MSG_DEBUG(" cluster eta = " << m_clu.Eta() << "\t pseudorapidity = " << m_clu.PseudoRapidity() << "\t phi = " << m_clu.Phi());
      //ATH_MSG_DEBUG(" cluster m_x = " << m_x << "\t m_y = " << m_y << "\t m_z = " << m_z );
      //m_h2csc_clus_eta_vs_phi_hitmap->Fill(m_clu.Phi(),m_clu.Eta()); 
      m_h2csc_clus_r_vs_z_hitmap->Fill(m_z,m_r);
      m_h2csc_clus_y_vs_x_hitmap->Fill(m_y,m_x);

      // convert to my coordinates
      int m_sectorNo  = m_stationEta * (2 * m_stationPhi - m_chamberType);   // [-16 -> -1] and [+1 -> +16]
      float m_secLayer = m_sectorNo + 0.2 * (m_wireLayer - 1) + 0.1;
      int xfac = m_measuresPhi ? -1 : 1;        // [-1 -> -48] / [+1 -> +192]


      //total cluster width (EA and EC) calculation
       if(m_secLayer > 0.) { 
          stripsSum_EA = stripsSum_EA + m_noStrips;
     }
       if(stripsSum_EA > stripsSum_EAtest) {
          stripsSum_EAtest = stripsSum_EA;
     }

       if(m_secLayer < 0. || m_secLayer == 0.) { 
          stripsSum_EC = stripsSum_EC + m_noStrips;
     }
       if(stripsSum_EC > stripsSum_ECtest) {
          stripsSum_ECtest = stripsSum_EC;
     }  


      // check boundaries of sector/layer - redundancy
      //if(!(m_sectorNo+16) < 33) m_sectorNo = 0;
      //if(!(m_wireLayer < 5)) m_wireLayer = 0;

      // compute the indices to store cluster count
      int m_ns = m_sectorNo < 0 ? m_sectorNo*(-1) : m_sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
      int m_nl = (m_measuresPhi ? m_wireLayer : m_wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)

      // increment the cluster-count for this layer
      m_clusCount[m_ns][m_nl]++;

      // indices for m_ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
      // indices for m_nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
      ATH_MSG_DEBUG(" m_ns = " << m_ns << "\tm_nl = " << m_nl << "\tm_sec = " << m_sectorNo << "\t m_lay= " << m_wireLayer << "\tmPhi = " << m_measuresPhi);


      // check the cluster status; probably need to read status info from jobOptions - not done for the moment
      // status = Muon::CscStatusUnspoiled (i.e 0) or Muon::CscStatusSplitUnspoiled (i.e 10) are considered good for precision clusters
      // status = Muon::CscStatusSimple (i.e 1) could be good for non-precision clusters (i.e for phi-layers)
      std::string m_stat = Muon::toString(m_iClus.status());
      bool m_cluster_status = ( (m_stat == "unspoiled")                 || 
          (m_stat == "unspoiled with split")                            ||
          (m_stat == "simple")
          ) ? true : false;

      // Also need at least three strips in an eta-cluster to compute Q_max, Q_left and Q_right
      bool m_eta_cluster_status = m_cluster_status && ( m_noStrips > 2 ) && (m_measuresPhi == 0); 

      // Also need at least one strip in a phi-cluster to compute Q_max = Q_sum
      bool m_phi_cluster_status = m_cluster_status && ( m_noStrips > 0 ) && (m_measuresPhi == 1);

      ATH_MSG_DEBUG ( " ClusterStatus eta = " << m_eta_cluster_status << " ,phi = " << m_phi_cluster_status);
      ATH_MSG_DEBUG ( " ClusterID (eta:" << m_stationEta << ",phi:" << m_stationPhi << ",type:" << m_chamberType << ", measPhi: " 
          << m_measuresPhi << ",wire:" << m_wireLayer << ") = " << m_secLayer << " status = " 
          << m_stat << " #of strips = " << m_noStrips );

      // if cluster is okay get Qmax, Qleft, Qright and Qsum = (Qmax + Qleft + Qright)
      if(m_eta_cluster_status || m_phi_cluster_status ) {
        const CscStripPrepDataCollection* m_pcol(0);
        bool m_found_id = true;
        std::vector <const CscStripPrepData*> m_stripVec;
        std::vector <float> m_fStripIDs;
        float m_maxStripCharge = 0., m_maxStipId = 0.;
        int m_sIdx = 0, m_mxIdx = 0; // index-counter and index of max strip in the vector of Id's

        // fill cluster width (no. of strips per cluster) 
        if(m_measuresPhi) {
          m_h2csc_clus_phicluswidth->Fill(m_noStrips,m_secLayer);  // fill phi-cluster width
          m_nPhiClusWidthCnt++;
        } else {
          m_h2csc_clus_etacluswidth->Fill(m_noStrips,m_secLayer);  // fill eta-cluster width
          m_nEtaClusWidthCnt++;
        }

        // Loop over strip id's vector / strip collection and match the id's from vector with strips in collection
        for ( std::vector<Identifier>::const_iterator sId = m_stripIds.begin(); sId != m_stripIds.end(); ++sId, m_sIdx++ ) {
          Identifier m_id = *sId; // for strip Id's
          int m_thisStrip = m_cscIdHelper->strip(m_id);
          float m_stripid = m_thisStrip * xfac;         // x-axis fill value
          m_fStripIDs.push_back(m_stripid);
          m_h2csc_clus_hitmap->Fill(m_stripid, m_secLayer);

          if(!m_pcol) {
            CscStripPrepDataContainer::const_iterator m_icol = m_strips.indexFind(m_clus.identifyHash());
            if ( m_icol == m_strips.end() ) {
              m_found_id = false;
              break;  // could not identify the strips
            } else {
              m_pcol = *m_icol;
              if(!m_pcol) m_found_id = false;
            }
          } // end if !m_pcol  

          bool m_found_strip = false;
          float m_maxsampChVal = 0.;
          if(m_found_id) {
            for ( CscStripPrepDataCollection::const_iterator m_istrip= m_pcol->begin(); m_istrip != m_pcol->end(); ++ m_istrip ) {
              m_found_strip = ( *m_istrip )->identify() == m_id ; 
              if(m_found_strip) {
                m_stripVec.push_back(*m_istrip);
                //std::ostringstream m_charges;
                std::vector<float> m_samp_charges = ( *m_istrip )->sampleCharges();
                for(unsigned int i = 0; i < m_samp_charges.size(); i++ ) {
                  //std::string m_ch; sprintf((char *)m_ch.c_str(),"%6.2f , ",m_samp_charges[i]);
                  //m_charges << m_ch.c_str();
                  if(m_samp_charges[i] > m_maxsampChVal) m_maxsampChVal = m_samp_charges[i];
                }
                if(m_maxsampChVal > m_maxStripCharge ) {
                  m_maxStripCharge = m_maxsampChVal; 
                  m_maxStipId = m_stripid;
                  m_mxIdx = m_sIdx;
                }
                //ATH_MSG_DEBUG ( " " << m_charges.str() << "\t time= " << ( *m_istrip )->timeOfFirstSample());
                break; // break from inner loop
              }
            } // end for loop on strip collection
            ATH_MSG_DEBUG ( " " << (m_found_strip? "FoundStrip " : "NoStripFound ") << " with max sampling = " << m_maxsampChVal);
          } // end if found_id
        } // end for loop over strips
        ATH_MSG_DEBUG ( " Max Strip charge = " << m_maxStripCharge  << " and strip Id = " << m_maxStipId << " and index = " << m_mxIdx);
        float m_qmax = 0., m_qleft = 0., m_qright = 0., m_qsum = 0.;

        // if we are here and loop over strips is successful we should have m_found_id = true
        // and the size of strip-ID-vector == size of strips-vector
        bool m_size_ids_coll = (m_noStrips == m_stripVec.size() ? true : false) ;

        if(m_found_id && m_size_ids_coll ) {
          // store results of three strips (Qmax, Qleft, Qright)
          std::vector<ICscStripFitter::Result> m_res;
          m_res.reserve(3);
          bool m_range_check = (m_mxIdx > -1) && (m_mxIdx < int(m_noStrips));

          ATH_MSG_DEBUG ( " Range check = (" << m_mxIdx  << " > -1 ) && (" << m_mxIdx << " < " << m_noStrips << " ) = " << m_range_check
              << "\t size of vec check " << m_noStrips << " == " << m_stripVec.size());

          if( m_range_check ) {
            // fit Q_left fit
            if(m_mxIdx-1 >= 0 ) {
              m_res[0] = m_stripFitter->fit(*m_stripVec[m_mxIdx-1]);
              m_qleft = m_res[0].charge;
              m_qsum += m_qleft;
              ATH_MSG_DEBUG ( " Left Strip q +- dq = " << m_res[0].charge  << " +- " << m_res[0].dcharge << "\t t +- dt = " 
                  << m_res[0].time << " +- " <<  m_res[0].dtime << "\t w +- dw = " << m_res[0].width << " +- " 
                  << m_res[0].dwidth << "\t status= " << m_res[0].status << "\t chisq= " << m_res[0].chsq);
            } // end if q_left
            // fit Q_max strip
            m_res[1] = m_stripFitter->fit(*m_stripVec[m_mxIdx]);
            m_qmax = m_res[1].charge;
            m_qsum += m_qmax; 
            ATH_MSG_DEBUG ( " Peak Strip q +- dq = " << m_res[1].charge  << " +- " << m_res[1].dcharge << "\t t +- dt = " 
                << m_res[1].time << " +- " <<  m_res[1].dtime << "\t w +- dw = " << m_res[1].width << " +- " 
                << m_res[1].dwidth << "\t status= " << m_res[1].status << "\t chisq= " << m_res[1].chsq);
            // fit Q_right strip
            if(m_mxIdx+1 < int(m_noStrips)) {
              m_res[2] = m_stripFitter->fit(*m_stripVec[m_mxIdx+1]);
              m_qright = m_res[2].charge;
              m_qsum += m_qright;
              ATH_MSG_DEBUG ( " Right Strip q +- dq = " << m_res[2].charge  << " +- " << m_res[2].dcharge << "\t t +- dt = " 
                  << m_res[2].time << " +- " <<  m_res[2].dtime << "\t w +- dw = " << m_res[2].width << " +- " 
                  << m_res[2].dwidth << "\t status= " << m_res[2].status << "\t chisq= " << m_res[2].chsq);
            } // end if q_right
          } // end if range_check

          // not used at the moment
          // 1 e = 1.602176487 10^{-19} C = 1.6022 x 10^{-4} fC
          // float m_fCperElectron = 1.6022e-4; // multiply # of electrons by this number to get fC


          float m_kiloele = 1.0e-3; // multiply # of electrons by this number to get kiloElectrons (1 ke = 1 ADC)

          // Assume 1000 e = 1 ADC for now = 1000 x 1.6022 x 10^{-4} fC = 0.16022 fC
          //float m_fCperADC = 0.16022; // multiply ADC counts with with this to get fC

          //float m_kEperADC = 1000.; // multiply ADC counts with with this to get #of electrons

          // convert qmax, qleft, qright into ADC 
          float m_QmaxADC = m_qmax * m_kiloele;
          float m_QsumADC = m_qsum * m_kiloele;

          // check if signal or noise
          // m_QmaxADC > m_qmaxADCCut is signal
          bool m_signal = m_QmaxADC > m_qmaxADCCut;

          // fill signal/noise histograms
          if(m_signal) {

            // increment signal-cluster counter
            m_sigclusCount[m_ns][m_nl]++;

            for(unsigned int j =0; j < m_fStripIDs.size(); j++)
              m_h2csc_clus_hitmap_signal->Fill(m_fStripIDs[j], m_secLayer);
            if(m_measuresPhi) {
              m_h2csc_clus_phicluswidth_signal->Fill(m_noStrips,m_secLayer);  
            } else {
              m_h2csc_clus_etacluswidth_signal->Fill(m_noStrips,m_secLayer);  
            }

            // Fill layer occupancy
            if(m_stationEta == 1) {
              m_h1csc_clus_occupancy_signal_EA->Fill(m_secLayer);
            } else {
              m_h1csc_clus_occupancy_signal_EC->Fill(m_secLayer);
            }
          } else {
            for(unsigned int j =0; j < m_fStripIDs.size(); j++)
              m_h2csc_clus_hitmap_noise->Fill(m_fStripIDs[j], m_secLayer);
            if(m_measuresPhi) {
              m_h2csc_clus_phicluswidth_noise->Fill(m_noStrips,m_secLayer);
            } else {
              m_h2csc_clus_etacluswidth_noise->Fill(m_noStrips,m_secLayer);
            }
          }

          m_h2csc_clus_qmax->Fill(m_QmaxADC, m_secLayer);
          if(m_signal) {
            m_h2csc_clus_qmax_signal->Fill(m_QmaxADC, m_secLayer);
            if(m_stationEta == 1) {
              m_h2csc_clus_qmax_signal_EA->Fill(m_QmaxADC, m_secLayer);
              m_h1csc_clus_qmax_signal_EA_count->Fill(m_QmaxADC);
              //m_h1csc_clus_qmax_signal_EA_occupancy->Fill(m_secLayer);
            } else {
              m_h2csc_clus_qmax_signal_EC->Fill(m_QmaxADC, m_secLayer);
              m_h1csc_clus_qmax_signal_EC_count->Fill(m_QmaxADC);
              //m_h1csc_clus_qmax_signal_EC_occupancy->Fill(m_secLayer);
            }
          } else {
            m_h2csc_clus_qmax_noise->Fill(m_QmaxADC, m_secLayer);
          }

          m_h2csc_clus_qsum->Fill(m_QsumADC, m_secLayer);
          if(m_signal) {
            m_h2csc_clus_qsum_signal->Fill(m_QsumADC, m_secLayer);
            if(m_stationEta == 1) {
              m_h2csc_clus_qsum_signal_EA->Fill(m_QsumADC, m_secLayer);
              m_h1csc_clus_qsum_signal_EA_count->Fill(m_QsumADC);
              //m_h1csc_clus_qsum_signal_EA_occupancy->Fill(m_secLayer);
            } else {
              m_h2csc_clus_qsum_signal_EC->Fill(m_QsumADC, m_secLayer);
              m_h1csc_clus_qsum_signal_EC_count->Fill(m_QsumADC);
              //m_h1csc_clus_qsum_signal_EC_occupancy->Fill(m_secLayer);
            }
          } else {
            m_h2csc_clus_qsum_noise->Fill(m_QsumADC, m_secLayer);
          }

          if(m_measuresPhi) {
            m_h1csc_clus_transverse_time->Fill(m_clu_time);
            m_h1csc_clus_transverse_charge->Fill(m_clu_charge*m_kiloele);
            if(m_signal) {
              m_h1csc_clus_transverse_time_signal->Fill(m_clu_time);
              m_h1csc_clus_transverse_charge_signal->Fill(m_clu_charge*m_kiloele);
            } else {
              m_h1csc_clus_transverse_time_noise->Fill(m_clu_time);
              m_h1csc_clus_transverse_charge_noise->Fill(m_clu_charge*m_kiloele);
            }
          } else {
            m_h1csc_clus_precision_time->Fill(m_clu_time); 
            m_h1csc_clus_precision_charge->Fill(m_clu_charge*m_kiloele);
            if(m_signal) {
              m_h1csc_clus_precision_time_signal->Fill(m_clu_time); 
              if(m_stationEta == 1) m_h1csc_clus_precision_time_signal_EA->Fill(m_clu_time);
              else m_h1csc_clus_precision_time_signal_EC->Fill(m_clu_time);
              m_h1csc_clus_precision_charge_signal->Fill(m_clu_charge*m_kiloele);
            } else {
              m_h1csc_clus_precision_time_noise->Fill(m_clu_time); 
              m_h1csc_clus_precision_charge_noise->Fill(m_clu_charge*m_kiloele);
            }
          }

          ATH_MSG_DEBUG ( " End of strip fits " ); 
        } // if found_id


      } // end if cluster_status

        //filling total cluster width histograms
        m_h1csc_clus_totalWidth_EA->Fill( stripsSum_EA );
        m_h1csc_clus_totalWidth_EC->Fill( stripsSum_EC );

    } // end for loop over prep-data collection
    ATH_MSG_DEBUG ( " End loop over clusters ============================");

    m_h2csc_clus_eta_vs_phi_cluswidth->Fill(m_nPhiClusWidthCnt,m_nEtaClusWidthCnt);

    // Fill cluster counts
    int m_numeta = 0, m_numphi = 0;
    int m_numetasignal = 0, m_numphisignal = 0;
    //loop over chambers
    for(int kl = 1; kl < 33; kl++ ) {

      // loop over layers
      int m_eta_hits[4] = {0,0,0,0};
      bool m_chamber_empty = true;  
      int m_sec = kl < 17 ? kl*(-1) : kl; // [1->16](-side)  [17-32] (+side)
      for(int km = 1; km < 9; km++ ) {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string m_wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";
        int m_count = m_clusCount[kl][km];
        int m_scount = m_sigclusCount[kl][km];

        if(m_count) {
          float m_secLayer = m_sec + 0.2 * (lay - 1) + 0.1;

          //for(int m_cnt = 0; m_cnt < m_scount; m_cnt++)  {
          //  m_h1csc_clus_count_perlayer->Fill(m_secLayer);
          //}

          ATH_MSG_DEBUG ("sec[" << m_sec << "]\t" << m_wlay << "[" << lay << "] = " <<
              m_secLayer << "= " << "\tNsig = " << m_scount << ", Ntot = " << m_count);

          if(mphi) {
            m_h2csc_clus_phicluscount->Fill(m_count,m_secLayer); // all phi-cluster counts
            m_numphi += m_count;
            if(m_scount) {
              m_chamber_empty = false;
              m_numphisignal += m_scount;
              m_h2csc_clus_phicluscount_signal->Fill(m_scount,m_secLayer); // signal phi-cluster count
            } else {
              m_h2csc_clus_phicluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise phi-cluster count
            }
          } else {
            m_h2csc_clus_etacluscount->Fill(m_count,m_secLayer);
            m_numeta += m_count;
            if(m_scount) {
              m_eta_hits[lay-1]++;
              m_chamber_empty = false;
              m_numetasignal += m_scount;
              m_h2csc_clus_etacluscount_signal->Fill(m_scount,m_secLayer); // signal eta-cluster count
            } else {
              m_h2csc_clus_etacluscount_noise->Fill((m_count-m_scount),m_secLayer); // noise eta-cluster count
            }
          }
          ATH_MSG_DEBUG ( m_wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " <<
              m_secLayer << "\t = " << m_count << "\t" << m_scount);
        }
      } // end loop over layers

      if(!m_chamber_empty) {
        std::ostringstream m_nseglist;
        std::bitset<4> m_segNum;
        for(unsigned int mm = 0; mm < 4; mm++) {
          bool m_set = (m_eta_hits[mm] > 0 ? true : false);
          if(m_set) m_segNum.set(mm);
          m_nseglist << (m_set ? "1" : "0");
        }
        m_h2csc_clus_segmap_signal->Fill(m_segNum.to_ulong(), float(m_sec+0.3));
        ATH_MSG_DEBUG("segments= " << m_nseglist.str() << "\t = " << m_segNum.to_ulong());
      } else {
        m_h2csc_clus_segmap_signal->Fill(-999., float(m_sec+0.3));
      }

    } // end loop over chambers

    ATH_MSG_DEBUG(" m_numphi = " << m_numphi << "\t m_numeta = " << m_numeta << "\tm_sphi = " 
        << m_numphisignal << "\t m_seta = " << m_numetasignal);

    m_h1csc_clus_count->Fill(m_numphi+m_numeta);
    m_h1csc_clus_count_signal->Fill(m_numphisignal+m_numetasignal);
    m_h1csc_clus_count_noise->Fill((m_numphi-m_numphisignal)+(m_numeta-m_numetasignal));

    m_h2csc_clus_eta_vs_phi_cluscount->Fill(m_numphi,m_numeta);
    m_h2csc_clus_eta_vs_phi_cluscount_signal->Fill(m_numphisignal,m_numetasignal);
    m_h2csc_clus_eta_vs_phi_cluscount_noise->Fill(m_numphi-m_numphisignal, m_numeta-m_numetasignal);

  } // end for loop over prep-data container

  ATH_MSG_DEBUG ( " END EVENT ============================");
}

//
//  evtSelTriggersPassed ----------------------------------------------------------------
//
bool CscClusterValAlg::evtSelTriggersPassed() {

  if(!m_doEvtSel) return true;
  std::vector<std::string>::const_iterator 
    it = m_sampSelTriggers.begin(), itE = m_sampSelTriggers.end();
  for ( ; it != itE; it++ ) {
    if (m_trigDec->isPassed(*it, TrigDefs::eventAccepted)) {
      return true;
    }
  }
  return false;

} // end evtSelTriggersPassed 

//
// procHistograms ----------------------------------------------------------------
//
StatusCode CscClusterValAlg::procHistograms() {

  StatusCode sc = StatusCode::SUCCESS;
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){}

  /* book these only at the end of run
  if(isEndOfRun){

    //MonGroup cscclus_oviewEC( this, m_cscGenPath+"CSC/Overview/CSCEC", shift, run );
    for(unsigned int j = 0; j < m_cscClusOviewEC.size(); j++ ) {
      TH1 *m_h(0); 
      m_h = m_cscClusOviewEC[j];
      if(m_h != NULL) {
        bool m_hist2d = m_h->IsA()->InheritsFrom("TH2");
        if(m_hist2d) {
          std::string m_hname = m_h->GetName();
          // Get Y-projection (sec+0.2*lay)
          TH1D *m_hY = dynamic_cast<TH2F* >(m_h)->ProjectionY(Form("%s_hY",m_hname.c_str()),0,-1,"");
          // set bin labels
          CscBins::BinLabels(m_hY,-1);
          // register histogram with Overview/CSCEC
          sc = cscclus_oviewEC->regHist(m_hY);
          if ( sc.isFailure() ) {
            ATH_MSG_ERROR (  "Cannot register histogram " << m_hY->GetName() );
            return sc;
          }
          // Get X-projection (counts)
          TH1D *m_hX = dynamic_cast<TH2F* >(m_h)->ProjectionX(Form("%s_hX",m_hname.c_str()),0,-1,"e");
          sc = cscclus_oviewEC->regHist(m_hX);
          if ( sc.isFailure() ) {
            ATH_MSG_ERROR (  "Cannot register histogram " << m_hX->GetName() );
            return sc;
          }
        } // end if hist2d
      } // end if m_h
    } // end for


    //MonGroup cscclus_oviewEA( this, m_cscGenPath+"CSC/Overview/CSCEA", shift, run );
    for(unsigned int j = 0; j < m_cscClusOviewEA.size(); j++ ) {
      TH1 *m_h(0); 
      m_h = m_cscClusOviewEA[j];
      if(m_h != NULL) {
        bool m_hist2d = m_h->IsA()->InheritsFrom("TH2");
        if(m_hist2d) {
          std::string m_hname = m_h->GetName();
          // Get Y-projection (sec+0.2*lay)
          TH1D *m_hY = dynamic_cast<TH2F* >(m_h)->ProjectionY(Form("%s_hY",m_hname.c_str()),0,-1,"");
          // set bin labels
          CscBins::BinLabels(m_hY,1);
          // register histogram with Overview/CSCEA
          sc = cscclus_oviewEA->regHist(m_hY);
          if ( sc.isFailure() ) {
            ATH_MSG_ERROR (  "Cannot register histogram " << m_hY->GetName() );
            return sc;
          }
          // Get X-projection (counts)
          TH1D *m_hX = dynamic_cast<TH2F* >(m_h)->ProjectionX(Form("%s_hX",m_hname.c_str()),0,-1,"e");
          sc = cscclus_oviewEA->regHist(m_hX);
          if ( sc.isFailure() ) {
            ATH_MSG_ERROR (  "Cannot register histogram " << m_hX->GetName() );
            return sc;
          }
        } // end if hist2d
      } // end if m_h
    } // end for


  } // end isEndofRun */
  return sc;
}

