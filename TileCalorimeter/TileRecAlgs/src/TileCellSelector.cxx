/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileCellSelector.h"
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileDigits.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileInfo.h"

// Calo includes
#include "CaloIdentifier/TileID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "CxxUtils/StrFormat.h"

#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <sstream>
#include <iomanip>
#include <numeric>
#include <inttypes.h>

using xAOD::EventInfo;

static std::string drwname(int id) {
  static const char name[5][6] = { "", "LBA", "LBC", "EBA", "EBC" };
  return CxxUtils::strformat ("%s%2.2d", name[id >> 8], id % 0x100 + 1);
}

TileCellSelector::TileCellSelector(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_counter(0)
  , m_accept(0)
  , m_minCell(0)
  , m_maxCell(0)
  , m_minChan(0)
  , m_maxChan(0)
  , m_jump(0)
  , m_const(0)
  , m_overLG(0)
  , m_overHG(0)
  , m_underLG(0)
  , m_underHG(0)
  , m_dqerr(0)
  , m_dmuerr(0)
  , m_warnerr(0)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cabling(0)
  , m_runNum(0)
  , m_lumiBlock(0)
  , m_evtNum(0)
  , m_evtBCID(0)
  , m_tileFlag(0)
  , m_tileError(0)
  , m_readCells(true)
  , m_readRawChannels(true)
  , m_readDigits(true)
  , m_tileInfo(0)
{

  declareProperty( "MinEnergyCell", m_minEneCell = -5000.);   // cut on cell energy
  declareProperty( "MaxEnergyCell", m_maxEneCell = 1000000.); // cut on cell energy
  declareProperty( "PtnEnergyCell", m_ptnEneCell =      101); // cell energy pattern, accept events only below min (+1), between min-max (+10), above max (+100)
  declareProperty( "MinEnergyChan", m_minEneChan[0] =  -5000.);  // cut on channel energy
  declareProperty( "MaxEnergyChan", m_maxEneChan[0] = 500000.);  // cut on channel energy
  declareProperty( "PtnEnergyChan", m_ptnEneChan[0] =     101);  // channel energy pattern
  declareProperty( "MinEnergyGap",  m_minEneChan[1] = -10000.);  // cut on channel energy
  declareProperty( "MaxEnergyGap",  m_maxEneChan[1] = 500000.);  // cut on channel energy
  declareProperty( "PtnEnergyGap",  m_ptnEneChan[1] =     101);  // channel energy pattern
  declareProperty( "MinEnergyMBTS", m_minEneChan[2] = -10000.);  // cut on channel energy
  declareProperty( "MaxEnergyMBTS", m_maxEneChan[2] = 500000.);  // cut on channel energy
  declareProperty( "PtnEnergyMBTS", m_ptnEneChan[2] =     101);  // channel energy pattern

  declareProperty( "MinTimeCell", m_minTimeCell = -100.);  // cut on cell time
  declareProperty( "MaxTimeCell", m_maxTimeCell =  100.);  // cut on cell time
  declareProperty( "PtnTimeCell", m_ptnTimeCell =  10);  // cell time pattern, accept events only below min (+1), between min-max (+10), above max (+100)
  declareProperty( "MinTimeChan", m_minTimeChan[0] = -100.);  // cut on channel time
  declareProperty( "MaxTimeChan", m_maxTimeChan[0] =  100.);  // cut on channel time
  declareProperty( "PtnTimeChan", m_ptnTimeChan[0] =  10);  // channel time pattern
  declareProperty( "MinTimeGap",  m_minTimeChan[1] = -100.);  // cut on channel time
  declareProperty( "MaxTimeGap",  m_maxTimeChan[1] =  100.);  // cut on channel time
  declareProperty( "PtnTimeGap",  m_ptnTimeChan[1] =  10);  // channel time pattern
  declareProperty( "MinTimeMBTS", m_minTimeChan[2] = -100.);  // cut on channel time
  declareProperty( "MaxTimeMBTS", m_maxTimeChan[2] =  100.);  // cut on channel time
  declareProperty( "PtnTimeMBTS", m_ptnTimeChan[2] =  10);  // channel time pattern

  declareProperty( "SelectGain",  m_selectGain = 2); // 0 - select LG only,  1 - HG only, 2 - both gains
  m_skipGain[TileID::LOWGAIN] = false;
  m_skipGain[TileID::HIGHGAIN] = false;

  // pattern - decimal number with up to 5 digits
  // only values 1(=true) and 0(=false) for every digit are used
  // digit 0 set to 1  - accept event if value < min
  // digit 1 set to 1  - accept event if min < value < max
  // digit 2 set to 1  - accept event if value > max
  // digit 3 set to 1  - accept ene only if quality is good
  //                     or accept time if time != 0
  // digit 4 set to 1  - accept ene only if quality is bad
  //                     or accept time if time == 0

  declareProperty( "SecondMaxLevel",m_secondMaxLevel = 0.3);  // sample below max should be above (max-min)*m_secondMax
  declareProperty( "JumpDeltaHG",  m_jumpDeltaHG = 50.0); // minimal jump in high gain
  declareProperty( "JumpDeltaLG",  m_jumpDeltaLG = 10.0); // minimal jump in low gain
  declareProperty( "PedDetlaHG",   m_pedDeltaHG  =  4.1); // max variation of "const" value in high gain
  declareProperty( "PedDetlaLG",   m_pedDeltaLG  =  4.1); // max variation of "const" value in low gain
  declareProperty( "ConstLength",  m_constLength = 6);    // min number of consecutive samples of the same value
  declareProperty( "MinBadDMU",    m_minBadDMU = 4);      // min number of bad DMUs to accept event
  declareProperty( "MaxBadDMU",    m_maxBadDMU = 15);     // max number of bad DMUs to accept event
  declareProperty( "MinBadMB",     m_minBadMB = 4);       // min number of bad motherboards in a drawer to accept event
  declareProperty( "SkipEmpty",    m_skipEmpty = true);   // ignore empty channels in selection or not
  declareProperty( "SkipMasked",   m_skipMasked = true);  // ignore masked channels in selection or not
  declareProperty( "SkipMBTS",     m_skipMBTS = true);    // ignore MBTS channels in selection or not
  declareProperty( "CheckDCS",     m_checkDCS = true);    // additional check for DCS status
  declareProperty( "DrawerToDump", m_drawer);             // for which drawer all channels should be printed
  declareProperty( "DrawerToCheck",m_drawerToCheck);      // for which drawer all checks should be performed
  declareProperty( "ChannelToCheck",m_chanToCheck);       // for which channels all checks should be performed

  declareProperty( "CheckJumps",   m_checkJumps = true);   // global flag which allows to swithc on/off all checks in digits
  declareProperty( "CheckDMUs",    m_checkDMUs  = true);   // global flag which allows to swithc on/off DMU checks
  declareProperty( "CheckOverLG"  ,m_checkOverLG = true);  // select events with overflow in low gain
  declareProperty( "CheckOverHG",  m_checkOverHG = false); // select events with overflow in high gain
  declareProperty( "CheckUnderLG", m_checkUnderLG = false); // select events with underflow in low gain
  declareProperty( "CheckUnderHG", m_checkUnderHG = false); // select events with underflow in high gain
  declareProperty( "OverflowLG",   m_overflowLG  = -0.1);   // threshold for overflow in low gain  (smaller than ADCmax by this value)
  declareProperty( "OverflowHG",   m_overflowHG  = -1.1);   // threshold for overflow in high gain (smaller than ADCmax by this value)
  declareProperty( "UnderflowLG",  m_underflowLG = 0.1);    // threshold for underflow in low gain
  declareProperty( "UnderflowHG",  m_underflowHG = 2.1);    // threshold for underflow in high gain

  declareProperty( "CheckWarning", m_checkWarning = false); // select events with warning status in TileCal status word
  declareProperty( "CheckError",   m_checkError = false);   // select events with error status in TileCal status word
  declareProperty( "PrintOnly",    m_printOnly = false);    // only print acccepted events, but do not accept anything

  declareProperty( "MaxVerboseCnt",m_maxVerboseCnt=20); // max number of verbose output lines about drawer off

  declareProperty("TileInfoName", m_infoName = "TileInfo");
}


TileCellSelector::~TileCellSelector() {
}




StatusCode TileCellSelector::initialize() {
  //ATH_MSG_DEBUG("in initialize()");

  ATH_CHECK( m_eventInfoKey.initialize() );

  ATH_CHECK(detStore()->retrieve(m_tileID, "TileID"));
  ATH_CHECK(detStore()->retrieve(m_tileHWID, "TileHWID"));

  ATH_CHECK(detStore()->retrieve(m_tileInfo, m_infoName));
  m_ADCmaxMinusEps = m_tileInfo->ADCmax() - 0.01;
  m_ADCmaskValueMinusEps = m_tileInfo->ADCmaskValue() - 0.01;  // indicates channels which were masked in background dataset

  m_cabling = TileCablingService::getInstance();

  ATH_CHECK(m_tileBadChanTool.retrieve());

  if (m_checkDCS) {
    ATH_CHECK(m_tileDCS.retrieve());
  } else {
    m_tileDCS.disable();
  }

  ATH_MSG_INFO( "Cell container " 
                << ((m_cellContainerKey.key().empty()) ? "NOT SET" : m_cellContainerKey.key()) );
  ATH_MSG_INFO( "Digits container " 
                << ((m_digitsContainerKey.key().empty()) ? "NOT SET" : m_digitsContainerKey.key()));
  ATH_MSG_INFO( "RawChannel container " 
                << ((m_rawChannelContainerKey.key().empty()) ? "NOT SET" : m_rawChannelContainerKey.key()));

  ATH_MSG_INFO( "CheckJumps " << ((m_checkJumps) ? "true" : "false"));
  ATH_MSG_INFO( "CheckDMUs " << ((m_checkDMUs) ? "true" : "false"));
  ATH_MSG_INFO( "CheckOverLG " << ((m_checkOverLG) ? "true" : "false"));
  ATH_MSG_INFO( "CheckOverHG " << ((m_checkOverHG) ? "true" : "false"));
  ATH_MSG_INFO( "CheckUnderLG " << ((m_checkUnderLG) ? "true" : "false"));
  ATH_MSG_INFO( "CheckUnderHG " << ((m_checkUnderHG) ? "true" : "false"));

  if (m_overflowLG < 0) m_overflowLG += m_tileInfo->ADCmax();
  if (m_overflowHG < 0) m_overflowHG += m_tileInfo->ADCmax();
  ATH_MSG_INFO( "OverflowLG " << m_overflowLG);
  ATH_MSG_INFO( "OverflowHG " << m_overflowHG);
  ATH_MSG_INFO( "UnderflowLG " << m_underflowLG);
  ATH_MSG_INFO( "UnderflowHG " << m_underflowHG);

  ATH_MSG_INFO( "SkipEmpty " << ((m_skipEmpty) ? "true" : "false"));
  ATH_MSG_INFO( "SkipMasked " << ((m_skipMasked) ? "true" : "false"));
  ATH_MSG_INFO( "SkipMBTS " << ((m_skipMBTS) ? "true" : "false"));
  ATH_MSG_INFO( "CheckDCS " << ((m_checkDCS) ? "true" : "false"));


  m_readCells = !m_cellContainerKey.key().empty();
  m_readRawChannels = !m_rawChannelContainerKey.key().empty();
  m_readDigits = !m_digitsContainerKey.key().empty();

  if (m_readCells) {
    ATH_MSG_INFO( "MinEnergyCell < " << m_minEneCell);
    ATH_MSG_INFO( "MaxEnergyCell > " << m_maxEneCell);
    ATH_MSG_INFO( "PtnEnergyCell = " << m_ptnEneCell);
    ATH_MSG_INFO( "MinTimeCell < " << m_minTimeCell);
    ATH_MSG_INFO( "MaxTimeCell > " << m_maxTimeCell);
    ATH_MSG_INFO( "PtnTimeCell = " << m_ptnTimeCell);
    
    ATH_CHECK( m_cellContainerKey.initialize() );
  }

  if (m_readCells || m_readRawChannels) {
    ATH_MSG_INFO( "MinEnergyChan < " << m_minEneChan[0]);
    ATH_MSG_INFO( "MaxEnergyChan > " << m_maxEneChan[0]);
    ATH_MSG_INFO( "PtnEnergyChan = " << m_ptnEneChan[0]);
    ATH_MSG_INFO( "MinEnergyGap  < " << m_minEneChan[1]);
    ATH_MSG_INFO( "MaxEnergyGap  > " << m_maxEneChan[1]);
    ATH_MSG_INFO( "PtnEnergyGap  = " << m_ptnEneChan[1]);
    ATH_MSG_INFO( "MinTimeChan < " << m_minTimeChan[0]);
    ATH_MSG_INFO( "MaxTimeChan > " << m_maxTimeChan[0]);
    ATH_MSG_INFO( "PtnTimeChan = " << m_ptnTimeChan[0]);
    ATH_MSG_INFO( "MinTimeGap  < " << m_minTimeChan[1]);
    ATH_MSG_INFO( "MaxTimeGap  > " << m_maxTimeChan[1]);
    ATH_MSG_INFO( "PtnTimeGap  = " << m_ptnTimeChan[1]);
  }

  if (m_readRawChannels) {
    ATH_MSG_INFO( "MinEnergyMBTS < " << m_minEneChan[2]);
    ATH_MSG_INFO( "MaxEnergyMBTS > " << m_maxEneChan[2]);
    ATH_MSG_INFO( "PtnEnergyMBTS = " << m_ptnEneChan[2]);
    ATH_MSG_INFO( "MinTimeMBTS < " << m_minTimeChan[2]);
    ATH_MSG_INFO( "MaxTimeMBTS > " << m_maxTimeChan[2]);
    ATH_MSG_INFO( "PtnTimeMBTS = " << m_ptnTimeChan[2]);

    ATH_CHECK( m_rawChannelContainerKey.initialize() );
  }

  switch (m_selectGain) {
    case 0:
      ATH_MSG_INFO( "Select Low gain channels only");
      m_skipGain[TileID::LOWGAIN] = false;
      m_skipGain[TileID::HIGHGAIN] = true;
      break;
    case 1:
      ATH_MSG_INFO( "Select High gain channels only");
      m_skipGain[TileID::LOWGAIN] = true;
      m_skipGain[TileID::HIGHGAIN] = false;
      break;
    default:
      ATH_MSG_INFO( "Select both gains");
      break;
  }

  if (!m_digitsContainerKey.key().empty()) {
    if (m_checkJumps) {
      ATH_MSG_INFO( "JumpDeltaHG " << m_jumpDeltaHG);
      ATH_MSG_INFO( "JumpDeltaLG " << m_jumpDeltaLG);
      ATH_MSG_INFO( "PedDetlaHG " << m_pedDeltaHG);
      ATH_MSG_INFO( "PedDetlaLG " << m_pedDeltaLG);
      ATH_MSG_INFO( "ConstLength " << m_constLength);
    }

    ATH_CHECK( m_digitsContainerKey.initialize() );

  }

  if (!m_rawChannelContainerKey.key().empty()) {
    if (m_checkDMUs) {
      ATH_MSG_INFO( "MinBadDMU " << m_minBadDMU);
      ATH_MSG_INFO( "MaxBadDMU " << m_maxBadDMU);
      ATH_MSG_INFO( "MinBadMB " << m_minBadMB);
    } else {
      m_minBadDMU = 99;
      m_maxBadDMU = -1;
      m_minBadMB = 99;
    }
    ATH_MSG_INFO( "MaxVerboseCnt " << m_maxVerboseCnt);
  }

  ATH_MSG_INFO( "CheckWarning " << ((m_checkWarning)? "true" : "false"));
  ATH_MSG_INFO( "CheckError " << ((m_checkError) ? "true" : "false"));
  ATH_MSG_INFO( "PrintOnly " << ((m_printOnly) ? "true" : "false"));

  if (m_drawer.size()>0) {
    msg(MSG::INFO) << "Drawers which will be always printed:" << MSG::hex;
    for (int frag: m_drawer) msg(MSG::INFO) << " 0x" << frag;
    msg(MSG::INFO) << MSG::dec << endmsg;
  }

  if (m_drawerToCheck.size()>0) {
    msg(MSG::INFO) << "Only those drawers will be checked:" << MSG::hex;
    for (int frag: m_drawerToCheck) msg(MSG::INFO) << " 0x" << frag;
    msg(MSG::INFO) << MSG::dec << endmsg;
  }

  if (m_chanToCheck.size()>0) {
    msg(MSG::INFO) << "Only those channels will be checked:";
    for (int ch: m_chanToCheck) msg(MSG::INFO) << " " << ch;
    msg(MSG::INFO) << endmsg;
  }

  m_nDrawerOff.resize(TileCalibUtils::getDrawerIdx(4, 63) + 1);

  //ATH_MSG_DEBUG ("initialize() successful");

  // convert patterns fo bolean arrays

  int digit=1;
  int scale=10;
  for (int i=0; i<ptnlength; ++i) {
      
    int bit=(m_ptnEneCell/digit)%scale;
    m_bitEneCell[i] = (bit!=0);      

    bit=(m_ptnTimeCell/digit)%scale;
    m_bitTimeCell[i] = (bit!=0);      

    for (int j=0; j<3; ++j) {

      int bit=(m_ptnEneChan[j]/digit)%scale;
      m_bitEneChan[j][i] = (bit!=0);      

      bit=(m_ptnTimeChan[j]/digit)%scale;
      m_bitTimeChan[j][i] = (bit!=0);      
    }
    
    digit *= scale;
  }

  m_chanToSkip.clear();
  m_drawerToSkip.clear();
  if (m_drawerToCheck.size()>0 || m_chanToCheck.size()>0 ) {
    if (m_drawerToCheck.size()==0) {
      m_drawerToCheck.resize(256);
      auto itr = m_drawerToCheck.begin();
      for (int frag : {0x100,0x200,0x300,0x400}) {
        auto itr1 = itr+64;
        std::iota(itr, itr1, frag);
        itr = itr1;
      }
    } else if (m_chanToCheck.size()==0) {
      m_chanToCheck.resize(48);
      std::iota(m_chanToCheck.begin(), m_chanToCheck.end(), 0);
    }
    m_chanToSkip.resize(1+TileHWID::NOT_VALID_HASH,true);
    m_drawerToSkip.resize(1+TileCalibUtils::getDrawerIdx(4,63),true);
    IdContext chan_context = m_tileHWID->channel_context();
    IdentifierHash hash;
    for (int frag : m_drawerToCheck) {
      int ros = frag >> 8;
      int drawer = frag & 0x3F;
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
      m_drawerToSkip[drawerIdx] = false;
      HWIdentifier ch_id = m_tileHWID->channel_id(ros,drawer,0);
      m_tileHWID->get_hash(ch_id, hash, &chan_context);
      for (int chan: m_chanToCheck)
        m_chanToSkip[hash+chan] = false;
    }
  } else {
    m_chanToSkip.resize(1+TileHWID::NOT_VALID_HASH,false);
    m_chanToSkip[TileHWID::NOT_VALID_HASH] = true;
    m_drawerToSkip.resize(1+TileCalibUtils::getDrawerIdx(4,63),false);
  }

  ATH_CHECK( m_dqStatusKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TileCellSelector::execute() {
  //ATH_MSG_DEBUG ("execute()");

  const EventContext& ctx = Gaudi::Hive::currentContext();

  ++m_counter;

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);
  if ( eventInfo.isValid() ) {
    m_runNum = eventInfo->runNumber();
    m_lumiBlock = eventInfo->lumiBlock();
    m_evtNum = eventInfo->eventNumber();
    m_evtBCID = eventInfo->bcid();
    m_tileFlag = eventInfo->eventFlags(EventInfo::Tile);
    m_tileError = eventInfo->errorState(EventInfo::Tile);
  } else {
    m_runNum = 0;
    m_evtNum = 0;
    m_lumiBlock = 0;
    m_evtBCID = 0;
    m_tileFlag = 0;
    m_tileError = 0;
  }

  std::ostringstream evState;
  evState << "Run "<< std::setw(6) << m_runNum
          <<" LB "<< std::setw(4) << m_lumiBlock
          <<" Evt "<< std::setw(9) << m_evtNum
          <<" ErrState " << m_tileError
          <<" Flags 0x" << std::hex << m_tileFlag << std::dec;

  std::ostringstream evtnum;
  evtnum << "Run "<< std::setw(6) << m_runNum
         <<" LB "<< std::setw(4) << m_lumiBlock
         <<" Evt "<< std::setw(9) << m_evtNum
         <<" BCID "<< std::setw(4) << m_evtBCID;

  std::ostringstream nevtnum;
  nevtnum << evtnum.str()
          <<" nevt "<< std::setw(6) << m_counter;

  bool emptyBad = true;
  bool badFromCell = false;
  m_chanBad.clear();
  m_chanBad.resize(1+TileHWID::NOT_VALID_HASH,true);
  m_chanEne.clear();
  m_chanEne.resize(1+TileHWID::NOT_VALID_HASH,0.0);
  m_chanTime.clear();
  m_chanTime.resize(1+TileHWID::NOT_VALID_HASH,0.0);
  m_chanQua.clear();
  m_chanQua.resize(1+TileHWID::NOT_VALID_HASH,0.0);
  m_chanSel.clear();
  m_chanSel.resize(1+TileHWID::NOT_VALID_HASH,false);

  IdContext chan_context = m_tileHWID->channel_context();
  IdentifierHash hash;
  for (size_t i=0; i<m_drawer.size(); ++i) {
    HWIdentifier drawer_id = m_tileHWID->drawer_id(m_drawer[i]);
    HWIdentifier ch_id = m_tileHWID->channel_id(drawer_id,0);
    m_tileHWID->get_hash(ch_id, hash, &chan_context);
    auto itr = m_chanSel.begin() + hash;
    std::fill(itr,itr+48,true);
  }

  bool statusOk = (m_checkWarning && m_tileError == EventInfo::Warning) ||
                  (m_checkError   && m_tileError == EventInfo::Error);
  if (statusOk) {
    ++m_warnerr;

    using namespace boost::local_time;
    using namespace boost::posix_time;
    static time_zone_ptr gva_tz(new posix_time_zone((std::string)"CET+01CEST01:00:00,M3.5.0/02:00:00,M10.5.0/03:00:00"));
    local_date_time gva_time(from_time_t(eventInfo->timeStamp()),gva_tz);
    evState << " " << gva_time << " ";
   

    const char * part[5] = { "UNK", "LBA", "LBC", "EBA", "EBC" };
    if (m_tileError == EventInfo::Warning) {
      int dn = (m_tileFlag >> 16) & 0xF;
      int n1 = (m_tileFlag >> 20) & 0x3F;
      int n2 = (n1 + dn - 1) % 64;
      int rr = ((m_tileFlag >> 26) & 0x3) + 1;
      evState <<  " "  << part[rr] <<std::setw(2)<<std::setfill('0')<<n1+1
              << " - " << part[rr] <<std::setw(2)<<std::setfill('0')<<n2+1
              <<  " "  << dn << " consec bad ";
    }
    else if (m_tileError == EventInfo::Error) {
      int dn = (m_tileFlag >> 16) & 0xF;
      int p0 = m_tileFlag & 0xF; // sends any data above threshold
      int p1 = (m_tileFlag >> 20) & 0xF; // 16 drawers masked
      int p2 = (m_tileFlag >> 24) & 0xF; // 16 drawers off
      int pp = 1;
      for (int rr = 1; rr < 5; ++rr) {
        if ((p2 & pp) || (p1 & pp)) {
          evState << " " << part[rr];
          if (p2 & pp) {
            if (p0 & pp) evState << " off";
            else evState << " OFF";
          }
          if (p1 & pp) {
            if (p0 & pp) evState << " mask";
            else evState << " MASK";
          }
        }
        pp <<= 1;
      }
      evState <<  "  "  << dn << " consec bad ";
    }

    if (m_checkDCS) {
      int n1 = -1;
      int n2 = -1;
      int dn = 0;
      int rr = 0;
      int m1 = -1;
      int m2 = -1;
      int dm = -1;
      std::vector<int> allmod;
      std::vector<int> consec;
      for (int ros = 1; ros < 5; ++ros) {
        int drmax = 65;
        for (int dr = 0; dr < drmax; ++dr) {
          int drawer = dr % 64;
          if (m_tileDCS->getDCSStatus(ros, drawer) == TileDCSState::ALERT_DRAWER) {
            if (m1 < 0) m1 = dr;
            m2 = dr;
            if (dr < 64) allmod.push_back((ros << 8) + dr);
          } else if (m1 >= 0) {
            dm = m2 - m1 + 1;
            if (m1 == 0) drmax += dm;
            if (dm > dn) {
              n1 = m1;
              n2 = m2;
              dn = dm;
              rr = ros;
              consec.clear();
              consec.push_back((ros << 8) + m1);
            } else if (dm == dn) {
              if (m1 < 64) consec.push_back((ros << 8) + m1);
            }
            m1 = m2 = -1;
          }
        }
      }
      evState << " DCS " << allmod.size() << " off ";
      if (dn > 1) {
        evState << dn;
        if (consec.size() > 1) evState << "*" << (consec.size());
        evState << " consec "
                << part[rr] << std::setw(2) << std::setfill('0') << (n1 % 64) + 1 << " - "
                << part[rr] << std::setw(2) << std::setfill('0') << (n2 % 64) + 1 << " ";
        n1 += (rr << 8);
        n2 += (rr << 8);
        for (size_t n = 1; n < consec.size(); ++n) {
          m1 = consec[n];
          m2 = m1 + dn - 1;
          evState << part[m1 >> 8] << std::setw(2) << std::setfill('0') << (m1 % 64) + 1 << " - "
                  << part[m2 >> 8] << std::setw(2) << std::setfill('0') << (m2 % 64) + 1 << " ";
          for (size_t m = 0; m < allmod.size(); ++m) {
            int mm = allmod[m];
            if (mm >= m1 && mm <= m2) {
              allmod[m] += n1 - m1;
            }
          }
        }
      } else {
        n1 = n2 = dn = 0;
      }
      if (allmod.size() > (size_t) dn) {
        for (size_t m = 0; m < allmod.size(); ++m) {
          int mm = allmod[m];
          if (!(mm >= n1 && mm <= n2)) {
            evState << part[mm >> 8] << std::setw(2) << std::setfill('0') << (mm % 64) + 1 << " ";
          }
        }
      }
    }

    ATH_MSG_DEBUG (evState.str() << " accepted");
  }

  int rawdata = -1;
  const TileCell* cellminCh = 0;
  const TileCell* cellmaxCh = 0;
  const TileCell* tcellminCh = 0;
  const TileCell* tcellmaxCh = 0;

  if (m_readCells) {

    // Get Calo cell container
    SG::ReadHandle<CaloCellContainer> cellContainer(m_cellContainerKey);

    if (!cellContainer.isValid()) {
      
      ATH_MSG_WARNING("Unable to read CaloCellContainer from EventStore, disable reading of this container");
      m_readCells = false;

    } else {

      float emin = 0.;
      float emax = 0.;
      float tmin = 0.;
      float tmax = 0.;
      float chmin = 0.;
      float chmax = 0.;
      float tcmin = 0.;
      float tcmax = 0.;
      const TileCell* cellmin = 0;
      const TileCell* cellmax = 0;
      const TileCell* tcellmin = 0;
      const TileCell* tcellmax = 0;

      // special case - check overflow here if digits container is not available
      // should be careful here, because in TileCell overflow bit is set to 1
      // both for overflow and underflow and underflow is HG are very often in gap cells
      // also, overflow in HG might be masked if quality is too bad, so we'll not select all overflows...
      // that's why only overflow in LG are checked
      bool checkOver = (m_checkOverLG && m_digitsContainerKey.key().empty());

      for (const CaloCell* cell : *cellContainer) {

        Identifier id = cell->ID();
        if ( m_tileID->is_tile(id) ) {
          const TileCell* tile_cell = dynamic_cast<const TileCell*> (cell);
          if (tile_cell==0) continue;
          const CaloDetDescrElement * caloDDE = cell->caloDDE();
          IdentifierHash hash1 = caloDDE->onl1();
          IdentifierHash hash2 = caloDDE->onl2();
          if ( m_chanToSkip[hash1] && m_chanToSkip[hash2] ) continue;
          int ch_type = (hash2 == TileHWID::NOT_VALID_HASH) ? 1 : 0;
          if (rawdata < 0) {
            rawdata = (tile_cell->qbit1() & TileCell::MASK_CMPC) ? 0 : 1;
          }

          bool  bad1 = tile_cell->badch1();
          bool  bad2 = tile_cell->badch2();
          float ene1 = tile_cell->ene1();
          float ene2 = tile_cell->ene2();
          float time1 = tile_cell->time1();
          float time2 = tile_cell->time2();
          m_chanBad[hash1] = bad1;
          m_chanBad[hash2] = bad2;
          m_chanEne[hash1] = ene1;
          m_chanEne[hash2] = ene2;
          m_chanTime[hash1] = time1;
          m_chanTime[hash2] = time2;
          m_chanQua[hash1] = tile_cell->qual1();
          m_chanQua[hash2] = tile_cell->qual2();

          float ene  = tile_cell->energy();
          bool eneOk = false;
          if (ene < m_minEneCell) {
            eneOk = m_bitEneCell[0];
          } else if (ene > m_maxEneCell) {
            eneOk = m_bitEneCell[2];
          } else {
            eneOk = m_bitEneCell[1];
          }
          
          if (eneOk) {
            if (bad1 && bad2) {
              if (m_bitEneCell[3]) eneOk = false; // request good cells only, but cell is bad
            } else {
              if (m_bitEneCell[4]) eneOk = false; // request bad cells only, but cell is good
            }
          }

          float time  = tile_cell->time();
          bool timeOk = false;
          if (time < m_minTimeCell) {
            timeOk = m_bitTimeCell[0];
          } else if (time > m_maxTimeCell ) {
            timeOk = m_bitTimeCell[2];
          } else {
            timeOk = m_bitTimeCell[1];
          }

          if (timeOk) {
            if (time != 0.) {
              if (m_bitTimeCell[4]) timeOk = false; // request time==0 only, but time!=0
            } else {
              if (m_bitTimeCell[3]) timeOk = false; // request time!=0 only, but time==0
            }
          }

          if (timeOk && eneOk) {

            ATH_MSG_VERBOSE( evtnum.str()
                << " cell " << std::left << std::setw(14) << m_tileID->to_string(id,-2)
                << " ene = " << ene << " time = " << time);

            m_chanSel[hash1] = true;
            m_chanSel[hash2] = true;

            if (ene < emin) {
              emin = ene;
              cellmin = tile_cell;
            } else if (ene > emax) {
              emax = ene;
              cellmax = tile_cell;
            }

            if (time<tmin) {
              tmin = time;
              tcellmin = tile_cell;
            }
            else if (time>tmax) {
              tmax = time;
              tcellmax = tile_cell;
            }
          }
          
          if ( !(bad1 && bad2) ) {

            bool ene1Ok = false;
            bool time1Ok = false;

            if ( !(bad1 || m_skipGain[tile_cell->gain1()]) ) {
              if (time1 < m_minTimeChan[ch_type] ) {
                time1Ok = m_bitTimeChan[ch_type][0];
              } else if (time1 > m_maxTimeChan[ch_type] ) {
                time1Ok = m_bitTimeChan[ch_type][2];
              } else {
                time1Ok = m_bitTimeChan[ch_type][1];
              }

              if (ene1 < m_minEneChan[ch_type] ) {
                ene1Ok = m_bitEneChan[ch_type][0];
              } else if (ene1 > m_maxEneChan[ch_type] ) {
                ene1Ok = m_bitEneChan[ch_type][2];
              } else {
                ene1Ok = m_bitEneChan[ch_type][1];
              }

              if (ene1Ok) {
                if (m_bitEneChan[ch_type][4]) ene1Ok = false; // request bad chan only, but chan is good
              }

              if (time1Ok) {
                if (time1 != 0.) {
                  if (m_bitTimeChan[ch_type][4]) time1Ok = false; // request time==0 only, but time!=0
                } else {
                  if (m_bitTimeChan[ch_type][3]) time1Ok = false; // request time!=0 only, but time==0
                }
              }
            }
            
            bool ene2Ok = false;
            bool time2Ok = false;

            if ( !(bad2 || m_skipGain[tile_cell->gain2()]) ) {
              if (ene2 < m_minEneChan[ch_type] ) {
                ene2Ok = m_bitEneChan[ch_type][0];
              } else if (ene2 > m_maxEneChan[ch_type] ) {
                ene2Ok = m_bitEneChan[ch_type][2];
              } else {
                ene2Ok = m_bitEneChan[ch_type][1];
              }

              if (time2 < m_minTimeChan[ch_type] ) {
                time2Ok = m_bitTimeChan[ch_type][0];
              } else if (time2 > m_maxTimeChan[ch_type] ) {
                time2Ok = m_bitTimeChan[ch_type][2];
              } else {
                time2Ok = m_bitTimeChan[ch_type][1];
              }

              if (ene2Ok) {
                if (m_bitEneChan[ch_type][4]) ene2Ok = false; // request bad chan only, but chan is good
              }

              if (time2Ok) {
                if (time2 != 0.) {
                  if (m_bitTimeChan[ch_type][4]) time2Ok = false; // request time==0 only, but time!=0
                } else {
                  if (m_bitTimeChan[ch_type][3]) time2Ok = false; // request time!=0 only, but time==0
                }
              }
            }

            bool over1=false;
            bool over2=false;
            if (checkOver) {
              over1 = ( (!bad1) && (tile_cell->qbit1() & TileCell::MASK_OVER) && tile_cell->gain1()==TileID::LOWGAIN);
              over2 = ( (!bad2) && (tile_cell->qbit2() & TileCell::MASK_OVER) && tile_cell->gain2()==TileID::LOWGAIN);
            }
            
            if ((ene1Ok && time1Ok) || over1) {

              ATH_MSG_VERBOSE( evtnum.str()
                  << " cell " << std::left << std::setw(14) << m_tileID->to_string(id,-2)
                  << " ch_ene1 = " << ene1 << " ch_t1 = " << time1
                  << ((over1)?" overflow":""));

              m_chanSel[hash1] = true;
              m_chanSel[hash2] = true;

              if (ene1 < chmin) {
                chmin = ene1;
                cellminCh = tile_cell;
              } else if (ene1 > chmax) {
                chmax = ene1;
                cellmaxCh = tile_cell;
              }

              if (time1 < tcmin) {
                tcmin = time1;
                tcellminCh = tile_cell;
              } else if (time1 > tcmax) {
                tcmax = time1;
                tcellmaxCh = tile_cell;
              }
            }
            
            if ((ene2Ok && time2Ok) || over2) {

              ATH_MSG_VERBOSE( evtnum.str()
                  << " cell " << std::left << std::setw(14) << m_tileID->to_string(id,-2)
                  << " ch_ene2 = " << ene2 << " ch_t2 = " << time2
                  << ((over2)?" overflow":""));

              m_chanSel[hash1] = true;
              m_chanSel[hash2] = true;

              if (ene2 < chmin) {
                chmin = ene2;
                cellminCh = tile_cell;
              } else if (ene2 > chmax) {
                chmax = ene2;
                cellmaxCh = tile_cell;
              }

              if (time2 < tcmin) {
                tcmin = time2;
                tcellminCh = tile_cell;
              } else if (time2 > tcmax) {
                tcmax = time2;
                tcellmaxCh = tile_cell;
              }
            }

          }
        }
      }

      if (tcellmin && tcellmin != cellmin && tcellmin != cellmax) {
        ATH_MSG_DEBUG( nevtnum.str()
            << " cell " << std::left << std::setw(14) << m_tileID->to_string(tcellmin->ID(),-2)
            << " ene = " << tcellmin->energy()
            << " tmin = " << tcellmin->time());
      }
      if (tcellmax && tcellmax != cellmin  && tcellmax != cellmax) {
        ATH_MSG_DEBUG( nevtnum.str()
            << " cell " << std::left << std::setw(14) << m_tileID->to_string(tcellmax->ID(),-2)
            << " ene = " << tcellmax->energy()
            << " tmax = " << tcellmax->energy());
      }

      if (tcellminCh && tcellminCh != cellminCh && tcellminCh != cellmaxCh) {
        ATH_MSG_DEBUG( nevtnum.str()
            << " cell " << std::left << std::setw(14) << m_tileID->to_string(tcellminCh->ID(),-2)
            << " ch_ene = " << tcellminCh->ene1() << " " << tcellminCh->ene2()
            << " ch_tmin = " << tcellminCh->time1() << " " << tcellminCh->time2());
      }
      if (tcellmaxCh && tcellmaxCh != cellminCh && tcellmaxCh != cellmaxCh) {
        ATH_MSG_DEBUG( nevtnum.str()
            << " cell " << std::left << std::setw(14) << m_tileID->to_string(tcellmaxCh->ID(),-2)
            << " ch_ene = " << tcellmaxCh->ene1() << " " << tcellmaxCh->ene2()
            << " ch_tmax = " << tcellmaxCh->time1() << " " << tcellmaxCh->time2());
      }
      
      if (cellmin) {
        ++m_minCell;
        statusOk = true;
        const char * tit = (tcellmin == cellmin) ? " tmin = ": ((tcellmax == cellmin) ? " tmax = ": " t = ");
        if (cellminCh!=cellmin) {
          ATH_MSG_DEBUG( nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellmin->ID(),-2)
              << " emin = " << emin 
              << tit << cellmin->time()
              << " accepted");

        } else {
          ATH_MSG_DEBUG( nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellmin->ID(),-2)
              << " emin = " << emin
              << " ch_emin = " << chmin
              << tit << cellmin->time()
              << " accepted");
        }
      }
      
      if (cellminCh) {
        ++m_minChan;
        statusOk = true;
        const char * tit = (tcellminCh == cellminCh) ? " tmin = ": ((tcellmaxCh == cellminCh) ? " tmax = ": " t = ");
        if (cellminCh!=cellmin) {
          ATH_MSG_DEBUG( nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellminCh->ID(),-2)
              << " ch_emin = " << chmin
              << tit << cellminCh->time()
              << " accepted");
        }
      }

      if (cellmax) {
        ++m_maxCell;
        statusOk = true;
        const char * tit = (tcellmin == cellmax) ? " tmin = ": ((tcellmax == cellmax) ? " tmax = ": " t = ");
        if (cellmaxCh!=cellmax) {
          ATH_MSG_DEBUG(  nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellmax->ID(),-2)
              << " emax = " << emax
              << tit << cellmax->time()
              << " accepted");

        } else {
          ATH_MSG_DEBUG( nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellmax->ID(),-2)
              << " emax = " << emax
              << " ch_emax = " << chmax
              << tit << cellmax->time()
              << " accepted");
        }
      }

      if (cellmaxCh) {
        ++m_maxChan;
        statusOk = true;
        const char * tit = (tcellminCh == cellmaxCh) ? " tmin = ": ((tcellmaxCh == cellmaxCh) ? " tmax = ": " t = ");
        if (cellmaxCh!=cellmax) {
          ATH_MSG_DEBUG( nevtnum.str()
              << " cell " << std::left << std::setw(14) << m_tileID->to_string(cellmaxCh->ID(),-2)
              << " ch_emax = " << chmax
              << tit << cellmaxCh->time()
              << " accepted");
        }
      }

      emptyBad = false;
      badFromCell = true;
    }
  }

  const TileDQstatus* DQstatus(0);

  if (m_readRawChannels) {

    // Get Tile RawChannel container
    SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey);

    if ( !rawChannelContainer.isValid() ) {
      ATH_MSG_WARNING("Unable to read TileRawChannelContainer from EventStore, disable reading of this container");
      m_readRawChannels = false;

    } else {

      float chmin = 0; // m_minEneChan[0];
      float chmax = 0; // m_maxEneChan[0];
      float tcmin = 0.;
      float tcmax = 0.;
      const TileRawChannel* minCh = 0;
      const TileRawChannel* maxCh = 0;
      const TileRawChannel* tminCh = 0;
      const TileRawChannel* tmaxCh = 0;
      TileRawChannelUnit::UNIT rChUnit = rawChannelContainer->get_unit();
      bool allowAmpCheck = ( ( rChUnit == TileRawChannelUnit::MegaElectronVolts || // allow MeV only as units
                               rChUnit == TileRawChannelUnit::OnlineMegaElectronVolts ) ) ;
      bool fillChanEne = ( !m_readCells && allowAmpCheck ); // use amplitude from channel if cell container was not checked
      if (!fillChanEne) {
        m_chanDsp.clear();
        m_chanDsp.resize(1+TileHWID::NOT_VALID_HASH,0.0);
        m_chanTDsp.clear();
        m_chanTDsp.resize(1+TileHWID::NOT_VALID_HASH,0.0);
      }

      if (rChUnit >= TileRawChannelUnit::OnlineADCcounts)
        DQstatus = SG::makeHandle (m_dqStatusKey, ctx).get();
      else
        rawdata = 0;

      IdContext chan_context = m_tileHWID->channel_context();
      IdentifierHash hash;
      int index=0, pmt;

      int nbadMax = 0;
      int nbadMBMax = 0;
      const TileRawChannelCollection * collMax = 0;
      const TileRawChannelCollection * collMBMax = 0;

      bool someDQerrors = false;

      for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

        int frag = rawChannelCollection->identify();
        bool eb = (frag > 0x2ff);
        bool ebsp = (frag == 0x30e || frag == 0x411);

        int ros = frag >> 8;
        int drawer = frag & 0x3F;
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
        if ( m_drawerToSkip[drawerIdx] ) continue;

        int chMBTS = -1;
        if (eb) {
          for (int ch: {12,4,0}) {
            m_cabling->h2s_cell_id_index(ros,drawer,ch,index,pmt);
            if (index == -2) {
              chMBTS = ch;
              break;
            }
          }
        }

        HWIdentifier ch_id = m_tileHWID->channel_id(ros,drawer,0);
        m_tileHWID->get_hash(ch_id, hash, &chan_context);
        int hashNext = index = (int)hash + 48; // hash ID of the channel after current drawer

        // all error words contains information in last 16 bits only
        // but they are stored in collection as 32 bit numbers
        uint32_t RODBCID         = rawChannelCollection->getRODBCID();
        uint32_t DSPBCID         = rawChannelCollection->getFragDSPBCID();
        uint32_t GlobalCRCErr    = rawChannelCollection->getFragGlobalCRC() & 0x1;
        uint32_t FE_DMUmask      = rawChannelCollection->getFragFEChipMask();
        uint32_t ROD_DMUmask     = rawChannelCollection->getFragRODChipMask();
        uint32_t BCIDErr         = rawChannelCollection->getFragBCID();
        uint32_t MemoryParityErr = rawChannelCollection->getFragMemoryPar();
        uint32_t HeaderFormatErr = rawChannelCollection->getFragHeaderBit();
        uint32_t HeaderParityErr = rawChannelCollection->getFragHeaderPar();
        uint32_t SampleFormatErr = rawChannelCollection->getFragSampleBit();
        uint32_t SampleParityErr = rawChannelCollection->getFragSamplePar();
        uint32_t SingleStrobeErr = rawChannelCollection->getFragSstrobe();
        uint32_t DoubleStrobeErr = rawChannelCollection->getFragDstrobe();

        if (RODBCID!=0 && RODBCID != m_evtBCID ) {
          if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
            ATH_MSG_VERBOSE(  evtnum.str()
                << "  drw " << drwname(rawChannelCollection->identify())
                << " ROD BCID " << RODBCID << " is wrong - skipping");

            if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
              ATH_MSG_VERBOSE( nevtnum.str()
                  << "   suppressing further messages about drawer 0x" << std::hex << rawChannelCollection->identify()
                  << std::dec << " being bad");
          }
          someDQerrors = true;
          continue;
        }

        if (DSPBCID >= 0x7FFF
            && GlobalCRCErr
            && FE_DMUmask == 0xFFFF
            && ROD_DMUmask == 0xFFFF
            && BCIDErr == 0xFFFF
            && MemoryParityErr == 0xFFFF
            && HeaderFormatErr == 0xFFFF
            && HeaderParityErr == 0xFFFF
            && SampleFormatErr == 0xFFFF
            && SampleParityErr == 0xFFFF
            && SingleStrobeErr == 0xFFFF
            && DoubleStrobeErr == 0xFFFF) {

          if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
            ATH_MSG_VERBOSE( evtnum.str()
                             << "  drw " << drwname(rawChannelCollection->identify()) 
                             << " is OFF - skipping");

            if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
              ATH_MSG_VERBOSE( nevtnum.str()
                               << "   suppressing further messages about drawer 0x" << std::hex 
                               << rawChannelCollection->identify()
                               << std::dec << " being bad");
          }
          continue;
        }

        if (DSPBCID == 0
            && GlobalCRCErr == 0 
            && FE_DMUmask == 0
            && ROD_DMUmask == 0
            && BCIDErr == 0
            && MemoryParityErr == 0
            && HeaderFormatErr == 0
            && HeaderParityErr == 0
            && SampleFormatErr == 0
            && SampleParityErr == 0
            && SingleStrobeErr == 0
            && DoubleStrobeErr == 0) {

          if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
            ATH_MSG_VERBOSE( evtnum.str()
                             << "  drw " << drwname(rawChannelCollection->identify()) 
                             << " is MISSING - skipping");

            if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
              ATH_MSG_VERBOSE( nevtnum.str()
                               << "   suppressing further messages about drawer 0x" << std::hex 
                               << rawChannelCollection->identify() << std::dec << " being bad");
          }
          continue;
        }

        if (GlobalCRCErr) {
          GlobalCRCErr = 0xFFFF; // global error - all wrong
          if (m_maxBadDMU<16) {
            if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
              ATH_MSG_VERBOSE( evtnum.str()
                               << "  drw " << drwname(rawChannelCollection->identify())
                               << " global CRC error - skipping");

              if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
                ATH_MSG_VERBOSE( nevtnum.str()
                                 << "   suppressing further messages about drawer 0x" << std::hex 
                                 << rawChannelCollection->identify() << std::dec << " being bad");
            }
            someDQerrors = true;
            continue;
          }
        }

        if (HeaderFormatErr || HeaderParityErr || SampleFormatErr || SampleParityErr ) {
          FE_DMUmask = 0xFFFF; // can not trust FE mask, assume that all DMUs are good
        } else {
          if (eb) { // extended barrel
            if (ebsp) FE_DMUmask<<=1; // shift by one DMU in EBA15 EBC18
            FE_DMUmask = (FE_DMUmask & 0xFF) | ((FE_DMUmask & 0xF00)<<2); // shift upper half by two DMUs
          }
        }

        FE_DMUmask = ~FE_DMUmask & 0xFFFF;   // inversion for FE CRC
        ROD_DMUmask = ~ROD_DMUmask & 0xFFFF; // inversion for ROD CRC

        if (BCIDErr & 0x2) { // DMU1 (second DMU) is bad - can not trust others
          BCIDErr = 0xFFFF;  // assume that all DMUs are bad
          if (m_maxBadDMU < 16) {
            if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
              ATH_MSG_VERBOSE( evtnum.str()
                               << "  drw " << drwname(rawChannelCollection->identify())
                               << " BCID in DMU1 is bad - skipping");

              if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
                ATH_MSG_VERBOSE( nevtnum.str()
                                 << "   suppressing further messages about drawer 0x"
                                 << std::hex << rawChannelCollection->identify() << std::dec << " being bad");
            }
            someDQerrors = true;
            continue;
          }

        } else {
          // additional check if DQ frag BCID is the same as event BCID 
          if ( DSPBCID!=0xDEAD && DSPBCID!=m_evtBCID ) { // DSP BCID doesn't match! all wrong
            BCIDErr = 0xFFFF;
            if (m_maxBadDMU < 16) {
              if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
                ATH_MSG_VERBOSE( evtnum.str()
                                 << "  drw " << drwname(rawChannelCollection->identify())
                                 << " DSP BCID is wrong - skipping");

                if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
                  ATH_MSG_VERBOSE(  nevtnum.str()
                                    << "   suppressing further messages about drawer 0x"
                                    << std::hex << rawChannelCollection->identify() << std::dec << " being bad");
              }
              someDQerrors = true;
              continue;
            }
          }
        }

        uint32_t error = GlobalCRCErr | FE_DMUmask | ROD_DMUmask | BCIDErr | MemoryParityErr |
          HeaderFormatErr | HeaderParityErr | SampleFormatErr | SampleParityErr;

        if (error==0xFFFF && m_maxBadDMU<16) {
          if (m_nDrawerOff[drawerIdx] < m_maxVerboseCnt) {
            ATH_MSG_VERBOSE( evtnum.str()
                << "  drw " << drwname(rawChannelCollection->identify())
                << " whole drawer is bad - skipping");

            if (++m_nDrawerOff[drawerIdx] == m_maxVerboseCnt)
              ATH_MSG_VERBOSE( nevtnum.str()
                  << "   suppressing further messages about drawer 0x"
                  << std::hex << rawChannelCollection->identify() << std::dec << " being bad");
          }
          someDQerrors = true;
          continue;
        }

        // no global error detected - wait m_max_verbose_cnt good events and eventually enable error messages again
        if (m_nDrawerOff[drawerIdx]>=m_maxVerboseCnt) {
          if (++m_nDrawerOff[drawerIdx] == 2*m_maxVerboseCnt) {
            m_nDrawerOff[drawerIdx] = 0;
            ATH_MSG_VERBOSE( nevtnum.str()
                             << "   enabling messages about drawer 0x" << std::hex 
                             << rawChannelCollection->identify()
                             << std::dec << " being bad after " << m_maxVerboseCnt << " good events");
          }
        }

        uint32_t errMB = BCIDErr;
        if (eb) { // do not count non-existing DMUs in EB
          if (ebsp) {
            errMB &= 0x3cfe;
          } else {
            errMB &= 0x3cff;
          }
        }
        int nbadMB = 0;
        while (errMB) {
          if (errMB & 0xF) ++nbadMB;
          errMB >>= 4;
        }
        someDQerrors = (nbadMB >= m_minBadMB);
        if (nbadMB > nbadMBMax) {
          nbadMBMax = nbadMB;
          collMBMax = rawChannelCollection;
        }

        int nerr = 0;
        for (uint32_t i = 0x8000; i != 0; i >>= 1) {
          if (error&i) {
            ++nerr;
            index-=3;
          } else {
            if (emptyBad && nbadMB < 4) {
              m_chanBad[--index] = false;
              m_chanBad[--index] = false;
              m_chanBad[--index] = false;
            } else {
              index -= 3;
            }
          }
        }
        //if (chMBTS>=0) m_chanBad[index+chMBTS] = true; // ignore completely MBTS channel
        int nbad = ((ebsp) ? nerr-5 : ((eb) ? nerr-4 : nerr));

        if (nbad >= m_minBadDMU && nerr <= m_maxBadDMU) {
          someDQerrors = true;
          if (nbad > nbadMax) {
            nbadMax = nbad;
            collMax = rawChannelCollection;
          }
        }
        if (someDQerrors) { // will print later samples for all channels in a drawer
          for ( ; index<hashNext; ++index) { // but if drawer was completely bad, this loop will be skipped
            m_chanSel[index] = true;
          }
        }

        if (allowAmpCheck || emptyBad) {

          for (const TileRawChannel* rawChannel : *rawChannelCollection) {

            HWIdentifier adcId = rawChannel->adc_HWID();
            HWIdentifier chId = m_tileHWID->channel_id(adcId);
            m_tileHWID->get_hash(chId, hash, &chan_context);
            if ( m_chanToSkip[hash] ) continue;
            int adc = m_tileHWID->adc(adcId);
            int channel = m_tileHWID->channel(adcId);
            int ch_type = 0;
            if (channel == chMBTS) {
              ch_type = 2;
            } else if ( (ebsp && (channel == 18 || channel == 19 || channel == 12 || channel == 13) )
                        || (eb && (channel == 0 || channel == 1  || channel == 12 || channel == 13) ) ) {
              ch_type = 1;
            }
            if (emptyBad  && !m_chanBad[hash] ) {
              m_chanBad[hash] = m_tileBadChanTool->getAdcStatus(drawerIdx,channel,adc).isBad() ||
                (DQstatus && !DQstatus->isAdcDQgood(ros,drawer,channel,adc)) ||
                (m_checkDCS && m_tileDCS->getDCSStatus(ros, drawer, channel) > TileDCSState::WARNING);
            }

            if (allowAmpCheck) {

              float amp = rawChannel->amplitude();
              float time = rawChannel->time();
              if (fillChanEne) {
                m_chanEne[hash] = amp;
                m_chanTime[hash] = time;
                m_chanQua[hash] = rawChannel->quality();
              } else {
                m_chanDsp[hash] = amp;
                m_chanTDsp[hash] = time;
              }

              if ( (m_skipMasked && m_chanBad[hash]) ||
                   (m_skipMBTS && channel == chMBTS) ||
                   (m_skipEmpty && TileDQstatus::isChEmpty(ros, drawer, channel) > 0) ||
                   m_skipGain[adc] )
                continue;

              bool ampOk = false;
              if (amp < m_minEneChan[ch_type] ) {
                ampOk = m_bitEneChan[ch_type][0];
              } else if (amp > m_maxEneChan[ch_type] ) {
                ampOk = m_bitEneChan[ch_type][2];
              } else {
                ampOk = m_bitEneChan[ch_type][1];
              }

              bool timeOk = false;
              if (time < m_minTimeChan[ch_type] ) {
                timeOk = m_bitTimeChan[ch_type][0];
              } else if (time > m_maxTimeChan[ch_type] ) {
                timeOk = m_bitTimeChan[ch_type][2];
              } else {
                timeOk = m_bitTimeChan[ch_type][1];
              }

              if (ampOk && timeOk) {

                ATH_MSG_VERBOSE(evtnum.str()
                    << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(adcId)
                    << " ch_ene = " << amp << " ch_t = " << time);

                m_chanSel[hash] = true;

                if (amp < chmin) {
                  chmin = amp;
                  minCh = rawChannel;
                } else if (amp > chmax) {
                  chmax = amp;
                  maxCh = rawChannel;
                }

                if (time<tcmin) {
                  tcmin = time;
                  tminCh = rawChannel;
                }
                else if (time>tcmax) {
                  tcmax = time;
                  tmaxCh = rawChannel;
                }
              }
            }
          }
        }

        for (index = hashNext - 48; index < hashNext; ++index) {
          if ((m_chanSel[index] && rawdata) || someDQerrors) {
            ATH_MSG_VERBOSE(evtnum.str()
                            << "  drw " << drwname(rawChannelCollection->identify())
                            << " nBadMB = " <<  nbadMB
                            << " nBadDMU = " <<  nbad
                            << " EvtBCID = " << m_evtBCID
                            << " DSPBCID = " << rawChannelCollection->getFragDSPBCID()
                            << " GlobCRC = " << rawChannelCollection->getFragGlobalCRC() << " " << GlobalCRCErr
                            << " error = 0x" << std::hex << error
                            << " FE_CRC = 0x"  << rawChannelCollection->getFragFEChipMask()  << " 0x" << FE_DMUmask
                            << " ROD_CRC = 0x" << rawChannelCollection->getFragRODChipMask() << " 0x" << ROD_DMUmask
                            << " BCIDErr = 0x" << rawChannelCollection->getFragBCID()        << " 0x" << BCIDErr
                            << " MemPar = 0x"  << rawChannelCollection->getFragMemoryPar()
                            << " HeadForm = 0x"<< rawChannelCollection->getFragHeaderBit()
                            << " HeadPar = 0x" << rawChannelCollection->getFragHeaderPar()
                            << " SampForm = 0x"<< rawChannelCollection->getFragSampleBit()
                            << " SampPar = 0x" << rawChannelCollection->getFragSamplePar()
                            << std::dec);
            break;
          }
        }
      }

      if (nbadMBMax >= m_minBadMB) {
        ++m_dqerr;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
            << "  drw " << drwname((collMBMax) ? collMBMax->identify() : 0)
            << " nBadMB = " << nbadMBMax
            << " accepted");

      } else if (nbadMax >= m_minBadDMU && nbadMax <= m_maxBadDMU) {
        ++m_dqerr;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
            << "  drw " << drwname((collMax)?collMax->identify():0)
            << " nBadDMU = " << nbadMax
            << " accepted");
      }

      if (tminCh && tminCh != minCh && tminCh != maxCh) {
        ATH_MSG_DEBUG(nevtnum.str()
            << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(tminCh->adc_HWID())
            << " ch_e = " << tminCh->amplitude()
            << " tmin =" << tminCh->time());
      }

      if (tmaxCh && tmaxCh != minCh && tmaxCh != maxCh) {
        ATH_MSG_DEBUG(nevtnum.str()
            << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(tmaxCh->adc_HWID())
            << " ch_e = " << tmaxCh->amplitude()
            << " tmax = " << tmaxCh->time());
      }

      if (minCh) {
        if (!cellminCh) ++m_minChan;
        statusOk = true;
        const char * tit = (tminCh == minCh) ? " tmin = ": ((tmaxCh == minCh) ? " tmax = ": " t = ");
        ATH_MSG_DEBUG(nevtnum.str()
            << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(minCh->adc_HWID())
            << " ch_emin = " << chmin
            << tit << minCh->time()
            << " accepted");
      }
      if (maxCh) {
        if (!cellmaxCh) ++m_maxChan;
        statusOk = true;
        const char * tit = (tminCh == maxCh) ? " tmin = ": ((tmaxCh == maxCh) ? " tmax = ": " t = ");
        ATH_MSG_DEBUG(nevtnum.str()
            << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(maxCh->adc_HWID())
            << " ch_emax = " << chmax
            << tit << maxCh->time()
            << " accepted");
      }
      emptyBad = false;
    }
  }


  if (m_readDigits) {

    // Pointer to a Tile digits container
    SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey);

    if (!digitsContainer.isValid()) {
      ATH_MSG_WARNING("Unable to read TileDigitsContainer from EventStore, disable reading of this container");
      m_readDigits = false;

    } else {

      IdContext chan_context = m_tileHWID->channel_context();
      IdentifierHash hash;
      int index,pmt;
      int nConst = 0;
      int nJump = 0;
      int nDmuErr = 0;
      int nOverLG = 0;
      int nOverHG = 0;
      int nUnderLG = 0;
      int nUnderHG = 0;

      for (const TileDigitsCollection * digitsCollection : *digitsContainer) {

        int frag = digitsCollection->identify();
        bool eb = (frag > 0x2ff);
        bool ebsp = (frag == 0x30e || frag == 0x411);

        int ros = frag >> 8;
        int drawer = frag & 0x3F;
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
        if ( m_drawerToSkip[drawerIdx] ) continue;

        int chMBTS = -1;
        if (eb) {
          for (int ch: {12,4,0}) {
            m_cabling->h2s_cell_id_index(ros,drawer,ch,index,pmt);
            if (index == -2) {
              chMBTS = ch;
              break;
            }
          }
        }

        int nChBadDB = 0;
        int nChBadNC = 0;
        int nChBad = 0;
        int nChTot = 0;
        int nChDmu[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int nChBadDmu[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        for (const TileDigits* tile_digits : *digitsCollection) {

          ++nChTot;

          HWIdentifier adcId = tile_digits->adc_HWID();
          HWIdentifier chId = m_tileHWID->channel_id(adcId);
          m_tileHWID->get_hash(chId, hash, &chan_context);
          if ( m_chanToSkip[hash] ) continue;
          int channel = m_tileHWID->channel(adcId);
          int dmu = channel/3;
          ++nChDmu[dmu];
          int chEmpty = TileDQstatus::isChEmpty(ros, drawer, channel);
          bool isConnected = (chEmpty < 2);
          if (!isConnected) ++nChBadNC;
          int adc = m_tileHWID->adc(adcId);
          const char *cellname = "";
          int ch_type = 0;
          if (channel == chMBTS) {
            cellname = " MBTS";
            ch_type = 2;
          } else if ( (ebsp && (channel == 18 || channel == 19 || channel == 12 || channel == 13) )
                      || (eb && (channel == 0 || channel == 1  || channel == 12 || channel == 13) ) ) {
            cellname = " GAP";
            ch_type = 1;
          } else if (chEmpty > 0) {
            cellname = " EMPTY";
          }

          const char *badname = "";
          if (DQstatus && !DQstatus->isAdcDQgood(ros,drawer,channel,adc)) {
            badname = " BADDQ";
            if (isConnected) {
              ++nChBad;
              ++nChBadDmu[dmu];
            }
          } else if (m_checkDCS && m_tileDCS->getDCSStatus(ros, drawer, channel) > TileDCSState::WARNING) {
            badname = " BADDCS";
          } else if (m_tileBadChanTool->getAdcStatus(drawerIdx,channel,adc).isBad()) {
            badname = " BADDB";
            if (isConnected) {
              ++nChBadDB;
            }
          } else if (m_chanBad[hash]) {
            if (badFromCell) {
              if (ch_type != 2) badname = " BADQUAL";
              m_chanQua[hash] = 999;
              if (isConnected && ch_type!=2) {
                ++nChBad;
                ++nChBadDmu[dmu];
              }
            } else {
              badname = " BADUNKN"; // should never see this
            }
          } else if (badFromCell && m_chanEne[hash] == 0.0) {
            badname = " BADDIGI"; // temporary fix, (for May-2012 ESD), should never get this
            if (isConnected) {
              ++nChBad;
              ++nChBadDmu[dmu];
            }
          }
          const char *enename = " ene = ";
          const char *timename = " time = ";
          const char *qualname = " qual = ";
          if (badFromCell && badname[0] != 0) {
            enename = " BAD = ";
            if (m_chanDsp.size()) {
              qualname = " eDSP = ";
              m_chanQua[hash] = m_chanDsp[hash];
            }
            if (m_chanTDsp.size()) {
              timename = " tDSP = ";
              m_chanTime[hash] = m_chanTDsp[hash];
            }
          }

          char badnm[30];
          sprintf(badnm," BADDIGIEX%s",badname);
          float dmin,dmax;

          std::vector<float> samples = tile_digits->samples();
          int nSamp = samples.size();
          if (nSamp > 6) {

            bool useCh= !( (m_skipMBTS && channel == chMBTS) ||
                           (m_skipEmpty && chEmpty > 0) );
            bool checkCh = !( (m_skipMasked && m_chanBad[hash]) ) && useCh && m_checkJumps;

            int err = TileRawChannelBuilder::CorruptedData(ros,drawer,channel,adc,samples,dmin,dmax,m_ADCmaxMinusEps,m_ADCmaskValueMinusEps);

            if (badname[0]==0) {
              if (err && err>-3) { // do not consider all zeros in empty samples as error
                if (isConnected || err != -2) {
                  if (checkCh) m_chanSel[hash] = true;
                  badname = badnm;
                  ++nChBad;
                  ++nChBadDmu[dmu];
                  if (!isConnected) --nChBadNC;
                }
                if (err > 0) {
                  if (err < 10) {
                    badnm[9] = 48+err;
                  } else if (err < 36) {
                    badnm[9] = 55+err;
                  }
                } else {
                  badnm[9] = 48;
                }
              } else {
                // old error types, used for tests in August 2012
                // expect to see only warningE7 and warningE8 for gap/crack
                int warn = Are3FF(samples, adc, ch_type);
                if (warn) {
                  if (checkCh) m_chanSel[hash] = true;
                  sprintf(badnm," warningE%d%s",warn,badname);
                  badname = badnm;
                }
              }
            }

            if ((!err)  // channel without bad patterns
                && (useCh)                // normal connected channel
                && (badname[0] == 0 || badname[1] == 'w' // no digital error
                || (badname[4] == 'Q' && !m_skipMasked))) { // error from TileCell but it is ignored

              if (adc) { // HG

                if (dmax > m_overflowHG) {
                  m_chanSel[hash] = true; // always print overflows
                  if (m_checkOverHG){
                    ++nOverHG;
                  }
                }
                if (dmin < m_underflowHG) {
                  m_chanSel[hash] = true; // always print underflows
                  if (m_checkUnderHG){
                    ++nUnderHG;
                  }
                }

              } else { // LG

                if (dmax > m_overflowLG) {
                  m_chanSel[hash] = true; // always print overflows
                  if (m_checkOverLG){
                    ++nOverLG;
                  }
                }
                if (dmin < m_underflowLG) {
                  m_chanSel[hash] = true; // always print underflows
                  if (m_checkUnderLG){
                    ++nUnderLG;
                  }
                }
              }
            }

            bool someSampErrors = false;

            if (m_checkJumps && (checkCh || m_chanSel[hash])) {

              float pedDelta = (adc ? m_pedDeltaHG : m_pedDeltaLG);
              float jumpDelta = (adc ? m_jumpDeltaHG : m_jumpDeltaLG);

              float ped = samples[0];
              float dmin = ped;
              float dmax = ped;
              int nped = 1;
              int npedmax = 1;
              bool cnstPed = true;
              bool cnstPedmax = true;
              for (int i = 1; i < nSamp; ++i) {
                float smp = samples[i];
                float dped = smp - ped;
                if (fabs(dped) < pedDelta) {
                  ++nped;
                  if (dped != 0.0) {
                    cnstPed = false;
                    ped += dped/nped;
                  }
                } else {
                  if (nped>npedmax) {
                    npedmax=nped;
                    cnstPedmax = cnstPed;
                  }
                  cnstPed = true;
                  ped = smp;
                  nped = 1;
                }
                if (smp<dmin) {
                  dmin = smp;
                } else if (smp>dmax) {
                  dmax = smp;
                }
              }
              if (nped>npedmax) {
                npedmax=nped;
                cnstPedmax = cnstPed;
              }

              if (dmax - dmin >= jumpDelta) {
                bool accEmin = (m_chanEne[hash]<m_minEneChan[ch_type]);
                bool accEmax = (m_chanEne[hash]>m_maxEneChan[ch_type]);
                bool accCnst = false;
                bool accJump = false;
                bool cnstMin = true;
                bool cnstMax = true;
                bool jumpNeg = false;
                bool jumpPos = false;
                bool jumpEnd = false;
                bool jumpZer = false;
                bool jumpOve = false;
                bool narrowUp = false;
                bool narrowDown = false;
                if (npedmax >= m_constLength && ((dmax-ped) >= jumpDelta || (ped-dmin) >= jumpDelta) ) {
                  ++nConst;
                  accCnst = true;
                }
                int nmin = 0;
                int nmax = 0;
                int pmin = -1;
                int pmax = -1;
                float abovemin = dmax;
                float belowmax = dmin;
                for (int i = 0; i < nSamp; ++i) {
                  float smp = samples[i];
                  if (smp - dmin < pedDelta) {
                    ++nmin;
                    pmin = i;
                    if (smp != dmin) cnstMin = false;
                  }
                  if (dmax - smp < pedDelta) {
                    ++nmax;
                    pmax = i;
                    if (smp != dmax) cnstMax = false;
                  }
                  if (smp < abovemin && smp > dmin) {
                    abovemin = smp;
                  }
                  if (smp > belowmax && smp < dmax) {
                    belowmax = smp;
                  }
                }
                if (nmax + nmin == nSamp) {
                  if (nmax > 1 && nmin > 1) {
                    ++nJump;
                    accJump = true;
                  } else if (nmax == 1) {
                    if (pmax < nSamp - 1) { // ignore jump in last sample
                      ++nJump;
                      accJump = true;
                      jumpPos = true;
                      cnstMax = false;
                    }
                    if (pmax == 0 || pmax == nSamp - 1) {
                      jumpEnd = true;
                    }
                  } else if (nmin == 1) {
                    ++nJump;
                    accJump = true;
                    jumpNeg = true;
                    cnstMin = false;
                    if (pmin == 0 || pmin == nSamp - 1) {
                      jumpEnd = true;
                    }
                  }
                }
                if (dmin == 0.0) {
                  if (!accJump) {
                    ++nJump;
                    accJump = true;
                    cnstMin = false;
                    cnstMax = false;
                  }
                  jumpZer = true;
                }
                if (dmax > m_ADCmaxMinusEps) {
                  if (!accJump) {
                    ++nJump;
                    accJump = true;
                    cnstMin = false;
                    cnstMax = false;
                  }
                  jumpOve = true;
                }
                float secondMax = (dmax-dmin)*m_secondMaxLevel;
                if (pmax > 0 && pmax < nSamp-1 && std::max(samples[pmax-1], samples[pmax+1]) < dmin+secondMax) {
                  if (!accJump) {
                    ++nJump;
                    accJump = true;
                    cnstMax = false;
                    if (nmin + nmax != nSamp) {
                      cnstMin = false;
                    }
                  }
                  narrowUp = true;
                }
                if (pmin > 0 && pmin < nSamp - 1 && std::min(samples[pmin - 1], samples[pmin + 1]) > dmax - secondMax) {
                  if (!accJump) {
                    ++nJump;
                    accJump = true;
                    cnstMin = false;
                    if (nmin + nmax != nSamp) {
                      cnstMax = false;
                    }
                  }
                  narrowDown = true;
                }

                if (accEmin || accEmax || accCnst || accJump) {
                  someSampErrors = true;
                  ATH_MSG_VERBOSE (evtnum.str()
                                   << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(adcId)
                                   << enename << m_chanEne[hash] << "  samp = " << samples[0]
                                   << " " << samples[1] << " " << samples[2] << " " << samples[3]
                                   << " " << samples[4] << " " << samples[5] << " " << samples[6]
                                   << timename << m_chanTime[hash]
                                   << qualname << m_chanQua[hash]
                                   << cellname << badname
                                   << ((accEmin) ? " neg_e" : "")
                                   << ((accEmax) ? " pos_e" : "")
                                   << ((accCnst) ? " const" : "")
                                   << ((accCnst&&cnstPedmax) ? "Const" : "")
                                   << ((accJump) ? " jump"  : "")
                                   << ((accJump&&jumpZer) ? "Zero" : "")
                                   << ((accJump&&jumpOve) ? "Over" : "")
                                   << ((accJump&&jumpPos) ? "SingleUp" : ((narrowUp) ? "NarrowUp"  : "") )
                                   << ((accJump&&jumpNeg) ? "SingleDown" : ((narrowDown) ? "NarrowDown"  : "") )
                                   << ((accJump&&jumpEnd) ? "AtEdge" : "")
                                   << ((accJump&&cnstMin) ? "ConstMin" : "")
                                   << ((accJump&&cnstMax) ? "ConstMax" : "")
                                   << " " << dmax-dmin);
                }
              }
            }

            if (someSampErrors) {
              m_chanSel[hash] = true;
            } else if (m_chanSel[hash]) {
              bool accEmin = (m_chanEne[hash] < m_minEneChan[ch_type]);
              bool accEmax = (m_chanEne[hash] > m_maxEneChan[ch_type]);
              bool jumpOve = (dmax>m_ADCmaxMinusEps);
              bool jumpZer = (dmin < 0.01);
              ATH_MSG_VERBOSE(evtnum.str()
                              << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(adcId)
                              << enename << m_chanEne[hash] << "  samp = " << samples[0]
                              << " " << samples[1] << " " << samples[2] << " " << samples[3]
                              << " " << samples[4] << " " << samples[5] << " " << samples[6]
                              << timename << m_chanTime[hash]
                              << qualname << m_chanQua[hash]
                              << cellname << badname
                              << ((accEmin) ? " neg_e" : "")
                              << ((accEmax) ? " pos_e" : "")
                              <<((jumpZer) ? " underflow" : "")
                              <<((jumpOve) ? " overflow" : "") );
            }

          }
        }
        if (m_checkDMUs && nChBad > 1 && nChBad + nChBadDB + nChBadNC < nChTot) {
          int nChBad1 = 0;
          int nChBad2 = 0;
          int nDmuBad1 = 0;
          int nDmuBad2 = 0;
          int nDmuBad = 0;
          int nDmuTot = 0;
          bool has23 = false;
          for (int dmu = 0; dmu < 16; ++dmu) {
            if (nChDmu[dmu] > 0) {
              ++nDmuTot;
              if (nChBadDmu[dmu] > 0) {
                ++nDmuBad;
                if (dmu < 8) {
                  nChBad1 += nChBadDmu[dmu];
                  ++nDmuBad1;
                } else {
                  nChBad2 += nChBadDmu[dmu];
                  ++nDmuBad2;
                }
                if (nChBadDmu[dmu] == 2 && nChDmu[dmu] == 3) has23 = true;
              }
            }
          }
          if (nDmuBad == 1 /* && (!has23) */  ) continue;
          if (nDmuBad == 2 && nChBad < 3) continue;

          if (nDmuBad>2 || nChBad > 9 || nChTot > 19 || has23) {

            ++nDmuErr;

            for (const TileDigits* tile_digits : *digitsCollection) {

              HWIdentifier adcId = tile_digits->adc_HWID();
              HWIdentifier chId = m_tileHWID->channel_id(adcId);
              m_tileHWID->get_hash(chId, hash, &chan_context);
              if ( m_chanToSkip[hash] ) continue;
              std::vector<float> samples = tile_digits->samples();

              if (!m_chanSel[hash] && samples.size()>6) {
                int channel = m_tileHWID->channel(adcId);
                int adc = m_tileHWID->adc(adcId);
                int chEmpty = TileDQstatus::isChEmpty(ros, drawer, channel);
                const char *cellname = "";
                int ch_type = 0;
                if (channel == chMBTS) {
                  cellname = " MBTS";
                  ch_type = 2;
                } else if ( (ebsp && (channel == 18 || channel == 19 || channel == 12 || channel == 13) )
                            || (eb && (channel == 0 || channel == 1  || channel == 12 || channel == 13) ) ) {
                  cellname = " GAP";
                  ch_type = 1;
                } else if (chEmpty > 0) {
                  cellname = " EMPTY";
                }
                const char *badname = "";
                if (m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad()) {
                  badname = " BADDB";
                } else if (DQstatus && !DQstatus->isAdcDQgood(ros, drawer, channel, adc)) {
                  badname = " BADDQ";
                } else if (m_checkDCS && m_tileDCS->getDCSStatus(ros, drawer, channel) > TileDCSState::WARNING) {
                  badname = " BADDCS";
                } else if (m_chanBad[hash]) {
                  if (badFromCell) {
                    if (ch_type != 2) badname = " BADQUAL";
                    m_chanQua[hash] = 999;
                  } else {
                    badname = " BADUNKN"; // should never see this
                  }
                } else if (badFromCell && m_chanEne[hash] == 0.0) {
                  badname = " BADDIGI"; // temporary fix, (for May-2012 ESD), should never get this
                }

                char badnm[30];
                sprintf(badnm, " BADDIGIEX%s", badname);
                float dmin, dmax;

                int err = TileRawChannelBuilder::CorruptedData(ros, drawer,
							       channel, adc, samples, dmin, dmax, m_ADCmaxMinusEps,m_ADCmaskValueMinusEps);
                if (err) {
                  bool isConnected = (chEmpty < 2);
                  if (isConnected || err != -2) {
                    badname = badnm;
                  }
                  if (err > 0) {
                    if (err < 10) {
                      badnm[9] = 48 + err;
                    } else if (err < 36) {
                      badnm[9] = 55 + err;
                    }
                  } else {
                    badnm[9] = 48;
                  }
                }

                const char *enename = " ene = ";
                const char *timename = " time = ";
                const char *qualname = " qual = ";
                if (badFromCell && badname[0] != 0) {
                  enename = " BAD = ";
                  if (m_chanDsp.size()) {
                    qualname = " eDSP = ";
                    m_chanQua[hash] = m_chanDsp[hash];
                  }
                  if (m_chanTDsp.size()) {
                    timename = " tDSP = ";
                    m_chanTime[hash] = m_chanTDsp[hash];
                  }
                }

                bool accEmin = (m_chanEne[hash]<m_minEneChan[ch_type]);
                bool accEmax = (m_chanEne[hash]>m_maxEneChan[ch_type]);
                bool jumpOve = (dmax > m_ADCmaxMinusEps);
                bool jumpZer = (dmin < 0.01);

                ATH_MSG_VERBOSE(evtnum.str()
                                << " chan " << std::left << std::setw(14) << m_tileHWID->to_string(adcId)
                                << enename << m_chanEne[hash] << "  samp = " << samples[0]
                                << " " << samples[1] << " " << samples[2] << " " << samples[3]
                                << " " << samples[4] << " " << samples[5] << " " << samples[6]
                                << timename << m_chanTime[hash]
                                << qualname << m_chanQua[hash]
                                << cellname << badname
                                << ((accEmin) ? " neg_e" : "")
                                << ((accEmax) ? " pos_e" : "")
                                << ((jumpZer) ? " underflow" : "")
                                << ((jumpOve) ? " overflow" : "") );

              }
            }
          }

          std::ostringstream badstr;
          badstr << "  ch: " << nChBad1 << " + " << nChBad2 << " = " << nChBad << " / " << nChTot << " = " << 100*nChBad/nChTot
                 << " %  dmu: " << nDmuBad1 << " + " << nDmuBad2 << " = " << nDmuBad << " / " << nDmuTot << " ";
          for (int dmu=0; dmu<16; ++dmu) {
            if (nChDmu[dmu]>0) {
              badstr << " " << std::hex << dmu << "=" << nChBadDmu[dmu] << "/" << nChDmu[dmu];
            }
          }
          ATH_MSG_VERBOSE (evtnum.str()
                           << "  drw " << drwname(digitsCollection->identify()) << badstr.str());
        }
      }

      if (nConst) {
        ++m_const;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
                      << " n_const_sample_errors = " << nConst
                      << " accepted");
      }
      if (nJump) {
        ++m_jump;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
                      << " n_jump_sample_errors = " << nJump
                      << " accepted");
      }
      if (nOverLG) {
        ++m_overLG;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
                      << " n_overflow_LG = " << nOverLG
                      << " accepted");
      }
      if (nOverHG) {
        ++m_overHG;
        statusOk = true;
        ATH_MSG_DEBUG(nevtnum.str()
                      << " n_overflow_HG = " << nOverHG
                      << " accepted");
      }
      if (nUnderLG) {
        ++m_underLG;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
                      << " n_underflow_LG = " << nUnderLG
                      << " accepted");
      }
      if (nUnderHG) {
        ++m_underHG;
        statusOk = true;
        ATH_MSG_DEBUG(nevtnum.str()
                      << " n_underflow_HG = " << nUnderHG
                      << " accepted");
      }
      if (nDmuErr) {
        ++m_dmuerr;
        statusOk = true;
        ATH_MSG_DEBUG( nevtnum.str()
                      << " n_DMU_errors = " << nDmuErr
                      << " accepted");
      }
    }
  }

  if (m_printOnly)
    this->setFilterPassed (false);
  else
    this->setFilterPassed (statusOk);

  if (statusOk) {
    ++m_accept;
    //ATH_MSG_VERBOSE (nevtnum.str() << " accepted");
  } else {
    //ATH_MSG_VERBOSE (nevtnum.str() << " rejected");
  }

  return StatusCode::SUCCESS;
}


