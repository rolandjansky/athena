// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauPriVtxFillerTool.h 523229 2012-10-25 16:04:51Z mtm $
/**
 * @file TauD3PDMaker/src/TauPriVtxFillerTool.h
 * @author KG <kong.guan.tan@cern.ch>
 * @date October, 2011
 * @brief Block filler tool for Tau Primary Vertex variables.
 */

#ifndef TAUD3PDMAKER_TAUPRIVTXFILLERTOOL_H
#define TAUD3PDMAKER_TAUPRIVTXFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Analysis { class TauJet; }

class StoreGateSvc; 
//class TauPVTool;

namespace D3PD {


/**
 * @brief Block filler tool for tau track details.
 *
 */
class TauPriVtxFillerTool
  : public BlockFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauPriVtxFillerTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual StatusCode initialize();
                      
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
  float* m_vtx_x;
  float* m_vtx_y;
  float* m_vtx_z;
  float* m_vtx_xx;
  float* m_vtx_yy;
  float* m_vtx_zz;
  float* m_vtx_xy;
  float* m_vtx_yz;
  float* m_vtx_zx;
  float* m_vtx_chiSquared;
  float* m_vtx_numberDoF;
  float* m_vtx_jvf;

  StoreGateSvc* m_storeGate;
  //  ToolHandle<TauPVTool> m_tauPVTool;
};


} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUPRIVTXFILLERTOOL_H
