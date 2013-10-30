/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_TRK2DDISTANCESEEDER_H
#define TRKVERTEXSEEDFINDERUTILS_TRK2DDISTANCESEEDER_H

#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace MagField { 
  class IMagFieldSvc;
}

namespace Trk
{

  /**
   @class Trk2dDistanceSeeder

   AlgoTool calculates the points of minimum distance between two 
   tracks on the transverse plane, 
   which is analitically well defined (intersection of two circles). In case of double intersection different possibilities 
   were considered, at the end I found reasonably good to choose the intersection point where the tracks are nearest 
   in the z coordinate...

   changes: 11-11-2007 Intersection case completely rewritten after having done analytical calculation (sign come out of calculation!)

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  static const InterfaceID IID_Trk2dDistanceSeeder("Trk2dDistanceSeeder", 1, 1);
  
  class Trk2dDistanceSeeder : public AthAlgTool
  {
  public:

    static const InterfaceID& interfaceID()
    {
        return IID_Trk2dDistanceSeeder;
    };


    StatusCode initialize();
    StatusCode finalize();
    
    //default constructor due to Athena interface
    Trk2dDistanceSeeder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //
    //destructor
    ~Trk2dDistanceSeeder();

    const TwoPointOnTrack GetSeed(const TwoTracks &);

    //additional methods to use stored information

    const Amg::Vector3D GetCrossingPoint() const;
  
    const TwoPoints GetClosestPoints() const;
    
    double GetDistance() const;        
        
  private:

    //option
    bool m_solveAmbiguityUsingZ;

    //Temporary variables needed in the calculation...

    std::pair<Amg::Vector3D,Amg::Vector3D> m_centersofcurv;

    double m_bfield1;
    double m_bfield2;
    
    //distance on the transverse plane
    double m_distance2d;
    
    //  radius of first track in transverse plane
    double m_abs1;
    
    // radius of second track in transverse plane
    double m_abs2;
    
    double m_sgnr1;
    
    double m_sgnr2;
    
    // phi angle of first track in circle
    double m_phi1;
    
    //same for second
    double m_phi2;

    //tangent phi at point of closest approach
    double m_phitanpoca1;
    
    //same fo
    double m_phitanpoca2;

    //variables for magnetic field tool needed to retrieve the correct Bz
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  };
}
#endif
