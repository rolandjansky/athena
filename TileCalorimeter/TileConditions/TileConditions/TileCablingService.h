/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECABLINGSERVICE_H
#define TILECONDITIONS_TILECABLINGSERVICE_H

// This class provides conversion between logical and hardware ID
// Implemented as a singleton

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/TileID_Exception.h" 
#include "CaloIdentifier/TileID.h" 
#include "TileIdentifier/TileHWID.h" 
#include "CaloIdentifier/TileTBID.h" 
#include "CaloIdentifier/CaloLVL1_ID.h" 

#include "AthenaKernel/MsgStreamMember.h"

#include <vector>

class TileCablingService
{
  friend class TileInfoLoader;
  friend class TileDetectorTool;
  friend class TileDetectorFactory;
  friend class TileTBFactory;
  friend class TileCablingSvc;
  friend class TileROD_Decoder;
  
public:

    /** get pointer to service instance*/
    static TileCablingService * instance(bool del);
    static TileCablingService * getInstance() {return instance(false); }

    /** delete service instance */
    static TileCablingService * deleteInstance() {return instance(true); }
    
    // Conversion between TileID and Trigger Tower ID

    Identifier          cell2tt_id              ( const Identifier & id ) const;
    Identifier          pmt2tt_id               ( const Identifier & id ) const;
    Identifier          cell2mt_id              ( const Identifier & id ) const;
    Identifier          pmt2mt_id               ( const Identifier & id ) const;

    Identifier          drawer2MBTS_id          ( const HWIdentifier & id ) const;

    // Conversion between TileID and TileHWID

//  Identifier          h2s_section_id          ( const HWIdentifier & id );
//  Identifier          h2s_region_id           ( const HWIdentifier & id );
//  Identifier          h2s_module_id           ( const HWIdentifier & id );
//  Identifier          h2s_tower_id            ( const HWIdentifier & id );
    Identifier          h2s_cell_id_index       ( const HWIdentifier & id, int & index, int & pmt ) const;
    Identifier          h2s_cell_id_index       ( int ros, int drawer, int channel, int & index, int & pmt ) const;
    Identifier          h2s_cell_id             ( const HWIdentifier & id ) const;
    Identifier          h2s_pmt_id              ( const HWIdentifier & id ) const;
    Identifier          h2s_adc_id              ( const HWIdentifier & id ) const;
                                                                  
//  HWIdentifier        s2h_ros_id              ( const Identifier & id );
    HWIdentifier        s2h_drawer_id           ( const Identifier & id ) const;
    HWIdentifier        s2h_channel_id          ( const Identifier & id ) const;
    HWIdentifier        s2h_adc_id              ( const Identifier & id ) const;

    int                 frag                    ( const HWIdentifier & id ) const;
    int                 frag                    ( const Identifier & id )   const;

    int                 frag2channels           ( const HWIdentifier & id, std::vector<HWIdentifier> & ids) const;
    int                 frag2channels           ( int frag, std::vector<HWIdentifier> & ids) const;

    // hole (pmt number in HV system, range 1-48, negative means not connected)
    int                 barrel_chan2hole        ( int channel ) const { return channel2hole(0,channel); }
    int                 extbar_chan2hole        ( int channel ) const { return channel2hole(2,channel); }
    static int          channel2hole            ( int ros, int channel );
    // cell index is the number from 0 to 23 (to count all cells in one drawer
    // index = -1 for non-connected channels
    static int          channel2cellindex       ( int ros, int channel );

    bool                TileGap_connected       ( const Identifier & id) const;
    static bool         C10_connected           ( int module );

    unsigned int        getNChanPerCell         (const Identifier& cell_id) const; 

    void                fillH2SIdCache          (void);

    enum TileCablingType { TestBeam = -1,
                           OldSim    = 0,
                           CrackOnly = 1,
                           MBTSOnly  = 2,
                           CrackAndMBTS = 3,
                           RUN2Cabling = 4,
                           UpgradeA = 10,
                           UpgradeBC = 11,
                           UpgradeABC = 12,
                           UnknownCabling };

