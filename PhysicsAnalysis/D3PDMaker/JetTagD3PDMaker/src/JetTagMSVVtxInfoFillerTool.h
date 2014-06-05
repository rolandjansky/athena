/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagMSVVtxInfoFillerTool.h
 * @author Georges Aad
 * @date Apr, 2011
 * @brief Block filler tool for the MSVVtxInfo object
 */

#ifndef JetTagD3PDMaker_JetTagMSVVtxInfoFillerTool_H
#define JetTagD3PDMaker_JetTagMSVVtxInfoFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis{
class MSVVtxInfo;
}


namespace D3PD {


/**
 * @brief Block filler tool for Jet SVInfoPlus object. 
 * Association to track is done in JetTagMSVVtxInfoTrackAssociationTool.
 */
class JetTagMSVVtxInfoFillerTool
  : public BlockFillerTool<Analysis::MSVVtxInfo>
{
public:

  JetTagMSVVtxInfoFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Analysis::MSVVtxInfo& p);
  
  
private:

  float *m_mass;
  float *m_pt;
  float *m_eta;
  float *m_phi;
  float *m_eFrac;
  float *m_x;
  float *m_y;
  float *m_z;
  float *m_err_x;
  float *m_err_y;
  float *m_err_z;
  float *m_cov_xy;
  float *m_cov_xz;
  float *m_cov_yz;
  float *m_chi2;
  int *m_ndof;        
  int *m_ntrk;
  float *m_normDist;

};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagMSVVtxInfoFillerTool_H
