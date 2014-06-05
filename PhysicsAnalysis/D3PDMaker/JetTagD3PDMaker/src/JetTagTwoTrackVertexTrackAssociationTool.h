/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagTwoTrackVertexTrackAssociationTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief association from two track vertices axis to tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagTwoTrackVertexTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagTwoTrackVertexTrackAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTwoTrackVertexCandidateHolder.h"

namespace Rec{
  class TrackParticle;
}

namespace Trk{
  class VxTrackAtVertex;
}


namespace D3PD {

  class JetTagTwoTrackVertexCandidateHolder;

class JetTagTwoTrackVertexTrackAssociationTool
  : public MultiAssociationTool<D3PD::JetTagTwoTrackVertexCandidateHolder,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagTwoTrackVertexTrackAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  typedef MultiAssociationTool<D3PD::JetTagTwoTrackVertexCandidateHolder,Rec::TrackParticle> base;


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const JetTagTwoTrackVertexCandidateHolder& p);


  /**
   * @brief Return a pointer to the next element in the association.
   * Return 0 when the association has been exhausted.
   */
  const Rec::TrackParticle* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:

 std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkItr;
 std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkEnd;


};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagTwoTrackVertexTrackAssociationTool_H
