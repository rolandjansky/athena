/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 TileInfoDump
 -----------------------------------------
 Test algorithm to dump out calibration constants from the TileInfoSvc


 2007-05-14      nils.gollub@cern.ch
 2008-06-06      lukas.pribyl@cern.ch
 ***************************************************************************/

#ifndef TILECALIBALGS_TILEINFODUMP_H
#define TILECALIBALGS_TILEINFODUMP_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"

class ITileBadChanTool;
class TileCondToolEmscale;
class TileCondToolIntegrator;
class TileCondToolTiming;
class TileCondToolPulseShape;
class TileCondToolNoiseSample;
class TileCondToolNoiseRawChn;
class TileCondToolOfcCool;
class TileCondToolMuID;

class TileCondToolAutoCr;
class ITHistSvc;
class TileHWID;
class TileID;
class CaloDetDescrManager;
class TH2F;

/**
 ** Algorithm to dump out calibration constants from the TileInfoSvc
 **/

class TileInfoDump: public AthAlgorithm {
  public:
    TileInfoDump(const std::string& name, ISvcLocator* pSvcLocator);
    ~TileInfoDump();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    bool m_isOnline;

    void printEmscale();
    bool m_printEmscale;

    void printEmscaleOnl();
    bool m_printEmscaleOnl;

    void printLaser();
    bool m_printLaser;

    void printIntegrator();
    bool m_printIntegrator;

    void printPedestals();
    bool m_printPedestals;

    void printTimingCorrections();
    bool m_printTimingCorrections;

    void printPulseShapes();
    bool m_printPulseShapes;
    float m_printPulseShapesStep;

    void printBadChannels();
    bool m_printBadChannels;

    void printBadCells();
    bool m_printBadCells;

    void printOfcs();
    bool m_printOfcsCool;

    void printMuID();
    bool m_printMuID;

    void print1gNoise();
    bool m_print1gNoise;

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,
      "EventInfo", "EventInfo", "EventInfo key"};

    //=== Non-property members
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    ToolHandle<TileCondToolIntegrator> m_tileToolIntegrator;
    ToolHandle<TileCondToolTiming> m_tileToolTiming;
    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape;
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample;
    ToolHandle<TileCondToolAutoCr> m_tileToolAutoCr;
    ToolHandle<TileCondToolNoiseRawChn> m_tileToolNoiseRawChn;
    ToolHandle<TileCondToolOfcCool> m_tileToolOfcCool;
    ToolHandle<TileCondToolMuID> m_tileToolMuID;

    //=== a handle on the Hist/TTree registration service 
    ITHistSvc* m_thistSvc;

    const TileHWID* m_tileHWID;
    const TileID* m_tileID;
    const CaloDetDescrManager* m_caloMgr;

    TH2F* m_h_badCellA;
    TH2F* m_h_badCellBC;
    TH2F* m_h_badCellD;
    TH2F* m_h_badCellGap;
    TH2F* m_h_badCell;

    unsigned int m_printOfcRos;
    unsigned int m_printOfcDrawer;
    unsigned int m_printOfcChannel;
};

#endif 
