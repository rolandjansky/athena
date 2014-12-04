// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/ROIthrPatternFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2014
 * @brief xxx
 */


#ifndef TRIGGERD3PDMAKER_ROITHRPATTERNFILLERTOOL_H
#define TRIGGERD3PDMAKER_ROITHRPATTERNFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"



namespace D3PD {


template <class ROI>
class ROIthrPatternFillerTool
  : public BlockFillerTool<ROI>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ROIthrPatternFillerTool (const std::string& type,
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
  virtual StatusCode fill (const ROI& p);


private:
  /// Variable: The pattern describing which thresholds were passed.
  int* m_thrPattern;
};


}


#include "ROIthrPatternFillerTool.icc"


#endif // not TRIGGERD3PDMAKER_ROITHRPATTERNFILLERTOOL_H
