/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagBTagWeightsFillerTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief Fill BTagging weights
*/


#ifndef JetTagD3PDMaker_JetTagBTagWeightsFillerTool_H
#define JetTagD3PDMaker_JetTagBTagWeightsFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {

/**
 * @brief Fill BTagging weights
 */

class JetTagBTagWeightsFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagBTagWeightsFillerTool (const std::string& type,
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

  void clearData();

  // variables:

  std::vector<float*> m_weights;
  std::vector<std::string> m_taggerNames;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagBTagWeightsFillerTool_H
