/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_SEEDNEWTONTRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_SEEDNEWTONTRKDISTANCEFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h" // For TwoPoints
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include <atomic>

namespace Trk
{

  class Trk2dDistanceSeeder;
  class NewtonTrkDistanceFinder;

  /**
   @class SeedNewtonTrkDistanceFinder

   Algotool which calculates the distance between the two tracks, using: the distance 
   finder you specify, which is an object of type NewtonTrkDistanceFinderAlgo, and 
   using a starting position on the tracks given by Trk2dDistanceSeeder.
   
   The class NewtonTrkDistanceFinderAlgo is based on a Newton minimization procedure, which 
   finds the twodimensional point (position on first and second track) on which the derivatives of 
   the distance is zero.

   To avoid maxima the algorithm is initialized with two initial points on the two tracks, which are obtained 
   using the Trk2dDistanceSeeder AlgoTool. This tool finds the point on minimum distance on the transverse plane, 
   which is analitically well defined (intersection of two circles). In case of double intersection different possibilities 
   were considered, at the end I found reasonably good to choose the intersection point where the tracks are nearest 
   in the z coordinate...

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */



  class SeedNewtonTrkDistanceFinder final: public extends<AthAlgTool, ITrkDistanceFinder>
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    //default constructor due to Athena interface
    SeedNewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~SeedNewtonTrkDistanceFinder();

    /** method to do the calculation starting from two Perigees*/
    /** If successful, returns the points on the two tracks at minimum distance. */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const Trk::Perigee &, const Trk::Perigee &) const override final;
    
    /** method to do the calculation starting from two tracks */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const  Trk::Track &, const Trk::Track &) const override final;

    /** method to do the calculation starting from two track particles */
    virtual std::optional<TwoPoints>
    CalculateMinimumDistance(const  Trk::TrackParticleBase &, const Trk::TrackParticleBase &) const override final;
    
        
  private:
    ToolHandle<Trk2dDistanceSeeder> m_2ddistanceseeder;
    ToolHandle<NewtonTrkDistanceFinder> m_distancefinder;
    mutable std::atomic<int> m_numberOfMinimizationFailures;
  };
}
#endif