// this method is not used for the moment
void TileCellSelector::printCell(const TileCell * tile_cell) {
  if (tile_cell==0) return;

  // int drw = 0; // drawer number, range 0-63, the same for both channels
  // int ch1 = -1, ch2 = -1; // channel number, range 0-47 or -1 for unknown
  int ros1 = 0, ros2 = 0;

  const CaloDetDescrElement * caloDDE = tile_cell->caloDDE();
  //Identifier id = tile_cell->ID();

  IdentifierHash hash1 = caloDDE->onl1();
  if (hash1 != TileHWID::NOT_VALID_HASH) {
    HWIdentifier hw1 = m_tileHWID->channel_id(hash1);
    //ch1 = m_tileHWID->channel(hw1);
    //drw = m_tileHWID->drawer(hw1);
    ros1 = m_tileHWID->ros(hw1);
  }

  IdentifierHash hash2 = caloDDE->onl2();
  if (hash2 != TileHWID::NOT_VALID_HASH) {
    HWIdentifier hw2 = m_tileHWID->channel_id(hash2);
    //ch2 = m_tileHWID->channel(hw2);
    //drw = m_tileHWID->drawer(hw2);
    ros2 = m_tileHWID->ros(hw2);
    if (ros1 == 0) ros1=ros2;
  } else {
    ros2 = ros1;
  }

  // something is wrong
  if (ros1 == 0) return;
  /*
    int module = m_tileID->module(id);

    int samp = m_tileID->sample(id);

    bool single_PMT_scin = (samp == TileID::SAMP_E);
    bool single_PMT_C10 = (m_tileID->section(id) == TileID::GAPDET &&
    samp == TileID::SAMP_C &&
    (! m_cabling->C10_connected(m_tileID->module(id))) );

    // distinguish cells with one or two PMTs
    bool single_PMT = single_PMT_C10 || single_PMT_scin;

    // distinguish normal cells and fantoms (e.g. non-existing D4 in EBA15, EBC18
    // or non-existing E3/E4 - they might appear in CaloCellContainer)
    bool real_cell  = single_PMT_C10 || m_cabling->TileGap_connected(id);

    // note that in single PMT cell both badch1() and badch2() are changed together
    bool badch1 = (tile_cell->badch1());
    bool badch2 = (tile_cell->badch2());

    // 0 = both PMTs are good; 1= 1 PMT is bad; 2= both PMTs are bad, or PMT is bad for single PMT cell
    int cell_isbad = (int)badch1 + (int)badch2;

    int gn1 = tile_cell->gain1(); // gain of first PMT
    int gn2 = tile_cell->gain2(); // gain of second PMT

    bool ch1Ok = (ch1>-1 && gn1 != CaloGain::INVALIDGAIN);
    bool ch2Ok = (ch2>-1 && gn2 != CaloGain::INVALIDGAIN);

    // get the cell energy, time and position info
    double energy = tile_cell->energy();
    double time = tile_cell->time();
    double eta = tile_cell->eta();
    double phi = tile_cell->phi();
    double ene1 = tile_cell->ene1();
    double ene2 = tile_cell->ene2();
    double ediff = (single_PMT) ? 0.0 : tile_cell->eneDiff();
    double eratio = (energy!=0.0) ? ediff/energy : 0.0;
    double t1 = tile_cell->time1();
    double t2 = tile_cell->time2();
    double tdiff = (single_PMT) ? 0.0 : tile_cell->timeDiff();
  */
}

StatusCode TileCellSelector::finalize() {
  //ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_INFO ("Processed " << m_counter << " events, accepted " << m_accept
                << " (" << m_minCell << "/" << m_minChan
                << "," << m_maxCell << "/" << m_maxChan
                << "," << m_jump << "/" << m_const
                << "," << m_overLG << "/" << m_overHG
                << "," << m_underLG << "/" << m_underHG
                << "," << m_dqerr << "/" << m_dmuerr
                << "," << m_warnerr << ") events.");

  //ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}

// copy of the function from OptFilter
// to see which channels will be masked 
int TileCellSelector::Are3FF(std::vector<float> & OptFilterDigits, int gain, int ch_type) {
  bool allSaturated = true;
  int error = 0;

  unsigned int nSamp = OptFilterDigits.size();
  if (nSamp) {
    float dmin = OptFilterDigits[0];
    float dmax = dmin;

    for (unsigned int i = 1; i < nSamp; ++i) {
      float dig = OptFilterDigits[i];
      if (dig > dmax) dmax = dig;
      else if (dig < dmin) dmin = dig;
    }
    allSaturated = (dmin > m_ADCmaxMinusEps);

    // FIXME:: set these parameters from JobOptions
    // FIXME:: move this method to base class 
    const float epsilon = 4.1; // allow +/- 2 counts fluctuations around const value
    const float delta[4] = {29.9, 29.9, 49.9, 99.9};  // jump levels between constLG, constHG, non-constLG, non-constHG
    const float level0 = 29.9; // jump from this level to zero is bad
    const float level1 = 99.9; // jump from this level to m_tileInfo->ADCmax() is bad 
    const float level2 = 199.9; // base line at this level is bad
    const float delt = std::min(std::min(std::min(delta[0], delta[1]), std::min(delta[2], delta[3])), level0);

    if (!allSaturated && (dmax - dmin) > delt) {
      float abovemin = dmax;
      float belowmax = dmin;
      unsigned int nmin = 0;
      unsigned int nmax = 0;
      unsigned int pmin = nSamp;
      unsigned int pmax = nSamp;
      for (unsigned int i = 0; i < nSamp; ++i) {
        float smp = OptFilterDigits[i];
        if (smp - dmin < epsilon) {
          ++nmin;
          pmin = i;
        }
        if (dmax - smp < epsilon) {
          ++nmax;
          pmax = i;
        }
        if (smp < abovemin && smp > dmin) {
          abovemin = smp;
        }
        if (smp > belowmax && smp < dmax) {
          belowmax = smp;
        }
      }

      if (abovemin != dmax || belowmax != dmin) { // more than two different values
        gain += 2; // shift index by 2, i.e. use thresholds for non-const levels
      }

      if (dmin < 0.01 && dmax > m_ADCmaxMinusEps) { // jump from zero to saturation
        error = 1;
      } else if (dmin < 0.01 && abovemin > level0 && nmin > 1) { // at least two samples at zero, others - above pedestal
        error = 2;
      } else if (dmax > m_ADCmaxMinusEps && belowmax < level1 && nmax > 1) { // at least two saturated. others - close to pedestal
        error = 3;
      } else if (dmin>level2 && (gain==0 || ch_type<2) ) { // baseline above threshold is bad
        error = 9;                                         // but should not apply that to MBTS
      } else if (nmax+nmin==nSamp && (dmax-dmin) > delta[gain]) {
        if (nmax>1 && nmin>1) { // at least 2 samples at two distinct levels
          error = 4;
        } else if (nmax==1) {
          if (pmax>0 && pmax<nSamp-1) { // jump up in one sample, but not at the edge
            error = 5;
          }
        } else if (nmin==1) { // jump down in one sample
          error = 6;
        }
      }
      if (!error && (dmax - dmin) > delta[gain]) {
        float secondMax = (dmax - dmin) * m_secondMaxLevel;
        if (pmax > 0 && pmax < nSamp - 1
            && std::max(OptFilterDigits[pmax - 1], OptFilterDigits[pmax + 1]) < dmin + secondMax) {

          error = 7; // jump up in one sample in the middle. which is much higher than all others
        } else if (pmin > 0 && pmin < nSamp - 1
            && std::min(OptFilterDigits[pmin - 1], OptFilterDigits[pmin + 1]) > dmax - secondMax) {

          error = 8; // jump down in one sample. which is much lower than all others
        }
      }
    }
  }

  if (allSaturated)
    return 99;
  else
    return error;
}
