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
  
  int getLayerCellOrder() const { return m_layerToStore; }
  void setLayerCellOrder(int layerToStore) { m_layerToStore = layerToStore; }
  int getRadiusCellOrder() const { return m_radiusToStore; }
  void setRadiusCellOrder(int radiusToStore) { m_radiusToStore = radiusToStore; }
  int getAverageEnergyDensity() const { return m_averageEnergyDensityPerRing; }
  void setAverageEnergyDensity(int averageEnergyDensityPerRing) { m_averageEnergyDensityPerRing = averageEnergyDensityPerRing; }

 
 private:
 
 
 int m_layerToStore;
 float m_radiusToStore;
 float m_averageEnergyDensityPerRing;
//  
};