    bool is_MBTS_merged_run2(int module) const;
    int E1_merged_with_run2(int ros, int module) const;

    int getMaxChannels(void) const { return m_maxChannels; };
    int getMaxGains(void) const { return m_maxGains; };


    /// Log a message using the Athena controlled logging system
    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

protected:
    
    /** Default constructor protected */                
    TileCablingService() ;
    

    /** Destructor protected */
    virtual ~TileCablingService() ;
     

private:

    static int          hwid2section            ( int ros, int channel );
           int          hwid2side               ( int ros, int channel ) const;
    static int          hwid2module             ( int drawer );
           int          hwid2tower              ( int ros, int channel ) const;

    static int          hwid2sample             ( int ros, int channel );
           int          hwid2pmt                ( int ros, int channel ) const;

           int          swid2ros                ( int section, int side ) const;
    static int          swid2drawer             ( int module );
           int          swid2channel            ( int section, int side, 
                                                  int tower, int sample, 
                                                  int pmt ) const;

           int          hwid2tower_upgradeABC   ( int ros, int channel ) const;
           int          hwid2sample_upgradeABC  ( int ros, int channel ) const;
           int          hwid2pmt_upgradeABC     ( int ros, int channel ) const;

           int          swid2channel_upgradeABC ( int section, int side, 
                                                  int tower, int sample, 
                                                  int pmt ) const;

    static int          hwid2tbtype             ( int drawer );
    static int          hwid2tbmodule           ( int drawer, int channel );
    static int          hwid2tbchannel          ( int drawer, int channel );

    static int          tbid2drawer             ( int tbtype, int tbmodule );
    static int          tbid2channel            ( int drawer, int tbchannel );

           bool         hwid2MBTSconnected      ( int ros, int drawer, int channel) const;
           int          hwid2MBTSphi            ( int drawer, int channel) const;
           int          hwid2MBTSeta            ( int drawer, int channel) const;
           int          MBTS2drawer             ( int phi, int eta) const;

           bool         hwid2MBTSconnected_real ( int ros, int drawer, int channel) const;
           int          hwid2MBTSphi_real       ( int ros, int drawer, int channel) const;
           int          hwid2MBTSeta_real       ( int ros, int drawer, int channel) const;
           int          MBTS2drawer_real        ( int side, int phi, int eta) const;

           bool         hwid2MBTSconnected_run2 (int ros, int drawer, int channel) const;
           bool         hwid2MBTSconnected_run2 (int ros, int drawer) const;
           int          hwid2MBTSphi_run2       (int drawer) const;
           int          hwid2MBTSeta_run2       (int drawer) const;
           int          MBTS2drawer_run2        (int phi, int eta) const;
           int          MBTS2channel_run2       (int eta) const;

           bool         hwid2E4prconnected_run2 (int ros, int drawer, int channel) const;
           bool         hwid2E4prconnected_run2 (int ros, int drawer) const;
           int          hwid2E4prphi_run2       (int drawer) const;
           int          hwid2E4preta_run2       (int drawer) const;

           int          swid2drawer_gapscin     ( int side, int module, int tower) const;
           int          swid2channel_gapscin    ( int side, int module, int tower) const;
           int          hwid2module_gapscin     ( int ros,  int drawer, int channel) const;
           int          hwid2tower_gapscin      ( int ros,  int drawer, int channel) const;

           int          swid2drawer_gapscin_run2( int side, int module, int tower) const;

    inline bool         isTileITCGapCrack       (int channel) const {return (channel < 6 || channel == 12 || channel == 13);}
    inline bool         isTileGapCrack          (int channel) const {return (channel < 2 || channel == 12 || channel == 13);}
    inline bool         D4                      (int channel) const { return ( (channel == 2) || (channel == 3) );}

    Identifier m_invalid_id;
    HWIdentifier m_invalid_hwid;

    const TileID*      m_tileID;
    const TileHWID*    m_tileHWID;
    const CaloLVL1_ID* m_TT_ID;
    const TileTBID*    m_tileTBID;
  
    void setCaloLVL1 (const CaloLVL1_ID* tt_ID) {m_TT_ID    = tt_ID;}      
    void setTileID   (const TileID* tileID)     {m_tileID   = tileID;}      
    void setTileHWID (const TileHWID* tileHWID) {m_tileHWID = tileHWID;}
    void setTileTBID (const TileTBID* tileTBID) {m_tileTBID = tileTBID;}

    bool m_testBeam;
    void setTestBeam (bool testBeam);
  
    bool m_connected[5][64];
    void setConnected(int section, int side, int modMin, int modMax);
    void setConnected(int ros, int draMin, int draMax);

    enum Partition { Ancillary = 0,
                     LBA = 1,
                     LBC = 2,
                     EBA = 3,
                     EBC = 4 };
  
    int m_EBAspec;
    int m_EBCspec;
    int m_E1chan;
    int m_E2chan;
    int m_E3chan;
    int m_E4chan;
    int m_E3special;
    int m_E4special;
    int m_MBTSchan;
    inline bool EB_special(int ros, int drawer) const { 
      return ( (ros == EBA && drawer == m_EBAspec ) || (ros == EBC && drawer == m_EBCspec ) ); 
    }
    inline void EB_special_move_channel(int ros, int drawer, int &channel) const {
      if (EB_special(ros,drawer)) {
        // swap special and normal E3,E4 channels
        // this will help to use the same arrays of connected channels everywhere
        if      (channel == m_E3special) channel = m_E3chan;
        else if (channel == m_E4special) channel = m_E4chan;
        else if (channel == m_E3chan) channel = m_E3special;
        else if (channel == m_E4chan) channel = m_E4special;
      }
    }
    
    TileCablingType m_cablingType;
    void setCablingType (TileCablingType type);
  
    int m_drawer_table[512];
    int m_channel_table[512];
    int m_module_table[512];
    int m_tower_table[512];

    void fillConnectionTables();

    bool m_isCacheFilled;
    std::vector<Identifier> m_ch2cell;
    std::vector<int> m_ch2pmt;
    std::vector<int> m_ch2index;

   Identifier h2s_cell_id_index_find ( int ros, int drawer, int channel, int & index, int & pmt ) const;
   Identifier h2s_cell_id_index_from_cache (int ros, int drawer, int channel, int & index, int & pmt ) const;

   // returns index used for caching (for ros != TileHWID::BEAM_ROS) and calculated as 
   //    channel * 64(max number of drawers) * 4(max number of ros) + drawer * 4(max number of ros) + (ros - 1)
   inline int cacheIndex(int ros, int drawer, int channel) const {return ((channel << 8) + (drawer << 2) + (ros - 1));};

   void setConnected (int ros, int drawer, bool yes) { m_connected[ros][drawer] = yes; }

   bool isChannelFromOppositeSide(int channel) const {return channel == m_maxChannels;};

   std::vector<bool> m_MBTSmergedRun2;
   std::vector<int> m_E1mergedRun2;

   std::vector<int> m_ch2pmtUpgradeABC;
   std::vector<int> m_ch2sampleUpgradeABC;
   std::vector<int> m_ch2towerUpgradeABC;

   bool m_run2;

   int m_maxChannels;
   int m_maxGains;

   mutable Athena::MsgStreamMember m_msg;

public:

    inline const TileID*       getTileID()         const { return m_tileID; }
    inline const TileHWID*     getTileHWID()       const { return m_tileHWID; }
    inline const TileTBID*     getTileTBID()       const { return m_tileTBID; }
    inline const CaloLVL1_ID*  getCaloLVL1_ID()    const { return m_TT_ID; }
    inline bool                getTestBeam()       const { return m_testBeam; }
    inline bool  connected (int ros, int drawer)   const { return m_connected[ros][drawer]; }
    inline int                 getCablingType()    const { return m_cablingType; }

};

#endif // TILECONDITIONS_TILECABLINGSERVICE_H

