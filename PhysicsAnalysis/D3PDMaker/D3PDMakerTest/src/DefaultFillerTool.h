// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/DefaultFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Test setting default values in d3pds.
 */


#ifndef D3PDMAKERTEST_DEFAULTFILLERTOOL_H
#define D3PDMAKERTEST_DEFAULTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PDTest {


/**
 * @brief Test setting default values in d3pds.
 */
class DefaultFillerTool
  : public D3PD::BlockFillerTool<void>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  DefaultFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill();


private:
  /// Variables.
  char               *m_c1;
  char               *m_c2;
  unsigned char      *m_uc1;
  unsigned char      *m_uc2;
  short              *m_s1;
  short              *m_s2;
  unsigned short     *m_us1;
  unsigned short     *m_us2;
  int                *m_i1;
  int                *m_i2;
  unsigned int       *m_ui1;
  unsigned int       *m_ui2;
  float              *m_f1;
  float              *m_f2;
  double             *m_d1;
  double             *m_d2;
  bool               *m_b1;
  bool               *m_b2;
};


} // namespace D3PDTest



#endif // not D3PDMAKERTEST_DEFAULTFILLERTOOL_H
