/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****** **************************************************************
//
// NAME:     TileRawChNoiseCalib.cxx
// PACKAGE:  TileCalib
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// April 2008        
// ********************************************************************



#include "GaudiKernel/ISvcLocator.h"

//Event info
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Tile includes
#include "TileCalibAlgs/TileRawChNoiseCalibAlg.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"


#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"
#include "TGraphErrors.h"
#include "TClonesArray.h"

#include <cmath>
#include <ctime>
#include <iostream>


#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif

#define MAX_DRAWERS 256
#define N_CHANS 48
#define N_DMUS 16


TileRawChNoiseCalibAlg::TileRawChNoiseCalibAlg(const std::string& name, ISvcLocator* pSvcLocator)
 : AthAlgorithm(name,pSvcLocator)
  , m_beamCnv(0)
  , m_cabling(0)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileIdTrans("TileCondIdTransforms")
  , m_cispar(0)
  //, m_nDrawers(0)
  , m_time(0)
  , m_year(0)
  , m_month(0)
  , m_day(0)
  , m_yday(0)
  , m_hour(0)
  , m_min(0)
  , m_run(0)
  , m_trigType(0)
{

  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");    
  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");
  declareProperty("doFit" , m_doFit  = true);
  declareProperty("doFixed", m_doFixed = true); 
  declareProperty("doOpt" , m_doOpt  = true); 
  declareProperty("doDsp" , m_doDsp  = true); 
  declareProperty("doOF1" , m_doOF1  = true); 
  declareProperty("doMF" , m_doMF  = true); 
  declareProperty("SaveHist", m_saveHist = false); // write all histograms to output file
  declareProperty("InvertChanRatio", m_invertChanRatio = true); // swap two sigmas and invert ratio if it is above 1.0 (channel fit only)
  declareProperty("MaskBadChannels",m_maskBadChannels = true);
  declareProperty("UseforCells" , m_UseforCells  = 2); // Fit=0, Fixed=1,  Opt=2, Dsp=3, OF1=4, MF=5
  declareProperty("CalibMode", m_calibMode = true);  
  declareProperty("UsePMT", m_usePMT = false);  
  declareProperty("RunNumber", m_run=0);
  declareProperty("FileNamePrefix", m_file="RawCh_NoiseCalib");
  declareProperty("NtupleID", m_ntupleID="RawCh_NoiseCalib");
  declareProperty("TreeSize", m_treeSize = 16000000000LL);
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");

  m_evtNr=-1;

  m_fillidx=false;

  m_histAmp = new TH1F*[RCnum][5][64][48][2]();
  m_histCellAmp = new TH1F*[2][64][4][17][6]();
  m_evt = new int[5][64][48][2]();
  m_ros = new uint8_t[5][64][48][2]();
  m_drawer = new uint8_t[5][64][48][2]();
  m_channel = new uint8_t[5][64][48][2]();
  m_gain = new bool[5][64][48][2]();
  m_rc_mean = new float[RCnum][5][64][48][2]();
  m_rc_sigma = new float[RCnum][5][64][48][2]();
  m_rc_av = new float[RCnum][5][64][48][2]();
  m_rc_rms = new float[RCnum][5][64][48][2]();
  m_rc_skewness = new float[RCnum][5][64][48][2]();
  m_rc_kurtosis = new float[RCnum][5][64][48][2]();
  m_rc_mean_err = new float[RCnum][5][64][48][2]();
  m_rc_sigma_err = new float[RCnum][5][64][48][2]();
  m_rc_chi2 = new float[RCnum][5][64][48][2]();
  m_rc_ndf = new float[RCnum][5][64][48][2]();
  m_rc_probC2 = new float[RCnum][5][64][48][2]();
  m_rc_ggpar = new float[RCnum][5][64][48][2][8]();
  m_rc_gsigma1 = new float[RCnum][5][64][48][2]();
  m_rc_gsigma2 = new float[RCnum][5][64][48][2]();
  m_rc_gnorm = new float[RCnum][5][64][48][2]();
  m_rc_gchi2 = new float[RCnum][5][64][48][2]();
  m_rc_gerrsigma1= new float[RCnum][5][64][48][2]();
  m_rc_gerrsigma2 = new float[RCnum][5][64][48][2]();
  m_rc_gerrnorm = new float[RCnum][5][64][48][2]();
  m_rc_gcorrsigma1sigma2 = new float[RCnum][5][64][48][2]();

  m_side = new bool[2][64][4][17][6]();
  m_phi = new uint8_t[2][64][4][17][6]();
  m_sample = new uint8_t[2][64][4][17][6]();
  m_tower = new uint8_t[2][64][4][17][6]();
  m_gg = new uint8_t[2][64][4][17][6]();
  m_ecell_av = new float[2][64][4][17][6]();
  m_ecell_rms= new float[2][64][4][17][6]();
  m_ecell_hash= new uint32_t[2][64][4][17]();
  
  m_cell_nch = new int[2][64][4][17][2]();
  m_ecell_ene = new float[2][64][4][17][2]();
  m_ggpar = new float[2][64][4][17][6][8]();
  m_gsigma1 = new float[2][64][4][17][6]();
  m_gsigma2 = new float[2][64][4][17][6]();
  m_gnorm = new float[2][64][4][17][6]();
  m_gchi2 = new float[2][64][4][17][6]();
  m_gerrsigma1 = new float[2][64][4][17][6]();
  m_gerrsigma2 = new float[2][64][4][17][6]();
  m_gerrnorm = new float[2][64][4][17][6]();
  m_gcorrsigma1sigma2 = new float[2][64][4][17][6]();
}

TileRawChNoiseCalibAlg::~TileRawChNoiseCalibAlg()
{ 
  delete[] m_histAmp;
  delete[] m_histCellAmp;
  delete[] m_evt;
  delete[] m_ros;
  delete[] m_drawer;
  delete[] m_channel;
  delete[] m_gain;
  delete[] m_rc_mean;
  delete[] m_rc_sigma;
  delete[] m_rc_av;
  delete[] m_rc_rms;
  delete[] m_rc_skewness;
  delete[] m_rc_kurtosis;
  delete[] m_rc_mean_err;
  delete[] m_rc_sigma_err;
  delete[] m_rc_chi2;
  delete[] m_rc_ndf;
  delete[] m_rc_probC2;
  delete[] m_rc_ggpar;
  delete[] m_rc_gsigma1;
  delete[] m_rc_gsigma2;
  delete[] m_rc_gnorm;
  delete[] m_rc_gchi2;
  delete[] m_rc_gerrsigma1;
  delete[] m_rc_gerrsigma2;
  delete[] m_rc_gerrnorm;
  delete[] m_rc_gcorrsigma1sigma2;

  delete[] m_side;
  delete[] m_phi;
  delete[] m_sample;
  delete[] m_tower;
  delete[] m_gg;
  delete[] m_ecell_av;
  delete[] m_ecell_rms;
  delete[] m_ecell_hash;
   
  delete[] m_cell_nch;
  delete[] m_ecell_ene;
  delete[] m_ggpar;
  delete[] m_gsigma1;
  delete[] m_gsigma2;
  delete[] m_gnorm;
  delete[] m_gchi2;
  delete[] m_gerrsigma1;
  delete[] m_gerrsigma2;
  delete[] m_gerrnorm;
  delete[] m_gcorrsigma1sigma2;
}

