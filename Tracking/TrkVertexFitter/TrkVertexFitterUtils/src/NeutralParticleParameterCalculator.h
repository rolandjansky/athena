/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERUTILS_NEUTRALPARTICLEPARCALCULATOR_H
#define TRKVERTEXFITTERUTILS_NEUTRALPARTICLEPARCALCULATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/INeutralParticleParameterCalculator.h"

#include "xAODTracking/TrackParticleContainer.h"  
#include "xAODTracking/Vertex.h"




/**
 * @class Trk::NeutralParticleParameterCalculator
 *
 * Tool taking a two-track vertex and creating a neutral track from it.
 * If the vertex is an extended vertex, it uses the full vertex-to-track covariance matrix
 * (constrained vertex fit), if not, then it assumes all the correlations come simply
 * from the fact that a common vertex is fit (unconstrained vertex fit), and reconstructs
 * back the vertex-to-track correlations from the vertex and track cov. matrices.
 * The former is used in egamma, while in JetFitter the latter is used.
 *
 * @author N. Giacinto Piacquadio (for the Freiburg Group)
 *
 * -----------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-26)
 *
 *   EDM Migration to xAOD - remove method using VxCandidate
 */

namespace Trk
{
  class IVertexLinearizedTrackFactory;
  class LinearizedTrack;

  class NeutralParticleParameterCalculator : public AthAlgTool, virtual public INeutralParticleParameterCalculator
  {
  public:
    virtual StatusCode initialize() override;

  /**
   * Default constructor due to Athena interface
   */
    NeutralParticleParameterCalculator(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * Destructor
    */
    ~NeutralParticleParameterCalculator();
    
   /**
    * method that makes the parameters of a V0 or photon before decay to two tracks
    */
    virtual NeutralPerigee* createNeutralTrackFromVertex(const xAOD::Vertex& ) const override;

  private:

    std::pair<AmgMatrix(3,3),AmgMatrix(3,3)> getPosMomentumAndMomentumCovMatrix(const Trk::LinearizedTrack* linTrack,
                                                                                const AmgMatrix(3,3) & vrt_cov,
                                                                                const AmgMatrix(3,3) & vrt_weight) const;
   
    AmgMatrix(3,3) getPhiThetaQOverPToPxPyPzJacobian(double qOverP,double theta,double phi) const;

    ToolHandle<Trk::IVertexLinearizedTrackFactory> m_LinearizedTrackFactory;
    bool m_linearizedTrackFactoryIsAvailable;
    
  };

}

#endif
