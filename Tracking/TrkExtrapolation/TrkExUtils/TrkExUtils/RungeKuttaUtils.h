/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class  RungeKuttaUtils, (c) ATLAS Detector software
//  author Igor Gavrilenko     
/////////////////////////////////////////////////////////////////////////////////

#ifndef RungeKuttaUtils_H
#define RungeKuttaUtils_H

#include <vector>
#include <map>
#include "GaudiKernel/AlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"

namespace Trk {

  class ConeSurface           ;
  class DiscSurface           ;
  class PlaneSurface          ;
  class CylinderSurface       ;
  class StraightLineSurface   ;
  class PatternTrackParameters;
  
  /**
  @class RungeKuttaUtils
  
  Trk::RungeKuttaUtils is set algorithms for track parameters transformation
  from local to global and global to local system coordinate and step
  estimation to surface.

    AtaPlane    AtaStraightLine      AtaDisc       AtaCylinder      Perigee
       |               |               |               |              |
       |               |               |               |              |
       V               V               V               V              V 
       ----------------------------------------------------------------- 
                                       |              Local->Global transformation
                                       V
                    Global position (Runge Kutta presentation)
                                       |
                                       V              Global->Local transformation
       ----------------------------------------------------------------
       |               |               |               |              |
       |               |               |               |              |
       V               V               V               V              V
   PlaneSurface StraightLineSurface DiscSurface CylinderSurface PerigeeSurface 

  For using Runge Kutta method we use global coordinate, direction,
  inverse momentum and Jacobian of transformation. All this parameters we save 
  in array P[42].
                   /dL0    /dL1    /dPhi   /dThe   /dCM
  X  ->P[0]  dX /   P[ 7]   P[14]   P[21]   P[28]   P[35]  
  Y  ->P[1]  dY /   P[ 8]   P[15]   P[22]   P[29]   P[36]  
  Z  ->P[2]  dZ /   P[ 9]   P[16]   P[23]   P[30]   P[37]   
  Ax ->P[3]  dAx/   P[10]   P[17]   P[24]   P[31]   P[38]  
  Ay ->P[4]  dAy/   P[11]   P[18]   P[25]   P[32]   P[39]  
  Az ->P[5]  dAz/   P[12]   P[19]   P[26]   P[33]   P[40]  
  CM ->P[6]  dCM/   P[13]   P[20]   P[27]   P[34]   P[41] 
 
  where 
       in case local presentation 

       L0  - first  local coordinate  (surface dependent)
       L1  - second local coordinate  (surface dependent)
       Phi - Azimuthal angle
       The - Polar     angle
       CM  - charge/momentum

       in case global presentation

       X   - global x-coordinate        = surface dependent
       Y   - global y-coordinate        = surface dependent
       Z   - global z-coordinate        = sutface dependent
       Ax  - direction cosine to x-axis = Sin(The)*Cos(Phi)
       Ay  - direction cosine to y-axis = Sin(The)*Sin(Phi)
       Az  - direction cosine to z-axis = Cos(The)
       CM  - charge/momentum            = local CM

  @author Igor.Gavrilenko@cern.ch     
  **/

  class RungeKuttaUtils
    {
      /////////////////////////////////////////////////////////////////////////////////
      // Public methods:
      /////////////////////////////////////////////////////////////////////////////////
      
    public:
      
      RungeKuttaUtils(){};
      virtual ~RungeKuttaUtils (){};

      /////////////////////////////////////////////////////////////////////////////////
      // Step estimators to surface
      /////////////////////////////////////////////////////////////////////////////////

      double stepEstimator              (int,double*,const double*,bool&) const;
      double stepEstimatorToCone            (double*,const double*,bool&) const;
      double stepEstimatorToPlane           (double*,const double*,bool&) const;
      double stepEstimatorToCylinder        (double*,const double*,bool&) const;
      double stepEstimatorToStraightLine    (double*,const double*,bool&) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Step estimators to surfaces
      /////////////////////////////////////////////////////////////////////////////////
      std::pair<double,int> stepEstimator 
	(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >&,
	 std::multimap<double,int>&,
	 const double*,const double*,double,double,int Nv,bool&) const;

      int fillDistancesMap
	(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >&,
	 std::multimap<double,int>&,
	 const double*,double,const Trk::Surface*,double*) const;
      
      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from local to global system coordinates 
      // for Trk::TrackParameters and Trk::NeutralParameters 
      /////////////////////////////////////////////////////////////////////////////////
  
      bool transformLocalToGlobal
	(bool,const Trk::TrackParameters&        ,double*) const;

      bool transformLocalToGlobal
	(bool,const Trk::NeutralParameters&       ,double*) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from local to global system coordinates 
      // for Trk::PatternTrackParameters
      /////////////////////////////////////////////////////////////////////////////////
  
      bool transformLocalToGlobal
	(bool,const Trk::PatternTrackParameters&        ,double*) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from local to global system coordinates 
      // for different surfaces
      /////////////////////////////////////////////////////////////////////////////////

      void transformDiscToGlobal        
	(bool,const Trk::Surface*,const double*,double*) const;
      void transformPlaneToGlobal       
	(bool,const Trk::Surface*,const double*,double*) const;
      void transformCylinderToGlobal    
	(bool,const Trk::Surface*,const double*,double*) const;
      void transformLineToGlobal
	(bool,const Trk::Surface*,const double*,double*) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from global to local system coordinates
      /////////////////////////////////////////////////////////////////////////////////

      void transformGlobalToLocal        (double*,double*) const;
      void transformGlobalToLocal
	 (const Trk::Surface*,bool,double*,double*,double*) const;
      void transformGlobalToCone
	(const Trk::Surface*,bool,double*,double*,double*) const;
      void transformGlobalToDisc
	(const Trk::Surface*,bool,double*,double*,double*) const;
      void transformGlobalToPlane
	(const Trk::Surface*,bool,double*,double*,double*) const;
      void transformGlobalToCylinder
  	(const Trk::Surface*,bool,double*,double*,double*) const;
      void transformGlobalToLine
	(const Trk::Surface*,bool,double*,double*,double*) const;

     /////////////////////////////////////////////////////////////////////////////////

      /////////////////////////////////////////////////////////////////////////////////
      // Covariance matrix production for Trk::TrackParameters
      /////////////////////////////////////////////////////////////////////////////////
      
      AmgSymMatrix(5)*  newCovarianceMatrix
	(double*,const AmgSymMatrix(5)&) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from curvilinear to global system coordinates 
      // covariance matrix only
      /////////////////////////////////////////////////////////////////////////////////

      void transformCurvilinearToGlobal        
	(double* ,double*) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Transformations from global to curvilinear system coordinates 
      // covariance matrix only
      /////////////////////////////////////////////////////////////////////////////////

      void transformGlobalToCurvilinear        
	(bool,double*,double*,double*) const;

      /////////////////////////////////////////////////////////////////////////////////
      // Jacobian of transformations from curvilinear to local system coordinates
      /////////////////////////////////////////////////////////////////////////////////

      void jacobianTransformCurvilinearToLocal
	(const Trk::TrackParameters&,double*);

      void jacobianTransformCurvilinearToLocal
	(const Trk::PatternTrackParameters&,double*); 

      void jacobianTransformCurvilinearToLocal
	(double*,const Trk::Surface*,double*);
   
      void jacobianTransformCurvilinearToDisc        (double*,double*) const;
      void jacobianTransformCurvilinearToPlane       (double*,double*) const;
      void jacobianTransformCurvilinearToCylinder    (double*,double*) const;
      void jacobianTransformCurvilinearToStraightLine(double*,double*) const;

    private:

      /////////////////////////////////////////////////////////////////////////////////
      // Private methods:
      /////////////////////////////////////////////////////////////////////////////////

      bool transformLocalToGlobal(bool,const Trk::Surface*,const double*,double*) const;
    };

}
 
#endif // RungeKuttaUtils_H
