/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  
  /** This class implements a full vertex fitting algorithm as proposed by P. Billoir.
  this algorithm tries to estimate the vertex position with refitting the track
  parameters.

  Changes:

  David Shope <david.richard.shope@cern.ch> (2016-04-19)

  EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
                          from Trk::RecVertex   to xAOD::Vertex,
                          from Trk::Vertex      to Amg::Vector3D

  */

  class FullVertexFitter : public extends<AthAlgTool, IVertexFitter>
  {
  public:
    // The following 'using' can be removed when IVertexFitter::fit has been
    // fully migrated to the one with the EventContext
    using Trk::IVertexFitter::fit;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    enum FitError
    {
      FITOK,
      MATINV,
      NEGTRCHI2,
      MAXCHI2,
      MAXTRCHI2,
      NOTRKS,
      NOFIT
    };
    FullVertexFitter(const std::string& t,
                     const std::string& n,
                     const IInterface* p);
    virtual ~FullVertexFitter(); //!< standard destructor

    /** Interface for ParametersBase with starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const Amg::Vector3D& startingPoint) const override;

    /** Interface for TrackParameters and NeutralParameters with starting point
     */
    virtual xAOD::Vertex* fit(
      const std::vector<const TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/,
      const Amg::Vector3D& startingPoint) const override
    {
      msg(MSG::WARNING)
        << "FullVertexFitter::fit(fit(const std::vector<const "
           "TrackParameters*>&,const std::vector<const "
           "Trk::NeutralParameters*>&,const Amg::Vector3D&) ignoring neutrals"
        << endmsg;
      return fit(perigeeList, startingPoint);
    };

    /** Interface for ParametersBase with vertex constraint */
    /** the position of the constraint is ALWAYS the starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const xAOD::Vertex& constraint) const override;

    /** Interface for TrackParameters and NeutralParameters with RecVertex
     * starting point */
    virtual xAOD::Vertex* fit(
      const std::vector<const TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/,
      const xAOD::Vertex& constraint) const override
    {
      msg(MSG::WARNING)
        << "FullVertexFitter::fit(fit(const std::vector<const "
           "TrackParameters*>&,const std::vector<const "
           "Trk::NeutralParameters*>&,const xAOD::Vertex&) ignoring neutrals"
        << endmsg;
      return fit(perigeeList, constraint);
    };

    /**
     * Fit interface with no starting point or constraint. (0,0,0) will be
     * assumed.
     */
    virtual xAOD::Vertex* fit(const std::vector<const Trk::TrackParameters*>&
                                perigeeList) const override;

    virtual xAOD::Vertex* fit(
      const std::vector<const TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/)
      const override
    {
      msg(MSG::WARNING) << "FullVertexFitter::fit(fit(const std::vector<const "
                           "TrackParameters*>&,const std::vector<const "
                           "Trk::NeutralParameters*>&) ignoring neutrals"
                        << endmsg;
      return fit(perigeeList);
    };

    /**  	     *Interface for xAOD::TrackParticle with starting point  */
    virtual xAOD::Vertex* fit(
      const std::vector<const xAOD::TrackParticle*>& vectorTrk,
      const Amg::Vector3D& startingPoint) const override;

    /**  	     *Interface for xAOD::TrackParticle and NeutralParticle with
     * starting point  */
    virtual xAOD::Vertex* fit(
      const std::vector<const xAOD::TrackParticle*>& vectorTrk,
      const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/,
      const Amg::Vector3D& startingPoint) const override
    {
      msg(MSG::WARNING)
        << "FullVertexFitter::fit(fit(const std::vector<const "
           "TrackParticle*>&,const std::vector<const "
           "Trk::NeutralParticle*>&,const Amg::Vector3D&) ignoring neutrals"
        << endmsg;
      return fit(vectorTrk, startingPoint);
    };

    /**  	     * Interface for xAOD::TrackParticle with vertex constraint
     * the position of the constraint is ALWAYS the starting point  	     */
    virtual xAOD::Vertex* fit(
      const std::vector<const xAOD::TrackParticle*>& vectorTrk,
      const xAOD::Vertex& constraint) const override;

    /**  	     * Interface for xAOD::TrackParticle and NeutralParticle with
     * vertex constraint the position of the constraint is ALWAYS the starting
     * point  	     */
    virtual xAOD::Vertex* fit(
      const std::vector<const xAOD::TrackParticle*>& vectorTrk,
      const std::vector<const xAOD::NeutralParticle*>& /*vectorNeut*/,
      const xAOD::Vertex& constraint) const override
    {
      msg(MSG::WARNING)
        << "FullVertexFitter::fit(fit(const std::vector<const "
           "TrackParticle*>&,const std::vector<const "
           "Trk::NeutralParticle*>&,const xAOD::Vertex&) ignoring neutrals"
        << endmsg;
      return fit(vectorTrk, constraint);
    };

  private:
    unsigned int m_maxIterations;
    double m_maxDchi2PerNdf;

    /** Data members to store the results */

    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    ToolHandle<Trk::IVertexLinearizedTrackFactory> m_linFactory;
  };
  }
#endif

