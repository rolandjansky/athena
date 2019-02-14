/* 
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// SATScaleTool.h

#ifndef SATSCALETOOL_H
#define SATSCALETOOL_H

// Jason Veatch (jason.veatch@cern.ch)
// November 2018
//
// Tool to produce rescaled tracks to be used as inputs to SAT jets
// The output is a xAOD::TrackParticle collection

#include "JetInterface/IJetExecuteTool.h"
#include "AsgTools/AsgTool.h"

#include "AthContainers/ConstDataVector.h"

#include "JetRecTools/TrackAssistHelper.h"

class SATScaleTool 
: public IJetExecuteTool,
  public asg::AsgTool,
  public TrackAssistHelper
{
  ASG_TOOL_CLASS(SATScaleTool, IJetExecuteTool)

  public:

    // Constructor
    SATScaleTool(const std::string& myname = "SATScaleTool" );

    // Initialize function
    StatusCode initialize() override;

    // Print all configurable parameters
    void print() const override;

    // Execute the tool
    int execute() const override;

  private:

    // Produce collection of rescaled tracks
    StatusCode makeSATTracks() const;

    // Local method to retrieve the track vertex association
    StatusCode getTrackVertexAssociation( const jet::TrackVertexAssociation *&tva ) const override;

    // Local method to retrieve the primary vertex
    StatusCode getPrimaryVertex( const xAOD::Vertex *&pvx ) const override;

    // If jet merging is turned on, flag jets that meet the merging criteria
    StatusCode flagJetsToMerge( const xAOD::JetContainer *jets ) const;

    // Configurable parameters
    std::string m_inTrackColl; 
    std::string m_inJetColl; 
    std::string m_outTrackColl; 
    float m_dRmatch;
    bool m_doMerge;
    float m_dRmerge;
    float m_pTdRmerge;

};

#endif // SATSCALETOOL_H
