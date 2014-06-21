// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.h
 * @date December, 2009
 * @brief Block filler tool for TauCellsDetails variables.
 */

#ifndef TAUD3PDMAKER_TAURADIUSFILLERTOOL_H
#define TAUD3PDMAKER_TAURADIUSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Block filler tool for tau track details.
 *
 */
class TauRadiusFillerTool
  : public BlockFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauRadiusFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Analysis::TauJet& p);


private:
  float* m_emrademscale;
  float* m_centfracemscale;
  float* m_isofracemscale;
  float* m_trkavgdist;

};


} // namespace D3PD


#endif // 
