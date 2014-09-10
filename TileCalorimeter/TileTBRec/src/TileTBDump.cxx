/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <cstdio>
#include <string>
#include <ctime>

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

  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfcCool", m_tileCondToolOfcCool, "TileCondToolOfcCool");
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  
  m_v3Format = true;
  m_frag5found = false;
  m_sizeOverhead = 3;
  m_unit = -1;
}


TileTBDump::~TileTBDump() {
}

// Alg standard interface function
StatusCode TileTBDump::initialize() {
  
  memset(StatFrag5, 0, sizeof(StatFrag5));
   
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
    printf("\nStatFrag5[40..129]");
    for (int i = 40; i < 130; i++) {
      if (i % 10 == 0) printf("\n  [%3d] : ", i);
      printf(" %8d", StatFrag5[i]);
    }
    printf("\n"); 
  }
  
  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileTBDump::execute() {


  static bool notFirst = false;
  if (m_dumpOnce && notFirst) return StatusCode::SUCCESS;
  notFirst = true;
  
  ATH_MSG_DEBUG( "execute()" );

  int verbosity = 0;
  if ( msgLvl(MSG::NIL) ) verbosity = 7;
  else if ( msgLvl(MSG::VERBOSE) ) verbosity = 2;
  else if ( msgLvl(MSG::DEBUG) ) verbosity = 1;

  // take full event
  const eformat::FullEventFragment<const uint32_t*> * event = m_RobSvc->getEvent();
  
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

    std::cout << "  ROB frag ID " << std::hex << "0x" << robf.source_id() << std::dec;
    std::cout << " size " << robf.fragment_size_word() << std::endl;

    // Here we should unpack the fragment.
    std::cout << "    ROD frag ID " << std::hex << "0x" << robf.rod_source_id() << std::dec;
    std::cout << " size " << robf.rod_fragment_size_word() << std::endl;

    //
    // get info on ROD
    //
    unsigned int version = robf.rod_version();
    unsigned int source_id = robf.rod_source_id();
    eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(source_id);
    unsigned int subdet_id = id.subdetector_id();
    unsigned int module_id = id.module_id();

    bool known = m_dumpUnknown || subdet_id == 0x70  // COMMON BEAM ROD in CTB2004
                 || (subdet_id >= 0x50 && subdet_id < 0x60); // TileCal IDs

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
    if (max_allowed_size > delta) {
      max_allowed_size -= delta;
    } else {
      std::cout << " Problem with ROD data: total length " << max_allowed_size
                << " is less than " << delta << " - size of header+trailer" << std::endl;
      max_allowed_size = 0;
    }
    
    if (m_dumpStatus) {

      unsigned int size = robf.rod_nstatus();
      if (robf.rod_status_position() > 1
          || robf.rod_ndata() > max_allowed_size
          || size > max_allowed_size - robf.rod_ndata()) {

        std::cout << " Problem with status words " << std::endl;
      } else if (size > 0) {
        const uint32_t * stat;
        robf.rod_status(stat);
        for (unsigned int ind = 0; ind < size; ++ind)
          std::cout << " Status[" << ind << "] = " << stat[ind] << "\t\t" << stat[ind] / 1000000. - 1. << std::endl;
      } else {
        std::cout << " No status words " << std::endl;
      }
    }
          
    if ( m_dumpData ) {

      unsigned int size = robf.rod_ndata();
      if (size > max_allowed_size) {
        size = max_allowed_size;
        std::cout<<" Problem with data size - assuming " << size << " words "<<std::endl;
      }

      if ( size > 0 ) {

        const uint32_t * data;
        robf.rod_data(data);

        if ((subdet_id >= 0x50 && subdet_id < 0x60) || // TileCal IDs
            subdet_id == 0x63 || // wrong id in first testbeam test runs 
            subdet_id == 0x70) { // COMMON BEAM ROD in CTB2004
          dump_digi(subdet_id,data, size, version, verbosity);
        } else if ( m_dumpUnknown ) {
          dump_data(data, size, version, verbosity);
        }
        
        std::cout<<std::endl;
      }
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

void TileTBDump::dump_data(const uint32_t * data, unsigned int size, unsigned int /* version */, int /* verbosity */) {

  boost::io::ios_base_all_saver coutsave(std::cout);
  std::cout << std::endl << " Fragment data as 4 byte words:" << std::hex << std::setw(8) << std::setprecision(8);

  for (unsigned int cnter = 0; cnter < size; ++cnter) {
    if (!(cnter % 8)) std::cout << std::endl;
    std::cout << (*data++) << " ";
  }

  std::cout << std::dec << std::endl << std::endl;

}

void TileTBDump::dump_digi(unsigned int subdet_id, const uint32_t* roddata, unsigned int rodsize
                           , unsigned int version, int verbosity) {

  int s, c, f, nfrag, ngain, nchan, nsamp, size, ch, extra = 0, pmt, fragType, nhits = 0;
  int id, type, rflag, unit, pulse, nsmpl, algor, niter;
  unsigned int* data;
  unsigned short time, flag, prev, edge, chan, bad, res1/*,last,res2*/;
  char fr[2] = { 'F', 'R' };
  char gb[2] = { 'G', 'B' };
  std::string unitName[4] = { "ADC count", "pCb", "Cs pCb", "MeV" };
  std::string shapeName[4] = { "Phys", "Laser", "CIS", "Simul" };
  std::string algName[8] = { "Unknown", "OF1", "OF2", "Fit", "ManyAmps", "Flat", "Alg6", "Alg7" };

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
    version &= 0xFF; // keep just minor version number
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

    if (id < 0x100 || beamROD) { // BEAM fragments
      id &= 0xFF; // set proper frag ID in Beam frag for old data

      switch (id) {

        case COMMON_TOF_FRAG:
          printf("\nBeam ToF TDC, %d hits found", size);
          prev = 0xFF;
          for (c = 0; c < size; ++c) {
            time = data[c] & 0x1FFF;
            res1 = (data[c] >> 13) & 0x1;
            chan = (data[c] >> 16) & 0x07FF;
            if (prev != chan) {
              printf("\n ch%3d:", chan);
              nhits = 0;
              prev = chan;
            } else if (nhits % 8 == 0) printf("\n       ");
            ++nhits;
            if (res1)
              printf(" U%4d", time);
            else
              printf("  %4d", time);
          }
          printf("\n");
          break;

        case BEAM_TDC_FRAG:
        case COMMON_TDC1_FRAG:
        case COMMON_TDC2_FRAG:
          printf("\nBeam TDC 0x%02x, %d hits found", id, size);
          prev = 0xFF;
          for (c = 0; c < size; ++c) {
            time = data[c] & 0xFFFF;
            flag = data[c] >> 16;
            edge = flag & 0x01;
            chan = (flag >> 1) & 0x0F;
            bad = (flag >> 5) & 0x01;
            res1 = (flag >> 6) & 0x01;
            //last = (flag >> 7) & 0x01;
            //res2 = (flag >> 8) & 0x0F;
            if (prev != chan) {
              printf("\n ch%3d:", chan);
              nhits = 0;
              prev = chan;
            } else if (nhits % 8 == 0) printf("\n       ");
            ++nhits;
            if (bad)
              printf(" %c%c%6d", gb[bad], fr[edge], time);
            else
              printf("  %c%6d", fr[edge], time);
          }
          printf("\n");
          break;

        case BEAM_ADC_FRAG:
        case COMMON_ADC1_FRAG:
        case COMMON_ADC2_FRAG:
          if (BEAM_ADC_FRAG == id)
            printf("\nTile Beam ADC, %d channels found", size);
          else
            printf("\nBeam ADC 0x%02x, %d channels found", id, size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9u", data[c]);
          }
          printf("\n");
          break;

        case MUON_ADC_FRAG:
          printf("\nMuon ADC, %d channels found", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9u", data[c]);
          }
          printf("\n");
          break;

        case ADDR_ADC_FRAG:
          printf("\nMuon2 ADC, %d channels found", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9u", data[c]);
          }
          printf("\n");
          break;

        case LASE_PTN_FRAG:
        case COMMON_PTN_FRAG:
          if (LASE_PTN_FRAG == id)
            printf("\nLaser Pattern Unit, %d words found (hex)", size);
          else
            printf("\nCommon Pattern Unit, %d words found (hex)", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf(" 0x%08x", data[c]);
          }
          printf("\n");
          break;

        case LASER_OBJ_FRAG:
          printf("\nLASTROD Laser Object, %d words found (hex)", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf(" 0x%08x", data[c]);
          }
          printf("\n");

          if (size != 31) {
            printf("CRITICAL ERROR! Unknown format!\n");
          } else {
            unsigned int * p;
            int Counter = 0;
            int Filter = 0, ReqAmp = 0, MeasAmp = 0, Delay = 0, TDC1 = 0, TDC2 = 0;
            p = data;
            Counter = *p;
            p++;
            printf("\n Laser Counter: %5d\n", Counter);

            if ((*p & 0xFF000000) == 0x20000000) {
              ReqAmp = *p & 0xFFFF;
              if (version > 1) {
                Filter = (((*p >> 16) & 7) ^ 7) + 2;
                if (Filter > 8) Filter -= 8;
                printf("  Filter Wheel: %5d\n", Filter);
              }
              printf("  Required Amp: %5d\n", ReqAmp);
            } else {
              printf("ERROR in Laser Fragment: decoding word 14.\n");
            }
            p++;

            if ((*p & 0xFF000000) == 0x21000000) {
              Delay = (*p >> 12) & 0xFFF;
              MeasAmp = *p & 0xFFF;
              printf("  Measured Amp: %5d\n", MeasAmp);
              printf("         Delay: %5d\n", Delay);
            } else {
              printf("ERROR in Laser Fragment: decoding word 15.\n");
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
              printf("ERROR in Laser Fragment: decoding word 16.\n");
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
              printf("ERROR in Laser Fragment: decoding word 17.\n");
              TDCPrint = false;
            }
            p++;
            if (TDCPrint) {
              printf("     TDC1 data: %5d\n", TDC1);
              printf("     TDC2 data: %5d\n", TDC2);
            }

            int chan0 = 0, chan1 = 0, chan2 = 0, chan3 = 0, chan4 = 0, chan5 = 0, chan6 = 0, chan7 = 0;

            if ((*p & 0xFF000000) == 0x44000000) {
              chan0 = (*p & 0xFFF) ^ 0xFFF;
              chan1 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
            } else {
              printf("ERROR in Laser Fragment: decoding word 18.\n");
            }
            p++;

            if ((*p & 0xFF000000) == 0x45000000) {
              chan2 = (*p & 0xFFF) ^ 0xFFF;
              chan3 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
            } else {
              printf("ERROR in Laser Fragment: decoding word 19.\n");
            }
            p++;

            if ((*p & 0xFF000000) == 0x46000000) {
              chan4 = (*p & 0xFFF) ^ 0xFFF;
              chan5 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
            } else {
              printf("ERROR in Laser Fragment: decoding word 20.\n");
            }
            p++;

            if ((*p & 0xFF000000) == 0x47000000) {
              chan6 = (*p & 0xFFF) ^ 0xFFF;
              chan7 = ((*p >> 12) & 0xFFF) ^ 0xFFF;
            } else {
              printf("ERROR in Laser Fragment: decoding word 21.\n");
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

            time_t PedAlpha_Last_Run;

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

            const char *sline = (
                version == 1 ? "\n   %s | %5d | %5.1f (%5.2f) | %5.1f (%5.2f) |"
                             : "\n   %s | %5d | %5.1f (%5.2f) | %5.1f (%5.2f) | %5.1f (%5.2f) |");

            printf("\n           |  ADC  | Pedestal(RMS) |  Alpha (RMS)  | PedAlpha(RMS) |");
            printf(sline, "Diode 1", chan0, diode1_Ped / 10.0, diode1_PedRMS / 100.0
                   , diode1_alpha / 10.0, diode1_alphaRMS / 100.0, diode1_PedAlpha / 10.0
                   , diode1_PedAlphaRMS / 100.0);

            printf(sline, "Diode 2", chan1, diode2_Ped / 10.0, diode2_PedRMS / 100.0
                   , diode2_alpha / 10.0, diode2_alphaRMS / 100.0, diode2_PedAlpha / 10.0
                   , diode2_PedAlphaRMS / 100.0);

            printf(sline, "Diode 3", chan2, diode3_Ped / 10.0, diode3_PedRMS / 100.0
                   , diode3_alpha / 10.0, diode3_alphaRMS / 100.0, diode3_PedAlpha / 10.0
                   , diode3_PedAlphaRMS / 100.0);

            printf(sline, "Diode 4", chan3, diode4_Ped / 10.0, diode4_PedRMS / 100.0
                   , diode4_alpha / 10.0, diode4_alphaRMS / 100.0, diode4_PedAlpha / 10.0
                   , diode4_PedAlphaRMS / 100.0);

            printf("\n   PMT 1   | %5d | %5.1f (%5.2f) |       x       |       x       |", chan4, PMT1_Ped / 10.0, PMT1_PedRMS / 100.0);

            printf("\n   PMT 2   | %5d | %5.1f (%5.2f) |       x       |       x       |", chan5, PMT2_Ped / 10.0, PMT2_PedRMS / 100.0);

            printf("\n   InjChrg | %5d |       x       |       x       |       x       |", chan6);
            printf("\n   Spare   | %5d |       x       |       x       |       x       |\n", chan7);

            printf("\n          |  Date & Time (GMT)  |");

            struct tm* TimeInfo;
            char buf[80];
            TimeInfo = gmtime(&Ped_Last_Run);
            strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);

            printf("\n Pedestal | %s |", buf);

            TimeInfo = gmtime(&Alpha_Last_Run);
            strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);

            printf("\n    Alpha | %s |", buf);

            TimeInfo = gmtime(&PedAlpha_Last_Run);
            strftime(buf, 80, "%d.%m.%Y %H:%M:%S", TimeInfo);

            printf("\n PedAlpha | %s |\n", buf);

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

            printf("\n                    |   Time  | Value |");
            printf("\n   Laser diode temp | %7d |  %4.1f |", secsDiodeT, diodeTemp / 10.0);
            printf("\n     Laser box temp | %7d |  %4.1f |", secsBoxT, boxTemp / 10.0);
            printf("\n Laser box humidity | %7d |  %4.1f |", secsBoxH, boxHum / 10.0);
            printf("\n Laser box gas flow | %7d |  %4.1f |\n", secsGasF, gasFlow / 10.0);

            int PLCstatus = *p & 0xFFF;
            int PLCtime = (*p >> 12) & 0xFFFFF;
            p++;

            int Err, Alarm, Ilock, ShClose, ShOpen, HV2, HV1, LV, Alpha2, Alpha1, Alpha0;

            Alpha0 = PLCstatus & 0x001;
            Alpha1 = PLCstatus & 0x002;
            Alpha2 = PLCstatus & 0x004;
            LV = PLCstatus & 0x008;
            HV1 = PLCstatus & 0x010;
            HV2 = PLCstatus & 0x020;
            ShOpen = PLCstatus & 0x040;
            ShClose = PLCstatus & 0x080;
            Ilock = PLCstatus & 0x100;
            Alarm = PLCstatus & 0x200;
            Err = PLCstatus & 0x800;

            printf("\n    Time  | Err | Alarm | Ilock | ShClose | ShOpen | HV2 | HV1 |  LV | Alpha2 | Alpha1 | Alpha0 |");

            printf("\n  %7d | %3s |  %3s  |  %3s  |   %3s   |  %3s   | %3s | %3s | %3s |  %3s   |  %3s   |  %3s   |\n"
                   , PLCtime
                   , (Err ? "Yes" : "No")
                   , (Alarm ? "On" : "Off")
                   , (Ilock ? "On" : "Off")
                   , (ShClose ? "Yes" : "No")
                   , (ShOpen ? "Yes" : "No")
                   , (HV2 ? "On" : "Off")
                   , (HV1 ? "On" : "Off")
                   , (LV ? "On" : "Off")
                   , (Alpha2 ? "On" : "Off")
                   , (Alpha1 ? "On" : "Off")
                   , (Alpha0 ? "On" : "Off"));

            if (p != &data[size]) {
              printf("CRITICAL ERROR! Wrong size!\n");
            }
          }
          break;

        case LASE_ADC_FRAG:
          printf("\nLaser ADC, %d channels found", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9u", data[c]);
          }
          printf("\n");
          break;

        case ECAL_ADC_FRAG:
          printf("\nECAL ADC, %d channels found", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9u", data[c]);
          }
          printf("\n");
          break;

        case DIGI_PAR_FRAG:
          printf("\nDigi parameters, %d words found", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%11u", data[c]);
          }
          printf("\n");

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

            printf("\n  CIS Counter: %3d\n", Counter);
            printf("\n     Run Type: %3d (%s)", RunType, RunTypeText);
            if (!DefFormat) {
              printf("\n      Samples: %3d", Samples);
              printf("\n     Pipeline: %3d\n", Pipeline);
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
              printf("\n   Time (GMT): %s", buf);
              printf("\n    Microsec.: %d\n", microsec);
              printf("\n         Mode: %3d (%s)", Mode, ModeText);
              printf("\n      Samples: %3d", Samples);
              printf("\n     Pipeline: %3d", Pipeline);
              printf("\n      I3Delay: %3d", I3Delay);
              printf("\n        Event: %3d", Event);
              printf("\n        Phase: %3d", Phase);
              printf("\n          DAC: %3d", DAC);
              printf("\n     Capacity: %3d pF", Capacity);
              printf("\n         Card: %3d\n", Card);

              if (size > 16) {
                int last = size - 1;
                for (; last > 15; --last) {
                  if (data[last] != 0) break;
                }
                if (last > 15) {
                  printf("\n Remaing %d non-zero words (hex):", last - 15);
                  for (c = 16; c <= last; ++c) {
                    if (c % 8 == 0) printf("\n mod%2d:", c / 8);
                    printf("%9x", data[c]);
                  }
                  printf("\n");
                }
              }
            }
          } else {
            printf("CRITICAL ERROR! Unknown format!\n");
          }
          break;

        case ADD_FADC_FRAG:
          printf("\nAdder FADC, %d words found (hex)", size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%9x", data[c]);
          }
          printf("\n");
          break;

        case COIN_TRIG1_FRAG:
        case COIN_TRIG2_FRAG:
        case COIN_TRIG3_FRAG:
        case COIN_TRIG4_FRAG:
        case COIN_TRIG5_FRAG:
        case COIN_TRIG6_FRAG:
        case COIN_TRIG7_FRAG:
        case COIN_TRIG8_FRAG:
          printf("\nCoincidence trigger frag %d, %d words found (hex)", id - COIN_TRIG1_FRAG + 1, size);
          for (c = 0; c < size; ++c) {
            if (c % 8 == 0) printf("\n mod%2d:", c / 8);
            printf("%11x", data[c]);
          }
          printf("\n");
          break;

        default:
          printf("\nUnknown fragment [%#x], %d words found\n", id, size);
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

            printf("\nDigitizer fragment %#x, %d words found:", id, size);
            printf("\t%d chips, %d+2 samples\n", nchan / 3, nsamp);

            if (ngain == 1) {
              extra = size - nchan * (nsamp + 2) / 3;
            } else if (ngain == 2) {
              extra = size - nchan / 2 * (2 * nsamp + 3) / 3;
            } else {
              extra = 0;
            }

            if (version == 0x1 || version == 0x2) {
              printf("\nfirst data word:%12u (0x%08x)", data[0], data[0]);
              --extra;
            }

            if (extra > 0) {
              printf("\n%3d extra words:", extra);
              for (c = size - extra; c < size; ++c) {
                printf("%12u (0x%08x)", data[c], data[c]);
                if ((c - size + extra) % 2 == 1) printf("\n                ");
              }
            }

            printf("\nPMT Ch | BCID M G");
            for (s = 0; s < nsamp; ++s) {
              printf("%4d ", s);
            }

            printf("  Head/Data/CRC \n---|---|-------------------------");
            for (s = 0; s < nsamp; ++s) {
              printf("-----");
            }

            {
              bool OK = true;
              for (ch = 0; ch < nchan; ++ch) {
                pmt = m_cabling->channel2hole(fragType, ch % 48);
                if (extra == 0 && pmt < 0) pmt = -pmt;

                if (pmt > 0) printf("\n%3d %2d |", pmt, ch);
                else printf("\n -- %2d |", ch);

                if (ch % 3 == 0)
                  printf(" %4u%2u", channel[ch].bcid, (channel[ch].flag >> 3) & 3);
                else printf("       ");

                printf("%2u", channel[ch].gain);

                for (s = 0; s < nsamp; ++s)
                  printf(" %4d", channel[ch].sample[s]);

                if (ch % 3 == 0) printf(" %#10.8x Head", channel[ch].head);
                if (ch % 3 == 1) printf(" %#10.8x Data", channel[ch].first);
                if (ch % 3 == 2) printf(" %#10.8x CRC ", channel[ch].crc);

                /*
                 if (ch < 48) {
                 int s[7];
                 for (int i = 0; i < 7; i++) s[i] = channel[ch].sample[i];
                 int gain = channel[ch].gain;
                 int ene_ctrl = m_rc2bytes5.amplitude(ofw, unit, ch, gain, s);
                 if (ene_ctrl < 0) ene_ctrl = 0;
                 if (ene_ctrl > 0x7FFF) ene_ctrl = 0x7FFF;
                 printf(" | %5d %5d", ene_ctrl, recocalib[ch].amp);
                 if (recocalib[ch].amp != ene_ctrl) { OK = false; printf(": ERROR"); }
                 }
                 */
                if (isFrag5) {
                  bool chOK = true;
                  for (int i = 0; i < 7; i++) {
                    if (Frag5Data[ch].s[i] != channel[ch].sample[i]) chOK = false;
                  }
                  if (!chOK) {
                    printf(" RawERR ");
                    m_rc2bytes5.print_code(Frag5Data[ch].code);
                    printf(" | ");
                    for (int i = 0; i < 7; i++) {
                      printf(" %4d", Frag5Data[ch].s[i]);
                    }
                  }
                }
              }
              if (!OK) printf("\nOF weights: ERROR");
            }

            if (isFrag5) {
              printf("\n");
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
                //  printf(" %d", ch);
                //  for (int i = 0; i < 22; i++) printf(" 0x%08X,", ofc[i]);
                //  printf("\n");
                //}
                if (Frag5Data[ch].gain != (int) channel[ch].gain) chOK = false;
                if (!chOK) {
                  OK = false;
                  printf(" %d,", ch);
                }
              }
              printf("\nCompare Raw <=> Frag5: ");
              if (OK)
                printf("OK\n");
              else
                printf("ERROR\n");
              isFrag5 = false;
            }
            break;

          case 1:
            if (tile_unpack_raw_comp(frag[f], rawcomp, MAX_DIGI_CHAN, version, verbosity, &ngain, &nchan, &nsamp)) {
              printf("\nRaw data compressed fragment %#x, %d words found\n", id, size);
              dump_data((uint32_t*) data, size, version, verbosity);
              break;
            }

            printf("\nRaw data compressed fragment %#x, %d words found:", id, size);
            printf("\t%d gain, %d channels in total\n", ngain, nchan);

            printf("\nPMT Ch |");
            for (int j = 1; j <= ((nsamp / 2) + 1); j++)
              printf(" HexWord%d ", j);

            printf("| G ");
            for (int l = 1; l <= nsamp; l++)
              printf(" Smp%d ", l);

            for (int i = 0; i < nchan && i < MAX_DIGI_CHAN; i++) {
              int ch = rawcomp[i].chan;
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) printf("\n%3d %2d |", pmt, ch);
              else printf("\n -- %2d |", ch);
              for (int j = 0; j < ((nsamp / 2) + 1); j++)
                printf(" %08x ", rawcomp[i].words[j]);

              printf("| %1d ", rawcomp[i].gain);
              for (int l = 0; l < nsamp; l++)
                printf(" %4d ", rawcomp[i].samples[l]);
              if (!rawcomp[i].verif) printf(" Wrong Data");
            }
            break;

          case 2: // fragment with gain/amp/time/quality in 32 bit words
            tile_unpack_reco(frag[f], recochan, MAX_DIGI_CHAN, version, verbosity, &ngain, &nchan);

            printf("\nReco non calibrated energy fragment %#x, %d words found:", id, size);
            printf("\t%d gain, %d channels in total\n", ngain, nchan);
            printf("\tATTENTION: HIGH gain amplitude is divided by 64\n");

            printf("\nPMT Ch |  full word | G  amp  time  q     amp        time      quality");
            for (ch = 0; ch < nchan; ++ch) {
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) printf("\n%3d %2d |", pmt, ch);
              else printf("\n -- %2d |", ch);

              printf(" %#10.8x | %1u %5u %4u %2u %10.4f %10.4f %10.4f"
                     , recochan[ch].word, recochan[ch].gain, recochan[ch].amp, recochan[ch].time, recochan[ch].quality
                     , (recochan[ch].gain ? recochan[ch].d_amp / 64. : recochan[ch].d_amp) //LF: If HG divide the amp by 64. req. by Bob.
                     , recochan[ch].d_time, recochan[ch].d_quality);
            }

            break;

          case 4: // fragment with gain/amp/time/bad/quality in 32 bit words
            m_unit = unit;
            m_rc2bytes4.setUnit(unit);
            tile_unpack_reco_calib(frag[f], recocalib, MAX_DIGI_CHAN, version, verbosity, &ngain,
                &nchan);

            printf("\nReco calibrated energy fragment %#x, %d words found:", id, size);
            printf("\t%d gain, %s units, %d channels in total\n", ((nchan > 48 && nchan < 96) ? 1 : ngain), unitName[unit].c_str(), nchan);

            if (pulse < 3)
              printf("Reco flags: 0x%x  units: %s  pulse_shape: %s  nsamples: %d  algorithm: %s  niterations: %d \n"
                     , rflag, unitName[unit].c_str(), shapeName[pulse].c_str(), 7 + 2 * nsmpl, algName[algor + 1].c_str(), niter);
            else
              printf("Reco flags: 0x%x  units: %s  pulse_shape: %s  nsamples: %d  algorithm: %s \n"
                     , rflag, unitName[unit].c_str(), shapeName[pulse].c_str(), 7 + 2 * nsmpl, algName[algor * 4 + niter].c_str());

            printf("\nPMT Ch |  full word | G  amp  time b  q     amp        time      quality");
            for (ch = 0; ch < nchan; ++ch) {
              pmt = m_cabling->channel2hole(fragType, ch % 48);
              if (pmt > 0) printf("\n%3d %2d |", pmt, ch);
              else printf("\n -- %2d |", ch);

              if (ch >= 48 && nchan < 96) { // sumE words
                printf(" %#10.8x |                   %10.4f", recocalib[ch].word, Frag5_unpack_bin2sum(unit, (int )recocalib[ch].word));
              } else {
                printf(" %#10.8x | %1u %5u %4u %1u %2u %10.4f %10.4f %10.4f"
                       , recocalib[ch].word, recocalib[ch].gain, recocalib[ch].amp
                       , recocalib[ch].time, recocalib[ch].bad, recocalib[ch].quality
                       , recocalib[ch].d_amp, recocalib[ch].d_time, recocalib[ch].d_quality);

                if (recocalib[ch].bad != 0) {
                  printf(" Bad channel");
                }
              }
            }

            break;

          case 5: // compressed fragment
          {
            isFrag5 = true;
            m_frag5found = true;
            m_unit = unit;
            m_rc2bytes5.setUnit(unit);
            nchan = 48;
            OFC.clear();

            printf("\nFrag5 Compressed fragment %#x, %d words found:", id, size);
            printf("\t%d gain, %s units, %d channels in total\n", 1, unitName[unit].c_str(), nchan);

            dump_data((uint32_t*) data, size, version, verbosity);

            { // fill OFC
              TileRawChannelUnit::UNIT chan_unit = (TileRawChannelUnit::UNIT) (TileRawChannelUnit::OnlineOffset + unit);
              unsigned int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(id);

              bool of2 = true;
              const TileOfcWeightsStruct* weights;
              std::vector<double> a(7), b(7), c(7), g(7), dg(7);

              for (ch = 0; ch < nchan; ++ch) {
                for (int gain = 0; gain < 2; ++gain) {
                  float phase = -m_tileToolTiming->getSignalPhase(drawerIdx, ch, gain);
                  weights = m_tileCondToolOfcCool->getOfcWeights(drawerIdx, ch, gain, phase, of2);

                  double calib = m_tileToolEmscale->channelCalibOnl(drawerIdx, ch, gain, 1.0, chan_unit);

                  if (unit != 0 && gain) calib = calib * 64.0;

                  for (int i = 0; i < 7; ++i) {
                    a[i] = weights->w_a[i];
                    b[i] = weights->w_b[i];
                    c[i] = weights->w_c[i];
                    g[i] = weights->g[i];
                    dg[i] = weights->dg[i];
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
            printf("size_L2: %d |", size_L2);
            if (size_L2 == 3) {
              double SumEt = m_rc2bytes5.getSumEt((uint32_t*) data - 3);
              double SumEz = m_rc2bytes5.getSumEz((uint32_t*) data - 3);
              double SumE = m_rc2bytes5.getSumE((uint32_t*) data - 3);
              printf(" SumEt: %f, SumEz: %f, SumE: %f\n", SumEt, SumEz, SumE);
            }
            printf("\n");

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

            printf("PMT Ch |  full word | Type  G  B  ectrl  ereco  ebin       ene  time |   s1   s2   s3   s4   s5   s6   s7");

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
              if (pmt > 0) printf("\n%3d %2d |", pmt, ch);
              else printf("\n -- %2d |", ch);

              uint32_t reco = data[ch];
              const char *scode = strcode_empty;
              int code = Frag5Data[ch].code;
              int gain = Frag5Data[ch].gain;
              int bad = Frag5Data[ch].bad;
              int ene_bin = Frag5Data[ch].ene_bin;
              float ene = Frag5Data[ch].ene;
              float time = Frag5Data[ch].time;
              int s[7];
              for (int i = 0; i < 7; i++)
                s[i] = Frag5Data[ch].s[i];

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
              if (ene_ctrl < 0)
                ene_ctrl = 0;
              else if (ene_ctrl > 0x7FFF) ene_ctrl = 0x7FFF;

              printf(" 0x%08x | %4s  %1d  %1d", reco, scode, gain, bad);
              switch (code) {
                case code_ped4:
                case code_ped5:
                case code_raws:
                case code_rawf:
                case code_dump:
                printf(" %6d %6d %5d %9.4f    -- ", ene_ctrl, ene_recobin, ene_bin, ene);
                break;
                case code_amp5:
                case code_amp6:
                printf(" %6d %6d %5d %9.4f %5.1f ", ene_ctrl, ene_recobin, ene_bin, ene, time);
                break;
                case code_full:
                case code_null:
                printf("  -----  -----  ----       ---    -- ");
                break;
                default:;
              }

              printf("| %4d %4d %4d %4d %4d %4d %4d ", s[0], s[1], s[2], s[3], s[4], s[5], s[6]);
              if (ene_ctrl != ene_recobin) {
                OK = false;
                printf(" ERR");
              }
            }

            printf("\n\nFrag5 Self-Consistency: ");
            if (OK) printf("OK\n");
            else printf("ERROR\n");

            printf( "\nped4 %d, ped5 %d, amp5 %d, amp6 %d, raws %d, rawf %d, full %d, dump %d, null %d\n"
                   , cnt_ped4, cnt_ped5, cnt_amp5, cnt_amp6, cnt_raws, cnt_rawf, cnt_full, cnt_dump, cnt_null);

            break;
          }

          case 0xA: // fragment with data quality words
            DQstat = tile_unpack_quality(frag[f], DQword);

            printf("\nQuality fragment %#x, %d words found:\n", id, size);

            if (DQstat) {
              std::cout << " ATTENTION: Error bits found in the Data Quality fragment 0xA " << std::endl;
            }

            std::cout << " --------------------------------------" << std::endl;
            std::cout << " | Quality Block      |  Word (16bit) " << std::endl;
            if (DQword.dspbcid >> 15)
              std::cout << " | DSP BCID           |  " << std::dec << (DQword.dspbcid & 0x7FFF) << std::endl;
            else
              std::cout << " | DSP BCID           |  not filled (" << std::dec << DQword.dspbcid << ")" << std::endl;

            std::cout << " | Global CRC         |  " << std::hex << "0x" << std::setw(1) << DQword.global_crc << std::dec << std::endl;
            std::cout << " | BCID checks        |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.bcid << std::dec << std::endl;
            std::cout << " | Mem parity err     |  " << std::hex << "0x" << std::setw(4)  << std::setfill('0') << DQword.memory << std::dec << std::endl;
            std::cout << " | Single strobe err  |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.Sstrobe << std::dec << std::endl;
            std::cout << " | Double strobe err  |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.Dstrobe << std::dec << std::endl;
            std::cout << " | Head format err    |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.headformat << std::dec << std::endl;
            std::cout << " | Head parity err    |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.headparity << std::dec << std::endl;
            std::cout << " | Sample format err  |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.sampleformat << std::dec << std::endl;
            std::cout << " | Sample parity err  |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.sampleparity << std::dec << std::endl;
            std::cout << " | FE chip mask err   |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.fe_chip_mask << std::dec << std::endl;
            std::cout << " | ROD chip mask err  |  " << std::hex << "0x" << std::setw(4) << std::setfill('0') << DQword.rod_chip_mask << std::dec << std::endl;
            std::cout << " --------------------------------------" << std::endl;
            break;

          default:
            printf("\nUnknown (type %d) fragment %#x, %d words found\n", type, id, size);
            dump_data((uint32_t*) data, size, version, verbosity); // Salukvadze
        }
        printf("\n");
      } /* if check for empty events */
      else { /* empty fragment */
        printf("\nType %d fragment %#x, %d words found\n", type, id, size);
        printf("\nEmpty Event\n");
      }
    }
  }
}

