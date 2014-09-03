/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_TRACKVERTEXASSOCIATIONTOOL_H
#define JETRECTOOLS_TRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/AsgTool.h"

#include "JetInterface/IJetExecuteTool.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

#include <string>

class TrackVertexAssociationTool :  virtual public asg::AsgTool,
                                    virtual public IJetExecuteTool
{
    ASG_TOOL_CLASS(TrackVertexAssociationTool,IJetExecuteTool)

    public:
        TrackVertexAssociationTool(const std::string & t);
        
        // Constuct a TrackVertexAssociation object
        const jet::TrackVertexAssociation* buildTrackVertexAssociation(const xAOD::TrackParticleContainer*, const xAOD::VertexContainer*) const;

        // Retrieve necessary inputs, call building function, store result in evtStore
        int execute() const;

    private:

        // Configurable parameters
        std::string m_trackContainer;
        std::string m_vertexContainer;
        std::string m_tvaStoreName;
        float m_transDistMax;
        float m_longDistMax;
};


#endif
