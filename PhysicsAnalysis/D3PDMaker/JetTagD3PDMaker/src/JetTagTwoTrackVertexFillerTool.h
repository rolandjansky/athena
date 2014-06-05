/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagTwoTrackVertexFillerTool.h.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for jet fitter two track vertices
 */

#ifndef JetTagD3PDMaker_JetTagTwoTrackVertexFillerTool_H
#define JetTagD3PDMaker_JetTagTwoTrackVertexFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PD {

  class JetTagTwoTrackVertexCandidateHolder ;

class JetTagTwoTrackVertexFillerTool
  : public BlockFillerTool<JetTagTwoTrackVertexCandidateHolder>
{
public:

  JetTagTwoTrackVertexFillerTool (const std::string& type,
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
  virtual StatusCode fill (const JetTagTwoTrackVertexCandidateHolder& p);
  
  
private:


  float* m_neutral;
  float* m_chi2;
  float* m_ndof;

  float* m_x;
  float* m_y;
  float* m_z;

  float* m_errx;
  float* m_erry;
  float* m_errz;

  float* m_mass;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagTwoTrackVertexFillerTool_H
