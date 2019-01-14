/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTBDump.cxx
//  Author   : Alexander Solodkov
//  Created  : May, 2004
//
//  DESCRIPTION:
//     Implement the algorithm to dump TB ByteStream file
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

//TileCalo include
#include "TileIdentifier/TileTBFrag.h"
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileTBRec/TileTBDump.h"
#include "TileByteStream/TileOFC.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolEmscale.h"

#include "boost/io/ios_state.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <string>
#include <ctime>

const char *cern_local_time(time_t unix_time)
{
    using namespace boost::local_time;
    using namespace boost::posix_time;
    
    static char dateTime[32];

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
    local_date_time gva_time(from_time_t(unix_time),gva_tz);

    //std::ostringstream otime;
    //otime << gva_time; // time in the format YYYY-MMM-DD HH:MM:SS TZ
    //strncpy(dateTime,otime.str().c_str(),31);

    //time_duration hms(gva_time.time_of_day()); - will give time of the day in GMT
    //int HHMMSS = hms.hours()*10000+hms.minutes()*100+hms.seconds();

    struct tm gva_tm(to_tm(gva_time));
    strftime(dateTime, 32, "%Y-%b-%d %H:%M:%S %Z", &gva_tm);
    //HHMMSS = gva_tm.tm_hour*10000+gva_tm.tm_min*100+gva_tm.tm_sec;

    // the code below is only valid when running at CERN (in Geneva time zone)
    //struct tm *time = localtime((time_t*)(&m_evTime));
    //HHMMSS = time->tm_hour*10000+time->tm_min*100+time->tm_sec;
    //strftime(dateTime, 32, "%Y-%m-%d %H:%M:%S %Z", time);

    return dateTime;
}

// Constructor & deconstructor

/// TileTBDump produces formatted dump for both RawChannels and Digits
/// Properties (JobOption Parameters):
///    drawerList - vector of frag IDs present in data (hex numbers as strings)
///    drawerType - type for every frag from drawerList
///                 1=B+, 2=B-, 3=EB+, 4=EB-
///    these parameters are needed only for 2003 data
///    (when 0x102 and 0x202 were ext.barrel modules)
///    correct values for 2004 are set by default
/** @class TileTBDump
 *  @brief Class to do a formatted dump of the TileCal ByteStream fragments
 */

TileTBDump::TileTBDump(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_RobSvc("ROBDataProviderSvc", name)
  , m_cabling(0)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfcCool("TileCondToolOfcCool")
  , m_tileToolEmscale("TileCondToolEmscale")
{
  m_drawerList.resize(9);    m_drawerType.resize(9);
  m_drawerList[0] = "0x200"; m_drawerType[0] = 2; // barrel neg
  m_drawerList[1] = "0x201"; m_drawerType[1] = 2; // barrel neg
  m_drawerList[2] = "0x202"; m_drawerType[2] = 2; // barrel neg
  m_drawerList[3] = "0x100"; m_drawerType[3] = 1; // barrel pos
  m_drawerList[4] = "0x101"; m_drawerType[4] = 1; // barrel pos
  m_drawerList[5] = "0x102"; m_drawerType[5] = 1; // barrel pos
  m_drawerList[6] = "0x400"; m_drawerType[6] = 4; // ext. barrel neg
  m_drawerList[7] = "0x401"; m_drawerType[7] = 4; // ext. barrel neg
  m_drawerList[8] = "0x402"; m_drawerType[8] = 4; // ext. barrel neg

  declareProperty("drawerList", m_drawerList);
  declareProperty("drawerType", m_drawerType);
  
  declareProperty("dumpHeader", m_dumpHeader = true);
  declareProperty("dumpData", m_dumpData = true);
  declareProperty("dumpStatus", m_dumpStatus = true);
  declareProperty("dumpOnce", m_dumpOnce = false);
  declareProperty("dumpUnknown", m_dumpUnknown = false);
  declareProperty("showUnknown", m_showUnknown = true);

  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfcCool", m_tileCondToolOfcCool, "TileCondToolOfcCool");
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
 
  declareProperty("bc_time_seconds",     m_bc_time_seconds      = -1);
  declareProperty("bc_time_nanoseconds", m_bc_time_nanoseconds  = -1);
  declareProperty("global_id",           m_global_id            = -1);
  declareProperty("run_type",            m_run_type             = -1);
  declareProperty("run_no",              m_run_no               = -1);
  declareProperty("lumi_block",          m_lumi_block           = -1);
  declareProperty("lvl1_id",             m_lvl1_id              = -1);
  declareProperty("bc_id",               m_bc_id                = -1);
  declareProperty("lvl1_trigger_type",   m_lvl1_trigger_type    = -1);
  declareProperty("nlvl1_trigger_info",  m_nlvl1_trigger_info   = -1);

  m_v3Format = true;
  m_frag5found = false;
  m_sizeOverhead = 3;
  m_unit = -1;
}


TileTBDump::~TileTBDump() {
}

// Alg standard interface function
StatusCode TileTBDump::initialize() {
  
  memset(m_statFrag5, 0, sizeof(m_statFrag5));
   
  CHECK( m_RobSvc.retrieve() );

  //=== get TileCondToolOfcCool
  CHECK( m_tileCondToolOfcCool.retrieve() );
  
  //=== get TileToolTiming
  CHECK( m_tileToolTiming.retrieve() );
  
  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  int size = m_drawerList.size();
  for (int dr = 0; dr < size; ++dr) {
    unsigned int frag = strtol(m_drawerList[dr].data(), NULL, 0);
    m_drawerMap[frag] = m_drawerType[dr];
  }

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


StatusCode TileTBDump::finalize() {

  std::string unitName[5]={"ADC counts","pCb","CspCb","MeV", "unknown"};

  if (m_unit < 0) {
    std::cout << std::endl << "DSP reco fragments not found" << std::endl;
    m_unit = 4;
  }
  std::cout << std::endl << "Units in DSP reco fragments are " << unitName[std::min(m_unit,4)] << std::endl;
  
  if (m_frag5found) {
    std::cout << std::endl << "StatFrag5[40..129]";
    for (int i = 40; i < 130; i++) {
      if (i % 10 == 0) std::cout << std::endl << "  [" << std::setw(3) << i << "] : ";
      std::cout << std::setw(9) << m_statFrag5[i];
    }
    std::cout << std::endl; 
  }
  
  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileTBDump::execute() {


  static bool notFirst = false;
  if (m_dumpOnce && notFirst) return StatusCode::SUCCESS;
  notFirst = true;
  boost::io::ios_base_all_saver coutsave(std::cout);
  std::cout << std::fixed;
  
  ATH_MSG_DEBUG( "execute()" );

  int verbosity = 0;
  if ( msgLvl(MSG::NIL) ) {
    verbosity = 7;
  } else if ( msgLvl(MSG::VERBOSE) ) {
    verbosity = 2;
  } else if ( msgLvl(MSG::DEBUG) ) {
    verbosity = 1;
  }
  
  // take full event
  const eformat::FullEventFragment<const uint32_t*> * event = m_RobSvc->getEvent();
  
  if (m_bc_time_seconds     >=0 && m_bc_time_seconds     != (int32_t)event->bc_time_seconds()) return StatusCode::SUCCESS;
  if (m_bc_time_nanoseconds >=0 && m_bc_time_nanoseconds != (int32_t)event->bc_time_nanoseconds()) return StatusCode::SUCCESS;
  if (m_global_id           >=0 && m_global_id           != (int32_t)event->global_id()) return StatusCode::SUCCESS;
  if (m_run_type            >=0 && m_run_type            != (int32_t)event->run_type()) return StatusCode::SUCCESS;
  if (m_run_no              >=0 && m_run_no              != (int32_t)event->run_no()) return StatusCode::SUCCESS;
  if (m_lumi_block          >=0 && m_lumi_block          != (int32_t)event->lumi_block()) return StatusCode::SUCCESS;
  if (m_lvl1_id             >=0 && m_lvl1_id             != (int32_t)event->lvl1_id()) return StatusCode::SUCCESS;
  if (m_bc_id               >=0 && m_bc_id               != (int32_t)event->bc_id()) return StatusCode::SUCCESS;
  if (m_lvl1_trigger_type   >=0 && m_lvl1_trigger_type   != (int32_t)event->lvl1_trigger_type()) return StatusCode::SUCCESS;
  if (m_nlvl1_trigger_info  >=0 && m_nlvl1_trigger_info  != (int32_t)event->nlvl1_trigger_info()) return StatusCode::SUCCESS;

  std::cout << "============================" << std::endl;
  std::cout << "Event time (sec): " << (uint32_t)event->bc_time_seconds() << std::endl;
  std::cout << "Event time (ns):  " << (uint32_t)event->bc_time_nanoseconds() << std::endl;
  std::cout << "Global ID:        " << (uint32_t)event->global_id() << std::endl;
  std::cout << "Run Type:         " << (uint32_t)event->run_type() << std::endl;
  std::cout << "Run Number:       " << (uint32_t)event->run_no() << std::endl;
  std::cout << "Lumi Block:       " << (uint32_t)event->lumi_block() << std::endl;
  std::cout << "Level1 ID:        " << (uint32_t)event->lvl1_id() << std::endl;
  std::cout << "BCID:             " << (uint32_t)event->bc_id() << std::endl;
  std::cout << "Level1 trig type: " << (uint32_t)event->lvl1_trigger_type() << std::endl;
  std::cout << "Level1 Nwords:    " << (uint32_t)event->nlvl1_trigger_info() << std::endl;
  std::cout << "============================" << std::endl;

  try {
    event->check_tree();
  } catch (...) {
    std::cout << " Invalid event, some ROB fragments might be truncated" << std::endl << std::endl; 
  }

  /*
    unpacking the event down to ROD fragments 
  */
 
  uint32_t nrob = event->nchildren();

  for (size_t irob = 0; irob < nrob; ++irob) {
    const uint32_t* fprob;
    event->child(fprob, irob);
    const eformat::ROBFragment<const uint32_t*> robf(fprob);

    //
    // get info on ROD
    //
    unsigned int version = robf.rod_version();
    unsigned int source_id = robf.rod_source_id();
    eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(source_id);
    unsigned int subdet_id = id.subdetector_id();
    unsigned int module_id = id.module_id();
//    int robsourceid = robf.source_id();

    bool known = m_dumpUnknown || subdet_id == 0x70  // COMMON BEAM ROD in CTB2004
                 || (subdet_id >= 0x50 && subdet_id < 0x60); // TileCal IDs

    if (!(known || m_showUnknown)) {
      continue;
    }

    std::cout << "  ROB frag ID " << std::hex << "0x" << robf.source_id() << std::dec
              << " size " << robf.fragment_size_word() << std::endl;

    // Here we should unpack the fragment.
    std::cout << "    ROD frag ID " << std::hex << "0x" << robf.rod_source_id() << std::dec
              << " size " << robf.rod_fragment_size_word() << std::endl;

    if (!known) {
      std::cout << std::endl;
      continue;
    }
    
    if ( m_dumpHeader ) {
      
      std::cout << " Format Vers.  " << std::hex << "0x" << robf.rod_version() << std::dec << std::endl;
      std::cout << " Source ID     " << std::hex << "0x" << robf.rod_source_id() << std::dec << std::endl;
      std::cout << " Source ID str " << eformat::helper::SourceIdentifier(robf.source_id()).human().c_str() << std::endl;
      std::cout << "  SubDetect ID " << std::hex << "0x" << subdet_id << std::dec << std::endl;
      std::cout << "  Module ID    " << std::hex << "0x" << module_id << std::dec << std::endl;
      std::cout << " Run number    " << (int) robf.rod_run_no() << std::endl;
      std::cout << " Level1 ID     " << robf.rod_lvl1_id() << std::endl;
      std::cout << " BCID          " << robf.rod_bc_id() << std::endl;
      std::cout << " Lvl1 TrigType " << robf.rod_lvl1_trigger_type() << std::endl;
      std::cout << " Event Type    " << robf.rod_detev_type() << std::endl;
      std::cout << " Fragment size " << robf.rod_fragment_size_word() << std::endl;
      std::cout << " Header   size " << robf.rod_header_size_word() << std::endl;
      std::cout << " Trailer  size " << robf.rod_trailer_size_word() << std::endl;
      std::cout << " N data        " << robf.rod_ndata() << std::endl;
      std::cout << " N status      " << robf.rod_nstatus() << std::endl;
      std::cout << " Status pos    " << robf.rod_status_position() << std::endl;
    }
          
    unsigned int max_allowed_size = robf.rod_fragment_size_word();
    unsigned int delta = robf.rod_header_size_word() + robf.rod_trailer_size_word();
    if (max_allowed_size >= delta) {
      max_allowed_size -= delta;
    } else {
      std::cout << " Problem with ROD data: total length " << max_allowed_size
                << " is less than " << delta << " - size of header+trailer" << std::endl;
      max_allowed_size = 0;
    }
    
    unsigned int size = robf.rod_nstatus();
    bool bad_status = (robf.rod_status_position()==0 && size > max_allowed_size);

    if (robf.rod_status_position() > 1
        || robf.rod_ndata() > max_allowed_size
        || size > max_allowed_size - robf.rod_ndata()
        || bad_status ) {
      std::cout << " Problem with status words - assuming no status words" << std::endl;

    } else if (m_dumpStatus) {

      if (size > 0) {
        const uint32_t * stat;
        robf.rod_status(stat);
        std::cout.unsetf(std::ios::fixed);
        for (unsigned int ind = 0; ind < size; ++ind) {
          std::cout << " Status[" << ind << "] = " << stat[ind] << "\t\t" << stat[ind] / 1000000. - 1. << std::endl;
        }
        std::cout << std::fixed;
      } else {
        std::cout << " No status words" << std::endl;
      }
    }
          
    if ( m_dumpData ) {

      unsigned int size = robf.rod_ndata();
      if (size > max_allowed_size) {
        if (size - robf.rod_trailer_size_word() < max_allowed_size) {
          std::cout<<" Problem with data size - assuming that trailer size is " << robf.rod_trailer_size_word()-(size-max_allowed_size)
                   <<" words instead of " << robf.rod_trailer_size_word() << " and data size is " << size << " words " << std::endl;
          max_allowed_size = size;
        } else if (size - robf.rod_trailer_size_word() == max_allowed_size) {
          std::cout<<" Problem with data size - assuming that trailer is absent "
                   << " ROD size " << robf.rod_fragment_size_word()
                   << " header size " << robf.rod_header_size_word()
                   << " data size " << size << std::endl;
          max_allowed_size = size;
        } else {
          max_allowed_size += robf.rod_trailer_size_word();
          size = max_allowed_size;
          std::cout<<" Problem with data size - assuming " << size << " words and no trailer at all"<<std::endl;
        }
        std::cout << std::endl << "Dump of whole ROB fragment 0x" << std::hex << robf.rod_source_id() << std::dec
                  << " (" << robf.rod_fragment_size_word()+robf.header_size_word() << " words)" << std::endl;
        dump_data(fprob, robf.rod_fragment_size_word()+robf.header_size_word(), version, verbosity);
      }

      if ( size > 0 ) {

        const uint32_t * data;
        if (bad_status) {
          robf.rod_status(data);
        } else {
          robf.rod_data(data);
        }
        
        if (subdet_id == 0) {
          std::cout<<" Problem with ROD frag - SubDetector ID is 0" <<std::endl;
        }
        if ((subdet_id >= 0x50 && subdet_id < 0x60) || // TileCal IDs
            subdet_id == 0x63 || // wrong id in first testbeam test runs 
            subdet_id == 0x70) { // COMMON BEAM ROD in CTB2004
          dump_digi(subdet_id,data, size, version, verbosity, source_id);
        } else if ( m_dumpUnknown ) {
          dump_data(data, size, version, verbosity);
          if (subdet_id == 0) { // try also to find normal fragments  
            dump_digi(subdet_id,data, size, version, verbosity, source_id);
          }
        }
      } else {
        std::cout << std::endl <<  std::hex << "NO DATA in ROB fragment 0x" << robf.rod_source_id() << std::dec << std::endl << std::endl;
      }
      std::cout << std::endl;
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    /* end of unpacking */
    msg(MSG::DEBUG) << "Found " << nrob << " ROB fragments, " << endmsg;

    // Execution completed.
    msg(MSG::DEBUG) << "execute() completed successfully" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//stream manipulators
std::ostream &setup0x4 (std::ostream &stream){
  stream << "0x" << std::setw(4);
  return stream;
}
std::ostream &setupMod (std::ostream &stream){
  stream << "\n mod" << std::setw(2);
  return stream;
}
std::ostream &setup0 (std::ostream &stream){
  stream << std::hex << std::setfill('0') << std::setw(8);
  return stream;
}
std::ostream &setupDec (std::ostream &stream){
  stream << std::setfill(' ') << std::dec;
  return stream;
}
std::ostream &setupPr1 (std::ostream &stream){
  stream << std::setw(5) << std::setprecision(1);
  return stream;
}
std::ostream &setupPr2 (std::ostream &stream){
  stream << std::setw(5) << std::setprecision(2);
  return stream;
}
std::ostream &setupPr3 (std::ostream &stream){
  stream << std::setw(4) << std::setprecision(1);
  return stream;
}
std::ostream &setupPr4 (std::ostream &stream){
  stream << std::setw(4) << std::setprecision(1);
  return stream;
}

void TileTBDump::dump_data(const uint32_t * data, unsigned int size, unsigned int /* version */, int /* verbosity */) {

  boost::io::ios_base_all_saver coutsave(std::cout);
  std::cout << std::endl << " Fragment data as 4 byte words:" << std::hex << std::setfill('0') ;

  for (unsigned int cnter = 0; cnter < size; ++cnter) {
    if (!(cnter % 8)) std::cout << std::endl;
    std::cout << std::setw(8) << (*data++) << " ";
  }

  std::cout <<std::setfill(' ') << std::dec << std::endl << std::endl;

}

void TileTBDump::dump_digi(unsigned int subdet_id, const uint32_t* roddata, unsigned int rodsize
                           , unsigned int version, int verbosity, unsigned int robsourceid) {

  int s, c, f, nfrag, ngain, nchan, nsamp, size, ch, extra = 0, pmt, fragType, nhits = 0;
  int id, type, rflag, unit, pulse, nsmpl, algor, niter;
  unsigned int* data;
  unsigned short time, flag, prev, edge, chan, bad/*, res1,last,res2*/;
  char fr[2] = { 'F', 'R' };
  char gb[2] = { 'G', 'B' };
  std::string unitName[4] = { "ADC count", "pCb", "Cs pCb", "MeV" };
  std::string shapeName[4] = { "Phys", "Laser", "CIS", "Simul" };
  std::string algName[8] = { "Unknown", "OF1", "OF2", "Fit", "ManyAmps", "Flat", "Alg6", "Alg7" };
  boost::io::ios_base_all_saver coutsave(std::cout);

  T_RodDataFrag* frag[MAX_ROD_FRAG];
  T_TileRawComp rawcomp[MAX_DIGI_CHAN];
  T_TileDigiChannel channel[MAX_DIGI_CHAN];
  T_TileRecoChannel recochan[MAX_DIGI_CHAN];
  T_TileRecoCalib recocalib[MAX_DIGI_CHAN];
  T_TileRecoQuality DQword;

  bool isFrag5 = false;
  uint32_t* ofw = 0;
  std::vector<unsigned int> OFC;

  TileRawChannel2Bytes5::TileChanData Frag5Data[48];
  
  bool beamROD = (subdet_id == 0x50 || subdet_id >= 0x60);
  if (subdet_id >= 0x60) {
    version = 0; // reset version in COMMON BEAM ROD
                 // don't expect different versions there
  } else {
    version &= 0xFFFF; // keep just minor version number
  }

  find_frag(roddata, rodsize, version, verbosity, frag, &nfrag);

  if (verbosity > 9) return;

  for (f = 0; f < nfrag; ++f) {

    id = frag[f]->id & 0xFFFF;
    type = (frag[f]->id >> 16) & 0xFF;
    rflag = (frag[f]->id) >> 24;
    unit = (rflag & 0xC0) >> 6;
    pulse = (rflag & 0x30) >> 4;
    nsmpl = (rflag & 0x08) >> 3;
    algor = (rflag & 0x04) >> 2;
    niter = (rflag & 0x03);
    size = frag[f]->size - m_sizeOverhead;
    data = frag[f]->data;

    if (type==0x40 || type==0x41 || type==0x42){

      unsigned char * adc;
      unsigned short * result;
      int tmdb_ch1 = std::min(5U,((robsourceid)>>16)&0xF);
      bool EB = (tmdb_ch1>2);
      int nmod = (EB)?8:4; // we have 8 modules per fragment in ext.barrel, 4 modules in barrel
      int tmdb_ch2 = (((robsourceid))&0xF)*nmod;
      const char * dr56EB[10] = { "D5-L","D5-R","D6-L","D6-R","D4-L","D4-R","XX-X","XX-X","XX-X","XX-X" };
      const char * dr56LB[10] = { "D0-x","D1-L","D1-R","D2-L","D2-R","D3-L","D3-R","B8-L","B8-R","XX-X" };
      const char ** dr56rl = (EB) ? dr56EB : dr56LB;
      const char * ch11[6] = { "AUX","LBA","LBC","EBA","EBC","UNK" };
      const char * ch12[6] = { "aux","lba","lbc","eba","ebc","unk" };
      const char * dr56hlEB[4] = {" D6L "," D6H "," D56L"," D56H"};
      const char * dr56hlLB[4] = {" DxL "," DxH "," DxxL"," DxxH"};
      const char ** dr56hl = (EB) ? dr56hlEB : dr56hlLB;
      const char * tit[4] = {"TMDB digits","TMDB energy","TMDB decision","Unknown"};

      std::cout << std::hex << std::endl << tit[type&3] <<" fragment 0x" << type << " vers 0x"<< id << ", "
                << std::dec << size << " words found"<< std::endl << std::endl;

      int nchmod = 4;
      int nsamp = 7;
      int nch = 32;
      int ntd = (EB) ? 3 : 1;
      int ntdl = (EB) ? 9 : 5;
      int count = 1;
      switch (type) {

        case 0x40: 
            //nsamp=4*size/nch; // here we assume that number of channels is fixed - doesn't work for simulated data
            nch=4*size/nsamp; // instead of assuming fixed number of channels assume fixed number of samples
            nchmod = nch/nmod;
            std::cout << "ch      cell   ";
            for (int ind=nsamp; ind>0; --ind) {
              std::cout << "    S"<<ind;
            }
            std::cout << std::endl;
            adc = reinterpret_cast<unsigned char *>(data);
            for (int pword=0;pword<nch;++pword) {
              int pword1=pword%nchmod;
              if (!EB && nchmod==8) {
                if (count&1) {
                  if (pword1==0) pword1=9;
                  else pword1 -= 1;
                } else {
                  if (pword1>6) pword1=9;
                }
              } else {
                if (pword1>9) pword1=9;
              }
              std::cout << std::setw(2) << pword << " | " << ch11[tmdb_ch1] <<std::setfill('0')<<std::setw(2) <<tmdb_ch2+count
                        << "-" <<std::setfill(' ')<<std::setw(4)<<dr56rl[pword1];
              for (int ind=nsamp-1; ind>-1; --ind) {
                std::cout << " | " << std::setw(3) << (  static_cast<unsigned>(adc[pword+nch*ind]) );
              }
              std::cout << std::endl;
              if ((pword+1)%nchmod==0) count+=1;
            }
            break;

        case 0x41: 
            nch = size; // one word per channel
            nchmod = nch/nmod;
            std::cout << "ch      cell      energy" << std::endl; 
            for (int pword=0;pword<size;++pword) {
              int pword1=pword%nchmod;
              if (!EB && nchmod==8) {
                if (count&1) {
                  if (pword1==0) pword1=9;
                  else pword1 -= 1;
                } else {
                  if (pword1>6) pword1=9;
                }
              } else {
                if (pword1>9) pword1=9;
              }
              std::cout << std::setw(2) << pword<< " | " << ch11[tmdb_ch1] <<std::setfill('0')<<std::setw(2) <<tmdb_ch2+count
                        << "-" <<std::setfill(' ')<<std::setw(4)<<dr56rl[pword1]
                        << " | "<< std::setw(6) << static_cast<int>(data[pword])
                        <<  std::endl;
              if ((pword+1)%nchmod==0) count+=1;
            }
            break;

        case 0x42:
            nchmod = nch/nmod;
            std::cout << "nn   name   " << dr56hl[3] << dr56hl[2]
                      << dr56hl[1] << dr56hl[0] << std::endl; 
            result = reinterpret_cast<unsigned short *>(data);
            if (size!=2) ntd=size*2;
            for (int pword=0;pword<ntd;++pword) {
              count=(EB)?pword*3:pword*4+1;
              unsigned short r=result[pword];
              for(int pqword=0;pqword<4;++pqword){
                  std::cout << std::setw(2) << pqword+pword*4 << " | " << ((count>0&&count<ntdl)?ch11[tmdb_ch1]:ch12[tmdb_ch1])
                          << std::setfill('0') << std::setw(2) << tmdb_ch2+count
                          << std::setfill(' ') << std::setw(5)<<((r>>3)&1) << std::setw(5)<<((r>>2)&1)
                          << std::setw(5)<<((r>>1)&1) << std::setw(5)<<(r&1) << std::endl;
                r>>=4;
                ++count;
              } 
            }
            break;
        default: 
            dump_data((uint32_t*) data, size, version, verbosity);
      }

    } else if (id < 0x100 || beamROD) { // BEAM fragments
      id &= 0xFF; // set proper frag ID in Beam frag for old data

      switch (id) {

        case COMMON_TOF_FRAG:
          std::cout << "\nBeam ToF TDC, " << size << " hits found"; 
          prev = 0xFF;
          for (c = 0; c < size; ++c) {
            time = data[c] & 0x1FFF;
            unsigned short res1 = (data[c] >> 13) & 0x1;
            chan = (data[c] >> 16) & 0x07FF;
            if (prev != chan) {
              std::cout << "\n ch" << std::setw(3) << chan << ":"; 
              nhits = 0;
              prev = chan;
            } else if (nhits % 8 == 0) {
              std::cout << "\n       ";
            }
            ++nhits;
            if (res1) {
              std::cout << " U" << std::setw(4) << time; 
            } else {
              std::cout << "  " << std::setw(4) << time;
            }
          }
          std::cout << std::endl;
          break;

        case BEAM_TDC_FRAG:
        case COMMON_TDC1_FRAG:
        case COMMON_TDC2_FRAG:
          std::cout << "\nBeam TDC 0x" << std::setfill('0') << std::hex << std::setw(2) << id << setupDec << ", " << size << " hits found";
          prev = 0xFF;
          for (c = 0; c < size; ++c) {
            time = data[c] & 0xFFFF;
            flag = data[c] >> 16;
            edge = flag & 0x01;
            chan = (flag >> 1) & 0x0F;
            bad = (flag >> 5) & 0x01;
            //res1 = (flag >> 6) & 0x01;
            //last = (flag >> 7) & 0x01;
            //res2 = (flag >> 8) & 0x0F;
            if (prev != chan) {
              std::cout << "\n ch" << std::setw(3) << chan << ":";
              nhits = 0;
              prev = chan;
            } else if (nhits % 8 == 0) {
              std::cout << "\n       ";
            }
            ++nhits;
            if (bad) {
              std::cout << " " << gb[bad] << fr[edge] << std::setw(6) << time;
            } else {
              std::cout << "  " << fr[edge] << std::setw(6) << time;
            }
          }
          std::cout << std::endl;
          break;

        case BEAM_ADC_FRAG:
        case COMMON_ADC1_FRAG:
        case COMMON_ADC2_FRAG:
          if (BEAM_ADC_FRAG == id) {
            std::cout << "\nTile Beam ADC, " << size << " channels found";
          } else {
            std::cout << "\nBeam ADC 0x" << std::hex << std::setfill('0') << std::setw(2) << id << setupDec << ", " << size << " channels found";
          }
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c / 8 << ":";
            std::cout << std::setw(9) << data[c];
          }
          std::cout << std::endl;
          break;

        case MUON_ADC_FRAG:
          std::cout << "\nMuon ADC, " << size << " channels found";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c / 8 << ":";
            std::cout << std::setw(9) << data[c];
          }
          std::cout << std::endl;
          break;

        case ADDR_ADC_FRAG:
          std::cout << "\nMuon2 ADC, " << size << " channels found";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c / 8 << ":";
            std::cout << std::setw(9) << data[c];
          }
          std::cout << std::endl;
          break;

        case LASE_PTN_FRAG:
        case COMMON_PTN_FRAG:
          if (LASE_PTN_FRAG == id) {
            std::cout<<"\nLaser Pattern Unit, " << size << " words found (hex)";
          } else {
            std::cout<<"\nCommon Pattern Unit, " << size << " words found (hex)";
          }
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8 << ":";
            std::cout <<  " 0x" << setup0 << data[c] << setupDec << std::endl;
          }
          std::cout << std::endl;
          break;

        case LASER_OBJ_FRAG:

          if (size !=25 && size != 26) {
            std::cout<<"\nLASTROD Laser Object, " << size << " words found (hex)";
            for (c = 0; c < size; ++c) {
              if (c % 8 == 0) std::cout << setupMod << c/8 << ":";
              std::cout <<  " 0x" << setup0 << data[c] << setupDec;
            }
            std::cout<<std::endl<<std::endl;

            if (size != 31) {
              std::cout<<"CRITICAL ERROR! Unknown format!"<<std::endl;
            } else {
              unsigned int * p;
              int Counter = 0;
              int Filter = 0, ReqAmp = 0, MeasAmp = 0, Delay = 0, TDC1 = 0, TDC2 = 0;
              p = data;
              Counter = *p;
              p++;
              std::cout << " Laser Counter: " << std::setw(5) << Counter << std::endl;
    
              if ((*p & 0xFF000000) == 0x20000000) {
                ReqAmp = *p & 0xFFFF;
                if (version > 1) {
                  Filter = (((*p >> 16) & 7) ^ 7) + 2;
                  if (Filter > 8) Filter -= 8;
                  std::cout << "  Filter Wheel: " << std::setw(5) << Filter << std::endl;
                }
                std::cout << "  Required Amp: " << std::setw(5) << ReqAmp << std::endl;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 14." << std::endl;
              }
              p++;
    
              if ((*p & 0xFF000000) == 0x21000000) {
                Delay = (*p >> 12) & 0xFFF;
                MeasAmp = *p & 0xFFF;
                std::cout << "  Measured Amp: " << std::setw(5) << MeasAmp << std::endl;
                std::cout << "         Delay: " << std::setw(5) << Delay << std::endl;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 15." << std::endl;
              }
              p++;
    
              bool TDCPrint = true;
    
              if ((*p & 0xFF000000) == 0x22000000) {
                if (version == 1) {
                  TDC1 = (*p >> 16) & 0xF;
                  TDC2 = (*p >> 20) & 0xF;
                } else {
                  TDC1 = *p & 0xFFFF;
                }
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 16." << std::endl;
                TDCPrint = false;
              }
              p++;
              if ((*p & 0xFF000000) == 0x23000000) {
                if (version == 1) {
                  TDC1 = (TDC1 << 12) + (*p & 0xFFF);
                  TDC2 = (TDC2 << 12) + ((*p >> 12) & 0xFFF);
                } else {
                  TDC2 = *p & 0xFFFF;
                }
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 17." << std::endl;
                TDCPrint = false;
              }
              p++;
              if (TDCPrint) {
                std::cout << "     TDC1 data: " << std::setw(5) << TDC1 << std::endl;
                std::cout << "     TDC2 data: " << std::setw(5) << TDC2 << std::endl;
              }
    
              int chan0 = 0, chan1 = 0, chan2 = 0, chan3 = 0, chan4 = 0, chan5 = 0, chan6 = 0, chan7 = 0;
    
              if ((*p & 0xFF000000) == 0x44000000) {
                chan0 = (*p & 0xFFF) ^ 0xFFF;
                chan1 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 18." << std::endl;
              }
              p++;
    
              if ((*p & 0xFF000000) == 0x45000000) {
                chan2 = (*p & 0xFFF) ^ 0xFFF;
                chan3 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 19." << std::endl;
              }
              p++;
    
              if ((*p & 0xFF000000) == 0x46000000) {
                chan4 = (*p & 0xFFF) ^ 0xFFF;
                chan5 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 20." << std::endl;
              }
              p++;
    
              if ((*p & 0xFF000000) == 0x47000000) {
                chan6 = (*p & 0xFFF) ^ 0xFFF;
                chan7 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
              } else {
                std::cout << "ERROR in Laser Fragment: decoding word 21." << std::endl;
              }
              p++;
    
              int diode1_Ped = 0, diode1_PedRMS = 0
                  , diode2_Ped = 0, diode2_PedRMS = 0
                  , diode3_Ped = 0, diode3_PedRMS = 0
                  , diode4_Ped = 0, diode4_PedRMS = 0
                  , PMT1_Ped = 0, PMT1_PedRMS = 0
                  , PMT2_Ped = 0, PMT2_PedRMS = 0;
    
              diode1_Ped = (*p >> 16) & 0xFFFF;
              diode1_PedRMS = *p & 0xFFFF;
              p++;
    
              diode2_Ped = (*p >> 16) & 0xFFFF;
              diode2_PedRMS = *p & 0xFFFF;
              p++;
    
              diode3_Ped = (*p >> 16) & 0xFFFF;
              diode3_PedRMS = *p & 0xFFFF;
              p++;
    
              diode4_Ped = (*p >> 16) & 0xFFFF;
              diode4_PedRMS = *p & 0xFFFF;
              p++;
    
              PMT1_Ped = (*p >> 16) & 0xFFFF;
              PMT1_PedRMS = *p & 0xFFFF;
              p++;
    
              PMT2_Ped = (*p >> 16) & 0xFFFF;
              PMT2_PedRMS = *p & 0xFFFF;
              p++;
    
              time_t Ped_Last_Run = *p;
              p++;
    
              int diode1_alpha = 0, diode1_alphaRMS = 0
                  , diode2_alpha = 0, diode2_alphaRMS = 0
                  , diode3_alpha = 0, diode3_alphaRMS = 0
                  , diode4_alpha = 0, diode4_alphaRMS = 0;
    
              diode1_alpha = (*p >> 16) & 0xFFFF;
              diode1_alphaRMS = *p & 0xFFFF;
              p++;
    
              diode2_alpha = (*p >> 16) & 0xFFFF;
              diode2_alphaRMS = *p & 0xFFFF;
              p++;
    
              diode3_alpha = (*p >> 16) & 0xFFFF;
              diode3_alphaRMS = *p & 0xFFFF;
              p++;
    
              diode4_alpha = (*p >> 16) & 0xFFFF;
              diode4_alphaRMS = *p & 0xFFFF;
              p++;
    
              time_t Alpha_Last_Run = *p;
              p++;
    
              time_t PedAlpha_Last_Run(0);
    
              int diode1_PedAlpha = 0, diode1_PedAlphaRMS = 0
                  , diode2_PedAlpha = 0, diode2_PedAlphaRMS = 0
                  , diode3_PedAlpha = 0, diode3_PedAlphaRMS = 0
                  , diode4_PedAlpha = 0, diode4_PedAlphaRMS = 0;
    
              if (version > 1) {
                diode1_PedAlpha = (*p >> 16) & 0xFFFF;
                diode1_PedAlphaRMS = *p & 0xFFFF;
                p++;
    
                diode2_PedAlpha = (*p >> 16) & 0xFFFF;
                diode2_PedAlphaRMS = *p & 0xFFFF;
                p++;
    
                diode3_PedAlpha = (*p >> 16) & 0xFFFF;
                diode3_PedAlphaRMS = *p & 0xFFFF;
                p++;
    
                diode4_PedAlpha = (*p >> 16) & 0xFFFF;
                diode4_PedAlphaRMS = *p & 0xFFFF;
                p++;
    
                PedAlpha_Last_Run = *p;
                p++;
              }
    
              std::cout << std::endl << "           |  ADC  | Pedestal(RMS) |  Alpha (RMS)  | PedAlpha(RMS) |" << std::endl;
              if (version == 1){
                std::cout << "   Diode 1 | " << std::setw(5) << chan0 << " | " << setupPr1 << diode1_Ped / 10.0 << " (" << setupPr2 << diode1_PedRMS / 100.0 << ") | " << setupPr1 << diode1_alpha / 10.0 << " (" << setupPr2 << diode1_alphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 2 | " << std::setw(5) << chan1 << " | " << setupPr1 << diode2_Ped / 10.0 << " (" << setupPr2 << diode2_PedRMS / 100.0 << ") | " << setupPr1 << diode2_alpha / 10.0 << " (" << setupPr2 << diode2_alphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 3 | " << std::setw(5) << chan2 << " | " << setupPr1 << diode3_Ped / 10.0 << " (" << setupPr2 << diode3_PedRMS / 100.0 << ") | " << setupPr1 << diode3_alpha / 10.0 << " (" << setupPr2 << diode3_alphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 4 | " << std::setw(5) << chan3 << " | " << setupPr1 << diode4_Ped / 10.0 << " (" << setupPr2 << diode4_PedRMS / 100.0 << ") | " << setupPr1 << diode4_alpha / 10.0 << " (" << setupPr2 << diode4_alphaRMS / 100.0 << ") |" << std::endl;
              }  else {
                std::cout << "   Diode 1 | " << std::setw(5) << chan0 << " | " << setupPr1 << diode1_Ped / 10.0 << " (" << setupPr2 << diode1_PedRMS / 100.0 << ") | " << setupPr1 << diode1_alpha / 10.0 << " (" << setupPr2 << diode1_alphaRMS / 100.0 << ") | " << setupPr1 << diode1_PedAlpha / 10.0 << " (" << setupPr2 << diode1_PedAlphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 2 | " << std::setw(5) << chan1 << " | " << setupPr1 << diode2_Ped / 10.0 << " (" << setupPr2 << diode2_PedRMS / 100.0 << ") | " << setupPr1 << diode2_alpha / 10.0 << " (" << setupPr2 << diode2_alphaRMS / 100.0 << ") | " << setupPr1 << diode2_PedAlpha / 10.0 << " (" << setupPr2 << diode2_PedAlphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 3 | " << std::setw(5) << chan2 << " | " << setupPr1 << diode3_Ped / 10.0 << " (" << setupPr2 << diode3_PedRMS / 100.0 << ") | " << setupPr1 << diode3_alpha / 10.0 << " (" << setupPr2 << diode3_alphaRMS / 100.0 << ") | " << setupPr1 << diode3_PedAlpha / 10.0 << " (" << setupPr2 << diode3_PedAlphaRMS / 100.0 << ") |" << std::endl;
                std::cout << "   Diode 4 | " << std::setw(5) << chan3 << " | " << setupPr1 << diode4_Ped / 10.0 << " (" << setupPr2 << diode4_PedRMS / 100.0 << ") | " << setupPr1 << diode4_alpha / 10.0 << " (" << setupPr2 << diode4_alphaRMS / 100.0 << ") | " << setupPr1 << diode4_PedAlpha / 10.0 << " (" << setupPr2 << diode4_PedAlphaRMS / 100.0 << ") |" << std::endl;
              }
     
              std::cout << "   PMT 1   | " << std::setw(5) << chan4 << " | " << setupPr1 << PMT1_Ped / 10.0 <<" (" << setupPr2 << PMT1_PedRMS / 100.0 << ") |       x       |       x       |" << std::endl;
              std::cout << "   PMT 2   | " << std::setw(5) << chan5 << " | " << setupPr1 << PMT2_Ped / 10.0 <<" (" << setupPr2 << PMT2_PedRMS / 100.0 << ") |       x       |       x       |" << std::endl;
              std::cout << "   InjChrg | " << std::setw(5) << chan6 << " |       x       |       x       |       x       |" << std::endl;
              std::cout << "   Spare   | " << std::setw(5) << chan7 << " |       x       |       x       |       x       |" << std::endl;
    
              std::cout << std::endl << "          |  Date & Time (GMT)  |  Date & Time (CERN)" << std::endl;
    
              struct tm* TimeInfo;
              char buf[80];
              TimeInfo = gmtime(&Ped_Last_Run);
              strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
    
              std::cout << " Pedestal | " << buf << " | " << cern_local_time(Ped_Last_Run) << std::endl;
    
              TimeInfo = gmtime(&Alpha_Last_Run);
              strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
    
              std::cout << "    Alpha | " << buf << " | " << cern_local_time(Alpha_Last_Run) << std::endl;
    
              TimeInfo = gmtime(&PedAlpha_Last_Run);
              strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
    
              std::cout << " PedAlpha | " << buf << " | " << cern_local_time(PedAlpha_Last_Run) << std::endl;
    
              int diodeTemp = 0, secsDiodeT = 0
                  , boxTemp = 0, secsBoxT = 0
                  , boxHum = 0, secsBoxH = 0
                  , gasFlow = 0, secsGasF = 0;
    
              diodeTemp = *p & 0xFFF;
              secsDiodeT = (*p >> 12) & 0xFFFFF;
              p++;
              boxTemp = *p & 0xFFF;
              secsBoxT = (*p >> 12) & 0xFFFFF;
              p++;
              boxHum = *p & 0xFFF;
              secsBoxH = (*p >> 12) & 0xFFFFF;
              p++;
              gasFlow = *p & 0xFFF;
              secsGasF = (*p >> 12) & 0xFFFFF;
              p++;
    
              std::cout << std::endl << "                    |   Time  | Value |" << std::endl;
              std::cout << "   Laser diode temp | " << std::setw(7) << secsDiodeT << " |  " <<  setupPr3 << diodeTemp / 10.0 << " |" << std::endl;
              std::cout << "     Laser box temp | " << std::setw(7) << secsBoxT   << " |  " <<  setupPr3 << boxTemp / 10.0 << " |" << std::endl;
              std::cout << " Laser box humidity | " << std::setw(7) << secsBoxH   << " |  " <<  setupPr3 << boxHum / 10.0 << " |" << std::endl;
              std::cout << " Laser box gas flow | " << std::setw(7) << secsGasF   << " |  " <<  setupPr3 << gasFlow / 10.0 << " |" << std::endl;
    
              std::bitset<32> PLCstatus = *p;
              int PLCtime = (*p >> 12) & 0xFFFFF;
              p++;
    
              int Alpha0 = PLCstatus[0];
              int Alpha1 = PLCstatus[1];
              int Alpha2 = PLCstatus[2];
              int LV = PLCstatus[3];
              int HV1 = PLCstatus[4];
              int HV2 = PLCstatus[5];
              int ShOpen = PLCstatus[6];
              int ShClose = PLCstatus[7];
              int Ilock = PLCstatus[8];
              int Alarm = PLCstatus[9];
              int Err = PLCstatus[11];
    
              const char *YesNo[2] = {" No","Yes"};
              const char *OnOff[2] = {"Off"," On"};

              std::cout << std::endl << "    Time  | Err | Alarm | Ilock | ShClose | ShOpen | HV2 | HV1 |  LV | Alpha2 | Alpha1 | Alpha0 |" 
                        << std::endl << "  " << std::setw(7) << PLCtime 
                        << " | " << YesNo[Err] << " |  " << OnOff[Alarm] << "  |  " << OnOff[Ilock] << "  |   " << YesNo[ShClose]
                        << "   |  " << YesNo[ShOpen] << "   | " << OnOff[HV2] << " | " << OnOff[HV1] << " | "  << OnOff[LV] 
                        << " |  " << OnOff[Alpha2] << "   |  " << OnOff[Alpha1] << "   |  " << OnOff[Alpha0] << "   |"  << std::endl;
    
              if (p != &data[size]) {
                std::cout << "CRITICAL ERROR! Wrong size" << std::endl;
              }
            }
            break;
          }
          /* FALLTHROUGH */

        case LASERII_OBJ_FRAG:

          {
            std::cout<<"\nLASTROD New Laser Object, " << size << " words found" << std::endl;
    
            bool first_half_present  = (size == 25 || size ==  26 || size == 128  || size == 129);
            bool second_half_present = (size == 99 || size == 100 || size == 128  || size == 129);
    
            if ( ! (first_half_present || second_half_present) ) {
              std::cout << "CRITICAL ERROR! Unknown format!" << std::endl;
            } else {
    
              const char *name[17] = {"      PhotoDiode 0",
                                      "      PhotoDiode 1",
                                      "      PhotoDiode 2",
                                      "      PhotoDiode 3",
                                      "      PhotoDiode 4",
                                      "      PhotoDiode 5",
                                      "      PhotoDiode 6",
                                      "      PhotoDiode 7",
                                      "      PhotoDiode 8",
                                      "      PhotoDiode 9",
                                      "             PMT 0",
                                      "    External CIS 0",
                                      "      Internal CIS",
                                      "      Diode Phocal",
                                      "             PMT 1",
                                      "    External CIS 1",
                                      "         TDC 1 & 0"
              };
              
              time_t tim;
              struct tm* TimeInfo;
              char buf[80];
    
              const unsigned int * p = data;
    
              if (first_half_present) {
    
                // p[0]    00 00 00 tt    Daq Type
                // p[1]    nn nn nn nn    Laser Count    
                // p[2]    rr rr mm mm    rrrr = Requested Intensity    mmmm = measured intensity 
                // p[3]    00 0f dd dd    f = filter    dddd = Delay Slama
                // p[4]    00 00 ll ll    Linearity DAC Value
    
                std::cout << std::endl << "      DAQ type: " << std::setw(5) << (data[0]%0xFF) << std::endl;
                std::cout << " Laser Counter: " << std::setw(5) << data[1] << std::endl;
                std::cout << "  Required Amp: " << std::setw(5) << (data[2]>>16) << std::endl;
                std::cout << "  Measured Amp: " << std::setw(5) << (data[2]&0xFFFF) << std::endl;
                std::cout << "  Filter Wheel: " << std::setw(5) << (data[3]>>16 & 0x000F) << std::endl;
                std::cout << "         Delay: " << std::setw(5) << (data[3]&0xFFFF) << std::endl;
                std::cout << " Linearity DAC: " << std::setw(5) << (data[4]&0xFFFF) << std::endl;
                std::cout << std::endl;
                
                p = data+5;
                // decode 32 ADC half-words (16 low & high channels)
                std::cout << "                                       HG    LG" << std::endl;
                for (int n=0; n<17; ++n) {
                  // ll ll hh hh    ADC Channel 0 & 1 (Low & High Gain)
                  std::cout << name[n] << ": " << std::setw(5) << ((*p)&0xFFFF) << std::setw(6) << ((*p)>>16) << "  =>  " << std::setw(5) << (8500-((*p)&0xFFFF)) << std::setw(6) << (8500-((*p)>>16))<< std::endl;
                  ++p;
                }
                
                // status word
                // 27: 1 if HOLA link not full
                // 26: 1 if HOLA link not down
                // 24: 1 if bad clock from VME (16MHz)
                // 22: 1 if bad clock from TTC (40MHz)
                // 20: 1 if TTC double error
                // 19: 1 if TTC single error
                // 16: 1 if PLL locked for VME clock (16MHz)
                // 15: 1 if PLL locked for TTC clock (40MHz)
                // 10: 1 if fault from laser temperature sensor
                //  9: 1 if laser diode off
                //  8: 1 if interlock closed
                //  6: 1 if combined run finished
                //  1: 1 if busy
                //  0: 1 if busy for longer than 5s 
    
    
                std::bitset<32> status = *(p++);
                const char *YesNo[2] = {" No","Yes"}; 
                std::cout << std::endl;
                std::cout << "| Link| Link| Bad | Bad | TTC | TTC | PLL | PLL |Laser|Laser|Inter| Comb| Busy| Long|" << std::endl;
                std::cout << "| not | not | VME | TTC |doubl|singl| lock| lock| temp| diod| lock| run |     | busy|" << std::endl;
                std::cout << "| full| down|clock|clock|error|error| VME | TTC |fault| off |close| fini|     |>5sec|" << std::endl;
                std::cout << "|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|" << std::endl;
                std::cout << "| " <<  YesNo[status[27]] << " | " << YesNo[status[26]] << " | " << YesNo[status[24]] << " | " << YesNo[status[22]] << " | " << YesNo[status[20]] << " | " << YesNo[status[19]] << " | " << YesNo[status[16]] << " | " << YesNo[status[15]] << " | " << YesNo[status[10]] << " | " << YesNo[status[9]] << " | " << YesNo[status[8]] << " | "  << YesNo[status[6]] << " | " << YesNo[status[1]] << " | " << YesNo[status[0]] << " |" << std::endl;
                 
                std::cout << std::endl << "  FPGA Global Status: 0x" << std::hex 
                          << status.to_ulong() << " => " << status.to_string() << std::dec << std::endl; 
                tim = *(p++);
                TimeInfo = gmtime(&tim);
                strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
                std::cout << "DCS Time Stamp (GMT): " << buf << " => " << cern_local_time(tim) << std::endl;
                std::cout << " PhotoDiode Polarity: " << std::setw(5) << (*p++) << std::endl;
                p+=4; // skip 4 free words
              }
    
              if (second_half_present) {
                std::cout << "    Calibration Type: " << std::setw(5) << (*p++) << std::endl;
                tim = p[97];
                TimeInfo = gmtime(&tim);
                strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
                std::cout << "    Time Stamp (GMT): " << buf << " => " << cern_local_time(tim) << std::endl;
                
                double nevt = double(p[96]);
                if (p[96]==0 || (p[96]==3072 && (*p)<21504000)) {
                  std::cout << "    Number of events: " << p[96] << " => assuming 1024" << std::endl << std::endl;
                  nevt=1024.; 
                } else {
                  std::cout << "    Number of events: " << std::setw(5) << p[96] << std::endl << std::endl;
                }
                std::cout << "                                                                                             pedHG       rmsHG    pedLG       rmsLG" << std::endl;
                for (int n=0; n<16; ++n) {
    
                  uint32_t sum0 = *(p++);
                  uint32_t sum1 = *(p++);
      
                  uint32_t lsb0 = *(p++);
                  uint32_t msb0 = *(p++); 
                  uint32_t lsb1 = *(p++);
                  uint32_t msb1 = *(p++);
                  uint64_t ssq0 = ((uint64_t) msb0 << 32) | ((uint64_t) lsb0);
                  uint64_t ssq1 = ((uint64_t) msb1 << 32) | ((uint64_t) lsb1);
    
                  // COMPUTE MEAN AND STANDARD DEVIATION
                  double ped0 = double(sum0)/nevt;
                  double ped1 = double(sum1)/nevt;
    
                  double rms0 = double(ssq0)/nevt - ped0*ped0;
                  double rms1 = double(ssq1)/nevt - ped1*ped1;
                  if (rms0>0.0) rms0 = sqrt(rms0);
                  if (rms1>0.0) rms1 = sqrt(rms1);
    
                  std::cout << name[n] << ":" << std::setw(11) << sum0 << std::setw(11) << sum1 << std::setw(11) << msb0 << std::setw(11) << lsb0 << std::setw(11) << msb1 << std::setw(11) << lsb1 << "  =>  " << std::setw(7) << std::setprecision(1) << 8500.-ped0 << " +/- " << std::setw(7) << std::setprecision(1) << rms0 << "  " << std::setw(7) << std::setprecision(1) << 8500.-ped1 << " +/- " << std::setw(7) << std::setprecision(1) << rms1 << std::endl;
                }
              }
            }
            break;
          }

        case LASE_ADC_FRAG:
          std::cout << "\nLaser ADC, " << size << " channels found";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
            std::cout << std::setw(9) << data[c];
          }
          std::cout << std::endl;
          break;

        case ECAL_ADC_FRAG:
          std::cout << "\nECAL ADC, " << size << " channels found";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
            std::cout << std::setw(9) << data[c];
          }
          std::cout << std::endl;
          break;

        case DIGI_PAR_FRAG:
          std::cout << "\nDigi parameters, " << size << " words found";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
            std::cout << std::setw(11) << data[c];
          }
          std::cout << std::endl;

          if (size == 4 || size == 16 || size == 110) {
            unsigned int *p = data;
            int Counter = 0, Mode = 0, Samples = 0, Pipeline = 0, I3Delay = 0, Event = 0, Phase = 0,
                DAC = 0, Capacity = 0, Card = 0, RunType = 0, microsec = 0;
            time_t Time;
            bool DefFormat = true;
            if (size == 4) DefFormat = false;
            if (!DefFormat) {
              Counter = *(p++);
              Samples = *(p++);
              Pipeline = *(p++);
              RunType = *(p++);
            } else {
              Mode = *(p++);
              Samples = *(p++);
              Pipeline = *(p++);
              I3Delay = *(p++);
              Event = *(p++);
              Phase = *(p++);
              DAC = *(p++);
              Capacity = *(p++);
              Card = *(p++);
              p++; //Reserved
              Time = *(p++);
              microsec = *(p++);
              RunType = *(p++);
              p++; //Reserved
              p++; //Reserved
              Counter = *(p++);
            }
            const char *RunTypeText;
            switch (RunType) {
              case 1: RunTypeText = "Physics"; break;
              case 2: RunTypeText = "Laser"; break;
              case 4: RunTypeText = "Pedestals"; break;
              case 8: RunTypeText = "CIS mono"; break;
              case 16: RunTypeText = "CIS scan"; break;
              default: RunTypeText = "Unknown"; break;
            }

            std::cout << "\n  CIS Counter: " << std::setw(3) << Counter<< std::endl;
            std::cout << "\n     Run Type: " << std::setw(3) << RunType << " (" << RunTypeText << ")";
            if (!DefFormat) {
              std::cout << "\n      Samples: " << std::setw(3) << Samples;
              std::cout << "\n     Pipeline: " << std::setw(3) << Pipeline << std::endl;
            } else {
              const char* ModeText;
              switch (Mode) {
                case 0:
                  ModeText = "Normal";
                  break;
                case 1: ModeText = "Calibration"; break;
                default: ModeText = "Unknown"; break;
              }

              struct tm* TimeInfo;
              char buf[80];
              TimeInfo = gmtime(&Time);
              strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);
              std::cout << std::endl;
              std::cout << "   Time (GMT): " << buf << " => " << cern_local_time(Time) << std::endl;
              std::cout << "    Microsec.: " << microsec << std::endl << std::endl;
              std::cout << "         Mode: " << std::setw(3) << Mode << " (" << ModeText << ")" << std::endl;
              std::cout << "      Samples: " << std::setw(3) << Samples << std::endl;
              std::cout << "     Pipeline: " << std::setw(3) << Pipeline << std::endl;
              std::cout << "      I3Delay: " << std::setw(3) << I3Delay << std::endl;
              std::cout << "        Event: " << std::setw(3) << Event << std::endl;
              std::cout << "        Phase: " << std::setw(3) << Phase << std::endl;
              std::cout << "          DAC: " << std::setw(3) << DAC << std::endl;
              std::cout << "     Capacity: " << std::setw(3) << Capacity << " pF" << std::endl;
              std::cout << "         Card: " << std::setw(3) << Card << std::endl;
              
              if (size > 16) {
                int last = size - 1;
                for (; last > 15; --last) {
                  if (data[last] != 0) break;
                }
                if (last > 15) {
                  std::cout << "\n Remaing " << last - 15 << " non-zero words (hex):";
                  for (c = 16; c <= last; ++c) {
                    if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
                    std::cout << std::hex << std::setw(11) << data[c] << std::dec;
                  }
                  std::cout << std:: endl;
                }
              }
            }
          } else {
            std::cout << "CRITICAL ERROR! Unknown format!" << std::endl;
          }
          break;

        case ADD_FADC_FRAG:
          std::cout << "\nAdder FADC, " << size << " words found (hex)" ;
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
            std::cout << std::hex << std::setw(9) << data[c] << std::dec;
          }
          std::cout << std::endl;
          break;

        case COIN_TRIG1_FRAG:
        case COIN_TRIG2_FRAG:
        case COIN_TRIG3_FRAG:
        case COIN_TRIG4_FRAG:
        case COIN_TRIG5_FRAG:
        case COIN_TRIG6_FRAG:
        case COIN_TRIG7_FRAG:
        case COIN_TRIG8_FRAG:
          std::cout << "\nCoincidence trigger frag " << id - COIN_TRIG1_FRAG + 1 << ", " << size << " words found (hex)";
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) std::cout << setupMod << c/8<< ":";
            std::cout << std::hex << std::setw(11) << data[c] << std::dec;
          }
          std::cout << std::endl;
          break;

        default:
          std::cout << "\nUnknown fragment [0x" << std::hex << id << std::dec << "], " << size << " words found" << std::endl;
          break;
      }
    } else { // normal ROD fragments

      drawerMap_iterator itr = m_drawerMap.find(id);
      if (itr != m_drawerMap.end()) {
        fragType = (*itr).second;
      } else {
        fragType = (id >> 8);
        if (fragType > 4 || fragType < 1) fragType = 2; // assume barrel negative for unknown types
      }

      /* check for empty fragments */
      if (size > 0) {

        int DQstat;

        switch (type) {
          case 0: // digitizer fragment
            tile_unpack_digi(frag[f], channel, MAX_DIGI_CHAN, version, verbosity, &ngain, &nchan, &nsamp);
            std::cout << "\nDigitizer fragment 0x" << std::hex << id << std::dec << ", " << size << " words found:"
                      << "\t" << nchan / 3 << " chips, " <<  nsamp << "+2 samples" << std::endl;

            if (ngain == 1) {
              extra = size - nchan * (nsamp + 2) / 3;
            } else if (ngain == 2) {
              extra = size - nchan / 2 * (2 * nsamp + 3) / 3;
            } else {
              extra = 0;
            }

            if (version == 0x1 || version == 0x2) {
              std::cout << "\nfirst data word:" << std::setw(12) << data[0] << " (0x"<< setup0 << data[0] << setupDec << ")";
              --extra;
            }

            if (extra > 0) {
              std::cout << "\n" << std::setw(3) << extra << " extra words:";
              for (c = size - extra; c < size; ++c) {
                std::cout << std::setw(12) << data[c] << " (0x"<< setup0 << data[c] << setupDec << ")";
                if ((c - size + extra) % 2 == 1 && c!=size-1) std::cout << "\n                ";
              }
              std::cout << std::endl;
            }

            std::cout << "\nPMT Ch | BCID M G";
            for (s = 0; s < nsamp; ++s) {
              std::cout << std::setw(4) << s << " ";
            }

            std::cout << "  Head/Data/CRC\n---|---|-------------------------";
            for (s = 0; s < nsamp; ++s) {
              std::cout << "-----";
            }

            {
              bool OK = true;
              for (ch = 0; ch < nchan; ++ch) {
                pmt = m_cabling->channel2hole(fragType, ch % 48);
                if (extra == 0 && pmt < 0) pmt = -pmt;

                if (pmt > 0) {
                  std::cout << "\n" << std::setw(3) << pmt << std::setw(3) << ch << " |";
                } else {
                  std::cout << "\n -- " << std::setw(2) << ch << " |";
                }
                
                if (ch % 3 == 0) {
                  std::cout << std::setw(5) << (channel[ch].bcid) << std::setw(2) << ((channel[ch].flag >> 3) & 3);
                } else {
                  std::cout << "       "; 
                }
                
                std::cout << std::setw(2) << (channel[ch].gain);

                for (s = 0; s < nsamp; ++s) {
                  std::cout << std::setw(5) << (channel[ch].sample[s]);
                }
                
                if (ch % 3 == 0) { 
                  if (channel[ch].head != 0) {
                    std::cout << " 0x"  << setup0 << channel[ch].head << setupDec << " Head";
                  } else {
                    std::cout << "   "  << setup0 << channel[ch].head << setupDec << " Head";
                  }
                }
                
                if (ch % 3 == 1) { 
                  if (channel[ch].first != 0) {
                    std::cout << " 0x"  << setup0 << channel[ch].first << setupDec << " Data";
                  } else {
                    std::cout << "   "  << setup0 << channel[ch].first << setupDec << " Data";
                  }
                }
                
                if (ch % 3 == 2) { 
                  if (channel[ch].crc != 0) {
                    std::cout << " 0x"  << setup0 <<  channel[ch].crc << setupDec << " CRC ";
                  } else {
                    std::cout << "   "  << setup0 <<  channel[ch].crc << setupDec << " CRC ";
                  }
                }

                /*
                 if (ch < 48) {
                 int s[7];
                 for (int i = 0; i < 7; i++) {
                   s[i] = channel[ch].sample[i];
                 }
                 int gain = channel[ch].gain;
                 int ene_ctrl = m_rc2bytes5.amplitude(ofw, unit, ch, gain, s);
                 if (ene_ctrl < 0) ene_ctrl = 0;
                 if (ene_ctrl > 0x7FFF) ene_ctrl = 0x7FFF;
                 std::cout << " | " << std::setw(5) << ene_ctrl << std::setw(6) << recocalib[ch].amp);
                 if (recocalib[ch].amp != ene_ctrl) { OK = false; std::cout << ": ERROR"; }
                 }
                */
                if (isFrag5) {
                  bool chOK = true;
                  for (int i = 0; i < 7; i++) {
                    if (Frag5Data[ch].s[i] != channel[ch].sample[i]) chOK = false;
                  }
                  if (!chOK) {
                    std::cout << " RawERR ";
                    m_rc2bytes5.print_code(Frag5Data[ch].code);
                    std::cout << " | ";
                    for (int i = 0; i < 7; i++) {
                      std::cout << std::setw(5) << Frag5Data[ch].s[i] ;
                    }
                  }
                }
              }
              if (!OK) std::cout << "\nOF weights: ERROR";
            }

            if (isFrag5) {
              std::cout << std::endl;
              bool OK = true;
              for (int ch = 0; ch < 48; ch++) {
                bool chOK = true;
                for (int i = 0; i < 7; i++) {
                  if (Frag5Data[ch].s[i] != channel[ch].sample[i]) chOK = false;
                }
                //uint32_t* ofc = ofw + (ch*2 + channel[ch].gain)*22;
                //int16_t a4 = ofc[2] & 0xFFFF;
                //if (abs(a4) < 16000) chOK = true;
                //if (!chOK) {
                //  OK = false;
                //  std::cout <<" " << ch << std::hex << std::setfill('0');
                //  for (int i = 0; i < 22; i++) {
                //    std::cout << " 0x" << std::setw(8) << ofc[i] << ",";
                //  }
                //  std::cout << std::dec << std::setfill(' ') << std::endl;
                //}
                if (Frag5Data[ch].gain != (int) channel[ch].gain) chOK = false;
                if (!chOK) {
                  OK = false;
                  std::cout << " " << ch << ",";
                }
              }
              std::cout << "\nCompare Raw <=> Frag5: ";
              if (OK) {
                std::cout << "OK" << std::endl;
              } else {
                std::cout << "ERROR" << std::endl;
              }
              isFrag5 = false;
            }
            break;

          case 1:
            if (tile_unpack_raw_comp(frag[f], rawcomp, MAX_DIGI_CHAN, version, verbosity, &ngain, &nchan, &nsamp)) {
              std::cout << "\nRaw data compressed fragment 0x" << std::hex << id << std::dec << ", " << size << " words found" << std::endl;
              dump_data((uint32_t*) data, size, version, verbosity);
              break;
            }

            std::cout << "\nRaw data compressed fragment 0x" << std::hex << id << std::dec << ", " << size << " words found:"
                      << "\t" << ngain << " gain, " << nchan << " channels in total" << std::endl;

            std::cout << "\nPMT Ch |";
            for (int j = 1; j <= ((nsamp / 2) + 1); j++) {
              std::cout << " HexWord" << j << " ";
            }
            
            std::cout << "| G";
            for (int l = 1; l <= nsamp; l++) {
              std::cout << "  Smp" << l;
            }
            
            for (int i = 0; i < nchan && i < MAX_DIGI_CHAN; i++) {
              int ch = rawcomp[i].chan;
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) {
                std::cout << "\n" << std::setw(3) << pmt << std::setw(3) << ch << " |";
              } else {
                std::cout << "\n -- " << std::setw(2) << ch << " |";
              }
              std::cout << std::hex << std::setfill('0');
              for (int j = 0; j < ((nsamp / 2) + 1); j++) {
                std::cout << " " << std::setw(8) << rawcomp[i].words[j] << " ";
              }
              std::cout << setupDec << "| " << rawcomp[i].gain;
              for (int l = 0; l < nsamp; l++) {
                std::cout << std::setw(6) << rawcomp[i].samples[l];
              }
              if (!rawcomp[i].verif) std::cout << " Wrong Data";
            }
            break;

          case 2: // fragment with gain/amp/time/quality in 32 bit words
            tile_unpack_reco(frag[f], recochan, MAX_DIGI_CHAN, version, verbosity, &ngain, &nchan);

            std::cout << "\nReco non calibrated energy fragment 0x" << std::hex  << id << std::dec << ", " << size << " words found:"
                      << "\t" << ngain << " gain, " << nchan << " channels in total" << std::endl
                      << "\tATTENTION: HIGH gain amplitude is divided by 64" << std::endl;

            std::cout << "\nPMT Ch |  full word | G  amp  time  q    amp        time      qual";
            for (ch = 0; ch < nchan; ++ch) {
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) {
                std::cout << "\n" << std::setw(3) << pmt << std::setw(3) << ch << " |";
              } else {
                std::cout << "\n -- " << std::setw(2) << ch << " |";
              }
              std::cout << " 0x" << setup0 << recochan[ch].word << setupDec << " | " 
                        << std::setw(1) << recochan[ch].gain << std::setw(6) << recochan[ch].amp << std::setw(5) << recochan[ch].time 
                        << std::setw(3) << recochan[ch].quality << std::setw(10)  << std::setprecision(1) << float (recochan[ch].gain ? recochan[ch].d_amp / 64. : recochan[ch].d_amp) 
                        << std::setw(11) << std::setprecision(4) << (float) recochan[ch].d_time << std::setw(8) << std::setprecision(1) << (float) recochan[ch].d_quality;
            }

            break;

          case 4: // fragment with gain/amp/time/bad/quality in 32 bit words
            m_unit = unit;
            tile_unpack_reco_calib(frag[f], recocalib, MAX_DIGI_CHAN, version, unit, verbosity, &ngain,
                &nchan);

            std::cout << "\nReco calibrated energy fragment 0x" << std::hex  << id << std::dec << ", " << size << " words found:"
                      << "\t" << ((nchan > 48 && nchan < 96) ? 1 : ngain) << " gain, " << unitName[unit].c_str() << " units, " << nchan << " channels in total" << std::endl;

            if (pulse < 3) {
              std::cout << "Reco flags: 0x" << std::hex << std::setfill('0') << rflag << setupDec << "  units: " << unitName[unit].c_str() << "  pulse_shape: " << shapeName[pulse].c_str() << "  nsamples: " << 7 + 2 * nsmpl << "  algorithm: " << algName[algor + 1].c_str() << "  niterations: " << niter << std::endl;
            } else {
              std::cout << "Reco flags: 0x" << std::hex << std::setfill('0') << rflag << setupDec << "  units: " << unitName[unit].c_str() << "  pulse_shape: " << shapeName[pulse].c_str() << "  nsamples: " << 7 + 2 * nsmpl << "  algorithm: " << algName[algor * 4 + niter].c_str() << std::endl;
            }
            
            std::cout << "\nPMT Ch |  full word | G  amp  time b  q    amp        time      qual";
            for (ch = 0; ch < nchan; ++ch) {
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) {
                std::cout << "\n" << std::setw(3) << pmt << std::setw(3) << ch << " |";
              } else {
                std::cout << "\n -- " << std::setw(2) << ch << " |";
              }

              if (ch >= 48 && nchan < 96) { // sumE words
                std::cout << " 0x" << setup0 << recocalib[ch].word << setupDec << " |                   " << std::setw(11)  << std::setprecision(4) << Frag5_unpack_bin2sum(unit, (int )recocalib[ch].word);
              } else {
                std::cout << " 0x" << setup0 << recocalib[ch].word << setupDec << " | " << std::setw(1) << recocalib[ch].gain << std::setw(6) << recocalib[ch].amp << std::setw(5) << recocalib[ch].time << std::setw(2) << recocalib[ch].bad << std::setw(3) << recocalib[ch].quality << std::setw(10)  << std::setprecision(1) << recocalib[ch].d_amp << std::setw(11) << std::setprecision(4) << recocalib[ch].d_time << std::setw(8) << std::setprecision(1) << recocalib[ch].d_quality;

                if (recocalib[ch].bad != 0) {
                  std::cout << " Bad channel";
                }
              }
            }

            break;

          case 5: // compressed fragment
          {
            isFrag5 = true;
            m_frag5found = true;
            m_unit = unit;
            nchan = 48;
            OFC.clear();

            std::cout << "\nFrag5 Compressed fragment 0x" << std::hex << id << std::dec << ", " << size << " words found:"
                      << "\t" << 1 << " gain, " << unitName[unit].c_str() << " units, " << nchan << " channels in total" << std::endl;

            dump_data((uint32_t*) data, size, version, verbosity);

            { // fill OFC
              TileRawChannelUnit::UNIT chan_unit = (TileRawChannelUnit::UNIT) (TileRawChannelUnit::OnlineOffset + unit);
              unsigned int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(id);

              bool of2 = true;
              std::vector<double> a(7), b(7), c(7), g(7), dg(7);

              for (ch = 0; ch < nchan; ++ch) {
                for (int gain = 0; gain < 2; ++gain) {
                  float phase = -m_tileToolTiming->getSignalPhase(drawerIdx, ch, gain);
                  TileOfcWeightsStruct weights;
                  if (m_tileCondToolOfcCool->getOfcWeights(drawerIdx, ch, gain, phase, of2, weights).isFailure())
                  {
                    ATH_MSG_ERROR( "getOfcWeights failed.");
                    continue;
                  }

                  double calib = m_tileToolEmscale->channelCalibOnl(drawerIdx, ch, gain, 1.0, chan_unit);

                  if (unit != 0 && gain) calib = calib * 64.0;

                  for (int i = 0; i < 7; ++i) {
                    a[i] = weights.w_a[i];
                    b[i] = weights.w_b[i];
                    c[i] = weights.w_c[i];
                    g[i] = weights.g[i];
                    dg[i] = weights.dg[i];
                  }

                  Format6(a, b, c, g, dg, ch // channel
                          , 0 // phase = 0 poskol'ku ne ponyal kak okruglyat'
                          , calib // calibration
                          , OFC, false  );// verbose

                } // gain
              } // ch

            } // fill OFC
            ofw = &(OFC[0]);

            int size_L2 = (*((uint32_t*) data - 3 + 2) >> (32 - 2 - 3)) & 0x7;
            std::cout << "size_L2: " << size_L2 << " |";
            if (size_L2 == 3) {
              double SumEt = m_rc2bytes5.getSumEt((uint32_t*) data - 3);
              double SumEz = m_rc2bytes5.getSumEz((uint32_t*) data - 3);
              double SumE = m_rc2bytes5.getSumE((uint32_t*) data - 3);
              std::cout << " SumEt: " << SumEt << ", SumEz: " << SumEz << ", SumE: " << SumE << std::endl;
            }
            std::cout << std::endl;

            m_rc2bytes5.unpack(ofw, (uint32_t*) data - 3, Frag5Data);

#define code_ped4 TileRawChannel2Bytes5::code_ped4
#define code_ped5 TileRawChannel2Bytes5::code_ped5
#define code_amp5 TileRawChannel2Bytes5::code_amp5
#define code_amp6 TileRawChannel2Bytes5::code_amp6
#define code_raws TileRawChannel2Bytes5::code_raws
#define code_rawf TileRawChannel2Bytes5::code_rawf
#define code_full TileRawChannel2Bytes5::code_full
#define code_dump TileRawChannel2Bytes5::code_dump
#define code_null TileRawChannel2Bytes5::code_null

            int cnt_ped4, cnt_ped5, cnt_amp5, cnt_amp6, cnt_raws, cnt_rawf, cnt_full, cnt_dump, cnt_null;
            cnt_ped4 = cnt_ped5 = cnt_amp5 = cnt_amp6 = cnt_raws = cnt_rawf = cnt_full = cnt_dump = cnt_null = 0;
            std::cout << "PMT Ch |  full word | Type  G  B  ectrl  ereco  ebin       ene  time |   s1   s2   s3   s4   s5   s6   s7";

            const char *strcode_empty = "----";
            const char *strcode_ped4 = "ped4";
            const char *strcode_ped5 = "ped5";
            const char *strcode_amp5 = "amp5";
            const char *strcode_amp6 = "amp6";
            const char *strcode_raws = "raws";
            const char *strcode_rawf = "rawf";
            const char *strcode_full = "full";
            const char *strcode_dump = "dump";
            const char *strcode_null = "null";
            const char *strcode_error = "ERR ";

            bool OK = true;
            for (ch = 0; ch < nchan; ++ch) {
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) {
                std::cout << "\n" << std::setw(3) << pmt << std::setw(3) << ch << " |";
              } else {
                std::cout << "\n -- " << std::setw(2) << ch << " |";
              }
              
              uint32_t reco = data[ch];
              const char *scode = strcode_empty;
              int code = Frag5Data[ch].code;
              int gain = Frag5Data[ch].gain;
              int bad = Frag5Data[ch].bad;
              int ene_bin = Frag5Data[ch].ene_bin;
              float ene = Frag5Data[ch].ene;
              float time = Frag5Data[ch].time;
              int s[7];
              for (int i = 0; i < 7; i++) {
                s[i] = Frag5Data[ch].s[i];
              }
              
              switch (code) {
                case code_ped4: scode = strcode_ped4; cnt_ped4++; break;
                case code_ped5: scode = strcode_ped5; cnt_ped5++; break;
                case code_amp5: scode = strcode_amp5; cnt_amp5++; break;
                case code_amp6: scode = strcode_amp6; cnt_amp6++; break;
                case code_raws: scode = strcode_raws; cnt_raws++; break;
                case code_rawf: scode = strcode_rawf; cnt_rawf++; break;
                case code_full: scode = strcode_full; cnt_full++; break;
                case code_dump: scode = strcode_dump; cnt_dump++; break;
                case code_null: scode = strcode_null; cnt_null++; break;
                default: scode = strcode_error; break;
              }

              int ene_recobin = ene_bin + (gain == 0 ? 512 : 2048);
              switch (code) {
                case code_ped4:
                case code_ped5:
                  ene_bin += 256;
                  break;
                case code_amp5:
                case code_amp6:
                case code_raws:
                case code_rawf:
                  ene_bin += (gain == 0 ? 512 : 2048);
                  break;
              }

              int ene_ctrl = m_rc2bytes5.amplitude(ofw, unit, ch, gain, s);
              if (ene_ctrl < 0) {
                ene_ctrl = 0;
              } else if (ene_ctrl > 0x7FFF) {
                ene_ctrl = 0x7FFF;
              }
              
              std::cout << " 0x" << setup0 << reco << setupDec << " | " << std::setw(4) << scode << "  " << gain << " " << bad;
              
              switch (code) {
                case code_ped4:
                case code_ped5:
                case code_raws:
                case code_rawf:
                case code_dump:
                std::cout << std::setw(7) << ene_ctrl << std::setw(7) << ene_recobin << std::setw(6) << ene_bin << std::setw(10) << std::setprecision(4) << ene << "    -- ";
                break;
                case code_amp5:
                case code_amp6:
                std::cout << std::setw(7) << ene_ctrl << std::setw(7) << ene_recobin << std::setw(6) << ene_bin << std::setw(10) << std::setprecision(4) << ene << std::setw(6) << std::setprecision(1) << time << " ";
                break;
                case code_full:
                case code_null:
                std::cout << "  -----  -----  ----       ---    -- ";
                break;
                default:;
              }

              std::cout << "| " << std::setw(4) << s[0] << std::setw(5) << s[1] << std::setw(5) << s[2] << std::setw(5) << s[3] << std::setw(5) << s[4] << std::setw(5) << s[5] << std::setw(5) << s[6] << " " ;
              if (ene_ctrl != ene_recobin) {
                OK = false;
                std::cout << " ERR";
              }
            }

            std::cout << "\n\nFrag5 Self-Consistency: ";
            if (OK) {
              std::cout << "OK" << std::endl;
            } else {
              std::cout << "ERROR" << std::endl;
            }
            
            std::cout << "\nped4" <<  cnt_ped4 << ", ped5 " << cnt_ped5 << ", amp5 " << cnt_amp5 << ", amp6 " << cnt_amp6 << ", raws " << cnt_raws << ", rawf " << cnt_rawf << ", full " << cnt_full << ", dump " << cnt_dump << ", null " << cnt_null << std::endl;
            
            break;
          }

          case 0xA: // fragment with data quality words
            DQstat = tile_unpack_quality(frag[f], DQword);

            std::cout << "\nQuality fragment 0x" << std::hex  << id << ", "  << std::dec  << size << " words found:" << std::endl;

            if (DQstat) {
              std::cout << " ATTENTION: Error bits found in the Data Quality fragment 0xA" << std::endl;
            }

            std::cout << " --------------------------------------" << std::endl;
            std::cout << " | Quality Block      |  Word (16bit)" << std::endl;
            if (DQword.dspbcid >> 15) {
              std::cout << " | DSP BCID           |  " << std::dec << (DQword.dspbcid & 0x7FFF) << std::endl;
            } else {
              std::cout << " | DSP BCID           |  not filled (" << std::dec << DQword.dspbcid << ")" << std::endl;
            }
            std::cout << " | Global CRC         |  " << std::hex << "0x" << std::setw(1) << DQword.global_crc << std::setfill('0') << std::endl;
            std::cout << " | BCID checks        |  " << setup0x4 << DQword.bcid << std::endl;
            std::cout << " | Mem parity err     |  " << setup0x4 << DQword.memory << std::endl;
            std::cout << " | Single strobe err  |  " << setup0x4 << DQword.Sstrobe << std::endl;
            std::cout << " | Double strobe err  |  " << setup0x4 << DQword.Dstrobe << std::endl;
            std::cout << " | Head format err    |  " << setup0x4 << DQword.headformat << std::endl;
            std::cout << " | Head parity err    |  " << setup0x4 << DQword.headparity << std::endl;
            std::cout << " | Sample format err  |  " << setup0x4 << DQword.sampleformat << std::endl;
            std::cout << " | Sample parity err  |  " << setup0x4 << DQword.sampleparity << std::endl;
            std::cout << " | FE chip mask err   |  " << setup0x4 << DQword.fe_chip_mask << std::endl;
            std::cout << " | ROD chip mask err  |  " << setup0x4 << DQword.rod_chip_mask << std::endl;
            std::cout << " --------------------------------------" << std::setfill(' ') << std::dec << std::endl;
            break;

          default:
            std::cout << "\nUnknown (type " << type << ") fragment  0x" << std::hex  << id << ", "  << std::dec  << size << " words found" << std::endl;
            dump_data((uint32_t*) data, size, version, verbosity); // Salukvadze
        }
        std::cout << std::endl;
      } else { /* empty fragment */
        std::cout << "\nType " << type << " fragment 0x" << std::hex  << id << ", "  << std::dec  << size << " words found" << std::endl;
        std::cout << "\nEmpty Event" << std::endl;
      }
    }
  }
}

void dump_it(unsigned int nw, unsigned int * data) {

  unsigned int i;

  std::cout  << "size of data block: " << nw << std::endl;
  for (i = 0; i < nw; i++, data++) {
    std::cout << std::setw(4) << i << ":   0x" << std::hex << std::setw(8) << *data << "\t"  << std::dec << std::setw(10) << *data << std::endl;
  }
}

/* ------------------------------------------------------------------------ */

void TileTBDump::find_frag(const uint32_t* data, unsigned int size, unsigned int version
                           , int verbosity, T_RodDataFrag** frag, int* nfrag) {
  unsigned int offset = 0;
  *nfrag = 0;
  m_v3Format = (*(data) == 0xff1234ff); // additional frag marker since Sep 2005
  m_v3Format |= (*(data) == 0x00123400); // another possible frag marker (can appear in buggy ROD frags)
  if (m_v3Format || (version > 0xff)) {
    m_sizeOverhead = 3;
    ++offset; // skip frag marker
    std::cout << " *(p) = 0x" << std::hex << (*(data)) << std::dec << ((m_v3Format)?"":" => ERROR Corrupted frag separator") << std::endl;
    std::cout << " v3Format = true" << std::endl;
    if (!m_v3Format) {
      m_v3Format = true;
      std::cout << std::endl << "Dump of whole data fragment ("
                << size << " words)" << std::endl;
      dump_data(data, size, version, verbosity);
    }
  } else {
    m_sizeOverhead = 2;
  }

  while (offset < size && *nfrag < MAX_ROD_FRAG) {
    //std::cout << "nfrag="<<(*nfrag) << " offset="<<offset<<" data[offset]="<<data[offset]<<std::endl;
    frag[*nfrag] = (T_RodDataFrag *) (data + offset);

    if (frag[*nfrag]->size < m_sizeOverhead
        || frag[*nfrag]->size > size - offset + m_sizeOverhead - 2) {
    
      std::cout << "\nWarning: garbage in frag "  << *nfrag << " of current ROD -> ignore it"  << std::endl;
      std::cout << "Size:         \t"  << std::setw(10) << (frag[*nfrag]->size) << "\tMin/Max Size: \t" << std::setw(10) << m_sizeOverhead << "\t" <<  std::setw(10) << size - offset + m_sizeOverhead - 2 << std::endl;
      std::cout << "Id:           \t"  << std::setw(10) << (frag[*nfrag]->id) << std::endl;
      std::cout << "Bad data:"  << std::endl;
      std::cout << "Before:\t"  << offset-1 << "\t" << data[offset-1] << "\t0x" << std::hex << data[offset-1] << std::dec << std::endl;
      
      for (; offset < size; ++offset) {
        std::cout << "\t"  << offset << "\t" << data[offset] << "\t0x" << std::hex << data[offset] << std::dec << std::endl;
        if (data[offset] == 0xff1234ff) break;
      }
      if (offset == size) {
        std::cout << "After:\t"  << offset << "\t" << data[offset] << "\t0x" << std::hex << data[offset] << std::dec << std::endl;
      }
      if (m_v3Format) {
        ++offset; // go to next good frag or jump outside ROD, if at the end
      }
      
    } else if (frag[*nfrag]->size < size - offset && m_v3Format && data[offset + frag[*nfrag]->size - 1] != 0xff1234ff) {

      std::cout << "\nWarning: frag "  << *nfrag << " of current ROD is damaged"  << std::endl;
      std::cout << "Size:         \t"  << std::setw(10) << (frag[*nfrag]->size) << "\tMin/Max Size: \t" << std::setw(10) << m_sizeOverhead << "\t" <<  std::setw(10) << size - offset + m_sizeOverhead - 2 << std::endl;
      std::cout << "Id:           \t"  << std::setw(10) << (frag[*nfrag]->id) << std::endl;
      std::cout << "Bad data:"  << std::endl; 
      unsigned int newsize = 0;
      std::cout << "Before:\t"  << offset-1 << "\t" << data[offset-1] << "\t0x" << std::hex << data[offset-1] << std::dec << std::endl;
      for (; offset < size; ++offset, ++newsize) {
        std::cout << "\t"  << offset << "\t" << data[offset] << "\t0x" << std::hex << data[offset] << std::dec << std::endl;
        if (data[offset] == 0xff1234ff) break;
      }
      if (offset == size) {
        std::cout << "After:\t"  << offset << "\t" << data[offset] << "\t0x" << std::hex << data[offset] << std::dec << std::endl;
      }
      if (m_v3Format) {
        ++newsize;
        ++offset; // go to next good frag or jump outside ROD, if at the end
      }
      std::cout << "Correct size is:\t" << std::setw(10) << newsize << std::endl;

    } else {
      offset += frag[*nfrag]->size;
      // if (version == 0x1 && offset < size) offset += 7; // skip extra header - was needed for 2001-2003 TB data only
      ++(*nfrag);
    }
  }

  if (m_v3Format) {
    --offset; // set offset back to correct value
  }
  
  if (offset > size) {
    --(*nfrag);
    std::cout << "\nWarning: last fragment in current ROD is garbage -> ignore it" << std::endl;
    std::cout << "N good frag:  \t" << std::setw(10) << *nfrag << std::endl;
    std::cout << "Last frag:" << std::endl;
    for (unsigned int i = offset - frag[*nfrag]->size; i < size; ++i) {
      std::cout << "\t" << i << "\t" << data[i] << "\t0x" << std::hex  << data[i] << std::dec << std::endl;
    }
  }
}



/*--------------------------------------------------------------------------*/
int TileTBDump::tile_unpack_quality(T_RodDataFrag *frag, T_TileRecoQuality & DQword) {
  /*--------------------------------------------------------------------------*/
// Errors are defined by a bit value of 1, while 0 means OK
  unsigned int status = 0;

  //int size = frag->size - m_sizeOverhead; /* size of the data part in the fragment */
  unsigned int *data = frag->data; /* first word of data */

  unsigned int w;
  w = (*data);
  //    std::cout << " word is 0x" << std::hex << std::setw(8)<< std::setfill('0') << w << std::endl;
  DQword.dspbcid = w >> 16; /* if upper bit is set, remaining bits are BCID set by DSP */
  DQword.global_crc = w & 0x1;
  status = status + DQword.global_crc;

  ++data;
  w = (*data);

  //    std::cout << " word is 0x" << std::hex << std::setw(8)<< std::setfill('0') << w << std::endl;

  DQword.bcid = w & 0xFFFF; // least sign. bit means DMU00 versus TTC. The other 15 bits mean DMUXX versus DMU00
  DQword.memory = (w >> 16) & 0xFFFF;

  /*    std::cout << " bcid is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.bcid << std::endl;
   std::cout << " memory is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.memory << std::endl;
   */
  status = status + DQword.memory;

  ++data;
  w = (*data);
  //    std::cout << " word is 0x" << std::hex << std::setw(8)<< std::setfill('0') << w << std::endl;
  DQword.Sstrobe = w & 0xFFFF;
  DQword.Dstrobe = (w >> 16) & 0xFFFF;
  status = status + DQword.Dstrobe;
  /*
   std::cout << " Sstrobe is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.Sstrobe << std::endl;
   std::cout << " Dstrobe is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.Dstrobe << std::endl;
   */

  ++data;
  w = (*data);
  //    std::cout << " word is 0x" << std::hex << std::setw(8)<< std::setfill('0') << w << std::endl;
  DQword.headformat = w & 0xFFFF; //bit 31
  DQword.headparity = (w >> 16) & 0xFFFF; // bit30, parity must be odd

  /*
   std::cout << " Head format is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.headformat << std::endl;
   std::cout << " Head parity is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.headparity << std::endl;
   */

  ++data;
  w = (*data);
  //    std::cout << " word is 0x" << std::hex << std::setw(8)<< std::setfill('0') << w << std::endl;
  DQword.sampleformat = w & 0xFFFF;
  DQword.sampleparity = (w >> 16) & 0xFFFF;

  /*
   std::cout << " Sample format is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.sampleformat << std::endl;
   std::cout << " Sample parity is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.sampleparity << std::endl;
   */

  ++data;
  w = (*data);
  DQword.fe_chip_mask = (w & 0xFFFF); //do not invert to get the error mask
  DQword.rod_chip_mask = (w >> 16) & 0xFFFF; //do not invert to get the error mask

  /*
   std::cout << " fe mask is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.fe_chip_mask << std::endl;
   std::cout << " rod mask is 0x" << std::hex << std::setw(8)<< std::setfill('0') << DQword.rod_chip_mask << std::endl;
   */

  return status;
}


/*--------------------------------------------------------------------------*/
int TileTBDump::tile_unpack_reco(T_RodDataFrag *frag, T_TileRecoChannel * channel
                                 , int nchannel_max, unsigned int /* version */
                                 , int /* verbosity */, int *ngain, int *nchannel) {
/*--------------------------------------------------------------------------*/

  int status = 0;

  int size = frag->size - m_sizeOverhead; /* size of the data part in the fragment */
  unsigned int *data = frag->data; /* first word of data */

  int ch = 0;
  for (; ch < size && ch < nchannel_max; ++ch) {
    unsigned int w = data[ch];
    channel[ch].chan = ch % 48;
    channel[ch].word = w;
    channel[ch].gain = (w >> GAIN_SHIFT2) & GAIN_RANGE2;
    channel[ch].amp = (w >> AMPLITUDE_SHIFT2) & AMPLITUDE_RANGE2;
    channel[ch].time = (w >> TIME_SHIFT2) & TIME_RANGE2;
    channel[ch].quality = (w >> QUALITY_SHIFT2) & QUALITY_RANGE2;
    channel[ch].d_amp = m_rc2bytes2.amplitude(w);
    channel[ch].d_time = m_rc2bytes2.time(w);
    channel[ch].d_quality = m_rc2bytes2.quality(w);
  }

  *ngain = (size - 1) / 48 + 1;
  *nchannel = ch;

  return status;
}



