/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagMultiSVInfoPlusFillerTool.h
 * @author Georges Aad
 * @date Apr, 2011
 * @brief Block filler tool for the MultiSVInfoPlus object
 */

#ifndef JetTagD3PDMaker_JetTagMultiSVInfoPlusFillerTool_H
#define JetTagD3PDMaker_JetTagMultiSVInfoPlusFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet MultiSVInfoPlus object. 
 * Association to MSVVtxInfo is done in JetTagMultiSVInfoMSVVtxInfoAssociationTool.
 */
class JetTagMultiSVInfoPlusFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagMultiSVInfoPlusFillerTool (const std::string& type,
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
  int *m_nVtx;
  float *m_normWeightedDist;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagMultiSVInfoPlusFillerTool_H
