/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagHadFlavorTagInfoFillerTool.h
 * @author Georges Aad
 * @date May, 2012
 * @brief Block filler tool for the HadFlavorTagInfo object
 */

#ifndef JetTagD3PDMaker_JetTagHadFlavorTagInfoFillerTool_H
#define JetTagD3PDMaker_JetTagHadFlavorTagInfoFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetTagHadFlavorTagInfoFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagHadFlavorTagInfoFillerTool (const std::string& type,
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

  int *m_nMatchingTracks;
  double *m_trkJetWidth;
  double *m_trkJetDRKtaxes;
};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagHadFlavorTagInfoFillerTool_H
