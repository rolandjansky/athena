/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagJetNavigable4MomentumAssociationTool.h
 * @author Georges Aad
 * @date July, 2012
 * @brief association from jets to any NavP4 object (TruthParticle)
 * 
 */

#ifndef JetTagD3PDMaker_JetTagJetNavigable4MomentumAssociationTool_H
#define JetTagD3PDMaker_JetTagJetNavigable4MomentumAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetEvent/JetINav4MomAssociation.h"
#include "McParticleEvent/TruthParticle.h"

class Jet;
//class INavigable4Momentum;

namespace D3PD {


/**
 * @brief Associate all track particles inside jets
 *
 */

  
class JetTagJetNavigable4MomentumAssociationTool
  : virtual public MultiAssociationTool<Jet,TruthParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagJetNavigable4MomentumAssociationTool (const std::string& type,
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
  const TruthParticle* next();



private:

  JetINav4MomAssociation::object_iter m_navItr; 
  JetINav4MomAssociation::object_iter m_navEnd; 

  std::string m_navAssocName;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetNavigable4MomentumAssociationTool_H
