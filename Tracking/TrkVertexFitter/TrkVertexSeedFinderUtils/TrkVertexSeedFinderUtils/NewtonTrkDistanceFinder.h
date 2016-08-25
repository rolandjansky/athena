/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_NEWTONTRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_NEWTONTRKDISTANCEFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace MagField { 
  class IMagFieldSvc;
}

namespace Trk
{

  /**
  @class NewtonTrkDistanceFinder

   AlgoTool which uses an iterative Newton method in two dimensions (the two points along the 
   two tracks) in order to find their minimum distance. For each track you can also provade a starting 
   point, in order to avoid local maxima or an undefined quadratic form near a point...

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  static const InterfaceID IID_NewtonTrkDistanceFinder("NewtonTrkDistanceFinder", 1, 1);
  
  class NewtonTrkDistanceFinder : public AthAlgTool
  {
  public:

    static const InterfaceID& interfaceID()
      {
        return IID_NewtonTrkDistanceFinder;
      };

    
    StatusCode initialize();
    StatusCode finalize();
    
    //default constructor due to Athena interface
    NewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    
    //
    //destructor
    virtual ~NewtonTrkDistanceFinder();

  const TwoPointOnTrack GetClosestPoints(const Perigee & a, const Perigee & b) {
    //with the constractur of PointOnTrackPar a track is constructed with, as seed, 
    //directly the point of closest approach (see for info PointOnTrack.h)
    return GetClosestPoints(PointOnTrack(a),PointOnTrack(b));
  }
  
  const TwoPointOnTrack GetClosestPoints(const PointOnTrack &, const PointOnTrack &);

  const TwoPointOnTrack GetClosestPoints(const TwoTracks & twotracks) {
    return GetClosestPoints(twotracks.getFirstPerigee(),twotracks.getSecondPerigee());
  }
  
  const TwoPointOnTrack GetClosestPoints(const TwoPointOnTrack & twopointontrack) {
    return GetClosestPoints(twopointontrack.first,twopointontrack.second);
  }

  const Amg::Vector3D GetCrossingPoint() const;
  
  const TwoPoints GetClosestPoints() const;
  
  double GetDistance() const;
    
  private:

  //prepare magnetic field in z direction for first and second track separately
  //(we will ask for Bz at the point of closest approach)
  double m_a_Bz,m_b_Bz;

  //prepare variable for first track ('a')
  double m_a_x0,m_a_y0,m_a_z0,m_a_phi0,m_a_Rt,m_a_cosphi0,m_a_sinphi0;
  
  //prepare more detailed part of calculations for first track
  double m_a_cotantheta;
   
  //prepare phi of point of closest approach which has to be found
  double m_a_phi,m_a_cosphi,m_a_sinphi;


  //prepare variable for second track ('b')
  double m_b_x0,m_b_y0,m_b_z0,m_b_phi0,m_b_Rt,m_b_cosphi0,m_b_sinphi0;
  
  //prepare more detailed part of calculations for first track
  double m_b_cotantheta;
   
  //prepare phi of point of closest approach which has to be found
  double m_b_phi,m_b_cosphi,m_b_sinphi;

  double m_ab_Dx0,m_ab_Dy0,m_ab_Dz0;

  //now the first and second derivatives respect to phi
  double m_d1da_phi,m_d1db_phi;
  double m_d2da_phi2,m_d2db_phi2,m_d2da_phib_phi;
    
  //calculate the determinant of the Jacobian of the derivatives for the inverse
  double m_det;

  //difference which the Newton method gives you to add to previous measurement
  double m_deltaa_phi;
  double m_deltab_phi;

  //parameters for precision
  double m_precision;//as job option
  double m_maxloopnumber;//as job option
  double m_loopsnumber;

  //variables for magnetic field service needed to retrieve the correct Bz
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;
  
  };

  namespace Error {
    struct NewtonProblem {
      const char* p;
      NewtonProblem(const char* q) {p=q; }
    };	
  }

}

#endif
