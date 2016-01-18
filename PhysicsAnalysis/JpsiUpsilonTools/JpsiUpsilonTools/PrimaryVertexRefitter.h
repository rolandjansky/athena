/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// PrimaryVertexRefitter header file
//
// James Catmore <James.Catmore@cern.ch>
// Evelina Bouhova-Thacker <e.bouhova@cern.ch>
//
// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef PRIMARYVERTEXREFITTER_H
#define PRIMARYVERTEXREFITTER_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"


namespace Trk { class ITrackToVertexIPEstimator; }

namespace Rec { class TrackParticle; }

namespace Analysis {

static const InterfaceID IID_PrimaryVertexRefitter("PrimaryVertexRefitter", 1, 0);

class PrimaryVertexRefitter:  virtual public AthAlgTool {
public:
        PrimaryVertexRefitter(const std::string& t, const std::string& n, const IInterface*  p);
        ~PrimaryVertexRefitter();
        StatusCode initialize();
        StatusCode finalize();

	static const InterfaceID& interfaceID() { return IID_PrimaryVertexRefitter;};

        //if ReturnCopy is true the method will return a copy of the original vertex if the fit cannot be changed
        //if Returncopy is false the method returns a null
  	const xAOD::Vertex* refitVertex(const xAOD::Vertex* vertex, const xAOD::Vertex* excludeVertex, bool ReturnCopy = true);
 	const xAOD::Vertex* refitVertex(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*> &tps, bool ReturnCopy = true);
        int getLastExitCode() { return m_lastExitCode;}
private:
        unsigned int m_ntrk_min;
        ToolHandle <Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimator;
        int m_lastExitCode;

};
} // end of namespace
#endif

