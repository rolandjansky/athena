/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileL2Builder.h
// Author   : Aranzazu Ruiz
// Created  : February 2009
//
// DESCRIPTION
//    TileL2 from TileRawChannel.
//
// BUGS:
//  
// History:
//  
//****************************************************************************

#ifndef TILEL2ALGS_TILEL2BUILDER_H
#define TILEL2ALGS_TILEL2BUILDER_H

// Tile includes
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileIdentifier/TileFragHash.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// C++ STL includes
#include <string>
#include <vector>

// forward declarations
class TileHWID;
class TileCondToolEmscale;
class TileCondToolNoiseRawChn;
class ITileBadChanTool;

class TileL2Builder: public AthAlgTool {

  public:

    TileL2Builder(const std::string& type
                  , const std::string& name
                  , const IInterface* parent);

    virtual ~TileL2Builder();

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual StatusCode process(int fragmin, int fragmax, TileL2Container *l2Container);

    /** Return colllection ID for a given index  */
    inline int indexToId(int i) {
      return m_hashFunc.identifier(i);
    }

    inline int idToIndex(int id) {
      return m_hashFunc(id);
    }

    // AlgTool InterfaceID
    static const InterfaceID& interfaceID();

    /**
     * function which does bad channel masking:
     * if one channel of a cell is bad, energy from second channel is taken
     * if both channels are bad, zero energy is assigned to both channels
     * @param partition partition (1=LBA, 2=LBC, 3=EBA, 4=EBC)
     * @param E         energy for all channels - on return it will contain corrected values
     * @param gain      gain for all channels - on return it will contain corrected values
     * @param bad       bad flag for all channels
     */
    void MaskBad(int partition, float* E, int* gain, bool* bad);

    /**
     * Muon tagging function for LB superdrawers as processed at the ROD DSPs.
     * @param EMeV      Calibrated energy (in MeV) for all channels
     * @param EtaMuons  Muons eta coordinate
     * @param EMuons0   Energy (in MeV) deposited by the muons in A cells
     * @param EMuons1   Energy (in MeV) deposited by the muons in BC cells
     * @param EMuons2   Energy (in MeV) deposited by the muons in D cells
     * @param qf        Quality factor
     * @param extraWord Encoded 32-bit words with muon info
     */
    void MTagLB(int partition
                , int drawer
                , float* EMeV
                , std::vector<float> &EtaMuons
                , std::vector<float> &EMuons0
                , std::vector<float> &EMuons1
                , std::vector<float> &EMuons2
                , std::vector<unsigned int> &qf
                , std::vector<unsigned int> &extraWord);

    inline void MTagLB(int partition
                      , int drawer
                      , float* EMeV
                      , int* gain
                      , bool* bad
                      , std::vector<float> &EtaMuons
                      , std::vector<float> &EMuons0
                      , std::vector<float> &EMuons1
                      , std::vector<float> &EMuons2
                      , std::vector<unsigned int> &qf
                      , std::vector<unsigned int> &extraWord) {
      MaskBad(partition, EMeV, gain, bad);
      MTagLB(partition, drawer, EMeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
    }

    /**
     * Muon tagging function for EB superdrawers as processed at the ROD DSPs.
     * @param EMeV      Calibrated energy (in MeV) for all channels
     * @param EtaMuons  Muons eta coordinate
     * @param EMuons0   Energy (in MeV) deposited by the muons in A cells
     * @param EMuons1   Energy (in MeV) deposited by the muons in B cells
     * @param EMuons2   Energy (in MeV) deposited by the muons in D cells
     * @param qf        Quality factor
     * @param extraWord Encoded 32-bit words with muon info
     */
    void MTagEB(int partition
                , int drawer
                , float* EMeV
                , std::vector<float> &EtaMuons
                , std::vector<float> &EMuons0
                , std::vector<float> &EMuons1
                , std::vector<float> &EMuons2
                , std::vector<unsigned int> &qf
                , std::vector<unsigned int> &extraWord);

    inline void MTagEB(int partition
                      , int drawer
                      , float* EMeV
                      , int* gain
                      , bool* bad
                      , std::vector<float> &EtaMuons
                      , std::vector<float> &EMuons0
                      , std::vector<float> &EMuons1
                      , std::vector<float> &EMuons2
                      , std::vector<unsigned int> &qf
                      , std::vector<unsigned int> &extraWord) {
      MaskBad(partition, EMeV, gain, bad);
      MTagEB(partition, drawer, EMeV, EtaMuons, EMuons0, EMuons1, EMuons2, qf, extraWord);
    }

    /**
     * SumE function for one superdrawer as processed at the ROD DSPs.
     * @param partition partition (1=LBA, 2=LBC, 3=EBA, 4=EBC)
     * @param drawer    drawer number (0-63)
     * @param unit      energy units (0=ADC counts, 3=MeV) for proper rounding a-la DSP
     * @param E         energy for all channels in some units
     * @param gain      gain for all channels
     * @param bad       bad flag for all channels
     * @param sumE      sumEt, sumEz and sumE for the drawer in one vector
     */
    void SumE(int partition, int drawer, float * E, std::vector<float> &sumE);
    void SumE(int partition, int drawer, int unit, float* E, int* gain, std::vector<float> &sumE);
    inline void SumE(int partition
                    , int drawer
                    , int unit
                    , float* E
                    , int* gain
                    , bool* bad
                    , std::vector<float> &sumE) {
      MaskBad(partition, E, gain, bad);
      SumE(partition, drawer, unit, E, gain, sumE);
    }

  protected:

    // properties
    /** TileRawChannelContainer in event store */
    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                        "TileRawChannelCnt",
                                                                        "Input Tile raw channel container key"};


    float m_noiseThreshold;
    int m_noiseType;

    /** Pointer to TileHWID */
    const TileHWID* m_tileHWID;

    /** Handle to Tile calibration tool */
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<TileCondToolNoiseRawChn> m_tileToolNoiseRawChn; //!< tool which provided noise values

    /** Handle to Tile bad channel tool */
    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

    TileFragHash m_hashFunc;

    float m_sinTh[4][48];
    float m_sinThRound[4][48];
    float m_cosTh[4][48];
    float m_cosThRound[4][48];
    bool m_connected[4][48];
    int m_channelPairs[4][48];
};

#endif // TILEL2ALGS_TILEL2BUILDER_H
