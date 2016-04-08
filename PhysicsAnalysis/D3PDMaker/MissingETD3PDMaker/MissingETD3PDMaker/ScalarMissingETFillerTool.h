// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/ScalarMissingETFillTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for Missing ET objects.
 */


#ifndef MISSINGETD3PPDMAKER_SCALARMISSINGETFILLERTOOL_H
#define MISSINGETD3PPDMAKER_SCALARMISSINGETFILLERTOOL_H

#include "StoreGate/StoreGateSvc.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET objects.
 */
class ScalarMissingETFillerTool
  : public BlockFillerTool<MissingET>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ScalarMissingETFillerTool (const std::string& type,
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
  virtual StatusCode fill (const MissingET& p);

protected:
 MsgStream mLog;

private:
  
 

  /// Variable: met.
  float* m_et;

 
};


} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_SCALARMISSINGETFILLERTOOL_H
