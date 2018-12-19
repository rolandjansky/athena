// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/TileMuonReceiverDumper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Dump a TileMuonReceiverContainer to a text file.
 */


#ifndef TILERECUTILS_TILEMUONRECEIVERDUMPER_H
#define TILERECUTILS_TILEMUONRECEIVERDUMPER_H


#include "TileEvent/TileMuonReceiverContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"


/**
 * @brief Dump a TileMuonReceiverContainer to a text file.
 *
 * We write one file per container, putting the key and run/event numbers in the
 * file name (for tests with MT jobs).
 */
class TileMuonReceiverDumper
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TileMuonReceiverDumper (const std::string& name, ISvcLocator* svcloc);


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
  SG::ReadHandleKey<TileMuonReceiverContainer> m_contKey
    { this, "TileMuonReceiverContainer", "TileMuRcvCnt", "Handle for input tile MuonReceiver objects." };

  /// Output file name prefix.
  Gaudi::Property<std::string> m_prefix
    { this, "Prefix", "", "Prefix for output file name." };
};


#endif // not TILERECUTILS_TILEMUONRECEIVERDUMPER_H
