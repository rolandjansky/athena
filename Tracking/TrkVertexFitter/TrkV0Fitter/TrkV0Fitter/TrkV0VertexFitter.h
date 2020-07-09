/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkV0VertexFitter.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////
/**
 *  A first concrete implementation of the IV0VertexFitter
 *  for the purposes of V0 fitting
 *
 *  @authors:   E. Bouhova-Thacker <e.bouhova@cern.ch>, R. Henderson <Rob.Henderson@cern.ch> 
 *  @date:   14 December 2006
 *
 */

#ifndef TRKV0FITTER_TRKV0VERTEXFITTER_H
#define TRKV0FITTER_TRKV0VERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace Trk
{
  class Track;
  class TrackParticleBase;
  class IExtrapolator;
  //class IVxCandidateXAODVertex;

  /** This class implements a vertex fitting algorithm optimised for V0 finding.
  The algorithm fits the full track information, incorporating the
  possibility of performing full kinematic constraints at the same time.
  The full covariance matrix from the fit, including track-track and track-vertex
  correlations is calculated and returned */

  class TrkV0VertexFitter : public extends<AthAlgTool, IVertexFitter>
  {
  public:
    // The following 'using' can be removed when IVertexFitter::fit 
    // has been fully migrated to the one with the EventContext
    using Trk::IVertexFitter::fit;
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    enum FitError {FITOK,MATINV,NEGTRCHI2,MAXCHI2,MAXTRCHI2,NOTRKS,NOFIT};
    TrkV0VertexFitter(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~TrkV0VertexFitter(); //!< standard destructor

    /** Interface for xAOD::TrackParticle with Amg::Vector3D starting point */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                               const Amg::Vector3D& startingPoint) const override;

    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                               const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
                               const Amg::Vector3D& startingPoint) const override
    {
      msg(MSG::WARNING)
        << "TrkV0VertexFitter::fit(fit(const std::vector<const "
           "TrackParticle*>&,const std::vector<const "
           "Trk::NeutralParticle*>&,const Amg::Vector3D&) ignoring neutrals"
        << endmsg;
      return fit(vectorTrk, startingPoint);
    };

    /** Interface for xAOD::TrackParticle with xAOD::Vertex starting point */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                               const xAOD::Vertex& constraint) const override;

    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
                               const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/, 
                               const xAOD::Vertex& constraint) const override
    {
      msg(MSG::WARNING)
        << "TrkV0VertexFitter::fit(fit(const std::vector<const "
           "TrackParticle*>&,const std::vector<const "
           "Trk::NeutralParticle*>&,const xAOD::Vertex&) ignoring neutrals"
        << endmsg;
      return fit(vectorTrk, constraint);
    };

    /** Fit interface for xAOD::TrackParticle with no starting point. (0,0,0)
     * will be assumed */
    virtual xAOD::Vertex* fit(
      const std::vector<const xAOD::TrackParticle*>& vectorTrk) const;

    /** Interface for Trk::TrackParameters with Amg::Vector3D starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const Amg::Vector3D& startingPoint) const override;

    /** Interface for Trk::TrackParameters and NeutralParameters with
     * Amg::Vector3D starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/,
      const Amg::Vector3D& startingPoint) const override
    {
      msg(MSG::WARNING)
        << "TrkV0VertexFitter::fit(fit(const std::vector<const "
           "Trk::TrackParameters*>&,const std::vector<const "
           "Trk::NeutralParameters*>&,const Amg::Vector3D&) ignoring neutrals"
        << endmsg;
      return fit(perigeeList, startingPoint);
    };

    /** Interface for TrackParameters with xAOD::Vertex starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const xAOD::Vertex& constraint) const override;

    /** Interface for TrackParameters and NeutralParameters with xAOD::Vertex
     * starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/,
      const xAOD::Vertex& constraint) const override
    {
      msg(MSG::WARNING)
        << "TrkV0VertexFitter::fit(fit(const std::vector<const "
           "Trk::TrackParameters*>&,const std::vector<const "
           "Trk::NeutralParameters*>&,const xAOD::Vertex&) ignoring neutrals"
        << endmsg;
      return fit(perigeeList, constraint);
    };

    /** Fit interface for TrackParameters with no starting point. (0,0,0) will
     * be assumed. */
    virtual xAOD::Vertex* fit(const std::vector<const Trk::TrackParameters*>&
                                perigeeList) const override;

    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/)
      const override
    {
      msg(MSG::WARNING) << "TrkV0VertexFitter::fit(fit(const std::vector<const "
                           "Trk::TrackParameters*>&,const std::vector<const "
                           "Trk::NeutralParameters*>&) ignoring neutrals"
                        << endmsg;
      return fit(perigeeList);
    };

   /**
    * Methods specific for the V0 fitter
    * Method taking a vector of tracks, vector of masses and starting point as arguments.
    * Action: Fits a vertex out of initial set of tracks and applies a mass
    * constraint to the particle reconstructed at the vertex and a pointing
    * constraint: in the case the pointingVertex is a zero pointer,
    * no pointing constraint applied.
    */
      
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*> & vectorTrk,
                               const std::vector<double>& masses,
                               const double& constraintMass,
                               const xAOD::Vertex* pointingVertex,
                               const Amg::Vector3D& startingPoint) const;

    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                               const std::vector<double>& masses,
                               const double& constraintMass,
                               const xAOD::Vertex* pointingVertex,
                               const Amg::Vector3D& startingPoint) const;


  private:
    int   m_maxIterations;
    double m_maxDchi2PerNdf;
    double m_maxR;
    double m_maxZ;
    bool   m_firstMeas;
    bool   m_deltaR;

    /** Data members to store the results */

    ToolHandle< Trk::IExtrapolator > m_extrapolator;
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey
       {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  };
}
#endif
