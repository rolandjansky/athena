#include "eflowRec/eflowCellLevelSubtractionTool.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

// class eflowCellLevelSubtractionTool;

class eflowCellOrderingParameters {
 
 public:
  eflowCellOrderingParameters ();
  ~eflowCellOrderingParameters () {};
  
  std::vector<int> getLayerCellOrder() const { return m_layerToStore; }
  void setLayerCellOrder(std::vector<int> layerToStoreVector) { m_layerToStore = layerToStoreVector; }
  int getRadiusCellOrder() const { return m_radiusToStore; }
  void setRadiusCellOrder(int radiusToStore) { m_radiusToStore = radiusToStore; }
  int getAverageEnergyDensity() const { return m_averageEnergyDensityPerRing; }
  void setAverageEnergyDensity(int averageEnergyDensityPerRing) { m_averageEnergyDensityPerRing = averageEnergyDensityPerRing; }

 
 private:
 
 
 std::vector<int> m_layerToStore;
 float m_radiusToStore;
 float m_averageEnergyDensityPerRing;
//  
};