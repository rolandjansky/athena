/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagVKalVxInJetBadTrackAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jet VKal vertex (SV vertex) to bad tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagVKalVxInJetBadTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagVKalVxInJetBadTrackAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;
namespace Trk{
  class TrackParticleBase;
}

namespace Rec{
  class TrackParticle;
}

namespace D3PD {


/**
 * @brief Associate all track particles inside jets
 *
 */

  
class JetTagVKalVxInJetBadTrackAssociationTool
  : public MultiAssociationTool<Jet,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagVKalVxInJetBadTrackAssociationTool (const std::string& type,
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

  std::vector<const Trk::TrackParticleBase*> m_holder;

  std::vector<const Trk::TrackParticleBase*>::const_iterator m_trkItr;
  std::vector<const Trk::TrackParticleBase*>::const_iterator m_trkEnd;

  std::string m_infoType;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagVKalVxInJetBadTrackAssociationTool_H
