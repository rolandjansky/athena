/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelRadiationDamageFluenceMapData.h"

PixelRadiationDamageFluenceMapData::PixelRadiationDamageFluenceMapData():
  m_fluenceLayer(),
  m_lorentzMap_e(),
  m_lorentzMap_h(),
  m_distanceMap_e(),
  m_distanceMap_h(),
  m_ramoPotentialMap(),
  m_fluenceLayer3D(),
  m_ramoPotentialMap3D(),
  m_eFieldMap3D(),
  m_xPositionMap3D_e(),
  m_xPositionMap3D_h(),
  m_yPositionMap3D_e(),
  m_yPositionMap3D_h(),
  m_timeMap3D_e(),
  m_timeMap3D_h()
{ }

PixelRadiationDamageFluenceMapData::~PixelRadiationDamageFluenceMapData() { }

// Map for radiation damage simulation
void PixelRadiationDamageFluenceMapData::setFluenceLayer(std::vector<double> fluenceLayer) { m_fluenceLayer = fluenceLayer; }
double PixelRadiationDamageFluenceMapData::getFluenceLayer(int layer) const { return m_fluenceLayer.at(layer); }

void PixelRadiationDamageFluenceMapData::setLorentzMap_e(std::vector<PixelHistoConverter> lorentzMap_e) { m_lorentzMap_e = lorentzMap_e; }
void PixelRadiationDamageFluenceMapData::setLorentzMap_h(std::vector<PixelHistoConverter> lorentzMap_h) { m_lorentzMap_h = lorentzMap_h; }
void PixelRadiationDamageFluenceMapData::setDistanceMap_e(std::vector<PixelHistoConverter> distanceMap_e) { m_distanceMap_e = distanceMap_e; }
void PixelRadiationDamageFluenceMapData::setDistanceMap_h(std::vector<PixelHistoConverter> distanceMap_h) { m_distanceMap_h = distanceMap_h; }
void PixelRadiationDamageFluenceMapData::setRamoPotentialMap(std::vector<PixelHistoConverter> ramoPotentialMap) { m_ramoPotentialMap = ramoPotentialMap; }

const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getLorentzMap_e(int layer) const { return m_lorentzMap_e.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getLorentzMap_h(int layer) const { return m_lorentzMap_h.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getDistanceMap_e(int layer) const { return m_distanceMap_e.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getDistanceMap_h(int layer) const { return m_distanceMap_h.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getRamoPotentialMap(int layer) const { return m_ramoPotentialMap.at(layer); }

// Map for radiation damage simulation for 3D sensor
void PixelRadiationDamageFluenceMapData::setFluenceLayer3D(std::vector<double> fluenceLayer) { m_fluenceLayer3D = fluenceLayer; }
double PixelRadiationDamageFluenceMapData::getFluenceLayer3D(int layer) const { return m_fluenceLayer3D.at(layer); }

void PixelRadiationDamageFluenceMapData::setRamoPotentialMap3D(std::vector<PixelHistoConverter> ramoPotentialMap3D) { m_ramoPotentialMap3D = ramoPotentialMap3D; }
void PixelRadiationDamageFluenceMapData::setEFieldMap3D(std::vector<PixelHistoConverter> eFieldMap3D) { m_eFieldMap3D = eFieldMap3D; }
void PixelRadiationDamageFluenceMapData::setXPositionMap3D_e(std::vector<PixelHistoConverter> xPositionMap3D_e) { m_xPositionMap3D_e = xPositionMap3D_e; }
void PixelRadiationDamageFluenceMapData::setXPositionMap3D_h(std::vector<PixelHistoConverter> xPositionMap3D_h) { m_xPositionMap3D_h = xPositionMap3D_h; }
void PixelRadiationDamageFluenceMapData::setYPositionMap3D_e(std::vector<PixelHistoConverter> yPositionMap3D_e) { m_yPositionMap3D_e = yPositionMap3D_e; }
void PixelRadiationDamageFluenceMapData::setYPositionMap3D_h(std::vector<PixelHistoConverter> yPositionMap3D_h) { m_yPositionMap3D_h = yPositionMap3D_h; }
void PixelRadiationDamageFluenceMapData::setTimeMap3D_e(std::vector<PixelHistoConverter> timeMap3D_e) { m_timeMap3D_e = timeMap3D_e; }
void PixelRadiationDamageFluenceMapData::setTimeMap3D_h(std::vector<PixelHistoConverter> timeMap3D_h) { m_timeMap3D_h = timeMap3D_h; }
void PixelRadiationDamageFluenceMapData::setAvgChargeMap3D_e(PixelHistoConverter avgChargeMap3D_e) { m_avgChargeMap3D_e = avgChargeMap3D_e; }
void PixelRadiationDamageFluenceMapData::setAvgChargeMap3D_h(PixelHistoConverter avgChargeMap3D_h) { m_avgChargeMap3D_h = avgChargeMap3D_h; }

const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getRamoPotentialMap3D(int layer) const { return m_ramoPotentialMap3D.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getEFieldMap3D(int layer) const { return m_eFieldMap3D.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getXPositionMap3D_e(int layer) const { return m_xPositionMap3D_e.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getXPositionMap3D_h(int layer) const { return m_xPositionMap3D_h.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getYPositionMap3D_e(int layer) const { return m_yPositionMap3D_e.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getYPositionMap3D_h(int layer) const { return m_yPositionMap3D_h.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getTimeMap3D_e(int layer) const { return m_timeMap3D_e.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getTimeMap3D_h(int layer) const { return m_timeMap3D_h.at(layer); }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getAvgChargeMap3D_e() const { return m_avgChargeMap3D_e; }
const PixelHistoConverter& PixelRadiationDamageFluenceMapData::getAvgChargeMap3D_h() const { return m_avgChargeMap3D_h; }

void PixelRadiationDamageFluenceMapData::clear() {
  m_fluenceLayer.clear();
  m_lorentzMap_e.clear();
  m_lorentzMap_h.clear();
  m_distanceMap_e.clear();
  m_distanceMap_h.clear();
  m_ramoPotentialMap.clear();
  m_fluenceLayer3D.clear();
  m_ramoPotentialMap3D.clear();
  m_eFieldMap3D.clear();
  m_xPositionMap3D_e.clear();
  m_xPositionMap3D_h.clear();
  m_yPositionMap3D_e.clear();
  m_yPositionMap3D_h.clear();
  m_timeMap3D_e.clear();
  m_timeMap3D_h.clear();
}

