/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileTBDump.h
// Author   : Alexander Solodkov
// Created  : May, 2004
//
// DESCRIPTION
// 
//    To dump contents of TestBeam ByteStream file
//    Old TileDump program converter to Athena Algorithm
//
// Properties (JobOption Parameters):
//    drawerList - vector of frag IDs present in data (hex numbers as strings)
//    drawerType - type for every frag from drawerList
//                 1=B+, 2=B-, 3=EB+, 4=EB-
//    these parameters are needed only for 2003 data
//    (when 0x102 and 0x202 were ext.barrel modules)
//    correct values for 2004 are set by default
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

/* @class TileTBDump
 @brief Class to dump TileCal bytestream fragments in a formatted way.
 */

#ifndef TileTBDump_H
#define TileTBDump_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Tile includes
#include "TileByteStream/TileRawChannel2Bytes2.h" 
#include "TileByteStream/TileRawChannel2Bytes4.h" 
#include "TileByteStream/TileRawChannel2Bytes5.h" 


class IROBDataProviderSvc;
class TileDigits;
class TileCablingService;
class TileCondToolOfcCool;
class TileCondToolTiming;
class TileCondToolEmscale;

#include <vector>

class TileTBDump: public AthAlgorithm {
  public:
    //Constructor
    TileTBDump(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileTBDump();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    int m_statFrag5[200];

    ServiceHandle<IROBDataProviderSvc> m_RobSvc;

    const TileCablingService* m_cabling;

    std::vector<std::string> m_drawerList; // list of frag IDs in correct order
    std::vector<int> m_drawerType; // type of every drawer 1-4: B+, B-, EB+, EB-
    std::map<unsigned int, unsigned int, std::less<unsigned int> > m_drawerMap; // map for frag IDs -> index
    typedef std::map<unsigned int, unsigned int, std::less<unsigned int> >::iterator drawerMap_iterator;

    ToolHandle<TileCondToolTiming> m_tileToolTiming;
    ToolHandle<TileCondToolOfcCool> m_tileCondToolOfcCool;
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;

    bool m_dumpHeader;
    bool m_dumpData;
    bool m_dumpStatus;
    bool m_dumpOnce;
    bool m_dumpUnknown;
    bool m_showUnknown;
    bool m_v3Format;
    bool m_frag5found;
    unsigned int m_sizeOverhead;
    int m_unit;

    int m_bc_time_seconds;
    int m_bc_time_nanoseconds;
    int m_global_id;
    int m_run_type;
    int m_run_no;
    int m_lumi_block;
    int m_lvl1_id;
    int m_bc_id;
    int m_lvl1_trigger_type;
    int m_nlvl1_trigger_info;
 
    /* the ROD data sub-fragment */
    typedef struct T_RodDataFrag {
        unsigned int size;
        unsigned int id;
        unsigned int data[1];
    } T_RodDataFrag;

#ifndef MAX_CHAN_SAMP
#define MAX_CHAN_SAMP 36
#endif

    typedef struct T_TileDigiChannel {
        int chan;
        unsigned int head;
        unsigned int first;
        unsigned int crc;
        unsigned int id;
        unsigned int gain;
        unsigned int flag;
        unsigned int bcid;
        unsigned int err;
        unsigned int par;
        unsigned short sample[MAX_CHAN_SAMP];
    } T_TileDigiChannel;

    typedef struct T_TileRecoQuality {
        uint16_t dspbcid;
        uint16_t global_crc;
        uint16_t bcid;
        uint16_t memory;
        uint16_t Sstrobe;
        uint16_t Dstrobe;
        uint16_t headformat;
        uint16_t headparity;
        uint16_t sampleformat;
        uint16_t sampleparity;
        uint16_t fe_chip_mask;
        uint16_t rod_chip_mask;
    } T_TileRecoQuality;

    typedef struct T_TileRecoChannel {
        int chan;
        unsigned int word;
        unsigned short gain;
        unsigned short amp;
        unsigned short time;
        unsigned short quality;
        double d_amp;
        double d_time;
        double d_quality;
    } T_TileRecoChannel;

    typedef struct T_TileRawComp { // Baxo
        unsigned int words[4];
        bool verif;
        int chan;
        unsigned short gain;
        unsigned short samples[7];
    } T_TileRawComp;

    typedef struct T_TileRecoCalib { // Baxo
        int chan;
        unsigned int word;
        unsigned short gain;
        unsigned short amp;
        unsigned short time;
        unsigned short bad;
        unsigned short quality;
        double d_amp;
        double d_time;
        double d_quality;
    } T_TileRecoCalib;


/*    void dump_TMDB(uint32_t subdet_id, const uint32_t* roddata, unsigned int rodsize
                               , uint32_t version, int verbosity, int robsource_id);*/

    void dump_data(const uint32_t* data, unsigned int size, unsigned int version, int verbosity);

    void dump_digi(unsigned int subdet_id, const uint32_t* data, unsigned int size, unsigned int version, int verbosity, unsigned int robsourceid, const EventContext& ctx);

    void dump_it(unsigned int nw, unsigned int* data);

    void find_frag(const uint32_t* rod, unsigned int size, unsigned int version, int verbosity, T_RodDataFrag** frag, int* nfrag);

    int tile_unpack_raw_comp(T_RodDataFrag* frag, T_TileRawComp* rawcomp, int nchannel_max
                             , unsigned int version, int verbosity, int* ngain, int* nchannel, int* nsample); // Baxo

    int tile_unpack_digi(T_RodDataFrag* frag, T_TileDigiChannel* channel, int nchannel_max
                         , unsigned int version, int verbosity, int* ngain, int* nchannel, int* nsample);

    int tile_unpack_reco(T_RodDataFrag* frag, T_TileRecoChannel* channel, int nchannel_max
                         , unsigned int version, int verbosity, int* ngain, int* nchannel);

    int tile_unpack_reco_calib(T_RodDataFrag* frag, T_TileRecoCalib* recocalib, int nchannel_max
                               , unsigned int version
                               , unsigned int unit
                               , int verbosity, int* ngain, int* nchannel); // Baxo

    int tile_unpack_quality(T_RodDataFrag* frag, T_TileRecoQuality & DQword);

    unsigned int tile_check_parity(unsigned int* frame, int length);

    unsigned int tile_check_startbit(unsigned int* frame, int length, unsigned int startbit);

    unsigned int tile_check_CRC(unsigned int* frame, int framelen, int delta);

    void tile_min_max(unsigned short* frame, int frame_length, unsigned short* smin, unsigned short* smax);

    TileRawChannel2Bytes2 m_rc2bytes2;
    TileRawChannel2Bytes4 m_rc2bytes4;
    TileRawChannel2Bytes5 m_rc2bytes5;

};

#endif
