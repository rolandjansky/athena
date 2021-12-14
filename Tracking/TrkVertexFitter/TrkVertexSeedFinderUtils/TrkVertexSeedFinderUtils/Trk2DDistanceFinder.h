/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_Trk2DDistanceFinder_H
#define TRKVERTEXSEEDFINDERUTILS_Trk2DDistanceFinder_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h" // For TwoPoints
#include "AthenaBaseComps/AthAlgTool.h"
#include <atomic>

namespace Trk
{

  class MeasuredPerigee;
  class Trk2dDistanceSeeder;

  /**
   @class Trk2DDistanceFinder

   Algotool which calculates the distance between the two tracks in 2d 
   only, re-using the Trk2dDistanceSeeder tool, which is based on the analytic 
   solution of the minimum distance between two circles.

   Of the two possible solutions, in Trk2dDistanceSeeder the one which 
   minimizes the distance in z is used. (not sure this is what we want 
   to find an approximate beam spot position event by event during 
   primary vertex finding).

   @author giacinto.piacquadio@cern.ch

   */



  class Trk2DDistanceFinder final: public extends<AthAlgTool, ITrkDistanceFinder>
  {
  public:
    //default constructor due to Athena interface
    Trk2DDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~Trk2DDistanceFinder();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** method to do the calculation starting from two MeasuredPerigees*/
    /** If successful, returns the points on the two tracks at minimum distance. */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const Trk::Perigee &, const Trk::Perigee &) const override final;
    
    /** method to do the calculation starting from two tracks */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const  Trk::Track &, const Trk::Track &) const override final;

    /** method to do the calculation starting from two track particles */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const  Trk::TrackParticleBase &,
                             const  Trk::TrackParticleBase &) const override final;


  private:
    ToolHandle<Trk2dDistanceSeeder> m_2ddistanceseeder;
    mutable std::atomic<int> m_numberOfMinimizationFailures;
  };
}
#endif
