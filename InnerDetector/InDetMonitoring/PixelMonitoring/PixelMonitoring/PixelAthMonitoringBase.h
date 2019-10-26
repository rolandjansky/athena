/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHMONITORINGBASE_H
#define PIXELATHMONITORINGBASE_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "InDetIdentifier/PixelID.h"

class PixelID;

class PixLayers {
 public:
  enum PixLayersID {kECA = 0, kECC, kB0, kB1, kB2, kIBL, kDBMA, kDBMC, COUNT};
};
const std::string pixLayersLabel[PixLayers::COUNT] = {"ECA", "ECC", "B0", "B1", "B2", "IBL", "DBMA", "DBMC"};
const float inv_nmod_per_layer[PixLayers::COUNT] = {1./144., 1./144., 1./286., 1./494., 1./676., 1./448., 1./12., 1./12.};
const int clusterToTMinCut[PixLayers::COUNT] = {15, 15, 15, 15, 15, 4, 4, 4};

namespace PixMon {
  const unsigned int kNumLayersDisk{3};
  const unsigned int kNumLayersDBM{3};
  const unsigned int kNumStavesIBL{14};
  const unsigned int kNumStavesL0{22};
  const unsigned int kNumStavesL1{38};
  const unsigned int kNumStavesL2{52};
  
  const unsigned int kNumFEsIBL{32};
  const unsigned int kNumModulesIBL{20};
  const unsigned int kNumModulesIBL2D{12};
  const unsigned int kNumModulesIBL3D{8};
  const unsigned int kNumModulesBarrel{13};
  const unsigned int kNumModulesDisk{48};
  const unsigned int kNumModulesDBM{4};
  const unsigned int kNumPP0sEC{24};
}

class PixelAthMonitoringBase : public virtual AthMonitorAlgorithm {

 public:

  StatusCode fill2DProfLayer( const std::string& prof2Dname, const std::string& layer, Identifier& id, const PixelID* pid, float weight=1.0, bool copy2DFEval=false ) const;
  StatusCode fill1DProfLumiLayers( std::string prof1Dname, int lb, float* weights ) const;
  StatusCode fill1DProfLayers( std::string name, float* values ) const;
  StatusCode fillPP0Histos( std::string name, int(&D_A)[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk], int(&D_C)[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk], int(&B0)[PixMon::kNumStavesL0][PixMon::kNumModulesBarrel], int(&B1)[PixMon::kNumStavesL1][PixMon::kNumModulesBarrel], int(&B2)[PixMon::kNumStavesL2][PixMon::kNumModulesBarrel], int(&IBL)[PixMon::kNumStavesIBL][PixMon::kNumFEsIBL] ) const;


  int getPixLayersID(int ec, int ld) const;
  std::string addTxt(std::string txt, bool ontrack) const;
  void getPhiEtaMod(const PixelID* pid, Identifier& id, int& phiMod, int& etaMod, bool& copyFE) const;
  bool isHitOnTrack(Identifier id, std::vector<Identifier> const &RDOIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const &ClusterIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const &ClusterIDs, double& cosalpha) const;

  /// helper class to accumulate points to fill a 2D plot with
  struct VecAccumulator2DMap {
    std::unordered_map<int, std::vector<int>> m_pm;
    std::unordered_map<int, std::vector<int>> m_em;
    std::unordered_map<int, std::vector<float>> m_val;
    bool m_copy2DFEval;
    std::string m_prof2Dname;

    VecAccumulator2DMap( const std::string& prof2Dname, bool copy2DFEval = false ) 
      : m_copy2DFEval(copy2DFEval)
      , m_prof2Dname(prof2Dname) {}
    
    void add( const int layer, const Identifier& id,
	      const PixelID* pid, float value=1.0 );
  };
  void fill2DProfLayerAccum( const VecAccumulator2DMap& accumulator ) const;
  
};

#endif
