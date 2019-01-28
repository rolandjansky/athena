/* 
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TARJetTool.h

#ifndef TARJETTOOL_H
#define TARJETTOOL_H

// Jason Veatch (jason.veatch@cern.ch)
// November 2018
//
// Tool to add rescaled tracks to reclustered jets in order to add TAR jet substructure moments
// The output is a new collection of xAOD::TrackParticle and ElementLinks for each jet

#include "JetInterface/IJetModifier.h"
#include "AsgTools/AsgTool.h"

#include "AthContainers/ConstDataVector.h"

#include "JetRecTools/TrackAssistHelper.h"

class TARJetTool 
: public IJetModifier,
  public asg::AsgTool,
  public TrackAssistHelper
{
  ASG_TOOL_CLASS(TARJetTool, IJetModifier)

  public:

    // Constructor
    TARJetTool(const std::string& myname = "TARJetTool" );

    // Initialize function
    StatusCode initialize() override;

    // Print all configurable parameters 
    void print() const override;

    // Modify jet collection
    int modify( xAOD::JetContainer& inJets ) const override;

  private:

    // Local method to retrieve the track vertex association
    StatusCode getTrackVertexAssociation( const jet::TrackVertexAssociation *&tva ) const override;

    // Local method to retrieve the primary vertex
    StatusCode getPrimaryVertex( const xAOD::Vertex *&pvx ) const override;

    // Configurable parameters
    std::string m_inTrackColl;
    std::string m_outTrackColl;
    std::string m_assocTracksOutName;
    float m_dRmatch;

};

#endif // TARJETTOOL_H
