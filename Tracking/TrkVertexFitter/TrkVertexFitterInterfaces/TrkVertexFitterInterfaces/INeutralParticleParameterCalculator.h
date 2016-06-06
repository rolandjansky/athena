/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERINTERFACES_INEUTRALPARTICLEPARAMETERCALCULATOR_H
#define TRKVERTEXFITTERINTERFACES_INEUTRALPARTICLEPARAMETERCALCULATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

#include "xAODTracking/VertexFwd.h"

/**
 * @class Trk::INeutralParticleParameterCalculator
 *
 * interface for tools taking a two-track vertex and creating a neutral track from it.
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
  static const InterfaceID IID_NeutralParticleParameterCalculator("INeutralParticleParameterCalculator", 1, 0);
  
  class INeutralParticleParameterCalculator : virtual public IAlgTool
  {
  public:
    
    static const InterfaceID& interfaceID() { return IID_NeutralParticleParameterCalculator; };
    
   /**
    * method calculating the parameters
    */
    virtual NeutralPerigee* createNeutralTrackFromVertex(const xAOD::Vertex& ) const = 0;



 };//end of class definition

}
#endif
