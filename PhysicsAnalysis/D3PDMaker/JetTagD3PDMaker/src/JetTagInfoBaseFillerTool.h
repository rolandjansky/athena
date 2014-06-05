/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagInfoBaseFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for the InfoBase object
 */

#ifndef JetTagD3PDMaker_JetTagInfoBaseFillerTool_H
#define JetTagD3PDMaker_JetTagInfoBaseFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetTagInfoBaseFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagInfoBaseFillerTool (const std::string& type,
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
  JetTagFinder m_finder;

  float* m_pu;
  float* m_pb;
  float* m_pc;
  float* m_ptau;
  float* m_weight;
  bool*  m_isValid;

  bool m_addValid;
  bool m_addPc;
  bool m_addPtau;
  bool m_addWeight;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagInfoBaseFillerTool_H