/*--------------------------------------------------------------------------*/
int TileTBDump::tile_unpack_reco_calib(T_RodDataFrag* frag, T_TileRecoCalib* recocalib
                                       , int nchannel_max, unsigned int /* version */
                                       , unsigned int unit
                                       , int /* verbosity */, int *ngain, int *nchannel) {// Baxo
    /*--------------------------------------------------------------------------*/

  int status = 0;

  int size = frag->size - m_sizeOverhead; // size of the data part in the fragment
  unsigned int *data = frag->data;       // first word of data

  int ch = 0;
  for (; ch < size && ch < nchannel_max; ++ch) {
    unsigned int w = data[ch];
    recocalib[ch].chan = ch % 48;
    recocalib[ch].word = w;
    recocalib[ch].gain = (w >> GAIN_SHIFT4) & GAIN_RANGE4;
    recocalib[ch].amp = (w >> AMPLITUDE_SHIFT4) & AMPLITUDE_RANGE4;
    recocalib[ch].time = (w >> TIME_SHIFT4) & TIME_RANGE4;
    recocalib[ch].bad = ((w >> QUALITY_SHIFT4) & QUALITY_RANGE4 & 0x10) >> 4;
    recocalib[ch].quality = ((w >> QUALITY_SHIFT4) & QUALITY_RANGE4 & 0xF);
    recocalib[ch].d_amp = m_rc2bytes4.amplitude(w, unit);
    recocalib[ch].d_time = m_rc2bytes4.time(w);
    recocalib[ch].d_quality = m_rc2bytes4.quality(w);
  }

  *ngain = (size - 1) / 48 + 1;
  *nchannel = ch;

  return status;
}


