/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagJetFitterGenericTagInfoFillerTool.h
 * @author Georges Aad
 * @date Oct, 2012
 * @brief Block filler tool for the JetFitterGenericTagInfo object
 */

#ifndef JetTagD3PDMaker_JetTagJetFitterGenericTagInfoFillerTool_H
#define JetTagD3PDMaker_JetTagJetFitterGenericTagInfoFillerTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for JetFitterTagInfo object.
 */
class JetTagJetFitterGenericTagInfoFillerTool
  : public BlockFillerTool<Jet>
{
public:

  JetTagJetFitterGenericTagInfoFillerTool (const std::string& type,
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

  std::vector<std::string>* m_namesDouble;
  std::vector<double>* m_valuesDouble;
    
  std::vector<std::string>* m_namesInt;
  std::vector<int>* m_valuesInt;

};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagJetFitterGenericTagInfoFillerTool_H
