// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj1FillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief D3PD maker regression tests.
 */

#ifndef D3PDMAKERTEST_OBJ1FILLERTOOL_H
#define D3PDMAKERTEST_OBJ1FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PDTest {


class Obj1;


/**
 * @brief D3PD maker regression tests.
 */
class Obj1FillerTool
  : public D3PD::BlockFillerTool<Obj1>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj1FillerTool (const std::string& type,
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


private:
  /// Variables.
  //std::string m_str;
  //const char* m_cstr;
  char               *m_c;
  unsigned char      *m_uc;
  short              *m_s;
  unsigned short     *m_us;
  int                *m_i;
  unsigned int       *m_ui;
  float              *m_f;
  double             *m_d;
  //long long          *m_ll; // problem???
  //unsigned long long *m_ull; // problem???
  bool               *m_b;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_OBJ1FILLERTOOL_H
