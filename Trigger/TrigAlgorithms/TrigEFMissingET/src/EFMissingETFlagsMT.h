/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFLAGSMT_H
#define TRIGEFMISSINGET_EFMISSINGETFLAGSMT_H



#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigEFMissingET/IMissingETTool.h"
#include <vector>

/**
  $class EFMissingETFlagsMT
  \author Gabriel Gallardo
  \data Mar 15 2019
  It should make checks about fake sources of MET (for the cell algorithm) and update the event status flag.
 **/

//Based on code EFMissingETFlags by Diego Casadei, May 15, 2008.

class EFMissingETFlagsMT : public extends<AthAlgTool, IMissingETTool> 
{
 public:

  EFMissingETFlagsMT(const std::string& type,
		   const std::string& name,
		   const IInterface* parent);

  virtual ~EFMissingETFlagsMT() = default;

  virtual StatusCode update(xAOD::TrigMissingET *met,
                  TrigEFMissingEtHelper *metHelper, const EventContext& /*ctx*/) const override;

 private:

  const static std::vector<float> s_dummyMax;
  const static std::vector<float> s_dummyMin;
  const static std::vector<float> s_dummyRatio;


  /** configurables */
  /// NB: Trivial limits are set here.
  ///     Look at TrigEFMissingETConfig.py for actual initialization
  ///     24 used for number of vector elements as nCaloSamplings==24
  Gaudi::Property<float> m_MaxEMfraction {this, "MaxEMfraction",  1.0, "max reasonable energy ratio in EM samplings"};
  Gaudi::Property<float> m_MinEMfraction {this, "MinEMfraction",  0.0, "min reasonable energy ratio in EM samplings"};

  Gaudi::Property<float> m_MaxTileGapEratio {this, "MaxTileGapEratio",  1.0, "max energy in TileGap scint. compared to global SumE"};

  Gaudi::Property<float> m_MaxSumEratioInEMB {this, "MaxSumEratioInEMB",  1.0, "max reasonable component energy ratio in EMB"};
  Gaudi::Property<float> m_MaxSumEratioInEME {this, "MaxSumEratioInEME",  1.0, "max reasonable component energy ratio in EME"};
  Gaudi::Property<float> m_MaxSumEratioInHEC {this, "MaxSumEratioInHEC",  1.0, "max reasonable component energy ratio in HEC"};
  Gaudi::Property<float> m_MaxSumEratioInTileBar {this, "MaxSumEratioInTileBar",  1.0, "max reasonable component energy ratio in Tile Barrel"};
  Gaudi::Property<float> m_MaxSumEratioInTileGap {this, "MaxSumEratioInTileGap",  1.0, "max reasonable component energy ratio in TileGap scintillators"};
  Gaudi::Property<float> m_MaxSumEratioInTileExt {this, "MaxSumEratioInTileExt",  1.0, "max reasonable component energy ratio in Tile Extended-Barrel"};
  Gaudi::Property<float> m_MaxSumEratioInFCal {this, "MaxSumEratioInFCal",  1.0, "max reasonable component energy ratio in FCal"};

  Gaudi::Property<float> m_MinSumEratioInEMB {this, "MinSumEratioInEMB",  0.0, "min reasonable component energy ratio in EMB"};
  Gaudi::Property<float> m_MinSumEratioInEME {this, "MinSumEratioInEME",  0.0, "min reasonable component energy ratio in EME"};
  Gaudi::Property<float> m_MinSumEratioInHEC {this, "MinSumEratioInHEC",  0.0, "min reasonable component energy ratio in HEC"};
  Gaudi::Property<float> m_MinSumEratioInTileBar {this, "MinSumEratioInTileBar",  0.0, "min reasonable component energy ratio in Tile Barrel"};
  Gaudi::Property<float> m_MinSumEratioInTileGap {this, "MinSumEratioInTileGap",  0.0, "min reasonable component energy ratio in TileGap scintillators"};
  Gaudi::Property<float> m_MinSumEratioInTileExt {this, "MinSumEratioInTileExt",  0.0, "min reasonable component energy ratio in Tile Extended-Barrel"};
  Gaudi::Property<float> m_MinSumEratioInFCal {this, "MinSumEratioInFCal",  0.0, "min reasonable component energy ratio in FCal"};

  Gaudi::Property<float> m_GlobMaxNoisyEnergyRatio {this, "GlobMaxNoisyEnergyRatio",  0.9, "max reasonable BadSET/SET"};

  Gaudi::Property<float> m_GlobMaxMEtSumEtRatio {this, "GlobMaxMEtSumEtRatio",  0.7, "max reasonable |MET/SumET|"};

  Gaudi::Property<std::vector<float> > m_MaxCompE {"MaxCompE", s_dummyMax , "(vector) max reasonable component energy"};
  Gaudi::Property<std::vector<float> > m_MinCompE {"MinCompE", s_dummyMin , "(vector) min reasonable component energy"};

  Gaudi::Property<std::vector<float> > m_MaxCellE {"MaxCellE", s_dummyMax , "(vector) max reasonable cell energy for each component"};
  Gaudi::Property<std::vector<float> > m_MinCellE {"MinCellE", s_dummyMin , "(vector) min reasonable cell energy for each component"};

  Gaudi::Property<std::vector<float> > m_MaxCellTime {"MaxCellTime", s_dummyMax , "(vector) max reasonable cell time for each component"};
  Gaudi::Property<std::vector<float> > m_MinCellTime {"MinCellTime", s_dummyMin , "(vector) min reasonable cell time for each component"};

  Gaudi::Property<std::vector<float> > m_CompMaxNoisyEnergyRatio {"CompMaxNoisyEnergyRatio", s_dummyRatio , "(vector) max reasonable BadSET/SET for each component"};

  Gaudi::Property<std::vector<float> > m_WorstCellQlty {"WorstCellQuality", s_dummyMax , "(vector) max reasonable CaloCell::quality() for each component"};

  Gaudi::Property<std::vector<float> > m_CompMaxMEtSumEtRatio {"CompMaxMEtSumEtRatio", s_dummyRatio , "(vector) max reasonable |MET/SumET| for each component"};

};

#endif // TRIGEFMISSINGET_EFMISSINGETFLAGSMT
