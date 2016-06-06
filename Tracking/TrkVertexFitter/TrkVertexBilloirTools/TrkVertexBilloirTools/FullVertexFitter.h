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
  class Track;
  class TrackParticleBase;
  class IExtrapolator;
  class IVertexLinearizedTrackFactory;
  class IVxCandidateXAODVertex; 
  
  /** This class implements a full vertex fitting algorithm as proposed by P. Billoir.
  this algorithm tries to estimate the vertex position with refitting the track
  parameters.

  Changes:

  David Shope <david.richard.shope@cern.ch> (2016-04-19)

  EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
                          from Trk::RecVertex   to xAOD::Vertex,
                          from Trk::Vertex      to Amg::Vector3D

  */

  class FullVertexFitter : public AthAlgTool, virtual public IVertexFitter
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    enum FitError {FITOK,MATINV,NEGTRCHI2,MAXCHI2,MAXTRCHI2,NOTRKS,NOFIT};
    FullVertexFitter(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~FullVertexFitter(); //!< standard destructor

    /** Interface for Track with starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::Track*> & vectorTrk,
                               const Amg::Vector3D& startingPoint);

    /** Interface for Track with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::Track*>& vectorTrk,
                               const xAOD::Vertex& constraint);

    /** Interface for TrackParticleBase with starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                               const Amg::Vector3D& startingPoint);

    /** Interface for TrackParticleBase with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                               const xAOD::Vertex& constraint);

    /** Interface for ParametersBase with starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                               const Amg::Vector3D& startingPoint);

    /** Interface for TrackParameters and NeutralParameters with starting point */
    xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
		       const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/,
		       const Amg::Vector3D& startingPoint) {msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&,const Amg::Vector3D&) ignoring neutrals" << endreq; return fit(perigeeList, startingPoint);};

    /** Interface for ParametersBase with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                               const xAOD::Vertex& constraint);
    
    /** Interface for TrackParameters and NeutralParameters with RecVertex starting point */
    xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
		       const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/,
		       const xAOD::Vertex& constraint){msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&,const xAOD::Vertex&) ignoring neutrals" << endreq; return fit(perigeeList, constraint);};

    /**
    * Fit interface with no starting point or constraint. (0,0,0) will be assumed.
    */
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*>& perigeeList);

    xAOD::Vertex * fit(const std::vector<const TrackParameters*>& perigeeList,const std::vector<const Trk::NeutralParameters*> & /*neutralPerigeeList*/){msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const TrackParameters*>&,const std::vector<const Trk::NeutralParameters*>&) ignoring neutrals" << endreq; return fit(perigeeList);};

    virtual xAOD::Vertex * fit(const std::vector<const Trk::Track*>& vectorTrk);


    /**  	     *Interface for xAOD::TrackParticle with starting point  */ 
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const Amg::Vector3D& startingPoint); 

     /**  	     *Interface for xAOD::TrackParticle and NeutralParticle with starting point  */ 
    xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
		       const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
		       const Amg::Vector3D& startingPoint){msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const TrackParticle*>&,const std::vector<const Trk::NeutralParticle*>&,const Amg::Vector3D&) ignoring neutrals" << endreq; return fit(vectorTrk, startingPoint);}; 

    /**  	     * Interface for xAOD::TrackParticle with vertex constraint		     
     * the position of the constraint is ALWAYS the starting point  	     */     
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const xAOD::Vertex& constraint); 

    /**  	     * Interface for xAOD::TrackParticle and NeutralParticle with vertex constraint			     
     * the position of the constraint is ALWAYS the starting point  	     */ 
    xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
		       const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
		       const xAOD::Vertex& constraint){msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const TrackParticle*>&,const std::vector<const Trk::NeutralParticle*>&,const xAOD::Vertex&) ignoring neutrals" << endreq; return fit(vectorTrk, constraint);}; 
    
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



