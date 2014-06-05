/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagSVInfoPlusFillerTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief Block filler tool for the SVInfoPlus object
 */

#ifndef JetTagD3PDMaker_JetTagSVInfoPlusFillerTool_H
#define JetTagD3PDMaker_JetTagSVInfoPlusFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet SVInfoPlus object. 
 * Association to track is done in JetTagSVInfoPlusTrackAssociationTool.
 */
class JetTagSVInfoPlusFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagSVInfoPlusFillerTool (const std::string& type,
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

  bool *m_isValid;
  int *m_ntrkVx;
  int *m_ntrkJet;
  int *m_n2trkVx;
  float *m_mass ;
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
  float *m_norm_dist; 
  int *m_ndof;        
  int *m_ntrk;

  bool m_addNormDist; 

};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagSVInfoPlusFillerTool_H
