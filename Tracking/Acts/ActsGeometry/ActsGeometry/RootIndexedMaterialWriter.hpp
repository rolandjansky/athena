// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// RootIndexedMaterialWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_ROOT_INDEXEDMATERIALWRITER_H
#define ACTFW_PLUGINS_ROOT_INDEXEDMATERIALWRITER_H

#include <mutex>
#include "Acts/Material/SurfaceMaterial.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "TFile.h"

namespace Acts {

  /// @class RootMaterialTrackWriter
  ///
  /// @brief Writes out MaterialTrack entities from a root file
  ///
  /// This service is the root implementation of the IWriterT.
  /// It writes out a MaterialTrack which is usually generated from
  /// Geant4 material mapping

  class RootIndexedMaterialWriter
  {

  public:
    /// @class Config
    /// Configuration of the Writer
    class Config
    {
    public:
      /// The name of the output tree
      std::string folderNameBase = "Material";
      /// The name of the output file
      std::string fileName;
      /// The default logger
      std::unique_ptr<const Acts::Logger> logger;
      /// The name of the service
      std::string name;

      //Config(Config& cfg)
        //: folderNameBase(cfg.folderNameBase)
        //, fileName(cfg.fileName)
        //, logger(std::move(cfg.logger))
        //, name(cfg.name)
      //{

      //}
      //Config(const std::string&   lname = "MaterialWriter",
             //std::unique_ptr<const Acts::Logger> _logger)
        //: logger(std::move(_logger)), name(lname)
      //{
      //}
    };

    /// Constructor
    RootIndexedMaterialWriter(Config& cfg);

    /// Virtual destructor
    ~RootIndexedMaterialWriter();

    /// Framework name() method
    std::string
    name() const;

    /// Interface method which writes out the MaterialTrack entities
    /// @param ism is the indexed surface material
    void
    write(const Acts::IndexedSurfaceMaterial& ism);

  private:
    /// The config class
    Config m_cfg;
    /// mutex used to protect multi-threaded writes
    std::mutex m_write_mutex;
    /// The output file name
    TFile* m_outputFile;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

  inline std::string
  RootIndexedMaterialWriter::name() const
  {
    return m_cfg.name;
  }

}  // namespace Acts

#endif  // ACTFW_PLUGINS_ROOT_INDEXEDMATERIALWRITER_H
