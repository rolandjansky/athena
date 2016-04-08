/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileTBFrag_H
#define TileTBFrag_H

/**
* @brief definition of various fragments expected in BS files from testbeam
* @author Alexander Solodkov
*
*/

#define TILE_BEAM_ID   0x50
#define TILE_POS_ID    0x51
#define TILE_NEG_ID    0x52
#define TILE_EBPOS_ID  0x53
#define TILE_EBNEG_ID  0x54
#define COMMON_BEAM_ID 0x70

#define MAX_ROD_FRAG  16 /* max number of fragments in any ROD */ 

#define BEAM_TDC_FRAG 0x000
#define BEAM_ADC_FRAG 0x001
#define MUON_ADC_FRAG 0x002
#define ADDR_ADC_FRAG 0x003
#define LASE_PTN_FRAG 0x004
#define LASE_ADC_FRAG 0x005
#define ADD_FADC_FRAG 0x006
#define ECAL_ADC_FRAG 0x007

#define COIN_TRIG1_FRAG 0x008
#define COIN_TRIG2_FRAG 0x009
#define COIN_TRIG3_FRAG 0x00A
#define COIN_TRIG4_FRAG 0x00B
#define COIN_TRIG5_FRAG 0x00C
#define COIN_TRIG6_FRAG 0x00D
#define COIN_TRIG7_FRAG 0x00E
#define COIN_TRIG8_FRAG 0x00F

#define DIGI_PAR_FRAG 0x0ff

#define COMMON_ADC1_FRAG 0x010
#define COMMON_ADC2_FRAG 0x011
#define COMMON_TOF_FRAG  0x012
#define COMMON_TDC1_FRAG 0x013
#define COMMON_TDC2_FRAG 0x014
#define COMMON_PTN_FRAG  0x015
#define LASER_OBJ_FRAG   0x016
#define LASERII_OBJ_FRAG 0x017

#define MAX_PMT 48 /* 48 pmts (channels) in one drawer */
#define MAX_DIGI_CHAN 96 /* 16*3*2 channels in calibration mode */
#define MAX_CHAN_SAMP 36 /* in calib mode including header and CRC */

#include <string>

static const std::string BeamFragName[32] = {
  "Tile Beam TDC",    // 0x000
  "Tile Beam ADC",    // 0x001
  "Tile MuWall ADC1", // 0x002
  "Tile MuWall ADC2", // 0x003
  "Tile Pattern Unit",// 0x004
  "Tile Laser ADC",   // 0x005
  "Tile Adder FADC",  // 0x006
  "Tile Phantom ADC", // 0x007
  "Tile Coincidence Trig1", // 0x008
  "Tile Coincidence Trig2", // 0x009
  "Tile Coincidence Trig3", // 0x00A
  "Tile Coincidence Trig4", // 0x00B
  "Tile Coincidence Trig5", // 0x00C
  "Tile Coincidence Trig6", // 0x00D
  "Tile Coincidence Trig7", // 0x00E
  "Tile Coincidence Trig8", // 0x00F
  "Common Beam ADC1",         // 0x010
  "Common Beam ADC2",         // 0x011
  "Common Beam ToF TDC",      // 0x012
  "Common Beam TDC1",         // 0x013
  "Common Beam TDC2",         // 0x014
  "Common Beam Pattern Unit", // 0x015
  "LASTROD Laser data",       // 0x016
  "", "", "", "", "", "", "", "",
  "CIS parameters", // 0x0FF -> 0x01F
};

#endif