/// Only array initialization is done here
/// All the helpers initialization is done at the first event
StatusCode TileRawChNoiseCalibAlg::initialize() {

  int nbin = 151;
  float binwidth[2] = { 0.125, 0.25 }; // in ADC
  float xmax[2] = { (float) nbin * binwidth[0] / 2.F, (float) nbin * binwidth[1] / 2.F }; // in ADC

  std::ostringstream sStr;
  std::string nam;

  for (int rc = 0; rc < RCnum; ++rc) {
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
        for (unsigned int ch = 0; ch < TileCalibUtils::MAX_CHAN; ++ch) {
          for (unsigned int g = 0; g < TileCalibUtils::MAX_GAIN; ++g) {
            sStr.str("");
            sStr << "Amplitudes_RC_" << rc << "_Part_" << ros << "_Drawer_" << drawer << "_Ch_" << ch << "_Gain_" << g;
            nam = sStr.str();
            m_histAmp[rc][ros][drawer][ch][g] = new TH1F(nam.c_str(), nam.c_str(), nbin, -xmax[g], xmax[g]);
            CAN_REBIN(m_histAmp[rc][ros][drawer][ch][g]); //in case some entries are outside the initial limits
            m_histAmp[rc][ros][drawer][ch][g]->SetDirectory(0);
          }
        }
      }
    }
  }

  nbin = 301;
  float cellbin[2] = { 80., 2.5 }; //in MeV
  float xcellmax[2] = { (float) nbin * cellbin[0] / 2.F, (float) nbin * cellbin[1] / 2.F }; //in MeV

  for (int side = 0; side < 2; side++) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int sample = 0; sample < 4; sample++) {
        for (int tower = 0; tower < 17; tower++) {
          for (int gg = 0; gg < 6; gg++) {
            sStr.str("");
            sStr << "CellAmplitude_Side_" << side << "_Drawer_" << drawer << "_Sample_" << sample << "_Tower_" << tower << "_Gains_" << gg;
            nam = sStr.str();
            m_histCellAmp[side][drawer][sample][tower][gg] = new TH1F(nam.c_str(), nam.c_str(), nbin, -xcellmax[gg / 3], xcellmax[gg / 3]); // cell limits should be at least sqrt(2)*channel limits
            CAN_REBIN(m_histCellAmp[side][drawer][sample][tower][gg]); //in case some entries are outside the initial limits
            m_histCellAmp[side][drawer][sample][tower][gg]->SetDirectory(0);
          }
        }
      }
    }
  }


  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  //=== get TileCondIdTransforms
  CHECK( m_tileIdTrans.retrieve() );

  CHECK( m_dqStatusKey.initialize() );

  ATH_CHECK( m_rawChannelContainerFitKey.initialize(m_doFit) );
  ATH_CHECK( m_rawChannelContainerFixedKey.initialize(m_doFixed) );
  ATH_CHECK( m_rawChannelContainerOptKey.initialize(m_doOpt) );
  ATH_CHECK( m_rawChannelContainerDspKey.initialize(m_doDsp) );
  ATH_CHECK( m_rawChannelContainerOF1Key.initialize(m_doOF1) );
  ATH_CHECK( m_rawChannelContainerMFKey.initialize(m_doMF) );

  if (!m_eventInfoKey.key().empty()) {
    ATH_CHECK( m_eventInfoKey.initialize() );
  }

  return StatusCode::SUCCESS;
}

/// Initialization done at the first event.
/// All the helpers and data dependent variables are initialized here
StatusCode TileRawChNoiseCalibAlg::FirstEvt_initialize() {

  
  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );

  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_MSG_INFO( "calibMode " << m_calibMode  );


  // set event number to 0 before first event

  if ( m_evtNr < 0 ) {
    m_evtNr = 0;
    if (m_beamElemContainer.length() > 0) {
      ServiceHandle<IConversionSvc> cnvSvc("ByteStreamCnvSvc","");
      if (cnvSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR( " Can't get ByteStreamCnvSvc " );
        m_beamCnv = NULL;
      
      } else {
      
        m_beamCnv = dynamic_cast<TileBeamElemContByteStreamCnv *> ( cnvSvc->converter( ClassID_traits<TileBeamElemContainer>::ID() ) );
  
        if ( m_beamCnv == NULL ) {
          ATH_MSG_ERROR( " Can't get TileBeamElemContByteStreamCnv " );
        }
      }
    } else {
      m_beamCnv = NULL;
    }
  }        

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;

}

/// Main method
StatusCode TileRawChNoiseCalibAlg::execute() {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus* dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  bool empty(false); // to add all StatusCodes

  if (m_evtNr < 0) {

    if (FirstEvt_initialize().isFailure()) { ATH_MSG_ERROR( "FirstEvt_initialize failed" ); }

    bool calibMode  = (dqStatus->calibMode() == 1);
    if ( calibMode != m_calibMode ) {
      ATH_MSG_INFO( "Overwriting calib mode [" << m_calibMode << "] by one from data: " << calibMode );
      m_calibMode = calibMode;
    }

    m_cispar = dqStatus->cispar();
    StoreRunInfo(dqStatus); // done only once
  }

  memset(m_ecell_ene     ,0,          2 * sizeof( *m_ecell_ene     ));
  memset(m_cell_nch      ,0,          2 * sizeof( *m_cell_nch      ));

  m_cispar = dqStatus->cispar();
  if (m_evtNr % 1000 == 0) ATH_MSG_INFO( " events processed so far " << m_evtNr );
  

  if (m_doFit) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerFitKey, Fit).isFailure());}
  if (m_doFixed) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerFixedKey, Fixed).isFailure());}
  if (m_doOpt) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerOptKey, Opt).isFailure());}
  if (m_doDsp) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerDspKey, Dsp).isFailure());}
  if (m_doOF1) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerOF1Key, OF1).isFailure());}
  if (m_doMF) {empty &= (fillRawChannels(dqStatus, m_rawChannelContainerMFKey, MF).isFailure());}


  if (empty) {ATH_MSG_ERROR( "Error in execute " ); }
  ++m_evtNr;

  return StatusCode::SUCCESS;
}