/*--------------------------------------------------------------------------*/
int TileTBDump::tile_unpack_raw_comp(T_RodDataFrag* frag, T_TileRawComp* rawcomp
                                     , int nchannel_max, unsigned int /* version */, int /* verbosity */
                                     , int* ngain, int* nchannel, int* nsample) {
/*--------------------------------------------------------------------------*/

  int status = 0;

  int size = frag->size - m_sizeOverhead; // size of the data part in the fragment
  int id = frag->id;
  int frag1version = (id >> 31) & 0x1;
  int nbchanformat1 = (id >> 24) & 0x3F;

  if (frag1version == 0) { //Old version

    if ((size % 4) != 0) {
      std::cout << " Format Type 1: Raw compressed : Wrong Size = " << size << std::endl;
      status = 1;
      return status;
    }

    int nsamp = 7;
    int nchan = size / 4;

    *ngain = 1;
    *nchannel = nchan;
    *nsample = nsamp;

    unsigned int *data = frag->data;
    int i = 0;

    if (data != 0) {
      for (int ch = 0; ch < nchan && ch < nchannel_max; ch++) {
        unsigned int w = data[i++];
        rawcomp[ch].words[0] = w;
        nsamp = ((w >> 8) & 0x0F);
        rawcomp[ch].chan = (w & 0xFF);
        rawcomp[ch].gain = (w >> 15) & 0x1;
        rawcomp[ch].samples[0] = (w >> 16);
        for (int j = 1; j < ((nsamp / 2) + 1); j++) {
          rawcomp[ch].words[j] = w = data[i++];
          for (int l = 1; l < nsamp; l += 2) {
            rawcomp[ch].samples[l] = (w & 0xFFFF);
            rawcomp[ch].samples[l + 1] = (w >> 16);
          }
        }
        rawcomp[ch].verif = (((*nsample) == nsamp) && (rawcomp[ch].chan < 48));
        if (!rawcomp[ch].verif) {
          status = 1;
          std::cout << " Verification ERROR for channel # " << ch << " (ch=" << rawcomp[ch].chan << " g=" << rawcomp[ch].gain << " ns=" << nsamp  << " 0x" << std::hex  << (rawcomp[ch].words[0] & 0xFFFF) << std::dec << ")!" << std::endl;
        } else {
          unsigned short v = 0;
          for (int k = 0; k < nsamp; k++) {
            v |= rawcomp[ch].samples[k];
          }
          rawcomp[ch].verif = !(v & 0xFC00);
        }
      }
    }

  } else if (frag1version == 1) { //New version

    int nsamp = 7; // New frag1 only for 7 samples
    int SizeOfFrag1 = size * 2; // Number of 16 bit words
    int nbchanformat2 = (SizeOfFrag1 - (3 * nbchanformat1)) / 5;

    int nchan = nbchanformat1 + nbchanformat2;

    *ngain = 1;
    *nchannel = nchan;
    *nsample = nsamp;
    unsigned int *p = frag->data;

    if ((nchan) > 48 || ((nbchanformat1 * 3) + (nbchanformat2 * 5) > SizeOfFrag1)) {
      std::cout << " Format Type 1: Raw compressed : ERROR" << " fragId=0x" << std::hex
          << (id & 0xFFFF) << std::dec << " frag1Version=" << frag1version << " Nsamp=" << nsamp
          << " NchanFormat1=" << nbchanformat1 << " NchanFormat2=" << nbchanformat2
          << " Wrong Size=" << size << std::endl;
      status = 1;

    } else {

      int ptr16index = 1;
      int channel = 0;
      uint16_t word1 = 0;
      uint16_t word2 = 0;
      uint16_t word3 = 0;
      uint16_t word4 = 0;
      uint16_t word5 = 0;
      int ch = 0;

      for (int chf1 = 0; chf1 < nbchanformat1; ++chf1) {

        if (ptr16index) {

          channel = ((*p >> 26) & 0x3F);
          word1 = (uint16_t) ((*p >> 16) & 0xFFFF);
          word2 = (uint16_t) (*p & 0xFFFF);
          word3 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
          ptr16index = 0;

          rawcomp[ch].words[0] = (*p);
          rawcomp[ch].words[1] = (*p + 1) & 0xFFFF0000;
          rawcomp[ch].words[2] = 0;
          rawcomp[ch].words[3] = 0;

        } else {

          channel = ((*p >> 10) & 0x3F);
          word1 = (uint16_t) (*p & 0xFFFF);
          word2 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
          word3 = (uint16_t) (*(p + 1) & 0xFFFF);
          ptr16index = 1;

          rawcomp[ch].words[0] = (*p) & 0xFFFF;
          rawcomp[ch].words[1] = (*p + 1);
          rawcomp[ch].words[2] = 0;
          rawcomp[ch].words[3] = 0;

        }

        int gain = 1;
        uint16_t Smin = (word1 & 0x3FF);

        rawcomp[ch].samples[0] = ((word3 >> 4) & 0xF) + Smin;
        rawcomp[ch].samples[1] = ((word3 >> 0) & 0xF) + Smin;
        rawcomp[ch].samples[2] = ((word3 >> 8) & 0xF) + Smin;
        rawcomp[ch].samples[3] = ((word3 >> 12) & 0xF) + Smin;
        rawcomp[ch].samples[4] = ((word2 >> 4) & 0xF) + Smin;
        rawcomp[ch].samples[5] = ((word2 >> 0) & 0xF) + Smin;
        rawcomp[ch].samples[6] = ((word2 >> 8) & 0xF) + Smin;

        rawcomp[ch].chan = channel;
        rawcomp[ch].gain = gain;
        rawcomp[ch].verif = true;

        p += 1 + ptr16index;
        ++ch;
      }

      for (int chf2 = 0; chf2 < nbchanformat2; ++chf2) {

        if (ptr16index) {

          channel = ((*p) & 0x3F);
          word1 = (uint16_t) ((*p >> 16) & 0xFFFF);
          word2 = (uint16_t) ((*p) & 0xFFFF);
          word3 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
          word4 = (uint16_t) (*(p + 1) & 0xFFFF);
          word5 = (uint16_t) ((*(p + 2) >> 16) & 0xFFFF);
          ptr16index = 0;

          rawcomp[ch].words[0] = (*p);
          rawcomp[ch].words[1] = (*p + 1);
          rawcomp[ch].words[2] = (*p + 2) & 0xFFFF0000;
          rawcomp[ch].words[3] = 0;

        } else {

          channel = ((*(p + 1) >> 16) & 0x3F);
          word1 = (uint16_t) ((*p) & 0xFFFF);
          word2 = (uint16_t) ((*(p + 1) >> 16) & 0xFFFF);
          word3 = (uint16_t) (*(p + 1) & 0xFFFF);
          word4 = (uint16_t) ((*(p + 2) >> 16) & 0xFFFF);
          word5 = (uint16_t) (*(p + 2) & 0xFFFF);
          ptr16index = 1;

          rawcomp[ch].words[0] = (*p) & 0xFFFF;
          rawcomp[ch].words[1] = (*p + 1);
          rawcomp[ch].words[2] = (*p + 2);
          rawcomp[ch].words[3] = 0;

        }

        int gain = ((word2 >> 6) & 0x1);

        rawcomp[ch].samples[0] = ((word1 << 9) & 0x200) + ((word2 >> 7) & 0x1FF);
        rawcomp[ch].samples[1] = (word1 >> 1) & 0x3FF;
        rawcomp[ch].samples[2] = (word4 << 5 & 0x3E0) + ((word1 >> 11) & 0x1F);
        rawcomp[ch].samples[3] = (word4 >> 5) & 0x3FF;
        rawcomp[ch].samples[4] = ((word3 << 1) & 0x3FE) + ((word4 >> 15) & 0x1);
        rawcomp[ch].samples[5] = ((word5 << 7) & 0x380) + ((word3 >> 9) & 0x7F);
        rawcomp[ch].samples[6] = (word5 >> 3) & 0x3FF;

        rawcomp[ch].chan = channel;
        rawcomp[ch].gain = gain;
        rawcomp[ch].verif = true;

        p += (2 + ptr16index);
        ++ch;
      }
    }
//} else {
//  status = 1; // Logically dead code
  }
  return status;
}



