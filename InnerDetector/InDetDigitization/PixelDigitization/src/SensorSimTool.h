/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/SensorSimTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Base class of pixel sensor simulation
 */

#ifndef PIXELDIGITIZATION_SensorSimTool_H
#define PIXELDIGITIZATION_SensorSimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "HitManagement/TimedHitPtr.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "StoreGate/ReadCondHandleKey.h"

static const InterfaceID IID_ISensorSimTool("SensorSimTool", 1, 0);

class SensorSimTool: public AthAlgTool, virtual public IAlgTool {
public:
  SensorSimTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent) {
    declareInterface<SensorSimTool>(this);
  }

  static const InterfaceID& interfaceID() {return IID_ISensorSimTool;}

  virtual StatusCode initialize() {
    ATH_CHECK(AthAlgTool::initialize());
    ATH_CHECK(m_siPropertiesTool.retrieve());
    ATH_CHECK(m_moduleDataKey.initialize());
    return StatusCode::SUCCESS;
  }

  virtual StatusCode finalize() {return StatusCode::FAILURE;}
  virtual ~SensorSimTool() {}
  virtual StatusCode induceCharge(const TimedHitPtr<SiHit>& phit, SiChargedDiodeCollection& chargedDiodes,
                                  const InDetDD::SiDetectorElement& Module, const InDetDD::PixelModuleDesign& p_design,
                                  std::vector< std::pair<double, double> >& trfHitRecord,
                                  std::vector<double>& initialConditions, CLHEP::HepRandomEngine* rndmEngine) = 0;
private:
  SensorSimTool();
protected:
  ToolHandle<ISiPropertiesTool> m_siPropertiesTool
  {
    this, "SiPropertiesTool", "SiPropertiesTool", "Tool to retrieve SiProperties"
  };

  SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
  {
    this, "PixelModuleData", "PixelModuleData", "Pixel module data"
  };
};

#endif // PIXELDIGITIZATION_SensorSimTool_H
