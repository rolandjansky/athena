// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetShapeFillerTool.h
 * @author Renaud Bruneliere < Renaud.Bruneliere AT cern.ch > 
 * @date April, 2010
 * @brief Block filler tool for Jet shapes
 * @brief Jet shapes are defined section 5.2 of ATL-COM-PHYS-2010-125 
 */

#ifndef JETD3PDMAKER_JETSHAPEFILLERTOOL_H
#define JETD3PDMAKER_JETSHAPEFILLERTOOL_H

#include "GaudiKernel/MsgStream.h"

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet shapes.
 */
class JetShapeFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetShapeFillerTool (const std::string& type,
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
  
protected:
  MsgStream m_log;
  
private:
  /// Property: DeltaR of jet shape annulus
  float m_deltaR;

  /// Property: Maximum radius R
  float m_Rmax;

  /// Property: Should we normalize to annulus area ?
  bool m_do_normToA;

  /// Property: Signal state Jets will be reverted to.
  int m_sigstate;

  /// Property: Prefix applied to FourMom variable names
  std::string m_prefix;

  /// Variable: Array of average jet transverse momentum.
  std::vector<float>* m_shape;

  /// Annulus area
  std::vector<float> m_shapeA;  
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETSHAPEFILLERTOOL_H
