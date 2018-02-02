/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <string>
#include <iostream>
#include <algorithm>

// Singleton method

//------------------------------------------------------------
TileCablingService * TileCablingService::instance(bool del)
{
    static  TileCablingService * cablingService = new TileCablingService();
    if (del) {
        delete cablingService;
        cablingService = 0;
    }
    return cablingService;
}

// default constructor

//-------------------------------------------------------------
TileCablingService::TileCablingService()  
  : m_tileID(0)
  , m_tileHWID(0)
  , m_TT_ID(0)
  , m_tileTBID(0)
  , m_cablingType(TileCablingService::OldSim)
  , m_isCacheFilled(false)
  , m_MBTSmergedRun2(64, 0)
  , m_E1mergedRun2(128, 0)
  , m_run2(false)
  , m_maxChannels(TileCalibUtils::MAX_CHAN)
  , m_maxGains(TileCalibUtils::MAX_GAIN)
  , m_msg("TileCablingService")
{
  m_testBeam = false;
  // old cabling for simulation - no special EB modules, wrong numbers for E1-E4
  m_EBAspec = -1;
  m_EBCspec = -1;
  m_E1chan = 13;
  m_E2chan = 12;
  m_E3chan = 1;
  m_E4chan = 0;
  m_E3special = 1; // no difference
  m_E4special = 0; // with normal drawers
  m_MBTSchan = 47;
  
  int ros=0;
  for ( ; ros<1; ++ros) { // no beam frags
    for (int drawer=0; drawer<64; ++drawer) {
      m_connected[ros][drawer] = false;
    }
  }
  for ( ; ros<5; ++ros) { // all drawers connected
    for (int drawer=0; drawer<64; ++drawer) {
      m_connected[ros][drawer] = true;
    }
  }

  fillConnectionTables();

  // Merged E1 and MBTS for run2
  // EBA
  m_E1mergedRun2[6]  =  7;
  m_E1mergedRun2[24] = 23;
  m_E1mergedRun2[43] = 42;
  m_E1mergedRun2[52] = 53;
  // EBC
  m_E1mergedRun2[64+6]  =  7;
  m_E1mergedRun2[64+24] = 23;
  m_E1mergedRun2[64+43] = 42;
  m_E1mergedRun2[64+52] = 53;
  // additional E4' in EBC
  m_E1mergedRun2[64+27] = 28;
  m_E1mergedRun2[64+30] = 31;
  m_E1mergedRun2[64+34] = 33;
  m_E1mergedRun2[64+37] = 36;


  m_MBTSmergedRun2[7] = true;
  m_MBTSmergedRun2[23] = true;
  m_MBTSmergedRun2[42] = true;
  m_MBTSmergedRun2[53] = true;
}

// destructor

//-------------------------------------------------------------
TileCablingService::~TileCablingService()
{
}

void
TileCablingService::setTestBeam (bool testBeam)
{ 
  if (testBeam) // don't print anything for full ATLAS configuration 
    std::cout << "==> Setting testbeam flag to " 
              << ((testBeam) ? "TRUE" : "FALSE")
              << " in TileCablingService " << std::endl;
  m_testBeam = testBeam;
  if (testBeam) {
    m_cablingType = TileCablingService::TestBeam;
    int ros=0;
    for ( ; ros<1; ++ros) {
      int drawer=0;
      for ( ; drawer<32; ++drawer) { // all 0x0? and 0x1? frags in beam crate
        m_connected[ros][drawer] = true;
      }
      for ( ; drawer<64; ++drawer) {
        m_connected[ros][drawer] = false;
      }
    }
    for ( ; ros<5; ++ros) { // nothing connected - use setConnected() to declare connected drawers
      for (int drawer=0; drawer<64; ++drawer) {
        m_connected[ros][drawer] = false;
      }
    }
  }
}

void
TileCablingService::setCablingType(TileCablingService::TileCablingType type)
{ 
// cabling type can be:
//  -1 for testbeam
//   0 for MBTS in spare channels and wrong cabling for gap/crack (like in simulation before rel 14.2.10)
//   1 new configuration without MBTS and correct gap/crack cells
//   2 new configuration with MBTS instead of crack scintillators
//   3 new configuration with MBTS in spare channels
  m_cablingType = type;

  m_maxGains = std::round(((double) m_tileID->adc_hash_max()) / m_tileID->pmt_hash_max());

  ATH_MSG_DEBUG("Detected maximum number of gains: " << m_maxGains);

  if (TileCablingService::TestBeam == type || TileCablingService::OldSim == type) {
    // old cabling for simulation - no special EB modules, wrong numbers for E1-E4
    m_EBAspec = -1;
    m_EBCspec = -1;
    m_E1chan = 13;
    m_E2chan = 12;
    m_E3chan = 1;
    m_E4chan = 0;
    m_E3special = 1; // no difference
    m_E4special = 0; // with normal drawers
    m_MBTSchan = 47;
    if (TileCablingService::TestBeam == type && !m_testBeam) 
      setTestBeam(true);
  } else {
    // new correct cabling for E1-E4 cells
    m_EBAspec = 14; // EBA15 - special
    m_EBCspec = 17; // EBC18 - special
    m_E1chan = 12;
    m_E2chan = 13;
    m_E3chan = 0;
    m_E4chan = 1;
    m_E3special = 18;
    m_E4special = 19;

    m_run2 = (type == TileCablingService::RUN2Cabling || type == TileCablingService::UpgradeABC);

    if (m_run2)
      m_MBTSchan = -1; // should use function MBTS2channel_run2(int eta)
    if (TileCablingService::MBTSOnly == type || m_run2)
      m_MBTSchan = 0;
    else // Crack and MBTS in spare channel
      m_MBTSchan = 47;

    if (type == TileCablingService::UpgradeABC) {

      if (m_tileID) {
        
        m_maxChannels = 240;
        const int nBarrelAndExtendedChannels(m_maxChannels * 2);
        
        m_ch2towerUpgradeABC.resize(nBarrelAndExtendedChannels, -1);
        m_ch2sampleUpgradeABC.resize(nBarrelAndExtendedChannels, -1);
        m_ch2pmtUpgradeABC.resize(nBarrelAndExtendedChannels, -1);
        
        
        IdContext pmtContext = m_tileID->pmt_context();
        unsigned int maxPmtHash = m_tileID->pmt_hash_max();
        
        for (IdentifierHash pmtHash = 0; pmtHash < maxPmtHash; pmtHash += 1) {
          Identifier swid;
          if (m_tileID->get_id(pmtHash, swid, &pmtContext) == 0) {
            int module = m_tileID->module(swid);
            if (module == 0) { // Use module 0 to fill caches
              int side = m_tileID->side(swid);
              if (side == TileID::POSITIVE) { // Use only positive side to fill caches
                int section = m_tileID->section(swid);
                int tower = m_tileID->tower(swid);
                int sample = m_tileID->sample(swid);
                int pmt = m_tileID->pmt(swid);

                if (tower == 41 || (tower > 42 && tower < 46)) continue; // Skip dummy towers
                
                int channel = swid2channel_upgradeABC(section, TileID::POSITIVE, tower, sample, pmt);
                
                if (channel >= 0 && (unsigned int) channel < m_ch2towerUpgradeABC.size()) {
                  if (section != TileID::BARREL) channel += m_maxChannels; 
                  m_ch2towerUpgradeABC[channel] = tower;
                  m_ch2sampleUpgradeABC[channel] = sample;
                  m_ch2pmtUpgradeABC[channel] = pmt;
                }
                
              }
            }
          }
        }
        
      } else {
        ATH_MSG_ERROR("Tile ID helpers should be set up before setting UpgradeABC cabling type!");
      }

    }

  }
}

void
TileCablingService::setConnected(int section, int side, int modMin, int modMax)
{ 
  if (modMax > modMin) {
    std::cout << "TileCablingService: in section " << section << " side " << side 
              << " modules from " << modMin << " to " << modMax-1 << " are connected" << std::endl;
    int ros=swid2ros(section,side);
    for (int module=modMin; module<modMax; ++module) {
      int drawer=swid2drawer(module);
      m_connected[ros][drawer] = true;
    }
  } else {
    std::cout << "TileCablingService: in section " << section << " side " << side 
              << " no modules connected " << std::endl;
  }  
}

void
TileCablingService::setConnected(int ros, int draMin, int draMax)
{ 
  for (int drawer=draMin; drawer<draMax; ++drawer) {
    m_connected[ros][drawer] = true;
  }
}

//
// Convert TileID to Trigger Tower ID
//

Identifier
TileCablingService::cell2tt_id ( const Identifier & id ) const
{
  // take only first pmt - might be wrong for D-cells
  return pmt2tt_id(m_tileID->pmt_id(id,0));
}

Identifier
TileCablingService::pmt2tt_id ( const Identifier & id ) const
{
  int mineta[4] = { 0, 0,  9,  9 };
  int maxeta[4] = { 0, 8, 14, 14 };
  
  int section  = m_tileID->section  (id);
  int side     = m_tileID->side     (id);
  int module   = m_tileID->module   (id);
  int tower    = m_tileID->tower    (id);
  int sample   = m_tileID->sample   (id);
  int pmt      = m_tileID->pmt      (id);

  if (m_cablingType == UpgradeABC) {
    tower /= 4;
    if (sample == TileID::SAMP_X) sample = TileID::SAMP_BC;
  }

  int posneg = side; // -1 or +1
  int sampling = 1;  // Hadronic
  int region = 0;    // abs(eta)<2.5
  int ieta = tower;
  int iphi = module;

  if (TileID::SAMP_E == sample) {
    ieta = maxeta[section] + 1; // all gap scintillators are outside
    // return m_invalid_id;
  } else {
    if (TileID::SAMP_D == sample) {
      if ( 0 == ieta ) {
        if (m_testBeam) {
          if ( 0 == pmt ) { // testbeam - cell D0, first pmt is in another side;
            if      ( TileID::NEGATIVE == side )  posneg = TileID::POSITIVE;
            else if ( TileID::POSITIVE == side )  posneg = TileID::NEGATIVE;
          }
        } else {
          if ( 1 == pmt ) { // cell D0, second pmt is in another side;
            if      ( TileID::NEGATIVE == side )  posneg = TileID::POSITIVE;
            else if ( TileID::POSITIVE == side )  posneg = TileID::NEGATIVE;
          }
        }
      } else {
        if ( ieta > 8 ) pmt = 1-pmt;
        if ( TileID::NEGATIVE == side ) pmt = 1-pmt;
        ieta -= pmt; // second pmt in D samling is in previous trigger tower
      }
    }
    
    // merge several towers in one trigger tower at section boundaries
    // A10 goes to the same tower as A9 and B9
    // D4 goes to the same tower as C10 and D5(pmt=1)
    // A16 goes to the same tower as A15 and B15
    ieta = std::max(ieta,mineta[section]);
    ieta = std::min(ieta,maxeta[section]);
  }
  
  Identifier tt_id = m_TT_ID->tower_id(posneg, sampling, region, ieta, iphi);

  return tt_id;
}

//
// Convert TileID to Muon Trigger Tower ID (valid only for D-cells)
//

Identifier
TileCablingService::cell2mt_id ( const Identifier & id ) const
{
  // take only first pmt - might be wrong for D0 cell
  return pmt2mt_id(m_tileID->pmt_id(id,0));
}

Identifier
TileCablingService::pmt2mt_id ( const Identifier & id ) const
{
//  int section  = m_tileID->section  (id);
  int side     = m_tileID->side     (id);
  int module   = m_tileID->module   (id);
  int tower    = m_tileID->tower    (id);
  int sample   = m_tileID->sample   (id);
  int pmt      = m_tileID->pmt      (id);

  if (m_cablingType == UpgradeABC) {
    tower /= 4;
    if (sample == TileID::SAMP_X) sample = TileID::SAMP_BC;
  }

  if (TileID::SAMP_D == sample) {
    int posneg = side; // -1 or +1
    int sampling = 2;  // muon signal from last sampling
    int region = 0;    // abs(eta)<2.5
    int ieta = tower;
    int iphi = module;

    if ( 0 == ieta ) {
      if (m_testBeam) {
        if ( 0 == pmt ) { // testbeam - cell D0, first pmt is in another side;
          if      ( TileID::NEGATIVE == side )  posneg = TileID::POSITIVE;
          else if ( TileID::POSITIVE == side )  posneg = TileID::NEGATIVE;
        }
      } else {
        if ( 1 == pmt ) { // cell D0, second pmt is in another side;
          if      ( TileID::NEGATIVE == side )  posneg = TileID::POSITIVE;
          else if ( TileID::POSITIVE == side )  posneg = TileID::NEGATIVE;
        }
      }
    }

    Identifier tt_id = m_TT_ID->tower_id(posneg, sampling, region, ieta, iphi) ;
    return tt_id;

  } else {

    return m_invalid_id;
  }
}
 
//
// Build MBTS offline ID from drawer HWID
// return invalid ID if there is no MBTS in a given drawer
//

Identifier
TileCablingService::drawer2MBTS_id ( const HWIdentifier & hwid ) const
{
  int ros      = m_tileHWID->ros     (hwid);
  int drawer   = m_tileHWID->drawer  (hwid);
  int channel  = m_MBTSchan;
  
  if ( TestBeam == m_cablingType || CrackOnly == m_cablingType ) {

    return m_invalid_id;
  
  } else if ( m_run2) {

    if (hwid2MBTSconnected_run2(ros,drawer)) {

      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_run2(drawer);
      int eta  = hwid2MBTSeta_run2(drawer);

      return m_tileTBID->channel_id(side, phi, eta);

    } else {
      
      return m_invalid_id;
    }

  } else if ( MBTSOnly == m_cablingType ) {

    if (hwid2MBTSconnected_real(ros,drawer,channel)) {

      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_real(ros,drawer,channel);
      int eta  = hwid2MBTSeta_real(ros,drawer,channel);

      return m_tileTBID->channel_id(side, phi, eta);

    } else {
      
      return m_invalid_id;
    }

  } else {

    if (hwid2MBTSconnected(ros,drawer,channel)) {

      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi(drawer,channel);
      int eta  = hwid2MBTSeta(drawer,channel);

      return m_tileTBID->channel_id(side, phi, eta);

    } else {

      return m_invalid_id;
    }
  }
}


//
// Convert TileHWID to TileID
//

Identifier
TileCablingService::h2s_cell_id ( const HWIdentifier & hwid ) const
{
  int ros      = m_tileHWID->ros     (hwid);
  int drawer   = m_tileHWID->drawer  (hwid);
  int channel  = m_tileHWID->channel (hwid);
  if ( EB_special(ros,drawer) ) {
    if ( D4(channel) ) return m_invalid_id;
    EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18
  }
  
  if ( TileHWID::BEAM_ROS == ros ) {
      
    int tbtype    = hwid2tbtype (drawer);
    int tbmodule  = hwid2tbmodule (drawer,channel);
    int tbchannel = hwid2tbchannel (drawer,channel);
      
    return m_tileTBID->channel_id(tbtype, tbmodule, tbchannel);
  }

  int sample = (m_cablingType != UpgradeABC) ? hwid2sample(ros, channel) 
                                             : hwid2sample_upgradeABC(ros, channel);

  if (sample < 0)
    return m_invalid_id; // for any kind of cabling can't have negagive sample

  if (m_run2) {

    if ((ros > 2) && hwid2MBTSconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_run2(drawer);
      int eta  = hwid2MBTSeta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if ((ros > 3) && hwid2E4prconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2E4prphi_run2(drawer);
      int eta  = hwid2E4preta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X && m_cablingType != TileCablingService::UpgradeABC) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else if (m_cablingType == MBTSOnly) {

    if ((ros > 2) && hwid2MBTSconnected_real(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_real(ros,drawer,channel);
      int eta  = hwid2MBTSeta_real(ros,drawer,channel);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else {

    if (sample == TileID::SAMP_X) { // MBTS

      if (m_cablingType == OldSim || m_cablingType == CrackAndMBTS) { // cablings with MBTS in spare channel
        if (hwid2MBTSconnected(ros,drawer,channel)) {
                                                                                            
          int side = hwid2side(ros,channel);
          int phi  = hwid2MBTSphi(drawer,channel);
          int eta  = hwid2MBTSeta(drawer,channel);
                                                                                            
          return m_tileTBID->channel_id(side, phi, eta);
                                                                                            
        } else
          return m_invalid_id;
      } else
        return m_invalid_id;
    }
  }

  int section = hwid2section  (ros,channel);
  int side    = hwid2side     (ros,channel);
  int module  = hwid2module   (drawer);

  int tower = (m_cablingType != UpgradeABC) ? hwid2tower(ros, channel)
                                            : hwid2tower_upgradeABC(ros, channel);

  if (m_cablingType == MBTSOnly) {
    if( ros > 2 && isTileGapCrack(channel) ) { // E3 might be disconnected - check this

      int module   = hwid2module_gapscin   (ros, drawer, channel);
      int tower    = hwid2tower_gapscin    (ros, drawer, channel);
     
      if (tower == -1) {
        return m_invalid_id;
      } else
	return m_tileID->cell_id(section, side, module, tower, sample);
    }
  }
                                                                                            
  return m_tileID->cell_id(section, side, module, tower, sample);
}

Identifier
TileCablingService::h2s_cell_id_index_find ( int ros, int drawer, int channel, int & index, int & pmt ) const
{
  if ( EB_special(ros,drawer) ) {
    if ( D4(channel) ) { index = pmt = -1; return m_invalid_id; }
    EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18
  }
  
  if ( TileHWID::BEAM_ROS == ros ) {

    int tbtype    = hwid2tbtype (drawer);
    int tbmodule  = hwid2tbmodule (drawer,channel);
    int tbchannel = hwid2tbchannel (drawer,channel);
    return m_tileTBID->channel_id(tbtype, tbmodule, tbchannel);
  }
  
  int sample = (m_cablingType != UpgradeABC) ? hwid2sample(ros, channel) 
                                             : hwid2sample_upgradeABC(ros, channel);

  if (sample < 0) {
    index = pmt = -1;
    return m_invalid_id; // for any kind of cabling can't have negagive sample
  }
  
  if (m_run2) {

    if ((ros > 2) && hwid2MBTSconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_run2(drawer);
      int eta  = hwid2MBTSeta_run2(drawer);

      pmt = 0;
      index = -2;
      return m_tileTBID->channel_id(side, phi, eta);

    } else if ((ros > 3) && hwid2E4prconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2E4prphi_run2(drawer);
      int eta  = hwid2E4preta_run2(drawer);

      pmt = 0;
      index = -3;
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X && m_cablingType != TileCablingService::UpgradeABC) { // can't have MBTS anymore
      index = pmt = -1;
      return m_invalid_id;
    }
    
  } else if (m_cablingType == MBTSOnly) {

    if ((ros > 2) && hwid2MBTSconnected_real(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_real(ros,drawer,channel);
      int eta  = hwid2MBTSeta_real(ros,drawer,channel);

      pmt = 0;
      index = -2;
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X) { // can't have MBTS anymore
      index = pmt = -1;
      return m_invalid_id;
    }
    
  } else {

    if (sample == TileID::SAMP_X) { // MBTS

      if (m_cablingType == OldSim || m_cablingType == CrackAndMBTS) { // cablings with MBTS in spare channel
        if (hwid2MBTSconnected(ros,drawer,channel)) {
                                                                                            
          int side = hwid2side(ros,channel);
          int phi  = hwid2MBTSphi(drawer,channel);
          int eta  = hwid2MBTSeta(drawer,channel);
                                                                                            
          pmt = 0;
          index = -2;
  
          return m_tileTBID->channel_id(side, phi, eta);
                                                                                            
        } else {
          index = pmt = -1;
          return m_invalid_id;
        }
      } else {
        index = pmt = -1;
        return m_invalid_id;
      }
    }
  }

  int section  = hwid2section  (ros,channel);
  int side     = hwid2side     (ros,channel);
  int module   = hwid2module   (drawer);

  int tower(-1);

  if (m_cablingType != UpgradeABC) {
    tower = hwid2tower(ros, channel);
    pmt = hwid2pmt(ros, channel);
  } else {
    tower = hwid2tower_upgradeABC(ros, channel);
    pmt = hwid2pmt_upgradeABC(ros, channel);
  }

  Identifier cellid;

  if (m_cablingType == MBTSOnly) {
    if( ros > 2 && isTileGapCrack(channel) ) { // E3 might be disconnected - check this

      int module   = hwid2module_gapscin   (ros, drawer, channel);
      int tower    = hwid2tower_gapscin    (ros, drawer, channel);
     
      if (tower == -1) {
        index = pmt = -1;
        return m_invalid_id;
      } else {
	cellid = m_tileID->cell_id(section, side, module, tower, sample);
	index = m_tileID->cell_hash(cellid);
	return cellid;
      }
    }
  }

  cellid = m_tileID->cell_id(section, side, module, tower, sample);
  index = m_tileID->cell_hash(cellid);
  return cellid;
}

Identifier
TileCablingService::h2s_pmt_id ( const HWIdentifier & hwid ) const
{
  int ros      = m_tileHWID->ros     (hwid);
  int drawer   = m_tileHWID->drawer  (hwid);
  int channel  = m_tileHWID->channel (hwid);
  if ( EB_special(ros,drawer) ) {
    if ( D4(channel) ) return m_invalid_id;
    EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18
  }

  if ( TileHWID::BEAM_ROS == ros ) {

    int tbtype    = hwid2tbtype (drawer);
    int tbmodule  = hwid2tbmodule (drawer,channel);
    int tbchannel = hwid2tbchannel (drawer,channel);

    return m_tileTBID->channel_id(tbtype, tbmodule, tbchannel);
  }

  int sample = (m_cablingType != UpgradeABC) ? hwid2sample(ros, channel) 
                                             : hwid2sample_upgradeABC(ros, channel);

  if (sample < 0)
    return m_invalid_id; // for any kind of cabling can't have negagive sample

  if (m_run2) {

    if ((ros > 2) && hwid2MBTSconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_run2(drawer);
      int eta  = hwid2MBTSeta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if ((ros > 3) && hwid2E4prconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2E4prphi_run2(drawer);
      int eta  = hwid2E4preta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X && m_cablingType != TileCablingService::UpgradeABC) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else if (m_cablingType == MBTSOnly) {

    if ((ros > 2) && hwid2MBTSconnected_real(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_real(ros,drawer,channel);
      int eta  = hwid2MBTSeta_real(ros,drawer,channel);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else {

    if (sample == TileID::SAMP_X) { // MBTS

      if (m_cablingType == OldSim || m_cablingType == CrackAndMBTS) { // cablings with MBTS in spare channel
        if (hwid2MBTSconnected(ros,drawer,channel)) {
                                                                                            
          int side = hwid2side(ros,channel);
          int phi  = hwid2MBTSphi(drawer,channel);
          int eta  = hwid2MBTSeta(drawer,channel);
                                                                                            
          return m_tileTBID->channel_id(side, phi, eta);
                                                                                            
        } else
          return m_invalid_id;
      } else
        return m_invalid_id;
    }
  }

  int section  = hwid2section  (ros,channel);
  int side     = hwid2side     (ros,channel);
  int module   = hwid2module   (drawer);

  int tower(-1);
  int pmt(-1);

  if (m_cablingType != UpgradeABC) {
    tower = hwid2tower(ros, channel);
    pmt = hwid2pmt(ros, channel);
  } else {
    tower = hwid2tower_upgradeABC(ros, channel);
    pmt = hwid2pmt_upgradeABC(ros, channel);
  }
    
  if (m_cablingType == MBTSOnly) {
    if( ros > 2 && isTileGapCrack(channel) ) { // E3 might be disconnected - check this

      int module   = hwid2module_gapscin   (ros, drawer, channel);
      int tower    = hwid2tower_gapscin    (ros, drawer, channel);
     
      if (tower == -1) {
        return m_invalid_id;
      } else
	return m_tileID->pmt_id(section, side, module, tower, sample, pmt);
    }
  }
                                                                                            
  return m_tileID->pmt_id(section, side, module, tower, sample, pmt);
}

Identifier
TileCablingService::h2s_adc_id ( const HWIdentifier & hwid ) const
{
  int ros      = m_tileHWID->ros     (hwid);
  int drawer   = m_tileHWID->drawer  (hwid);
  int channel  = m_tileHWID->channel (hwid);
  if ( EB_special(ros,drawer) ) {
    if ( D4(channel) ) return m_invalid_id;
    EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18
  }

  if ( TileHWID::BEAM_ROS == ros ) {

    int tbtype    = hwid2tbtype (drawer);
    int tbmodule  = hwid2tbmodule (drawer,channel);
    int tbchannel = hwid2tbchannel (drawer,channel);

    return m_tileTBID->channel_id(tbtype, tbmodule, tbchannel);
  }

  int sample = (m_cablingType != UpgradeABC) ? hwid2sample(ros, channel) 
                                             : hwid2sample_upgradeABC(ros, channel);

  if (sample < 0)
    return m_invalid_id;

  if (m_run2) {

    if ((ros > 2) && hwid2MBTSconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_run2(drawer);
      int eta  = hwid2MBTSeta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if ((ros > 3) && hwid2E4prconnected_run2(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2E4prphi_run2(drawer);
      int eta  = hwid2E4preta_run2(drawer);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X && m_cablingType != TileCablingService::UpgradeABC) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else if (m_cablingType == MBTSOnly) {

    if ((ros > 2) && hwid2MBTSconnected_real(ros,drawer,channel)) {
      int side = hwid2side(ros,channel);
      int phi  = hwid2MBTSphi_real(ros,drawer,channel);
      int eta  = hwid2MBTSeta_real(ros,drawer,channel);
      return m_tileTBID->channel_id(side, phi, eta);

    } else if (sample == TileID::SAMP_X) { // can't have MBTS anymore
      return m_invalid_id;
    }
    
  } else {

    if (sample == TileID::SAMP_X) { // MBTS

      if (m_cablingType == OldSim || m_cablingType == CrackAndMBTS) { // cablings with MBTS in spare channel
        if (hwid2MBTSconnected(ros,drawer,channel)) {
                                                                                            
          int side = hwid2side(ros,channel);
          int phi  = hwid2MBTSphi(drawer,channel);
          int eta  = hwid2MBTSeta(drawer,channel);
                                                                                            
          return m_tileTBID->channel_id(side, phi, eta);
                                                                                            
        } else
          return m_invalid_id;
      } else
        return m_invalid_id;
    }
  }

  int section  = hwid2section  (ros,channel);
  int side     = hwid2side     (ros,channel);
  int module   = hwid2module   (drawer);
  int adc      = m_tileHWID->adc(hwid);

  int tower(-1);
  int pmt(-1);

  if (m_cablingType != UpgradeABC) {
    tower = hwid2tower(ros, channel);
    pmt = hwid2pmt(ros, channel);
  } else {
    tower = hwid2tower_upgradeABC(ros, channel);
    pmt = hwid2pmt_upgradeABC(ros, channel);
  }

    
  if (m_cablingType == MBTSOnly) {
    if( ros > 2 && isTileGapCrack(channel) ) { // E3 might be disconnected - check this

      int module   = hwid2module_gapscin   (ros, drawer, channel);
      int tower    = hwid2tower_gapscin    (ros, drawer, channel);
     
      if (tower == -1) {
        return m_invalid_id;
      } else
        return m_tileID->adc_id(section, side, module, tower, sample, pmt, adc);
    }
  }
                                                                                            
  return m_tileID->adc_id(section, side, module, tower, sample, pmt, adc);
}

//
// Convert TileID to TileHWID
//

HWIdentifier
TileCablingService::s2h_drawer_id ( const Identifier & swid ) const
{
  int section  = m_tileID->section  (swid);
  int ros, drawer;

  if ( TileTBID::TILE_TESTBEAM == section ) {
      
    int tbtype    = m_tileTBID->type(swid);
    int tbmodule  = m_tileTBID->module(swid);
    int tbchannel = m_tileTBID->channel(swid);

    if (tbtype < 2) { // MBTS side +/- 1

      ros = swid2ros(TileID::EXTBAR,tbtype);

      if (m_run2) {
        drawer = MBTS2drawer_run2(tbmodule,tbchannel);
      } else if (m_cablingType == MBTSOnly) {
        drawer = MBTS2drawer_real(ros, tbmodule,tbchannel);
      } else {
        drawer = MBTS2drawer(tbmodule,tbchannel);
      }

      return m_tileHWID->drawer_id(ros, drawer);
    }

    drawer =  tbid2drawer(tbtype,tbmodule);

    return m_tileHWID->drawer_id(TileHWID::BEAM_ROS,drawer);
  }
    
  int side     = m_tileID->side     (swid);
  int module   = m_tileID->module   (swid);
  int tower    = m_tileID->tower    (swid);
  int sample   = m_tileID->sample   (swid);

  if (tower == 0 ) { // special case for D-0 cell
    int pmt      = m_tileID->pmt      (swid);

    int channel = (m_cablingType != UpgradeABC) 
      ? swid2channel(section,side,tower,sample,pmt)
      : swid2channel_upgradeABC (section,side,tower,sample,pmt);
      
    // special case for D0 cell, 2 PMTs in different sides
    if (isChannelFromOppositeSide(channel)) {
      if      ( TileID::NEGATIVE == side )  side = TileID::POSITIVE;
      else if ( TileID::POSITIVE == side )  side = TileID::NEGATIVE;
    }
  }
    
  ros = swid2ros (section,side);

  if (sample == TileID::SAMP_E && (m_run2)) {
    drawer = swid2drawer_gapscin_run2(side, module,tower); // E1 might go to another module
  } else if (sample == TileID::SAMP_E && m_cablingType == MBTSOnly) {
    drawer   = swid2drawer_gapscin  (side,module,tower);
    if (drawer < 0) drawer = module; // special trick for disconnected E3 and E4
  } else {
    drawer   = swid2drawer  (module);
  }

  return m_tileHWID->drawer_id(ros, drawer);
}

HWIdentifier
TileCablingService::s2h_channel_id ( const Identifier & swid ) const
{
  int section  = m_tileID->section  (swid);
  int ros, drawer, channel;

  if ( TileTBID::TILE_TESTBEAM == section ) {

    int tbtype    = m_tileTBID->type(swid);
    int tbmodule  = m_tileTBID->module(swid);
    int tbchannel = m_tileTBID->channel(swid);

    if (tbtype < 2) { // MBTS side +/- 1

      ros = swid2ros(TileID::EXTBAR,tbtype);

      if (m_run2) {
        drawer = MBTS2drawer_run2(tbmodule,tbchannel);
        return m_tileHWID->channel_id(ros, drawer, MBTS2channel_run2(tbchannel));
      } else if (m_cablingType == MBTSOnly) {
        drawer = MBTS2drawer_real(ros, tbmodule,tbchannel);
      } else {
        drawer = MBTS2drawer(tbmodule,tbchannel);
      }

      return m_tileHWID->channel_id(ros, drawer, m_MBTSchan);
    }

    drawer =  tbid2drawer(tbtype,tbmodule);
    channel = tbid2channel(drawer,tbchannel);

    return m_tileHWID->channel_id(TileHWID::BEAM_ROS, drawer, channel);
  }

  int side     = m_tileID->side     (swid);
  int module   = m_tileID->module   (swid);
  int tower    = m_tileID->tower    (swid);
  int sample   = m_tileID->sample   (swid);
  int pmt      = m_tileID->pmt      (swid);

  if (sample == TileID::SAMP_E && (m_run2)) {
    drawer   = swid2drawer_gapscin_run2  (side, module,tower);  // E1 might go to another module

    channel = (m_cablingType != UpgradeABC) 
      ? swid2channel(section,side,tower,sample,pmt)
      : swid2channel_upgradeABC (section,side,tower,sample,pmt);

  } else if (sample == TileID::SAMP_E && m_cablingType == MBTSOnly) {
    drawer   = swid2drawer_gapscin  (side,module,tower);
    channel  = swid2channel_gapscin (side,module,tower);
    if (channel < 0) {
      drawer = module;
      channel = (tower<15) ? 45 : 46; // special trick : connect non-existing E3 and E4
    }
    
  } else {
    drawer   = swid2drawer  (module);

    channel = (m_cablingType != UpgradeABC) 
      ? swid2channel(section,side,tower,sample,pmt)
      : swid2channel_upgradeABC (section,side,tower,sample,pmt);
  }

  if (channel < 0)
    return m_invalid_hwid;
    
  // special case for D0 cell, 2 PMTs in different sides
  if (isChannelFromOppositeSide(channel)) {
    if      ( TileID::NEGATIVE == side )  side = TileID::POSITIVE;
    else if ( TileID::POSITIVE == side )  side = TileID::NEGATIVE;

    channel = (m_cablingType != UpgradeABC) 
      ? swid2channel(section,side,tower,sample,pmt)
      : swid2channel_upgradeABC (section,side,tower,sample,pmt);
  }

  ros = swid2ros (section,side);
  EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18

  return m_tileHWID->channel_id(ros, drawer, channel);
}

HWIdentifier
TileCablingService::s2h_adc_id ( const Identifier & swid ) const
{
  int section  = m_tileID->section  (swid);
  int ros, drawer, channel;

  if ( TileTBID::TILE_TESTBEAM == section ) {

    int tbtype    = m_tileTBID->type(swid);
    int tbmodule  = m_tileTBID->module(swid);
    int tbchannel = m_tileTBID->channel(swid);

    if (tbtype < 2) { // MBTS side +/- 1

      ros = swid2ros(TileID::EXTBAR,tbtype);

      if (m_run2) {
        drawer = MBTS2drawer_run2(tbmodule,tbchannel);
        return m_tileHWID->adc_id(ros, drawer, MBTS2channel_run2(tbchannel), TileHWID::HIGHGAIN);
      } else if (m_cablingType == MBTSOnly) {
        drawer = MBTS2drawer_real(ros, tbmodule,tbchannel);
      } else {
        drawer = MBTS2drawer(tbmodule,tbchannel);
      }

      return m_tileHWID->adc_id(ros, drawer,m_MBTSchan,TileHWID::HIGHGAIN);
    }
    
    drawer =  tbid2drawer(tbtype,tbmodule);
    channel = tbid2channel(drawer,tbchannel);

    return m_tileHWID->channel_id(TileHWID::BEAM_ROS, drawer, channel);
  }

  int side     = m_tileID->side     (swid);
  int module   = m_tileID->module   (swid);
  int tower    = m_tileID->tower    (swid);
  int sample   = m_tileID->sample   (swid);
  int pmt      = m_tileID->pmt      (swid);
  int adc      = m_tileID->adc      (swid);

  if (sample == TileID::SAMP_E && (m_run2)) {
    drawer   = swid2drawer_gapscin_run2  (side, module,tower); // E1 might go to another module

    channel = ((m_cablingType != UpgradeABC) ? swid2channel(section,side,tower,sample,pmt)
                                             : swid2channel_upgradeABC(section,side,tower,sample,pmt));

  } else if (sample == TileID::SAMP_E && m_cablingType == MBTSOnly) {
    drawer   = swid2drawer_gapscin  (side,module,tower);
    channel  = swid2channel_gapscin (side,module,tower);
    if (channel < 0) {
      drawer = module;
      channel = (tower<15) ? 45 : 46; // special trick : connect non-existing E3 and E4
    }
    
  } else {
    drawer   = swid2drawer  (module);

    channel = (m_cablingType != UpgradeABC) ? swid2channel(section,side,tower,sample,pmt)
                                            : swid2channel_upgradeABC (section,side,tower,sample,pmt);
  }

  if (channel < 0)
    return m_invalid_hwid;

  // special case for D0 cell, 2 PMTs in different sides
  if (isChannelFromOppositeSide(channel)) {
    if      ( TileID::NEGATIVE == side )  side = TileID::POSITIVE;
    else if ( TileID::POSITIVE == side )  side = TileID::NEGATIVE;

    channel = (m_cablingType != UpgradeABC) ? swid2channel(section,side,tower,sample,pmt)
                                            : swid2channel_upgradeABC (section,side,tower,sample,pmt);
  }

  ros = swid2ros(section,side);
  EB_special_move_channel(ros,drawer,channel); // move channel 18,19 to 0,1 for EBA15, EBC18

  return m_tileHWID->adc_id(ros, drawer, channel, adc);
}

//
// find frag number for given ID
//

int
TileCablingService::frag ( const Identifier & swid ) const
{
  int section  = m_tileID->section(swid);
  int ros, drawer;
 
  if ( TileTBID::TILE_TESTBEAM == section ) {
      
    int tbtype    = m_tileTBID->type(swid);
    int tbmodule  = m_tileTBID->module(swid);
    int tbchannel = m_tileTBID->channel(swid);

    if (tbtype < 2) { // MBTS side +/- 1

      ros = swid2ros(TileID::EXTBAR,tbtype);

      if (m_run2) {
        drawer = MBTS2drawer_run2(tbmodule,tbchannel);
      } else if (m_cablingType == MBTSOnly) {
        drawer = MBTS2drawer_real(ros, tbmodule,tbchannel);
      } else {
        drawer = MBTS2drawer(tbmodule,tbchannel);
      }

      return m_tileHWID->frag(ros, drawer);
    }

    drawer = tbid2drawer(tbtype,tbmodule);

    return m_tileHWID->frag(TileHWID::BEAM_ROS,drawer);
  }
    
  int side     = m_tileID->side(swid);
  int module   = m_tileID->module(swid);
  int tower    = m_tileID->tower(swid);
  int sample   = m_tileID->sample(swid);

  if (tower == 0 ) { // special case for D-0 cell
    int pmt = m_tileID->pmt(swid);

    int channel = (m_cablingType != UpgradeABC) ? swid2channel(section,side,tower,sample,pmt)
                                                : swid2channel_upgradeABC (section,side,tower,sample,pmt);

    // special case for D0 cell, 2 PMTs in different sides
    if (isChannelFromOppositeSide(channel)) {
      if      ( TileID::NEGATIVE == side )  side = TileID::POSITIVE;
      else if ( TileID::POSITIVE == side )  side = TileID::NEGATIVE;
    }
  }
    
  ros = swid2ros (section, side);

  if (sample == TileID::SAMP_E && (m_run2)) {
    drawer = swid2drawer_gapscin_run2  (side, module,tower); // E1 might go to another module
  } else if (sample == TileID::SAMP_E && m_cablingType == MBTSOnly) {
    drawer = swid2drawer_gapscin(side,module,tower);
    if (drawer < 0) drawer = module; // special trick for disconnected E3 and E4
  } else {
    drawer = swid2drawer(module);
  }

  return m_tileHWID->frag(ros, drawer);
}

int
TileCablingService::frag ( const HWIdentifier & hwid ) const
{
    int ros      = m_tileHWID->ros     (hwid);
    int drawer   = m_tileHWID->drawer  (hwid);

    return m_tileHWID->frag(ros, drawer);
}

int
TileCablingService::frag2channels ( const HWIdentifier & hwid, std::vector<HWIdentifier> & ids) const
{
    ids.clear();
    ids.reserve(m_maxChannels);
  
    int channel = 0;
    for ( ; channel < m_maxChannels; ++channel) {
        ids.push_back(m_tileHWID->channel_id(hwid, channel));
    }
    
    return channel;
}

int
TileCablingService::frag2channels ( int frag, std::vector<HWIdentifier> & ids) const
{ 
  return frag2channels (m_tileHWID->drawer_id(frag),ids);
}

//
// All static methods for mapping between TileID and TileHWID
//

int
TileCablingService::hwid2section ( int ros, int channel )
{
    int section;

    if (ros > 2) { // extended barrel ROS

        // move cells D4, C10 and gap scin to gap section
        if (channel < 6 || channel == 12 || channel == 13)
            section = TileID::GAPDET;
        else
	    section = TileID::EXTBAR;
    }
    else {
        section = TileID::BARREL;
    }

    return section;
}

int
TileCablingService::hwid2side ( int ros, int channel ) const
{
    int side;

    if (channel == 0 && ( ros == LBA || ros == LBC)) { // put both PMTs of cell D0 in positive side

      side = TileID::POSITIVE;

    } else {
    
      if ( ros & 1 ) { // odd crates are in positive side
          side = TileID::POSITIVE;
      } else {
          side = TileID::NEGATIVE;
      }

      // at the testbeam "A" and "C" sides have opposite meaning
      if (m_testBeam) {
        if      ( TileID::NEGATIVE == side )  side = TileID::POSITIVE;
        else if ( TileID::POSITIVE == side )  side = TileID::NEGATIVE;
      }
    }
    
    return side;
}

int
TileCablingService::hwid2module ( int drawer )
{
    return drawer;
}

int
TileCablingService::hwid2tower ( int ros, int channel ) const
{
    int twr[96] = {
        0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,
        2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,
        4,  4,  5,  5,  5,  6, -1, -1,  6,  6,  6,  7,
        8,  8,  7,  7,  7,  6,  8, -1,  6,  9,  9,  8, 

       15, 13,  8,  8,  9,  9, 11, 11, 10, 10, 12, 12,
       11, 10, 11, 11, 10, 10, -1, -1, 13, 13, 12, 12,
       -1, -1, -1, -1, -1, -1, 13, 14, 14, -1, -1, 13,
       14, 12, 12, 14, 15, 15, -1, -1, -1, -1, -1, -1 };
    
    if (ros > 2) {
      channel += 48; // extended barrel
      twr[48+m_E1chan] = 10; // old and new simulation has different numbers here
      twr[48+m_E2chan] = 11;
      twr[48+m_E3chan] = 13;
      twr[48+m_E4chan] = 15;
    }
    int tower = twr[channel];
    
    return tower;
}

int
TileCablingService::hwid2tower_upgradeABC( int ros, int channel ) const {

  if (channel >= 0) {
    if (ros > 2) channel += m_maxChannels; // extended barrel
    
    if ((unsigned int) channel < m_ch2towerUpgradeABC.size()) {
      return m_ch2towerUpgradeABC[channel];
    }
  }

  return -1;
}


int
TileCablingService::hwid2sample ( int ros, int channel  )
{
    int smp[96] = {
        2,  0,  1,  1,  0,  0,  1,  1,  0,  0,  0,  1,
        1,  2,  2,  0,  1,  1,  0,  0,  0,  1,  1,  0,
        2,  2,  0,  1,  1,  0, -1, -1,  0,  1,  1,  0,
        0,  0,  0,  1,  1,  2,  1, -1,  2,  0,  0,  1,

        3,  3,  2,  2,  1,  1,  0,  0,  1,  1,  0,  0,
        3,  3,  1,  1,  2,  2, -1, -1,  0,  0,  1,  1,
       -1, -1, -1, -1, -1, -1,  1,  0,  0, -1, -1,  1,
        1,  2,  2,  1,  0,  0, -1, -1, -1, -1, -1,  4 }; // last one is MBTS
    
    if (ros > 2) channel += 48; // extended barrel
    int sample = smp[channel];

    return sample;
}

int
TileCablingService::hwid2sample_upgradeABC(int ros, int channel) const {
    
  if (channel >= 0) {
    if (ros > 2) channel += m_maxChannels; // extended barrel
    
    if ((unsigned int) channel < m_ch2towerUpgradeABC.size()) {
      return m_ch2sampleUpgradeABC[channel];
    }
  }

  return -1;
}


void
TileCablingService::fillConnectionTables()
{
  // mapping from module (offline id) to drawer (onile id) 
  // for gap and crack scintillators E1-E4
  // 0 - means "no shift", drawer = module
  // +/-1 - shift by one module: drawer = module +/- 1
  // NC - not connected
  // note: module number in offline identifer is in range 0-63 
  //       and in most of the cases is equal to drawer number  
  const int NC = 65;
  // PB - possibly broken E3, but for the moment assume that this channel is still alive
  const int PB = 0;
  
  int tmp_drawer[512] = {
    //Part EBA
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,PB, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,-1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

    //Part EBC
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,+1, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,+1, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,+1, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC, 0,  0, 0,+1, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,+1, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,  0, 0, 0, 0 
  };

  // mapping from module (offline id) to channel (onile id) 
  // for gap and crack scintillators E1-E4
  // 0 - means "no shift", i.e. E1 in channel 12 E2 in channel 13, E3 in channel 0, E4 in channel 1
  // SH - shift by one channel for E3, i.e. E3 is in channel 1 instead of channel 0
  // NC - not connected
  const int SH = 1;
  int tmp_channel[512] = {
    //Part EBA
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,PB, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,SH, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

    //Part EBC
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,SH, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,SH, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,SH, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC, 0,  0, 0,SH, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,SH, 0,  0, 0,NC,NC,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0,NC,NC,  0, 0,NC, 0,  0, 0, 0, 0,  0, 0, 0, 0 
  };

  //Now in tables like m_[drawer]_table are not shifts - there are real numbers of [drawers]
  for (int i = 0; i < 512; i++) {
   if (tmp_drawer[i] != NC) {
    m_drawer_table[i] = ((i%256)/4) + tmp_drawer[i]; 
    int channel_number;
    switch (i%4) {
     case 0: case 1: channel_number = i%4 + 12; break;
     default: channel_number = i%4 - 2;
    }
    m_channel_table[i] = channel_number + tmp_channel[i];
   } else 
    m_drawer_table[i] = m_channel_table[i] = -1;
  }


  //Printing for test
  /*
  std::cout << " drawer table " << std::endl;

  for (int i = 0; i < 128; ++i) {
    for (int j = 0; j < 4; j++) {
        std::cout << m_drawer_table[i<<2|j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << " channel table " << std::endl;

  for (int i = 0; i < 128; ++i) {
    for (int j = 0; j < 4; j++) {
        std::cout << m_channel_table[i<<2|j] << " ";
    }
    std::cout << std::endl;
  }
  */


  for (int i = 0; i < 512; i++) m_module_table[i] = m_tower_table[i] = -1;

  for (int side = 0; side < 2; side++) {
    for (int module_count = 0; module_count < 64; module_count++ ) {
      for (int tower_count = 0; tower_count < 4; tower_count++) {
	int drawer_number  = m_drawer_table[(side<<8)|(module_count<<2)|(tower_count)];
	int channel_number = m_channel_table[(side<<8)|(module_count<<2)|(tower_count)];
	
        //Here I have drawer number and channel number
	if ((channel_number  == 12)||(channel_number  == 13)||(channel_number  == 0)||(channel_number  == 1)){
          int tower_number;
          switch (tower_count) {
            case 2: tower_number = 13; break;
            case 3: tower_number = 15; break;
            default: tower_number = tower_count + 10;
          }
	  int channel_count;
	  switch (channel_number) {
	    case 12: case 13: channel_count = channel_number - 12; break;
	    default: channel_count = channel_number + 2;
	  }
          //Here I have coordinates in table of drawer and channel
	  int index = (side<<8)|(drawer_number<<2)|(channel_count);
          m_module_table[index] = module_count;
          m_tower_table[index] = tower_number;	

/*	  std::cout << "m_module_table[(side<<8)|(drawer_number<<2)|(channel_count)] " << m_module_table[index] <<
		       "   m_tower_table[(side<<8)|(drawer_number<<2)|(channel_count)] " << m_tower_table[index] << 
		       "   index " << index << std::endl;
*/
	  //std::cout << "module_count " << module_count << " tower_number " << tower_number << " : side " << side << " tower_count " << tower_count << std::endl;
	}
      }
    }
  }
/*
    //Printing for test
  std::cout << " module table " << std::endl;
  for (int i = 0; i < 128; ++i) {
    for (int j = 0; j < 4; j++) {
        std::cout << m_module_table[i<<2|j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << " tower table " << std::endl;
  for (int i = 0; i < 128; ++i) {
    for (int j = 0; j < 4; j++) {
        std::cout << m_tower_table[i<<2|j] << " ";
    }
    std::cout << std::endl;
  }
*/
}

// Layout of gap/crack scintillators:
//
// E1:  cell = 0,  tower = 10,  channel = 12
// E2:  cell = 1,  tower = 11,  channel = 13
// E3:  cell = 2,  tower = 13,  channel =  0
// E4:  cell = 3,  tower = 15,  channel =  1

int
TileCablingService::swid2drawer_gapscin_run2 (int side, int module, int tower) const
{
  // merged E1 cells in EBA and EBC
  // module 07,08 -> 07 (i.e. drawer 07 to drawer 06) --
  // module 24,25 -> 25 (i.e. drawer 23 to drawer 24) ++
  // module 43,44 -> 44 (i.e. drawer 42 to drawer 43) ++
  // module 53,54 -> 53 (i.e. drawer 53 to drawer 52) --
  //
  // merged cells in EBC only
  // module 28,29 -> 28 (i.e. drawer 28 to drawer 27) --
  // module 31,32 -> 31 (i.e. drawer 31 to drawer 30) --
  // module 34,35 -> 35 (i.e. drawer 33 to drawer 34) ++
  // module 37,38 -> 38 (i.e. drawer 36 to drawer 37) ++

  int drawer = module;

  const int towerE1 = (m_cablingType != UpgradeABC) ? 10 : 42;

  if (tower == towerE1) {
    if (drawer == 7 || drawer == 53) --drawer;
    else if (drawer == 42 || drawer == 23) ++drawer;
    else if (side < 0) {
      if (drawer == 28 || drawer == 31) --drawer;
      else if (drawer == 33 || drawer == 36) ++drawer;
    }
  }

  return drawer;
}



int
TileCablingService::swid2drawer_gapscin (int side, int module, int tower) const
{
  side = (side == TileID::POSITIVE) ? 0 : 1;
  int cell;
  switch (tower){
    case 15: cell=3; break;
    case 13: cell=2; break;
    case 11: cell=1; break;
    default: cell=0;
  }
  int p = (side<<8)|(module<<2)|cell;
  return m_drawer_table[p];
}

int
TileCablingService::swid2channel_gapscin (int side, int module, int tower) const
{
  side = (side == TileID::POSITIVE) ? 0 : 1;
  int cell;
  switch (tower){
    case 15: cell=3; break;
    case 13: cell=2; break;
    case 11: cell=1; break;
    default: cell=0;
  }
  int p = (side<<8)|(module<<2)|cell;
  return m_channel_table[p];
}

int
TileCablingService::hwid2module_gapscin (int ros, int drawer, int channel) const
{
  int side = (ros < 4) ? 0 : 1; // set side to 1 for ROS=4 which is EBC (negative side)
  int cell;
  switch (channel) {
    case  1: cell=3; break;
    case  0: cell=2; break;
    case 13: cell=1; break;
    default: cell=0;
  }
  int p = (side<<8)|(drawer<<2)|cell;
  return m_module_table[p];
}

int
TileCablingService::hwid2tower_gapscin (int ros, int drawer, int channel) const
{
  int side = (ros < 4) ? 0 : 1; // set side to 1 for ROS=4 which is EBC (negative side)
  int cell;
  switch (channel) {
    case  1: cell=3; break;
    case  0: cell=2; break;
    case 13: cell=1; break;
    default: cell=0;
  }
  int p = (side<<8)|(drawer<<2)|cell;
  return m_tower_table[p];
}

//
// This method tells us which ITC cells and gap/crack scintillators
// are really connected
//
// It returns false for 36 disconnected crack scintillators, for 2 non-existing D4
// and for 16 special C10 (where 1 out of 2 channels is disconnected)
//
bool
TileCablingService::TileGap_connected(const Identifier & id) const
{
  if (m_tileID->is_tile_gap(id)) { // ITC of gap/crack scin
    switch (m_tileID->sample(id)) {

      case TileID::SAMP_C: // cell C10
        return C10_connected(m_tileID->module(id));

      case TileID::SAMP_D: // cell D4 - return false for EBA15 and EBC18
        return !(EB_special(((m_tileID->side(id)>0)?EBA:EBC),m_tileID->module(id)));

      case TileID::SAMP_E: // E1-E4 - gap/crack scin
        return (m_run2 || swid2channel_gapscin(m_tileID->side(id),
                                               m_tileID->module(id),
                                               m_tileID->tower(id)) != -1);
      default:
        return false; // other samples do not exist 
    }
  }
  return true; // true for all normal cells in barrel and extended barrel
}

bool
TileCablingService::C10_connected(int module)
{
  const bool C = true;
  const bool NC = false;
  
  bool cell_present[64] = { // counting from 1 in comments
    C, C, C, C, C, C, C, C, // 1-8
    C, C, C, C, C, C, C, C, // 9-16
    C, C, C, C, C, C, C, C, // 17-24
    C, C, C, C, C, C, C, C, // 25-32
    C, C, C, C, C, C,NC,NC, // 33-40  39-40 do not exist
   NC,NC, C, C, C, C, C, C, // 41-48  41-42 do not exist
    C, C, C, C, C, C,NC,NC, // 49-56  55-56 do not exist
   NC,NC, C, C, C, C, C, C  // 57-64  57-58 do not exist
  };
  
  return cell_present[module];
}

int
TileCablingService::hwid2pmt ( int ros, int channel ) const
{
    int pm[96] = {
        0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
        0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
        0,  1,  0,  1,  0,  1, -1, -1,  0,  1,  0,  1,
        0,  1,  0,  1,  0,  1,  0, -1,  0,  1,  0,  1,

        0,  0,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
        0,  0,  0,  1,  0,  1, -1, -1,  0,  1,  0,  1,
       -1, -1, -1, -1, -1, -1,  0,  0,  1, -1, -1,  1,
        1,  1,  0,  0,  1,  0, -1, -1, -1, -1, -1, -1 };

    int pmt;
    
    // in gap scintillators there is only one pmt per cell               
    if ( ros > 2 && (channel == m_E1chan || channel == m_E2chan || channel == m_E3chan || channel == m_E4chan) ) {
        pmt = 0;

    } else {

        if (ros > 2) channel += 48; // extended barrel

        pmt = pm[channel];

        // mirroring of odd/even numbers in negative side
        // (central symmetry of neg/pos drawers)
        if ( (pmt != -1 && TileID::NEGATIVE == hwid2side(ros,channel)) || ((channel==0 && ros==((m_testBeam)?LBA:LBC))))
            pmt = 1 - pmt;
    }
    
    return pmt;
}

int
TileCablingService::hwid2pmt_upgradeABC(int ros, int channel ) const {

  int pmt(-1);
  
  if (channel >= 0) {
    if (ros > 2) channel += m_maxChannels; // extended barrel
    
    if ((unsigned int) channel < m_ch2pmtUpgradeABC.size()) {
      pmt = m_ch2pmtUpgradeABC[channel];

      // mirroring of odd/even numbers in negative side
      if ((pmt >= 0 && hwid2side(ros, channel) == TileID::NEGATIVE
           && m_ch2sampleUpgradeABC[channel] != TileID::SAMP_E)
          || (channel == 0 && ros == LBC)) {
        pmt = 1 - pmt; 
      }
    }
  }

  return pmt;
}


int
TileCablingService::swid2ros( int section, int side ) const
{
    // odd crates are in positive side
    int ros = ( TileID::POSITIVE == side ) ? 1 : 2;

    // at the testbeam "A" and "C" sides have opposite meaning
    if (m_testBeam)
      // even crates are in positive side
      ros = ( TileID::POSITIVE == side ) ? 2 : 1;
    
    // another crate for extended barrel and gap section
    if ( TileID::BARREL != section ) ros += 2;
    
    return ros;
}

int
TileCablingService::swid2drawer( int module )
{
    return module;
}

int
TileCablingService::swid2channel( int section, int side, int tower, int sample, int pmt) const
{
    // 3 samplings * 2 pmt * 16 towers in barrel and ext barrel ITC    
    //           and 2 pmt * 16 towers for scintillators from gap section (samp=3)
    // pmt=m_maxChannels for D0/down means that this pmt belongs to another drawer
    // this exeption should be treated correctly in calling routine

    int cell[128] = {
      //-------------barrel--------------//       //-extended barrel-//
      4,  8, 10, 18, 20, 26, 32, 38, 36, 46, -1,  6, 10, 20, 31, 41,  // sampling A
      1,  5,  9, 15, 19, 23, 29, 35, 37, 45, -1,  7, 11, 21, 32, 40,  // sampling A
      
      //-----------barrel------------//  ITC  //-extended barrel-//
      2,  6, 12, 16, 22, 28, 34, 40, 42,  4,  8, 14, 22, 30, 39, -1,  // sampling BC
      3,  7, 11, 17, 21, 27, 33, 39, 47,  5,  9, 15, 23, 35, 36, -1,  // sampling BC
      
      //-------barrel--------//      ITC    /extended barrel/
      0, -1, 14, -1, 24, -1, 44, -1,  2, -1, 16, -1, 38, -1, -1, -1,  // sampling D 
      m_maxChannels, -1, 13, -1, 25, -1, 41, -1,  3, -1, 17, -1, 37, -1, -1, -1,  // sampling D
      
      //-----gap scin-----// 
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, m_E1chan, m_E2chan, -1,  m_E3chan, -1,  m_E4chan,  // gap scin 
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };// pmt=1 doesn't exist
    
    // preserve pmt number for gap scintillators (only pmt=0 exists)
    if ( sample != TileID::SAMP_E || section != TileID::GAPDET ) {
        
        // mirroring of odd/even numbers in negative side
        if (TileID::NEGATIVE == side) pmt = 1 - pmt;
    }
    
    int channel = cell[(sample << 5) | (pmt << 4) | tower];

    return channel;
}

int
TileCablingService::swid2channel_upgradeABC( int section, int side, int tower, int sample, int pmt) const
{

  int channel(-1);

  if (sample == TileID::SAMP_A) {
    int offset = (48 * (1 + (tower % 4)));
    channel = offset + swid2channel( section, side, tower / 4, TileID::SAMP_A, pmt);
  } else if (sample == TileID::SAMP_B) {
    if (((tower - 2) % 4) == 0) {
      int offset = (section == TileID::BARREL) ? 48 : 0;
      channel = offset + swid2channel( section, side, (tower - 2) / 4, TileID::SAMP_B, pmt);
    }
  } else if (sample == TileID::SAMP_D) {
    if ((tower % 8) == 0)
      channel = swid2channel( section, side, tower / 4, TileID::SAMP_D, pmt);
  } else if (sample == TileID::SAMP_X) { // sample C in upgrade
    if (((tower - 2) % 4) == 0)
      channel = 96 + swid2channel( section, side, (tower - 2) / 4, TileID::SAMP_BC, pmt);
  } else {
    channel = swid2channel( section, side, tower / 4, sample, pmt);
  }

  return channel;
}


int
TileCablingService::channel2hole(int ros, int channel)
{
  // negative means not connected !

  int pos[96] = { 1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                  27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34,
                  39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46,
  
                  1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                  13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,
                 -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34,
                  44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46 };

  if (ros > 2) channel += 48; // extended barrel
  int hole = pos[channel];
  // FIXME:: need to do something for special modules EBA15 and EBC18
  // the hole numbers -18 and -19 are not expected numbers for gap scin
  return hole;
}

int
TileCablingService::channel2cellindex(int ros, int channel)
{
  // negative means not connected !

  int ind[96] = {
      0,   1,  2,  2,  1,  3,  4,  4,  3,  5,  5,  6,
      6,   7,  7,  8,  9,  9,  8, 10, 10, 11, 11, 12,
      13, 13, 12, 14, 14, 15, -1, -1, 15, 16, 16, 17,
      18, 18, 17, 19, 19, 20, 21, -1, 20, 22, 22, 21,

      1,   2,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,
      3,   4, 10, 10, 11, 11, -1, -1, 12, 12, 13, 13,
      -1, -1, -1, -1, -1, -1, 14, 15, 15, -1, -1, 14,
      16, 17, 17, 16, 18, 18, -1, -1, -1, -1, -1, -1 
    };
  
  if (ros > 2) channel += 48; // extended barrel
  int index = ind[channel];
  
  return index;
}
//label
//
// create cabling for very few ancillary detectors at the testbeam
// complete mapping will be loaded from database
//

int
TileCablingService::hwid2tbtype ( int drawer )
{
  // input: drawer 
  //
  // #define BEAM_TDC_FRAG 0x000
  // #define BEAM_ADC_FRAG 0x001
  // #define MUON_ADC_FRAG 0x002
  // #define ADDR_ADC_FRAG 0x003
  // #define LASE_PTN_FRAG 0x004
  // #define LASE_ADC_FRAG 0x005
  // #define ADD_FADC_FRAG 0x006
  // #define ECAL_ADC_FRAG 0x007
  //
   static int tbtype[8] = {
     TileTBID::TDC_TYPE,
     TileTBID::ADC_TYPE,
     TileTBID::ADC_TYPE,
     TileTBID::ADC_TYPE,
     TileTBID::INT_TYPE,
     TileTBID::ADC_TYPE,
     TileTBID::FADC_TYPE,
     TileTBID::ADC_TYPE
   };

   if (drawer<8) return tbtype[drawer];
   else return TileTBID::INT_TYPE;
}

int
TileCablingService::hwid2tbmodule ( int drawer, int /* channel */ )
{
  static int tbmodule[8] = {
    0,
    TileTBID::CRACK_WALL,
    TileTBID::BACK_WALL,
    0,
    0,
    0,
    0,
    TileTBID::PHANTOM_CALO
  };
  
  if (drawer<8) return tbmodule[drawer];
  else return 0;
}

int
TileCablingService::hwid2tbchannel ( int /* drawer */, int channel )
{
  return channel;
}
      
int
TileCablingService::tbid2drawer ( int /* tbtype */, int tbmodule )
{
  switch (tbmodule) {
  case TileTBID::CRACK_WALL:   return 1;
  case TileTBID::BACK_WALL:    return 2;
  case TileTBID::PHANTOM_CALO: return 7;
  default: return 0xFF;
  }
}

int
TileCablingService::tbid2channel ( int /* drawer */, int tbchannel )
{
  return tbchannel;
}

//
// Next 6 methods provide FAKE cabling for MBTS 
// MBTS is connected to last (47) channel in every 4th drawer
// used in simulation only
//
bool
TileCablingService::hwid2MBTSconnected(int ros, int drawer, int channel) const
{
  return ((ros > 2) && (channel == 47) && (drawer%4 == 0)); // fake cabling, last channel in every 4th drawer
}

int
TileCablingService::hwid2MBTSphi(int drawer, int /* channel */) const
{
  return (drawer/8);
}

int
TileCablingService::hwid2MBTSeta(int drawer, int /* channel */) const
{
  return ((drawer%8)/4);
}

int
TileCablingService::MBTS2drawer(int phi, int eta) const
{
  return (phi*8 + eta*4); // fake cabling, every 4th drawer 
}

//
// Next 4 methods provide REAL cabling for MBTS 
// used for reconstruction of real data
//
bool
TileCablingService::hwid2MBTSconnected_real(int ros, int drawer, int channel) const
{
  return ((ros > 2) && (channel == 0) && hwid2MBTSeta_real(ros,drawer,channel) != -1);
}

int
TileCablingService::hwid2MBTSphi_real(int /* ros */, int drawer, int /* channel */) const
{
  return (drawer/8); // correct formula even for real MBTS (see MBTS2drawer_real below)
}

int
TileCablingService::hwid2MBTSeta_real(int ros, int drawer, int /* channel */) const
{ 
  int eta[128] = {
    // EBA
    -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1, -1,  1,  0, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,  1,  0,
    -1, -1, -1, -1, -1,  1,  0, -1,
    -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1, -1,  0,  1, -1, -1, -1,
    -1, -1, -1, -1,  0,  1, -1, -1,
    -1, -1, -1,  1,  0, -1, -1, -1,
    // EBC
    -1, -1, -1,  1,  0, -1, -1, -1,
    -1, -1, -1,  1,  0, -1, -1, -1,
    -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1, -1,  1,  0, -1, -1, -1,
    -1, -1, -1,  1,  0, -1, -1, -1,
    -1, -1, -1, -1, -1,  1,  0, -1,
    -1, -1, -1, -1,  1,  0, -1, -1
  };
  // no protection against wrong numbers
  // assume that ROS can be only 3 or 4
  if (ros == EBC) drawer += 64; // negative extended barrel, ROS=4
  return eta[drawer];
}

int
TileCablingService::MBTS2drawer_real(int ros, int phi, int eta) const
{
  int drawer[32] = {
        //part EBA
        3, 12, 23, 30, 35, 43, 52, 60, //E6
        2, 11, 22, 29, 34, 44, 53, 59, //E5
        //part EBC
        4, 12, 19, 27, 36, 44, 54, 61,  //E6
        3, 11, 18, 26, 35, 43, 53, 60 };//E5

  int side = (ros == 3) ? 0 : 1;

  return (drawer[(side<<4)|(eta<<3)|phi]);
}

//
// Next 5 methods provide RUN2 cabling for MBTS 
// used for reconstruction of real data
//
bool
TileCablingService::hwid2MBTSconnected_run2(int ros, int drawer, int channel) const
{
  return ( (ros > 2) && 
           ( (channel ==  4 && hwid2MBTSeta_run2(drawer) == 0) || 
             (channel == 12 && hwid2MBTSeta_run2(drawer) == 1) ) ); 
}

bool
TileCablingService::hwid2MBTSconnected_run2(int ros, int drawer) const
{
  return ((ros > 2) && hwid2MBTSeta_run2(drawer) != -1);
}

bool
TileCablingService::hwid2E4prconnected_run2(int ros, int drawer, int channel) const
{
  return ( (ros > 3) && 
           (channel == 12 && hwid2E4preta_run2(drawer) != -1) ); 
}

bool
TileCablingService::hwid2E4prconnected_run2(int ros, int drawer) const
{
  return ((ros > 3) && hwid2E4preta_run2(drawer) != -1);
}


/*

E4' cabling for run2

modules 29,32,34,37
drawers 28,31,33,36

MBTS cabling for RUN2
(in the table drawer = module-1)

Outer MBTS (eta=1)
=========================
    phi   | connected to
    pos   | module/drawer
==========|==============
 04+12  0 |   08    07
 20+28  2 |   24    23
 36+44  4 |   43    42
 52+60  6 |   54    53


Inner MBTS (eta=0)
======================
  phi  | connected to
  pos  | module/drawer
=======|==============
 04  0 |   57    56
 12  1 |   58    57
 20  2 |   39    38
 28  3 |   40    39
 36  4 |   41    40
 44  5 |   42    41
 52  6 |   55    54
 60  7 |   56    55

*/

int
TileCablingService::hwid2MBTSphi_run2(int drawer) const
{
  int phi[64] = {
    -1, -1, -1, -1, -1, -1, -1,  0,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1,  2,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,  2,  3,
     4,  5,  4, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,  6,  6,  7,
     0,  1, -1, -1, -1, -1, -1, -1
  };

  return phi[drawer];
}

int
TileCablingService::hwid2MBTSeta_run2(int drawer) const
{ 
  int eta[64] = {
    -1, -1, -1, -1, -1, -1, -1,  1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1,  1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,  0,  0,
     0,  0,  1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,  1,  0,  0,
     0,  0, -1, -1, -1, -1, -1, -1
  };

  return eta[drawer];
}

int
TileCablingService::hwid2E4prphi_run2(int drawer) const
{
  int phi[64] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1,  0, -1, -1,  1,
    -1,  2, -1, -1,  3, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1
  };

  return phi[drawer];
}

int
TileCablingService::hwid2E4preta_run2(int drawer) const
{ 
  int eta[64] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1,  2, -1, -1,  2,
    -1,  2, -1, -1,  2, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1
  };

  return eta[drawer];
}

int
TileCablingService::MBTS2drawer_run2(int phi, int eta) const
{
  int drawer[24] = {
       56, 57, 38, 39, 40, 41, 54, 55,  //E6 (inner)
        7,  7, 23, 23, 42, 42, 53, 53,  //E5 (outer)
       28, 31, 33, 36, 28, 31, 33, 36 };//E4' 

  return (drawer[(eta<<3)|phi]);
}

int
TileCablingService::MBTS2channel_run2(int eta) const
{
  return ((eta==0)?4:12); // both MBTS and E4'
}

unsigned int 
TileCablingService::getNChanPerCell(const Identifier& cell_id) const 
{ 
  return m_tileID->is_tile_gapscin(cell_id) ? 1:2; 
}

void 
TileCablingService::fillH2SIdCache(void) {

  if (m_isCacheFilled) return;
  
  unsigned int nChannels = (TileCalibUtils::MAX_ROS - 1) * TileCalibUtils::MAX_DRAWER * m_maxChannels;

  m_ch2cell.resize(nChannels);
  m_ch2pmt.resize(nChannels);
  m_ch2index.resize(nChannels);

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {  
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int channel = 0; channel < m_maxChannels; ++channel) {
	int index;
        int pmt;
        Identifier cell_id = h2s_cell_id_index_find(ros, drawer, channel,index, pmt);
	int ind = cacheIndex(ros, drawer, channel);
	m_ch2cell[ind] = cell_id;
        m_ch2pmt[ind] = pmt;
        m_ch2index[ind] = index;
      }
    }
  }
  m_isCacheFilled = true;
}

Identifier
TileCablingService::h2s_cell_id_index ( const HWIdentifier & hwid, int & index, int & pmt ) const
{
  int ros      = m_tileHWID->ros     (hwid);
  int drawer   = m_tileHWID->drawer  (hwid);
  int channel  = m_tileHWID->channel (hwid);  

  if (m_isCacheFilled && ros != TileHWID::BEAM_ROS) {
    return h2s_cell_id_index_from_cache(ros, drawer, channel, index, pmt);
  } else {
    return h2s_cell_id_index_find(ros, drawer, channel, index, pmt);
  }
}

Identifier
TileCablingService::h2s_cell_id_index (int ros, int drawer, int channel, int& index, int& pmt ) const  
{

  if (m_isCacheFilled && ros != TileHWID::BEAM_ROS) {
    return h2s_cell_id_index_from_cache(ros, drawer, channel, index, pmt);
  } else {
    return h2s_cell_id_index_find(ros, drawer, channel, index, pmt);
  }
}


Identifier
TileCablingService::h2s_cell_id_index_from_cache (int ros, int drawer, int channel, 
						  int & index, int & pmt ) const
{
  int ind = cacheIndex(ros, drawer, channel);
  index = m_ch2index[ind];
  pmt = m_ch2pmt[ind];
  return m_ch2cell[ind];  
}

bool TileCablingService::is_MBTS_merged_run2(int module) const {
  return m_MBTSmergedRun2[module];
}

int TileCablingService::E1_merged_with_run2(int ros, int module) const {
  return m_E1mergedRun2[(ros-3)*64+module];
}
