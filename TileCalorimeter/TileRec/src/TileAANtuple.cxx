/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///*****************************************************************************
//  Filename : TileAANtuple.cxx
//  Author   : Alexander Solodkov
//  Created  : April, 2010
//
//  DESCRIPTION:
//     Algorithm which puts in ntuple all raw data (digits) and results of Opt filter
//
//  HISTORY:
//     29-Apr-2010 First version - simplified version of TileTBAANtuple
//
//  BUGS:
//
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/ITHistSvc.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"
#include "xAODEventInfo/EventInfo.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Calo includes
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/TileID.h"

//Tile includes
#include "TileRec/TileAANtuple.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileDCSSvc.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileEvent/TileMuonReceiverContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileL2Algs/TileL2Builder.h"

#include "TTree.h"
#include <iomanip>
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>
#include <sstream>

#define CLEAR(array)				\
memset(array,0,sizeof(array))

#define CLEAR1(array)				\
memset(array,-1,sizeof(array))

#define CLEAR2(array,size)			\
memset(array,0,sizeof(array)/size)

#define CLEAR3(array,size)			\
memset(array,-1,sizeof(array)/size)

#define NAME1(s1)				\
s1.c_str()

#define NAME2(s1,s2)				\
(s1+s2).c_str()

#define NAME3(s1,s2,s3)				\
(s1+s2+s3).c_str()

// Constructor & deconstructor
/** @class TileAANtuple
 *  @brief class to produce TileCal commissioning ntuples
 */


TileAANtuple::TileAANtuple(std::string name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name, pSvcLocator)
, m_evTime(0)
, m_run(0)
, m_evt(0)
, m_lumiBlock(0)
, m_HHMMSS(0)
, m_dateTime()
, m_trigType(0)
, m_dspFlags(0)
, m_l1ID()
, m_l1Type()
, m_evBCID()
, m_evType()
, m_cispar()
, m_las_version(0)
, m_las_BCID(0)
, m_las_Filt(0)
, m_las_ReqAmp(0)
, m_las_MeasAmp(0)
, m_las_Temperature(0)
, m_qdctimeout(0)
, m_tdctimeout(0)
, m_daqtype(0)
, m_sampleFlt()
, m_gainFlt()
, m_sample()
, m_gain()
, m_rodBCID()
, m_fragSize()
, m_dmuMask()
, m_slinkCRC()
, m_DMUheader()
, m_DMUbcid()
, m_DMUformatErr()
, m_DMUparityErr()
, m_DMUmemoryErr()
, m_DMUDstrobeErr()
, m_DMUSstrobeErr()
, m_feCRC()
, m_rodCRC()
, m_ene()
, m_time()
, m_ped()
, m_chi2()
, m_eFit()
, m_tFit()
, m_pedFit()
, m_chi2Fit()
, m_eFitc()
, m_tFitc()
, m_pedFitc()
, m_chi2Fitc()
, m_eOpt()
, m_tOpt()
, m_pedOpt()
, m_chi2Opt()
, m_eOF1()
, m_tOF1()
, m_pedOF1()
, m_chi2OF1()
, m_eDsp()
, m_tDsp()
, m_pedDsp()
, m_chi2Dsp()
, m_eMF()
, m_tMF()
, m_chi2MF()
, m_pedMF()
, m_ROD_GlobalCRC()
, m_ROD_BCID()
, m_ROD_DMUBCIDErr()
, m_ROD_DMUmemoryErr()
, m_ROD_DMUSstrobeErr()
, m_ROD_DMUDstrobeErr()
, m_ROD_DMUHeadformatErr()
, m_ROD_DMUHeadparityErr()
, m_ROD_DMUDataformatErr()
, m_ROD_DMUDataparityErr()
, m_ROD_DMUfeCRC()
, m_ROD_DMUrodCRC()
, m_ROD_DMUMask()
, m_eTMDB()
, m_sampleTMDB()
, m_decisionTMDB()
, m_TEMP()
, m_HV()
, m_HVSET()
, m_DRSTATES()
, m_HVSTATUS()
, m_DRSTATUS()
, m_CHSTATUS()
, m_nBadDr(0)
, m_nBadHV(0)
, m_nBadDCS(0)
, m_nBadDB(0)
, m_nBadTotal(0)
, m_rchUnit(TileRawChannelUnit::MegaElectronVolts)
, m_dspUnit(TileRawChannelUnit::ADCcounts)
, m_ntuplePtr(0)
, m_DCSntuplePtr(0)
, m_thistSvc("THistSvc", name)
, m_tileID(0)
, m_tileHWID(0)
, m_cabling(0)
, m_tileMgr(0)
, m_tileBadChanTool("TileBadChanTool")
, m_tileToolEmscale("TileCondToolEmscale")
, m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
, m_beamCnv(0)
, m_tileDCSSvc("TileDCSSvc",name)
, m_l2Builder()
, m_sumEt_xx()
, m_sumEz_xx()
, m_sumE_xx()
, m_sumEt_yy()
, m_sumEz_yy()
, m_sumE_yy()
, m_sumEt_zz()
, m_sumEz_zz()
, m_sumE_zz()
, m_bad()
{
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileDCSSvc", m_tileDCSSvc);
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");
  declareProperty("TileDigitsContainerFlt", m_fltDigitsContainer = "" /* "TileDigitsFlt" */);
  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");
  declareProperty("TileRawChannelContainer", m_rawChannelContainer = "TileRawChannelCnt");
  declareProperty("TileRawChannelContainerFit", m_fitRawChannelContainer = "");      //
  declareProperty("TileRawChannelContainerFitCool", m_fitcRawChannelContainer = ""); // don't create
  declareProperty("TileRawChannelContainerOpt", m_optRawChannelContainer = "");      // by default
  declareProperty("TileRawChannelContainerQIE", m_qieRawChannelContainer = "");      // processed QIE data
  declareProperty("TileRawChannelContainerOF1", m_of1RawChannelContainer = "");      //
  declareProperty("TileRawChannelContainerDsp", m_dspRawChannelContainer = "");      //
  declareProperty("TileRawChannelContainerMF", m_mfRawChannelContainer = "");      //
  declareProperty("TileMuRcvRawChannelContainer", m_tileMuRcvRawChannelContainer = "MuRcvRawChCnt");// TMDB
  declareProperty("TileMuRcvDigitsContainer", m_tileMuRcvDigitsContainer = "MuRcvDigitsCnt");// TMDB
  declareProperty("TileMuRcvContainer", m_tileMuRcvContainer = "TileMuRcvCnt");// TMDB
  declareProperty("TileLaserObject", m_laserObject = "" /* "TileLaserObj" */);       //
  declareProperty("CalibrateEnergy", m_calibrateEnergy = true);
  declareProperty("UseDspUnits", m_useDspUnits = false);
  declareProperty("OfflineUnits", m_finalUnit = TileRawChannelUnit::MegaElectronVolts);
  declareProperty("CalibMode", m_calibMode = false);
  declareProperty("CompareMode", m_compareMode = false);
  declareProperty("BSInput", m_bsInput = true);
  declareProperty("PMTOrder", m_pmtOrder = false);
  
  declareProperty("StreamName", m_streamName = "AANT");
  declareProperty("NTupleID", m_ntupleID = "h2000");
  declareProperty("TreeSize", m_treeSize = 16000000000LL);
  
  declareProperty("CheckDCS",m_checkDCS = false);
  declareProperty("DCSBranches",m_DCSBranches = 111111111);

  declareProperty("SkipEvents", m_skipEvents = 0);
  
  m_evtNr = -1;
  m_DCScounter = 0;
  
  // LASERII
  memset(m_chan, 0, sizeof(m_chan));
  memset(m_chan_Ped, 0, sizeof(m_chan_Ped));
  memset(m_chan_Led, 0, sizeof(m_chan_Led));
  memset(m_chan_Lin, 0, sizeof(m_chan_Lin));
  memset(m_chan_Alpha, 0, sizeof(m_chan_Alpha));
  memset(m_chan_SPed, 0, sizeof(m_chan_SPed));
  memset(m_chan_SLed, 0, sizeof(m_chan_SLed));
  memset(m_chan_SLin, 0, sizeof(m_chan_SLin));
  memset(m_chan_SAlpha, 0, sizeof(m_chan_SAlpha));
  
  // LASERI
  memset(m_las_D_ADC,          0, sizeof(m_las_D_ADC));
  memset(m_las_D_Ped,          0, sizeof(m_las_D_Ped));
  memset(m_las_D_Ped_RMS,      0, sizeof(m_las_D_Ped_RMS));
  memset(m_las_D_Alpha,        0, sizeof(m_las_D_Alpha));
  memset(m_las_D_Alpha_RMS,    0, sizeof(m_las_D_Alpha_RMS));
  memset(m_las_D_AlphaPed,     0, sizeof(m_las_D_AlphaPed));
  memset(m_las_D_AlphaPed_RMS, 0, sizeof(m_las_D_AlphaPed_RMS));
  
  memset(m_las_PMT_ADC,        0, sizeof(m_las_PMT_ADC));
  memset(m_las_PMT_TDC,        0, sizeof(m_las_PMT_TDC));
  memset(m_las_PMT_Ped,        0, sizeof(m_las_PMT_Ped));
  memset(m_las_PMT_Ped_RMS,    0, sizeof(m_las_PMT_Ped_RMS));

  // TMDB
  memset(m_eTMDB,0,sizeof(m_eTMDB));
  memset(m_sampleTMDB,0,sizeof(m_sampleTMDB));
  memset(m_decisionTMDB,0,sizeof(m_decisionTMDB));
}

TileAANtuple::~TileAANtuple() {
}

/// Alg standard interface function
StatusCode TileAANtuple::initialize() {
  ATH_MSG_INFO( "Initialization started");
  
  const IGeoModelSvc *geoModel = 0;
  CHECK( service("GeoModelSvc", geoModel));
  
  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;
  
  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  } else {
    CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel, &TileAANtuple::geoInit, this) );
  }
  
  return StatusCode::SUCCESS;
}

/// callback for delayed initialization
StatusCode TileAANtuple::geoInit(IOVSVC_CALLBACK_ARGS) {
  
  ATH_MSG_INFO( "Initialization in geoInit callback started" );
  
  // find TileCablingService
  m_cabling = TileCablingService::getInstance();
  
  // retrieve TileDetDescr Manager det store
  CHECK( detStore()->retrieve(m_tileMgr) );
  
  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  
  //=== get TileDCSSvc
  if (m_checkDCS) {
    CHECK( m_tileDCSSvc.retrieve() );
  }
  
  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );
  
  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );
  
  //=== get TileBeamInfo
  CHECK( m_beamInfo.retrieve() );
  
  //=== get TileL2Builder
  if (m_compareMode) {
    CHECK( m_l2Builder.retrieve() );
  }
  
  //=== change properties of TileBeamInfo - to make sure that all parameters are consistent
  if (m_beamElemContainer.size() > 0) {
    CHECK( m_beamInfo->setProperty("TileBeamElemContainer",m_beamElemContainer) );
  }
  
  if (m_digitsContainer.size() > 0) {
    CHECK( m_beamInfo->setProperty("TileDigitsContainer",m_digitsContainer) );
  }
  
  if (m_dspRawChannelContainer.size() > 0) {
    CHECK( m_beamInfo->setProperty("TileRawChannelContainer",m_dspRawChannelContainer) );
  }
  
  ATH_MSG_INFO( "initialization completed" ) ;
  return StatusCode::SUCCESS;
}


StatusCode TileAANtuple::ntuple_initialize() {
  
  if (m_bsInput) {
    ServiceHandle<IConversionSvc> cnvSvc("ByteStreamCnvSvc", "");
    if (cnvSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR( " Can't get ByteStreamCnvSvc " );
      m_beamCnv = NULL;
      
    } else {
      
      m_beamCnv =
      dynamic_cast<TileBeamElemContByteStreamCnv *>(cnvSvc->converter(
                                                                      ClassID_traits<TileBeamElemContainer>::ID()));
      
      if (m_beamCnv == NULL) {
        ATH_MSG_ERROR( " Can't get TileBeamElemContByteStreamCnv " );
      }
    }
  } else {
    m_beamCnv = NULL;
  }
  
  uint32_t calib = m_beamInfo->calibMode();
  bool calibMode  = (calib == 1);
  if ( calibMode != m_calibMode && calib!=0xFFFFFFFF ) {
    ATH_MSG_INFO( "Calib mode from data is " << calibMode );
    ATH_MSG_INFO( "  Overwriting calib mode " );
    m_calibMode = calibMode;
  }
  
  if (m_finalUnit < TileRawChannelUnit::ADCcounts
      || m_finalUnit > TileRawChannelUnit::OnlineMegaElectronVolts) {
    
    m_finalUnit = -1;
    if ( !m_useDspUnits && m_calibrateEnergy ) {
      m_useDspUnits = true;
      ATH_MSG_INFO( "Final offline units are not set, will use DSP units" );
    }
  }
  
  if ( !m_calibrateEnergy && m_useDspUnits) {
    ATH_MSG_INFO( "calibrateEnergy is disabled, don't want to use DSP units" );
    m_useDspUnits = false;
  }
  
  ATH_MSG_INFO( "calibMode " << m_calibMode );
  ATH_MSG_INFO( "calibrateEnergy " << m_calibrateEnergy );
  ATH_MSG_INFO( "offlineUnits " << m_finalUnit );
  ATH_MSG_INFO( "useDspUnits " << m_useDspUnits );
  
  // set event number to 0 before first event
  m_evtNr = 0;
  
  CHECK( m_thistSvc.retrieve() );
  
  if(initNTuple().isFailure()) {
    ATH_MSG_ERROR( " Error during ntuple initialization" );
  }
  
  ATH_MSG_INFO( "ntuple initialization completed" );
  return StatusCode::SUCCESS;
}


StatusCode TileAANtuple::execute() {
  
  if (m_evtNr < 0) {
    if (ntuple_initialize().isFailure()) {
      ATH_MSG_ERROR( "ntuple_initialize failed" );
    }
  }
  
  if (m_evtNr%1000 == 0) {
    ATH_MSG_INFO( m_evtNr << " events processed so far" );
  }
  
  if (ntuple_clear().isFailure()) {
    ATH_MSG_ERROR( "ntuple_clear failed" );
  }
  
  bool empty = true;
  
  // store BeamElements
  if (m_beamElemContainer.size() > 0) {
    empty &= storeBeamElements().isFailure();
  }
  
  //store Laser Object
  if (m_laserObject.size() > 0) {
    empty &= storeLaser().isFailure();
  }
  
  // store TileDigits
  empty &= storeDigits(m_fltDigitsContainer,m_sampleFlt,m_gainFlt,false).isFailure();
  empty &= storeDigits(m_digitsContainer,   m_sample,   m_gain,   true ).isFailure();
  
  // store TileRawChannels
  // start from DSP channels - so we can find out what is the DSP units
  empty &= storeRawChannels(m_dspRawChannelContainer,  m_eDsp,  m_tDsp,  m_chi2Dsp, m_pedDsp, true ).isFailure();
  empty &= storeRawChannels(m_rawChannelContainer,     m_ene,   m_time,  m_chi2,    m_ped,    false).isFailure();
  empty &= storeMFRawChannels(m_mfRawChannelContainer, m_eMF,   m_tMF,   m_chi2MF,  m_pedMF,  false).isFailure();
  empty &= storeRawChannels(m_fitRawChannelContainer,  m_eFit,  m_tFit,  m_chi2Fit, m_pedFit, false).isFailure();
  empty &= storeRawChannels(m_fitcRawChannelContainer, m_eFitc, m_tFitc, m_chi2Fitc,m_pedFitc,false).isFailure();
  empty &= storeRawChannels(m_optRawChannelContainer,  m_eOpt,  m_tOpt,  m_chi2Opt, m_pedOpt, false).isFailure();
  empty &= storeRawChannels(m_qieRawChannelContainer,  m_eQIE,  m_tQIE,  m_chi2QIE, m_pedQIE, false).isFailure();
  empty &= storeRawChannels(m_of1RawChannelContainer,  m_eOF1,  m_tOF1,  m_chi2OF1, m_pedOF1, false).isFailure();
  
  // store TMDB data
  //
  empty &= storeTMDBDecision().isFailure();
  empty &= storeTMDBDigits().isFailure();
  empty &= storeTMDBRawChannel().isFailure();

  if (m_beamCnv) {
    m_evTime = m_beamCnv->eventFragment()->bc_time_seconds();
    m_evt = m_beamCnv->eventFragment()->global_id();
    if ( m_beamCnv->validBeamFrag() )
      m_run = m_beamCnv->robFragment()->rod_run_no();   // take it from beam ROD header
    else
      m_run = m_beamCnv->eventFragment()->run_no();  // sometimes run_no is 0 here
  } else {
    m_evTime = 0;
    m_evt = m_evtNr;
    m_run = 0;
  }
  m_lumiBlock = -1; // placeholder
  
  // new way to set run/event/lumi block
  // Retrieve eventInfo from StoreGate
  const xAOD::EventInfo* eventInfo(0);
  if (evtStore()->retrieve(eventInfo).isSuccess()){
    
    //Get run and event numbers
    m_run = eventInfo->runNumber();
    m_evt = eventInfo->eventNumber();
    
    if ( eventInfo->lumiBlock() ){
      m_lumiBlock = eventInfo->lumiBlock();
    }
    
    //Get timestamp of the event
    if (eventInfo->timeStamp() > 0) {
      m_evTime = eventInfo->timeStamp();
    }
  }
  
  if (m_evTime>0) {
    using namespace boost::local_time;
    using namespace boost::posix_time;
    
    /*
     // just an example how to read file with time zones
     tz_database tz_db;
     try {
     tz_db.load_from_file("../data/date_time_zonespec.csv");
     time_zone_ptr gva_tz = tz_db.time_zone_from_region("Europe/Zurich");
     }catch(data_not_accessible dna) {
     std::cerr << "Error with time zone data file: " << dna.what() << std::endl;
     //exit(EXIT_FAILURE);
     }catch(bad_field_count bfc) {
     std::cerr << "Error with time zone data file: " << bfc.what() << std::endl;
     //exit(EXIT_FAILURE);
     }
     */
    //"Europe/Zurich","CET","CET","CEST","CEST","+01:00:00","+01:00:00","-1;0;3","+02:00:00","-1;0;10","+03:00:00"
    static time_zone_ptr gva_tz(new posix_time_zone((std::string)"CET+01CEST01:00:00,M3.5.0/02:00:00,M10.5.0/03:00:00"));
    local_date_time gva_time(from_time_t(m_evTime),gva_tz);
    
    //std::ostringstream otime;
    //otime << gva_time; // time in the format YYYY-MMM-DD HH:MM:SS TZ
    //strncpy(m_dateTime,otime.str().c_str(),31);
    
    //time_duration hms(gva_time.time_of_day()); - will give time of the day in GMT
    //m_HHMMSS = hms.hours()*10000+hms.minutes()*100+hms.seconds();
    
    struct tm gva_tm(to_tm(gva_time));
    strftime(m_dateTime, 32, "%Y-%b-%d %H:%M:%S %Z", &gva_tm);
    m_HHMMSS = gva_tm.tm_hour*10000+gva_tm.tm_min*100+gva_tm.tm_sec;
    
    // the code below is only valid when running at CERN (in Geneva time zone)
    //struct tm *time = localtime((time_t*)(&m_evTime));
    //m_HHMMSS = time->tm_hour*10000+time->tm_min*100+time->tm_sec;
    //strftime(m_dateTime, 32, "%Y-%m-%d %H:%M:%S %Z", time);
    
  } else {
    m_HHMMSS = -1;
    m_dateTime[0] = '\0'; // empty string
  }
  
  // store DCS data
  if (m_checkDCS) {
    empty &= storeDCS().isFailure();
  }
 
  if (empty) {
    ATH_MSG_WARNING( "Some problems in execute - ntuple was not filled at all" );
  }
  
  // FIRST 4 EVENTS ARE SKIPPED TO RETRIEVE LASER PEDESTALS
  if (m_ntuplePtr && m_evtNr >= m_skipEvents){
    m_ntuplePtr->Fill();
  } // IF
  
  ++m_evtNr;
  
  // Execution completed.
  
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}


//
// Here the LASER object is opened and corresponding variable are stored
//
StatusCode TileAANtuple::storeLaser() {
  
  ATH_MSG_DEBUG("TileAANtuple::storeLaser()");
  const char* gainnames[2]  = {"LG","HG"};

  const TileLaserObject* laserObj;
  CHECK( evtStore()->retrieve(laserObj, m_laserObject) );
  
  m_las_BCID = laserObj->getBCID();
  
  m_las_Filt = laserObj->getFiltNumber();
  m_las_ReqAmp = laserObj->getDiodeCurrOrd();
  m_las_MeasAmp = laserObj->getDiodeCurrMeas();
  m_las_Temperature = laserObj->getPumpDiodeTemp();
  ATH_MSG_VERBOSE( "Laser BCID " << m_las_BCID
                   << " Filt " << m_las_Filt
                   << " ReqAmp " << m_las_ReqAmp
                   << " MeasAmp " << m_las_MeasAmp
                   << " Temp " << m_las_Temperature);
  
  ATH_MSG_DEBUG("LASER"<<(laserObj->isLASERII()?"II":"I")<<" VERSION IS " << laserObj->getVersion());
  
  if(laserObj->isLASERII()){
    m_qdctimeout = laserObj->getQDCTimeout();
    m_tdctimeout = laserObj->getTDCTimeout();
    m_daqtype = laserObj->getDaqType();
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "DAQ Type    " << m_daqtype << endreq;
      msg(MSG::DEBUG) << "QDC TimeOut " << m_qdctimeout << endreq;
      msg(MSG::DEBUG) << "TDC TimeOut " << m_tdctimeout << endreq;
    }

    // RETRIEVE SIGNAL IN ADC COUNTS
    for(int chan=0;chan<28;++chan){
      int ch=chan>>1;
      int gn=chan&1;
      // MONITORING DIODES
      m_chan[chan] = laserObj->getDiodeADC(ch,gn);
      ATH_MSG_DEBUG("LASERII CHANNEL " << ch << " ("<<gainnames[gn]<<") " << m_chan[chan]);
    } // FOR

    for(int chan=28;chan<32;++chan){
      int ch=(chan-28)>>1;
      int gn=chan&1;
      // MONITORING PMTS
      m_chan[chan] = laserObj->getPMADC(ch,gn);
      ATH_MSG_DEBUG("LASERII PMT " << ch << " ("<<gainnames[gn]<<") " << m_chan[chan]);
    } // FOR
    
    // RETRIEVE PEDESTALS IF NOT ALREADY SET
    for(int chan=0;chan<32;++chan){
      int ch=chan>>1;
      int gn=chan&1;
      if(laserObj->isSet(ch, gn, 0) && laserObj->getMean (ch,gn,0)>0) m_chan_Ped[chan]    = laserObj->getMean (ch,gn,0);
      if(laserObj->isSet(ch, gn, 2) && laserObj->getMean (ch,gn,2)>0) m_chan_Led[chan]    = laserObj->getMean (ch,gn,2);
      if(laserObj->isSet(ch, gn, 3) && laserObj->getMean (ch,gn,3)>0) m_chan_Alpha[chan]  = laserObj->getMean (ch,gn,3);
      if(laserObj->isSet(ch, gn, 1) && laserObj->getMean (ch,gn,1)>0) m_chan_Lin[chan]    = laserObj->getMean (ch,gn,1);
      if(laserObj->isSet(ch, gn, 0) && laserObj->getSigma(ch,gn,0)>0) m_chan_SPed[chan]   = laserObj->getSigma(ch,gn,0);
      if(laserObj->isSet(ch, gn, 2) && laserObj->getSigma(ch,gn,2)>0) m_chan_SLed[chan]   = laserObj->getSigma(ch,gn,2);
      if(laserObj->isSet(ch, gn, 3) && laserObj->getSigma(ch,gn,3)>0) m_chan_SAlpha[chan] = laserObj->getSigma(ch,gn,3);
      if(laserObj->isSet(ch, gn, 1) && laserObj->getSigma(ch,gn,1)>0) m_chan_SLin[chan]   = laserObj->getSigma(ch,gn,1);
      
      // DEBUG OUTPUT
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << gainnames[gn] << " CHAN " << ch << " SIG= " << m_chan[chan] << endreq;
        msg(MSG::DEBUG) << gainnames[gn] << " CHAN " << ch << " PED= " << m_chan_Ped[chan]   << "+/-" << m_chan_SPed[chan]   << " ( " << laserObj->isSet(ch, gn, 0) << " ) " << endreq;
        msg(MSG::DEBUG) << gainnames[gn] << " CHAN " << ch << " PED= " << m_chan_Lin[chan]   << "+/-" << m_chan_SLin[chan]   << " ( " << laserObj->isSet(ch, gn, 1) << " ) " << endreq;
        msg(MSG::DEBUG) << gainnames[gn] << " CHAN " << ch << " LED= " << m_chan_Led[chan]   << "+/-" << m_chan_SLed[chan]   << " ( " << laserObj->isSet(ch, gn, 2) << " ) " << endreq;
        msg(MSG::DEBUG) << gainnames[gn] << " CHAN " << ch << " ALP= " << m_chan_Alpha[chan] << "+/-" << m_chan_SAlpha[chan] << " ( " << laserObj->isSet(ch, gn, 3) << " ) " << endreq;
      } // IF
    } // FOR
  } // IF
  else{
    for (unsigned int gn=0; gn<TileLaserObject::nbGains; ++gn) {
      for (unsigned int i=0; i<TileLaserObject::nbPmts; ++i) {
        m_las_PMT_ADC[gn][i] = laserObj->getPMADC(i,gn);
        m_las_PMT_TDC[gn][i] = laserObj->getTDC(i,gn);
        m_las_PMT_Ped[gn][i] = laserObj->getPMPedestal(i,gn);
        m_las_PMT_Ped_RMS[gn][i] = laserObj->getPMSigmaPedestal(i,gn);
        ATH_MSG_VERBOSE( "LasPMT" << i << " g " << gn
                         << " adc " << m_las_PMT_ADC[gn][i]
                         << " ped " << m_las_PMT_Ped[gn][i]
                         << " rms " << m_las_PMT_Ped_RMS[gn][i]
                         << " tdc " << m_las_PMT_TDC[gn][i]);
      } // FOR
      
      for (unsigned int i=0; i<14; ++i) {
        m_las_D_ADC[gn][i] = laserObj->getDiodeADC(i,gn);
        m_las_D_Ped[gn][i] = laserObj->getDiodePedestal(i,gn);
        m_las_D_Ped_RMS[gn][i] = laserObj->getDiodeSigmaPedestal(i,gn);
        m_las_D_Alpha[gn][i] = laserObj->getAlpha(i,gn);
        m_las_D_Alpha_RMS[gn][i] = laserObj->getSigmaAlpha(i,gn);
        m_las_D_AlphaPed[gn][i] = laserObj->getPedestalAlpha(i,gn);
        m_las_D_AlphaPed_RMS[gn][i] = laserObj->getSigmaPedAlpha(i,gn);
        
        ATH_MSG_VERBOSE( "LasD" << i << " g " << gn
                         << " adc " << m_las_D_ADC[gn][i]
                         << " ped " << m_las_D_Ped[gn][i]
                         << " rms " << m_las_D_Ped_RMS[gn][i]
                         << " alp " << m_las_D_Alpha[gn][i]
                         << " rms " << m_las_D_Alpha_RMS[gn][i]
                         << " ape " << m_las_D_AlphaPed[gn][i]
                         << " rms " << m_las_D_AlphaPed_RMS[gn][i]);
      } // FOR
    } // FOR
  } // ELSE
  
  return StatusCode::SUCCESS;
}

