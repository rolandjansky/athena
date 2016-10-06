// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj2FillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief D3PD maker regression tests.
 */

#ifndef D3PDMAKERTEST_OBJ2FILLERTOOL_H
#define D3PDMAKERTEST_OBJ2FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PDTest {


class Obj2;


/**
 * @brief Block filler tool for TrigElectron calo variables (LOD 2).
 */
class Obj2FillerTool
  : public D3PD::BlockFillerTool<Obj2>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj2FillerTool (const std::string& type,
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
  virtual StatusCode fill (const Obj2& p);


private:
  int*   m_i;
  float* m_f;
};


} // namespace D3PD


#endif // not D3PDMAKERTEST_OBJ2FILLERTOOL_H
