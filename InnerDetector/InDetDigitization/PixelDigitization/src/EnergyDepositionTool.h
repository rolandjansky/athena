/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/EnergyDepositionTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Provides energy deposition in sensor. Default:Bichsel model
 */

#ifndef PIXELDIGITIZATION_EnergyDepositionTool_H
#define PIXELDIGITIZATION_EnergyDepositionTool_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Service.h"
#include "HitManagement/TimedHitPtr.h"
#include "BichselData.h"

#include "PixelConditionsData/PixelDistortionData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include <vector>
#include <utility> //for std::pair


class PixelID;
namespace InDetDD{
 class SiDetectorElement;
}

namespace CLHEP{
  class HepRandomEngine;
}
class SiHit;



//====================
//  C L A S S   D E F
//====================
class EnergyDepositionTool: public AthAlgTool {
public:
  EnergyDepositionTool(const std::string& type, const std::string& name, const IInterface* parent);

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual ~EnergyDepositionTool();
  StatusCode initTools();

  std::vector<std::pair<double, double> > BichselSim(double BetaGamma, int ParticleType, double TotalLength,
                                                     double InciEnergy, CLHEP::HepRandomEngine* rndmEngine) const;   // output hit record in the format (hit position, energy loss)

  std::vector<std::pair<double, double> > ClusterHits(std::vector<std::pair<double, double> >& rawHitRecord,
                                                      int n_pieces) const;         // cluster hits into n steps (there could be thousands of hit)                                                            

  virtual StatusCode depositEnergy(const TimedHitPtr<SiHit>& phit, const InDetDD::SiDetectorElement& Module,
                                   std::vector<std::pair<double, double> >& trfHitRecord,
                                   std::vector<double>& initialConditions, CLHEP::HepRandomEngine* rndmEngine,
                                   const EventContext &ctx);

  // Variables
private:
  EnergyDepositionTool();

  const PixelID* m_pixelID {
    nullptr
  };

  std::vector<BichselData> m_BichselData;      // vector to store Bichsel Data. Each entry is for one particle type

  Gaudi::Property<int> m_numberOfSteps
  {
    this, "numberOfSteps", 50, "Geant4:number of steps for PixelPlanar"
  };

  Gaudi::Property<int> m_numberOfCharges
  {
    this, "numberOfCharges", 10, "Geant4:number of charges for PixelPlanar"
  };

  Gaudi::Property<bool> m_disableDistortions
  {
    this, "DisableDistortions", false, "Disable simulation of module distortions"
  };

  Gaudi::Property<bool> m_doBichsel
  {
    this, "doBichsel", true, "re-do charge deposition following Bichsel model"
  };

  Gaudi::Property<double> m_doBichselBetaGammaCut
  {
    this, "doBichselBetaGammaCut", 0.1, "minimum beta-gamma for particle to be re-simulated through Bichsel Model"
  };

  Gaudi::Property<bool> m_doDeltaRay
  {
    this, "doDeltaRay", false, "whether we simulate delta-ray using Bichsel model"
  };

  Gaudi::Property<double> m_DeltaRayCut
  {
    this, "DeltaRayCut", 80.7687, "Cut of delta ray [keV] - Value should be consistent with range cut in simulation"
  };

  Gaudi::Property<bool> m_doPU
  {
    this, "doPU", true, "Whether we apply Bichsel model on PU"
  };

  Gaudi::Property<int> m_nCols
  {
    this, "nCols", 1, "Number of collision for each sampling"
  };

  Gaudi::Property<int> m_LoopLimit
  {
    this, "LoopLimit", 100000, "Limit assuming 1 collision per sampling"
  };

  SG::ReadCondHandleKey<PixelDistortionData> m_distortionKey
  {
    this, "PixelDistortionData", "PixelDistortionData", "Output readout distortion data"
  };
private:
  void simulateBow(const InDetDD::SiDetectorElement* element, double& xi, double& yi, const double zi, double& xf,
                   double& yf, const double zf) const;
                                                                                           // ! unit is eV
};

#endif //PIXELDIGITIZATION_EnergyDepositionTool_H