///The output ntuple is created in finalize method
StatusCode TileRawChNoiseCalibAlg::finalize() {

  ATH_MSG_INFO( "Finalizing TileRawChNoiseCalibAlg" );

  if (m_doFit) finalRawCh(Fit);
  if (m_doFixed) finalRawCh(Fixed);
  if (m_doOpt) finalRawCh(Opt);
  if (m_doDsp) finalRawCh(Dsp);
  if (m_doOF1) finalRawCh(OF1);
  if (m_doMF) finalRawCh(MF);

  if (m_UseforCells == 0) {
    if (m_doFit) finalCell();
  } else if (m_UseforCells == 1) {
    if (m_doFixed) finalCell();
  } else if (m_UseforCells == 2) {
    if (m_doOpt) finalCell();
  } else if (m_UseforCells == 3) {
    if (m_doDsp) finalCell();
  } else if (m_UseforCells == 4) {
    if (m_doOF1) finalCell();
  } else if (m_UseforCells == 5) {
    if (m_doMF) finalCell();
  } else {
    ATH_MSG_WARNING( "unknown rawchannel type used for cells" << m_UseforCells );
  }

  std::ostringstream sStr;
  std::string trig_str;

  if (m_trigType == Phys) trig_str = "Phys";
  else if (m_trigType == Las) trig_str = "Las";
  else if (m_trigType == Ped) trig_str = "Ped";
  else if (m_trigType == Cis) trig_str = "Cis";
  else {
    ATH_MSG_WARNING( "Unknown trigger type " << m_trigType );
    trig_str = "Unk";
  }

  sStr << m_file << "_" << m_run  << "_" << trig_str << ".root";
  m_file = sStr.str();
  ATH_MSG_INFO( "Writing calibrations to file " << m_file );

  // Create output file: for now creating file for just this
  // algorithm; want to add to ntuple file eventually??
  TFile* fout = new TFile(m_file.c_str(), "recreate");

  // Create tree with branches
  TTree* t = new TTree(m_ntupleID.c_str(), "Tile_RC_NoiseCalib-Ntuple");

  t->Branch("RunNumber", &m_run, "RunNumber/I");
  t->Branch("TrigType", &m_trigType, "TrigType/I");
  t->Branch("Time", &m_time, "Time/I");
  t->Branch("Year", &m_year, "Year/I");
  t->Branch("Month", &m_month, "Month/I");
  t->Branch("Day", &m_day, "Day/I");
  t->Branch("YDay", &m_yday, "YDay/I");
  t->Branch("Hour", &m_hour, "Hour/I");
  t->Branch("Min", &m_min, "Min/I"); 
  t->Branch("nEvt", &m_evtNr, "nEvt/I"); // events processed
  t->Branch("EvtNr", &m_evtNr, "EvtNr/I");
  t->Branch("EvtGood", *m_evt, "Evt[5][64][48][2]/I"); // events used in the noise calculation for every channel


  t->Branch("ros", *m_ros, "ros[5][64][48][2]/b");
  t->Branch("drawer", *m_drawer, "drawer[5][64][48][2]/b");
  t->Branch("channel", *m_channel, "channel[5][64][48][2]/b");
  t->Branch("gain", *m_gain, "gain[5][64][48][2]/O");

  t->Branch("efit_mean",*(m_rc_mean[Fit]),"efit_mean[5][64][48][2]/F");
  t->Branch("efit_av",*(m_rc_av[Fit]),"efit_av[5][64][48][2]/F");
  t->Branch("efit_rms",*(m_rc_rms[Fit]),"efit_rms[5][64][48][2]/F");
  t->Branch("efit_sigma",*(m_rc_sigma[Fit]),"efit_sigma[5][64][48][2]/F");
  t->Branch("efit_mean_err",*(m_rc_mean_err[Fit]),"efit_mean_err[5][64][48][2]/F");
  t->Branch("efit_sigma_err",*(m_rc_sigma_err[Fit]),"efit_sigma_err[5][64][48][2]/F");
  t->Branch("efit_kurtosis",*(m_rc_kurtosis[Fit]),"efit_kurtosis[5][64][48][2]/F");
  t->Branch("efit_skewness",*(m_rc_skewness[Fit]),"efit_skewness[5][64][48][2]/F");
  t->Branch("efit_chi2",*(m_rc_chi2[Fit]),"efit_chi2[5][64][48][2]/F");
  t->Branch("efit_ndf",*(m_rc_ndf[Fit]),"efit_ndf[5][64][48][2]/F");
  t->Branch("efit_probC2",*(m_rc_probC2[Fit]),"efit_probC2[5][64][48][2]/F");

  t->Branch("efit_gsigma1",*(m_rc_gsigma1[Fit]),"efit_gsigma1[5][64][48][2]/F");
  t->Branch("efit_gsigma2",*(m_rc_gsigma2[Fit]),"efit_gsigma2[5][64][48][2]/F");
  t->Branch("efit_gnorm",*(m_rc_gnorm[Fit]),"efit_gnorm[5][64][48][2]/F");
  t->Branch("efit_gchi2",*(m_rc_gchi2[Fit]),"efit_gchi2[5][64][48][2]/F");
  t->Branch("efit_gerrsigma1",*(m_rc_gerrsigma1[Fit]),"efit_gerrsigma1[5][64][48][2]/F");
  t->Branch("efit_gerrnorm",*(m_rc_gerrnorm[Fit]),"efit_gerrnorm[5][64][48][2]/F");
  t->Branch("efit_gerrsigma2",*(m_rc_gerrsigma2[Fit]),"efit_gerrsigma2[5][64][48][2]/F");  
  t->Branch("efit_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[Fit]),"efit_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("efixed_mean",*(m_rc_mean[Fixed]),"efixed_mean[5][64][48][2]/F");
  t->Branch("efixed_av",*(m_rc_av[Fixed]),"efixed_av[5][64][48][2]/F");
  t->Branch("efixed_rms",*(m_rc_rms[Fixed]),"efixed_rms[5][64][48][2]/F");
  t->Branch("efixed_sigma",*(m_rc_sigma[Fixed]),"efixed_sigma[5][64][48][2]/F");
  t->Branch("efixed_mean_err",*(m_rc_mean_err[Fixed]),"efixed_mean_err[5][64][48][2]/F");
  t->Branch("efixed_sigma_err",*(m_rc_sigma_err[Fixed]),"efixed_sigma_err[5][64][48][2]/F");
  t->Branch("efixed_kurtosis",*(m_rc_kurtosis[Fixed]),"efixed_kurtosis[5][64][48][2]/F");
  t->Branch("efixed_skewness",*(m_rc_skewness[Fixed]),"efixed_skewness[5][64][48][2]/F");
  t->Branch("efixed_chi2",*(m_rc_chi2[Fixed]),"efixed_chi2[5][64][48][2]/F");
  t->Branch("efixed_ndf",*(m_rc_ndf[Fixed]),"efixed_ndf[5][64][48][2]/F");
  t->Branch("efixed_probC2",*(m_rc_probC2[Fixed]),"efixed_probC2[5][64][48][2]/F");

  t->Branch("efixed_gsigma1",*(m_rc_gsigma1[Fixed]),"efixed_gsigma1[5][64][48][2]/F");
  t->Branch("efixed_gsigma2",*(m_rc_gsigma2[Fixed]),"efixed_gsigma2[5][64][48][2]/F");
  t->Branch("efixed_gnorm",*(m_rc_gnorm[Fixed]),"efixed_gnorm[5][64][48][2]/F");
  t->Branch("efixed_gchi2",*(m_rc_gchi2[Fixed]),"efixed_gchi2[5][64][48][2]/F");
  t->Branch("efixed_gerrsigma1",*(m_rc_gerrsigma1[Fixed]),"efixed_gerrsigma1[5][64][48][2]/F");
  t->Branch("efixed_gerrnorm",*(m_rc_gerrnorm[Fixed]),"efixed_gerrnorm[5][64][48][2]/F");
  t->Branch("efixed_gerrsigma2",*(m_rc_gerrsigma2[Fixed]),"efixed_gerrsigma2[5][64][48][2]/F");  
  t->Branch("efixed_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[Fixed]),"efixed_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("eopt_mean",*(m_rc_mean[Opt]),"eopt_mean[5][64][48][2]/F");
  t->Branch("eopt_av",*(m_rc_av[Opt]),"eopt_av[5][64][48][2]/F");
  t->Branch("eopt_rms",*(m_rc_rms[Opt]),"eopt_rms[5][64][48][2]/F");
  t->Branch("eopt_sigma",*(m_rc_sigma[Opt]),"eopt_sigma[5][64][48][2]/F");
  t->Branch("eopt_mean_err",*(m_rc_mean_err[Opt]),"eopt_mean_err[5][64][48][2]/F");
  t->Branch("eopt_sigma_err",*(m_rc_sigma_err[Opt]),"eopt_sigma_err[5][64][48][2]/F");
  t->Branch("eopt_kurtosis",*(m_rc_kurtosis[Opt]),"eopt_kurtosis[5][64][48][2]/F");
  t->Branch("eopt_skewness",*(m_rc_skewness[Opt]),"eopt_skewness[5][64][48][2]/F");
  t->Branch("eopt_chi2",*(m_rc_chi2[Opt]),"eopt_chi2[5][64][48][2]/F");
  t->Branch("eopt_ndf",*(m_rc_ndf[Opt]),"eopt_ndf[5][64][48][2]/F");
  t->Branch("eopt_probC2",*(m_rc_probC2[Opt]),"eopt_probC2[5][64][48][2]/F");

  t->Branch("eopt_gsigma1",*(m_rc_gsigma1[Opt]),"eopt_gsigma1[5][64][48][2]/F");
  t->Branch("eopt_gsigma2",*(m_rc_gsigma2[Opt]),"eopt_gsigma2[5][64][48][2]/F");
  t->Branch("eopt_gnorm",*(m_rc_gnorm[Opt]),"eopt_gnorm[5][64][48][2]/F");
  t->Branch("eopt_gchi2",*(m_rc_gchi2[Opt]),"eopt_gchi2[5][64][48][2]/F");
  t->Branch("eopt_gerrsigma1",*(m_rc_gerrsigma1[Opt]),"eopt_gerrsigma1[5][64][48][2]/F");
  t->Branch("eopt_gerrnorm",*(m_rc_gerrnorm[Opt]),"eopt_gerrnorm[5][64][48][2]/F");
  t->Branch("eopt_gerrsigma2",*(m_rc_gerrsigma2[Opt]),"eopt_gerrsigma2[5][64][48][2]/F");  
  t->Branch("eopt_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[Opt]),"eopt_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("edsp_mean",*(m_rc_mean[Dsp]),"edsp_mean[5][64][48][2]/F");
  t->Branch("edsp_av",*(m_rc_av[Dsp]),"edsp_av[5][64][48][2]/F");
  t->Branch("edsp_rms",*(m_rc_rms[Dsp]),"edsp_rms[5][64][48][2]/F");
  t->Branch("edsp_sigma",*(m_rc_sigma[Dsp]),"edsp_sigma[5][64][48][2]/F");
  t->Branch("edsp_mean_err",*(m_rc_mean_err[Dsp]),"edsp_mean_err[5][64][48][2]/F");
  t->Branch("edsp_sigma_err",*(m_rc_sigma_err[Dsp]),"edsp_sigma_err[5][64][48][2]/F");
  t->Branch("edsp_kurtosis",*(m_rc_kurtosis[Dsp]),"edsp_kurtosis[5][64][48][2]/F");
  t->Branch("edsp_skewness",*(m_rc_skewness[Dsp]),"edsp_skewness[5][64][48][2]/F");
  t->Branch("edsp_chi2",*(m_rc_chi2[Dsp]),"edsp_chi2[5][64][48][2]/F");
  t->Branch("edsp_ndf",*(m_rc_ndf[Dsp]),"edsp_ndf[5][64][48][2]/F");
  t->Branch("edsp_probC2",*(m_rc_probC2[Dsp]),"edsp_probC2[5][64][48][2]/F");

  t->Branch("edsp_gsigma1",*(m_rc_gsigma1[Dsp]),"edsp_gsigma1[5][64][48][2]/F");
  t->Branch("edsp_gsigma2",*(m_rc_gsigma2[Dsp]),"edsp_gsigma2[5][64][48][2]/F");
  t->Branch("edsp_gnorm",*(m_rc_gnorm[Dsp]),"edsp_gnorm[5][64][48][2]/F");
  t->Branch("edsp_gchi2",*(m_rc_gchi2[Dsp]),"edsp_gchi2[5][64][48][2]/F");
  t->Branch("edsp_gerrsigma1",*(m_rc_gerrsigma1[Dsp]),"edsp_gerrsigma1[5][64][48][2]/F");
  t->Branch("edsp_gerrnorm",*(m_rc_gerrnorm[Dsp]),"edsp_gerrnorm[5][64][48][2]/F");
  t->Branch("edsp_gerrsigma2",*(m_rc_gerrsigma2[Dsp]),"edsp_gerrsigma2[5][64][48][2]/F");  
  t->Branch("edsp_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[Dsp]),"edsp_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("eOF1_mean",*(m_rc_mean[OF1]),"eOF1_mean[5][64][48][2]/F");
  t->Branch("eOF1_av",*(m_rc_av[OF1]),"eOF1_av[5][64][48][2]/F");
  t->Branch("eOF1_rms",*(m_rc_rms[OF1]),"eOF1_rms[5][64][48][2]/F");
  t->Branch("eOF1_sigma",*(m_rc_sigma[OF1]),"eOF1_sigma[5][64][48][2]/F");
  t->Branch("eOF1_mean_err",*(m_rc_mean_err[OF1]),"eOF1_mean_err[5][64][48][2]/F");
  t->Branch("eOF1_sigma_err",*(m_rc_sigma_err[OF1]),"eOF1_sigma_err[5][64][48][2]/F");
  t->Branch("eOF1_kurtosis",*(m_rc_kurtosis[OF1]),"eOF1_kurtosis[5][64][48][2]/F");
  t->Branch("eOF1_skewness",*(m_rc_skewness[OF1]),"eOF1_skewness[5][64][48][2]/F");
  t->Branch("eOF1_chi2",*(m_rc_chi2[OF1]),"eOF1_chi2[5][64][48][2]/F");
  t->Branch("eOF1_ndf",*(m_rc_ndf[OF1]),"eOF1_ndf[5][64][48][2]/F");
  t->Branch("eOF1_probC2",*(m_rc_probC2[OF1]),"eOF1_probC2[5][64][48][2]/F");

  t->Branch("eOF1_gsigma1",*(m_rc_gsigma1[OF1]),"eOF1_gsigma1[5][64][48][2]/F");
  t->Branch("eOF1_gsigma2",*(m_rc_gsigma2[OF1]),"eOF1_gsigma2[5][64][48][2]/F");
  t->Branch("eOF1_gnorm",*(m_rc_gnorm[OF1]),"eOF1_gnorm[5][64][48][2]/F");
  t->Branch("eOF1_gchi2",*(m_rc_gchi2[OF1]),"eOF1_gchi2[5][64][48][2]/F");
  t->Branch("eOF1_gerrsigma1",*(m_rc_gerrsigma1[OF1]),"eOF1_gerrsigma1[5][64][48][2]/F");
  t->Branch("eOF1_gerrnorm",*(m_rc_gerrnorm[OF1]),"eOF1_gerrnorm[5][64][48][2]/F");
  t->Branch("eOF1_gerrsigma2",*(m_rc_gerrsigma2[OF1]),"eOF1_gerrsigma2[5][64][48][2]/F");  
  t->Branch("eOF1_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[OF1]),"eOF1_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("eMF_mean",*(m_rc_mean[MF]),"eMF_mean[5][64][48][2]/F");
  t->Branch("eMF_av",*(m_rc_av[MF]),"eMF_av[5][64][48][2]/F");
  t->Branch("eMF_rms",*(m_rc_rms[MF]),"eMF_rms[5][64][48][2]/F");
  t->Branch("eMF_sigma",*(m_rc_sigma[MF]),"eMF_sigma[5][64][48][2]/F");
  t->Branch("eMF_mean_err",*(m_rc_mean_err[MF]),"eMF_mean_err[5][64][48][2]/F");
  t->Branch("eMF_sigma_err",*(m_rc_sigma_err[MF]),"eMF_sigma_err[5][64][48][2]/F");
  t->Branch("eMF_kurtosis",*(m_rc_kurtosis[MF]),"eMF_kurtosis[5][64][48][2]/F");
  t->Branch("eMF_skewness",*(m_rc_skewness[MF]),"eMF_skewness[5][64][48][2]/F");
  t->Branch("eMF_chi2",*(m_rc_chi2[MF]),"eMF_chi2[5][64][48][2]/F");
  t->Branch("eMF_ndf",*(m_rc_ndf[MF]),"eMF_ndf[5][64][48][2]/F");
  t->Branch("eMF_probC2",*(m_rc_probC2[MF]),"eMF_probC2[5][64][48][2]/F");

  t->Branch("eMF_gsigma1",*(m_rc_gsigma1[MF]),"eMF_gsigma1[5][64][48][2]/F");
  t->Branch("eMF_gsigma2",*(m_rc_gsigma2[MF]),"eMF_gsigma2[5][64][48][2]/F");
  t->Branch("eMF_gnorm",*(m_rc_gnorm[MF]),"eMF_gnorm[5][64][48][2]/F");
  t->Branch("eMF_gchi2",*(m_rc_gchi2[MF]),"eMF_gchi2[5][64][48][2]/F");
  t->Branch("eMF_gerrsigma1",*(m_rc_gerrsigma1[MF]),"eMF_gerrsigma1[5][64][48][2]/F");
  t->Branch("eMF_gerrnorm",*(m_rc_gerrnorm[MF]),"eMF_gerrnorm[5][64][48][2]/F");
  t->Branch("eMF_gerrsigma2",*(m_rc_gerrsigma2[MF]),"eMF_gerrsigma2[5][64][48][2]/F");  
  t->Branch("eMF_gcorrsigma1sigma2",*(m_rc_gcorrsigma1sigma2[MF]),"eMF_gcorrsigma1sigma2[5][64][48][2]/F");


  t->Branch("ecell_av",*(m_ecell_av),"ecell_av[2][64][4][17][6]/F");
  t->Branch("ecell_rms",*(m_ecell_rms),"ecell_rms[2][64][4][17][6]/F");
  t->Branch("ecell_hash",*(m_ecell_hash),"ecell_hash[2][64][4][17]/i");
  t->Branch("ecell_gsigma1",*(m_gsigma1),"ecell_gsigma1[2][64][4][17][6]/F");
  t->Branch("ecell_gsigma2",*(m_gsigma2),"ecell_gsigma2[2][64][4][17][6]/F");
  t->Branch("ecell_gnorm",*(m_gnorm),"ecell_gnorm[2][64][4][17][6]/F");
  t->Branch("ecell_gchi2",*(m_gchi2),"ecell_gchi2[2][64][4][17][6]/F");
  t->Branch("ecell_gerrsigma1",*(m_gerrsigma1),"ecell_gerrsigma1[2][64][4][17][6]/F");
  t->Branch("ecell_gerrnorm",*(m_gerrnorm),"ecell_gerrnorm[2][64][4][17][6]/F");
  t->Branch("ecell_gerrsigma2",*(m_gerrsigma2),"ecell_gerrsigma2[2][64][4][17][6]/F");  
  t->Branch("ecell_gcorrsigma1sigma2",*(m_gcorrsigma1sigma2),"ecell_gcorrsigma1sigma2[2][64][4][17][6]/F");
//  t->Branch("ecell_gcorrsigma1norm",*(gcorrsigma1norm),"ecell_gcorrsigma1norm[2][64][4][17][6]/F");
//  t->Branch("ecell_gcorrsigma2norm",*(gcorrsigma2norm),"ecell_gcorrsigma2norm[2][64][4][17][6]/F");
  t->Branch("side", *m_side, "side[2][64][4][17][6]/O");
  t->Branch("phi", *m_phi, "phi[2][64][4][17][6]/b");
  t->Branch("sample", *m_sample, "sample[2][64][4][17][6]/b");
  t->Branch("tower", *m_tower, "tower[2][64][4][17][6]/b");
  t->Branch("gaingain", *m_gg, "gaingain[2][64][4][17][6]/b");

  // Fill with current values (i.e. tree will have only one entry for this whole run)

  t->Fill();
  t->Write();

  if (m_saveHist) {
      
    for (int rc = 0; rc < RCnum; ++rc) {

      if ((m_doFit && rc == Fit)
          || (m_doFixed && rc == Fixed)
          || (m_doOpt && rc == Opt)
          || (m_doDsp && rc == Dsp)
          || (m_doOF1 && rc == OF1)
          || (m_doMF && rc == MF)) {

        for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
          for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
            for (unsigned int ch = 0; ch < TileCalibUtils::MAX_CHAN; ++ch) {
              for (unsigned int g = 0; g < TileCalibUtils::MAX_GAIN; ++g) {
                m_histAmp[rc][ros][drawer][ch][g]->Write();
              }
            }
          }
        }
      }
    }
    
    for (int side = 0; side < 2; side++) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
        for (int sample = 0; sample < 4; ++sample) {
          for (int tower = 0; tower < 17; ++tower) {
            for (int gg = 0; gg < 6; ++gg) {
              m_histCellAmp[side][drawer][sample][tower][gg]->Write();
            }
          }
        }
      }
    }
  }

  fout->Close();


  deleteHist();

  return StatusCode::SUCCESS;
}

/// StoreRunInfo is called only during the first event
void TileRawChNoiseCalibAlg::StoreRunInfo(const TileDQstatus* dqStatus) {

  MsgStream log(msgSvc(), name());

  if (dqStatus->calibMode() == 1 && m_beamElemContainer.length() > 0) {// Bigain can use cispar

    if (m_beamCnv) {

      if (m_beamCnv->validBeamFrag()) {
        m_run = m_beamCnv->robFragment()->rod_run_no();   // take it from beam ROD header
      } else {
        m_run = 0;
      }
    } else
      m_run = 0;

    if (dqStatus && m_cispar) {
      m_time = m_cispar[10]; //time in sc from 1970
      m_trigType = m_cispar[12];
    } else {
      m_time = 0;
      m_year = 0;
      m_month = 0;
      m_day = 0;
      m_yday = 0;
      m_hour = 0;
      m_min = 0;
      m_trigType = 0;
    }
  } else { // monogain can use eventinfo

    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey); 
    if ( !eventInfo.isValid() ) {
      ATH_MSG_ERROR( "No EventInfo object found! Can't read run number!" );
      m_run = 0;
      m_time = 0;
      m_trigType = 0;
    } else {
      m_run = eventInfo->runNumber();
      m_time = eventInfo->timeStamp();
      if (!(eventInfo->eventType(xAOD::EventInfo::IS_CALIBRATION))) // if not calibration, physics
        m_trigType = 1;
      else
        m_trigType = 0;
    }
  }

  if (m_time != 0) {
    time_t t_time = m_time;
    tm* timestr = localtime(&t_time);
    m_year = (*timestr).tm_year + 1900;
    m_month = (*timestr).tm_mon + 1;
    m_day = (*timestr).tm_mday;
    m_yday = (*timestr).tm_yday + 1;
    m_hour = (*timestr).tm_hour;
    m_min = (*timestr).tm_min;
  } else {
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_yday = 0;
    m_hour = 0;
    m_min = 0;
  }
}

