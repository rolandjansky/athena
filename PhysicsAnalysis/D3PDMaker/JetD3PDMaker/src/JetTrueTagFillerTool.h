// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetFlavorTagComponentsFillerTool.cxx
 * @author Erik Devetak <edevetak@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for Jet BTag related quantities. The class implements most of the code in PhysicsAnalysis/JetTagging/JetTagValidation/JetTagNtuple/tags/JetTagNtuple-00-00-25/src/JetTagNtuple_JetTool.cxx by 
 */

#ifndef JETD3PDMAKER_JETTRUETAGFILLERTOOL_H
#define JETD3PDMAKER_JETTRUETAGFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetTrueTagFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTrueTagFillerTool  (const std::string& type,
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

  double* m_dRminToB;
  double* m_dRminToC;
  double* m_dRminToT;
  int*    m_truepdg; 
  int*    m_iflav; 
};


} // namespace D3PD


#endif 
