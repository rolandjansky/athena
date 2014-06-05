/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagSoftElecInfoegammaAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets SoftElectronInfo object to egamma
 * 
 */

#ifndef JetTagD3PDMaker_JetTagSoftElecInfoegammaAssociationTool_H
#define JetTagD3PDMaker_JetTagSoftElecInfoegammaAssociationTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;
class egamma;

namespace Analysis{
  class SoftElectronInfo;
}


namespace D3PD {


/**
 * @brief Associate muons in SoftElectronInfo to electron/photon containers
 *
 */

  
class JetTagSoftElecInfoegammaAssociationTool
  : public MultiAssociationTool<Jet,egamma>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagSoftElecInfoegammaAssociationTool (const std::string& type,
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
  const egamma* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:
  JetTagFinder m_finder;

  bool *m_isElectron;
  float *m_d0; 
  float *m_pTRel;

  const Analysis::SoftElectronInfo* m_softElectronInfo;

  int m_egItr;
  int m_egEnd;

  bool m_fillVariables;
};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagSoftElecInfoegammaAssociationTool_H
