/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//TileTBBeamMonTool.h

#ifndef TILETBBEAMMONTOOL_H
#define TILETBBEAMMONTOOL_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// // Athena includes
// #include "AthenaBaseComps/AthAlgorithm.h"

// Tile includes
// #include "TileConditions/TileCablingService.h"
// #include "TileIdentifier/TileRawChannelUnit.h"
#include "TileMonitoring/TileFatherMonTool.h"

#include "TFile.h"
#include "TMatrixT.h"
#include "TTree.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class ITHistSvc;
class TileBeamInfoProvider;
class TileDQstatus;
class TileID;
class TileHWID;
class TileInfo;

class TileTBBeamMonTool: public TileFatherMonTool {
  public:
    //Constructor
    TileTBBeamMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    //Destructor 
    virtual ~TileTBBeamMonTool();

    virtual StatusCode initialize();

    //virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:
    void initFirstEvent(void);
    StatusCode storeBeamElements();

    //Histogramming
    TH2F* m_BC1Profile = nullptr;
    TH2F* m_BC2Profile = nullptr;
    TH2F* m_BC1ProfileSum = nullptr;
    TH2F* m_BC2ProfileSum = nullptr;
    TH1F* m_BC1Xhist = nullptr;
    TH1F* m_BC1Yhist = nullptr;
    TH1F* m_BC2Xhist = nullptr;
    TH1F* m_BC2Yhist = nullptr;
    TH2F* m_CellEnergyImpactX = nullptr;
    TH2F* m_CellEnergyImpactY = nullptr;
    TH2F* m_TotalEnergyImpactX = nullptr;
    TH2F* m_TotalEnergyImpactY = nullptr;
    TH2F* m_ImpactProfile = nullptr;
    TH2F* m_Cher1Energy = nullptr;
    TH2F* m_Cher2Energy = nullptr;
    TH2F* m_Cher3Energy = nullptr;
    TH2F* m_CherCompare = nullptr;
    TH2F* m_ScinCalEnergy = nullptr;
    TH1F* m_S1hist = nullptr;
    TH1F* m_S2hist = nullptr;
    TH1F* m_S3hist = nullptr;
    TH1F* m_Cher1hist = nullptr;
    TH1F* m_Cher2hist = nullptr;
    TH1F* m_Cher3hist = nullptr;
    TH1F* m_MuonEnergy = nullptr;
    TH1F* m_PMT1 = nullptr;
    TH1F* m_PMT2 = nullptr;
    TH1F* m_PMT3 = nullptr;
    TH1F* m_PMT4 = nullptr;
    TH1F* m_PMT5 = nullptr;
    TH1F* m_PMT6 = nullptr;
    TH1F* m_PMT7 = nullptr;
    TH1F* m_PMT8 = nullptr;
    TH1F* m_PMT9 = nullptr;
    TH1F* m_PMT10 = nullptr;
    TH1F* m_PMT11 = nullptr;
    TH1F* m_PMT12 = nullptr;
    TH2F* m_PMTHitMap = nullptr;

    bool m_isFirstEvent;

    // TDC/BEAM Items
    int m_s1cou;
    int m_s2cou;
    int m_s3cou;
    int m_cher1;
    int m_cher2;
    int m_cher3;
    int m_muTag;
    int m_muHalo;
    int m_muVeto;

    float m_muBack[16]; // MUON/MuBack

    int m_btdc[16];

    //PMT counters
    int m_pmt1cou;
    int m_pmt2cou;
    int m_pmt3cou;
    int m_pmt4cou;
    int m_pmt5cou;
    int m_pmt6cou;
    int m_pmt7cou;
    int m_pmt8cou;
    int m_pmt9cou;
    int m_pmt10cou;
    int m_pmt11cou;
    int m_pmt12cou;

    float m_xCha1;
    float m_yCha1;
    float m_xCha2;
    float m_yCha2;
    float m_xCha1_sum;
    float m_yCha1_sum;
    float m_xCha2_sum;
    float m_yCha2_sum;
    float m_xImp;
    float m_yImp;
    //float m_deltaX1;
    //float m_deltaY1;
    //float m_deltaX2;
    //float m_deltaY2;

    //Calibration coords
    float m_horizontal_slope1;
    float m_horizontal_offset1;
    float m_vertical_slope1;
    float m_vertical_offset1;
    float m_horizontal_slope2;
    float m_horizontal_offset2;
    float m_vertical_slope2;
    float m_vertical_offset2;
    float m_beamBC1Z;
    float m_beamBC2Z;

    //Cell energy and calorimeter energy
    double m_total_energy;
    double m_cell_energy;
    double m_cut_energy_min;
    double m_cut_energy_max;
    double m_total_muon_energy;

    //Total event energy
    std::string m_cellContainer; 

    //run number
    //int m_runNumber;

    //Beam element container
    std::string m_beamElemContainer;

    
};

#endif
