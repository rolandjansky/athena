/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagMultiSVInfoMSVVtxInfoAssociationTool.h
 * @author Georges Aad
 * @date Apr, 2011
 * @brief association from jets MultiSVInfo object to MSVVtxInfo
 * 
 */

#ifndef JetTagD3PDMaker_JetTagMultiSVInfoMSVVtxInfoAssociationTool_H
#define JetTagD3PDMaker_JetTagMultiSVInfoMSVVtxInfoAssociationTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;

namespace Analysis{
  class MultiSVInfoPlus;
  class MSVVtxInfo;
}


namespace D3PD {


/**
 * @brief Associate all MSVVtxInfo which hold the vertex in jet info for the multi sv tagger to jets
 *
 */

  
class JetTagMultiSVInfoMSVVtxInfoAssociationTool
  : public MultiAssociationTool<Jet,Analysis::MSVVtxInfo>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagMultiSVInfoMSVVtxInfoAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Jet& p);


  /**
   * @brief Return a pointer to the next element in the association.
   * Return 0 when the association has been exhausted.
   */
  const Analysis::MSVVtxInfo* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:
  JetTagFinder m_finder;

  const Analysis::MultiSVInfoPlus* m_mSVInfoPlus;

  unsigned int m_vtxItr;
  unsigned int m_vtxEnd;
};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagMultiSVInfoMSVVtxInfoAssociationTool_H
