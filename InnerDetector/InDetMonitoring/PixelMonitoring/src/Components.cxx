#include "PixelMonitoring/Components.h"

namespace PixMon {
// Definitions for barrel layers and IBL
const std::array<std::string, kNumStavesIBL> StavesIBL = {{
    "S01", "S02", "S03", "S04", "S05", "S06", "S07", "S08", "S09", "S10", "S11",
    "S12", "S13","S14"}};

const std::array<std::string, kNumStavesL0> StavesL0 = {{
    "B11_S2", "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
    "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1",
    "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2","B10_S1", "B10_S2",
    "B11_S1"}};

const std::array<std::string, kNumStavesL1> StavesL1 = {{
    "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1",
    "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2",
    "B08_S1", "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1",
    "B11_S2", "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2",
    "B15_S1", "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1",
    "B18_S2", "B19_S1", "B19_S2"}};

const std::array<std::string, kNumStavesL2> StavesL2 = {{
    "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2", "B04_S1", "B04_S2",
    "B05_S1", "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2", "B08_S1",
    "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1", "B11_S2",
    "B12_S1", "B12_S2", "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1",
    "B15_S2", "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
    "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2", "B22_S1",
    "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2", "B25_S1", "B25_S2",
    "B26_S1", "B26_S2", "B01_S1"}};

const std::array<std::string, kNumModulesIBL> ModulesIBL = {{
    "C8", "", "C7", "", "C6", "", "C5", "", "C4", "", "C3", "", "C2", "",
    "C1", "", "A1", "", "A2", "", "A3", "", "A4", "", "A5", "", "A6", "",
    "A7", "", "A8", ""}};

const std::array<std::string, kNumModulesIBL2D> ModulesIBL2D = {{
    "M3_C6", "M3_C5", "M2_C4", "M1_C3", "M1_C2", "M1_C1", "M1_A1", "M1_A2",
    "M2_A3", "M2_A4", "M3_A5", "M3_A6"}};

const std::array<std::string, kNumModulesIBL3D> ModulesIBL3D = {{
    "M4_C8_2", "M4_C8_1", "M4_C7_2", "M4_C7_1", "M4_A7_1", "M4_A7_2", "M4_A8_1",
    "M4_A8_2"}};

const std::array<std::string, kNumModulesBarrel> ModulesBarrel = {{
    "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A",
    "M5A", "M6A"}};


// Definitions for end-caps and DBM
const std::array<std::string, kNumLayersDisk> LayersDisk = {{"Disk 1", "Disk 2", "Disk 3"}};

const std::array<std::string, kNumLayersDBM> LayersDBM = {{"Layer 0", "Layer 1", "Layer 2"}};

const std::array<std::string, kNumModulesDisk> ModulesECA = {{
    "B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3",
    "B01_S2_M4", "B02_S1_M1", "B02_S1_M6", "B02_S1_M2", "B02_S1_M5",
    "B02_S1_M3", "B02_S1_M4", "B02_S2_M1", "B02_S2_M6", "B02_S2_M2",
    "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", "B03_S1_M1", "B03_S1_M6",
    "B03_S1_M2", "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", "B03_S2_M1",
    "B03_S2_M6", "B03_S2_M2", "B03_S2_M5", "B03_S2_M3", "B03_S2_M4",
    "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3",
    "B04_S1_M4", "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5",
    "B04_S2_M3", "B04_S2_M4", "B01_S1_M1", "B01_S1_M6", "B01_S1_M2",
    "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"}};

const std::array<std::string, kNumModulesDisk> ModulesECC = {{
    "B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6",
    "B01_S2_M1", "B02_S1_M4", "B02_S1_M3", "B02_S1_M5", "B02_S1_M2",
    "B02_S1_M6", "B02_S1_M1", "B02_S2_M4", "B02_S2_M3", "B02_S2_M5",
    "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", "B03_S1_M4", "B03_S1_M3",
    "B03_S1_M5", "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", "B03_S2_M4",
    "B03_S2_M3", "B03_S2_M5", "B03_S2_M2", "B03_S2_M6", "B03_S2_M1",
    "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6",
    "B04_S1_M1", "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2",
    "B04_S2_M6", "B04_S2_M1", "B01_S1_M4", "B01_S1_M3", "B01_S1_M5",
    "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"}};

const std::array<std::string, kNumModulesDBM> ModulesDBM = {{"M3", "M4", "M1", "M2"}};

bool HasComponent(const HistConf& config, const LayerIBL2D3DDBM& component) {
  if (component >= LayerIBL2D3DDBM::COUNT) {
    // return here if we are out of scope
    return false;
  } else if (config == HistConf::kPix && component <= LayerIBL2D3DDBM::kB2) {
    return true;
  } else if (config == HistConf::kPixIBL && (component <= LayerIBL2D3DDBM::kB2 || component == LayerIBL2D3DDBM::kIBL)) {
    return true;
  } else if (config == HistConf::kPixIBL2D3D && (component <= LayerIBL2D3DDBM::kB2 || component >= LayerIBL2D3DDBM::kIBL)) {
    return true;
  } else if (config == HistConf::kPixDBM && component <= LayerIBL2D3DDBM::kDBMC) {
    return true;
  } else if (config == HistConf::kPixDBMIBL && component <= LayerIBL2D3DDBM::kIBL) {
    return true;
  } else if (config == HistConf::kPixDBMIBL2D3D) {
    return true;
  } else if (config == HistConf::kDBM && (component == LayerIBL2D3DDBM::kDBMA || component == LayerIBL2D3DDBM::kDBMC)) {
    return true;
  } else if (config == HistConf::kDBMIBL && component >= LayerIBL2D3DDBM::kDBMA && component <= LayerIBL2D3DDBM::kIBL) {
    return true;
  } else if (config == HistConf::kDBMIBL2D3D && component >= LayerIBL2D3DDBM::kDBMA) {
    return true;
  } else if (config == HistConf::kIBL && component == LayerIBL2D3DDBM::kIBL) {
    return true;
  } else if (config == HistConf::kIBL2D3D && component >= LayerIBL2D3DDBM::kIBL) {
    return true;
  } else {
    return false;
  }
}
}  // namespace PixMon
