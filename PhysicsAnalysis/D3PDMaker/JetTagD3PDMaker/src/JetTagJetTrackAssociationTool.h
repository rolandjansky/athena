/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagJetTrackAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets to tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagJetTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagJetTrackAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;
namespace Rec{
  class TrackParticle;
}

namespace Trk{
  class ITrackToVertexIPEstimator;
  class VxCandidate;
}

namespace D3PD {


/**
 * @brief Associate all track particles inside jets
 *
 */

  
class JetTagJetTrackAssociationTool
  : public MultiAssociationTool<Jet,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagJetTrackAssociationTool (const std::string& type,
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
  const Rec::TrackParticle* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:

  ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimator;

  int *m_signOfIP;
  int *m_signOfZIP;

  float* m_ud0wrtPriVtx;
  float* m_ud0ErrwrtPriVtx;
  float* m_uz0wrtPriVtx;
  float* m_uz0ErrwrtPriVtx;

  const Jet* m_jet;
  const Trk::VxCandidate* m_vertex; 
  std::vector<const Rec::TrackParticle*>* m_tracks;

  std::vector<const Rec::TrackParticle*>::iterator m_trkItr;
  std::vector<const Rec::TrackParticle*>::iterator m_trkEnd;

  std::string m_primaryVertexCollection;
  std::string m_trackAssocName;
  bool m_fillVariables;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetTrackAssociationTool_H
