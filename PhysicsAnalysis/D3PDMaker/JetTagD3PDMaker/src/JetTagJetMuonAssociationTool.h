/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagJetMuonAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets to muons
 * 
 */

#ifndef JetTagD3PDMaker_JetTagJetMuonAssociationTool_H
#define JetTagD3PDMaker_JetTagJetMuonAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIDEvent/MuonAssociation.h"

class Jet;


namespace D3PD {


/**
 * @brief Associate muons inside jets
 *
 */

  
class JetTagJetMuonAssociationTool
  : public MultiAssociationTool<Jet,Analysis::Muon>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagJetMuonAssociationTool (const std::string& type,
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
  const Analysis::Muon* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:

  Navigable<Analysis::MuonContainer,double>::object_iter m_muItr;
  Navigable<Analysis::MuonContainer,double>::object_iter m_muEnd;

  std::string m_muonsName;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetMuonAssociationTool_H
