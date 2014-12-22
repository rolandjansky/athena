// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj5FillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief D3PD maker regression tests.
 */

#ifndef D3PDMAKERTEST_OBJ5FILLERTOOL_H
#define D3PDMAKERTEST_OBJ5FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PDTest {


class Obj5;


/**
 * @brief D3PD maker regression tests.
 */
class Obj5FillerTool
  : public D3PD::BlockFillerTool<Obj5>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj5FillerTool (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Obj5& p);


private:
  /// Variables.
  int* m_obj5;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_OBJ5FILLERTOOL_H
