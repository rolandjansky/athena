/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagQGPartonTruthInfoFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for the jet truth info object
 */

#ifndef JetTagD3PDMaker_JetTagQGPartonTruthInfoFillerTool_H
#define JetTagD3PDMaker_JetTagQGPartonTruthInfoFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


class JetTagQGPartonTruthInfoFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagQGPartonTruthInfoFillerTool (const std::string& type,
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

  int *m_label;
  float *m_drParton;

  std::string m_infoType;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagQGPartonTruthInfoFillerTool_H
