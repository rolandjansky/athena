/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTBStat.cxx
//  Author   : Alexander Solodkov
//  Created  : Jun, 2004
//
//  DESCRIPTION:
//     Provide statistics for a run
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi Includes


//Atlas include
#include "eformat/FullEventFragment.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "AthenaKernel/errorcheck.h"

//TileCal include
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileByteStream/TileBeamElemContByteStreamCnv.h"
#include "TileTBRec/TileTBStat.h"

#include "boost/io/ios_state.hpp"

#include <iostream>
#include <ctime>
#include <cmath>

// Constructor & deconstructor
/** @class TileTBStat
 *  @brief Class to produce run statistics. Used to fill commissioning database. 
 */
 
StatDouble::StatDouble() {
  Min = HUGE_VAL;
  Max = -HUGE_VAL;
  Sum = 0;
  SumSq = 0;
  Counter = 0;
}

void StatDouble::addValue(double Value) {
  if (Value <= 0.0) return;
  if (Max < Value) Max = Value;
  if (Min > Value) Min = Value;
  Sum += Value;
  SumSq += Value * Value;
  Counter++;
}

double StatDouble::getMean() {
  return Sum / Counter;
}

double StatDouble::getChi2() {
  if (Counter <= 1) return 0;
  return sqrt(std::max(0.0,((SumSq - ((Sum * Sum) / Counter)) / (Counter - 1))));
}

void StatDouble::print(const char* s, bool minMaxOnly) {
  boost::io::ios_base_all_saver coutsave(std::cout);
  std::cout << s << " " << std::fixed;
  if (minMaxOnly) {
    if (Counter > 0)
      std::cout << " "   << std::setw(4) << std::setprecision(1) << Min 
                << " - " << std::setw(4) << std::setprecision(1) << Max;
    else
      std::cout << " ---- - ----";
  } else {
    if (Counter > 0)
      std::cout << " "        << std::setw(4) << std::setprecision(1) << Min 
                << " - "      << std::setw(4) << std::setprecision(1) << Max
                << "  mean= " << std::setw(6) << std::setprecision(3) << getMean()
                << " rms= "   << std::setw(5) << std::setprecision(3) << getChi2();
    else
      std::cout << " ---- - ----  mean= ------ rms= -----";
  }
  std::cout << std::endl;
}

StatInt::StatInt() {
}

void StatInt::addValue(int Value) {
  ++checkOut[Value];
}

void StatInt::print(const char* s, bool minMaxOnly) {
  std::cout << s << " ";
  std::map<int, int>::const_iterator min = checkOut.begin();
  std::map<int, int>::const_iterator max = checkOut.end();
  if (min == max) { // empty map
    std::cout << " ---";
  } else {
    --max;
    if (min == max) { // one value only
      std::cout << " " << min->first;
    } else {
      if (minMaxOnly) {
        std::cout << " " << min->first << " - " << max->first;
      } else {
        for (++max; min != max; ++min)
          std::cout << " " << min->first << "(" << min->second << ")";
      }
    }
  }
  std::cout << std::endl;
}

TileTBStat::TileTBStat(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_RobSvc("ROBDataProviderSvc", name)
  , m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider")
  , m_beamCnv(0)
  , m_evtNr(0)
  , m_lasStatus(0)
  , m_checkOn(0)
  , m_checkOff(0)
  , m_check0(0)
  , m_check1(0)
  , m_check2(0)
  , m_Alarm(0)
  , m_Error(0)
  , m_ShOpen(0)
  , m_ShClose(0)
  , m_evTime(0)
  , m_trigType(0)
  , m_prevTrig(0)
  , m_timeBegin(0)
  , m_timeStart(0)
  , m_timeLast(0)
  , m_calibMode(0)
  , m_spillPattern(0)
  , m_nSpill(0)
  , m_nEvt()
  , m_cisBeg()
  , m_cisPar()
{
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");    
  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");

  declareProperty("PrintAllEvents", m_printAllEvents = false);
  declareProperty("DetectDummyFragments", m_detectDummyFragments = false);

  m_runNo = m_evtMin =  m_evtMax = m_evtBegin = m_evtNo = 0;

  
}


TileTBStat::~TileTBStat() {
}

// Alg standard interface function
StatusCode TileTBStat::initialize() {

  ATH_MSG_INFO( "Initialization" );

  m_checkOn = 0;
  m_checkOff = 0;
  m_check0 = 0;
  m_check1 = 0;
  m_check2 = 0;
  
  m_lasStatus = 0;


  CHECK(  m_beamInfo.retrieve() );
  CHECK( m_beamInfo->setProperty("TileBeamElemContainer", m_beamElemContainer) );
  CHECK( m_beamInfo->setProperty("TileDigitsContainer", m_digitsContainer) );

  // start with event 0
  m_evtNr = 0;
  m_runNo = m_evTime = m_evtNo = m_trigType = m_nSpill = m_timeBegin = m_timeLast = m_evtMax = 0;
  m_calibMode = m_prevTrig = m_spillPattern = m_evtMin = 0xFFFFFFFF;
  memset(m_nEvt, 0, sizeof(m_nEvt));
  memset(m_cisBeg, 0, sizeof(m_cisBeg));
  memset(m_cisPar, 0, sizeof(m_cisPar));
  memset(m_nEventsPerTrigger, 0, sizeof(m_nEventsPerTrigger));

  m_timeStart = time(0);
  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
} 

StatusCode TileTBStat::execute() {


  static bool first=true;
  static bool firstORsecond=true;
  
  if ( first ) {

    ServiceHandle<IConversionSvc> cnvSvc("ByteStreamCnvSvc", name());
    CHECK( cnvSvc.retrieve() );
  
    m_beamCnv = dynamic_cast<TileBeamElemContByteStreamCnv *> ( cnvSvc->converter( ClassID_traits<TileBeamElemContainer>::ID() ) );
  
    if (m_beamCnv == NULL ) {
      ATH_MSG_ERROR( " Can't get TileBeamElemContByteStreamCnv " );
      return StatusCode::FAILURE;
    }

    CHECK( m_RobSvc.retrieve() );

  }
  
  if ( firstORsecond ) {
    const eformat::FullEventFragment<const uint32_t*> * event = m_RobSvc->getEvent();
    if( true /* event->check_tree() */) { // valid event
      if ( ! first ) firstORsecond = m_printAllEvents;
      first=false;
      m_fragMap.clear();

      uint32_t nrob = event->nchildren();

      for (size_t irob=0; irob<nrob; ++irob) {
        const uint32_t* fprob;
        event->child(fprob, irob);
        const eformat::ROBFragment<const uint32_t*> robf(fprob);

        T_RobRodFragMap ROBfrag;
        ROBfrag.ROBid = robf.source_id();
        ROBfrag.RODid = robf.rod_source_id();

        //unsigned int version     = robf.rod_version();
        unsigned int source_id   = robf.rod_source_id();
        eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(source_id);
        unsigned int subdet_id   = id.subdetector_id();
        //unsigned int module_id   = id.module_id();

        unsigned int max_allowed_size = robf.rod_fragment_size_word();
        unsigned int delta = robf.rod_header_size_word() + robf.rod_trailer_size_word();
        if (max_allowed_size > delta) {
          max_allowed_size -= delta;
        } else {
          max_allowed_size = 0;
        }
            
        unsigned int size = robf.rod_ndata();
        if (size > max_allowed_size) {

          if (size - robf.rod_trailer_size_word() < max_allowed_size) {
            ATH_MSG_ERROR( "Problem with data size - assuming that trailer size is " << robf.rod_trailer_size_word()-(size-max_allowed_size)
                           <<" words instead of " << robf.rod_trailer_size_word() << " and data size is " << size << " words " );
            max_allowed_size = size;
          } else if (size - robf.rod_trailer_size_word() == max_allowed_size) {
            ATH_MSG_ERROR( "Problem with data size - assuming that trailer is absent "
                           << " ROD size " << robf.rod_fragment_size_word()
                           << " header size " << robf.rod_header_size_word()
                           << " data size " << size );
            max_allowed_size = size;
          } else {
            max_allowed_size += robf.rod_trailer_size_word();
            size = max_allowed_size;
            ATH_MSG_ERROR( "Problem with data size - assuming " << size << " words and no trailer at all" );
          }
        }

        if ( size > 0 ) {

          const uint32_t * data;
          robf.rod_data(data);

          switch ( subdet_id ) {
          case TILE_BEAM_ID:   // TILE BEAM ROD
          case COMMON_BEAM_ID: // COMMON BEAM ROD
          case 0x63: // wrong id in first test runs
          case TILE_POS_ID: // POSITIVE ROD
          case TILE_NEG_ID: // NEGATIVE ROD
          case TILE_EBPOS_ID: // POSITIVE EB ROD
          case TILE_EBNEG_ID: // NEGATIVE EB ROD
            find_frag(data, size, ROBfrag);
            break;
          default:
            break;
          }
        }
        m_fragMap.push_back(ROBfrag);
      }

      if (m_printAllEvents)
        std::cout << "Fragments found in event " << m_beamCnv->eventFragment()->global_id() << "  calib mode=" << m_beamInfo->checkCalibMode() << std::endl;
      else if (firstORsecond)
        std::cout << "Fragments found in first event, calib mode=" << m_beamInfo->checkCalibMode() << std::endl;
      else
        std::cout << "Fragments found in second event, calib mode=" << m_beamInfo->checkCalibMode() << std::endl;
      std::cout << "  ROB ID   ROD ID   Frag IDs" << std::endl;
      for (unsigned int i = 0; i < nrob; ++i) {
        std::cout << std::hex << " 0x" << m_fragMap[i].ROBid << " 0x" << m_fragMap[i].RODid;
        for (unsigned int j = 0; j < m_fragMap[i].fragID.size(); ++j)
          std::cout << std::hex << " 0x" << m_fragMap[i].fragID[j];
        std::cout << std::dec << std::endl;
      }
    }
  }
  
  // take values from event header

  memcpy(m_cisPar,m_beamInfo->cispar(), sizeof (m_cisPar));
  
  unsigned int testsum=0;
  for (int k = 4; k < 16; ++k) testsum += m_cisPar[k];
  if ( testsum == 0 && m_cisPar[3] == 8 ) { // broken CIS par
    m_cisSamples.addValue(m_cisPar[1]);
    m_cisI3Delay.addValue(m_cisPar[2]);
  } else {
    m_cisMode.addValue(m_cisPar[0]);
    m_cisSamples.addValue(m_cisPar[1]);
    m_cisPipeline.addValue(m_cisPar[2]);
    m_cisI3Delay.addValue(m_cisPar[3]);
    m_cisEvent.addValue(m_cisPar[4]);
    m_cisPhase.addValue(m_cisPar[5]);
    m_cisDAC.addValue(m_cisPar[6]);
    m_cisCap.addValue(m_cisPar[7]);
    m_cisCard.addValue(m_cisPar[8]);
  }
  
  const eformat::FullEventFragment<const uint32_t*> * event = m_RobSvc->getEvent();

  m_evTime = event->bc_time_seconds();
  if (m_evTime >= m_timeStart || m_evTime < 1054321000 ) {
    if (m_evtNr == 0)
      ATH_MSG_WARNING( "bad time in event header (" << m_evTime << "), taking time from cispar (" << m_cisPar[10] << ")" );

    m_evTime = m_cisPar[10];
  }
  
  m_trigType = m_beamInfo->trigType();
  if (m_trigType < 1 || m_trigType > 0x80 ) {
    if (m_evtNr == 0)
      ATH_MSG_WARNING( "no event trig type available (" << m_trigType << ") , taking trig from cispar (" << m_cisPar[12] << ")" );

    m_trigType = m_cisPar[12];
  }
  
  m_evtNo = event->global_id();
  if (m_evtNo < m_evtMin) m_evtMin = m_evtNo;
  if (m_evtNo > m_evtMax) m_evtMax = m_evtNo;
  
  if (m_evtNr == 0) {
    if (m_beamCnv->robFragment()) 
      m_runNo = m_beamCnv->robFragment()->rod_run_no();   // take it from beam ROD header
    else
      m_runNo = m_beamCnv->eventFragment()->run_no();  // run_no sometimes is not filled here

    m_timeBegin = m_evTime;
    m_evtBegin  = m_evtNo;
    m_calibMode = m_beamInfo->calibMode();
    memcpy(m_cisBeg,m_cisPar,sizeof(m_cisBeg));
  } else if (m_evtNr == 1) {
    // once again, first event can be junk
    m_calibMode = m_beamInfo->calibMode();
  }

  if ( ( m_evTime < m_timeBegin && (m_timeBegin - m_evTime) < 10*24*3600 ) ||  // less then 10 days shift in neg direction
       ( m_evTime > m_timeBegin && (m_evTime - m_timeBegin) > 10*24*3600 ) ) { // more than 10 days shift in pos direction

    ATH_MSG_WARNING( " in evt " << m_evtNo << " ( " << m_evtNr << " ) "
                     << " event header time " << event->bc_time_seconds()
                     << " CISpar time " << m_cisPar[10]
                     << " old begin time " <<  m_timeBegin );

    if ( m_cisPar[10] > 0 && m_cisPar[10] - m_evTime > 600 ) { // more than 10 min difference !
      ATH_MSG_WARNING( "Strange time in event header, ignore it! " );

      m_evTime = m_cisPar[10];
    } else {
      ATH_MSG_WARNING( "Strange begin time, change it from " << m_timeBegin << " to " << m_evTime );

      m_timeBegin = m_evTime;
    }    
  }
  
  if ((m_evTime >= m_timeBegin) && ((m_evTime - m_timeBegin) < 10 * 24 * 3600)
      && (m_evTime > m_timeLast)) {
    m_timeLast = m_evTime;
  }
  
  int ind = 4;
  switch ( m_trigType ) {
  case  1: ind = 0; break;
  case  2: ind = 1; break;
  case  4: ind = 2; break;
  case  8: ind = 3; break;
  case 16: ind = 3; break;
  default: ind = 4; break;
  }
  ++(m_nEvt[ind]);

  unsigned int lvl1_trigger_type = event->lvl1_trigger_type();
  if (lvl1_trigger_type < 256U) ++m_nEventsPerTrigger[lvl1_trigger_type];
  else ++m_nEventsPerTrigger[256];

  m_spillPattern <<= 4;
  m_spillPattern |= (m_trigType & 0xF);

  if (m_spillPattern != m_prevTrig && m_spillPattern == 0x11111111) 
    ++m_nSpill;
  
  m_prevTrig = m_spillPattern;

  ++m_evtNr;

  
  const TileLaserObject* pTileLasObj = 0;

  if(evtStore()->retrieve(pTileLasObj, "TileLaserObj").isFailure()) {
    ATH_MSG_ERROR( "There is a problem opening the LASER object" );

    //return sc;
  } else {
    m_lasFiltNum.addValue(pTileLasObj->getFiltNumber());
    m_lasAmp.addValue((int)pTileLasObj->getDiodeCurrOrd());
    m_laserBoxTemp.addValue(pTileLasObj->getDiodeBoxTemp());
    m_laserDiodeTemp.addValue(pTileLasObj->getPumpDiodeTemp());
    m_laserBoxHum.addValue(pTileLasObj->getHumidity());
    m_gasFlow.addValue(pTileLasObj->getGasFlux());
    m_lasStatus = (pTileLasObj->getAlarm() << 0x9) + (pTileLasObj->getInterlock() << 0x8) + (pTileLasObj->getShutter() << 0x6)
    + ((int)pTileLasObj->getHVpmts() << 0x4) + ((int)pTileLasObj->getLVdiodes() << 0x3) + (pTileLasObj->getAlphaPos());
    int lasAlpha = (int)pTileLasObj->getAlphaPos() & 0x7;
    m_lasAlphaPos.addValue(lasAlpha);
    m_checkOn = m_checkOn | m_lasStatus;
    m_checkOff = m_checkOff | (~m_lasStatus);
//    std::cout << std::endl << "Status: " << std::hex << m_lasStatus << std::dec
//              << std::endl << "Alpha: "  << lasAlpha << std::endl   << std::endl;
  }
  
  //log<<MSG::DEBUG<<"execute() completed successfully"<<endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TileTBStat::finalize() {

  ATH_MSG_INFO( "Finalization" );

  std::cout << std::endl << "Run number " << m_runNo  << std::endl;
  std::cout << "Min event number " << m_evtMin << std::endl;
  std::cout << "Max event number " << m_evtMax << std::endl;
  std::cout << "First event " << m_evtBegin << std::endl;
  std::cout << "Last  event " << m_evtNo << std::endl;

  // time stored in event header is time since 1-jan-1970 00:00:00 Geneva time
  // has to make one hour correction to convert to UNIX (UTC) time

  time_t tim;
  char ctim[50];
  char format[30] = "%a, %d %b %Y %H:%M:%S %Z (%z)";
  
  if (m_evTime >= m_timeStart || m_evTime == 0 ) {
    if (m_evtNr == 0)
      ATH_MSG_WARNING( "no event header available, taking time from cispar" );

    m_evTime = m_cisPar[10];
  }

  if (m_timeBegin != 0 && m_timeBegin < 1104537600 ) { // CTB 2004 time  
    std::cout << "Begin time " << m_timeBegin;
    tim = m_timeBegin - 3600; 
    std::cout << "-3600=" << tim;
    strftime(ctim, 50, format, localtime(&tim) );
    std::cout << "  " << ctim;
    strftime(ctim, 50, format, gmtime(&tim) );
    std::cout << "  " << ctim;
    std::cout << std::endl;

    std::cout << "End   time " << m_evTime;
    tim = m_evTime - 3600; 
    std::cout << "-3600=" << tim;
    strftime(ctim, 50, format, localtime(&tim) );
    std::cout << "  " << ctim;
    strftime(ctim, 50, format, gmtime(&tim) );
    std::cout << "  " << ctim;
    std::cout << std::endl;

  } else {

    std::cout << "Begin time " << m_timeBegin;
    tim = m_timeBegin; 
    strftime(ctim, 50, format, localtime(&tim) );
    std::cout << "  " << ctim;
    strftime(ctim, 50, format, gmtime(&tim) );
    std::cout << "  " << ctim;
    std::cout << std::endl;
    std::cout << "End   time " << m_timeLast;
    tim = m_timeLast; 
    strftime(ctim, 50, format, localtime(&tim) );
    std::cout << "  " << ctim;
    strftime(ctim, 50, format, gmtime(&tim) );
    std::cout << "  " << ctim;
    std::cout << std::endl;
  }
  
  std::cout << std::endl << "Ngains " << ( (m_calibMode == 1) ? 2 : 1 ) << std::endl;

  std::cout << "CISpar begin ";
  for (int i = 0; i < 16; ++i)
    std::cout << m_cisBeg[i] << "  ";
  std::cout << std::endl << "CISpar end   ";
  for (int i = 0; i < 16; ++i)
    std::cout << m_cisPar[i] << "  ";
  std::cout << std::endl << std::endl;
  
  std::cout << "Spills " << m_nSpill  << std::endl;
  std::cout << "Events " << m_evtNr   << std::endl;
  std::cout << "Phy    " << m_nEvt[0] << std::endl;
  std::cout << "Las    " << m_nEvt[1] << std::endl;
  std::cout << "Ped    " << m_nEvt[2] << std::endl;
  std::cout << "CIS    " << m_nEvt[3] << std::endl;
  std::cout << "Bad    " << m_nEvt[4] << std::endl;
  std::cout << std::endl << std::endl;

  std::cout << "Number of events per trigger:" << std::endl;
  for (int i = 0; i < 256; ++i) {
    if (m_nEventsPerTrigger[i] != 0) {
      std::cout << " Level1 trigger type: " << i << " (0x"<< std::hex << i << std::dec << ") => " <<  m_nEventsPerTrigger[i] << std::endl;
    }
  }

  if (m_nEventsPerTrigger[256] != 0) {
    std::cout << " Level1 trigger type: > 255(0xFF) => " <<  m_nEventsPerTrigger[256] << std::endl;
  }

  std::cout << std::endl << std::endl;

  std::cout << "LasFrag begin" << std::endl;
  m_lasFiltNum.print("FilterWheel");
  m_lasAmp.print("ReqAmp");
  std::cout << "Counter " << m_laserDiodeTemp.Counter << std::endl;
  m_laserDiodeTemp.print("LaserDiodeTemp");
  m_laserBoxTemp.print  ("LaserBoxTemp  ");
  m_laserBoxHum.print   ("LaserBoxHum   ");
  m_gasFlow.print       ("GasFlow       ");
  m_check0 = ~m_checkOn;
  m_check1 = ~m_checkOff;
  m_check2 = ~(m_check0 ^ m_check1);
  m_lasAlphaPos.print("LasAlpha");
  int bit = 0x800;
//  std::cout << "StatusHEX: " << std::hex << m_lasStatus << std::endl;
  std::cout << "GlobalStatus ";
  for (int i = 0; i < 9; i++) {
    if (m_check0 & bit)      std::cout << " 0"; 
    else if (m_check1 & bit) std::cout << " 1"; 
    else                     std::cout << " 2";
    bit = bit >> 1;
  }
  m_Alarm = 0;
  m_Error = 0;
  m_ShOpen = 0;
  m_ShClose = 0;
  bit = 0x200;
  if (m_check0 & bit) m_Error = 0; else
  if (m_check1 & bit) m_Error = 1; else
    m_Error = 2;
  std::cout << std::endl << "LasError " << m_Error;
  bit = 0x800;
  if (m_check0 & bit) m_Alarm = 0; else
  if (m_check1 & bit) m_Alarm = 1; else
    m_Alarm = 2;
  std::cout  << std::endl << "LasAlarm " << m_Alarm;
  bit = bit >> 4;
  if (m_check0 & bit) m_ShOpen = 0; else
  if (m_check1 & bit) m_ShOpen = 1; else
    m_ShOpen = 2;
  std::cout  << std::endl << "LasShOpen " << m_ShOpen;
  bit = bit >> 1;
  if (m_check0 & bit) m_ShClose = 0; else
  if (m_check1 & bit) m_ShClose = 1; else
    m_ShClose = 2;
  std::cout << std::endl << "LasShClosed " << m_ShClose;
  std::cout << std::endl << "LasFrag end" << std::endl << std::endl;
  
  std::cout << "CISparFrag begin" << std::endl;
  m_cisMode.print("CISparMode");
  m_cisSamples.print("CISparSamples");
  m_cisPipeline.print("CISparPipeline");
  m_cisI3Delay.print("CISparI3Delay");
  m_cisDAC.print("CISparDAC");
  m_cisCap.print("CISparCap");
  m_cisEvent.print("CISparEvent");
  m_cisPhase.print("CISparPhase");
  m_cisCard.print("CISparCard");
  std::cout << "CISparFrag end" << std::endl << std::endl;

  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

void TileTBStat::find_frag(const uint32_t * data, unsigned int size, T_RobRodFragMap & ROBfrag) {

  unsigned int offset = 0, sizeOverhead = 2;
  bool v3Format = ( *(data) == 0xff1234ff ); // additional frag marker since Sep 2005
  v3Format |= ( *(data) == 0x00123400 ); // another possible frag marker (can appear in buggy ROD frags)
  if ( v3Format ) {
    ++sizeOverhead;
    ++offset; // skip frag marker
  }
  
  while (offset < size ) {
    T_RodDataFrag* frag = (T_RodDataFrag *)(data + offset);
    if ( frag->size < sizeOverhead ) { // too small size, frag contains garbage
      break;
    }
    const uint32_t * fdata = frag->data;
    int n = frag->size - sizeOverhead - 1;
    for (; n > -1; --n) {
      if ( fdata[n]!=0 && fdata[n]!=0xffffffff) break;
    }
    if (n == -1 && m_detectDummyFragments) { // nothing reasonable found
      ROBfrag.fragID.push_back(frag->id + 0xff000000);
    } else {
      ROBfrag.fragID.push_back(frag->id);
    }
    offset += frag->size;
  }

  if ( v3Format ) --offset; // set offset back to correct value
  if ( offset > size && ROBfrag.fragID.size() > 0 ) { // wrong length in last frag, drop it
    ROBfrag.fragID.resize(ROBfrag.fragID.size()-1);
  }
}
