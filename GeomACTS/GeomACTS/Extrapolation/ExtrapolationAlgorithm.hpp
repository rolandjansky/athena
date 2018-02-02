// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
#define ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H

#include <cmath>
#include <limits>
#include <memory>

#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Utilities/GeometryID.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
class TrackingVolume;
}  // namespace Acts


class ExtrapolationAlgorithm
{
public:
  struct Config
  {
    /// the extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// the cuts applied in this case
    /// @todo remove later and replace by particle selector
    double maxD0  = std::numeric_limits<double>::max();
    double maxEta = std::numeric_limits<double>::max();
    ;
    double minPt = 0.0;
    /// skip or process neutral particles
    bool skipNeutral = false;
    /// configuration: sensitive collection
    bool collectSensitive = true;
    /// configuration: collect passive
    bool collectPassive = true;
    /// configuration: collect boundary
    bool collectBoundary = true;
    /// configuration: collect material
    bool collectMaterial = true;
    /// configuration: don't collapse
    bool sensitiveCurvilinear = false;
    /// define how robust the search mode is
    int searchMode = 0;
    /// set the patch limit of the extrapolation
    double pathLimit = -1.;
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the loggin level
  ExtrapolationAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  void
  execute() const final;

private:
  Config m_cfg;  ///< the config class

  /// the templated execute test method for
  /// charged and netural particles
  /// @param [in] the start parameters
  /// @param [in] the particle barcode
  /// @param [in] the detector data container
  template <class T>
  executeTestT(
      const T&                                 startParameters,
      //barcode_type                             barcode,
      std::vector<Acts::ExtrapolationCell<T>>& eCells,
      //DetectorData<geo_id_value,
                   //std::pair<std::unique_ptr<const T>, barcode_type>>* dData
      //= nullptr
      ) const;
};


template <class T>
FW::ProcessCode
FW::ExtrapolationAlgorithm::executeTestT(
    const T&                                 startParameters,
    barcode_type                             barcode,
    std::vector<Acts::ExtrapolationCell<T>>& eCells,
    //FW::DetectorData<geo_id_value,
                     //std::pair<std::unique_ptr<const T>, barcode_type>>* dData

    )
    const
{
  // setup the extrapolation how you'd like it
  Acts::ExtrapolationCell<T> ecc(startParameters);
  // ecc.setParticleHypothesis(m_cfg.particleType);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
  ecc.searchMode = m_cfg.searchMode;
  // now set the behavioral bits
  if (m_cfg.collectSensitive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
  if (m_cfg.collectPassive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
  if (m_cfg.collectBoundary)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
  if (m_cfg.collectMaterial)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
  if (m_cfg.sensitiveCurvilinear) ecc.sensitiveCurvilinear = true;

  // force a stop in the extrapoaltion mode
  if (m_cfg.pathLimit > 0.) {
    ecc.pathLimit = m_cfg.pathLimit;
    ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithPathLimit);
  }
  // screen output
  ACTS_DEBUG("===> forward extrapolation - collecting information <<===");

  // theta / phi
  auto   sPosition = startParameters.position();
  double sTheta    = startParameters.momentum().theta();
  double sPhi      = startParameters.momentum().theta();
  double tX0       = 0.;
  double tL0       = 0.;
  // material steps to be  filled
  std::vector<Acts::MaterialStep> materialSteps;

  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
  if (eCode.isFailure()) {
    ACTS_WARNING("Extrapolation failed.");
    return FW::ProcessCode::ABORT;
  }

  // create the detector hits data
  /*if (dData && m_cfg.simulatedHitsCollection != "") {
    /// loop over steps and get the sensitive
    for (auto& es : ecc.extrapolationSteps) {
      // check if you have parameters
      if (es.parameters) {
        // get the surface
        const Acts::Surface& sf = es.parameters->referenceSurface();
        // check if you have material
        if (es.configuration.checkMode(
                Acts::ExtrapolationMode::CollectSensitive)
            && dData) {
          // fill the appropriate vector
          geo_id_value volumeID
              = sf.geoID().value(Acts::GeometryID::volume_mask);
          geo_id_value layerID = sf.geoID().value(Acts::GeometryID::layer_mask);
          geo_id_value moduleID
              = sf.geoID().value(Acts::GeometryID::sensitive_mask);
          // search and/or insert - we need to clone as the ECC will be wrritten
          auto parcpptr = std::unique_ptr<const T>(es.parameters->clone());
          std::pair<std::unique_ptr<const T>, barcode_type> eHit(
              std::move(parcpptr), barcode);
          FW::Data::insert(
              *dData, volumeID, layerID, moduleID, std::move(eHit));
        }
      }
    }
    // also check the end parameters
    if (ecc.endParameters) {
      // get the endSurface
      const Acts::Surface& esf = ecc.endParameters->referenceSurface();
      // check if sensitive
      if (esf.associatedDetectorElement()) {
        // fill the appropriate vector
        geo_id_value volumeID
            = esf.geoID().value(Acts::GeometryID::volume_mask);
        geo_id_value layerID = esf.geoID().value(Acts::GeometryID::layer_mask);
        geo_id_value moduleID
            = esf.geoID().value(Acts::GeometryID::sensitive_mask);
        // search and/or insert
        std::pair<std::unique_ptr<const T>, barcode_type> eHit(
            std::move(ecc.endParameters), barcode);
        FW::Data::insert(*dData, volumeID, layerID, moduleID, std::move(eHit));
      }
    }
  }*/
  /// fill the ecc step into the container at the end
  eCells.push_back(std::move(ecc));

  // return success
  return FW::ProcessCode::SUCCESS;
}

#endif  // ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