// fillRawChannels is called at every events.
// Statistics is summed for Average, RMS calculations
/*---------------------------------------------------------*/
StatusCode TileRawChNoiseCalibAlg::fillRawChannels(const TileDQstatus* dqStatus,
                                                   const SG::ReadHandleKey<TileRawChannelContainer>& rawChannelContainerKey,
                                                   RCtype rctype) {
/*---------------------------------------------------------*/

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(rawChannelContainerKey);
  ATH_CHECK( rawChannelContainer.isValid() );

  if ((rctype == Dsp) && (m_trigType != Phys)) {
    ATH_MSG_ERROR( "Removing DSP RawChannelContainer for non Phys run. TrigType is: " << m_trigType );
    return StatusCode::FAILURE;
  }

  TileRawChannelUnit::UNIT RChUnit = rawChannelContainer->get_unit();

  TileRawChannelContainer::const_iterator collItr = rawChannelContainer->begin();
  TileRawChannelContainer::const_iterator lastColl = rawChannelContainer->end();

  for (; collItr != lastColl; ++collItr) {

    TileRawChannelCollection::const_iterator chItr = (*collItr)->begin();
    TileRawChannelCollection::const_iterator lastCh = (*collItr)->end();

    for (; chItr != lastCh; ++chItr) {

      const TileRawChannel* rch = (*chItr);

      HWIdentifier adc_id = (*chItr)->adc_HWID();
      unsigned int ros(0), drawer(0), channel(0), gain(0);
      m_tileIdTrans->getIndices(adc_id, ros, drawer, channel, gain);
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      if (dqStatus->isChEmpty(ros, drawer, channel)) continue;

      // If DQ problem, do not fill calib ntuple
      if (m_calibMode == 1) { // Bigain: check indivual adc's

        if (!(dqStatus->isAdcDQgood(ros, drawer, channel, gain))) {
          ATH_MSG_VERBOSE(  "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
                           << " channel: " << channel
                           << " ADC: " << gain << " due to DQ error found." );

          continue;
        }
      } else { // monogain, just check channel

        if (!(dqStatus->isChanDQgood(ros, drawer, channel))) {
          ATH_MSG_VERBOSE( "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
                           << " channel: " << channel << " due to DQ error found."  );

          continue;
        }
      }

      // we fill the cell information now for selected method
      // note that fillCell is called only for good channels
      if (rctype == m_UseforCells) fillCell(RChUnit, rch);

      double amp = rch->amplitude();
      if (RChUnit > TileRawChannelUnit::OnlineADCcounts) {
        // go from online units to ADC counts
        amp = m_tileToolEmscale->undoOnlCalib(drawerIdx, channel, gain, amp, RChUnit);
      } else if (RChUnit == TileRawChannelUnit::OnlineADCcounts) {
        // nothing to do
      } else if (RChUnit > TileRawChannelUnit::ADCcounts) {
        // go from offline units to ADC counts
        amp /= m_tileToolEmscale->channelCalib(drawerIdx, channel, gain, 1.0, TileRawChannelUnit::ADCcounts, RChUnit);
      }

      // PMT-1 or channel index depending on jobOptions
      unsigned int chan = (m_usePMT) ? digiChannel2PMT(ros, channel) : channel;

      m_evt[ros][drawer][chan][gain]++;
      m_histAmp[rctype][ros][drawer][chan][gain]->Fill(amp);
    }
  }

  if (rctype == m_UseforCells) fillCellHist(); //we fill the cell histograms only at the end of the loop over the channels,
                                               //when all the cells have been built

  return StatusCode::SUCCESS;
}

