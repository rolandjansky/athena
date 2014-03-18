// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/HitsFillerAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Put objects into StoreGate to test D3PD maker.
 *
 * Makes a dummy MDTSimHitsCollection.
 */


#ifndef D3PDMAKERTEST_HITSFILLERALG_H
#define D3PDMAKERTEST_HITSFILLERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace D3PDTest {



/**
 * @brief Test class for D3PD maker.
 *
 * This gets put into StoreGate and we fill tuples from it.
 * Makes a dummy MDTSimHitsCollection.
 */
class HitsFillerAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  HitsFillerAlg (const std::string& name,
                 ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Fill a MDTSimHitCollection
  StatusCode fillMDTSimHitCollection();

  /// Fill a TrackRecordCollection
  StatusCode fillTrackRecordCollection();

  std::string m_MDTSimHitName;
  std::string m_TrackRecordName;

  int m_count;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_HITSFILLERALG_H
