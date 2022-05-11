/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTowerMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//	     
//
// ********************************************************************
#ifndef TILETOWERMONTOOL_H
#define TILETOWERMONTOOL_H

#include "TileFatherMonTool.h"

/** @class TileTowerMonTool
 *  @brief Class for TileTower based monitoring
 */

class TileTowerMonTool: public TileFatherMonTool {

  public:

    TileTowerMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTowerMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistTrigPart(int trig, int part);
    StatusCode bookHistTrig(int trig);

    void cleanHistVec();

  private:

    int32_t m_TileTowerTrig;
    double m_Threshold;
    std::string m_towersContName;

    std::vector<TH2F*> m_TileTowerEtaPhi;
    //std::vector<TH1F*> m_TileTowerEta;
    //std::vector<TH1F*> m_TileTowerPhi;
    std::vector<TH2F*> m_TileTowerEtaPhiDiff;
    std::vector<TH1F*> m_TileTowerEneDiff;
    std::vector<TH1F*> m_TileAllTowerEnergy[NPartHisto];
    //std::vector<TH1F*> m_TilenTowers[NPartHisto];
    //std::vector<TH1F*> m_TileTowernCells[NPartHisto];
    //std::vector<TH1F*> m_TileTowerEnergy[NPartHisto];
    //std::vector<TH1F*> m_TileTowerEnergyRebin[NPartHisto];
    //std::vector<TProfile*> m_TileTowerEneTim;
    //std::vector<TProfile*> m_TileTowerEneLumi;
    std::vector<TH1F*> m_TileTowerEt[NPartHisto];
    //  TH2D* m_test;
};

#endif
