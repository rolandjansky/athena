/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelDigitization/SensorSimPlanarTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Planar pixel sensor simulation
 */

#ifndef PIXELDIGITIZATION_SensorSimPlanarTool_H
#define PIXELDIGITIZATION_SensorSimPlanarTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "SensorSimTool.h"
#include "RadDamageUtil.h"
#include "PixelConditionsData/PixelHistoConverter.h"


class SensorSimPlanarTool: public SensorSimTool {
public:
  SensorSimPlanarTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual ~SensorSimPlanarTool();

  virtual StatusCode induceCharge(const TimedHitPtr<SiHit>& phit,
                                  SiChargedDiodeCollection& chargedDiodes,
                                  const InDetDD::SiDetectorElement& Module,
                                  const InDetDD::PixelModuleDesign& p_design,
                                  const PixelModuleData *moduleData, 
                                  std::vector< std::pair<double, double> >& trfHitRecord,
                                  std::vector<double>& initialConditions,
                                  CLHEP::HepRandomEngine* rndmEngine,
                                  const EventContext &ctx) override;

private:
  SensorSimPlanarTool();
  
  //Apply slim edge inefficiencies for IBL sensors
  void applyIBLSlimEdges(double& energyPerStep, double& eta_drifted) const;

  // Map for radiation damage simulation
  std::vector<PixelHistoConverter> m_ramoPotentialMap;
  std::vector<PixelHistoConverter> m_distanceMap_e;
  std::vector<PixelHistoConverter> m_distanceMap_h;
  std::vector<PixelHistoConverter> m_lorentzMap_e;
  std::vector<PixelHistoConverter> m_lorentzMap_h;

  Gaudi::Property<int> m_numberOfSteps
  {
    this, "numberOfSteps", 50, "Geant4:number of steps for PixelPlanar"
  };

  Gaudi::Property<double> m_diffusionConstant
  {
    this, "diffusionConstant", 0.0, "Geant4:Diffusion Constant for PixelPlanar"
  };

  Gaudi::Property<bool> m_doRadDamage
  {
    this, "doRadDamage", false, "doRadDmaage bool: should be flag"
  };

  Gaudi::Property<bool> m_doInterpolateEfield
  {
    this, "doInterpolateEfield", false, "doInterpolateEfield bool: should be flag"
  };

  Gaudi::Property<std::vector<std::string> > m_fluenceMap
  {
    this, "FluenceMap", {
      "PixelDigitization/maps_IBL_PL_400V_fl5_5e14.root",
      "PixelDigitization/maps_PIX_400V_fl5_19e14.root",
      "PixelDigitization/maps_PIX_250V_fl2_28e14.root",
      "PixelDigitization/maps_PIX_250V_fl1_53e14.root"
    },
    "Fluence map for radiation damage when interpolation method is activated"
  };

  Gaudi::Property<std::vector<double> > m_fluenceLayer
  {
    this, "FluenceLayer", {
      5.50e14, 5.19e14, 2.28e14, 1.53e14
    }, "Fluence for radiation damage when interpolation method is activated"
  };

  Gaudi::Property<std::vector<float> > m_voltageLayer
  {
    this, "BiasVoltageLayer", {
      400.0, 400.0, 250.0, 250.0
    }, "Bias voltage for radiation damage when interpolation method is activated"
  };

  ToolHandle<RadDamageUtil> m_radDamageUtil
  {
    this, "RadDamageUtil", "RadDamageUtil", "Rad Damage utility"
  };

  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool
  {
    this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"
  };
};

#endif // PIXELDIGITIZATION_SensorSimPlanarTool_H
