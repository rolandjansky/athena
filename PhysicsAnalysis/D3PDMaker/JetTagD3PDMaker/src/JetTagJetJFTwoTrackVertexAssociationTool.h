/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagJetJFTwoTrackVertexAssociationTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief association from jets to JetFitter 2 track vertices
 * 
 */

#ifndef JetTagD3PDMaker_JetTagJetJFTwoTrackVertexAssociationTool_H
#define JetTagD3PDMaker_JetTagJetJFTwoTrackVertexAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTwoTrackVertexCandidateHolder.h"


class Jet;

namespace Trk{
  class VxCandidate;
  //class TrackParticleBase;
}

namespace D3PD {

  
class JetTagJetJFTwoTrackVertexAssociationTool
  : public MultiAssociationTool<Jet,D3PD::JetTagTwoTrackVertexCandidateHolder>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagJetJFTwoTrackVertexAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  typedef MultiAssociationTool<Jet,D3PD::JetTagTwoTrackVertexCandidateHolder> base;


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
  const JetTagTwoTrackVertexCandidateHolder* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:

  const JetTagTwoTrackVertexDataHolder* m_twoTrackDataHolder;

  std::vector<const Trk::VxCandidate*>::const_iterator m_vxIter;
  std::vector<const Trk::VxCandidate*>::const_iterator m_vxEnd;

  std::string m_twoTrackDataName;
  std::string m_infoName;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetJFTwoTrackVertexAssociationTool_H
