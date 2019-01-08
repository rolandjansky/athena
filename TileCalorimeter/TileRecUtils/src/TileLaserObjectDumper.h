// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/TileLaserObjectDumper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Dump a TileLaserObject to a text file.
 */


#ifndef TILERECUTILS_TILELASEROBJECTDUMPER_H
#define TILERECUTILS_TILELASEROBJECTDUMPER_H


#include "TileEvent/TileLaserObject.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"


/**
 * @brief Dump a TileLaserObject to a text file.
 *
 * We write one file per object, putting the key and run/event numbers in the
 * file name (for tests with MT jobs).
 */
class TileLaserObjectDumper
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TileLaserObjectDumper (const std::string& name, ISvcLocator* svcloc);


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
  /// Handle to access input object.
  SG::ReadHandleKey<TileLaserObject> m_contKey
    { this, "TileLaserObject", "TileLaserObj", "Handle for input laser objects." };

  /// Output file name prefix.
  Gaudi::Property<std::string> m_prefix
    { this, "Prefix", "", "Prefix for output file name." };
};


#endif // not TILERECUTILS_TILELASEROBJECTDUMPER_H
