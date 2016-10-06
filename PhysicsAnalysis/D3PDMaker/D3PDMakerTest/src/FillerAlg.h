// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/FillerAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Put objects into StoreGate to test D3PD maker.
 */


#ifndef D3PDMAKERTEST_FILLERALG_H
#define D3PDMAKERTEST_FILLERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace D3PDTest {



/**
 * @brief Test class for D3PD maker.
 *
 * This gets put into StoreGate and we fill tuples from it.
 */
class FillerAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  FillerAlg (const std::string& name,
             ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Fill collections involving Obj1/2.
  StatusCode fillObj1Collections();

  /// Fill collections involving Obj3.
  StatusCode fillObj3Collections();

  /// Fill collections involving Obj4.
  StatusCode fillObj4Collections();

  /// Fill collections involving Obj5.
  StatusCode fillObj5Collections();

  std::string m_sgkeyObj1cont;
  std::string m_sgkeyObj1sel;
  std::string m_sgkeyObj1;
  std::string m_sgkeyObj3cont;
  std::string m_sgkeyObj4cont;
  std::string m_sgkeyObj5cont;

  int m_count;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_FILLERALG_H
