/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#pragma once

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

