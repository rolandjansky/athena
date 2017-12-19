/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class  STEP_Propagator
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track status propagation through magnetic field
/////////////////////////////////////////////////////////////////////////////////
// Based on RungeKuttaPropagator Version 1.0 21/05/2004 I.Gavrilenko
// Version 0.1 16/2/2005 Esben Lund
// Extended by S.Todorova for propagation to multiple surfaces 
/////////////////////////////////////////////////////////////////////////////////

#ifndef STEP_Propagator_H
#define STEP_Propagator_H

#include <list>
#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h" //TrackParameters typedef
#include "TrkGeometry/BinnedMaterial.h" //Identified material typedef
#include "TrkExUtils/MaterialInteraction.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
//#include "TrkExUtils/ExtrapolationCache.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {
  class Surface;
  class TrackingVolume;
  class ITimedMatEffUpdator;
  class ScatteringAngles;
  class AlignableTrackingVolume;
  class ExtrapolationCache;
  /**
     @class STEP_Propagator

     STEP_Propagator is an algorithm for track parameters propagation through a
     magnetic field, including material effects, with or without jacobian of transformation.
     This algorithm contains three steps:

     1.The first step of the algorithm is track parameters transformation from
     local presentation for given start surface to global Runge Kutta coordinates.

     2.The second step is propagation through magnetic field with or without jacobian.

     3.Third step is transformation from global Runge Kutta presentation to local
     presentation of given output surface.


     AtaPlane    AtaStraightLine      AtaDisc       AtaCylinder      Perigee
   |               |               |               |              |
   |               |               |               |              |
       V               V               V               V              V
       -----------------------------------------------------------------
   |              Local->Global transformation
                                       V
                    Global position (Runge Kutta presentation)
   |
   |
                 Propagation to next surface with or without jacobian
   |
                                       V              Global->Local transformation
       ----------------------------------------------------------------
   |               |               |               |              |
   |               |               |               |              |
       V               V               V               V              V
     PlaneSurface StraightLineSurface DiscSurface CylinderSurface PerigeeSurface

     For propagation using Runge Kutta method we use global coordinate, direction,
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

     The Runge-Kutta method:
     The equations of motion are solved using an embedded pair of Runge-Kutta formulas.
     This method, Runge-Kutta-Fehlberg, calculates a number of points along the step and
     adds them up in different ways to get two different solutions, of different order, for the integration.
     The higher order solution is used for the propagation and the lower order solution for error control.
     The difference between these solutions is used to estimate the quality of the integration (propagation),
     and to calculate the step size for the next step. If the quality is below a given tolerance then the
     step is rejected and repeated with a shorter step length. This propagator uses the TP43
     (Tsitouras-Papakostas 4th and 3rd order) Runge-Kutta pair.

     The step size algoritm by L.P.Endresen and J.Myrheim was choosen for its low step rejection and effective
     step size calculation. The low step rejection is achieved by letting the step size oscillate around
     the optimal value instead of repeating steps every time they fall below the tolerance level.

     Units are mm, MeV and kiloGauss.

     @author esben.lund@fys.uio.no
   **/

  class STEP_Propagator : public AthAlgTool, virtual public IPropagator
  {
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
  public:
    using IPropagator::propagate;
    using IPropagator::propagateT;

    STEP_Propagator(const std::string&,const std::string&,const IInterface*);


    virtual ~STEP_Propagator ();


    /** AlgTool initailize method.*/
    StatusCode initialize();


    /** AlgTool finalize method */
    StatusCode finalize();


    /** Main propagation method for ParametersBase. Use StraightLinePropagator for neutrals */
/*
    const Trk::ParametersBase*
      propagate (const Trk::ParametersBase&          trackParameters,
                 const Trk::Surface&                 targetSurface,
                       Trk::PropDirection            propagationDirection,
                       Trk::BoundaryCheck            boundaryCheck,
                 const      MagneticFieldProperties& magneticFieldProperties,
		            ParticleHypothesis       particle,
		            bool                     returnCurv=false) const;

*/
			    
    /** Main propagation method NeutralParameters. Use StraightLinePropagator for neutrals*/
    const Trk::NeutralParameters*
      propagate (const Trk::NeutralParameters&,
                 const Trk::Surface&,
		 Trk::PropDirection,
		 Trk::BoundaryCheck,
		 bool rC=false) const;
    
    
    /** Propagate parameters and covariance without returning the Jacobian */
    const Trk::TrackParameters*
      propagate (const Trk::TrackParameters&         trackParameters,
	         const Trk::Surface&                 targetSurface,
		 Trk::PropDirection            propagationDirection,
		 Trk::BoundaryCheck            boundaryCheck,
		 const      MagneticFieldProperties& magneticFieldProperties,
		 ParticleHypothesis            particle,
		 bool                          returnCurv = false,
		 const Trk::TrackingVolume*          tVol = 0) const;
    
    /** Propagate parameters and covariance with search of closest surface */
    const Trk::TrackParameters*    
      propagate  (const Trk::TrackParameters&        trackParameters,
		  std::vector<Trk::DestSurf>&        targetSurfaces,
		  Trk::PropDirection                 propagationDirection,
		  const MagneticFieldProperties&     magneticFieldProperties,
		  ParticleHypothesis                 particle,
		  std::vector<unsigned int>&         solutions,
		  double&                            path,
                  bool                               usePathLimit = false,
		  bool                               returnCurv = false,
		  const Trk::TrackingVolume*          tVol = 0) const;       
    
    /** Propagate parameters and covariance with search of closest surface */
    const Trk::TrackParameters*    
      propagateT  (const Trk::TrackParameters&        trackParameters,
		   std::vector<Trk::DestSurf>&        targetSurfaces,
		   Trk::PropDirection                 propagationDirection,
		   const MagneticFieldProperties&     magneticFieldProperties,
		   ParticleHypothesis                 particle,
		   std::vector<unsigned int>&         solutions,
		   Trk::PathLimit&                    path,
		   Trk::TimeLimit&                    time,
		   bool                               returnCurv,
		   const Trk::TrackingVolume*         tVol,
		   std::vector<Trk::HitInfo>*& hitVector) const;
       
    /** Propagate parameters and covariance with search of closest surface and material collection */
    const Trk::TrackParameters*    
      propagateM  (const Trk::TrackParameters&        trackParameters,
		   std::vector<Trk::DestSurf>&        targetSurfaces,
		   Trk::PropDirection                 propagationDirection,
		   const MagneticFieldProperties&     magneticFieldProperties,
		   ParticleHypothesis                 particle,
		   std::vector<unsigned int>&         solutions,
		   std::vector<const Trk::TrackStateOnSurface*>*& matstates,
		   std::vector<std::pair<const Trk::TrackParameters*,int> >*& intersections,
		   double&                            path,
		   bool                               usePathLimit = false,
		   bool                               returnCurv = false,
		   const Trk::TrackingVolume*         tVol = 0,
                   Trk::ExtrapolationCache*           = 0) const;       

    /** Propagate parameters and covariance, and return the Jacobian. WARNING: Multiple Scattering is not included in the Jacobian! */
    const Trk::TrackParameters*
      propagate (const Trk::TrackParameters&         trackParameters,
	         const Trk::Surface&                 targetSurface,
		 Trk::PropDirection            propagationDirection,
		 Trk::BoundaryCheck            boundaryCheck,
                 const MagneticFieldProperties&      magneticFieldProperties,
		 Trk::TransportJacobian*&      jacobian,
		 double& pathLimit,
		 ParticleHypothesis       particle,
		 bool                     returnCurv=false,
		 const Trk::TrackingVolume*          tVol = 0) const;       
    
    
    /** Propagate parameters only */
    const Trk::TrackParameters*
      propagateParameters (const Trk::TrackParameters&         trackParameters,
  	      	           const Trk::Surface&                 targetSurface,
			   Trk::PropDirection            propagationDirection,
			   Trk::BoundaryCheck            boundaryCheck,
			   const MagneticFieldProperties&      magneticFieldProperties,
			   ParticleHypothesis       particle,
			   bool                     returnCurv = false,
			   const Trk::TrackingVolume*          tVol = 0) const;       
    
    
    /** Propagate parameters and return Jacobian. WARNING: Multiple Scattering is not included in the Jacobian! */
    const Trk::TrackParameters*
      propagateParameters (const Trk::TrackParameters&         trackParameters,
  	      	           const Trk::Surface&                 targetSurface,
			   Trk::PropDirection            propagationDirection,
			   Trk::BoundaryCheck            boundaryCheck,
			   const MagneticFieldProperties&      magneticFieldProperties,
			   Trk::TransportJacobian*&      jacobian,
			   ParticleHypothesis       particle,
			   bool                     returnCurv = false,
			   const Trk::TrackingVolume*          tVol = 0) const;       
    
    
    /** Propagate parameters and return path (Similar to propagateParameters */
    const IntersectionSolution*
      intersect (const Trk::TrackParameters&         trackParameters,
      		 const Trk::Surface&                 targetSurface,
                 const Trk::MagneticFieldProperties& magneticFieldProperties,
		 ParticleHypothesis       particle,
		 const Trk::TrackingVolume*          tVol = 0) const;       
    
    /** Intersection and propagation:
     */
    
    const TrackSurfaceIntersection* intersectSurface(const Surface&         surface,
                                                     const TrackSurfaceIntersection*    trackIntersection,
                                                     const double               qOverP,
                                                     const MagneticFieldProperties& mft,
                                                     ParticleHypothesis       particle) const; 
    
    /** Return a list of positions along the track */
    void
      globalPositions (std::list<Amg::Vector3D>&  positionsList,
		       const TrackParameters&           trackParameters,
		       const MagneticFieldProperties&   magneticFieldProperties,
		       const CylinderBounds&            cylinderBounds,
		       double                     maxStepSize,
		       ParticleHypothesis         particle,
		       const Trk::TrackingVolume*       tVol = 0) const;       
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // Private methods:
    /////////////////////////////////////////////////////////////////////////////////
  private:
    
    enum SurfaceType { LINE, PLANE, CYLINDER, CONE};
    
    /////////////////////////////////////////////////////////////////////////////////
    // Main functions for propagation
    /////////////////////////////////////////////////////////////////////////////////
    const Trk::TrackParameters*
      propagateRungeKutta (bool 	               errorPropagation,
			   const Trk::TrackParameters&         trackParameters,
			   const Trk::Surface&                 targetSurface,
			   Trk::PropDirection            propagationDirection,
			   const      MagneticFieldProperties& magneticFieldProperties,
			   ParticleHypothesis       particle,
			   Trk::BoundaryCheck            boundaryCheck,
			   double*                  Jacobian,
			   bool                     returnCurv=false) const;
    
    /////////////////////////////////////////////////////////////////////////////////
    // Main function for propagation
    // with search of closest surface (ST) 
    /////////////////////////////////////////////////////////////////////////////////
    const Trk::TrackParameters*
      propagateRungeKutta (bool 	               errorPropagation,
			   const Trk::TrackParameters&         trackParameters,
			   std::vector<DestSurf>&               targetSurfaces,
			   Trk::PropDirection            propagationDirection,
                           const      MagneticFieldProperties& magneticFieldProperties,
			   ParticleHypothesis       particle,
			   std::vector<unsigned int>&           solutions,
			   double&                  path, 
			   bool                     returnCurv=false) const;
    


    /////////////////////////////////////////////////////////////////////////////////
    // Method of the propagation
    /////////////////////////////////////////////////////////////////////////////////
    bool
      propagateWithJacobian (bool        errorPropagation,
			     Surface::SurfaceType surfaceType,
			     double*     targetSurface,
			     double*     P,
			     double&     path) const;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Method for propagation with search of closest surface (ST)
    ////////////////////////////////////////////////////////////////////////////////
    bool
      propagateWithJacobian (bool        errorPropagation,
			     std::vector< DestSurf >& sfs,
			     double*     P,
			     Trk::PropDirection  propDir,
			     std::vector<unsigned int>&  solutions,
			     double&     path,
			     double      sumPath) const;
    
    /////////////////////////////////////////////////////////////////////////////////
    // Trajectory model
    /////////////////////////////////////////////////////////////////////////////////
    bool
      rungeKuttaStep(bool    errorPropagation,
		     double& h,
		     double* P,
		     double* dDir,
		     float*  BG1,
		     bool&   firstStep,
		     double& distanceStepped) const;
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // Get the magnetic field and gradients
    // Input: Globalposition
    // Output: BG, which contains Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz, dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz
    /////////////////////////////////////////////////////////////////////////////////
    void
      getMagneticField(const Amg::Vector3D&  position,
		       bool            getGradients,
		       float*          BG) const;
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // Distance to surface
    /////////////////////////////////////////////////////////////////////////////////
    double
      distance (Surface::SurfaceType surfaceType,
		double*     targetSurface,
		const double*     P,
		bool&       distanceSuccessful) const;
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // dg/dlambda for non-electrons (g=dEdX and lambda=q/p).
    /////////////////////////////////////////////////////////////////////////////////
    double
      dgdlambda( double l) const;
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // Multiple scattering and straggling contributionto the covariance matrix
    // local covariance - to be phased out
    /////////////////////////////////////////////////////////////////////////////////
    void
      covarianceContribution( const Trk::TrackParameters*  trackParameters,
			      double                 path,
			      const Trk::TrackParameters*  targetParms,
			      AmgSymMatrix(5)* measurementCovariance) const;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Multiple scattering and straggling contributionto the covariance matrix in curvilinear representation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void
      covarianceContribution( const Trk::TrackParameters*  trackParameters,
			      double                 path,
			      double                 finalMomentum,
			      AmgSymMatrix(5)* measurementCovariance) const;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // dump material effects
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void dumpMaterialEffects( const Trk::CurvilinearParameters*  trackParameters,
			      double                 path) const;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // update material effects   // to follow change of material
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateMaterialEffects( double p, double sinTh, double path) const;
    
    
    /////////////////////////////////////////////////////////////////////////////////
    // Create straight line in case q/p = 0
    /////////////////////////////////////////////////////////////////////////////////
    const Trk::TrackParameters*
      createStraightLine( const Trk::TrackParameters*  inputTrackParameters) const;
    
    void clearCache() const;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Calculate energy loss in MeV/mm. The radiative effects are scaled by m_radiationScale (1=mean, 0.5=mean(log10), 0.1=mpv)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double
      dEds( double p) const;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Momentum smearing (simulation mode)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void smear( double& phi, double& theta, const Trk::TrackParameters* parms, double radDist) const;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Bremstrahlung (simulation mode)  
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void sampleBrem( double mom) const; 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // propagation of neutrals (simulation mode)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const Trk::TrackParameters*  propagateNeutral(const Trk::TrackParameters&   parm,
						  std::vector<DestSurf>&        targetSurfaces,
						  Trk::PropDirection            propagationDirection,
						  std::vector<unsigned int>&    solutions,
						  double&                  path,
						  bool                     usePathLimit,
						  bool                     returnCurv=false) const;


    double                         m_tolerance;         //!< Error tolerance. Low tolerance gives high accuracy
    bool                           m_materialEffects;   //!< Switch material effects on or off
    bool                           m_includeBgradients; //!< Include B-gradients in the error propagation
    bool                           m_includeGgradient;  //!< Include g-gradient in the error propagation
    double                         m_momentumCutOff;    //!< Stop propagation below this momentum
    bool                           m_multipleScattering;//!< Switch multiple scattering on or off
    bool                           m_energyLoss;
    mutable bool                   m_detailedEloss;
    bool                           m_straggling;
    bool                           m_MPV;
    double                         m_stragglingScale;
    double                         m_scatteringScale;
    double                         m_maxPath;
    double                         m_maxSteps;
    double                         m_layXmax;

    //mutable const Trk::AlignableTrackingVolume* m_aliTV;
    mutable const Trk::BinnedMaterial*   m_binMat;    
    mutable std::vector<const Trk::TrackStateOnSurface*>*      m_matstates; //!< cache of TrackStateOnSurfaces
    mutable std::vector<std::pair<const Trk::TrackParameters*,int> >*  m_identifiedParameters; //!< cache of intersections
    mutable std::vector<Trk::HitInfo>*  m_hitVector; //!< cache of intersections/hit info
    mutable size_t                 m_currentLayerBin;
    mutable double                 m_matupd_lastmom;   
    mutable double                 m_matupd_lastpath;   
    mutable double                 m_matdump_lastpath;   
    mutable double                 m_delRad;           // deRad/dl;
    mutable double                 m_delIoni;           // deIoni/dl;
    mutable double                 m_sigmaIoni;        // dsigma(ioni)/dl;
    mutable double                 m_kazL;            // kazL constant;
    mutable double                 m_sigmaRad;         // dsigma(rad)/dl;
    mutable double                 m_stragglingVariance;
    mutable Trk::EnergyLoss        m_combinedEloss;
    mutable double                 m_combinedThickness;

    // cache for combined covariance matrix contribution
    mutable AmgSymMatrix(5)        m_combinedCovariance; 
    // cache for differential covariance matrix contribution ( partial material dump )
    mutable AmgSymMatrix(5)        m_covariance; 
    // cache for input variance
    mutable double                 m_inputThetaVariance;

    Trk::MaterialInteraction       m_matInt;

    // simulation mode
    bool                              m_simulation;
    ToolHandle<ITimedMatEffUpdator>   m_simMatUpdator;           //!< secondary interactions (brem photon emission)
 
    /** Random Generator service */
    ServiceHandle<IAtRndmGenSvc>       m_rndGenSvc;
    
    /** Random engine */
    CLHEP::HepRandomEngine*           m_randomEngine;
    std::string                       m_randomEngineName;
    //
    mutable double                    m_pathLimit;
    mutable int                       m_propagateWithPathLimit; 
    mutable double                    m_timeOfFlight;
    mutable double                    m_timeStep;
    mutable double                    m_particleMass;      //!< cache 
    mutable double                    m_charge; 
    static  ParticleMasses                 s_particleMasses;    //!< Struct of Particle masses
    mutable bool                           m_matPropOK;         //!< Switch for turning off material effects temporarily
    mutable ParticleHypothesis             m_particle;
    mutable const TrackingVolume*          m_trackingVolume;
    mutable const Material*                m_material;
    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
    MagField::IMagFieldSvc*                m_fieldService;
    mutable bool                           m_solenoid;

    // caches
    mutable std::vector<std::pair<int,std::pair<double,double> > > m_currentDist;
    unsigned int m_maxCurrentDist;
    mutable double m_P[45];

    //secondary interactions
    mutable double                         m_timeIn;
    mutable bool                           m_brem;
    mutable double                         m_bremMom; 
    mutable double                         m_bremEmitThreshold; 
    mutable double                         m_bremSampleThreshold; 

    mutable Trk::ExtrapolationCache*                           m_extrapolationCache;      //!< cache for collecting the total X0 ans Elos

    void getField        (double*,double*        ) const;
    void getFieldGradient(double*,double*,double*) const;

  };
 /////////////////////////////////////////////////////////////////////////////////
 // Inline methods for magnetic field information
 /////////////////////////////////////////////////////////////////////////////////
	
  inline void STEP_Propagator::getField(double* R,double* H) const
  {
    //if(m_solenoid) return m_fieldService->getFieldZR(R,H);
    return                m_fieldService->getField  (R,H);
  }
  
  inline void STEP_Propagator::getFieldGradient(double* R,double* H,double* dH) const
  {
    //if(m_solenoid) return m_fieldService->getFieldZR(R,H,dH);
    return                m_fieldService->getField  (R,H,dH);
  }
}

#endif // STEP_Propagator_H
