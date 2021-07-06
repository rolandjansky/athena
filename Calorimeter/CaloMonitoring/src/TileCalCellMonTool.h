//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONTOOL_H
#define TILEMONTOOL_H

#include "CaloMonToolBase.h"
#include "GaudiKernel/ToolHandle.h"


#include "AthenaMonitoring/IDQFilterTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell;
class TileID;

class TH1F;
class TH1D;
class TH2F;
class TProfile2D;
class TProfile;


class TileCalCellMonTool : public CaloMonToolBase {
 
 public:

  TileCalCellMonTool(const std::string & type, const std::string& name, 
		 const IInterface* parent);
  virtual ~TileCalCellMonTool();
  
  virtual StatusCode initialize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode procHistograms() override;
  virtual StatusCode fillHistograms() override;


private:
  const std::string m_tilePath{"/CaloMonitoring/TileCellMon_NoTrigSel/General"};

  //Private methods:
  void initParam();
  void getRMSdevNoise(TProfile *t1, TProfile *t2, TH1F *t3);

  //Job Properties and other private variables
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerName { this, "CaloCellContainer", "AllCalo", "SG key of the input cell container" };

  bool m_useTwoGaus;
  SG::ReadCondHandleKey<CaloNoise> m_noiseKey
    { this, "NoiseKey", "totalNoise", "SG key for noise" };

  // tile energy threshold
  float m_tileThreshold;

  const TileID* m_tile_id;
  
 //   Tile histograms parameters 
  std::vector<double> m_b_Ncells;

  std::vector<double> m_b_CellsE;

  std::vector<double> m_b_CellsEta;
  std::vector<double> m_b_CellsEtaTile;

  std::vector<double> m_b_CellsPhi;

  std::vector<double> m_b_CellsTower;
  std::vector<double> m_b_CellsModule;

  std::vector<double> m_b_CellsEtaPhi;

  // Tile histograms
  TH1F* m_h_CellsNoiseE=nullptr;  // E/DBnoise distribution for all not bad tile cells in sample <3

  // for tile cells with energy > m_tileThreshold 
  TH1F* m_h_Ncells=nullptr;
  TH1F* m_h_CellsE=nullptr;  
  TH2F* m_h_CellsEtaPhi=nullptr;
  TProfile*  m_h_CellsEta=nullptr;
  TProfile*  m_h_CellsPhi=nullptr;
  TProfile* m_h_CellsModuleS1=nullptr;
  TProfile* m_h_CellsModuleS2=nullptr;
  TProfile* m_h_CellsModuleS3=nullptr;
  TProfile* m_h_CellsModuleS4=nullptr;
  TProfile* m_h_CellsTower=nullptr;

  // for not bad tile cells with |E/DBnoise| >4.0 in sample<3
  TH2F* m_h_CellsNoiseEtaPhi=nullptr;
  TH2F* m_h_CellsNoiseEtaPhiA=nullptr;
  TH2F* m_h_CellsNoiseEtaPhiBC=nullptr;
  TH2F* m_h_CellsNoiseEtaPhiD=nullptr;
  TH1F* m_h_CellsNoiseHash=nullptr;

  // for all not bad tile cells in sample <3; h_CellsRMSdivDBnoiseEta(Phi) are derived  from temporary plots h_CellsNoiseEta(phi) and h_CellsRMSEta(Phi) 
  TProfile*  m_h_CellsNoiseEta=nullptr;
  TProfile*  m_h_CellsNoisePhi=nullptr;
  TProfile*  m_h_CellsRMSPhi=nullptr;
  TProfile*  m_h_CellsRMSEta=nullptr;
  TH1F * m_h_CellsRMSdivDBnoiseEta=nullptr;
  TH1F * m_h_CellsRMSdivDBnoisePhi=nullptr;


};

#endif 