StatusCode TileAANtuple::storeBeamElements() {
  
  static const uint32_t* cispar = m_beamInfo->cispar();
  
  uint32_t oldval = 0;
  int last = 0;
  for(int i = 0; i< N_CISPAR; ++i) {
    m_cispar[i] = cispar[i];
    if (msgLvl(MSG::VERBOSE)) {
      if (oldval != cispar[i]) {
        if (last < i-1) {
          msg(MSG::VERBOSE) << "cispar[" << last << ".." << i-1 << "] = "
          << oldval << endmsg;
        } else if (last == i-1) {
          msg(MSG::VERBOSE) << "cispar[" << last << "] = " << oldval << endmsg;
        }
        last = i;
        oldval = cispar[i];
      }
    }
  }
  
  if (msgLvl(MSG::VERBOSE)) {
    if (last < N_CISPAR-1) {
      msg(MSG::VERBOSE) << "cispar[" << last << ".." << N_CISPAR-1 << "] = "
      << oldval << endmsg;
    } else {
      msg(MSG::VERBOSE) << "cispar[" << last << "] = "<< oldval << endmsg;
    }
  }
  
  m_trigType = cispar[12];
  
  // Store ROD header info from collection
  if (m_beamCnv) {
    const eformat::ROBFragment<const uint32_t*> *rob = m_beamCnv->robFragment();
    if (rob) {
      int rod = N_RODS-1;
      m_l1ID[rod]   = rob->rod_lvl1_id();
      m_l1Type[rod] = rob->rod_lvl1_trigger_type();
      m_evType[rod] = rob->rod_detev_type();
      m_evBCID[rod] = rob->rod_bc_id();
      if (m_trigType == 0) m_trigType = -m_l1Type[rod]; // make negative to distinguish from TileCal internal trig types
    }
  }
  
  return StatusCode::SUCCESS;
}


/**
 /// Fill ntuple with data from TRC.
 */