/// finalDigits is called during finalize
/// Here the average Ped, lfn, hfn and covariance are calculated.
/*---------------------------------------------------------*/
void TileRawChNoiseCalibAlg::finalRawCh(int rctype) {
  /*---------------------------------------------------------*/

  TF1 * fit_gaus = new TF1("g", "gaus");

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    ATH_MSG_INFO( "Fitting RCh container " << rctype << " ROS " << ros );

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
        for (unsigned int chan = 0; chan < TileCalibUtils::MAX_CHAN; ++chan) {

          if (!m_fillidx) {
            m_fillidx = true;
            m_ros[ros][drawer][chan][gain] = ros;
            m_drawer[ros][drawer][chan][gain] = drawer;
            m_channel[ros][drawer][chan][gain] = chan;
            m_gain[ros][drawer][chan][gain] = gain;
          }

          if (m_evt[ros][drawer][chan][gain] > 0) {

            m_histAmp[rctype][ros][drawer][chan][gain]->Fit("g", "NQ");

            m_rc_av[rctype][ros][drawer][chan][gain] = m_histAmp[rctype][ros][drawer][chan][gain]->GetMean();
            m_rc_rms[rctype][ros][drawer][chan][gain] = m_histAmp[rctype][ros][drawer][chan][gain]->GetRMS();

            if (TMath::Abs(m_histAmp[rctype][ros][drawer][chan][gain]->GetSkewness()) < 1000.)
              m_rc_skewness[rctype][ros][drawer][chan][gain] = m_histAmp[rctype][ros][drawer][chan][gain]->GetSkewness();
            if (TMath::Abs(m_histAmp[rctype][ros][drawer][chan][gain]->GetKurtosis()) < 1000.)
              m_rc_kurtosis[rctype][ros][drawer][chan][gain] = m_histAmp[rctype][ros][drawer][chan][gain]->GetKurtosis();

            m_rc_mean[rctype][ros][drawer][chan][gain] = fit_gaus->GetParameter(1);
            m_rc_mean_err[rctype][ros][drawer][chan][gain] = fit_gaus->GetParError(1);
            m_rc_sigma[rctype][ros][drawer][chan][gain] = fit_gaus->GetParameter(2);
            m_rc_sigma_err[rctype][ros][drawer][chan][gain] = fit_gaus->GetParError(2);
            m_rc_chi2[rctype][ros][drawer][chan][gain] = fit_gaus->GetChisquare();
            m_rc_ndf[rctype][ros][drawer][chan][gain] = fit_gaus->GetNDF();
            m_rc_probC2[rctype][ros][drawer][chan][gain] = fit_gaus->GetProb();

            doFit(m_histAmp[rctype][ros][drawer][chan][gain], m_rc_ggpar[rctype][ros][drawer][chan][gain], m_invertChanRatio);

            m_rc_gsigma1[rctype][ros][drawer][chan][gain] = m_rc_ggpar[rctype][ros][drawer][chan][gain][0];
            m_rc_gsigma2[rctype][ros][drawer][chan][gain] = m_rc_ggpar[rctype][ros][drawer][chan][gain][2];
            m_rc_gnorm[rctype][ros][drawer][chan][gain] =   m_rc_ggpar[rctype][ros][drawer][chan][gain][1];
            m_rc_gchi2[rctype][ros][drawer][chan][gain] =   m_rc_ggpar[rctype][ros][drawer][chan][gain][3];
            m_rc_gerrsigma1[rctype][ros][drawer][chan][gain] = m_rc_ggpar[rctype][ros][drawer][chan][gain][4];
            m_rc_gerrnorm[rctype][ros][drawer][chan][gain] =   m_rc_ggpar[rctype][ros][drawer][chan][gain][5];
            m_rc_gerrsigma2[rctype][ros][drawer][chan][gain] = m_rc_ggpar[rctype][ros][drawer][chan][gain][6];
            m_rc_gcorrsigma1sigma2[rctype][ros][drawer][chan][gain] = m_rc_ggpar[rctype][ros][drawer][chan][gain][7];

          } // end if evt>0 

        }
      }
    }
  } // end if ros

  delete fit_gaus;

}


// fillCell is called at every events.
// Statistics is summed for Average, RMS calculations
/*---------------------------------------------------------*/
void TileRawChNoiseCalibAlg::fillCell(TileRawChannelUnit::UNIT RChUnit, const TileRawChannel * rch) {
  /*---------------------------------------------------------*/

  int index, pmt;
  Identifier cell_id = rch->cell_ID_index(index, pmt);
  if (index > -1) { //disconnect channel index is -1/ MBTS cell index is -2 and they don't have an idhash
    unsigned int ros(0), drawer(0), channel(0), gain(0);
    m_tileIdTrans->getIndices(rch->adc_HWID(), ros, drawer, channel, gain);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

    if (m_maskBadChannels && m_tileBadChanTool->getAdcStatus(drawerIdx, channel, gain).isBad()) {
      ATH_MSG_VERBOSE( "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
                       << " channel: " << channel
                       << " ADC: " << gain
                       << " in fillCell() because channel is bad in DB." );
      return;
    }

    int sample = m_tileID->sample(cell_id);
    int tower = m_tileID->tower(cell_id);
    int side = m_tileID->side(cell_id);

    if (side == 1) side = 0; //A side
    else if (side == -1) side = 1; //C side
    else side = 0; //D0 cell? we put it in LBA

    if (m_evtNr < 1) { //first event
      m_ecell_hash[side][drawer][sample][tower] = m_tileID->cell_hash(cell_id);
    }

    int g, gg;
    if (gain == 0) {
      gg = 0;
      if (pmt == 0) g = 1; //in most cases even channels on side A AND odd channels on side C
      else  g = 2;                        //are the first channels of the cells
    } else {
      gg = 3;
      if (pmt == 0) g = 4; //pmt tells us which is the first and second channel of the cells!!
      else g = 5;
    }

    //not needed anymore    if (channel==0 && ros==2) g++;  //D0 second channel is special : move from first to second channel

    double amp = rch->amplitude();
    amp = m_tileToolEmscale->channelCalib(drawerIdx, channel, gain, amp, RChUnit, TileRawChannelUnit::MegaElectronVolts);
    int nch = 1;
    
    if (m_cabling->isRun2Cabling() && (ros > 2)) { // Ext.barrel modules
        
      if (channel == E1_CHANNEL) { // Raw channel -> E1 cell.
        int drawer2 = m_cabling->E1_merged_with_run2(ros, drawer);
        if (drawer2 != 0) { // Raw channel splitted into two E1 cells for Run 2.
          amp /= 2.0F;
          m_ecell_ene[side][drawer2][sample][tower][gg / 3] += amp;
          ++m_cell_nch[side][drawer2][sample][tower][gg / 3];
        
          if (TMath::Abs(amp) > 1.e-5) {
            m_histCellAmp[side][drawer2][sample][tower][g]->Fill(amp);
          }
        }

      } else if (!m_cabling->C10_connected(drawer)) { // modules with special C10
        if (channel == OUTER_MBTS_CHANNEL) {
          amp = 0.0; // ignore MBTS completely
          nch = 0;
        } else if (channel == SPECIAL_C10_CHANNEL) {
          nch = 2; // count this channel twice - needed for correct bad-channel masking
        }
      }
    }

    m_ecell_ene[side][drawer][sample][tower][gg / 3] += amp;
    m_cell_nch[side][drawer][sample][tower][gg / 3] += nch;

    if (TMath::Abs(amp) > 1.e-5) {
      m_histCellAmp[side][drawer][sample][tower][g]->Fill(amp);
    }

  } // is a connected channel
}


/// finalCell is called during finalize
/// Here the cell variables of the ntuple are filled.
/*---------------------------------------------------------*/
void TileRawChNoiseCalibAlg::finalCell() {
  /*---------------------------------------------------------*/

  for (int side = 0; side < 2; ++side) {
    ATH_MSG_INFO( "Fitting cells side " << side );
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int sample = 0; sample < 4; ++sample) {
        for (int tower = 0; tower < 17; ++tower) {
          for (int gg = 0; gg < 6; ++gg) {

            m_side[side][drawer][sample][tower][gg] = side;
            m_phi[side][drawer][sample][tower][gg] = drawer;
            m_sample[side][drawer][sample][tower][gg] = sample;
            m_tower[side][drawer][sample][tower][gg] = tower;
            m_gg[side][drawer][sample][tower][gg] = gg;

            if (m_histCellAmp[side][drawer][sample][tower][gg]->GetEntries() > 0) {

              m_ecell_av[side][drawer][sample][tower][gg] = m_histCellAmp[side][drawer][sample][tower][gg]->GetMean();
              m_ecell_rms[side][drawer][sample][tower][gg] = m_histCellAmp[side][drawer][sample][tower][gg]->GetRMS();
              doFit(m_histCellAmp[side][drawer][sample][tower][gg], m_ggpar[side][drawer][sample][tower][gg]);
              m_gsigma1[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][0];
              m_gsigma2[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][2];
              m_gnorm[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][1];
              m_gchi2[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][3];
              m_gerrsigma1[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][4];
              m_gerrnorm[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][5];
              m_gerrsigma2[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][6];
              m_gcorrsigma1sigma2[side][drawer][sample][tower][gg] = m_ggpar[side][drawer][sample][tower][gg][7];
//              gcorrsigma1norm[side][drawer][sample][tower][gg]=m_ggpar[side][drawer][sample][tower][gg][8];
//              gcorrsigma2norm[side][drawer][sample][tower][gg]=m_ggpar[side][drawer][sample][tower][gg][9];

            }

          }
        }
      }
    }
  }

}

