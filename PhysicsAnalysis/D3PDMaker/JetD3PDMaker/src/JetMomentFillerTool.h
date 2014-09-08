// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetMomentFillerTool.h
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date October, 2009
 * @brief Block filler tool for Jet Moments
 */

#ifndef JETD3PDMAKER_JETMOMENTFILLERTOOL_H
#define JETD3PDMAKER_JETMOMENTFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet Moments.
 */
class JetMomentFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetMomentFillerTool (const std::string& type,
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
  /// Variable: Jet EM Fraction.
  std::vector<float*> m_momentValues;
  
  /// Propert: List of moments to write
  std::vector<std::string> m_momentList;
  bool m_timingMomentsOnlyPosE;

  double  calculateOotFracMoment(const Jet* jet, std::string momentName);
  
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETMOMENTFILLERTOOL_H
