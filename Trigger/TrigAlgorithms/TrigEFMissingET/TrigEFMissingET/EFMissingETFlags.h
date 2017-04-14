/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFLAGS_H
#define TRIGEFMISSINGET_EFMISSINGETFLAGS_H

/********************************************************************

NAME:     EFMissingETFlags.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Diego Casadei
CREATED:  May 15, 2008

PURPOSE:  Performs checks with global and auxiliary information
and updates the global event status flag

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TH2F.h"
#include <vector>

/**
  $class EFMissingETFlags
  It should make checks about fake sources of MET and update the event status flag.
 **/

class EFMissingETFlags : public EFMissingETBaseTool
{
 public:

  EFMissingETFlags(const std::string& type,
		   const std::string& name,
		   const IInterface* parent);

  ~EFMissingETFlags();

  virtual StatusCode start();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);

 private:

  /** configurables */
  float m_MaxEMfraction;  //<! max reasonable energy in EM samplings compared to global SumE
  float m_MinEMfraction;  //<! min reasonable energy in EM samplings compared to global SumE

  float m_MaxTileGapEratio; //<! max energy in TileGap scint. compared to global SumE

  float m_MaxSumEratioInEMB;     //<! max reasonable component energy ratio in EMB
  float m_MaxSumEratioInEME;     //<! max reasonable component energy ratio in EME
  float m_MaxSumEratioInHEC;     //<! max reasonable component energy ratio in HEC
  float m_MaxSumEratioInTileBar; //<! max reasonable component energy ratio in Tile Barrel
  float m_MaxSumEratioInTileGap; //<! max reasonable component energy ratio in TileGap scintillators
  float m_MaxSumEratioInTileExt; //<! max reasonable component energy ratio in Tile Extended-Barrel
  float m_MaxSumEratioInFCal;    //<! max reasonable component energy ratio in FCal

  float m_MinSumEratioInEMB;     //<! min reasonable component energy ratio in EMB
  float m_MinSumEratioInEME;     //<! min reasonable component energy ratio in EME
  float m_MinSumEratioInHEC;     //<! min reasonable component energy ratio in HEC
  float m_MinSumEratioInTileBar; //<! min reasonable component energy ratio in Tile Barrel
  float m_MinSumEratioInTileGap; //<! min reasonable component energy ratio in TileGap scintillators
  float m_MinSumEratioInTileExt; //<! min reasonable component energy ratio in Tile Extended-Barrel
  float m_MinSumEratioInFCal;    //<! min reasonable component energy ratio in FCal

  float m_GlobMaxMEtSumEtRatio;  //<! max reasonable |MET/SumET|
  float m_GlobMaxNoisyEnergyRatio; //<! max reasonable BadSET/SET

  std::vector<float> m_MaxCompE;   //<! max reasonable component energy
  std::vector<float> m_MinCompE;   //<! min reasonable component energy
  std::vector<float> m_MaxCellE;   //<! max reasonable cell energy for each component
  std::vector<float> m_MinCellE;   //<! min reasonable cell energy for each component
  std::vector<float> m_MaxCellTime;   //<! max reasonable cell time for each component
  std::vector<float> m_MinCellTime;   //<! min reasonable cell time for each component
  std::vector<float> m_CompMaxNoisyEnergyRatio; //<! max reasonable BadSET/SET for each component
  std::vector<float> m_WorstCellQlty; //<! max reasonable CaloCell::quality() for each component
  std::vector<float> m_CompMaxMEtSumEtRatio; //<! max reasonable |MET/SumET|

  TH2F* m_hCompFlags; //<! monitoring histograms for component status bits
};

#endif // TRIGEFMISSINGET_EFMISSINGETFLAGS
