/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#pragma once

#include "Acts/Geometry/GeometryContext.hpp"
#include <fstream>
#include <iostream>
#include <mutex>
#include "ObjHelper.h"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {

/// @class ObjSurfaceWriter
///
/// An Obj writer for the geometry
///
class ObjSurfaceWriter
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    /// the default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// the name of the algorithm
    std::string name;
    /// approximate cyinders by that
    unsigned int outputPhiSegments = 72;
    /// write thickness if available
    double outputThickness = 2.;
    /// write sensitive surfaces
    bool outputSensitive = true;
    /// write the layer surface out
    bool outputLayerSurface = true;
    /// output scalor
    double outputScalor = 1.;
    /// precision for out
    unsigned int outputPrecision = 6;
    /// file prefix to be written out
    std::string filePrefix = "";
    /// prefixes
    /// @todo These aren't used anywhere, should they be dropped?
    std::string planarPrefix   = "";
    std::string cylinderPrefix = "";
    std::string diskPrefix     = "";
    /// the output stream
    std::shared_ptr<std::ofstream> outputStream = nullptr;

    Config(const std::string&   lname = "ObjSurfaceWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
    {
    }
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  ObjSurfaceWriter(const Config& cfg);

  /// Framework name() method
  std::string
  name() const;

  /// The write interface
  /// @param surface to be written out
  void
  write(const Acts::GeometryContext &gctx, const Acts::Surface &surface);

  /// write a bit of string
  /// @param is the string to be written
  void
  write(const std::string& sinfo);

private:
  Config                  m_cfg;         ///< the config class
  ObjHelper::VtnCounter m_vtnCounter;  ///< vertex, texture, normal
  std::mutex m_write_mutex;  ///< mutex to protect multi-threaded writes

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

}