StatusCode
TileAANtuple::storeRawChannels(std::string containerId
                               , float ene[N_ROS2][N_MODULES][N_CHANS]
                               , float time[N_ROS2][N_MODULES][N_CHANS]
                               , float chi2[N_ROS2][N_MODULES][N_CHANS]
                               , float ped[N_ROS2][N_MODULES][N_CHANS]
                               , bool fillAll)
{
  if (containerId.size() == 0) {// empty name, nothing to do
    if (fillAll && m_bsInput) {
      containerId = "TileRawChannelCnt"; // try DSP container name to read DQ status
    } else {
      return StatusCode::FAILURE;
    }
  }
  
  // get named container
  const TileRawChannelContainer* rcCnt;
  CHECK( evtStore()->retrieve(rcCnt, containerId) );
  ATH_MSG_VERBOSE( "Conteiner ID " << containerId );
  
  TileRawChannelUnit::UNIT rChUnit = rcCnt->get_unit();
  ATH_MSG_VERBOSE( "RawChannel unit is " << rChUnit );
  
  bool dspCont = ( rChUnit >= TileRawChannelUnit::OnlineADCcounts );
  if (dspCont) { // this is container with DSP results
    m_dspUnit = rChUnit;
    m_dspFlags = rcCnt->get_bsflags() >> 16;
    ATH_MSG_VERBOSE( "DSP flag is 0x" << MSG::hex << m_dspFlags << MSG::dec
                    << " DSP unit is " << m_dspUnit);
    
  } else if ((m_useDspUnits || m_finalUnit >= TileRawChannelUnit::OnlineADCcounts)
             && rChUnit != TileRawChannelUnit::ADCcounts) {
    ATH_MSG_ERROR( "RawChannel units are not ADC counts, can't apply DSP-like calibration" );
    return StatusCode::FAILURE;
  }
  
  if (m_calibrateEnergy) {
    if (m_useDspUnits) { // calibrate a-la online
      m_rchUnit = m_dspUnit;
    } else { // convert to final units
      m_rchUnit = (TileRawChannelUnit::UNIT)m_finalUnit;
    }
  } else {
    m_rchUnit = rChUnit;
  }
  ATH_MSG_VERBOSE( "Final RawChannel unit is " << m_rchUnit );
  
  std::vector<float> sumE(3);
  float E[48];
  int gain[48];
  if (m_compareMode && dspCont) memset(m_bad,0,sizeof(m_bad));
  
  // Get iterator for all TRCColl in TRCCont
  TileRawChannelContainer::const_iterator itColl = (*rcCnt).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*rcCnt).end();
  
  TileRawChannelCollection::const_iterator it, itEnd;
  
  // Go through all TileRawChannelCollections
  for(; itColl != itCollEnd; ++itColl) {
    int fragId = (*itColl)->identify();
    int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);
    int drawer = fragId & 0x3F;
    int ROS = (fragId>>8);
    int rosI = ROS-1;
    int rosL = rosI;
    int rosH = rosI + N_ROS;
    
    ATH_MSG_VERBOSE( "TRC ("<< containerId
                    <<") Event# "<< m_evtNr
                    << " Frag id 0x" << MSG::hex << fragId << MSG::dec
                    << " ROS " << ROS
                    << " drawer " << drawer );
    
    // go through all TileRawChannels in collection
    it = (*itColl)->begin();
    itEnd = (*itColl)->end();
    
    int cmpCounter = 0;
    if (m_compareMode) {
      memset(E, 0, sizeof(E));
      memset(gain, 0, sizeof(gain));
    }
    
    for(; it != itEnd; ++it) {
      const TileRawChannel* rch = (*it);
      
      HWIdentifier hwid = rch->adc_HWID();
      
      // determine channel
      int channel = m_tileHWID->channel(hwid);
      // convert channel number to PMT number if needed
      if (m_pmtOrder) channel = digiChannel2PMT(ROS,channel);
      
      // determine gain and set ros index accordingly
      int adc = m_tileHWID->adc(hwid);
      if (m_calibMode) {
        if (m_compareMode) {
          ++cmpCounter;
          if(cmpCounter>48) rosI = rosH;
          else              rosI = rosL;
        } else {
          if(adc == 1) rosI = rosH;
          else         rosI = rosL;
        }
      }
      
      /// final calibration
      float energy =  rch->amplitude();
      if (m_rchUnit != rChUnit) {
        if (m_rchUnit < TileRawChannelUnit::OnlineOffset)
          energy = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, energy, rChUnit, m_rchUnit);
        else
          energy = m_tileToolEmscale->channelCalibOnl(drawerIdx, channel, adc, energy, m_rchUnit);
      }
      
      ene[rosI][drawer][channel] = energy;
      time[rosI][drawer][channel] = rch->time();
      chi2[rosI][drawer][channel] = rch->quality();
      ped[rosI][drawer][channel] = rch->pedestal();
      if (m_gain[rosI][drawer][channel] < 0)
        m_gain[rosI][drawer][channel] = adc;
      
      if (m_compareMode) { // filling array for SumEt calculations
        E[channel] = energy;
        gain[channel] = adc;
        if (dspCont) { // use bad flag from DSP container only
          m_bad[rosL][drawer][channel] = (rch->quality()>15.99);
          //} else {
          //m_bad[rosL][drawer][channel] = m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad();
        }
      }
      
      if (msgLvl(MSG::VERBOSE)) {
        int index,pmt;
        rch->cell_ID_index(index,pmt);
        msg(MSG::VERBOSE) << "TRC ch " << channel
        << " gain " << adc
        << " type " << std::min(index,0)
        << " ene=" << energy
        << " time=" << rch->time()
        << " chi2=" << rch->quality()
        << " ped=" << rch->pedestal() << endmsg;
      }
    }
    
    if (fillAll) {
      
      m_ROD_GlobalCRC[rosL][drawer] = (*itColl)->getFragGlobalCRC() & 1;
      m_ROD_BCID[rosL][drawer] = (*itColl)->getFragDSPBCID();
      m_ROD_DMUMask[rosL][drawer][0] = (*itColl)->getFragRODChipMask();
      m_ROD_DMUMask[rosL][drawer][1] = (*itColl)->getFragFEChipMask();
      
      for(unsigned int dmu=0;dmu<N_DMUS;dmu++) {
        
        m_ROD_DMUBCIDErr[rosL][drawer][dmu] = ((*itColl)->getFragBCID() >> dmu) & 1;
        m_ROD_DMUmemoryErr[rosL][drawer][dmu] = ((*itColl)->getFragMemoryPar() >> dmu) & 1;
        m_ROD_DMUSstrobeErr[rosL][drawer][dmu] = ((*itColl)->getFragSstrobe() >> dmu) & 1;
        m_ROD_DMUDstrobeErr[rosL][drawer][dmu]    = ((*itColl)->getFragDstrobe() >> dmu) & 1;
        m_ROD_DMUHeadformatErr[rosL][drawer][dmu] = ((*itColl)->getFragHeaderBit() >> dmu) & 1;
        m_ROD_DMUHeadparityErr[rosL][drawer][dmu] = ((*itColl)->getFragHeaderPar() >> dmu) & 1;
        m_ROD_DMUDataformatErr[rosL][drawer][dmu] = ((*itColl)->getFragSampleBit() >> dmu) & 1;
        m_ROD_DMUDataparityErr[rosL][drawer][dmu] = ((*itColl)->getFragSamplePar() >> dmu) & 1;
        m_ROD_DMUfeCRC[rosL][drawer][dmu] = ((*itColl)->getFragFEChipMask() >> dmu) & 1;
        m_ROD_DMUrodCRC[rosL][drawer][dmu] = ((*itColl)->getFragRODChipMask() >> dmu) & 1;
      }
    }
    
    if (m_compareMode) {
      m_l2Builder->SumE(ROS,drawer,m_rchUnit,E,gain,m_bad[rosL][drawer],sumE);
      if (dspCont) {
        m_sumEt_xx[m_l2Builder->idToIndex(fragId)] = sumE[0];
        m_sumEz_xx[m_l2Builder->idToIndex(fragId)] = sumE[1];
        m_sumE_xx[m_l2Builder->idToIndex(fragId)] = sumE[2];
      }
      else {
        m_sumEt_zz[m_l2Builder->idToIndex(fragId)] = sumE[0];
        m_sumEz_zz[m_l2Builder->idToIndex(fragId)] = sumE[1];
        m_sumE_zz[m_l2Builder->idToIndex(fragId)] = sumE[2];
      }
    }
  }
  
  if (m_compareMode && dspCont) {
    
    const TileL2Container* l2Cnt;
    CHECK( evtStore()->retrieve(l2Cnt, "TileL2Cnt") );
    
    TileL2Container::const_iterator it = l2Cnt->begin();
    TileL2Container::const_iterator end= l2Cnt->end();
    int i=0;
    for(; it != end; ++it) {
      m_sumEt_yy[i++] = (*it)->sumEt();
      m_sumEz_yy[i++] = (*it)->sumEz();
      m_sumE_yy[i++]  = (*it)->sumE();
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode
TileAANtuple::storeMFRawChannels(std::string containerId
                                 , float ene[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
                                 , float time[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
                                 , float chi2[N_ROS2][N_MODULES][N_CHANS]
                                 , float ped[N_ROS2][N_MODULES][N_CHANS]
                                 , bool fillAll)
{
  if (containerId.size() == 0) {// empty name, nothing to do
    if (fillAll && m_bsInput) {
      containerId = "TileRawChannelCnt"; // try DSP container name to read DQ status
    } else {
      return StatusCode::FAILURE;
    }
  }
  
  // get named container
  const TileRawChannelContainer* rcCnt;
  CHECK( evtStore()->retrieve(rcCnt, containerId) );
  
  TileRawChannelUnit::UNIT rChUnit = rcCnt->get_unit();
  ATH_MSG_VERBOSE( "RawChannel unit is " << rChUnit );
  
  bool dspCont = ( rChUnit >= TileRawChannelUnit::OnlineADCcounts );
  if (dspCont) { // this is container with DSP results
    m_dspUnit = rChUnit;
    m_dspFlags = rcCnt->get_bsflags() >> 16;
    ATH_MSG_VERBOSE( "DSP flag is 0x" << MSG::hex << m_dspFlags << MSG::dec
                    << " DSP unit is " << m_dspUnit);
    
  } else if ((m_useDspUnits || m_finalUnit >= TileRawChannelUnit::OnlineADCcounts)
             && rChUnit != TileRawChannelUnit::ADCcounts) {
    ATH_MSG_ERROR( "RawChannel units are not ADC counts, can't apply DSP-like calibration" );
    return StatusCode::FAILURE;
  }
  
  if (m_calibrateEnergy) {
    if (m_useDspUnits) { // calibrate a-la online
      m_rchUnit = m_dspUnit;
    } else { // convert to final units
      m_rchUnit = (TileRawChannelUnit::UNIT)m_finalUnit;
    }
  } else {
    m_rchUnit = rChUnit;
  }
  ATH_MSG_VERBOSE( "Final RawChannel unit is " << m_rchUnit );
  
  std::vector<float> sumE(3);
  float E[48];
  int gain[48];
  if (m_compareMode && dspCont) memset(m_bad, 0, sizeof(m_bad));
  
  // Get iterator for all TRCColl in TRCCont
  TileRawChannelContainer::const_iterator itColl = (*rcCnt).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*rcCnt).end();
  
  TileRawChannelCollection::const_iterator it, itEnd;
  
  // Go through all TileRawChannelCollections
  for (; itColl != itCollEnd; ++itColl) {
    int fragId = (*itColl)->identify();
    int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);
    int drawer = fragId & 0x3F;
    int ROS = (fragId >> 8);
    int rosI = ROS - 1;
    int rosL = rosI;
    int rosH = rosI + N_ROS;
    
    ATH_MSG_VERBOSE( "TRC ("<< containerId
                    <<") Event# "<< m_evtNr
                    << " Frag id 0x" << MSG::hex << fragId << MSG::dec
                    << " ROS " << ROS
                    << " drawer " << drawer );
    
    // go through all TileRawChannels in collection
    it = (*itColl)->begin();
    itEnd = (*itColl)->end();
    
    int cmpCounter = 0;
    if (m_compareMode) {
      memset(E, 0, sizeof(E));
      memset(gain, 0, sizeof(gain));
    }
    for(; it != itEnd; ++it) {
      const TileRawChannel* rch = (*it);
      
      HWIdentifier hwid=rch->adc_HWID();
      
      // determine channel
      int channel = m_tileHWID->channel(hwid);
      // convert channel number to PMT number if needed
      if (m_pmtOrder) channel = digiChannel2PMT(ROS,channel);
      
      // determine gain and set ros index accordingly
      int adc = m_tileHWID->adc(hwid);
      if (m_calibMode) {
        if (m_compareMode) {
          ++cmpCounter;
          if(cmpCounter>48) rosI = rosH;
          else              rosI = rosL;
        } else {
          if(adc == 1) rosI = rosH;
          else         rosI = rosL;
        }
      }
      
      /// final calibration
      float energy = 0.;
      for (int i = 0; i < 7; ++i) {
        energy = rch->amplitude(i);
        if (m_rchUnit != rChUnit) {
          if (m_rchUnit < TileRawChannelUnit::OnlineOffset)
            energy = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, energy, rChUnit, m_rchUnit);
          else
            energy = m_tileToolEmscale->channelCalibOnl(drawerIdx, channel, adc, energy, m_rchUnit);
        }
	      	
        ene[rosI][drawer][channel][i] = energy;
        time[rosI][drawer][channel][i] = rch->time(i);
      }
      chi2[rosI][drawer][channel] = rch->quality();
      ped[rosI][drawer][channel] = rch->pedestal();
      
      if (m_gain[rosI][drawer][channel] < 0)
        m_gain[rosI][drawer][channel] = adc;
      
      if (m_compareMode) { // filling array for SumEt calculations
        E[channel] = ene[rosI][drawer][channel][0];
        gain[channel] = adc;
        if (dspCont) { // use bad flag from DSP container only
          m_bad[rosL][drawer][channel] = (rch->quality() > 15.99);
          //} else {
          //m_bad[rosL][drawer][channel] = m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad();
        }
      }
      
      if (msgLvl(MSG::VERBOSE)) {
        int index,pmt;
        rch->cell_ID_index(index,pmt);
        msg(MSG::VERBOSE) << "TRC ch " << channel
        << " gain " << adc
        << " type " << std::min(index,0)
        << " ene=" << ene[rosI][drawer][channel][0]
        << " time=" << rch->time()
        << " chi2=" << rch->quality()
        << " ped=" << rch->pedestal() << endmsg;
      }
    }
    
    if (fillAll) {
      
      m_ROD_GlobalCRC[rosL][drawer] = (*itColl)->getFragGlobalCRC() & 1;
      m_ROD_BCID[rosL][drawer] = (*itColl)->getFragDSPBCID();
      m_ROD_DMUMask[rosL][drawer][0] = (*itColl)->getFragRODChipMask();
      m_ROD_DMUMask[rosL][drawer][1] = (*itColl)->getFragFEChipMask();
      
      for(unsigned int dmu=0;dmu<N_DMUS;dmu++) {
        
        m_ROD_DMUBCIDErr[rosL][drawer][dmu] = ((*itColl)->getFragBCID() >> dmu) & 1;
        m_ROD_DMUmemoryErr[rosL][drawer][dmu] = ((*itColl)->getFragMemoryPar() >> dmu) & 1;
        m_ROD_DMUSstrobeErr[rosL][drawer][dmu] = ((*itColl)->getFragSstrobe() >> dmu) & 1;
        m_ROD_DMUDstrobeErr[rosL][drawer][dmu]    = ((*itColl)->getFragDstrobe() >> dmu) & 1;
        m_ROD_DMUHeadformatErr[rosL][drawer][dmu] = ((*itColl)->getFragHeaderBit() >> dmu) & 1;
        m_ROD_DMUHeadparityErr[rosL][drawer][dmu] = ((*itColl)->getFragHeaderPar() >> dmu) & 1;
        m_ROD_DMUDataformatErr[rosL][drawer][dmu] = ((*itColl)->getFragSampleBit() >> dmu) & 1;
        m_ROD_DMUDataparityErr[rosL][drawer][dmu] = ((*itColl)->getFragSamplePar() >> dmu) & 1;
        m_ROD_DMUfeCRC[rosL][drawer][dmu] = ((*itColl)->getFragFEChipMask() >> dmu) & 1;
        m_ROD_DMUrodCRC[rosL][drawer][dmu] = ((*itColl)->getFragRODChipMask() >> dmu) & 1;
      }
    }
    
    if (m_compareMode) {
      m_l2Builder->SumE(ROS, drawer, m_rchUnit, E, gain, m_bad[rosL][drawer], sumE);
      if (dspCont) {
        m_sumEt_xx[m_l2Builder->idToIndex(fragId)] = sumE[0];
        m_sumEz_xx[m_l2Builder->idToIndex(fragId)] = sumE[1];
        m_sumE_xx[m_l2Builder->idToIndex(fragId)] = sumE[2];
      }
      else {
        m_sumEt_zz[m_l2Builder->idToIndex(fragId)] = sumE[0];
        m_sumEz_zz[m_l2Builder->idToIndex(fragId)] = sumE[1];
        m_sumE_zz[m_l2Builder->idToIndex(fragId)] = sumE[2];
      }
    }
  }
  
  if (m_compareMode && dspCont) {
    
    const TileL2Container* l2Cnt;
    CHECK( evtStore()->retrieve(l2Cnt, "TileL2Cnt") );
    
    TileL2Container::const_iterator it = l2Cnt->begin();
    TileL2Container::const_iterator end= l2Cnt->end();
    int i=0;
    for(; it != end; ++it) {
      m_sumEt_yy[i++] = (*it)->sumEt();
      m_sumEz_yy[i++] = (*it)->sumEz();
      m_sumE_yy[i++]  = (*it)->sumE();
    }
  }
  
  return StatusCode::SUCCESS;
}


/**
 /// Fill Ntuple with info from TileDigits
 /// Return true if the collection is empty
 */
StatusCode
TileAANtuple::storeDigits(std::string containerId
                          , short a_sample[N_ROS2][N_MODULES][N_CHANS][N_SAMPLES]
                          , short a_gain[N_ROS2][N_MODULES][N_CHANS]
                          , bool fillAll)
{
  if (containerId.size() == 0) // empty name, nothing to do
    return StatusCode::FAILURE;
  
  // Read Digits from TES
  const TileDigitsContainer* digitsCnt;
  CHECK( evtStore()->retrieve(digitsCnt, containerId) );
  
  bool emptyColl = true;
  
  // Get iterator for all TDColl in TDCont
  TileDigitsContainer::const_iterator itColl = (*digitsCnt).begin();
  TileDigitsContainer::const_iterator itCollEnd = (*digitsCnt).end();
  
  // Go through all TileDigitsCollections
  for(; itColl != itCollEnd; itColl++) {
    int fragId = (*itColl)->identify();
    int drawer = fragId & 0x3F;
    int ROS = (fragId>>8);
    int rosI = ROS-1;
    int rosL = rosI;
    int rosH = rosI + N_ROS;
    
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Event# " << m_evtNr
      << " Frag id 0x" << MSG::hex << fragId << MSG::dec
      << " ROS " << ROS
      << " drawer " << drawer << endmsg;
      
      if (fillAll) {
        msg(MSG::VERBOSE) << "       Size=" << (*itColl)->getFragSize()
        << " BCID=" << (*itColl)->getFragBCID() << MSG::hex
        << " CRC=0x" << ((*itColl)->getFragCRC()&0xffff)
        << " DMUMask=0x" << ((*itColl)->getFragDMUMask()&0xffff) << MSG::dec << endmsg;
        
        msg(MSG::VERBOSE) << "       Lvl1ID=" << (*itColl)->getLvl1Id()
        << " Lvl1Type=" << (*itColl)->getLvl1Type()
        << " EvBCID=" << (*itColl)->getRODBCID()
        << " EvType=" << (*itColl)->getDetEvType() << endmsg;
        
        msg(MSG::VERBOSE) <<"       Header=" << (*itColl)->getFragChipHeaderWords() << endmsg;
      }
    }
    
    /// Store ROD header info from collection
    /// (should be just one per ROD, i.e. 4 subsequent drawers give the same ROD number)
    int rod = (rosL*N_MODULES + drawer) >> 2;
    
    m_l1ID[rod] = (*itColl)->getLvl1Id();
    m_l1Type[rod] = (*itColl)->getLvl1Type();
    m_evType[rod] = (*itColl)->getDetEvType();
    m_evBCID[rod] = (*itColl)->getRODBCID();
    if (m_trigType == 0) m_trigType = -m_l1Type[rod]; // make negative to distinguish from TileCal internal trig types
    
    TileDigitsCollection::const_iterator it = (*itColl)->begin();
    TileDigitsCollection::const_iterator itEnd = (*itColl)->end();
    
    // non empty collection
    if(it != itEnd) {
      emptyColl = false;
      
      if (fillAll) {
        
        // store evtnr, bcid,crc, size
        // Same for lo and hi, because they come from the same fragment
        
        m_rodBCID[rosL][drawer] = (*itColl)->getRODBCID();
        m_fragSize[rosL][drawer]=(*itColl)->getFragSize();
        
        m_slinkCRC[rosL][drawer][0] = ((*itColl)->getFragCRC()>>16) & 0xffff;
        m_dmuMask[rosL][drawer][0] = ((*itColl)->getFragDMUMask()>>16) & 0xffff;
        m_slinkCRC[rosL][drawer][1] = (*itColl)->getFragCRC() & 0xffff;
        m_dmuMask[rosL][drawer][1] = (*itColl)->getFragDMUMask() & 0xffff;
        
        uint32_t CRCmask = (*itColl)->getFragDMUMask(); //mask of FE+ROD DMU crc check (16bit+16bit) 0xffffffff == All ok
        uint32_t fe_crc = CRCmask & 0xFFFF;
        uint32_t rod_crc = CRCmask >> 16;
        
        const std::vector<uint32_t> & headerVec = (*itColl)->getFragChipHeaderWords();
        unsigned int headsize = std::min(16U, static_cast<unsigned int>(headerVec.size()));
        
        for (unsigned int ih = 0; ih < headsize; ++ih) {
          
          m_DMUheader[rosL][drawer][ih] = headerVec[ih];   /// Full DMU header, stored for debugging
          m_DMUbcid[rosL][drawer][ih] = (headerVec[ih] & 0xFFF);             /// BCID in DMU header
          m_DMUformatErr[rosL][drawer][ih] = CheckDMUFormat(headerVec[ih]); /// bit_31==1 and bit_17==0
          m_DMUparityErr[rosL][drawer][ih] = CheckDMUParity(headerVec[ih]); /// parity must be an odd number
          m_DMUmemoryErr[rosL][drawer][ih] = (headerVec[ih] >> 25 & 0x1); /// memory parity error bit_25
          m_DMUSstrobeErr[rosL][drawer][ih] = (headerVec[ih] >> 24 & 0x1); /// single strobe error bit_24 (it is recovered)
          m_DMUDstrobeErr[rosL][drawer][ih] = (headerVec[ih] >> 23 & 0x1); /// double strobe error bit_23 (cannot be recovered)
          
          m_feCRC[rosL][drawer][ih] = (fe_crc >> ih & 0x1);
          m_rodCRC[rosL][drawer][ih] = (rod_crc >> ih & 0x1);
        }
        
        if (m_calibMode) {
          const std::vector<uint32_t> & headerVecHi = (*itColl)->getFragChipHeaderWordsHigh();
          unsigned int headsizehi = std::min(16U, static_cast<unsigned int>(headerVecHi.size()));
          
          for (unsigned int ih = 0; ih < headsizehi; ++ih) {
            
            m_DMUheader[rosH][drawer][ih] = headerVecHi[ih];                    /// Full DMU header, stored for debugging
            m_DMUbcid[rosH][drawer][ih] = (headerVecHi[ih] & 0xFFF);            /// BCID in DMU header
            m_DMUformatErr[rosH][drawer][ih] = CheckDMUFormat(headerVecHi[ih]); /// bit_31==1 and bit_17==0
            m_DMUparityErr[rosH][drawer][ih] = CheckDMUParity(headerVecHi[ih]); /// parity must be an odd number
            m_DMUmemoryErr[rosH][drawer][ih]  = (headerVecHi[ih] >> 25 & 0x1);  /// memory parity error bit_25
            m_DMUSstrobeErr[rosH][drawer][ih] = (headerVecHi[ih] >> 24 & 0x1);  /// single strobe error bit_24 (it is recovered)
            m_DMUDstrobeErr[rosH][drawer][ih] = (headerVecHi[ih] >> 23 & 0x1);  /// double strobe error bit_23 (cannot be recovered)
            
            m_feCRC[rosH][drawer][ih]  = -1 ; //Variables must be filled anyway, empty variables are not allowed
            m_rodCRC[rosH][drawer][ih] = -1;  //Variables must be filled anyway, empty variables are not allowed
          }
        }
      }
      
      int cmpCounter = 0;
      // go through all TileDigits in collection
      for (; it != itEnd; it++) {
        const TileDigits* digit = (*it);
        
        HWIdentifier hwid = digit->adc_HWID();
        
        // determine channel
        int channel = m_tileHWID->channel(hwid);
        // convert channel number to PMT number if needed
        if (m_pmtOrder) channel = digiChannel2PMT(ROS,channel);
        
        // determine gain and set ros index accordingly
        int gain = m_tileHWID->adc(hwid);
        if (m_calibMode) {
          if (m_compareMode) {
            ++cmpCounter;
            if (cmpCounter > 48) rosI = rosH;
            else rosI = rosL;
          } else {
            if (gain == 1) rosI = rosH;
            else rosI = rosL;
          }
        }
        
        a_gain[rosI][drawer][channel] = gain;
        
        // get digits
        const std::vector<float> & sampleVec = digit->samples();
        int siz = sampleVec.size();
        if (msgLvl(MSG::VERBOSE)) {
          int index,pmt;
          digit->cell_ID_index(index,pmt);
          msg(MSG::VERBOSE) << "TD ch " << channel
          << " gain "<< gain
          << " type " << std::min(index,0) << " {";
          for(int i=0;i<siz;i++) {
            msg(MSG::VERBOSE) <<(int)sampleVec[i] << " ";
          }
        }
        if (siz > N_SAMPLES) {
          siz = N_SAMPLES;
          if (msgLvl(MSG::VERBOSE))
            msg(MSG::VERBOSE) << "} ONLY " << siz << " digits saved to ntuple"
            << endmsg;
        } else {
          if (msgLvl(MSG::VERBOSE))
            msg(MSG::VERBOSE) << "}" << endmsg;
        }
        for (int n = 0; n < siz; ++n) {
          a_sample[rosI][drawer][channel][n] = (short) sampleVec[n];
        }
      }
    }
  }
  
  if (emptyColl) return StatusCode::FAILURE;
  else return StatusCode::SUCCESS;
}

StatusCode TileAANtuple::storeTMDBDecision() {

  const char * part[4] = {"LBA","LBC","EBA","EBC"};

  // Read Decision from TES
  //
  if (m_tileMuRcvContainer.size()>0){

    ATH_MSG_VERBOSE( "reading TMDB decision from " << m_tileMuRcvContainer ); 

    const TileMuonReceiverContainer *decisionCnt;
    CHECK( evtStore()->retrieve(decisionCnt, m_tileMuRcvContainer) );
  
    TileMuonReceiverContainer::const_iterator it = decisionCnt->begin();
    TileMuonReceiverContainer::const_iterator itLast = decisionCnt->end();
  
    // Go through all decisions
    for(; it != itLast; ++it) {

      const TileMuonReceiverObj * obj = (*it);

      const std::vector<bool> & decision = obj->GetDecision(); 
      int siz = decision.size();

      if (siz>0) {

        int fragId = (*it)->identify();
        int drawer = fragId & 0x3F;
        int ros    = (fragId>>8) - 1;
 
        if (siz > N_TMDBDECISIONS) {
          ATH_MSG_VERBOSE( "ONLY " << N_TMDBDECISIONS << " decisions saved to ntuple instead of " << siz);
          siz = N_TMDBDECISIONS;
        }

        for (int n = 0; n < siz; ++n) {
          m_decisionTMDB[ros][drawer][n] = (unsigned char) decision[n];
        }

        if (msgLvl(MSG::VERBOSE)) {
          std::stringstream ss;
          for (int n = 0; n < siz; ++n) {
            ss<<std::setw(5)<<(int)m_decisionTMDB[ros][drawer][n];
          }
          msg(MSG::VERBOSE) << "   0x" <<MSG::hex<< fragId <<MSG::dec<<" "<< part[ros] 
                            << std::setfill('0') << std::setw(2)
                            << drawer+1 << std::setfill(' ') 
                            << "      decision: " <<ss.str() << endmsg;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
 
StatusCode TileAANtuple::storeTMDBDigits() {

  const char * part[4] = {"LBA","LBC","EBA","EBC"};

  // Read Digits from TES
  //
  if (m_tileMuRcvDigitsContainer.size()>0){

    ATH_MSG_VERBOSE( "reading TMDB digits from " << m_tileMuRcvDigitsContainer ); 

    const TileDigitsContainer* digitsCnt;
    CHECK( evtStore()->retrieve(digitsCnt, m_tileMuRcvDigitsContainer) );
  
    TileDigitsContainer::const_iterator itColl1 = (*digitsCnt).begin();
    TileDigitsContainer::const_iterator itCollEnd1 = (*digitsCnt).end();
  
    // Go through all TileDigitsCollections
    for(; itColl1 != itCollEnd1; ++itColl1) {

      TileDigitsCollection::const_iterator it1 = (*itColl1)->begin();
      TileDigitsCollection::const_iterator itEnd1 = (*itColl1)->end();

      if (it1!=itEnd1) {

        int fragId = (*itColl1)->identify();
        int drawer = fragId & 0x3F;
        int ros    = (fragId>>8) - 1;
        int ichannel = 0;
 
        ATH_MSG_VERBOSE( "   0x" <<MSG::hex<< fragId <<MSG::dec<<" "<< part[ros]
                         << std::setfill('0') << std::setw(2)
                          << drawer+1 << std::setfill(' ') ); 

        for (; it1 != itEnd1; ++it1) {

          if (ichannel>=N_TMDBCHANS) {
            ATH_MSG_WARNING("Too many channels in TMDB Digi container for frag 0x" <<MSG::hex<< fragId <<MSG::dec <<" keeping only first " << N_TMDBCHANS << " channels in ntuple ");
            break;
          }

          const TileDigits* digit = (*it1);
        
          // get digits
          const std::vector<float> & sampleVec = digit->samples();
          int siz = sampleVec.size();

          if (siz > N_SAMPLES) {
            ATH_MSG_VERBOSE( "ONLY " << N_SAMPLES << " digits saved to ntuple instead of " << siz);
            siz = N_SAMPLES;
          }

          for (int n = 0; n < siz; ++n) {
            m_sampleTMDB[ros][drawer][ichannel][n] = (unsigned char) sampleVec[n];
          }

          if (msgLvl(MSG::VERBOSE)) {
            std::stringstream ss;
            for (int n = 0; n < siz; ++n) {
              ss<<std::setw(5)<<(int)m_sampleTMDB[ros][drawer][ichannel][n];
            }
            msg(MSG::VERBOSE) << "      dig: " <<ros+1<<"/"<<drawer<<"/"<<m_tileHWID->channel(digit->adc_HWID())<<": "<<ss.str() << endmsg;;
          }
      
          ++ichannel;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileAANtuple::storeTMDBRawChannel() {

  const char * part[4] = {"LBA","LBC","EBA","EBC"};

  // Read Raw Channels from TDS
  //
  if (m_tileMuRcvRawChannelContainer.size()>0){

    ATH_MSG_VERBOSE( "reading TMDB energies from " << m_tileMuRcvRawChannelContainer ); 

    const TileRawChannelContainer* rcCnt;
    CHECK( evtStore()->retrieve(rcCnt, m_tileMuRcvRawChannelContainer) );

    TileRawChannelContainer::const_iterator itColl2 = (*rcCnt).begin();
    TileRawChannelContainer::const_iterator itCollEnd2 = (*rcCnt).end();
  
    // Go through all TileDigitsCollections
    for(; itColl2 != itCollEnd2; ++itColl2) {

      TileRawChannelCollection::const_iterator it2 = (*itColl2)->begin();
      TileRawChannelCollection::const_iterator itEnd2 = (*itColl2)->end();

      if (it2!=itEnd2) {

        int fragId = (*itColl2)->identify();
        int drawer = fragId & 0x3F;
        int ros    = (fragId>>8) - 1;
        int ichannel = 0;
  
        ATH_MSG_VERBOSE( "   0x" <<MSG::hex<< fragId <<MSG::dec<<" "<< part[ros]
                         << std::setfill('0') << std::setw(2)
                         << drawer+1 << std::setfill(' ') ); 

        for (; it2 != itEnd2; ++it2) {

          if (ichannel>=N_TMDBCHANS) {
            ATH_MSG_WARNING("Too many channels in TMDB RCh container for frag 0x" <<MSG::hex<< fragId <<MSG::dec <<" keeping only first " << N_TMDBCHANS << " channels in ntuple ");
            break;
          }

          const TileRawChannel* rc = (*it2);
        
          m_eTMDB[ros][drawer][ichannel] =  rc -> amplitude();

          ATH_MSG_VERBOSE( "      rc: " <<ros+1<<"/"<<drawer<<"/"<<m_tileHWID->channel(rc->adc_HWID())<< ": " << m_eTMDB[ros][drawer][ichannel] );

          ++ichannel;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
TileAANtuple::finalize() {
  ATH_MSG_INFO( "finalize() successfully" );
  return StatusCode::SUCCESS;
}

StatusCode
TileAANtuple::ntuple_clear() {
  
  TRIGGER_clearBranch();
  CISPAR_clearBranch();
  LASER_clearBranch();
  DIGI_clearBranch();
  TMDB_clearBranch();

  return StatusCode::SUCCESS;
}

StatusCode
TileAANtuple::initNTuple(void) {
  //Aux Ntuple creation
  
  if (m_ntupleID.size() > 0) {
    
    std::string ntupleID = m_ntupleID + "_map";
    
    TTree* ntuplePtr = new TTree(ntupleID.c_str(), "TileCal-CellMap");
    if(m_thistSvc->regTree("/" + m_streamName + "/" + ntupleID, ntuplePtr).isFailure()) {
      ATH_MSG_ERROR( "Problem registering TileRec CellMap Tree" );
    }
    
    fillCellMap(ntuplePtr);
    
    //Ntuple creation
    m_ntuplePtr = new TTree(m_ntupleID.c_str(), "TileCal-Ntuple");
    m_ntuplePtr->SetMaxTreeSize(m_treeSize);
    if (m_thistSvc->regTree("/" + m_streamName + "/" + m_ntupleID, m_ntuplePtr).isFailure()) {
      ATH_MSG_ERROR( "Problem registering TileRec Tree");
    }
    
    TRIGGER_addBranch();
    CISPAR_addBranch();
    if (m_laserObject.size() > 0) {
      const TileLaserObject* laserObj;
      CHECK( evtStore()->retrieve(laserObj, m_laserObject) );
      m_las_version = laserObj->getVersion();
      LASER_addBranch();
    }
    DIGI_addBranch();
    TMDB_addBranch();
  }
  
  //DCS Ntuple creation
  if (m_checkDCS) {
    std::string ntupleDCS = "Tile_DCS";
    m_DCSntuplePtr = new TTree(ntupleDCS.c_str(), "TileCal-DCS data");
    if (m_thistSvc->regTree("/" + m_streamName + "/" + ntupleDCS, m_DCSntuplePtr).isFailure()) {
      ATH_MSG_ERROR( "Problem registering TileRec DCS Tree");
    }
    DCS_addBranch();
  }
  
  return StatusCode::SUCCESS;
}


/**
 /////////////////////////////////////////////////////////////////////////////
 //
 ///    Variables Legenda
 ///
 ///  - C : a character string terminated by the 0 character
 ///  - B : an 8 bit signed integer
 ///  - b : an 8 bit unsigned integer                    2^8=256
 ///  - S : a 16 bit signed integer (i.e. a "short")
 ///  - s : a 16 bit unsigned integer                    2^16=65536
 ///  - I : a 32 bit signed integer (i.e an "int")
 ///  - i : a 32 bit unsigned integer                    2^32=4294967296
 ///  - F : a 32 bit floating point (i.e. a "float")
 ///  - D : a 64 bit floating point (i.e. a "double")
 ///  - L : a 64 bit signed integer
 ///  - l : a 64 bit unsigned integer
 ///  - O : a boolean
 //
 */

/**
 //////////////////////////////////////////////////////////////////////////////
 ///Create eta-phi map for all channels
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::fillCellMap(TTree* ntuplePtr) {
  
  float eta[4][64][48];
  float phi[4][64][48];
  short tower[4][64][48];
  short sample[4][64][48];
  short ind[4][64][48];
  short pmt[4][64][48];
  short bad[4][64][48][2];
  
  CLEAR(eta);
  CLEAR(phi);
  CLEAR1(tower);
  CLEAR1(sample);
  CLEAR1(ind);
  CLEAR(pmt);
  CLEAR1(bad);
  
  ntuplePtr->Branch("eta", eta, "eta[4][64][48]/F");
  ntuplePtr->Branch("phi", phi, "phi[4][64][48]/F");
  ntuplePtr->Branch("tower", tower, "tower[4][64][48]/S");
  ntuplePtr->Branch("sample", sample, "sample[4][64][48]/S");
  ntuplePtr->Branch("ind", ind, "ind[4][64][48]/S");
  ntuplePtr->Branch("pmt", pmt, "pmt[4][64][48]/S");
  ntuplePtr->Branch("bad", bad, "bad[4][64][48][2]/S");
  
  TileDetDescrManager::calo_element_const_iterator itr = m_tileMgr->tile_cell_begin();
  TileDetDescrManager::calo_element_const_iterator end = m_tileMgr->tile_cell_end();
  
  for (; itr != end; ++itr) {
    const CaloDetDescrElement * caloDDE = (*itr);
    Identifier cell_id = caloDDE->identify();
    IdentifierHash hash[2] = { caloDDE->onl1(), caloDDE->onl2() };
    for (int i = 0; i < 2; ++i) {
      if (hash[i] != TileHWID::NOT_VALID_HASH) {
        HWIdentifier adc_id = m_tileHWID->adc_id(hash[i], 0);
        int ROS = m_tileHWID->ros(adc_id);
        int drawer = m_tileHWID->drawer(adc_id);
        int chan = m_tileHWID->channel(adc_id);
        int index, pm;
        m_cabling->h2s_cell_id_index(adc_id, index, pm); // index=-2 for MBTS or -1 for all disconnected channels
        if (index > -1) index = i; // just put back 0 or 1 for all connected channels
        pm = m_cabling->channel2hole(ROS, chan); // convert channel to PMT number, ignoring difference between drawers
        if ((ROS == 3 && drawer == 14) || (ROS == 4 && drawer == 17)) {
          if (pm < 0) pm = -pm; // crack scin in EBA15 EBC18 in non-standard place - recover positive pmt number
          if (chan == 2 || chan == 3) pm = -pm; // no D4 in special EBA15 EBC18 - put negative sign
        }
        if (m_pmtOrder) chan = digiChannel2PMT(ROS,chan); // convert channel to PMT-1
        int rosI = ROS-1; // make ros index from 0 to 3
        eta[rosI][drawer][chan] = caloDDE->eta();
        phi[rosI][drawer][chan] = caloDDE->phi();
        tower[rosI][drawer][chan] = m_tileID->tower(cell_id);
        sample[rosI][drawer][chan] = m_tileID->sample(cell_id);
        ind[rosI][drawer][chan] = index;
        pmt[rosI][drawer][chan] = pm;
      }
    }
  }
  
  const MbtsDetDescrManager* mbtsMgr; //!< Pointer to MbtsDetDescrManager
  if ( detStore()->retrieve(mbtsMgr).isFailure() ) {
    ATH_MSG_WARNING( "Unable to retrieve MbtsDetDescrManager from DetectorStore" );
    mbtsMgr = 0;
  }
  for (int ROS = 1; ROS < 5; ++ROS) {
    int rosI = ROS - 1;
    for (int drawer = 0; drawer < 64; ++drawer) {
      for (int chan = 0; chan < 48; ++chan) {
        for (int adc = 0; adc < 2; ++adc) {
          HWIdentifier adc_id = m_tileHWID->adc_id(ROS, drawer, chan, adc);
          bad[rosI][drawer][chan][adc] = (short) m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
          int index, pm;
          Identifier cell_id = m_cabling->h2s_cell_id_index(adc_id, index, pm);
          if (index == -2) { // MBTS
            ind[rosI][drawer][chan] = index;
            pmt[rosI][drawer][chan] = 1; // we know that all MBTS are in PMT 1
            if (mbtsMgr) {
              const CaloDetDescrElement * caloDDE = mbtsMgr->get_element(cell_id);
              if (caloDDE) {
                if (caloDDE->z() > 0.0)
                  eta[rosI][drawer][chan] = fabs(caloDDE->eta());
                else
                  eta[rosI][drawer][chan] = -fabs(caloDDE->eta());
                phi[rosI][drawer][chan] = caloDDE->phi();
              }
            }
          }
        }
      }
    }
  }
  
  ntuplePtr->Fill();
}


/**
 //////////////////////////////////////////////////////////////////////////////
 ///Add TRIGGER variables to the Tree
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::TRIGGER_addBranch(void) {
  m_ntuplePtr->Branch("EvTime",&m_evTime,"EvTime/I");
  m_ntuplePtr->Branch("Run",&m_run,"Run/I");
  m_ntuplePtr->Branch("LumiBlock",&m_lumiBlock,"LumiBlock/I");
  m_ntuplePtr->Branch("Evt",&m_evt,"Evt/I");
  m_ntuplePtr->Branch("EvtNr",&m_evtNr,"EvtNr/I");
  m_ntuplePtr->Branch("Trig",&m_trigType,"Trig/I");
  m_ntuplePtr->Branch("DSPflags",&m_dspFlags,"DSPflags/i");
  m_ntuplePtr->Branch("DSPunits",&m_dspUnit,"DSPunits/S");
  m_ntuplePtr->Branch("OFLunits",&m_rchUnit,"OFLunits/S");
  
  if (m_bsInput) {
    m_ntuplePtr->Branch("L1ID",   m_l1ID,   "L1ID[65]/I");
    m_ntuplePtr->Branch("L1Type", m_l1Type, "L1Type[65]/I");
    m_ntuplePtr->Branch("EvType", m_evType, "EvType[65]/I");
    m_ntuplePtr->Branch("EvBCID", m_evBCID, "EvBCID[65]/I");
  }
}


/**
 //////////////////////////////////////////////////////////////////////////////
 //Clear Tree TRIGGER variables
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::TRIGGER_clearBranch(void) {
  m_evTime=0;
  m_run=0;
  m_evt=0;
  m_trigType=0;
  m_dspFlags=0;
  
  CLEAR1(m_l1ID);
  CLEAR1(m_l1Type);
  CLEAR1(m_evType);
  CLEAR1(m_evBCID);
}


/**
 //////////////////////////////////////////////////////////////////////////////
 ///Add Tree CISPAR variables Tree
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::CISPAR_addBranch(void) {
  if (m_beamElemContainer.size() > 0) {
    m_ntuplePtr->Branch("cispar",m_cispar,"cispar[110]/i");
  }
}


/**
 //////////////////////////////////////////////////////////////////////////////
 //Clear Tree CISPAR variables
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::CISPAR_clearBranch(void) {
  if (m_beamElemContainer.size() > 0) {
    CLEAR(m_cispar);
  }
}


/**
 //////////////////////////////////////////////////////////////////////////////
 ///Add Tree LASER variables Tree
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::LASER_addBranch(void) {
  
  if (m_laserObject.size() > 0) {
    
    const char* gainnames[2]  = {"LG","HG"};
    const char* channames[16] = {"Diode0","Diode1","Diode2","Diode3","Diode4","Diode5","Diode6","Diode7",
      "Diode8","Diode9","PMT1","ExtCIS0","IntCIS","DiodePhocal","PMT2","ExtCIS1"};
    
    m_ntuplePtr->Branch("LASER_BCID", &m_las_BCID, "LASER_BCID/I");
    
    m_ntuplePtr->Branch("LASER_FILTER", &m_las_Filt, "LASER_FILTER/I");
    m_ntuplePtr->Branch("LASER_REQAMP", &m_las_ReqAmp, "LASER_REQAMP/F");
    m_ntuplePtr->Branch("LASER_MEASAMP", &m_las_MeasAmp, "LASER_MEASAMP/F");
    
    if(m_las_version==2) {
      
      ATH_MSG_DEBUG("LASERII BRANCHING..");
      
      m_ntuplePtr->Branch(Form("LASER_QDCTIMEOUT"),&(m_qdctimeout),Form("LASER_QDCTIMEOUT/O"));
      m_ntuplePtr->Branch(Form("LASER_TDCTIMEOUT"),&(m_tdctimeout),Form("LASER_TDCTIMEOUT/O"));
      m_ntuplePtr->Branch(Form("LASER_DAQTYPE"),&(m_daqtype),Form("LASER_DAQTYPE/I"));
      for(int chan=0;chan<32;++chan){
        int ch=chan>>1;
        int gn=chan&1;
        m_ntuplePtr->Branch(Form("LASER_%s_%s_ADC",gainnames[gn],channames[ch]),&(m_chan[chan]),Form("LASER_%s_%s_ADC/I",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Ped",gainnames[gn],channames[ch]),&(m_chan_Ped[chan]),Form("LASER_%s_%s_Ped/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Led",gainnames[gn],channames[ch]),&(m_chan_Led[chan]),Form("LASER_%s_%s_Led/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Ped1",gainnames[gn],channames[ch]),&(m_chan_Lin[chan]),Form("LASER_%s_%s_Ped1/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Alpha",gainnames[gn],channames[ch]),&(m_chan_Alpha[chan]),Form("LASER_%s_%s_Alpha/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Sigma_Ped",gainnames[gn],channames[ch]),&(m_chan_SPed[chan]),Form("LASER_%s_%s_Sigma_Ped/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Sigma_Led",gainnames[gn],channames[ch]),&(m_chan_SLed[chan]),Form("LASER_%s_%s_Sigma_Led/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Sigma_Ped1",gainnames[gn],channames[ch]),&(m_chan_SLin[chan]),Form("LASER_%s_%s_Sigma_Ped1/F",gainnames[gn],channames[ch]));
        m_ntuplePtr->Branch(Form("LASER_%s_%s_Sigma_Alpha",gainnames[gn],channames[ch]),&(m_chan_SAlpha[chan]),Form("LASER_%s_%s_Sigma_Alpha/F",gainnames[gn],channames[ch]));
      } // FOR
      
    } else {
      
      ATH_MSG_DEBUG("LASERI BRANCHING..");
      
      m_ntuplePtr->Branch("LASER_Diode_1_ADC", &(m_las_D_ADC[0][0]), "LASER_Diode_1_ADC/I");
      m_ntuplePtr->Branch("LASER_Diode_2_ADC", &(m_las_D_ADC[0][1]), "LASER_Diode_2_ADC/I");
      m_ntuplePtr->Branch("LASER_Diode_3_ADC", &(m_las_D_ADC[0][2]), "LASER_Diode_3_ADC/I");
      m_ntuplePtr->Branch("LASER_Diode_4_ADC", &(m_las_D_ADC[0][3]), "LASER_Diode_4_ADC/I");
      
      m_ntuplePtr->Branch("LASER_Diode_1_Ped", &(m_las_D_Ped[0][0]), "LASER_Diode_1_Ped/F");
      m_ntuplePtr->Branch("LASER_Diode_2_Ped", &(m_las_D_Ped[0][1]), "LASER_Diode_2_Ped/F");
      m_ntuplePtr->Branch("LASER_Diode_3_Ped", &(m_las_D_Ped[0][2]), "LASER_Diode_3_Ped/F");
      m_ntuplePtr->Branch("LASER_Diode_4_Ped", &(m_las_D_Ped[0][3]), "LASER_Diode_4_Ped/F");
      
      m_ntuplePtr->Branch("LASER_Diode_1_Ped_RMS", &(m_las_D_Ped_RMS[0][0]), "LASER_Diode_1_Ped_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_2_Ped_RMS", &(m_las_D_Ped_RMS[0][1]), "LASER_Diode_2_Ped_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_3_Ped_RMS", &(m_las_D_Ped_RMS[0][2]), "LASER_Diode_3_Ped_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_4_Ped_RMS", &(m_las_D_Ped_RMS[0][3]), "LASER_Diode_4_Ped_RMS/F");
      
      m_ntuplePtr->Branch("LASER_Diode_1_Alpha", &(m_las_D_Alpha[0][0]), "LASER_Diode_1_Alpha/F");
      m_ntuplePtr->Branch("LASER_Diode_2_Alpha", &(m_las_D_Alpha[0][1]), "LASER_Diode_2_Alpha/F");
      m_ntuplePtr->Branch("LASER_Diode_3_Alpha", &(m_las_D_Alpha[0][2]), "LASER_Diode_3_Alpha/F");
      m_ntuplePtr->Branch("LASER_Diode_4_Alpha", &(m_las_D_Alpha[0][3]), "LASER_Diode_4_Alpha/F");
      
      m_ntuplePtr->Branch("LASER_Diode_1_Alpha_RMS", &(m_las_D_Alpha_RMS[0][0]), "LASER_Diode_1_Alpha_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_2_Alpha_RMS", &(m_las_D_Alpha_RMS[0][1]), "LASER_Diode_2_Alpha_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_3_Alpha_RMS", &(m_las_D_Alpha_RMS[0][2]), "LASER_Diode_3_Alpha_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_4_Alpha_RMS", &(m_las_D_Alpha_RMS[0][3]), "LASER_Diode_4_Alpha_RMS/F");
      
      m_ntuplePtr->Branch("LASER_Diode_1_AlphaPed", &(m_las_D_AlphaPed[0][0]), "LASER_Diode_1_AlphaPed/F");
      m_ntuplePtr->Branch("LASER_Diode_2_AlphaPed", &(m_las_D_AlphaPed[0][1]), "LASER_Diode_2_AlphaPed/F");
      m_ntuplePtr->Branch("LASER_Diode_3_AlphaPed", &(m_las_D_AlphaPed[0][2]), "LASER_Diode_3_AlphaPed/F");
      m_ntuplePtr->Branch("LASER_Diode_4_AlphaPed", &(m_las_D_AlphaPed[0][3]), "LASER_Diode_4_AlphaPed/F");
      
      m_ntuplePtr->Branch("LASER_Diode_1_AlphaPed_RMS", &(m_las_D_AlphaPed_RMS[0][0]), "LASER_Diode_1_AlphaPed_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_2_AlphaPed_RMS", &(m_las_D_AlphaPed_RMS[0][1]), "LASER_Diode_2_AlphaPed_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_3_AlphaPed_RMS", &(m_las_D_AlphaPed_RMS[0][2]), "LASER_Diode_3_AlphaPed_RMS/F");
      m_ntuplePtr->Branch("LASER_Diode_4_AlphaPed_RMS", &(m_las_D_AlphaPed_RMS[0][3]), "LASER_Diode_4_AlphaPed_RMS/F");
      
      m_ntuplePtr->Branch("LASER_PMT_1_ADC", &(m_las_PMT_ADC[0][0]), "LASER_PMT_1_ADC/I");
      m_ntuplePtr->Branch("LASER_PMT_2_ADC", &(m_las_PMT_ADC[0][1]), "LASER_PMT_2_ADC/I");
      
      m_ntuplePtr->Branch("LASER_PMT_1_TDC", &(m_las_PMT_TDC[0][0]), "LASER_PMT_1_TDC/I");
      m_ntuplePtr->Branch("LASER_PMT_2_TDC", &(m_las_PMT_TDC[0][1]), "LASER_PMT_2_TDC/I");
      
      m_ntuplePtr->Branch("LASER_PMT_1_Ped", &(m_las_PMT_Ped[0][0]), "LASER_PMT_1_Ped/F");
      m_ntuplePtr->Branch("LASER_PMT_2_Ped", &(m_las_PMT_Ped[0][1]), "LASER_PMT_2_Ped/F");
      
      m_ntuplePtr->Branch("LASER_PMT_1_Ped_RMS", &(m_las_PMT_Ped_RMS[0][0]), "LASER_PMT_1_Ped_RMS/F");
      m_ntuplePtr->Branch("LASER_PMT_2_Ped_RMS", &(m_las_PMT_Ped_RMS[0][1]), "LASER_PMT_2_Ped_RMS/F");
      
    }
    
    m_ntuplePtr->Branch("LASER_HEAD_Temp", &m_las_Temperature, "LASER_HEAD_Temp/F");
  }
}


/**
 //////////////////////////////////////////////////////////////////////////////
 //Clear Tree LASER variables
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::LASER_clearBranch(void) {
  
  if (m_laserObject.size() > 0) {
    
    m_las_BCID = 0;
    
    m_las_Filt = 0;
    m_las_ReqAmp = 0.0;
    m_las_MeasAmp = 0.0;
    m_las_Temperature = 0.0;
    
    // LASERII
    //    memset(m_chan, 0, sizeof(m_chan));
    //    memset(m_chan_Ped, 0, sizeof(m_chan_Ped));
    //    memset(m_chan_Led, 0, sizeof(m_chan_Led));
    //    memset(m_chan_Lin, 0, sizeof(m_chan_Lin));
    //    memset(m_chan_Alpha, 0, sizeof(m_chan_Alpha));
    //    memset(m_chan_SPed, 0, sizeof(m_chan_SPed));
    //    memset(m_chan_SLed, 0, sizeof(m_chan_SLed));
    //    memset(m_chan_SLin, 0, sizeof(m_chan_SLin));
    //    memset(m_chan_SAlpha, 0, sizeof(m_chan_SAlpha));
    
    // LASERI
    memset(m_las_D_ADC,          0, sizeof(m_las_D_ADC));
    memset(m_las_D_Ped,          0, sizeof(m_las_D_Ped));
    memset(m_las_D_Ped_RMS,      0, sizeof(m_las_D_Ped_RMS));
    memset(m_las_D_Alpha,        0, sizeof(m_las_D_Alpha));
    memset(m_las_D_Alpha_RMS,    0, sizeof(m_las_D_Alpha_RMS));
    memset(m_las_D_AlphaPed,     0, sizeof(m_las_D_AlphaPed));
    memset(m_las_D_AlphaPed_RMS, 0, sizeof(m_las_D_AlphaPed_RMS));
    
    memset(m_las_PMT_ADC,        0, sizeof(m_las_PMT_ADC));
    memset(m_las_PMT_TDC,        0, sizeof(m_las_PMT_TDC));
    memset(m_las_PMT_Ped,        0, sizeof(m_las_PMT_Ped));
    memset(m_las_PMT_Ped_RMS,    0, sizeof(m_las_PMT_Ped_RMS));
    
  }
}


/**
 //////////////////////////////////////////////////////////////////////////////
 ///Add Tree DIGI variables Tree
 //
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::DIGI_addBranch(void)
{
  std::string suf[3] = {"_lo","_hi",""};
  if (m_compareMode) {
    suf[0] = "_xx";
    suf[1] = "_yy";
    suf[2] = "_zz";
    m_ntuplePtr->Branch(NAME2("sumEt",suf[0]),       m_sumEt_xx,     NAME3("sumEt",    suf[0],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumEz",suf[0]),       m_sumEz_xx,     NAME3("sumEz",    suf[0],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumE", suf[0]),       m_sumE_xx,      NAME3("sumE",     suf[0],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumEt",suf[1]),       m_sumEt_yy,     NAME3("sumEt",    suf[1],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumEz",suf[1]),       m_sumEz_yy,     NAME3("sumEz",    suf[1],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumE", suf[1]),       m_sumE_yy,      NAME3("sumE",     suf[1],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumEt",suf[2]),       m_sumEt_zz,     NAME3("sumEt",    suf[2],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumEz",suf[2]),       m_sumEz_zz,     NAME3("sumEz",    suf[2],"[4][64]/F")); // float
    m_ntuplePtr->Branch(NAME2("sumE", suf[2]),       m_sumE_zz,      NAME3("sumE",     suf[2],"[4][64]/F")); // float
  }
  
  int imin = 2, imax = 3, ir = 0;
  
  if (m_calibMode) {
    imin = 0;
    imax = 2;
  }
  
  for (int i = imin; i < imax; ++i) {
    
    std::string f_suf(suf[i]);
    
    if (m_fltDigitsContainer.size() == 0 && m_digitsContainer.size() == 0
        && (m_rawChannelContainer.size() > 0
            || m_fitRawChannelContainer.size() > 0
            || m_fitcRawChannelContainer.size() > 0
            || m_optRawChannelContainer.size() > 0
            || m_qieRawChannelContainer.size() > 0
            || m_dspRawChannelContainer.size() > 0
            || m_mfRawChannelContainer.size() > 0
            || m_of1RawChannelContainer.size() > 0
            || m_bsInput) ) {
          
          m_ntuplePtr->Branch(NAME2("gain",f_suf),            m_gain[ir],          NAME3("gain",         f_suf,"[4][64][48]/S"));    // short
          
        } else {
          
          if (m_fltDigitsContainer.size() > 0) {
            if (m_digitsContainer.size() > 0) { // should use different names for two containers
              
              m_ntuplePtr->Branch(NAME2("sampleFlt",f_suf),   m_sampleFlt[ir],     NAME3("sampleFlt",    f_suf,"[4][64][48][7]/S")); // short
              m_ntuplePtr->Branch(NAME2("gainFlt",f_suf),     m_gainFlt[ir],       NAME3("gainFlt",      f_suf,"[4][64][48]/S"));    // short
            } else {
              m_ntuplePtr->Branch(NAME2("sample",f_suf),      m_sampleFlt[ir],     NAME3("sampleFlt",    f_suf,"[4][64][48][7]/S")); // short
              if (m_rawChannelContainer.size() > 0
                  || m_fitRawChannelContainer.size() > 0
                  || m_fitcRawChannelContainer.size() > 0
                  || m_optRawChannelContainer.size() > 0
                  || m_qieRawChannelContainer.size() > 0
                  || m_of1RawChannelContainer.size() > 0
                  || m_dspRawChannelContainer.size() > 0
                  || m_bsInput) {
                
                m_ntuplePtr->Branch(NAME2("gain",f_suf),      m_gain[ir],          NAME3("gain",         f_suf,"[4][64][48]/S"));    // short
              } else {
                m_ntuplePtr->Branch(NAME2("gain",f_suf),      m_gainFlt[ir],       NAME3("gainFlt",      f_suf,"[4][64][48]/S"));    // short
              }
            }
          }
          
          if (m_digitsContainer.size() > 0) {
            m_ntuplePtr->Branch(NAME2("sample",f_suf),          m_sample[ir],        NAME3("sample",       f_suf,"[4][64][48][7]/S")); // short
            m_ntuplePtr->Branch(NAME2("gain",f_suf),            m_gain[ir],          NAME3("gain",         f_suf,"[4][64][48]/S"));    // short
            
            if (m_bsInput) {
              m_ntuplePtr->Branch(NAME2("DMUheader",f_suf),       m_DMUheader[ir],     NAME3("DMUheader",        f_suf,"[4][64][16]/i")); // uint32
              m_ntuplePtr->Branch(NAME2("DMUBCID",f_suf),         m_DMUbcid[ir],       NAME3("DMUBCID",          f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("DMUmemoryErr",f_suf),    m_DMUmemoryErr[ir],  NAME3("DMUmemoryErr",     f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("DMUSstrobeErr",f_suf),   m_DMUSstrobeErr[ir], NAME3("DMUSstrobeErr",    f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("DMUDstrobeErr",f_suf),   m_DMUDstrobeErr[ir], NAME3("DMUDstrobeErr",    f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("DMUheadformatErr",f_suf),m_DMUformatErr[ir],  NAME3("DMUheadformatErr", f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("DMUheadparityErr",f_suf),m_DMUparityErr[ir],  NAME3("DMUheadparityErr", f_suf,"[4][64][16]/S")); // short
              
              m_ntuplePtr->Branch(NAME2("feCRC",f_suf),         m_feCRC[ir],         NAME3("feCRC",        f_suf,"[4][64][16]/S")); // short
              m_ntuplePtr->Branch(NAME2("rodCRC",f_suf),        m_rodCRC[ir],        NAME3("rodCRC",       f_suf,"[4][64][16]/S")); // short
              
              if (i == imin) { // common for low and high gain
                m_ntuplePtr->Branch("rodBCID",  m_rodBCID,  "rodBCID[4][64]/S");    // short
                m_ntuplePtr->Branch("fragSize", m_fragSize,"fragSize[4][64]/S");    // short
                m_ntuplePtr->Branch("DMUmask",  m_dmuMask,  "DMUmask[4][64][2]/s"); // unsigned short
                m_ntuplePtr->Branch("slinkCRC", m_slinkCRC,"slinkCRC[4][64][2]/s"); // unsigned short
              }
            }
          }
        }
    
    if (m_rawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("ene",f_suf),     m_ene[ir],          NAME3("ene",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("time",f_suf),    m_time[ir],        NAME3("time",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("ped",f_suf),     m_ped[ir],          NAME3("ped",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2",f_suf),    m_chi2[ir],        NAME3("chi2",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_fitRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eFit",f_suf),    m_eFit[ir],        NAME3("eFit",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tFit",f_suf),    m_tFit[ir],        NAME3("tFit",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedFit",f_suf),  m_pedFit[ir],    NAME3("pedFit",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2Fit",f_suf), m_chi2Fit[ir],  NAME3("chi2Fit",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_fitcRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eFitc",f_suf),   m_eFitc[ir],      NAME3("eFitc",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tFitc",f_suf),   m_tFitc[ir],      NAME3("tFitc",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedFitc",f_suf), m_pedFitc[ir],  NAME3("pedFitc",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2Fitc",f_suf),m_chi2Fitc[ir],NAME3("chi2Fitc",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_optRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eOpt",f_suf),    m_eOpt[ir],        NAME3("eOpt",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tOpt",f_suf),    m_tOpt[ir],        NAME3("tOpt",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedOpt",f_suf),  m_pedOpt[ir],    NAME3("pedOpt",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2Opt",f_suf), m_chi2Opt[ir],  NAME3("chi2Opt",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_qieRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eQIE",f_suf),    m_eQIE[ir],        NAME3("eQIE",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tQIE",f_suf),    m_tQIE[ir],        NAME3("tQIE",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedQIE",f_suf),  m_pedQIE[ir],    NAME3("pedQIE",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2QIE",f_suf), m_chi2QIE[ir],  NAME3("chi2QIE",f_suf,"[4][64][48]/F")); // float
    }

    if (m_of1RawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eOF1",f_suf),    m_eOF1[ir],        NAME3("eOF1",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tOF1",f_suf),    m_tOF1[ir],        NAME3("tOF1",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedOF1",f_suf),  m_pedOF1[ir],    NAME3("pedOF1",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2OF1",f_suf), m_chi2OF1[ir],  NAME3("chi2OF1",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_dspRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eDsp",f_suf),    m_eDsp[ir],        NAME3("eDsp",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("tDsp",f_suf),    m_tDsp[ir],        NAME3("tDsp",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedDsp",f_suf),  m_pedDsp[ir],    NAME3("pedDsp",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2Dsp",f_suf), m_chi2Dsp[ir],  NAME3("chi2Dsp",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_mfRawChannelContainer.size() > 0) {
      m_ntuplePtr->Branch(NAME2("eMF",f_suf),    m_eMF[ir],        NAME3("eMF",f_suf,"[4][64][48][7]/F")); // float
      m_ntuplePtr->Branch(NAME2("tMF",f_suf),    m_tMF[ir],        NAME3("tMF",f_suf,"[4][64][48][7]/F")); // float
      m_ntuplePtr->Branch(NAME2("chi2MF",f_suf), m_chi2MF[ir],     NAME3("chi2MF",f_suf,"[4][64][48]/F")); // float
      m_ntuplePtr->Branch(NAME2("pedMF",f_suf),  m_pedMF[ir],      NAME3("pedMF",f_suf,"[4][64][48]/F")); // float
    }
    
    if (m_bsInput) {
      if (i == imin) { // common for low and high gain
        m_ntuplePtr->Branch("ROD_GlobalCRC",       m_ROD_GlobalCRC,        "ROD_GlobalCRC[4][64]/S");            // short
        m_ntuplePtr->Branch("ROD_BCID",            m_ROD_BCID,             "ROD_BCID[4][64]/S");                 // short
        m_ntuplePtr->Branch("ROD_DMUBCIDErr",      m_ROD_DMUBCIDErr,       "ROD_DMUBCIDErr[4][64][16]/S");       // short
        m_ntuplePtr->Branch("ROD_DMUmemoryErr",    m_ROD_DMUmemoryErr,     "ROD_DMUmemoryErr[4][64][16]/S");     // short
        m_ntuplePtr->Branch("ROD_DMUSstrobeErr",   m_ROD_DMUSstrobeErr,    "ROD_DMUSstrobeErr[4][64][16]/S");    // short
        m_ntuplePtr->Branch("ROD_DMUDstrobeErr",   m_ROD_DMUDstrobeErr,    "ROD_DMUDstrobeErr[4][64][16]/S");    // short
        m_ntuplePtr->Branch("ROD_DMUheadformatErr",m_ROD_DMUHeadformatErr, "ROD_DMUheadformatErr[4][64][16]/S"); // short
        m_ntuplePtr->Branch("ROD_DMUheadparityErr",m_ROD_DMUHeadparityErr, "ROD_DMUheadparityErr[4][64][16]/S"); // short
        m_ntuplePtr->Branch("ROD_DMUdataformatErr",m_ROD_DMUDataformatErr, "ROD_DMUdataformatErr[4][64][16]/S"); // short
        m_ntuplePtr->Branch("ROD_DMUdataparityErr",m_ROD_DMUDataparityErr, "ROD_DMUdataparityErr[4][64][16]/S"); // short
        
        m_ntuplePtr->Branch("ROD_feCRC",           m_ROD_DMUfeCRC,         "ROD_feCRC[4][64][16]/S");            // short
        m_ntuplePtr->Branch("ROD_rodCRC",          m_ROD_DMUrodCRC,        "ROD_rodCRC[4][64][16]/S");           // short
        m_ntuplePtr->Branch("ROD_DMUmask",         m_ROD_DMUMask,          "ROD_DMUmask[4][64][2]/s");           // unsigned short
      }
    }
    ir += N_ROS;
  }
}

/**
 //////////////////////////////////////////////////////////////////////////////
 ///Clear Tree DIGI variables
 //////////////////////////////////////////////////////////////////////////////
 */
void TileAANtuple::DIGI_clearBranch(void) {
  unsigned int size = (m_calibMode) ? 1 : 2;
  
  if (m_compareMode) {
    CLEAR(m_sumEt_xx);
    CLEAR(m_sumEt_yy);
    CLEAR(m_sumEt_zz);
    CLEAR(m_sumEz_xx);
    CLEAR(m_sumEz_yy);
    CLEAR(m_sumEz_zz);
    CLEAR(m_sumE_xx);
    CLEAR(m_sumE_yy);
    CLEAR(m_sumE_zz);
  }
  
  CLEAR3(m_gain, size);
  
  if (m_fltDigitsContainer.size() > 0) {
    CLEAR3(m_sampleFlt, size);
    CLEAR3(m_gainFlt, size);
  }
  
  if (m_digitsContainer.size() > 0) {
    CLEAR3(m_sample, size);
    
    if (m_bsInput) {
      CLEAR2(m_DMUheader, size);
      CLEAR3(m_DMUbcid, size);
      CLEAR3(m_DMUformatErr, size);
      CLEAR3(m_DMUparityErr, size);
      CLEAR3(m_DMUmemoryErr, size);
      CLEAR3(m_DMUSstrobeErr, size);
      CLEAR3(m_DMUDstrobeErr, size);
      
      CLEAR3(m_feCRC, size);
      CLEAR3(m_rodCRC, size);
      
      CLEAR1(m_rodBCID);
      CLEAR1(m_fragSize);
      CLEAR(m_dmuMask);
      CLEAR(m_slinkCRC);
    }
    
  }
  
  if (m_rawChannelContainer.size() > 0) {
    CLEAR2(m_ene, size);
    CLEAR2(m_time, size);
    CLEAR2(m_ped, size);
    CLEAR2(m_chi2, size);
  }
  
  if (m_fitRawChannelContainer.size() > 0) {
    CLEAR2(m_eFit, size);
    CLEAR2(m_tFit, size);
    CLEAR2(m_pedFit, size);
    CLEAR2(m_chi2Fit, size);
  }
  
  if (m_fitcRawChannelContainer.size() > 0) {
    CLEAR2(m_eFitc, size);
    CLEAR2(m_tFitc, size);
    CLEAR2(m_pedFitc, size);
    CLEAR2(m_chi2Fitc, size);
  }
  
  if (m_optRawChannelContainer.size() > 0) {
    CLEAR2(m_eOpt, size);
    CLEAR2(m_tOpt, size);
    CLEAR2(m_pedOpt, size);
    CLEAR2(m_chi2Opt, size);
  }
  

  if (m_qieRawChannelContainer.size() > 0) {
    CLEAR2(m_eQIE, size);
    CLEAR2(m_tQIE, size);
    CLEAR2(m_pedQIE, size);
    CLEAR2(m_chi2QIE, size);
  }

  if (m_of1RawChannelContainer.size() > 0) {
    CLEAR2(m_eOF1, size);
    CLEAR2(m_tOF1, size);
    CLEAR2(m_pedOF1, size);
    CLEAR2(m_chi2OF1, size);
  }
  
  if (m_dspRawChannelContainer.size() > 0) {
    CLEAR2(m_eDsp, size);
    CLEAR2(m_tDsp, size);
    CLEAR2(m_pedDsp, size);
    CLEAR2(m_chi2Dsp, size);
  }
  
  if (m_mfRawChannelContainer.size() > 0) {
    CLEAR2(m_eMF, size);
    CLEAR2(m_tMF, size);
    CLEAR2(m_chi2MF, size);
    CLEAR2(m_pedMF, size);
  }
  
  if (m_bsInput) {
    CLEAR1(m_ROD_GlobalCRC);
    CLEAR1(m_ROD_BCID);
    CLEAR1(m_ROD_DMUBCIDErr);
    CLEAR1(m_ROD_DMUmemoryErr);
    CLEAR1(m_ROD_DMUSstrobeErr);
    CLEAR1(m_ROD_DMUDstrobeErr);
    CLEAR1(m_ROD_DMUHeadformatErr);
    CLEAR1(m_ROD_DMUHeadparityErr);
    CLEAR1(m_ROD_DMUDataformatErr);
    CLEAR1(m_ROD_DMUDataparityErr);
    CLEAR1(m_ROD_DMUfeCRC);
    CLEAR1(m_ROD_DMUrodCRC);
    CLEAR(m_ROD_DMUMask);
  }
  
}

/*//////////////////////////////////////////////////////////////////////////////
 // TMDB variables
 //////////////////////////////////////////////////////////////////////////////
 */

void TileAANtuple::TMDB_addBranch(void)
{

  if (m_tileMuRcvRawChannelContainer.size()>0) {
    m_ntuplePtr->Branch("eTMDB", m_eTMDB, "eTMDB[4][64][8]/F");  // float m_eTMDB[N_ROS][N_MODULES][N_TMDBCHANS]
  }

  if (m_tileMuRcvDigitsContainer.size()>0) {
    m_ntuplePtr->Branch("sampleTMDB", m_sampleTMDB, "sampleTMDB[4][64][8][7]/b"); // unsigned char m_sampleTMDB[N_ROS][N_MODULES][N_TMDBCHANS][N_SAMPLES]
  }

  if (m_tileMuRcvContainer.size()>0) {
    m_ntuplePtr->Branch("decisionTMDB", m_decisionTMDB, "decisionTMDB[4][64][4]/b"); // unsigned char m_decisionTMDB[N_ROS][N_MODULES][N_TMDBDECISIONS]
  }

}

void TileAANtuple::TMDB_clearBranch(void)
{
  if (m_tileMuRcvRawChannelContainer.size()>0) CLEAR(m_eTMDB);
  if (m_tileMuRcvDigitsContainer.size()>0) CLEAR(m_sampleTMDB);
  if (m_tileMuRcvContainer.size()>0) CLEAR(m_decisionTMDB);
}

/*/////////////////////////////////////////////////////////////////////////////
 // DCS variables
 /////////////////////////////////////////////////////////////////////////////
 */

void TileAANtuple::DCS_addBranch() {
  bool br[9];
  int mask = m_DCSBranches;
  
  for (int i = 0; i < 9; ++i) {
    br[i] = (mask % 10);
    mask /= 10;
  }
  
  if (br[0]) {
    m_DCSntuplePtr->Branch("EvTime", &m_evTime,  "EvTime/I");
    m_DCSntuplePtr->Branch("Run",    &m_run,     "Run/I");
    m_DCSntuplePtr->Branch("LumiBlock",&m_lumiBlock,"LumiBlock/I");
    m_DCSntuplePtr->Branch("HHMMSS", &m_HHMMSS,  "HHMMSS/I");
    m_DCSntuplePtr->Branch("Evt",    &m_evt,     "Evt/I");
    m_DCSntuplePtr->Branch("EvtNr",  &m_evtNr,   "EvtNr/I");
  }
  
  if (br[1]) m_DCSntuplePtr->Branch("TEMP",    m_TEMP,    "TEMP[4][64][7]/F");
  if (br[2]) m_DCSntuplePtr->Branch("HV",      m_HV,      "HV[4][64][48]/F");
  if (br[3]) m_DCSntuplePtr->Branch("HVSET",   m_HVSET,   "HVSET[4][64][48]/F");
  if (br[4]) m_DCSntuplePtr->Branch("DRSTATES",m_DRSTATES,"DRSTATES[4][64]/I");
  if (br[5]) m_DCSntuplePtr->Branch("HVSTATUS",m_HVSTATUS,"HVSTATUS[4][64][48]/S");
  if (br[6]) m_DCSntuplePtr->Branch("DRSTATUS",m_DRSTATUS,"DRSTATUS[4][64]/S");
  if (br[7]) m_DCSntuplePtr->Branch("CHSTATUS",m_CHSTATUS,"CHSTATUS[4][64][48]/S");
  if (br[8]) {
    m_DCSntuplePtr->Branch("nBadDr",    &m_nBadDr,    "nBadDr/I");
    m_DCSntuplePtr->Branch("nBadHV",    &m_nBadHV,    "nBadHV/I");
    m_DCSntuplePtr->Branch("nBadDCS",   &m_nBadDCS,   "nBadDCS/I");
    m_DCSntuplePtr->Branch("nBadDB",    &m_nBadDB,    "nBadDB/I");
    m_DCSntuplePtr->Branch("nBadTotal", &m_nBadTotal, "nBadTotal/I");
  }
}

StatusCode TileAANtuple::storeDCS()
{
  if (m_DCScounter != m_tileDCSSvc->getNcalls()) {
    
    ATH_MSG_DEBUG( "Filling DCS ntuple: cnt=" << m_DCScounter
                  <<" evtCnt=" << m_evtNr
                  << " evt=" << m_evt
                  << " lumi=" << m_lumiBlock << "  " << m_dateTime );
    
    m_DCScounter = m_tileDCSSvc->getNcalls();
    
    CLEAR(m_TEMP);
    CLEAR(m_HV);
    CLEAR(m_HVSET);
    CLEAR(m_DRSTATES);
    CLEAR(m_HVSTATUS);
    CLEAR(m_DRSTATUS);
    CLEAR(m_CHSTATUS);
    
    m_nBadDr = 0;
    m_nBadHV = 0;
    m_nBadDCS = 0;
    m_nBadDB  = 0;
    m_nBadTotal = 0;
    for (int ROS = 1; ROS < 5; ++ROS) {
      int rosI = ROS - 1;
      
      for (int drawer = 0; drawer < 64; ++drawer) {
        int module = drawer + 1;
        m_DRSTATES[rosI][drawer] = m_tileDCSSvc->getDCSSTATES(ROS, module);
        m_DRSTATUS[rosI][drawer] = m_tileDCSSvc->getDCSSTATUS(ROS, drawer);
        bool drbad = m_tileDCSSvc->statusIsBad(ROS, drawer);
        
        if (drbad) {
          ++m_nBadDr;
        }
        
        if (msgLvl(MSG::VERBOSE) || m_DRSTATUS[rosI][drawer] != TileDCSSvc::OK_DRAWER) {
          msg(MSG::VERBOSE) << "Module=" << m_tileDCSSvc->partitionName(ROS)
          << std::setw(2) << std::setfill('0') << module
          << " DRSTATES=" << m_DRSTATES[rosI][drawer]
          << " DRSTATUS=" << m_DRSTATUS[rosI][drawer]
          << " => " << ((drbad) ? "bad" : "good") << endmsg;
        }
        
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ROS,drawer);
        for (int channel=0; channel<48; ++channel){
          TileBchStatus chStat = m_tileBadChanTool->getChannelStatus(drawerIdx,channel);
          int pmt=abs(m_cabling->channel2hole(ROS,channel));
          m_HV[rosI][drawer][channel]       = m_tileDCSSvc->getDCSHV(ROS, module, pmt);
          m_HVSET[rosI][drawer][channel]    = m_tileDCSSvc->getDCSHVSET(ROS, module, pmt);
          m_HVSTATUS[rosI][drawer][channel] = m_tileDCSSvc->getDCSHVSTATUS(ROS, drawer, channel);
          m_CHSTATUS[rosI][drawer][channel] = m_tileDCSSvc->getDCSSTATUS(ROS, drawer, channel)
          + 100 * m_tileBadChanTool->encodeStatus(chStat);
          bool chbad=m_tileDCSSvc->statusIsBad(ROS,drawer,channel);
          
          if (chbad || chStat.isBad()) {
            ++m_nBadTotal;
            if (chbad) ++m_nBadDCS;
            if (chStat.isBad()) ++m_nBadDB;
          }
          
          if (m_tileDCSSvc->statusHVIsBad(ROS,drawer,channel)) {
            ++m_nBadHV;
          }
          
          if (msgLvl(MSG::VERBOSE) || (chbad && !drbad)) {
            msg(MSG::VERBOSE) << "Module=" << m_tileDCSSvc->partitionName(ROS)
            << std::setw(2) << std::setfill('0') << module
            << " channel=" << channel << " pmt=" << pmt
            << " HV=" << m_HV[rosI][drawer][channel]
            << " HVSET=" << m_HVSET[rosI][drawer][channel]
            << " HVSTATUS=" << m_HVSTATUS[rosI][drawer][channel]
            << " CHSTATUS=" << m_CHSTATUS[rosI][drawer][channel]
            << " => " << ((chbad) ? "bad" : "good") << endmsg;
          }
        }
        
        for (int ind=0; ind<7; ++ind){
          m_TEMP[rosI][drawer][ind] = m_tileDCSSvc->getDCSHV(ROS, module, ind+49);
          ATH_MSG_VERBOSE( "Module=" << m_tileDCSSvc->partitionName(ROS)
                          << std::setw(2) << std::setfill('0') << module
                          << " TEMP" << ind+1 << "=" << m_TEMP[rosI][drawer][ind] );
          
        }
      }
    }
    
    ATH_MSG_DEBUG( "BAD status in DCS: nBadDr=" << m_nBadDr
                  << " nBadHV=" << m_nBadHV
                  << " nBadDCS=" << m_nBadDCS
                  << " nBadDB="  << m_nBadDB
                  << " nBadTotal=" << m_nBadTotal );
    
    m_DCSntuplePtr->Fill();
  }
  
  return StatusCode::SUCCESS;
}
