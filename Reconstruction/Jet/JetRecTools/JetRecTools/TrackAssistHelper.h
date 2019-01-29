/* 
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TrackAssistHelper.h

#ifndef TRACKASSISTHELPER_H
#define TRACKASSISTHELPER_H

// Jason Veatch (jason.veatch@cern.ch)
// November 2018
//
// Base class for tools that implement track-assisted jet substructure algorithms

#include "AsgTools/ToolHandle.h"
#include "AsgTools/Check.h"
#include "AsgTools/MessageCheck.h"

#include "xAODCore/ShallowCopy.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "JetInterface/IJetTrackSelector.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODBase/IParticleHelpers.h"

using namespace asg::msgUserCode;

class TrackAssistHelper
{

  public:

    // Destructor.
    virtual ~TrackAssistHelper() noexcept = default;

    // Rescale tracks using jets with pre-determined scaling weights
    StatusCode rescaleTracks( const xAOD::JetContainer *jets, xAOD::TrackParticleContainer *tracks) const;

  protected:

    // Configurable parameters
    std::string m_assocTracksInName;
    std::string m_vertexColl;
    std::string m_trackVtxAssoc;
    bool m_doTrackVtxAssoc;
    ToolHandle<IJetTrackSelector> m_trackSelTool;

    // Local method to check if track passes selection and vertex association criteria
    bool isGoodTrack( const xAOD::TrackParticle &track, const xAOD::Vertex &pvx, const jet::TrackVertexAssociation &tva ) const;

    // Constructor 
    TrackAssistHelper();

    // Initialize function
    virtual StatusCode initialize();

    // Print configured parameters
    virtual void print() const;

    // Local method to retrieve all vertex information
    StatusCode getVertexInfo( const xAOD::Vertex *&pvx, const jet::TrackVertexAssociation *&tva ) const;

    // Local method to retrieve the track vertex association
    virtual StatusCode getTrackVertexAssociation( const jet::TrackVertexAssociation *&tva ) const = 0;

    // Local method to retrieve the primary vertex
    virtual StatusCode getPrimaryVertex( const xAOD::Vertex *&pvx ) const = 0;

  private:

    // Local method to calculate the factors to rescale each track
    StatusCode getRescaleFactors( const xAOD::Jet &jet, xAOD::TrackParticleContainer &tracks) const;

};

#endif // TRACKASSISTHELPER_H
