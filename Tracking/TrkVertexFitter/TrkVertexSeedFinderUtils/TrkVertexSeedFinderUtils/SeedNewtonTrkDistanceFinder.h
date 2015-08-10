/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_SEEDNEWTONTRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_SEEDNEWTONTRKDISTANCEFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkParameters/TrackParameters.h"

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



  class SeedNewtonTrkDistanceFinder : public AthAlgTool, virtual public ITrkDistanceFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    SeedNewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~SeedNewtonTrkDistanceFinder();

    /** method to do the calculation starting from two Perigees*/
    /** return value is true if calculation is successfull */
    virtual bool CalculateMinimumDistance(const Trk::Perigee &, const Trk::Perigee &);
    
    /** method to do the calculation starting from two tracks */
    virtual bool CalculateMinimumDistance(const  Trk::Track &, const Trk::Track &);

    /** method to do the calculation starting from two track particles */
    virtual bool CalculateMinimumDistance(const  Trk::TrackParticleBase &, const Trk::TrackParticleBase &);
    
    /**method to obtain the distance (call CalculateMinimumDistance before) **/
    virtual double GetDistance() const;
    
    /** method to obtain the points on the two tracks at minimum distance **/
    virtual const std::pair<Amg::Vector3D,Amg::Vector3D> GetPoints() const;
    
    
        
  private:

    ToolHandle<Trk2dDistanceSeeder> m_2ddistanceseeder;
    ToolHandle<NewtonTrkDistanceFinder> m_distancefinder;
    int m_numberOfMinimizationFailures;

  };
}
#endif
