/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_Trk2DDistanceFinder_H
#define TRKVERTEXSEEDFINDERUTILS_Trk2DDistanceFinder_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk
{

  class MeasuredPerigee;
  class Trk2dDistanceSeeder;
  class NewtonTrkDistanceFinder;

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



  class Trk2DDistanceFinder : public AthAlgTool, virtual public ITrkDistanceFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    Trk2DDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~Trk2DDistanceFinder();

    /** method to do the calculation starting from two MeasuredPerigees*/
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
    int m_numberOfMinimizationFailures;

  };
}
#endif
