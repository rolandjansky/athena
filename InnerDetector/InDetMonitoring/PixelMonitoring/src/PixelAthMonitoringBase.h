/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PIXELATHMONITORINGBASE_H
#define PIXELATHMONITORINGBASE_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "InDetIdentifier/PixelID.h"

class PixelID;

class PixLayers {
public:
  enum PixLayersID {
    kECA = 0, kECC, kB0, kB1, kB2, kIBL2D, NFEI3LAYERS=kIBL2D, kIBL=kIBL2D, kIBL3D, NBASELAYERS=kIBL3D, COUNT
  };
};
class DataReadErrors {
public:
  enum DataReadErrorsID {
    ContainerInvalid = 0, CollectionInvalid, EmptyContainer, COUNT
  };
};
const std::string pixLayersLabel[PixLayers::COUNT] = {
  "ECA", "ECC", "BLayer", "Layer1", "Layer2", "IBL2D", "IBL3D"
};
const std::string pixBaseLayersLabel[PixLayers::NBASELAYERS] = {
  "ECA", "ECC", "BLayer", "Layer1", "Layer2", "IBL"
};
const float inv_nmod_per_layer[PixLayers::COUNT] = {
  1. / 144., 1. / 144., 1. / 286., 1. / 494., 1. / 676., 1. / 336., 1. / 112.
};
const int clusterToTMinCut[PixLayers::COUNT] = {
  15, 15, 15, 15, 15, 4, 4
};

namespace PixMon {
  const unsigned int kNumLayersDisk {
    3
  };
  const unsigned int kNumStavesIBL {
    14
  };
  const unsigned int kNumStavesL0 {
    22
  };
  const unsigned int kNumStavesL1 {
    38
  };
  const unsigned int kNumStavesL2 {
    52
  };
  const unsigned int kNumFEsIBL {
    32
  };
  const unsigned int kNumModulesBarrel {
    13
  };
  const unsigned int kNumModulesDisk {
    48
  };
  const unsigned int kNumPP0sEC {
    24
  };
}

const std::vector<float> iblFEetaEdges = { 0.5894, 1.0531, 1.3851, 1.6499, 1.8559, 2.0336, 2.1805, 2.3132,
					   2.4268, 2.5324, 2.6249, 2.7116, 2.7906, 2.8638, 2.9321, 2.9953 };
const std::vector<float> iblFEphiLoEdges = { -3.0551, -2.6063, -2.1575, -1.7087, -1.2599, -0.8111, -0.3623,
					     0.0865, 0.5353, 0.9841, 1.4329, 1.8817, 2.3305, 2.7793 };
const std::vector<float> iblFEphiUpEdges = { -3.0215, -2.5727, -2.1239, -1.6751, -1.2263, -0.7775, -0.3287,
					      0.1201, 0.5689, 1.0177, 1.4665, 1.9153, 2.3641, 2.8129 };

class PixelAthMonitoringBase: public virtual AthMonitorAlgorithm {
public:
  void fill1DProfLumiLayers(const std::string& prof1Dname, int lb, float* weights,
                            int nlayers = PixLayers::COUNT) const;
  void fill2DProfLumiLayers(const std::string& prof2Dname, int lb, float (*weights)[PixLayers::COUNT],
                            const int* nCategories) const;

  int getPixLayersID(int ec, int ld) const;
  bool isIBL2D(int hashID) const;
  bool isIBL3D(int hashID) const;
  int getNumberOfFEs(int pixlayer, int etaMod) const;
  void getPhiEtaMod(const PixelID* pid, Identifier& id, int& phiMod, int& etaMod, bool& copyFE) const;
  bool isHitOnTrack(Identifier id, std::vector<Identifier> const& RDOIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const& ClusterIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const& ClusterIDs,
                        double& cosalpha) const;
  std::pair<int, int> getIBLFEIdxsfromTrackEtaPhi(float eta, float phi) const;
  /// helper class to accumulate points to fill a 2D plot with
  struct VecAccumulator2DMap {
    std::unordered_map<int, std::vector<int> > m_pm;
    std::unordered_map<int, std::vector<int> > m_em;
    std::unordered_map<int, std::vector<float> > m_val;
    bool m_copy2DFEval;
    std::string m_prof2Dname;

    VecAccumulator2DMap(const std::string& prof2Dname, bool copy2DFEval = false)
      : m_copy2DFEval(copy2DFEval)
      , m_prof2Dname(prof2Dname) {}

    void add(const int layer, const Identifier& id,
             const PixelID* pid, float value = 1.0);
    void add(const int layer, const Identifier& id,
             const PixelID* pid, int iFE, float value);
  };
  void fill2DProfLayerAccum(const VecAccumulator2DMap& accumulator) const;

  struct AccumulatorArrays {
    int DA[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk];
    int DC[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk];
    int B0[PixMon::kNumStavesL0][PixMon::kNumModulesBarrel];
    int B1[PixMon::kNumStavesL1][PixMon::kNumModulesBarrel];
    int B2[PixMon::kNumStavesL2][PixMon::kNumModulesBarrel];
    int IBL[PixMon::kNumStavesIBL][PixMon::kNumFEsIBL];
  };
  void fillFromArrays(const std::string& namePP0, AccumulatorArrays& pixarrays,
                      const std::string& name2DMap = "") const;
};

#endif
