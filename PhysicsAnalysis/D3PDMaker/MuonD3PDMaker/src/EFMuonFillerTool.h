/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/EFMuonFillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for TrigMuonEF
 */

#ifndef MUOND3PDMAKER_EFMUONFILLERTOOL_H
#define MUOND3PDMAKER_EFMUONFILLERTOOL_H 1


#include "D3PDMakerUtils/BlockFillerTool.h"
class TrigMuonEF;


namespace D3PD {


/**
 * @brief Block filler tool for TrigMuonEF
 */
class EFMuonFillerTool
  : public BlockFillerTool<TrigMuonEF>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EFMuonFillerTool (const std::string& type,
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
  virtual StatusCode fill (const TrigMuonEF& p);


private:
  /// Variable: muon code
  int* m_muonCode;
};


}

#endif 