/// doFit performs the double gaussian fit of the amplitude
/*---------------------------------------------------------*/
void  TileRawChNoiseCalibAlg::doFit(TH1F* h, float* gp, bool invert) {
/*---------------------------------------------------------*/
  Double_t par[6];

  float xmin = h->GetBinCenter(1);
  float xmax = h->GetBinCenter(h->GetNbinsX());
  TF1* total = new TF1("total", "gaus(0)+gaus(3)", xmin, xmax);
  total->SetLineColor(2);

  float nentries = h->GetEntries();
  float rms = h->GetRMS();
  float bin = h->GetBinWidth(0);

  par[0] = 0.1 * nentries;
  par[1] = 0.;
  par[2] = 0.7 * std::max(rms, bin);

  par[3] = 0.15 * par[0];
  par[4] = 0.;
  par[5] = 5. * par[2];

  total->SetParameters(par);

  float lim1 = bin / 2.;
  float lim2 = std::max(rms * 1.05, bin * 2.0);
  float lim3 = std::max(rms * 10.0, bin * 20.);

  float limN1 = nentries;
  if (lim1 < 0.5) limN1 /= (2. * lim1); // a bit more than Nentries / ( sqrt(2*pi) * sigma1 )
  float limN2 = nentries;
  if (lim2 < 0.5) limN2 /= (2. * lim2); // a bit more than Nentries / ( sqrt(2*pi) * sigma2 )
   
  total->SetParLimits(0, 0., limN1);
  total->FixParameter(1, 0.);
  total->SetParLimits(2, lim1, lim2);
  total->SetParLimits(3, 0., limN2);
  total->FixParameter(4, 0.);
  total->SetParLimits(5, lim2, lim3);

  TFitResultPtr resfit = h->Fit(total, "BLQRS");

  float par0 = total->GetParameter(0);
  float par3 = total->GetParameter(3);

  float sigma1 = total->GetParameter(2); //sigma gauss1
  float sigma2 = total->GetParameter(5); //sigma gauss1

  //Get errors
  float errpar0 = total->GetParError(0);
  float errpar3 = total->GetParError(3);

  float errsigma1 = total->GetParError(2);
  float errsigma2 = total->GetParError(5);

  float norm = par3 / par0; //rel normalization of the gaussians

  if (invert && norm > 1.) {  //invert the 2 gaussians if normalization is greater than 1

    gp[0] = sigma2;
    gp[1] = 1. / norm;
    gp[2] = sigma1;

    gp[4] = errsigma2;
    gp[5] = sqrt((errpar0 * errpar0) + (errpar3 * errpar3) * (par0 * par0) / (par3 * par3)) / par3;
    gp[6] = errsigma1;

  } else {

    gp[0] = sigma1;
    gp[1] = norm;
    gp[2] = sigma2;

    gp[4] = errsigma1;
    gp[5] = sqrt((errpar3 * errpar3) + (errpar0 * errpar0) * (par3 * par3) / (par0 * par0)) / par0;
    gp[6] = errsigma2;

  }

  if (total->GetNDF() > 0) {
    gp[3] = total->GetChisquare() / total->GetNDF(); //chi2/ndf
  } else {
    gp[3] = 0.;
  }

  // Get correlation sigma1, sigma2
  TMatrixDSym corr = resfit->GetCorrelationMatrix();
  gp[7] = corr(2, 5);

  delete total;
}


/// fillCellHist is called during execute
/// It fill the HGHG and LGLG combination of the cell energies
/*---------------------------------------------------------*/
void TileRawChNoiseCalibAlg::fillCellHist() {
/*---------------------------------------------------------*/

  for (int side = 0; side < 2; ++side) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int sample = 0; sample < 4; ++sample) {
        for (int tower = 0; tower < 17; ++tower) {
          for (unsigned int gg = 0; gg < TileCalibUtils::MAX_GAIN; ++gg) {

            float ene = m_ecell_ene[side][drawer][sample][tower][gg];
            if (m_cell_nch[side][drawer][sample][tower][gg] == 1 && sample != 3) ene *= 2; // one good channel in normal cell - multiply energy by 2

            if (TMath::Abs(ene) > 1.e-5) {
              m_histCellAmp[side][drawer][sample][tower][gg * 3]->Fill(ene);
            }

          }
        }
      }
    }
  }

}



/// deleteHist is called at finalize to ensure that no histogram
/// goes into any output root file
/// delete [] didn't work properly
/*---------------------------------------------------------*/
void TileRawChNoiseCalibAlg::deleteHist() {
/*---------------------------------------------------------*/

  for (int rc = 0; rc < RCnum; ++rc) {
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

        for (unsigned int ch = 0; ch < TileCalibUtils::MAX_CHAN; ++ch) {
          for (unsigned int g = 0; g < TileCalibUtils::MAX_GAIN; ++g) {
            delete m_histAmp[rc][ros][drawer][ch][g];
          }
        }
      }
    }
  }

  for (int side = 0; side < 2; side++) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int sample = 0; sample < 4; ++sample) {
        for (int tower = 0; tower < 17; ++tower) {
          for (int gg = 0; gg < 6; gg++) {
            delete m_histCellAmp[side][drawer][sample][tower][gg];
          }
        }
      }
    }
  }

}
