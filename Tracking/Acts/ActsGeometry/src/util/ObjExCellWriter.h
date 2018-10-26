/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#pragma once

#include <TTree.h>
#include <mutex>
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Utilities/Logger.hpp"



  /// @class ExtrapolationCellWriter
  ///
  /// A obj based implementation to write out extrapolation steps
  /// it is used to write obj representations of tracks for displaying
  ///
  /// Safe to use from multiple writer threads.
  ///
  template <class T>
  class ObjExCellWriter 
  {
  public:
    // The nested configuration struct
    struct Config
    {
    public:
      /// output scalor
      double outputScalor = 1.;
      /// precision for out
      size_t outputPrecision = 4;
      /// the output stream
      std::shared_ptr<std::ofstream> outputStream = nullptr;
    };

    /// Constructor
    ///
    /// @param cfg is the configuration object
    /// @parm level is the output logging level
    ObjExCellWriter(const Config&        cfg
        //Acts::Logging::Level lvl = Acts::Logging::INFO
        )
    : m_cfg(cfg), m_vCounter(1)
    {
      if (!m_cfg.outputStream) {
        throw std::invalid_argument("Missing output stream");
      }
    }

    /// The protected writeT method, called by the WriterT base
    /// @param ctx is the algorithm context for event consistency
    void
    write(const std::vector<Acts::ExtrapolationCell<T>>& ecells) {

      // loop over the cells
      for (auto& eCell : ecells) {
        // remember the first counter
        size_t fCounter = m_vCounter;

        // increase the vertex counter
        ++m_vCounter;
        // the event paramters
        auto sPosition = eCell.startParameters->position();
        // write the space point
        (*(m_cfg.outputStream))
          << "v " << m_cfg.outputScalor * sPosition.x() << ", "
          << m_cfg.outputScalor * sPosition.y() << ", "
          << m_cfg.outputScalor * sPosition.z() << '\n';

        // loop over extrapolation steps
        for (auto& es : eCell.extrapolationSteps) {
          if (es.parameters) {
            /// step parameters
            const T& pars      = (*es.parameters);
            auto     tPosition = pars.position();
            // increase the counter
            ++m_vCounter;
            // write the space point
            (*(m_cfg.outputStream))
              << "v " << m_cfg.outputScalor * tPosition.x() << ", "
              << m_cfg.outputScalor * tPosition.y() << ", "
              << m_cfg.outputScalor * tPosition.z() << '\n';
          }
        }
        // write out the line
        for (size_t i=fCounter+1;i<m_vCounter;i++) {
          (*(m_cfg.outputStream)) << "l ";
          (*(m_cfg.outputStream)) << (i-1) << " " << i;
          (*(m_cfg.outputStream)) << '\n';
        }
        //(*(m_cfg.outputStream)) << "l ";
        //for (size_t iv = fCounter; iv < m_vCounter; ++iv)
          //(*(m_cfg.outputStream)) << iv << " ";

        //(*(m_cfg.outputStream)) << '\n';
        // new line
        (*(m_cfg.outputStream)) << '\n';
      }
    }

  private:
    Config m_cfg;
    size_t m_vCounter;
};

