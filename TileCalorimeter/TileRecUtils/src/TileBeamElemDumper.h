// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/TileBeamElemDumper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Dump a TileBeamElemContainer to a text file.
 */


#ifndef TILERECUTILS_TILEBEAMELEMDUMPER_H
#define TILERECUTILS_TILEBEAMELEMDUMPER_H


#include "TileEvent/TileBeamElemContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"


/**
 * @brief Dump a TileBeamElemContainer to a text file.
 *
 * We write one file per container, putting the key and run/event numbers in the
 * file name (for tests with MT jobs).
 */
class TileBeamElemDumper
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TileBeamElemDumper (const std::string& name, ISvcLocator* svcloc);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   * @param ctx Event Context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// Handle to access input tile digits.
  SG::ReadHandleKey<TileBeamElemContainer> m_contKey
    { this, "TileBeamElemContainer", "TileBeamElemCnt", "Handle for input tile beam info." };

  /// Output file name prefix.
  Gaudi::Property<std::string> m_prefix
    { this, "Prefix", "", "Prefix for output file name." };
};


#endif // not TILERECUTILS_TILEBEAMELEMDUMPER_H
