/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXMON_COMPONENTS_H_
#define PIXMON_COMPONENTS_H_

#include <array>
#include <string>

namespace PixMon {
/**
 * Enum class listing all pixel detector components
 *
 * N.B. This is identical to the PixLayerIBL2D3DDBM enum of PixelMainMon and is
 * supposed to replace it in the future.
 */
enum class LayerIBL2D3DDBM {kECA = 0, kECC, kB0, kB1, kB2, kDBMA, kDBMC, kIBL, kIBL2D, kIBL3D, COUNT};

const unsigned int kNumLayersDisk{3};
const unsigned int kNumLayersDBM{3};
const unsigned int kNumStavesIBL{14};
const unsigned int kNumStavesL0{22};
const unsigned int kNumStavesL1{38};
const unsigned int kNumStavesL2{52};

const unsigned int kNumModulesIBL{32};
const unsigned int kNumModulesIBL2D{12};
const unsigned int kNumModulesIBL3D{8};
const unsigned int kNumModulesBarrel{13};
const unsigned int kNumModulesDisk{48};
const unsigned int kNumModulesDBM{4};

extern const std::array<std::string, kNumStavesIBL> StavesIBL;
extern const std::array<std::string, kNumStavesL0> StavesL0;
extern const std::array<std::string, kNumStavesL1> StavesL1;
extern const std::array<std::string, kNumStavesL2> StavesL2;
extern const std::array<std::string, kNumModulesBarrel> ModulesBarrel;
extern const std::array<std::string, kNumModulesIBL> ModulesIBL;
extern const std::array<std::string, kNumModulesIBL2D> ModulesIBL2D;
extern const std::array<std::string, kNumModulesIBL3D> ModulesIBL3D;

extern const std::array<std::string, kNumLayersDisk> LayersDisk;
extern const std::array<std::string, kNumLayersDBM> LayersDBM;
extern const std::array<std::string, kNumModulesDisk> ModulesECA;
extern const std::array<std::string, kNumModulesDisk> ModulesECC;
extern const std::array<std::string, kNumModulesDBM> ModulesDBM;

/**
 * Enum class to set configuration of histogram containers
 *
 * This object is given to all histogram classes and determines for which
 * components the histograms are initialised.
 *   - Pix  = pixel components (L0, L1, L2, ECA, ECC)
 *   - IBL  = IBL
 *   - IBL2D3D = IBL plus additional IBL2D and IBL3D
 *   - DBM  = DBM
 */
enum class HistConf {kPix = 0, kPixIBL, kPixIBL2D3D, kPixDBM, kPixDBMIBL, kPixDBMIBL2D3D, kDBM, kDBMIBL, kDBMIBL2D3D, kIBL, kIBL2D3D, COUNT};

/**
 * Function to check whether the HistConfig object 'config' includes a certain
 * pixel detector component (desribed by enum class LayerIBL2D3DDBM).
 *
 * @param config: histogram configuration object
 * @param component: the pixel detector component to be tested
 */
bool HasComponent(const HistConf& config, const LayerIBL2D3DDBM& component);
}  // namespace PixMon

#endif  // PIXMON_COMPONENTS_H_
