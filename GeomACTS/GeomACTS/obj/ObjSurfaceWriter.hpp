// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OBJ_PLUGINS_SURFACEWRITER_H
#define ACTFW_OBJ_PLUGINS_SURFACEWRITER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include "GeomACTS/obj/ObjHelper.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/Logger.hpp"


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
    unsigned int outputPhiSegemnts = 72;
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
  write(const Acts::Surface& surface);

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


#endif  // ACTFW_OBJ_PLUGINS_SURFACEWRITER_H
