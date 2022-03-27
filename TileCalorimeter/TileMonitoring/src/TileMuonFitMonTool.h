/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileMuonFitMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//           Francesc Vives (fvives@ifae.es)
//
// ********************************************************************
#ifndef TILEMUONFITMONTOOL_H
#define TILEMUONFITMONTOOL_H

#include "TileFatherMonTool.h"

class TileCosmicMuon;

/** @class TileMuonFitMonTool
 *  @brief Class for Tile MuonFit based monitoring
 */

class TileMuonFitMonTool: public TileFatherMonTool {

  public:

    TileMuonFitMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileMuonFitMonTool();

    StatusCode initialize();

    //pure virtual methods
    //  StatusCode bookHists();
    //  StatusCode fillHists();
    //  StatusCode finalHists();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistograms();
    StatusCode procHistograms();
    StatusCode fillHistograms();

    StatusCode bookHistTrig(int trig);

    void cleanHistVec(); //necessary to avoid problems at the eb, lumi blocks boundaries
    //  void fillDensity(float x0, float z0, float phi, float theta, float path); //necessary to avoid problems at the eb, lumi blocks boundaries

  private:

    bool m_isFirstEv;
    bool m_trigok;
    bool m_useLVL1;

    std::vector<std::string> m_l1triggers;

    int getTMFpart(const TileCosmicMuon* tmf);
    int32_t m_tileMuonFitTrig;
    //double m_Threshold;
    std::string m_muonfitContName;
    std::string m_cellContainerKey;

    std::vector<TH1F*> m_tileMuonFit;
    std::vector<TH2F*> m_tileMuonFitPos;
    std::vector<TH1F*> m_tileMuonFitDir;
    std::vector<TH1F*> m_tileMuonFitDir2;
    std::vector<TH1F*> m_tileMuonFitTime;
    std::vector<TH2F*> m_tileMuonFitDirPos;
    std::vector<TH1F*> m_tileMuonFitEnergy;
    //std::vector<TH1F*> m_tileMuonFitEnergyRebin;
    std::vector<TH1F*> m_tileMuonFitPath;
    std::vector<TH1F*> m_tileMuonFitNCells;
    std::vector<TH1F*> m_tileMuonFitEnergyDensity;
    std::vector<TProfile*> m_tileMuonFitTimePos;
    std::vector<TProfile*> m_tileMuonFitTimePart;
    TH2F* m_tileMuonFitTimePartLVL1[NumPart];
};

#endif
