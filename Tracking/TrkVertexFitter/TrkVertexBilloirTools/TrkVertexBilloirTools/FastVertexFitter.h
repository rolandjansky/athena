/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERBILLOIR_FASTVERTEXFITTER_H
#define TRKVERTEXFITTERBILLOIR_FASTVERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

namespace Trk {
class Track;
class TrackParticleBase;
class IExtrapolator;
class IVertexLinearizedTrackFactory;

/** This class implements a fast vertex fitting algorithm as proposed by P.
Billoir. this algorithm tries to estimate the vertex position without refitting
the track parameters. It therefore only needs to vary two of five perigee
parameters (namely d0, z0) to estimate the vertex position.

Changes:

David Shope <david.richard.shope@cern.ch> (2016-04-19)

EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex,
                        from Trk::RecVertex   to xAOD::Vertex,
                        from Trk::Vertex      to Amg::Vector3D
*/

class ATLAS_NOT_THREAD_SAFE FastVertexFitter //const_cast the extrapolator result
  : public extends<AthAlgTool, IVertexFitter>
{
public:
  // The following 'using' can be removed when IVertexFitter::fit has been fully
  // migrated to the one with the EventContext
  using Trk::IVertexFitter::fit;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  FastVertexFitter(const std::string& t,
                   const std::string& n,
                   const IInterface* p);
  virtual ~FastVertexFitter(); //!< standard destructor

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
      << "FastVertexFitter::fit(fit(const std::vector<const "
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
      << "FastVertexFitter::fit(fit(const std::vector<const "
         "TrackParameters*>&,const std::vector<const "
         "Trk::NeutralParameters*>&,const xAOD::Vertex&) ignoring neutrals"
      << endmsg;
    return fit(perigeeList, constraint);
  };

  /**
   * Fit interface with no starting point or constraint. (0,0,0) will be
   * assumed.
   */
  virtual xAOD::Vertex* fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList) const override;

  virtual xAOD::Vertex* fit(
    const std::vector<const TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& /*neutralPerigeeList*/)
    const override
  {
    msg(MSG::WARNING) << "FastVertexFitter::fit(fit(const std::vector<const "
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
      << "FastVertexFitter::fit(fit(const std::vector<const "
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
      << "FastVertexFitter::fit(fit(const std::vector<const "
         "TrackParticle*>&,const std::vector<const "
         "Trk::NeutralParticle*>&,const xAOD::Vertex&) ignoring neutrals"
      << endmsg;
    return fit(vectorTrk, constraint);
  };

private:
  unsigned int m_maxIterations;
  double m_maxDchi2PerNdf;

  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ToolHandle<Trk::IVertexLinearizedTrackFactory> m_linFactory;
};
}
#endif

