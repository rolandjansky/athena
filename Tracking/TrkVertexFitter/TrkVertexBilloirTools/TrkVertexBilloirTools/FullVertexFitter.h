/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERBILLOIR_FULLVERTEXFITTER_H
#define TRKVERTEXFITTERBILLOIR_FULLVERTEXFITTER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/VertexFwd.h" 
#include "xAODTracking/TrackParticleFwd.h" 


namespace Trk
{
  class Vertex;
  class RecVertex;
  class VxCandidate;
  class Track;
  class TrackParticleBase;
  class IExtrapolator;
  class IVertexLinearizedTrackFactory;
  class IVxCandidateXAODVertex; 
  
  /** This class implements a full vertex fitting algorithm as proposed by P. Billoir.
  this algorithm tries to estimate the vertex position with refitting the track
  parameters. */
  class FullVertexFitter : public AthAlgTool, virtual public IVertexFitter
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    enum FitError {FITOK,MATINV,NEGTRCHI2,MAXCHI2,MAXTRCHI2,NOTRKS,NOFIT};
    FullVertexFitter(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~FullVertexFitter(); //!< standard destructor

    /** Interface for Track with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::Track*> & vectorTrk,
                              const Vertex& startingPoint);

    /** Interface for Track with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk,
                              const RecVertex& constraint);

    /** Interface for TrackParticleBase with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                              const Vertex& startingPoint);

    /** Interface for TrackParticleBase with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                              const RecVertex& constraint);

    /** Interface for ParametersBase with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                              const Vertex& startingPoint);

    /** Interface for ParametersBase with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                              const RecVertex& constraint);
    
    /**
    * Fit interface with no starting point or constraint. (0,0,0) will be assumed.
    */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*>& perigeeList);

    virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk);


    /**  	     *Interface for xAOD::TrackParticle with starting point  */ 
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const Vertex& startingPoint); 
    /**  	     * Interface for xAOD::TrackParticle with vertex constraint	
		     
     * the position of the constraint is ALWAYS the starting point  	     */ 
    
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const RecVertex& constraint); 
    
  private:
    unsigned int    m_maxIterations;
    double m_maxDchi2PerNdf;

    /** Data members to store the results */
    FitError                m_error;

    ToolHandle< Trk::IExtrapolator > m_extrapolator;
    ToolHandle< Trk::IVertexLinearizedTrackFactory > m_linFactory;
    ToolHandle< IVxCandidateXAODVertex > m_xaodConverter; 
  };
}
#endif



