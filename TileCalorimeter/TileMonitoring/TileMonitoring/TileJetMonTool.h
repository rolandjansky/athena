/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileJetMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Tomas Davidek (Tomas.Davidek@cern.ch)
//	     
//
// ********************************************************************
#ifndef TILEMONITORING_TILEJETMONTOOL_H
#define TILEMONITORING_TILEJETMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class ITileBadChanTool;
class Jet;

// Define flags & constants not defined anywhere else
#define FLAG_OF2 0x2   // OF2 has been applied
#define NPART 4 // number of Tilecal partitions
#define NMOD 64 // number of Tilecal modules per partition
#define NPMT 48 // number of Tilecal channels in a module

/** @class TileJetMonTool
 *  @brief Class for Tile Jet based monitoring
 */

class TileJetMonTool: public TileFatherMonTool {
  private:
    float m_jetPtMin;
    float m_jetEtaMax;
    std::string m_jetCollectionName;
    std::string m_THistSvcStreamName;
    float m_energyChanMin;
    float m_energyChanMax;

    std::string m_name;

    std::string m_partname[NPART];

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //Tile bad channel tool

    // this variable holds the histograms and is initialized in bookTimeHistograms,
    // called from initialize
    std::vector<TH2F*> m_TileChanTime[NPART];
    //  std::vector<TProfile*> m_TileChanTime[NPART];

  public:
    TileJetMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    //  : TileFatherMonTool(type, name, parent);

    ~TileJetMonTool();

    StatusCode initialize();
    //  StatusCode execute();
    StatusCode fillHistograms();
    StatusCode finalize();
    StatusCode bookTimeHistograms();
    void clearTimeHistograms();
    StatusCode fillTimeHistograms(const Jet& jet, uint32_t LumiBlock);

    bool isGoodChannel(int part, int mod, int pmt, uint32_t bad, unsigned int qbit, float energy);
    bool isGoodJet(const Jet &jet);

    /* copy & paste from JetD3PDMaker/src/JetCaloUtilsFillerTool.h. It is defined
     as private, so we cannot directly include that file
     */
    typedef enum {
      LooseBad, MediumBad, TightBad
    } BadJetCategory;

    bool isBad(BadJetCategory criteria, double quality, double NegE, double emf, double hecf,
        double time, double fmax, double eta, double chf, double HecQ);

};

#endif
