// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj12FillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief D3PD maker regression tests.
 */


#ifndef D3PDMAKERTEST_OBJ12FILLERTOOL_H
#define D3PDMAKERTEST_OBJ12FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PDTest {


class Obj1;
class Obj2;

/**
 * @brief D3PD maker regression tests.
 */
class Obj12FillerTool
  : public D3PD::BlockFillerTool<D3PD::Types<Obj1, Obj2> >
{
public:
  typedef D3PD::BlockFillerTool<D3PD::Types<Obj1, Obj2> > Base;
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj12FillerTool (const std::string& type,
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
  virtual StatusCode fill (const Obj1& p);


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
  /// Variables.
  int                *m_mult_i;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_OBJ12FILLERTOOL_H
