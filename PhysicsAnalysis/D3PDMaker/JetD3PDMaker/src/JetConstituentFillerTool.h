// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetConstituentFillerTool.h
 * @author Michele Petteni < mpetteni AT cern.ch > 
 * @date November, 2009
 * @brief Block filler tool for Jet constituents
 */

#ifndef JETD3PDMAKER_JETCONSTITUENTFILLERTOOL_H
#define JETD3PDMAKER_JETCONSTITUENTFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet EM Fraction.
 */
class JetConstituentFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetConstituentFillerTool (const std::string& type,
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
  /// Variable: Array of Energy Samplings.
  int* m_Ncon;
  std::vector<float>* m_ptcon;
  std::vector<float>* m_econ;
  std::vector<float>* m_etacon;
  std::vector<float>* m_phicon;
  std::vector<float>* m_weightcon;

  int m_sigstate;
  std::string m_prefix;
  
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETSAMPLINGSFILLERTOOL_H
