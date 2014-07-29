/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkV0VertexFitter.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////
/**
 *  A first concrete implementation of the IV0VertexFitter
 *  for the purposes of V0 fitting
 *
 *  @authors:   E. Bouhova-Thacker <e.bouhova@cern.ch>, R. Henderson <Rob.Henderson@cern.ch> 
 *  @created:   14 December 2006
 *  @modified:  
 *
 */

#ifndef TRKVOFITTER_TRKV0VERTEXFITTER_H
#define TRKV0FITTER_TRKV0VERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

namespace Trk
{
  class Vertex;
  class RecVertex;
  class ExtendedVxCandidate;
  class VxCandidate;
  class Track;
  class TrackParticleBase;
  class IExtrapolator;
  class IVxCandidateXAODVertex;

  /** This class implements a vertex fitting algorithm optimised for V0 finding.
  The algorithm fits the full track information, incorporating the
  possibility of performing full kinematic constraints at the same time.
  The full covariance matrix from the fit, including track-track and track-vertex
  correlations is calculated and returned via Trk::ExtendedVxCandidate */

  class TrkV0VertexFitter : public AthAlgTool, virtual public IVertexFitter
  {
  public:
  
    StatusCode initialize();
    StatusCode finalize();

    enum FitError {FITOK,MATINV,NEGTRCHI2,MAXCHI2,MAXTRCHI2,NOTRKS,NOFIT};
    TrkV0VertexFitter(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~TrkV0VertexFitter(); //!< standard destructor

    /** Interface for Track with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::Track*> & vectorTrk,
                              const Vertex& startingPoint);

    /** Interface for Track with RecVertex starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::Track*> & vectorTrk,
                              const RecVertex& constraint);

    /** Interface for TrackParticleBase with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                              const Vertex& startingPoint);

    /** Interface for TrackParticleBase with RecVertex starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                              const RecVertex& constraint);

    /** Interface for TrackParameters with starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                              const Vertex& startingPoint);

    /** Interface for TrackParameters and NeutralParameters with starting point */
    VxCandidate * fit(const std::vector<const TrackParameters*> & perigeeList,
		      const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/,
		      const Vertex& startingPoint) {msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&,const Vertex&) ignoring neutrals" << endreq; return fit(perigeeList, startingPoint);};
 

    /** Interface for TrackParameters with RecVertex starting point */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                              const RecVertex& constraint);

    /** Interface for TrackParameters and NeutralParameters with RecVertex starting point */
    VxCandidate * fit(const std::vector<const TrackParameters*> & perigeeList,
		      const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/,
		      const RecVertex& constraint){msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&,const RecVertex&) ignoring neutrals" << endreq; return fit(perigeeList, constraint);};

    
    /**
    * Fit interface with no starting point or constraint. (0,0,0) will be assumed.
    */
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*>& perigeeList);

    VxCandidate * fit(const std::vector<const TrackParameters*>& perigeeList,const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/){msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&) ignoring neutrals" << endreq; return fit(perigeeList);};

    virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk);

    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk);

    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const Vertex& startingPoint);

    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const RecVertex& constraint);

    xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
		       const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
		       const Vertex& startingPoint){msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const TrackParticle*>&,const std::vector<const Trk::NeutralParticle*>&,const Vertex&) ignoring neutrals" << endreq; return fit(vectorTrk, startingPoint);}; 
    xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
		       const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
		       const RecVertex& constraint){msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const TrackParticle*>&,const std::vector<const Trk::NeutralParticle*>&,const RecVertex&) ignoring neutrals" << endreq; return fit(vectorTrk, constraint);}; 

    
   /**
    * Methods specific for the V0 fitter
    * Method taking a vector of tracks, vector of masses  and  starting 
    * point as arguments.
    * Action: Fits a vertex out of initial set of tracks and applies a mass
    * constraint to the particle reconstructed at the vertex and a pointing
    * constraint: in the case the pointingVertex is a zero pointer,
    * no pointing constraint applied.
    */
      
    virtual VxCandidate * fit(const std::vector<const Trk::Track*> & vectorTrk,
                              const std::vector<double> masses,
                              const double& constraintMass,
                              const Trk::RecVertex* pointingVertex,
                              const Trk::Vertex& startingPoint);

    virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                              const std::vector<double> masses,
                              const double& constraintMass,
                              const Trk::RecVertex* pointingVertex,
                              const Trk::Vertex& startingPoint);
   
    virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                              const std::vector<double> masses,
                              const double& constraintMass,
                              const Trk::RecVertex* pointingVertex,
                              const Trk::Vertex& startingPoint);

    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*> & vectorTrk,
                               const std::vector<double> masses,
                               const double& constraintMass,
                               const Trk::RecVertex* pointingVertex,
                               const Trk::Vertex& firstStartingPoint);


  private:
    long   m_maxIterations;
    double m_maxDchi2PerNdf;
    double m_maxR;
    double m_maxZ;
    bool   m_deltaR;

    /** Data members to store the results */
    long                    m_iter;
    FitError                m_error;

    ToolHandle< Trk::IExtrapolator > m_extrapolator;
    ServiceHandle < MagField::IMagFieldSvc > m_magFieldSvc;
    ToolHandle< IVxCandidateXAODVertex > m_xaodConverter;
  };
}
#endif