void dump_it(unsigned int nw, unsigned int * data) {

  unsigned int i;

  printf("size of data block: %d\n", nw);
  for (i = 0; i < nw; i++, data++) {
    printf("%4d:   %#10x\t%10u\n", i, *data, *data);
  }
}

/* ------------------------------------------------------------------------ */

void TileTBDump::find_frag(const uint32_t* data, unsigned int size, unsigned int version
                           , int /* verbosity */, T_RodDataFrag** frag, int* nfrag) {
  unsigned int offset = 0;
  *nfrag = 0;
  bool m_v3Format = (*(data) == 0xff1234ff); // additional frag marker since Sep 2005
  m_v3Format |= (*(data) == 0x00123400); // another possible frag marker (can appear in buggy ROD frags)
  if (m_v3Format) {
    m_sizeOverhead = 3;
    ++offset; // skip frag marker
    std::cout << " *(p) = 0x" << std::hex << (*(data)) << std::dec << std::endl;
    std::cout << " v3Format = true" << std::endl;
  } else {
    m_sizeOverhead = 2;
  }

  while (offset < size && *nfrag < MAX_ROD_FRAG) {
    //std::cout << "nfrag="<<(*nfrag) << " offset="<<offset<<" data[offset]="<<data[offset]<<std::endl;
    frag[*nfrag] = (T_RodDataFrag *) (data + offset);

    if (frag[*nfrag]->size < m_sizeOverhead
        || frag[*nfrag]->size > size - offset + m_sizeOverhead - 2) {

      printf("\nWarning: garbage in frag %d of current ROD -> ignore it\n", *nfrag);
      printf("Size:         \t%10u\n", frag[*nfrag]->size);
      printf("Id:           \t%10u\n", frag[*nfrag]->id);
      printf("Bad data:\n");
      for (; offset < size; ++offset) {
        printf("\t%d\t%d\t0x%x\n", offset, data[offset], data[offset]);
        if (data[offset] == 0xff1234ff) break;
      }
      if (m_v3Format) ++offset; // go to next good frag or jump outside ROD, if at the end

    } else if (frag[*nfrag]->size < size - offset && m_v3Format
                && data[offset + frag[*nfrag]->size - 1] != 0xff1234ff) {

      printf("\nWarning: frag %d of current ROD is damaged\n", *nfrag);
      printf("Size:         \t%10u\n", frag[*nfrag]->size);
      printf("Id:           \t%10u\n", frag[*nfrag]->id);
      printf("Bad data:\n");
      unsigned int newsize = 0;
      for (; offset < size; ++offset, ++newsize) {
        printf("\t%d\t%d\t0x%x\n", offset, data[offset], data[offset]);
        if (data[offset] == 0xff1234ff) break;
      }
      if (m_v3Format) {
        ++newsize;
        ++offset; // go to next good frag or jump outside ROD, if at the end
      }
      printf("Correct size is:\t%10u\n", newsize);

    } else {
      offset += frag[*nfrag]->size;
      if (version == 0x1 && offset < size) offset += 7; // skip extra header
      ++(*nfrag);
    }
  }

  if (m_v3Format) --offset; // set offset back to correct value

  if (offset > size) {
    --(*nfrag);
    printf("\nWarning: last fragment in current ROD is garbage -> ignore it\n");
    printf("N good frag:  \t%10d\n", *nfrag);
    printf("Last frag:\n");
    for (unsigned int i = offset - frag[*nfrag]->size; i < size; ++i) {
      printf("\t%d\t%d\t0x%x\n", i, data[i], data[i]);
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
    recocalib[ch].d_amp = m_rc2bytes4.amplitude(w);
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
      printf(" Format Type 1: Raw compressed : Wrong Size = %d\n", size);
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
          printf(" Verification ERROR for channel # %d (ch=%d g=%d ns=%d 0x%x)! \n", ch
                 , rawcomp[ch].chan, rawcomp[ch].gain, nsamp, rawcomp[ch].words[0] & 0xFFFF);
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
  } else {
    status = 1;
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
    if (size > 176 && size < 205)
      size = 179; /* we expect this number (9+2)*16+1+2 */
    else if (size > 272 && size < 405) size = 275; /* we expect this number (7*2+3)*16+1+2 */
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
          printf("Good parity, chip%3d, mode%2d, head %#8.8x, data %#8.8x\n", m, (int) dgm, data[0], data[1]);
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
          printf("Chip%3d, mode%2d, head 0x%8.8x, data 0x%8.8x\n", m, (int) dgm, data[0], data[1]);
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
          printf("Warning: No valid header found, calibration running mode(=1) assumed\n");
        } else {
          digi_mode = 0;
          printf("Warning: No valid header found, normal running mode(=0) assumed\n");
        }
      } else {
        if (verbosity > 3) {
          printf("Warning: No valid header found, keeping  previous running mode(=%d)\n", (int) digi_mode);
        }
      }
      status |= 2;
    } else {
      digi_mode = dgm; /* last found digi mode */
      if (dm > 2) { /* more than 2 good headers found */
        for (c = 0; c < dm; ++c)
          ++digm[digim[c]]; /* count different digi_modes */
        for (c = 0; c < 4; ++c)
          if (digm[c] > digm[digi_mode]) /* find most frequent digi_mode */
          digi_mode = c;
      }
      if (first) {
        first = 0;
        if (digi_mode > 0) m <<= 1;
        if (tile_check_parity(data, 1) == 0)
          printf("\nMode=%d found in header of chip %d\n", digi_mode, m);
        else
          printf("\nMode=%d found in header of chip %d with bad parity\n", digi_mode, m);
        if (digi_mode > 0)
          printf("\nCalibration mode selected, effective number of chips is twice bigger\n");
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
    if (gain_offs > 0)
      crcword = data[nsamp1 + gain_offs]; /* first gain */
    else
      crcword = data[nsamp1]; /* second gain */
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
    for (k = 16; reg != 0; reg >>= 1)
      reg1 |= (reg & 1) << (--k);

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

