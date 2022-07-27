/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PIXELATHMONITORINGBASE_H
#define PIXELATHMONITORINGBASE_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"

#include "InDetIdentifier/PixelID.h"

#include <tuple>

class PixelID;

class PixLayers {
public:
  enum PixLayersID {
    kECA = 0, kECC, kBLayer, kLayer1, kLayer2, kIBL2D, NFEI3LAYERS=kIBL2D, kIBL=kIBL2D, kIBL3D, NBASELAYERS=kIBL3D, COUNT
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
  const int pixPhiSteps[PixLayers::NBASELAYERS] = {kNumModulesDisk, kNumModulesDisk, kNumStavesL0, kNumStavesL1, kNumStavesL2, kNumStavesIBL};
  const int pixEtaSteps[PixLayers::NBASELAYERS] = {kNumLayersDisk, kNumLayersDisk, kNumModulesBarrel, kNumModulesBarrel, kNumModulesBarrel, kNumFEsIBL};
}

const std::vector<float> iblFEetaEdges = { 0.5894, 1.0531, 1.3851, 1.6499, 1.8559, 2.0336, 2.1805, 2.3132,
					   2.4268, 2.5324, 2.6249, 2.7116, 2.7906, 2.8638, 2.9321, 2.9953 };
const std::vector<float> iblFEphiLoEdges = { -3.0551, -2.6063, -2.1575, -1.7087, -1.2599, -0.8111, -0.3623,
					     0.0865, 0.5353, 0.9841, 1.4329, 1.8817, 2.3305, 2.7793 };
const std::vector<float> iblFEphiUpEdges = { -3.0215, -2.5727, -2.1239, -1.6751, -1.2263, -0.7775, -0.3287,
					      0.1201, 0.5689, 1.0177, 1.4665, 1.9153, 2.3641, 2.8129 };

class PixelAthMonitoringBase: public virtual AthMonitorAlgorithm {
public:
  virtual StatusCode initialize() override;

  void fill1DProfLumiLayers(const std::string& prof1Dname, int lb, float* weights,
                            int nlayers = PixLayers::COUNT) const;
  void fill2DProfLumiLayers(const std::string& prof2Dname, int lb, float (*weights)[PixLayers::COUNT],
                            const int* nCategories) const;

  int getPixLayersID(int ec, int ld) const;
  bool isIBL2D(int hashID) const;
  bool isIBL3D(int hashID) const;
  int getNumberOfFEs(int pixlayer, int etaMod) const;
  void getPhiEtaMod(Identifier& id, int& phiMod, int& etaMod, bool& copyFE) const;
  bool isHitOnTrack(Identifier id, std::vector<Identifier> const& RDOIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const& ClusterIDs) const;
  bool isClusterOnTrack(Identifier id, std::vector<std::pair<Identifier, double> > const& ClusterIDs,
                        double& cosalpha) const;
  /// helper class to accumulate points to fill a 2D plot with
  struct VecAccumulator2DMap {
    std::unordered_map<int, std::vector<int> > m_pm;
    std::unordered_map<int, std::vector<int> > m_em;
    std::unordered_map<int, std::vector<float> > m_val;
    const PixelAthMonitoringBase& m_host;
    std::string m_prof2Dname;
    bool m_copy2DFEval;
    
    VecAccumulator2DMap(const PixelAthMonitoringBase& host, const std::string& prof2Dname,  bool copy2DFEval = false) 
    : m_host(host), 
      m_prof2Dname(prof2Dname),
      m_copy2DFEval(copy2DFEval) {}
    void add(const int layer, const Identifier& id, float value = 1.0);
    void add(const int layer, const Identifier& id,
             int iFE, float value);
  };
  void fill2DProfLayerAccum(const VecAccumulator2DMap& accumulator) const;
  void fill1DModProfAccum(const VecAccumulator2DMap& accumulator, int lumiblock) const;

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

protected:
  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool {
    this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"
  };
  ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
  {
    this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager"
  };
  const PixelID* m_pixelid{};
  std::vector<int> m_modData[PixLayers::NBASELAYERS];

  /** @brief Optional read handle to get status data to test whether a pixel detector element is good.
   * If set to e.g. PixelDetectorElementStatus the event data will be used instead of the pixel conditions summary tool.
   */
  SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_pixelDetElStatus
     {this, "PixelDetElStatus", "", "Key of SiDetectorElementStatus for Pixel"};

  /** @brief Optional read handle to get status data to test whether a pixel detector element is active.
   * The optional event data is used to test whether a pixel detector element is active but not necessarily good.
   * If set to e.g. PixelDetectorElementStatusActiveOnly the event data will be used instead of the pixel conditions summary tool.
   */
  SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_pixelDetElStatusActiveOnly
     {this, "PixelDetElStatusActiveOnly", "", "Key of SiDetectorElementStatus for Pixel which reflects only whether modules or chips are active rather than delivering good data"};

