// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetVtxFillerTool.h
 * @author Michele Petteni
 * @date Feb, 2010
 * @brief Block filler tool for Jet Associated Vertex
 */

#ifndef JETD3PDMAKER_JETVTXFILLERTOOL_H
#define JETD3PDMAKER_JETVTXFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <string>


class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet associated vertex.
 */
class JetVtxFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetVtxFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);
  
  
private:
  /// Variables: jet vertex position 
  float* m_jvtx_x;
  float* m_jvtx_y;
  float* m_jvtx_z;
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETVTXFILLERTOOL_H
