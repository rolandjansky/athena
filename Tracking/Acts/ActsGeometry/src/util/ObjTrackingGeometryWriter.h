/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#pragma once

#include <mutex>

#include <fstream>
#include <iostream>
#include "ObjSurfaceWriter.h"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Geometry/GeometryContext.hpp"

namespace Acts {
class TrackingVolume;

/// @class ObjTrackingGeometryWriter
///
/// An Obj writer for the geometry
/// It delegates the writing of surfaces to the surface writers
class ObjTrackingGeometryWriter {
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    /// the default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// the name of the writer
    std::string name = "";
    /// surfaceWriters
    std::vector<std::shared_ptr<ObjSurfaceWriter>> surfaceWriters;
    std::string                                    filePrefix           = "";
    std::string                                    sensitiveGroupPrefix = "";
    std::string                                    layerPrefix          = "";

    Config(const std::string&   lname = "ObjTrackingGeometryWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
      , surfaceWriters()
    {
    }
  };

  /// Constructor
  /// @param cfg is the configuration class
  ObjTrackingGeometryWriter(const Config& cfg);

  /// Framework name() method
  /// @return the name of the tool
  std::string
  name() const;

  /// The write interface
  /// @param tGeometry is the geometry to be written out
  /// @return ProcessCode to indicate success/failure
  void
  write(const Acts::GeometryContext& gctx, const Acts::TrackingGeometry& tGeometry);

private:
  Config m_cfg;  ///< the config class

  /// process this volume
  /// @param tVolume the volume to be processed
  void
  write(const Acts::GeometryContext& gctx, const Acts::TrackingVolume& tVolume);

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

}
