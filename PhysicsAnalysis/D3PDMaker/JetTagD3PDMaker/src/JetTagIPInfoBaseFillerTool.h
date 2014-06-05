/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagIPInfoBaseFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for the IPInfoBase object
 */

#ifndef JetTagD3PDMaker_JetTagIPInfoBaseFillerTool_H
#define JetTagD3PDMaker_JetTagIPInfoBaseFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetTagIPInfoBaseFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagIPInfoBaseFillerTool (const std::string& type,
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

  int*    m_ntrk;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagIPInfoBaseFillerTool_H
