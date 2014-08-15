// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/LArCollisionTimeFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Block filler tool for collision time information.
 */


#ifndef LARCOLLISIONTIMEFILLERTOOL_H
#define LARCOLLISIONTIMEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class LArCollisionTime;


namespace D3PD {


/**
 * @brief Block filler tool for collision time information.
 */
class LArCollisionTimeFillerTool
  : public BlockFillerTool<LArCollisionTime>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LArCollisionTimeFillerTool (const std::string& type,
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
  virtual StatusCode fill (const LArCollisionTime& c);


private:
  /// Variable: Number of A-side cells for time calculation.
  int* m_ncellA;

  /// Variable: Number of C-side cells for time calculation.
  int* m_ncellC;

  /// Variable: Total A-side energy.
  float* m_energyA;

  /// Variable: Total C-side energy.
  float* m_energyC;

  /// Variable: A-side time.
  float* m_timeA;

  /// Variable: C-side time.
  float* m_timeC;

  /// Variable: A-C LAr cell time difference.
  float* m_timeDiff;
};


} // namespace D3PD


#endif // not LARCOLLISIONTIMEFILLERTOOL_H
