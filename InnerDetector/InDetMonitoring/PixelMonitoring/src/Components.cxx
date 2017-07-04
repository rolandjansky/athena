#include "PixelMonitoring/Components.h"

namespace PixMon {
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
