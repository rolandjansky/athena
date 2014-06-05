/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagVxOnJetAxisFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for jet fitter vertices on jet axis
 */

#ifndef JetTagD3PDMaker_JetTagVxOnJetAxisFillerTool_H
#define JetTagD3PDMaker_JetTagVxOnJetAxisFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PD {

  class JetTagVxOnJetAxisCandidateHolder;

class JetTagVxOnJetAxisFillerTool
  : public BlockFillerTool<JetTagVxOnJetAxisCandidateHolder>
{
public:

  JetTagVxOnJetAxisFillerTool (const std::string& type,
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
  virtual StatusCode fill (const JetTagVxOnJetAxisCandidateHolder& p);
  
  
private:

  float* m_vtxPos;
  float* m_vtxErr;

};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagVxOnJetAxisFillerTool_H
