/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/SensorSim3DTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief 3D pixel sensor simulation
 */

#ifndef PIXELDIGITIZATION_SensorSim3DTool_H
#define PIXELDIGITIZATION_SensorSim3DTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SensorSimTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "RadDamageUtil.h"
#include "PixelConditionsData/PixelHistoConverter.h"

class SensorSim3DTool: public SensorSimTool {
public:
  SensorSim3DTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual ~SensorSim3DTool();

  virtual StatusCode induceCharge(const TimedHitPtr<SiHit>& phit, SiChargedDiodeCollection& chargedDiodes,
                                  const InDetDD::SiDetectorElement& Module, const InDetDD::PixelModuleDesign& p_design,
                                  std::vector< std::pair<double, double> >& trfHitRecord,
                                  std::vector<double>& initialConditions, CLHEP::HepRandomEngine* rndmEngine) override;


  // 3D sensor simulation using probability density map (used in RUN-2 (no radiation damage)
  StatusCode readProbMap(const std::string&);
  StatusCode printProbMap(const std::string&);
  double getProbMapEntry(const std::string&, int, int) const;

  double getElectricField(double x, double y);
  double getMobility(double electricField, bool isHoleBit);
  double getDriftTime(bool isHoleBit);
  double getTimeToElectrode(double x, double y, bool isHoleBit);
  double getTrappingPositionX(double initX, double initY, double driftTime, bool isHoleBit);
  double getTrappingPositionY(double initX, double initY, double driftTime, bool isHoleBit);
private:
  SensorSim3DTool();

  // 3D sensor simulation using probability density map (used in RUN-2 (no radiation damage)
  std::multimap<std::pair<int, int>, double> m_probMapFEI4;
  std::multimap<std::pair<int, int>, double> m_probMapFEI3;

  // Map for radiation damage simulation
  std::vector<PixelHistoConverter> m_ramoPotentialMap;
  std::vector<PixelHistoConverter> m_eFieldMap;
  std::vector<PixelHistoConverter> m_xPositionMap_e;
  std::vector<PixelHistoConverter> m_xPositionMap_h;
  std::vector<PixelHistoConverter> m_yPositionMap_e;
  std::vector<PixelHistoConverter> m_yPositionMap_h;
  std::vector<PixelHistoConverter> m_timeMap_e;
  std::vector<PixelHistoConverter> m_timeMap_h;
  PixelHistoConverter m_avgChargeMap_e;
  PixelHistoConverter m_avgChargeMap_h;

  std::vector<double> m_fluence_layers;

  Gaudi::Property<std::string> m_cc_prob_file_fei3
  {
    this, "CCProbMapFileFEI3", "PixelDigitization/3DFEI3-3E-problist-1um_v1.txt",
    "Input probability file for 3D FEI3 sensor."
  };

  Gaudi::Property<std::string> m_cc_prob_file_fei4
  {
    this, "CCProbMapFileFEI4", "PixelDigitization/3DFEI4-2E-problist-1um_v0.txt",
    "Input probability file for 3D FEI4 sensor."
  };

  Gaudi::Property<int> m_numberOfSteps
  {
    this, "numberOfSteps", 50, "Number of steps for Pixel3D module"
  };

  Gaudi::Property<bool> m_doRadDamage
  {
    this, "doRadDamage", false, "doRadDmaage bool: should be flag"
  };

  Gaudi::Property<bool> m_doChunkCorrection
  {
    this, "doChunkCorrection", false, "doChunkCorrection bool: should be flag"
  };

  Gaudi::Property<double> m_fluence
  {
    this, "fluence", 0,
    "this is the fluence benchmark, 0-6.  0 is unirradiated, 1 is start of Run 2, 5 is end of 2018 and 6 is projected end of 2018"
  };

  Gaudi::Property<double> m_trappingTimeElectrons
  {
    this, "trappingTimeElectrons", 0.0, "Characteristic time till electron is trapped [ns]"
  };

  Gaudi::Property<double> m_trappingTimeHoles
  {
    this, "trappingTimeHoles", 0.0, "Characteristic time till hole is trapped [ns]"
  };

  Gaudi::Property<double> m_temperature
  {
    this, "Temperature", 300.0, "Default temperature [K]"
  };

  ToolHandle<RadDamageUtil> m_radDamageUtil
  {
    this, "RadDamageUtil", "RadDamageUtil", "Rad Damage utility"
  };
};

#endif // PIXELDIGITIZATION_SensorSim3DTool_H
