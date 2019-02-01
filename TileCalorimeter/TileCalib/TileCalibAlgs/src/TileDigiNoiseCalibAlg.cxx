/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseCalib.cxx
// PACKAGE:  TileCalib
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// February 2008	
// ********************************************************************

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

//Event info
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileCalibAlgs/TileDigiNoiseCalibAlg.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileCalibAlgs/TileOFCorrelation.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TClonesArray.h"
#include <cmath>
#include <ctime>

TileDigiNoiseCalibAlg::TileDigiNoiseCalibAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_adderFilterAlgTool("TileRawChannelBuilderFlatFilter/TileAdderFlatFilter", this)
  , m_beamCnv(0)
  , m_cabling(0)
  , m_tileOFCorrelation(0)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cispar(0)
//  , m_nDrawers(0)
  , m_time(0)
  , m_year(0)
  , m_month(0)
  , m_day(0)
  , m_yday(0)
  , m_hour(0)
  , m_min(0)
  , m_trigType(0)
{
  declareProperty("TileAdderFlatFilter", m_adderFilterAlgTool);
  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");
  /*  declareProperty("TileRawChannelContainerFlat", m_flatRawChannelContainer = "TileRawChannelFlat");
   declareProperty("TileRawChannelContainerFit", m_fitRawChannelContainer = ""); // don't create
   declareProperty("TileRawChannelContainerOpt", m_optRawChannelContainer = ""); // by default */
  declareProperty("TileRawChannelContainerDsp", m_dspRawChannelContainer = "TileRawChannelCnt");
  declareProperty("CalibMode", m_calibMode = true);
  declareProperty("usePMT", m_usePMT = false);
  declareProperty("RunNumber", m_run = 0);
  declareProperty("FileNamePrefix", m_file = "Digi_NoiseCalib");
  declareProperty("NtupleID", m_ntupleID = "Digi_NoiseCalib");
  declareProperty("TreeSize", m_treeSize = 16000000000LL);
  declareProperty("NSamples", m_nSamples = 7);
  declareProperty("DoAvgCorr", m_doAvgCorr = false);
  declareProperty("DoRobustCov", m_doRobustCov = false);
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");

  m_run = 0;
  m_evtNr = -1;

  m_sumPed2 = new double[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_sumRms2 = new double[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_meanAmp = new double[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_meanAmp_ij = new double[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_evt = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_ros = new uint8_t[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_drawer = new uint8_t[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_channel = new uint8_t[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_gain = new bool[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_ped = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_lfn = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_hfn = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_noise_cov = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_GAIN]();
  m_auto_corr = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NVALS]();
}

TileDigiNoiseCalibAlg::~TileDigiNoiseCalibAlg() {

  delete[] m_sumPed2;
  delete[] m_sumRms2;
  delete[] m_meanAmp;
  delete[] m_meanAmp_ij;
  delete[] m_evt;
  delete[] m_ros;
  delete[] m_drawer;
  delete[] m_channel;
  delete[] m_gain;
  delete[] m_ped;
  delete[] m_lfn;
  delete[] m_hfn;
  delete[] m_noise_cov;
  delete[] m_auto_corr;
}

/// Only array initialization is done here
/// All the helpers initialization is done at the first event
StatusCode TileDigiNoiseCalibAlg::initialize() {

  CHECK( m_dqStatusKey.initialize() );

  if (!m_eventInfoKey.key().empty()) {
    ATH_CHECK( m_eventInfoKey.initialize() );
  }
  // TileDigitsContainer initialization
  ATH_CHECK( m_digitsContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

/// Initialization done at the first event.
/// All the helpers and data dependent variables are initialized here
StatusCode TileDigiNoiseCalibAlg::FirstEvt_initialize() {

  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );

  CHECK( detStore()->retrieve(m_tileHWID) );

  CHECK( m_adderFilterAlgTool.retrieve() );

  m_adderFilterAlgTool->setProperty("TileRawChannelContainer", "TileAdderFlat");
  m_adderFilterAlgTool->setProperty("calibrateEnergy", "true");
  m_adderFilterAlgTool->setProperty("PedStart", "0");
  m_adderFilterAlgTool->setProperty("PedLength", "1");
  m_adderFilterAlgTool->setProperty("PedOffset", "0");
  m_adderFilterAlgTool->setProperty("SignalStart", "1");
  m_adderFilterAlgTool->setProperty("SignalLength", "15");
  m_adderFilterAlgTool->setProperty("FilterLength", "5");
  m_adderFilterAlgTool->setProperty("FrameLength", "16");
  m_adderFilterAlgTool->setProperty("DeltaCutLo", "9.5");
  m_adderFilterAlgTool->setProperty("DeltaCutHi", "9.5");
  m_adderFilterAlgTool->setProperty("RMSCutLo", "1.0");
  m_adderFilterAlgTool->setProperty("RMSCutHi", "1.0");

  m_tileOFCorrelation = new TileOFCorrelation();
  m_tileOFCorrelation->SetCorrelationZero(msg(), m_nSamples);

  ATH_MSG_INFO( "calibMode " << m_calibMode );
  ATH_MSG_INFO( "number of samples " << m_nSamples );

  // set event number to 0 before first event
  m_evtNr = 0;

  if (m_evtNr == 0) {
    if (m_beamElemContainer.length() > 0) {
      ServiceHandle<IConversionSvc> cnvSvc("ByteStreamCnvSvc", "");
      if (cnvSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR( " Can't get ByteStreamCnvSvc " );
        m_beamCnv = NULL;
      } else {
        m_beamCnv = dynamic_cast<TileBeamElemContByteStreamCnv *>(cnvSvc->converter(ClassID_traits<TileBeamElemContainer>::ID()));
        if (m_beamCnv == NULL) {
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
StatusCode TileDigiNoiseCalibAlg::execute() {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus * dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  StatusCode sc;
  bool empty(false);

  if (m_evtNr < 0) {

    if (FirstEvt_initialize().isFailure()) {
      ATH_MSG_ERROR( "FirstEvt_initialize failed" );
    }

    bool calibMode = (dqStatus->calibMode() == 1);
    if (calibMode != m_calibMode) {
      ATH_MSG_INFO( "Calib mode from data is " );
      ATH_MSG_INFO( "  Overwriting calib mode " );
      m_calibMode = calibMode;
    }

    m_cispar = dqStatus->cispar();
    StoreRunInfo(dqStatus); // done only once
  }

  m_cispar = dqStatus->cispar();
  if (m_evtNr % 1000 == 0)
    ATH_MSG_WARNING( m_evtNr << " events processed so far" );

  // store TileDigits
  if (m_nSamples > 0) sc = fillDigits (dqStatus);
  empty &= (sc.isFailure());

  if (empty) {
    ATH_MSG_WARNING( "Error in execute "  );
  }
  ++m_evtNr;
  return StatusCode::SUCCESS;
}

///The output ntuple is created in finalize method
StatusCode TileDigiNoiseCalibAlg::finalize() {

  ATH_MSG_INFO( "Finalizing TileDigiNoiseCalibAlg" );

  finalDigits();

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
  sStr << m_file << "_" << m_run << "_" << trig_str << ".root";
  m_file = sStr.str();
  ATH_MSG_INFO( "Writing calibrations to file " << m_file );

  // Create output file: for now creating file for just this
  // algorithm; want to add to ntuple file eventually??
  TFile* fout = new TFile(m_file.c_str(), "recreate");

  // Create tree with branches
  TTree* t = new TTree(m_ntupleID.c_str(), "TileCalib-Ntuple");

  t->Branch("RunNumber", &m_run, "RunNumber/I");
  t->Branch("TrigType", &m_trigType, "TrigType/I");
  t->Branch("Time", &m_time, "Time/I");
  t->Branch("Year", &m_year, "Year/I");
  t->Branch("Month", &m_month, "Month/I");
  t->Branch("Day", &m_day, "Day/I");
  t->Branch("YDay", &m_yday, "YDay/I");
  t->Branch("Hour", &m_hour, "Hour/I");
  t->Branch("Min", &m_min, "Min/I");
  t->Branch("nSamples", &m_nSamples, "nSamples/I");
  t->Branch("nEvt", &m_evtNr, "nEvt/I"); // events processed
  t->Branch("ros", *m_ros, "ros[5][64][48][2]/b");
  t->Branch("drawer", *m_drawer, "drawer[5][64][48][2]/b");
  t->Branch("channel", *m_channel, "channel[5][64][48][2]/b");
  t->Branch("gain", *m_gain, "gain[5][64][48][2]/O");
  t->Branch("EvtGood", *m_evt, "Evt[5][64][48][2]/I"); // events used in the noise calculation for every channel
  t->Branch("ped", *m_ped, "ped[5][64][48][2]/F");
  t->Branch("lfn", *m_lfn, "lfn[5][64][48][2]/F");
  t->Branch("hfn", *m_hfn, "hfn[5][64][48][2]/F");
  t->Branch("noise_cov", *m_noise_cov, "noise_cov[5][64][2]/F");
  // AutoCorrelation Matrix: Store only non-diagonal half of symmetric matrix
  t->Branch("auto_corr", *m_auto_corr, "auto_corr[5][64][48][2][36]/F");

  // Fill with current values (i.e. tree will have only one entry for this whole run)

  t->Fill();
  t->Write();

  fout->Close();

  return StatusCode::SUCCESS;
}

/// StoreRunInfo is called only during the first event
void TileDigiNoiseCalibAlg::StoreRunInfo (const TileDQstatus* dqStatus) {

  if (dqStatus->calibMode() == 1 && m_beamElemContainer.length() > 0) {// Bigain can use cispar
    if (m_beamCnv) {
      //    std::cout << "LUCA m_time= "<< m_time << "   bc_time_seconds= "<<  m_beamCnv->eventFragment()->bc_time_seconds() <<
      //  "   bc_time_nanoseconds= " << m_beamCnv->eventFragment()->bc_time_nanoseconds() << std::endl;
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
  } else {// monogain can use eventinfo

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

/// fillDigits is called at every events.
/// Statistics is summed for Average, RMS and covariance calculations
/*---------------------------------------------------------*/
StatusCode TileDigiNoiseCalibAlg::fillDigits (const TileDQstatus* theDQstatus) {
/*---------------------------------------------------------*/

  SG::ReadHandle<TileDigitsContainer> digitsCnt(m_digitsContainerKey);
  ATH_CHECK( digitsCnt.isValid() );

  TileDigitsContainer::const_iterator collItr = digitsCnt->begin();
  TileDigitsContainer::const_iterator lastColl = digitsCnt->end();

  for (; collItr != lastColl; ++collItr) {

    TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

    if (digitsItr != lastDigits) { //loop over all the drawers in the data

      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      // IMPORTANT! ros for TileCal partitions goes like LBA=1  LBC=2  EBA=3  EBC=4

      int drawer = m_tileHWID->drawer(adc_id);
      // IMPORTANT! Drawers are from 0 to 63!

      double mean_tmp[48][16][2];
      memset(mean_tmp, 0, sizeof(mean_tmp));

      for (; digitsItr != lastDigits; ++digitsItr) { // loop over all channels in the drawer

        adc_id = (*digitsItr)->adc_HWID();
        int chan = m_tileHWID->channel(adc_id);
        if (m_usePMT) {
          chan = digiChannel2PMT(ros, chan);   // IMPORTANT: 0-47 PMT format!
          // by default we use CHANNELS.
          // PMTs usage is possible by switching on m_usePMT
        }
        int gain = m_tileHWID->adc(adc_id);

        std::vector<float> vdigits = (*digitsItr)->samples();

        // Needed to calculate the AutoCorrealtion matrix at end of run
        //m_tileOFCorrelation->RunningCorrelation(vdigits,ros-1,drawer,chan,gain,msg(),false,m_nSamples,100);

        // if (theDQstatus->isChEmpty(ros, drawer, chan)) {
        //   ATH_MSG_DEBUG( "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
        //                 << " channel: " << chan
        //                 << " ADC: " << gain
        //                 << " because empty" );
        //   continue;
        // }

        // If DQ problem, do not fill calib ntuple
        if (m_calibMode == 1) {// Bigain: check indivual adc's
           if (!(theDQstatus->isAdcDQgood(ros, drawer, chan, gain))) {
             ATH_MSG_DEBUG( "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
                           << " channel: " << chan
                           << " ADC: " << gain
                           << " due to DQ error found." );
            continue;
          }
        } else {// monogain, just check channel

          if (!(theDQstatus->isChanDQgood(ros, drawer, chan))) {
            ATH_MSG_DEBUG( "Skipping Module: " << TileCalibUtils::getDrawerString(ros, drawer)
                          << " channel: " << chan
                          << " due to DQ error found." );
            continue;
          }
        }

        double meansamp = 0.0;
        double rmssamp = 0.0;
        unsigned int dsize = vdigits.size();

        if (dsize > 16) {
          ATH_MSG_ERROR( "length of digits vector " << dsize << " - greater than 16 !" );
          dsize = 16;
        }

        for (unsigned int i = 0; i < dsize; ++i) {
          double dig = vdigits[i];
          meansamp += dig;
          rmssamp += dig * dig;
          mean_tmp[chan][i][gain] = dig;
        }
        if (dsize > 0) {
          m_ped[ros][drawer][chan][gain] += vdigits[0];
          m_sumPed2[ros][drawer][chan][gain] += vdigits[0] * vdigits[0];

          if (dsize > 1) {
            m_evt[ros][drawer][chan][gain]++;
            meansamp /= dsize;
            rmssamp = rmssamp / dsize - meansamp * meansamp;
            rmssamp = (rmssamp > 0.0) ? sqrt(rmssamp * dsize / (dsize - 1)) : 0.0;
            m_hfn[ros][drawer][chan][gain] += rmssamp;
            m_sumRms2[ros][drawer][chan][gain] += rmssamp * rmssamp;
          }
        }

        // Fill event-by-event correlation values
        if (m_doAvgCorr)
          m_tileOFCorrelation->RunningCorrelation(vdigits, ros - 1, drawer, chan, gain, msg(), false, m_nSamples, 100);
        else
          m_tileOFCorrelation->Sum(vdigits, ros - 1, drawer, chan, gain, msg(), false, m_nSamples, m_doRobustCov);

      } // loop over channels

      //For cor&cov
      for (int sample = 0; sample < m_nSamples; ++sample) {
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
          for (unsigned int chan_i = 0; chan_i < TileCalibUtils::MAX_CHAN; ++chan_i) {
            m_meanAmp[ros][drawer][chan_i][gain] += mean_tmp[chan_i][sample][gain];
            for (unsigned int chan_j = 0; chan_j < TileCalibUtils::MAX_CHAN; ++chan_j)
              m_meanAmp_ij[ros][drawer][chan_i][chan_j][gain] += mean_tmp[chan_i][sample][gain] * mean_tmp[chan_j][sample][gain];
          }
        }
      }
    } //check if Digits in module
  } //loop over drawers

  return StatusCode::SUCCESS;
}

/// finalDigits is called during finalize
/// Here the average Ped, m_lfn, m_hfn and covariance are calculated.
/*---------------------------------------------------------*/
void TileDigiNoiseCalibAlg::finalDigits() {
/*---------------------------------------------------------*/


  // Calculate AutoCorrealtion matrix
  // --Average calculation (Assumes matrix is Toeplitz)
  if (m_doAvgCorr)
    m_tileOFCorrelation->CalcRunningCorrelation(msg(), m_nSamples, 100, false);
  // --Rigorous calculation
  else
    m_tileOFCorrelation->CalcCorrelation(msg(), m_nSamples, false, m_doRobustCov);

  // Needed to store autoCorrelation matrix
  float tmpCorr[9][9];
  memset(tmpCorr, 0, sizeof(tmpCorr));

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
        for (unsigned int chan = 0; chan < TileCalibUtils::MAX_CHAN; ++chan) {

          m_ros[ros][drawer][chan][gain] = ros;
          m_drawer[ros][drawer][chan][gain] = drawer;
          m_channel[ros][drawer][chan][gain] = chan;
          m_gain[ros][drawer][chan][gain] = gain;

          if (m_evt[ros][drawer][chan][gain] > 0) {
            int nev = m_evt[ros][drawer][chan][gain];
            m_ped[ros][drawer][chan][gain] /= nev;
            double Ped = m_ped[ros][drawer][chan][gain];
            m_hfn[ros][drawer][chan][gain] /= nev;

            if (nev > 1) {
              double PedRMS = m_sumPed2[ros][drawer][chan][gain] / nev - Ped * Ped;
              PedRMS = m_sumPed2[ros][drawer][chan][gain] / nev - Ped * Ped;
              PedRMS = (PedRMS > 0.0) ? sqrt(PedRMS * nev / (nev - 1)) : 0.0;
              m_lfn[ros][drawer][chan][gain] = PedRMS;
            }
          }

          // Get values from AutoCorrelation matrix
          // matrix is symmetric, so only half-retrieved and stored
          m_tileOFCorrelation->GetCorrelation(m_nSamples, tmpCorr, ros - 1, drawer, chan, gain);
          //std::cout << "Printing AutoCorr values: " << std::endl;
          int nVals = 0;
 	  if (m_doRobustCov){ //save 28 elements for robust method
            for (int i = 0; i < m_nSamples; i++) {
              for (int j = i; j < m_nSamples; j++) {
                //std::cout << "Auto Corr [" << i << "][" << j << "]:" << tmpCorr[i][j] << std::endl;
                //std::cout << "Auto CorrSym [" << j << "][" << i << "]:" << tmpCorr[j][i] << std::endl;
                m_auto_corr[ros][drawer][chan][gain][nVals] = tmpCorr[i][j];
                nVals++;
              }
            }
	  } else {
            for (int i = 0; i < m_nSamples; i++) {
              for (int j = i+1; j < m_nSamples; j++) {
                //std::cout << "Auto Corr [" << i << "][" << j << "]:" << tmpCorr[i][j] << std::endl;
                //std::cout << "Auto CorrSym [" << j << "][" << i << "]:" << tmpCorr[j][i] << std::endl;
                m_auto_corr[ros][drawer][chan][gain][nVals] = tmpCorr[i][j];
                nVals++;
              }
            }
	  }

        } // end chan loop

        //Calculate covariance loop over chan_i, chan_j
        if (m_evtNr * m_nSamples > 0) {
          //replace m_evtNr with sqrt(m_evt[ch_i]*m_evt[ch_j])

          for (unsigned int chan_i = 0; chan_i < TileCalibUtils::MAX_CHAN; ++chan_i) {
            m_meanAmp[ros][drawer][chan_i][gain] /= m_evtNr * m_nSamples;
            for (unsigned int chan_j = 0; chan_j < TileCalibUtils::MAX_CHAN; ++chan_j)
              m_meanAmp_ij[ros][drawer][chan_i][chan_j][gain] /= m_evtNr * m_nSamples;
          }

          double covar[48][48];
          double mean_cov_ii = 0.; // mean values of covar in diag terms
          double mean_cov_ij = 0.;  // mean values of covar in off diag terms

          for (unsigned int chan_i = 0; chan_i < TileCalibUtils::MAX_CHAN; ++chan_i) {
            for (unsigned int chan_j = 0; chan_j < TileCalibUtils::MAX_CHAN; ++chan_j) {
              covar[chan_i][chan_j] = m_meanAmp_ij[ros][drawer][chan_i][chan_j][gain] - m_meanAmp[ros][drawer][chan_i][gain] * m_meanAmp[ros][drawer][chan_j][gain];

              if (chan_j < chan_i) {
                mean_cov_ij += covar[chan_i][chan_j]; //LF: we take C_ij with its sign
              }
            }
            mean_cov_ii += covar[chan_i][chan_i];
          }

          if (mean_cov_ii != 0.) {
            m_noise_cov[ros][drawer][gain] = (2. * mean_cov_ij) / (mean_cov_ii * 47.); //(2*cov_ij/(48*47))/(cov_ii/48)

          } else {
            m_noise_cov[ros][drawer][gain] = 0.;
          }

        }
      }
    }
  }
}