/*--------------------------------------------------------------------------*/
int TileTBDump::tile_unpack_digi(T_RodDataFrag* frag, T_TileDigiChannel* channel
                                 , int nchannel_max, unsigned int version, int verbosity
                                 , int* ngain, int* nchannel, int* nsample) {
/*--------------------------------------------------------------------------*/

  static int first = 1;
  static int digi_mode = 0;
  int m, c, s/*,id*/, size, ch, dm, dgm = 0, digim[5], digm[4] = { 0, 0, 0, 0 }, status = 0;
  int nchip, nchip2, nchan, nchan2, nsamp, nsamp1, nsamp2, gain_offs;
  unsigned int *data, val, headword, firstword, crcword;
  unsigned int hlflags, word1, word2, word3, word4, word5;
  unsigned short samp[3][MAX_CHAN_SAMP], smin, smax;

  //id   = frag->id & 0xFFFF;       /* fragment ID in the range 0x000 - 0x3FF */
  size = frag->size - m_sizeOverhead; /* size of the data part in the fragment */
  data = frag->data; /* first word of data */

  int dataoffset = 0;
  if (version == 0x2 || version == 0x1) { /* can not guess number of samples from size */
    if (size > 176 && size < 205) {
      size = 179; /* we expect this number (9+2)*16+1+2 */
    } else if (size > 272 && size < 405) {
        size = 275; /* we expect this number (7*2+3)*16+1+2 */
    }
    dataoffset = 1; // ignore first word
  }
    
    nchip  = 16;         /* number of chips in the data, only 16 is expected */
    nchan  = nchip*3;    /* one chip contains 3 channles */
    nsamp2 = size/nchip; /* calculate number of data words per chip */

      
        
    /* find digitizers mode (calibration or normal) */
    /* do not do this if fragment has bad length */

  if (first || status == 0) {
    data = frag->data + dataoffset;
    dm = 0;
    for (m = 0; m < nchip; m++) {
      if (tile_check_parity(data, 1) == 0) { /* check parity to be sure */
        dgm = ((*data) >> 15) & 3; /* that mode is correct    */
        if (verbosity > 3) {
          std::cout << "Good parity, chip" << std::setw(3) << m << ", mode" << std::setw(2) << (int) dgm << ", head 0x" << setup0 << data[0] << ", data 0x" << std::setw(8) << data[1] << setupDec << std::endl;
        }

        if (((data[0] >> 31) == 1) && ((data[1] >> 31) == 0)) {
          digim[dm++] = dgm;
          if (dm == 5) break;
        }
      }
      data += nsamp2;
    }

    if (m == nchip && dm == 0) { /* the same check, but ignore parity now */
      data = frag->data + dataoffset;
      for (m = 0; m < nchip; m++) {
        dgm = ((*data) >> 15) & 3;
        if (verbosity > 3) {
          std::cout << "Chip" << std::setw(3) << m << ", mode" << std::setw(2) << (int) dgm << ", head 0x" << setup0 << data[0] << ", data 0x" << std::setw(8) << data[1] << setupDec << std::endl;
        }
        if (((data[0] >> 31) == 1) && ((data[1] >> 31) == 0)) {
          digim[dm++] = dgm;
          if (dm == 5) break;
        }
        data += nsamp2;
      }
    }

    if (m == nchip && dm == 0) {
      if (first) {
        first = 0;
        if (nsamp2 == 17) {
          digi_mode = 1;
          std::cout << "Warning: No valid header found, calibration running mode(=1) assumed" << std::endl;
        } else {
          digi_mode = 0;
          std::cout << "Warning: No valid header found, normal running mode(=0) assumed" << std::endl;
        }
      } else {
        if (verbosity > 3) {
          std::cout << "Warning: No valid header found, keeping  previous running mode(=" << (int) digi_mode << ")" << std::endl;
        }
      }
      status |= 2;
    } else {
      digi_mode = dgm; /* last found digi mode */
      if (dm > 2) { /* more than 2 good headers found */
        for (c = 0; c < dm; ++c) {
          ++digm[digim[c]]; /* count different digi_modes */
        }
        for (c = 0; c < 4; ++c) {
          if (digm[c] > digm[digi_mode]) {/* find most frequent digi_mode */
            digi_mode = c;
          }
        }
      }
      if (first) {
        first = 0;
        if (digi_mode > 0) m <<= 1;
        if (tile_check_parity(data, 1) == 0) {
          std::cout << "\nMode=" << digi_mode << " found in header of chip " << m << std::endl;
        } else {
          std::cout << "\nMode=" << digi_mode << " found in header of chip " << m << " with bad parity" << std::endl;
        }
        if (digi_mode > 0) {
          std::cout << "\nCalibration mode selected, effective number of chips is twice bigger" << std::endl;
        }
      }
    }
  }

  /* put offset in the gain_offs variable */
  if (digi_mode > 0) {
    nchip *= 2; /* number of chips is twice bigger in calib mode*/
    nchan = nchip * 3;
    nchan2 = nchan / 2; /* real number of channels is one half of total */
    nchip2 = nchip / 2; /* real number of chips is one half of total */

    if (nsamp2 % 2 == 0) {
      /* old mode, all low gain channels and then all high gain channels
       CRC word is duplicated */

      nsamp2 = size / nchip;
      nsamp1 = nsamp2 - 1; /* length of data    with header and without CRC word */
      nsamp = nsamp1 - 1; /* length of data without header and without CRC word */
      gain_offs = size / 2; /* first header of high gain in the second half of the data */

    } else {
      /* new mode, low gain + high gain + CRC word for one channel, then
       next channel etc */

      nsamp1 = (nsamp2 - 1) / 2; /* length of data    with header and without CRC word */
      nsamp = nsamp1 - 1; /* length of data without header and without CRC word */
      gain_offs = nsamp1; /* first header of high gain just after first low gain */
    }
  } else {
    nsamp1 = nsamp2 - 1; /* length of data    with header and without CRC word */
    nsamp = nsamp1 - 1; /* length of data without header and without CRC word */

    nchan2 = nchan;
    gain_offs = 0; /* only one low or high gain value from chip */
    nchip2 = nchip; /* all the chips are independent */
  }

  data = frag->data + dataoffset;
  for (m = 0; m < nchip; m++) {
    if (m == nchip2) { /* another gain */
      data = frag->data + dataoffset;
      data += gain_offs;
      gain_offs *= -1;
    }

    /* extract all samples for 3 channels in the chip */
    for (s = nsamp; s > 0;) { /* shifted by 1 due to header word */
      val = data[s--]; /* decrement s to be real sample number */
      for (c = 0; c < 3; c++) {
        samp[c][s] = val & 0x3FF;
        val = val >> 10;
      }
    }

    headword = data[0];
    firstword = data[1];
    if (gain_offs > 0) {
      crcword = data[nsamp1 + gain_offs]; /* first gain */
    } else {
      crcword = data[nsamp1]; /* second gain */
    }
    hlflags = ((headword) >> 12) & 7;

    /* check parity of all datawords in the frame, header and CRC */
    word5 = tile_check_parity(data + 1, nsamp); /* data parity (one bit per sample) */
    word4 = (word5 != 0) ? 32 : 0; /* just one bit (bad/good) for all data words */
    word4 |= (tile_check_parity(data, 1) << 4); /* header word parity */
    word4 |= (tile_check_startbit(data + 1, nsamp, 0) << 7); /* data words startbits */
    word4 |= (tile_check_startbit(data, 1, 1) << 6); /* header word startbit */
    word4 |= tile_check_CRC(data, nsamp1, gain_offs); /* bad/good CRC flag */

    word3 = (headword) & 0xFFF; /* bunch crossing ID   */
    word2 = (headword >> 12) & 0x3FFF; /* parity, mode, gain  */
    word1 = (headword >> 26) & 0xF; /* derandomizer length */
    word1 = (word1 << 8) | (m << 2); /* insert chip number  */

    for (c = 0; c < 3; c++) {
      ch = m * 3 + c;
      if (ch < nchannel_max) {
        tile_min_max(samp[c], nsamp, &smin, &smax); /* find min and max sample  */
        channel[ch].chan = ch % nchan2; /* the same for low gain and high gain */
        channel[ch].head = headword;
        channel[ch].first = firstword;
        channel[ch].crc = crcword;
        channel[ch].id = word1 | c; /* insert channel number */
        channel[ch].gain = (hlflags >> c) & 1;
        channel[ch].flag = word2;
        channel[ch].bcid = word3;
        channel[ch].err = word4 | ((smax == 1023) ? 256 : 0) /* overflow flag */
                                | ((smin == 0) ? 512 : 0); /* underflow flag */
        channel[ch].par = word5;
        memcpy(channel[ch].sample, samp[c], nsamp * sizeof(short));
      }
    }

    data += nsamp2;
  }

  *ngain = (gain_offs != 0) ? 2 : 1;
  *nchannel = nchan;
  *nsample = nsamp;

  return status;
}

unsigned int TileTBDump::tile_check_parity(unsigned int *frame, int length) {
/*--------------------------------------------------------------------------*/
/* Name: tile_check_parity                                                  */
/*                                                                          */
/* Author: Magnus Ramstedt                       Date: 10/09/99             */
/*                                                                          */
/* Description:                                                             */
/* Checks the even parity on all data words for one channel                 */
/* data bits= |xp<---ch3--><---ch2--><---ch1-->|                            */
/* where x is any bit,                                                      */
/* where p is the parity bit,                                               */
/* where ch1,ch2,ch3 - three channels, 10 bits each                         */
/* returns word with "length" status bits, one status bit per every word    */
/* (0 - if OK else 1)                                                       */
/*                                                                          */
/* Input arguments: array of data words and its length                      */
/*                                                                          */
/* Output arguments:                                                        */
/*                                                                          */
/* Libraries used:                                                          */
/*                                                                          */
/* Routines called:                                                         */
/*                                                                          */
/* Return Value: word with "length" status bits (0 if OK else 1)            */
/*                                                                          */
/*--------------------------------------------------------------------------*/

  int i, j;
  unsigned int parity, data_word, answer = 0;

  for (i = 0; i < length; ++i) {

    data_word = *frame++;

    parity = 0;
    for (j = 0; j < 32; j++) {
      parity ^= data_word;
      data_word >>= 1;
    }

    if ((parity &= 1) == 0) answer |= 1 << i;
  }

  return answer;
}

/*--------------------------------------------------------------------------*/
unsigned int TileTBDump::tile_check_startbit(unsigned int* frame, int length, unsigned int startbit) {
/*--------------------------------------------------------------------------*/
/* Name: tile_check_startbit                                                */
/*                                                                          */
/* Author: Alexandre Solodkov                    Date: 06/08/01             */
/*                                                                          */
/* Description:                                                             */
/* Checks that first bit in all "length" words of "*frame"                  */
/* is equal to "startbit"                                                   */
/* where p is the parity bit,                                               */
/* returns (0 - if OK else 1)                                               */
/*                                                                          */
/* Input arguments: array of data words, its length and startbit to compare */
/*                                                                          */
/* Output arguments:                                                        */
/*                                                                          */
/* Libraries used:                                                          */
/*                                                                          */
/* Routines called:                                                         */
/*                                                                          */
/* Return Value: 0 if OK else 1                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/

  int i;
  unsigned int data_word, answer = 0;

  for (i = 0; i < length; ++i) {

    data_word = *frame++;

    if ((data_word >> 31) != startbit) {
      answer = 1;
      break;
    }
  }

  return answer;
}

/*--------------------------------------------------------------------------*/
unsigned int TileTBDump::tile_check_CRC(unsigned int *frame, int framelen, int delta) {
/*--------------------------------------------------------------------------*/
/* Name: tile_check_CRC                                                     */
/*                                                                          */
/* Author: Magnus Ramstedt                       Date: 04/10/99             */
/*                                                                          */
/* Description:                                                             */
/* Calculate CRC and compare with input value                               */
/* return 0 if OK else 1,2,3 or 4 depending on the type of error            */
/*                                                                          */
/* Input arguments: array of data words its length and delta for the        */
/*                  address af the second half of the data                  */
/*                                                                          */
/* Output arguments:                                                        */
/*                                                                          */
/* Libraries used:                                                          */
/*                                                                          */
/* Routines called:                                                         */
/*                                                                          */
/* Return Value: 0 if OK else 1,2,3 or 4 depending on the type of error     */
/*                                                                          */
/*--------------------------------------------------------------------------*/

#define CRC_ok           0
#define CRC_error_0      1
#define CRC_error_1      2
#define CRC_do_not_match 4
#ifndef CRC_POLY
#define CRC_POLY  0x8005
#endif

  unsigned int CRC_error = CRC_ok;
  static unsigned int error[3] = { CRC_error_0, CRC_error_1, CRC_do_not_match };

  int i, j, k, length;
  unsigned int *data, word, CRC_word;
  unsigned short bit_in, bit_out, reg, reg1, reg2;

  /* put all the data in one array with empty word at the end */

  if (delta != 0) { /* low gain and high gain in different places */
    length = 2 * framelen + 1;
    data = (unsigned int *) malloc(length * sizeof(int));
    if (delta > 0) {
      memcpy(data, frame, framelen * sizeof(int));
      memcpy(data + framelen, frame + delta, framelen * sizeof(int));
      CRC_word = frame[framelen + delta]; /* after second part of the data */
    } else {
      memcpy(data, frame + delta, framelen * sizeof(int));
      memcpy(data + framelen, frame, framelen * sizeof(int));
      CRC_word = frame[framelen]; /* just after the data */
    }
  } else {
    length = framelen + 1;
    data = (unsigned int *) malloc(length * sizeof(int));
    memcpy(data, frame, framelen * sizeof(int));
    CRC_word = frame[framelen]; /* just after the data */
  }

  data[length - 1] = 0;

  /* Calculates the CRC16 from *data */

  for (i = 0; i < 2; i++) { /* odd and even bits separately */

    reg = 0;
    for (j = 0; j < length; j++) { /* all datawords */

      word = data[j];
      for (k = i; k < 32; k += 2) { /* 16 bits (odd or even) from one word */

        bit_in = (word >> k) & 1; /* This is the incomming bit */
        bit_out = reg & 0x8000; /* Saving the kicked out bit */
        reg = ((reg << 1) | bit_in); /* Filling 'reg' with 'bit_in' */

        if (bit_out) reg ^= CRC_POLY; /* xor reg with poly if bit_out!=0 */
      }
    }

    /* there were a bus swaped */

    reg1 = 0;
    for (k = 16; reg != 0; reg >>= 1) {
      reg1 |= (reg & 1) << (--k);
    }
    
    /* reading the recived CRC (16 bit long) */

    reg2 = 0;
    for (k = i; k < 32; k += 2) { /* 16 bits (odd or even) from one word */

      bit_in = (CRC_word >> k) & 1; /* This is the incomming bit */
      reg2 = ((reg2 << 1) | bit_in); /* Filling 'reg2' with 'bit_in' */
    }

    if (reg1 != reg2) CRC_error |= error[i];
  }

  free(data);
  return CRC_error;
}

/*--------------------------------------------------------------------------*/
void TileTBDump::tile_min_max ( unsigned short *frame, int frame_length, unsigned short *smin, unsigned short *smax ) {
/*--------------------------------------------------------------------------*/
/* Name: tile_min_max                                                       */
/*                                                                          */
/* Author: Alexandre Solodkov                    Date: 12/07/01             */
/*                                                                          */
/* Description: Tries to determine if there is some signal in the frame     */
/*                                                                          */
/* Input arguments: array of samples                                        */
/*                                                                          */
/* Output arguments: max and min sample                                     */
/*                                                                          */
/* Libraries used:                                                          */
/*                                                                          */
/* Routines called:                                                         */
/*                                                                          */
/* Return Value:                                                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/

  int t;
  unsigned short val, Min = 0xFFFF, Max = 0;

  for (t = 0; t < frame_length; ++t) {
    val = frame[t];

    if (val < Min) {
      Min = val;
    }
    if (val > Max) {
      Max = val;
    }
  }

  *smin = Min;
  *smax = Max;
}