  SG::ReadHandle<InDet::SiDetectorElementStatus> getPixelDetElStatus(const SG::ReadHandleKey<InDet::SiDetectorElementStatus> &key, const EventContext& ctx) const {
     SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus;
     if (!key.empty()) {
        pixelDetElStatus = SG::ReadHandle<InDet::SiDetectorElementStatus>(key, ctx);
        if (!pixelDetElStatus.isValid()) {
           std::stringstream msg;
           msg << "Failed to get " << key.key() << " from StoreGate in " << name();
           throw std::runtime_error(msg.str());
        }
     }
     return pixelDetElStatus;
  }
  bool isActive(const  InDet::SiDetectorElementStatus *element_status,
                const IdentifierHash &module_hash) const {
     bool ret  { element_status ? element_status->isGood(module_hash) : m_pixelCondSummaryTool->isActive(module_hash)  };
     VALIDATE_STATUS_ARRAY(element_status, element_status->isGood(module_hash), m_pixelCondSummaryTool->isActive(module_hash)  );
     return ret;
  }
  bool isGood(const InDet::SiDetectorElementStatus *element_status,
              const IdentifierHash &module_hash) const {
     bool ret  ( element_status ? element_status->isGood(module_hash) : m_pixelCondSummaryTool->isGood(module_hash));
     VALIDATE_STATUS_ARRAY(element_status, element_status->isGood(module_hash), m_pixelCondSummaryTool->isGood(module_hash)  );
     return ret;
  }
  std::tuple<bool,bool> isChipGood(const IdentifierHash &module_hash,
                                   unsigned int chip_i) const {
     bool is_active=false;
     bool is_good=false;
     Identifier pixelID = m_pixelReadout->getPixelIdfromHash(module_hash, chip_i, 1, 1);
     if (pixelID.is_valid()) {
        is_active = m_pixelCondSummaryTool->isActive(module_hash,pixelID);
        if (is_active) {
           is_good = m_pixelCondSummaryTool->isGood(module_hash, pixelID);
        }
     }
     return std::make_tuple(is_active,is_good);
  }
  bool isChipActive(const IdentifierHash &module_hash,
                    unsigned int chip_i) const {
     bool is_active=false;
     Identifier pixelID = m_pixelReadout->getPixelIdfromHash(module_hash, chip_i, 1, 1);
     if (pixelID.is_valid()) {
        is_active = m_pixelCondSummaryTool->isActive(module_hash,pixelID);
     }
     return is_active;
  }
  std::tuple<bool,bool> isChipGood(const InDet::SiDetectorElementStatus &element_active,
                                   const InDet::SiDetectorElementStatus &element_status,
                                   const IdentifierHash &module_hash,
                                   unsigned int chip_i) const {
     return std::make_tuple(element_active.isChipGood(module_hash, chip_i), element_status.isChipGood(module_hash, chip_i) );
  }
  std::tuple<bool,bool> isChipGood(const InDet::SiDetectorElementStatus *element_active,
                                   const InDet::SiDetectorElementStatus *element_status,
                                   const IdentifierHash &module_hash,
                                   unsigned int chip_i) const {
     std::tuple<bool,bool> ret( element_active && element_status
                                ? isChipGood( *element_active, *element_status, module_hash, chip_i)
                                : isChipGood( module_hash, chip_i) );
#ifdef DO_VALIDATE_STATUS_ARRAY
     Identifier pixelID = m_pixelReadout->getPixelIdfromHash(module_hash, chip_i, 1, 1);
     VALIDATE_STATUS_ARRAY(element_active, element_active->isChipGood(module_hash, chip_i), m_pixelCondSummaryTool->isActive(module_hash,pixelID)  );
     VALIDATE_STATUS_ARRAY(element_status, element_status->isChipGood(module_hash, chip_i), m_pixelCondSummaryTool->isGood(module_hash,pixelID)  );
#endif
     return ret;
  }
  bool isChipActive(const InDet::SiDetectorElementStatus *element_active,
                    const IdentifierHash &module_hash,
                    unsigned int chip_i) const {
     bool ret( element_active
               ? element_active->isChipGood(module_hash, chip_i)
               : isChipActive( module_hash, chip_i) );
#ifdef DO_VALIDATE_STATUS_ARRAY
     Identifier pixelID = m_pixelReadout->getPixelIdfromHash(module_hash, chip_i, 1, 1);
     VALIDATE_STATUS_ARRAY(element_active, element_active->isChipGood(module_hash, chip_i), m_pixelCondSummaryTool->isActive(module_hash,pixelID)  );
#endif
     return ret;
  }
};

#endif
