/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Implementation file for class Trk::STEP_Propagator
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Based on RungeKuttaPropagator Version 1.0 21/05/2004 I.Gavrilenko
// Version 0.1 16/2/2005 Esben Lund
// Version 1.0 27/7/2006 Esben Lund
/////////////////////////////////////////////////////////////////////////////////

#include "TrkExSTEP_Propagator/STEP_Propagator.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/BinnedMaterial.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/RungeKuttaUtils.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkExInterfaces/ITimedMatEffUpdator.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkExUtils/ExtrapolationCache.h"
// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
//Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

/// enables -ftree-vectorize in gcc
#include "CxxUtils/vectorize.h"
ATH_ENABLE_VECTORIZATION;

//static particle masses
namespace{
  constexpr Trk::ParticleMasses s_particleMasses{};
}

/////////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////////

Trk::STEP_Propagator::STEP_Propagator
(const std::string& p,const std::string& n,const IInterface* t) :
  AthAlgTool(p,n,t),
  m_tolerance( 1e-5),         //Error tolerance of the propagator. A low tolerance gives a high accuracy.
  m_materialEffects( true),   //Switches off all material effects if false. Does nothing when true.
  m_includeBgradients( true), //Include B-field gradients into the error propagation?
  m_includeGgradient( false), //Include dg/dlambda into the error propagation? Only relevant when energy loss is true.
  m_momentumCutOff( 50.),     //Minimum allowed momentum in MeV.
  m_multipleScattering(true), //Add multiple scattering to the covariance matrix?
  m_energyLoss(true),         //Include energy loss?
  m_detailedEloss(true),      //Provide the extended EnergyLoss object with MopIonization etc.
  m_straggling(true),         //Add energy loss fluctuations (straggling) to the covariance matrix?
  m_MPV(false),                //Use the most probable value of the energy loss, else use the mean energy loss.
  m_stragglingScale(1.),      //Scale for adjusting the width of the energy loss fluctuations.
  m_scatteringScale(1.),      //Scale for adjusting the multiple scattering contribution to the covariance matrix.
  m_maxPath(100000.),         //Maximum propagation length in mm.
  m_maxSteps(10000),          //Maximum number of allowed steps (to avoid infinite loops).
  m_layXmax(1.),              // maximal layer thickness for multiple scattering calculations
  m_simulation(false),        //flag for simulation mode 
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_randomEngine(nullptr),
  m_randomEngineName("FatrasRnd")
{
  declareInterface<Trk::IPropagator>(this);
  declareProperty( "Tolerance",          m_tolerance);
  declareProperty( "MaterialEffects",    m_materialEffects);
  declareProperty( "IncludeBgradients",  m_includeBgradients);
  declareProperty( "IncludeGgradient",   m_includeGgradient);
  declareProperty( "MomentumCutOff",     m_momentumCutOff);
  declareProperty( "MultipleScattering", m_multipleScattering);
  declareProperty( "EnergyLoss",         m_energyLoss);
  declareProperty( "Straggling",         m_straggling);
  declareProperty( "MostProbableEnergyLoss", m_MPV);
  declareProperty( "StragglingScale",    m_stragglingScale);
  declareProperty( "DetailedEloss",      m_detailedEloss);
  declareProperty( "MultipleScatteringScale", m_scatteringScale);
  declareProperty( "MaxPath",            m_maxPath);
  declareProperty( "MaxSteps",           m_maxSteps);
  declareProperty( "MSstepMax",          m_layXmax);
  declareProperty( "SimulationMode",     m_simulation);
  declareProperty( "SimMatEffUpdator",   m_simMatUpdator);
  declareProperty( "RandomNumberService", m_rndGenSvc               , "Random number generator");
  declareProperty( "RandomStreamName"   , m_randomEngineName        , "Name of the random number stream");
}


/////////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////////

Trk::STEP_Propagator::~STEP_Propagator()= default;

// Athena standard methods
// initialize
StatusCode Trk::STEP_Propagator::initialize()
{
  ATH_MSG_VERBOSE(" STEP_Propagator initialize() successful" );

  // Read handle for AtlasFieldCacheCondObj
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
    
  if (!m_materialEffects) { //override all material interactions
    m_multipleScattering = false;
    m_energyLoss = false;
    m_straggling = false;
  }
  else if (!m_energyLoss) { //override straggling
    m_straggling = false;
  }

  if ( m_simulation && m_simMatUpdator.retrieve().isFailure() ) {
    ATH_MSG_WARNING( "Simulation mode requested but material updator not found - no brem photon emission." );
  }

  if (m_simulation) {
    // get the random generator serice
    if (m_rndGenSvc.retrieve().isFailure()){
      ATH_MSG_WARNING( "Could not retrieve " << m_rndGenSvc <<", no smearing done." );
      m_randomEngine = nullptr;
    } else {
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );

      //Get own engine with own seeds:
      m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
      if (!m_randomEngine) {
        ATH_MSG_WARNING( "Could not get random engine '" << m_randomEngineName << "', no smearing done."  );
      }   
    } 
  }

  return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::STEP_Propagator::finalize()
{
  ATH_MSG_VERBOSE(" finalize() successful" );
  return StatusCode::SUCCESS;
}


/** Main propagation method NeutralParameters. Use StraightLinePropagator for neutrals*/
Trk::NeutralParameters*
Trk::STEP_Propagator::propagate (const Trk::NeutralParameters&,
                                 const Trk::Surface&,
                                 Trk::PropDirection,
                                 const Trk::BoundaryCheck&,
                                 bool) const
{
  ATH_MSG_WARNING( "[STEP_Propagator] STEP_Propagator does not handle neutral track parameters." 
                   <<"Use the StraightLinePropagator instead." );
  return nullptr;
}


/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagate (const EventContext&            ctx,
                                 const Trk::TrackParameters&    trackParameters,
                                 const Trk::Surface&            targetSurface,
                                 Trk::PropDirection             propagationDirection,
                                 const Trk::BoundaryCheck&           boundaryCheck,
                                 const MagneticFieldProperties& magneticFieldProperties,
                                 ParticleHypothesis             particle,
                                 bool                           returnCurv,
                                 const Trk::TrackingVolume*     tVol) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 1");

  double Jacobian[25];
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  cache.m_matupd_lastmom = trackParameters.momentum().mag();
  cache.m_matupd_lastpath = 0.;
  cache.m_matdump_lastpath = 0.;

  // no identified intersections needed/ no material dump / no path cache
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = nullptr;

  return propagateRungeKutta( cache,true, trackParameters, targetSurface, propagationDirection,
                              magneticFieldProperties, particle, boundaryCheck, Jacobian, returnCurv);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
// with search of closest surface (ST) 
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagate (const EventContext&                 ctx,
                                 const Trk::TrackParameters&         trackParameters,
                                 std::vector<DestSurf>&              targetSurfaces,
                                 Trk::PropDirection                  propagationDirection,
                                 const Trk::MagneticFieldProperties& magneticFieldProperties,
                                 ParticleHypothesis                  particle,
                                 std::vector<unsigned int>&          solutions,
                                 double&                             path,
                                 bool                                usePathLimit,
                                 bool                                returnCurv,
                                 const Trk::TrackingVolume*          tVol) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 2");

  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = nullptr;

  cache.m_matupd_lastmom = trackParameters.momentum().mag();
  cache.m_matupd_lastpath = 0.;
  cache.m_matdump_lastpath = 0.;

  // resolve path limit input
  if (path>0.) {
    cache.m_propagateWithPathLimit = usePathLimit? 1 : 0;
    cache.m_pathLimit = path;
    path = 0.;
  } else {
    cache.m_propagateWithPathLimit = 0;
    cache.m_pathLimit = -1.;
    path = 0.; 
  } 
  if ( particle==Trk::neutron || particle==Trk::photon || particle==Trk::pi0 || particle==Trk::k0 )
    return propagateNeutral(trackParameters,targetSurfaces,propagationDirection,solutions,path,usePathLimit,returnCurv);

  return propagateRungeKutta( cache,true, trackParameters, targetSurfaces, propagationDirection, magneticFieldProperties,
                              particle, solutions, path, returnCurv);  
}


/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
// with search of closest surface and time info (ST) 
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagateT (const EventContext&                 ctx,
                                  const Trk::TrackParameters&         trackParameters,
                                  std::vector<DestSurf>&              targetSurfaces,
                                  Trk::PropDirection                  propagationDirection,
                                  const Trk::MagneticFieldProperties& magneticFieldProperties,
                                  ParticleHypothesis                  particle,
                                  std::vector<unsigned int>&          solutions,
                                  PathLimit&                          pathLim,
                                  TimeLimit&                          timeLim,
                                  bool                                returnCurv,
                                  const Trk::TrackingVolume*          tVol,
                                  std::vector<Trk::HitInfo>*&         hitVector) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 3");

  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  // cache particle mass
  cache.m_particleMass = s_particleMasses.mass[particle]; //Get particle mass from ParticleHypothesis

  // cache input timing - for secondary track emission
  cache.m_timeIn = timeLim.time;

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = hitVector;

  cache.m_matupd_lastmom = trackParameters.momentum().mag();
  cache.m_matupd_lastpath = 0.;
  cache.m_matdump_lastpath = 0.;

  // convert time/path limits into trajectory limit (in mm)
  double dMat = pathLim.x0Max-pathLim.x0Collected;
  double path = dMat>0 && cache.m_matPropOK && cache.m_material->x0()>0. ? dMat * cache.m_material->x0() : -1.;

  double dTim = timeLim.tMax - timeLim.time; 
  double beta = 1.;
  if (dTim>0.) {
    double mom = trackParameters.momentum().mag(); 
    beta = mom/std::sqrt(mom*mom+cache.m_particleMass*cache.m_particleMass);
  }
  double timMax = dTim>0 ?  dTim*beta*Gaudi::Units::c_light : -1.;

  if ( timMax>0. && timMax < path ) path = timMax;
  bool usePathLimit = (path > 0.);    

  // resolve path limit input
  if (path>0.) {
    cache.m_propagateWithPathLimit = usePathLimit? 1 : 0;
    cache.m_pathLimit = path;
    path = 0.;
  } else {
    cache.m_propagateWithPathLimit = 0;
    cache.m_pathLimit = -1.;
    path = 0.; 
  } 

  Trk::TrackParameters* nextPar = nullptr;

  if ( particle==Trk::neutron || particle==Trk::photon || particle==Trk::pi0 || particle==Trk::k0 ){
    nextPar = propagateNeutral(trackParameters,targetSurfaces,propagationDirection,
                               solutions,path,usePathLimit,returnCurv);
  } else{ 
    nextPar =  propagateRungeKutta( cache,true, trackParameters, targetSurfaces, propagationDirection, 
                                    magneticFieldProperties,
                                    particle, solutions, path,returnCurv);  
  }
  // update material path
  if (cache.m_matPropOK && cache.m_material->x0()>0. && path>0.) {
    pathLim.updateMat( path/cache.m_material->x0(),cache.m_material->averageZ(),0.);   
  }
  // return value
  timeLim.time +=cache.m_timeOfFlight;
  return nextPar;   
}


/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
// with search of closest surface and material collection (ST) 
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagateM (const EventContext&                                        ctx,
                                  const Trk::TrackParameters&                                trackParameters,
                                  std::vector<DestSurf>&                                     targetSurfaces,
                                  Trk::PropDirection                                         propagationDirection,
                                  const Trk::MagneticFieldProperties&                        magneticFieldProperties,
                                  ParticleHypothesis                                         particle,
                                  std::vector<unsigned int>&                                 solutions,
                                  std::vector<const Trk::TrackStateOnSurface*>*&             matstates,
                                  std::vector<std::pair<const Trk::TrackParameters*,int> >*& intersections,
                                  double&                                                    path,
                                  bool                                                       usePathLimit,
                                  bool                                                       returnCurv,
                                  const Trk::TrackingVolume*                                 tVol,
                                  Trk::ExtrapolationCache*  extrapCache) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 4");

  Cache cache{};

    // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  cache.m_matstates = matstates;
  cache.m_identifiedParameters = intersections;
  cache.m_extrapolationCache = extrapCache;
  cache.m_hitVector = nullptr;

  cache.m_matupd_lastmom = trackParameters.momentum().mag();
  cache.m_matupd_lastpath = 0.;
  cache.m_matdump_lastpath = 0.;
  cache.m_extrapolationCache=extrapCache;

  // switch on the detailed energy loss
  if (cache.m_extrapolationCache) {
    cache.m_detailedElossFlag=true;
  }
  // resolve path limit input
  if (path>0.) {
    cache.m_propagateWithPathLimit = usePathLimit? 1 : 0;
    cache.m_pathLimit = path;
    path = 0.;
  } else {
    cache.m_propagateWithPathLimit = 0;
    cache.m_pathLimit = -1.;
    path = 0.; 
  } 
  if ( particle==Trk::neutron || particle==Trk::photon || particle==Trk::pi0 || particle==Trk::k0 ){
    return propagateNeutral(trackParameters,targetSurfaces,propagationDirection,solutions,path,
                            usePathLimit,returnCurv);
  }
  return propagateRungeKutta( cache,true, trackParameters, targetSurfaces, 
                              propagationDirection, magneticFieldProperties,
                              particle, solutions, path,returnCurv);  
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation.
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagate (const EventContext&                 ctx,
                                 const Trk::TrackParameters&         trackParameters,
                                 const Trk::Surface&                 targetSurface,
                                 Trk::PropDirection                  propagationDirection,
                                 const Trk::BoundaryCheck&           boundaryCheck,
                                 const Trk::MagneticFieldProperties& magneticFieldProperties,
                                 Trk::TransportJacobian*&            jacobian,
                                 double&,
                                 ParticleHypothesis                  particle,
                                 bool                                returnCurv,
                                 const Trk::TrackingVolume*          tVol) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 5");

  double Jacobian[25];
  Cache cache{};

    // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = nullptr;

  cache.m_matupd_lastmom = trackParameters.momentum().mag();
  cache.m_matupd_lastpath = 0.;
  cache.m_matdump_lastpath = 0.;

  Trk::TrackParameters* parameters = propagateRungeKutta(cache, true, trackParameters, targetSurface, 
                                                               propagationDirection,magneticFieldProperties, 
                                                               particle, boundaryCheck, Jacobian, returnCurv);

  if (parameters) {
    Jacobian[24]=Jacobian[20]; Jacobian[23]=0.; Jacobian[22]=0.; Jacobian[21]=0.; Jacobian[20]=0.;
    jacobian = new Trk::TransportJacobian(Jacobian);
  } else {
    jacobian = nullptr;
  }

  return parameters;
}


/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters propagation without covariance matrix
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagateParameters (const EventContext&                 ctx,
                                           const Trk::TrackParameters&         trackParameters,
                                           const Trk::Surface&                 targetSurface,
                                           Trk::PropDirection                  propagationDirection,
                                           const Trk::BoundaryCheck&           boundaryCheck,
                                           const Trk::MagneticFieldProperties& magneticFieldProperties,
                                           ParticleHypothesis                  particle,
                                           bool                                returnCurv,
                                           const Trk::TrackingVolume*          tVol) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 6");

  double Jacobian[25];
  Cache cache{};

    // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 


  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_hitVector = nullptr;

  return propagateRungeKutta( cache,false, trackParameters, 
                              targetSurface, propagationDirection,
                              magneticFieldProperties, particle, boundaryCheck, 
                              Jacobian, returnCurv);
}


/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters propagation without covariance matrix.
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagateParameters (const EventContext&                 ctx,
                                           const Trk::TrackParameters&         trackParameters,
                                           const Trk::Surface&                 targetSurface,
                                           Trk::PropDirection                  propagationDirection,
                                           const Trk::BoundaryCheck&           boundaryCheck,
                                           const Trk::MagneticFieldProperties& magneticFieldProperties,
                                           Trk::TransportJacobian*&            jacobian,
                                           ParticleHypothesis                  particle,
                                           bool                                returnCurv,
                                           const Trk::TrackingVolume*          tVol) const
{

    // ATH_MSG_WARNING( "[STEP_Propagator] enter 7");
 
 double Jacobian[25];

  Cache cache{};

    // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = nullptr;

  Trk::TrackParameters* parameters = propagateRungeKutta( cache,true, trackParameters, targetSurface, 
                                                                propagationDirection,magneticFieldProperties,
                                                                particle, boundaryCheck, Jacobian, returnCurv);

  if (parameters) { 
    Jacobian[24]=Jacobian[20]; Jacobian[23]=0.; Jacobian[22]=0.; Jacobian[21]=0.; Jacobian[20]=0.;
    jacobian = new Trk::TransportJacobian(Jacobian);
  } else {
    jacobian = nullptr;
  }

  return parameters;
}


/////////////////////////////////////////////////////////////////////////////////
// Function for finding the intersection point with a surface
/////////////////////////////////////////////////////////////////////////////////

const Trk::IntersectionSolution*
Trk::STEP_Propagator::intersect (const EventContext&                 ctx,
                                 const Trk::TrackParameters&         trackParameters,
                                 const Trk::Surface&                 targetSurface,
                                 const Trk::MagneticFieldProperties& mft,
                                 ParticleHypothesis                  particle,
                                 const Trk::TrackingVolume*          tVol) const
{

  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 


  cache.m_particle = particle; //Store for later use

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  // no identified intersections needed/ no material dump
  cache.m_identifiedParameters = nullptr;
  cache.m_matstates = nullptr;
  cache.m_extrapolationCache = nullptr;
  cache.m_hitVector = nullptr;

  // Bfield mode
  mft.magneticFieldMode() == Trk::FastField ? cache.m_solenoid = true : cache.m_solenoid = false;  

  //Check inputvalues
  if (m_tolerance <= 0.) return nullptr;
  if (m_momentumCutOff < 0. ) return nullptr;
  if (fabs( 1./trackParameters.parameters()[Trk::qOverP]) <= m_momentumCutOff) {
    return nullptr;
  }

  //Check for empty volumes. If x != x then x is not a number.
  if (cache.m_matPropOK && (
                            (cache.m_material->zOverAtimesRho() == 0.) || (cache.m_material->x0() == 0.) ||
                            (cache.m_material->zOverAtimesRho() != cache.m_material->zOverAtimesRho()))) {
    cache.m_matPropOK = false;
  }

  //double P[45];
  if (!Trk::RungeKuttaUtils::transformLocalToGlobal( false, trackParameters, cache.m_P)) return nullptr;
  double path = 0.;

  const Amg::Transform3D&  T = targetSurface.transform();   
  Trk::Surface::SurfaceType ty = targetSurface.type();  

  if  (ty == Trk::Surface::Plane || ty == Trk::Surface::Disc ) { 
    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2); 

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;} 
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;} 
    if (!propagateWithJacobian(cache, false, ty, s, cache.m_P, path)) return nullptr;
  }

  else if (ty == Trk::Surface::Line     ) { 

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)}; 
    if (!propagateWithJacobian( cache,false, ty, s, cache.m_P, path)) return nullptr;
  }

  else if (ty == Trk::Surface::Cylinder ) { 

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(&targetSurface); 
    double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),Trk::alongMomentum,0.}; 
    if (!propagateWithJacobian(cache, false, ty, s, cache.m_P, path)) return nullptr;
  }

  else if (ty == Trk::Surface::Cone     ) { 

    double k     = static_cast<const Trk::ConeSurface*>(&targetSurface)->bounds().tanAlpha(); k = k*k+1.; 
    double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,Trk::alongMomentum,0.}; 
    if (!propagateWithJacobian(cache, false, ty, s, cache.m_P, path)) return nullptr;
  }

  else if (ty == Trk::Surface::Perigee  ) { 

    double s[6] ={T(0,3),T(1,3),T(2,3),0.,0.,1.}; 
    if (!propagateWithJacobian( cache,false, ty, s, cache.m_P, path)) return nullptr;
  }

  else {      // presumably curvilinear

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2); 

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;} 
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;} 
    if (!propagateWithJacobian( cache,false, ty, s, cache.m_P, path)) return nullptr;

  }

  Amg::Vector3D globalPosition( cache.m_P[0],cache.m_P[1],cache.m_P[2]);
  Amg::Vector3D direction( cache.m_P[3],cache.m_P[4],cache.m_P[5]);
  Trk::IntersectionSolution* intersectionSolution = new Trk::IntersectionSolution();
  intersectionSolution->push_back(new Trk::TrackSurfaceIntersection( globalPosition, direction, path));
  return intersectionSolution;
}

const Trk::TrackSurfaceIntersection*
Trk::STEP_Propagator::intersectSurface(const EventContext&             ctx,
                                       const Trk::Surface&         surface,
                                       const Trk::TrackSurfaceIntersection*    
                                       trackIntersection,
                                       const double               qOverP,
                                       const Trk::MagneticFieldProperties& mft,
                                       ParticleHypothesis       particle) const 
{

  Amg::Vector3D origin = trackIntersection->position();
  Amg::Vector3D direction = trackIntersection->direction();

  PerigeeSurface* perigeeSurface  = new PerigeeSurface(origin);
  const Trk::TrackParameters* trackParameters = perigeeSurface->createTrackParameters(0.,0.,
                                                                                      direction.phi(),
                                                                                      direction.theta(),qOverP,nullptr);

  const Trk::IntersectionSolution* solution = qOverP==0? intersect(ctx,
                                                                   *trackParameters,surface,
                                                                   Trk::MagneticFieldProperties(Trk::NoField),
                                                                   particle):intersect(*trackParameters,surface,
                                                                                       mft,particle,nullptr);

  delete perigeeSurface;
  delete trackParameters;
  if(!solution) return nullptr;

  Trk::IntersectionSolutionIter output_iter = solution->begin();
  if(*output_iter) {
    Trk::TrackSurfaceIntersection*  result =   new Trk::TrackSurfaceIntersection(*(*output_iter));
    delete solution;   
    return result; 
  } 
  delete solution;   
  return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////
// Global positions calculation inside CylinderBounds
// if max step allowed > 0 -> propagate along momentum else propagate opposite momentum
/////////////////////////////////////////////////////////////////////////////////

void
Trk::STEP_Propagator::globalPositions ( const EventContext&                 ctx,
                                        std::list<Amg::Vector3D>&           positionsList,
                                        const Trk::TrackParameters&         trackParameters,
                                        const Trk::MagneticFieldProperties& mft,
                                        const Trk::CylinderBounds&          cylinderBounds,
                                        double                              maxStepSize,
                                        ParticleHypothesis                  particle,
                                        const Trk::TrackingVolume*          tVol) const
{
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_detailedElossFlag=m_detailedEloss;
  clearCache(cache); 


  cache.m_particle = particle; //Store for later use

  //Check for tracking volume (materialproperties)
  cache.m_trackingVolume = tVol;
  cache.m_material = tVol;
  cache.m_matPropOK = tVol != nullptr;

  //Check for empty volumes. If x != x then x is not a number.
  if (cache.m_matPropOK && ((cache.m_material->zOverAtimesRho() == 0.) || (cache.m_material->x0() == 0.) ||
                            (cache.m_material->zOverAtimesRho() != cache.m_material->zOverAtimesRho()))) {
    cache.m_matPropOK = false;
  }

  mft.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  

  //Check inputvalues
  if (m_tolerance <= 0.) return;

  double       PP[7];
  if (!Trk::RungeKuttaUtils::transformLocalToGlobal( false, trackParameters, PP)) return;

  double       maxPath = m_maxPath;  			// Max path allowed
  double       dDir[3]   = { 0., 0., 0.};		// Start directions derivs. Zero in case of no RK steps
  double       distanceStepped = 0.;
  float        BG1[12] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; // Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz,
  // dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz
  bool         firstStep = true;                        // Poll B1, else recycle B4
  double       path = 0.;  				// path of the trajectory
  double       radius2Max = cylinderBounds.r()*cylinderBounds.r();// max. radius**2 of region
  double       zMax = cylinderBounds.halflengthZ();     // max. Z of region
  double       radius2 = PP[0]*PP[0]+PP[1]*PP[1];           // Start radius**2
  double       direction = PP[0]*PP[3]+PP[1]*PP[4];         // Direction
  double       h = maxStepSize;                         // max step allowed

  // Test position of the track
  if ((fabs(PP[2]) > zMax) || (radius2 > radius2Max)) return;

  //Store initial position
  Amg::Vector3D initialPosition(PP[0],PP[1],PP[2]);
  positionsList.push_back(initialPosition);

  bool perigee = false;
  if (fabs(direction) < 0.00001) {
    perigee = true;
  }

  for (int i=0; i!=2; ++i) {
    if (i) {
      if (perigee) return;
      h = -h;
    }
    double p[7] = {PP[0],PP[1],PP[2],PP[3],PP[4],PP[5],PP[6]};

    while (fabs(path) < maxPath) {
      //Do the step.
      if (!rungeKuttaStep( cache,false, h, p, dDir, BG1, firstStep, distanceStepped)) break;
      path = path + distanceStepped;

      //Keep h within max stepsize
      if (h > maxStepSize) {
        h = maxStepSize;
      } else if (h < -maxStepSize) {
        h = -maxStepSize;
      }

      //store current step
      Amg::Vector3D globalPosition( p[0],p[1],p[2]);
      if (!i) {
        positionsList.push_back(globalPosition);
      }
      else {
        positionsList.push_front(globalPosition);
      }

      // Test position of the track
      radius2 = p[0]*p[0]+p[1]*p[1];
      if ((fabs( p[2]) > zMax) || (radius2 > radius2Max)) break;

      // Test perigee
      if ((p[0]*p[3] + p[1]*p[4])*direction < 0.) {
        if (i) return;
        perigee = true;
      }
    }
  }
  }


/////////////////////////////////////////////////////////////////////////////////
// Main function for track propagation with or without jacobian
/////////////////////////////////////////////////////////////////////////////////

// Force this function to be optimized, even in unoptimized builds:
// this makes heavy use of Eigen, and Eigen is extremely slow if optimization
// is disabled.  If you do need to debug this function, you can comment
// this out.
#if defined(__GNUC__) && !defined(__OPTIMIZE__)
__attribute__ ((optimize(2)))
#endif
Trk::TrackParameters*
Trk::STEP_Propagator::propagateRungeKutta (Cache&                              cache,           
                                           bool 	                             errorPropagation,
                                           const Trk::TrackParameters&         inputTrackParameters,
                                           const Trk::Surface&                 targetSurface,
                                           Trk::PropDirection                  propagationDirection,
                                           const Trk::MagneticFieldProperties& mft,
                                           ParticleHypothesis                  particle,
                                           const Trk::BoundaryCheck&                  boundaryCheck,
                                           double*                             Jacobian,
                                           bool                                returnCurv) const
{
  //Store for later use
  cache.m_particle = particle;
  cache.m_charge   = inputTrackParameters.charge();

  const Trk::TrackParameters* trackParameters = nullptr;

  // Bfield mode
  mft.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  

  //Check inputvalues
  if (m_tolerance <= 0.) return nullptr;
  if (m_momentumCutOff < 0.) return nullptr;

  //Set momentum to 1e10 (straight line) and charge to + if q/p is zero
  if (inputTrackParameters.parameters()[Trk::qOverP] == 0) {
    trackParameters = createStraightLine( &inputTrackParameters);
    if (!trackParameters) {
      return nullptr;
    }
  }
  else {
    trackParameters = &inputTrackParameters;
  }

  if (fabs( 1./trackParameters->parameters()[Trk::qOverP]) <= m_momentumCutOff) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }

  //Check for empty volumes. If x != x then x is not a number.
  if (cache.m_matPropOK && ((cache.m_material->zOverAtimesRho() == 0.) || (cache.m_material->x0() == 0.) ||
                            (cache.m_material->zOverAtimesRho() != cache.m_material->zOverAtimesRho()))) {
    cache.m_matPropOK = false;
  }
  if (cache.m_matPropOK && m_straggling) cache.m_stragglingVariance = 0.;
  if (errorPropagation || cache.m_matstates)  {
    cache.m_combinedCovariance.setZero();
    cache.m_covariance.setZero();
    // this needs debugging
    cache.m_inputThetaVariance = trackParameters->covariance() ?  (*trackParameters->covariance())(3,3) : 0.;
    cache.m_combinedEloss.set(0.,0.,0.,0.,0.,0.);
    cache.m_combinedThickness=0.;
  }

  if (!Trk::RungeKuttaUtils::transformLocalToGlobal( errorPropagation, *trackParameters,cache.m_P)) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }
  double path = 0.;

  const Amg::Transform3D&  T = targetSurface.transform();   
  Trk::Surface::SurfaceType ty = targetSurface.type();  

  if  (ty == Trk::Surface::Plane || ty == Trk::Surface::Disc ) { 
    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2); 

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;} 
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;} 
    if (!propagateWithJacobian(cache,errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    }
  }

  else if (ty == Trk::Surface::Line     ) { 

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)}; 
    if (!propagateWithJacobian( cache,errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    }
  }

  else if (ty == Trk::Surface::Cylinder ) { 

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(&targetSurface); 
    double s[9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),(double)propagationDirection,0.}; 
    if (!propagateWithJacobian( cache,errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    } 
  }

  else if (ty == Trk::Surface::Cone     ) { 

    double k     = static_cast<const Trk::ConeSurface*>(&targetSurface)->bounds().tanAlpha(); k = k*k+1.; 
    double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,(double)propagationDirection,0.}; 
    if (!propagateWithJacobian(cache, errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    } 
  }

  else if (ty == Trk::Surface::Perigee  ) { 

    double s[6] ={T(0,3),T(1,3),T(2,3),0.,0.,1.}; 
    if (!propagateWithJacobian(cache, errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    } 
  }

  else {      // presumably curvilinear

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2); 

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;} 
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;} 
    if (!propagateWithJacobian(cache, errorPropagation, ty, s, cache.m_P, path)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    } 
  }

  if (propagationDirection * path < 0.) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }

  double localp[5];
  // output in curvilinear parameters 
  if (returnCurv || ty==Trk::Surface::Cone)  {

    Trk::RungeKuttaUtils::transformGlobalToLocal(cache.m_P,localp);
    Amg::Vector3D gp(cache.m_P[0],cache.m_P[1],cache.m_P[2]);

    if ( boundaryCheck && !targetSurface.isOnSurface(gp) ) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;    
    }

    if ( !errorPropagation || !trackParameters->covariance() ) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return new Trk::CurvilinearParameters(gp, localp[2], localp[3], localp[4]);
    }

    double useless[2];
    Trk::RungeKuttaUtils::transformGlobalToCurvilinear( true, cache.m_P, useless, Jacobian);
    AmgSymMatrix(5)* measurementCovariance = Trk::RungeKuttaUtils::newCovarianceMatrix(
                                                                                 Jacobian, *trackParameters->covariance());

    if (cache.m_matPropOK && (m_multipleScattering || m_straggling) && fabs(path)>0. ) 
      covarianceContribution( cache,trackParameters, path, fabs( 1./cache.m_P[6]), measurementCovariance);

    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return new Trk::CurvilinearParameters(gp,localp[2],localp[3],localp[4],measurementCovariance); 
  }

  // Common transformation for all surfaces 
  Trk::RungeKuttaUtils::transformGlobalToLocal(&targetSurface,errorPropagation,cache.m_P,localp,Jacobian);

  if (boundaryCheck) {
    Amg::Vector2D localPosition( localp[0], localp[1]);
    if (!targetSurface.insideBounds( localPosition, 0.)) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    }
  }

  Trk::TrackParameters* onTargetSurf = targetSurface.createTrackParameters(localp[0],localp[1],localp[2],
                                                                                 localp[3],localp[4],nullptr); 

  if ( !errorPropagation || !trackParameters->covariance() ) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return onTargetSurf; 
  }

  //Errormatrix is included. Use Jacobian to calculate new covariance
  AmgSymMatrix(5)* measurementCovariance = Trk::RungeKuttaUtils::newCovarianceMatrix(
                                                                               Jacobian, *trackParameters->covariance());

  //Calculate multiple scattering and straggling covariance contribution.
  if (cache.m_matPropOK && (m_multipleScattering || m_straggling) && fabs(path)>0. ) 
    covarianceContribution( cache,trackParameters, path, onTargetSurf, measurementCovariance);
  delete onTargetSurf;                   // the covariance matrix can be just added instead of recreating ?

  if (trackParameters != &inputTrackParameters) delete trackParameters;
  return targetSurface.createTrackParameters(localp[0],localp[1],localp[2],localp[3],localp[4],measurementCovariance); 
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track propagation with or without jacobian
// with search of closest surface
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters*
Trk::STEP_Propagator::propagateRungeKutta ( Cache&                              cache,
                                            bool 	                              errorPropagation,
                                            const Trk::TrackParameters&         inputTrackParameters,
                                            std::vector< DestSurf>&             targetSurfaces,
                                            Trk::PropDirection            propagationDirection,
                                            const Trk::MagneticFieldProperties& mft,
                                            ParticleHypothesis       particle,
                                            std::vector<unsigned int>&          solutions,
                                            double&                  totalPath,
                                            bool                     returnCurv) const
{
  //Store for later use
  cache.m_particle = particle; 
  cache.m_charge   = inputTrackParameters.charge();
  cache.m_inputThetaVariance = 0.;

  const Trk::TrackParameters* trackParameters = nullptr;

  // Bfield mode
  mft.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  

  //Check inputvalues
  if (m_tolerance <= 0.) return nullptr;
  if (m_momentumCutOff < 0.) return nullptr;

  //Set momentum to 1e10 (straight line) and charge to + if q/p is zero
  if (inputTrackParameters.parameters()[Trk::qOverP] == 0) {
    trackParameters = createStraightLine( &inputTrackParameters);
    if (!trackParameters) {
      return nullptr;
    }
  }
  else {
    trackParameters = &inputTrackParameters;
  }

  if (fabs( 1./trackParameters->parameters()[Trk::qOverP]) <= m_momentumCutOff) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }

  //Check for empty volumes. If x != x then x is not a number.
  if (cache.m_matPropOK && ((cache.m_material->zOverAtimesRho() == 0.) || (cache.m_material->x0() == 0.) ||
                            (cache.m_material->zOverAtimesRho() !=cache.m_material->zOverAtimesRho()))) {
    cache.m_matPropOK = false;
  }

  if (errorPropagation && !trackParameters->covariance() ) {
    errorPropagation = false;
  }

  if (cache.m_matPropOK && errorPropagation && m_straggling) cache.m_stragglingVariance = 0.;
  cache.m_combinedCovariance.setZero();
  cache.m_covariance.setZero();

  if (errorPropagation || cache.m_matstates)  {
    // this needs debugging
    cache.m_inputThetaVariance = trackParameters->covariance() ? (*trackParameters->covariance())(3,3) : 0.;
    cache.m_combinedEloss.set(0.,0.,0.,0.,0.,0.);
    cache.m_combinedThickness=0.;
  }

  //double P[45]; // Track parameters and jacobian
  if (!Trk::RungeKuttaUtils::transformLocalToGlobal( errorPropagation, *trackParameters, cache.m_P)) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }

  double path = 0.;

  // activate brem photon emission if required
  cache.m_brem = m_simulation && particle==Trk::electron && m_simMatUpdator && m_randomEngine;

  // loop while valid solutions
  bool validStep = true;
  totalPath = 0.; cache.m_timeOfFlight = 0.;
  // Common transformation for all surfaces (angles and momentum)
  double localp[5];
  double Jacobian[21];
  while ( validStep ) { 
    // propagation to next surface
    validStep = propagateWithJacobian( cache,errorPropagation, targetSurfaces, cache.m_P, 
                                       propagationDirection, solutions, path, totalPath);
    if (!validStep) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    }
    if (propagationDirection * path <= 0.) {
      if (trackParameters != &inputTrackParameters) delete trackParameters;
      return nullptr;
    }
    totalPath += path;  cache.m_timeOfFlight += cache.m_timeStep;
    if (cache.m_propagateWithPathLimit>1 || cache.m_binMat ) {    
      // make sure that for sliding surfaces the result does not get distorted
      // return curvilinear parameters
      Trk::CurvilinearParameters* cPar = nullptr;
      Trk::RungeKuttaUtils::transformGlobalToLocal(cache.m_P, localp);
      if (!errorPropagation) { 
        cPar =  new Trk::CurvilinearParameters(Amg::Vector3D(cache.m_P[0],cache.m_P[1],cache.m_P[2]),
                                               localp[2],localp[3],localp[4]); 
      }	else {
        double useless[2];
        Trk::RungeKuttaUtils::transformGlobalToCurvilinear( true, cache.m_P, useless, Jacobian);
        AmgSymMatrix(5)* measurementCovariance = Trk::RungeKuttaUtils::newCovarianceMatrix(Jacobian,
                                                                                     *trackParameters->covariance());
        //Calculate multiple scattering and straggling covariance contribution.
        if (cache.m_matPropOK && (m_multipleScattering || m_straggling) && fabs(totalPath)>0.) {
          covarianceContribution(cache, trackParameters, totalPath, fabs( 1./cache.m_P[6]), measurementCovariance);
        }
        if (trackParameters != &inputTrackParameters) delete trackParameters;
        cPar = new Trk::CurvilinearParameters(Amg::Vector3D(cache.m_P[0],cache.m_P[1],cache.m_P[2]),
                                              localp[2],localp[3],localp[4],
                                              measurementCovariance);
      }
      // material collection : first iteration, bin material averaged
      // collect material
      if ( cache.m_binMat && (cache.m_matstates || 
                              (errorPropagation && cache.m_extrapolationCache)) && 
           fabs(totalPath-cache.m_matdump_lastpath)>1.) {
        dumpMaterialEffects( cache,cPar, totalPath);
      }
      return cPar;
    }
    if (cache.m_propagateWithPathLimit>0) cache.m_pathLimit -= path;
    // boundary check
    // take into account that there may be many identical surfaces with different boundaries
    Amg::Vector3D gp(cache.m_P[0],cache.m_P[1],cache.m_P[2]);  
    bool solution = false;
    std::vector<unsigned int> valid_solutions;
    valid_solutions.reserve(solutions.size());

    std::vector<unsigned int>::iterator iSol= solutions.begin();
    while ( iSol != solutions.end() ) {  
      if ( targetSurfaces[*iSol].first->isOnSurface(gp,targetSurfaces[*iSol].second ,0.001,0.001) ) {
        if (!solution) {
          Trk::RungeKuttaUtils::transformGlobalToLocal(cache.m_P, localp);
          if (returnCurv || targetSurfaces[*iSol].first->type()==Trk::Surface::Cone) {
            Trk::RungeKuttaUtils::transformGlobalToCurvilinear(errorPropagation,cache.m_P,localp,Jacobian); 
          } else Trk::RungeKuttaUtils::transformGlobalToLocal(targetSurfaces[*iSol].first,errorPropagation,cache.m_P,localp,Jacobian);
          solution = true;
        }
        valid_solutions.push_back( *iSol );
      } 
      iSol++;
    }
    solutions = valid_solutions;
    if (solution) break;
  }

  if (solutions.empty()) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    return nullptr;
  }

  // simulation mode : smear momentum
  if (m_simulation && cache.m_matPropOK && m_randomEngine ) {
    double radDist = totalPath/cache.m_material->x0();
    smear(cache,localp[2],localp[3],trackParameters,radDist);
  }

  Trk::TrackParameters* onTargetSurf = (returnCurv || targetSurfaces[solutions[0]].first->type()==Trk::Surface::Cone) ? 
    nullptr : targetSurfaces[solutions[0]].first->createTrackParameters(localp[0],localp[1],localp[2],localp[3],localp[4],nullptr);

  if (!errorPropagation) {
    if (trackParameters != &inputTrackParameters) delete trackParameters;
    if (returnCurv || targetSurfaces[solutions[0]].first->type()==Trk::Surface::Cone)  {
      Amg::Vector3D gp(cache.m_P[0],cache.m_P[1],cache.m_P[2]);
      return new Trk::CurvilinearParameters(gp, localp[2], localp[3], localp[4]);
    }
    return onTargetSurf;
  }

  //Errormatrix is included. Use Jacobian to calculate new covariance
  AmgSymMatrix(5)* measurementCovariance = Trk::RungeKuttaUtils::newCovarianceMatrix(
                                                                               Jacobian, *trackParameters->covariance());

  //Calculate multiple scattering and straggling covariance contribution.
  if (cache.m_matPropOK && (m_multipleScattering || m_straggling) && fabs(totalPath)>0.) {
    if (returnCurv || targetSurfaces[solutions[0]].first->type()==Trk::Surface::Cone)  {
      covarianceContribution( cache,trackParameters, totalPath, fabs( 1./cache.m_P[6]), measurementCovariance);
    } else {
      covarianceContribution( cache,trackParameters, totalPath, onTargetSurf, measurementCovariance);
    }
  }

  if (trackParameters != &inputTrackParameters) delete trackParameters;
  if (returnCurv || targetSurfaces[solutions[0]].first->type()==Trk::Surface::Cone)  {
    Amg::Vector3D gp(cache.m_P[0],cache.m_P[1],cache.m_P[2]);
    return new Trk::CurvilinearParameters(gp, localp[2], localp[3], localp[4], measurementCovariance);
  }

  delete onTargetSurf;          // the covariance matrix can be just added instead of recreating ?
  return targetSurfaces[solutions[0]].first->createTrackParameters(localp[0],localp[1],localp[2],localp[3],localp[4],
                                                                   measurementCovariance); 

}


/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta main program for propagation with or without Jacobian
/////////////////////////////////////////////////////////////////////////////////

bool
Trk::STEP_Propagator::propagateWithJacobian (Cache&      cache,      
                                             bool        errorPropagation,
                                             Trk::Surface::SurfaceType surfaceType,
                                             double*     targetSurface,
                                             double*     P,                        
                                             double&     path) const
{
  double       maxPath = m_maxPath;  			// Max path allowed
  double*      pos     = &P[0];  			// Start coordinates
  double*      dir       = &P[3];  			// Start directions
  double       dDir[3]   = { 0., 0., 0.};		// Start directions derivs. Zero in case of no RK steps
  int          targetPassed = 0;      			// how many times have we passed the target?
  double       previousDistance = 0.;
  double       distanceStepped = 0.;
  bool         distanceEstimationSuccessful = false; 	// Was the distance estimation successful?
  float        BG1[12] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; // Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz,
  // dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz at first point
  bool         firstStep = true;                        // Poll BG1, else recycle BG4
  double       absolutePath = 0.;                       // Absolute path to register oscillating behaviour
  int          steps = 0;
  path    = 0.;  				// signed path of the trajectory
  double       mom = 0.;
  double       beta = 1.;

  double distanceToTarget = distance( surfaceType, targetSurface, P, distanceEstimationSuccessful);
  if (!distanceEstimationSuccessful) return false;

  //Set initial step length to 100mm or the distance to the target surface.
  double h;
  distanceToTarget > 100. ? h = 100. : distanceToTarget < -100. ? h = -100. : h = distanceToTarget;

  // Step to within distanceTolerance, then do the rest as a Taylor expansion.
  // Keep distanceTolerance within [1 nanometer, 10 microns].
  // This means that no final Taylor expansions beyond 10 microns and no
  // Runge-Kutta steps less than 1 nanometer are allowed.
  double distanceTolerance = std::min( std::max( fabs( distanceToTarget) * m_tolerance, 1e-6), 1e-2);

  while (fabs( distanceToTarget) > distanceTolerance) { // Step until within tolerance
    //Do the step. Stop the propagation if the energy goes below m_momentumCutOff
    if (!rungeKuttaStep( cache,errorPropagation, h, P, dDir, BG1, firstStep, distanceStepped)) return false;
    path += distanceStepped;
    absolutePath += fabs( distanceStepped);

    if(fabs(distanceStepped)>0.001) {
      cache.m_sigmaIoni = cache.m_sigmaIoni - cache.m_kazL*log(fabs(distanceStepped));// the non-linear term
    }
    // update straggling covariance 
    if (errorPropagation && m_straggling) {
      double sigTot2 = cache.m_sigmaIoni*cache.m_sigmaIoni + cache.m_sigmaRad*cache.m_sigmaRad;
      // /(beta*beta*p*p*p*p) transforms Var(E) to Var(q/p)
      mom = fabs(1./P[6]); beta = mom/std::sqrt(mom*mom+cache.m_particleMass*cache.m_particleMass);
      double bp2 = beta*mom*mom;
      cache.m_stragglingVariance += sigTot2/(bp2*bp2)*distanceStepped*distanceStepped;  
    }
    if (cache.m_matstates || errorPropagation) 
      cache.m_combinedEloss.update(cache.m_delIoni*distanceStepped,cache.m_sigmaIoni*fabs(distanceStepped),
                             cache.m_delRad *distanceStepped,cache.m_sigmaRad *fabs(distanceStepped),m_MPV);
    //Calculate new distance to target
    previousDistance = fabs( distanceToTarget);
    distanceToTarget = distance( surfaceType, targetSurface, P, distanceEstimationSuccessful);
    if (!distanceEstimationSuccessful) return false;

    //If h and distance are in opposite directions, target is passed. Flip propagation direction
    if (h * distanceToTarget < 0.) {
      h = -h;				//Flip direction
      targetPassed++;
    }
    //don't step beyond surface
    if (fabs( h) > fabs( distanceToTarget)) h = distanceToTarget;

    //Abort if maxPath is reached or solution is diverging
    if ((targetPassed > 3 && fabs( distanceToTarget) >= previousDistance) || (absolutePath > maxPath)) return false;

    if (steps++ > m_maxSteps) return false; //Too many steps, something is wrong
  }

  if (cache.m_material && cache.m_material->x0()!=0.) cache.m_combinedThickness += fabs(path)/cache.m_material->x0(); 

  //Use Taylor expansions to step the remaining distance (typically microns).
  path = path + distanceToTarget;

  //pos = pos + h*dir + 1/2*h*h*dDir. Second order Taylor expansion.
  pos[0] = pos[0] + distanceToTarget*(dir[0] + 0.5*distanceToTarget*dDir[0]);
  pos[1] = pos[1] + distanceToTarget*(dir[1] + 0.5*distanceToTarget*dDir[1]);
  pos[2] = pos[2] + distanceToTarget*(dir[2] + 0.5*distanceToTarget*dDir[2]);

  //dir = dir + h*dDir. First order Taylor expansion (Euler).
  dir[0] = dir[0] + distanceToTarget*dDir[0];
  dir[1] = dir[1] + distanceToTarget*dDir[1];
  dir[2] = dir[2] + distanceToTarget*dDir[2];

  //Normalize dir
  double norm  = 1./std::sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
  dir[0] = norm*dir[0];
  dir[1] = norm*dir[1];
  dir[2] = norm*dir[2];
  P[42]  = dDir[0];
  P[43]  = dDir[1];
  P[44]  = dDir[2];
  return true;
}


/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta main program for propagation with or without Jacobian
// with search of closest surface (ST)
/////////////////////////////////////////////////////////////////////////////////

bool
Trk::STEP_Propagator::propagateWithJacobian (Cache& cache,      
                                             bool          errorPropagation,
                                             std::vector<DestSurf >&   sfs,
                                             double*       P,
                                             Trk::PropDirection   propDir,
                                             std::vector<unsigned int>&   solutions,
                                             double&       path,
                                             double        sumPath) const
{
  double       maxPath = m_maxPath;  			// Max path allowed
  double*      pos     = &P[0];  			// Start coordinates
  double*      dir       = &P[3];  			// Start directions
  double       dDir[3]   = { 0., 0., 0.};		// Start directions derivs. Zero in case of no RK steps
  //int          targetPassed = 0;      			// how many times have we passed the target?
  double       previousDistance = 0.;
  double       distanceStepped = 0.;
  float        BG1[12] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; // Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz,
  // dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz at first point
  bool         firstStep = true;                        // Poll BG1, else recycle BG4
  int          steps = 0;
  path    = 0.;  				// path of the trajectory
  cache.m_timeStep = 0.;                              // time separation corresponding to the trajectory
  double mom   = 0.;                            // need momentum and beta for timing
  double beta  = 1.;                            // need momentum and beta for timing

  // factor to stabilize iteration for soft tracks
  double helpSoft = 1.;

  // limit number of recovery attempts
  int restartLimit =10;

  Amg::Vector3D position(P[0],P[1],P[2]); 
  Amg::Vector3D direction0(P[3],P[4],P[5]); 

  // binned material ?
  cache.m_binMat = nullptr;
  if (cache.m_trackingVolume && cache.m_trackingVolume->isAlignable()){
    const Trk::AlignableTrackingVolume* aliTV = static_cast<const Trk::AlignableTrackingVolume*> (cache.m_trackingVolume);
    cache.m_binMat = aliTV->binnedMaterial();
  }

  // closest distance estimate
  // maintain count of oscilations and previous distance for each surface;
  // skip initial trivial solutions (input parameters at surface) - should be treated before call to the propagator !
  double tol = 0.001; 
  solutions.clear();
  double distanceToTarget = propDir*maxPath;
  cache.m_currentDist.resize(sfs.size());      // keep size through the call

  int nextSf=sfs.size();
  int nextSfCand=nextSf;
  std::vector<DestSurf >::iterator sIter = sfs.begin();
  std::vector<DestSurf >::iterator sBeg  = sfs.begin();
  unsigned int numSf=0; 
  unsigned int iCurr=0;         // index for m_currentDist
  int startSf = -99;
  for (; sIter!=sfs.end(); sIter++) {
    Trk::DistanceSolution distSol = (*sIter).first->straightLineDistanceEstimate(position,direction0);
    double distEst = -propDir*maxPath;
    double dist1Est = -propDir*maxPath;
    if (distSol.numberOfSolutions()>0 ) {
      distEst = distSol.first();
      dist1Est = distSol.first();
      if ( distSol.numberOfSolutions()>1 && ( fabs(distEst) < tol || 
                                              (propDir*distEst<-tol && propDir*distSol.second()>tol)) ) 
        distEst = distSol.second();
    }
    // select input surfaces;
    // do not accept trivial solutions (at the surface)
    // but include them into further distance estimate (aca return to the same surface) 
    if ( distEst*propDir > -tol ) {
      if (distSol.currentDistance()>500.) {
        cache.m_currentDist[iCurr]=std::pair<int,std::pair<double,double> >
          (0,std::pair<double,double>(distEst,distSol.currentDistance(true)));
      }else{ 
        cache.m_currentDist[iCurr]=std::pair<int,std::pair<double,double> >
          (1,std::pair<double,double>(distEst,distSol.currentDistance(true)));
      }
      if (tol < propDir*distEst && propDir*distEst < propDir*distanceToTarget) {
        distanceToTarget = distEst;
        nextSf = iCurr;
      }
      numSf++;
    } else {
      // save the nearest distance to surface
      cache.m_currentDist[iCurr]=std::pair<int,std::pair<double,double> >
        (-1,std::pair<double,double>(distSol.currentDistance(),distSol.currentDistance(true)));
    }
    if(fabs(dist1Est)<tol) startSf = (int) iCurr;
    iCurr++;
  }   

  if (distanceToTarget == maxPath || numSf == 0 ) return false; 

  // these do not change
  std::vector< std::pair<int,std::pair<double,double> > >::iterator vsBeg   = cache.m_currentDist.begin();
  std::vector< std::pair<int,std::pair<double,double> > >::iterator vsEnd   = cache.m_currentDist.end();

  //Set initial step length to 100mm or the distance to the target surface.
  double h;
  double absPath=0.;
  distanceToTarget > 100. ? h = 100. : distanceToTarget < -100. ? h = -100. : h = distanceToTarget;

  const Trk::IdentifiedMaterial* binIDMat = nullptr;
  //Adapt step size to the material binning : change of bin layer triggers dump of material effects
  if (cache.m_binMat) {
    const Trk::BinUtility* lbu =  cache.m_binMat->layerBinUtility(position);
    if (lbu) {
      cache.m_currentLayerBin = cache.m_binMat->layerBin(position);
      binIDMat        = cache.m_binMat->material(position);
      std::pair<size_t,float> dist2next = lbu->distanceToNext(position,propDir*direction0);
      if (dist2next.first < lbu->bins() && fabs(dist2next.second)>1. && fabs(dist2next.second)< fabs(h) ){
        h = dist2next.second*propDir; 
      }
      if (binIDMat) cache.m_material = binIDMat->first;
    }
  }

  // Step to within distanceTolerance, then do the rest as a Taylor expansion.
  // Keep distanceTolerance within [1 nanometer, 10 microns].
  // This means that no final Taylor expansions beyond 10 microns and no
  // Runge-Kutta steps less than 1 nanometer are allowed.
  double distanceTolerance = std::min( std::max( fabs( distanceToTarget) * m_tolerance, 1e-6), 1e-2);

  // bremstrahlung : sample if activated
  if (cache.m_brem) {
    mom = fabs(1./P[6]); 
    sampleBrem(cache,mom);
  }

  while ( numSf > 0 && ( fabs( distanceToTarget) > distanceTolerance || 
                         fabs(path+distanceStepped)<tol) ) { // Step until within tolerance
    //Do the step. Stop the propagation if the energy goes below m_momentumCutOff
    if (!rungeKuttaStep(cache, errorPropagation, h, P, dDir, BG1, firstStep, distanceStepped)) {
      // emit brem photon before stopped ?
      if (cache.m_brem) {
        if ( m_momentumCutOff < cache.m_bremEmitThreshold && m_simMatUpdator ) {
          Amg::Vector3D position(P[0],P[1],P[2]); 
          Amg::Vector3D direction(P[3],P[4],P[5]); 
          m_simMatUpdator->recordBremPhoton(cache.m_timeIn+cache.m_timeOfFlight+cache.m_timeStep, 
                                            m_momentumCutOff, cache.m_bremMom, position, direction, cache.m_particle);
          // the recoil can be skipped here 
          for (int i=0; i<3; i++) P[3+i] = direction[i];
          // end recoil ( momentum not adjusted here ! continuous energy loss maintained for the moment)
        }
      }
      // collect material and update timing
      path = path + distanceStepped;
      // timing
      mom = fabs(1./P[6]); beta = mom/std::sqrt(mom*mom+cache.m_particleMass*cache.m_particleMass);
      cache.m_timeStep += distanceStepped/beta/CLHEP::c_light;

      if(fabs(distanceStepped)>0.001) {
        cache.m_sigmaIoni = cache.m_sigmaIoni - cache.m_kazL*log(fabs(distanceStepped));
      }
      // update straggling covariance 
      if (errorPropagation && m_straggling) {
        // 15% of the Radition moves the MOP value thus only 85% is accounted for by the Mean-MOP shift
        double sigTot2 = cache.m_sigmaIoni*cache.m_sigmaIoni + cache.m_sigmaRad*cache.m_sigmaRad;
        // /(beta*beta*p*p*p*p) transforms Var(E) to Var(q/p)
        double bp2 = beta*mom*mom;
        cache.m_stragglingVariance += sigTot2/(bp2*bp2)*distanceStepped*distanceStepped;  
      }
      if (cache.m_matstates||errorPropagation){
        cache.m_combinedEloss.update(cache.m_delIoni*distanceStepped,
                               cache.m_sigmaIoni*fabs(distanceStepped),
                               cache.m_delRad *distanceStepped,cache.m_sigmaRad *fabs(distanceStepped),m_MPV);
      }
      if (cache.m_material && cache.m_material->x0()!=0.) {
        cache.m_combinedThickness += propDir*distanceStepped/cache.m_material->x0(); 
      }

      return false;
    }
    path = path + distanceStepped;
    absPath += fabs(distanceStepped);

    // timing
    mom = fabs(1./P[6]); beta = mom/std::sqrt(mom*mom+cache.m_particleMass*cache.m_particleMass);
    cache.m_timeStep += distanceStepped/beta/Gaudi::Units::c_light;

    if(fabs(distanceStepped)>0.001) cache.m_sigmaIoni = cache.m_sigmaIoni - cache.m_kazL*log(fabs(distanceStepped));
    // update straggling covariance 
    if (errorPropagation && m_straggling) {
      // 15% of the Radition moves the MOP value thus only 85% is accounted for by the Mean-MOP shift
      double sigTot2 = cache.m_sigmaIoni*cache.m_sigmaIoni + cache.m_sigmaRad*cache.m_sigmaRad;
      // /(beta*beta*p*p*p*p) transforms Var(E) to Var(q/p)
      double bp2 = beta*mom*mom;
      cache.m_stragglingVariance += sigTot2/(bp2*bp2)*distanceStepped*distanceStepped;  
    }
    if (cache.m_matstates||errorPropagation){
      cache.m_combinedEloss.update(cache.m_delIoni*distanceStepped,cache.m_sigmaIoni*fabs(distanceStepped),
                                   cache.m_delRad *distanceStepped,cache.m_sigmaRad *fabs(distanceStepped),m_MPV);
    }
    if (cache.m_material && cache.m_material->x0()!=0.) {
      cache.m_combinedThickness += propDir*distanceStepped/cache.m_material->x0(); 
    }

    if (absPath > maxPath) return false;

    // path limit implemented
    if (cache.m_propagateWithPathLimit>0 && cache.m_pathLimit<= path) { cache.m_propagateWithPathLimit++; return true; }  

    bool restart = false; 
    // in case of problems, make shorter steps
    if ( propDir*path < -tol || absPath-fabs(path)>10.) {
      helpSoft = fabs(path)/absPath > 0.5 ? fabs(path)/absPath : 0.5;
    } 

    Amg::Vector3D position(P[0],P[1],P[2]); 
    Amg::Vector3D direction(P[3],P[4],P[5]); 

    //Adapt step size to the material binning : change of bin layer triggers dump of material effects
    float distanceToNextBin = h;          // default    
    if (cache.m_binMat) {
      const Trk::BinUtility* lbu =  cache.m_binMat->layerBinUtility(position);
      if (lbu) {
        size_t layerBin = cache.m_binMat->layerBin(position);
        const Trk::IdentifiedMaterial* iMat = cache.m_binMat->material(position); 
        std::pair<size_t,float> dist2next = lbu->distanceToNext(position,propDir*direction);
        distanceToNextBin = dist2next.second;
        if (layerBin != cache.m_currentLayerBin ) {       // step into new bin
          // check the overshoot
          std::pair<size_t,float> dist2previous = lbu->distanceToNext(position,-propDir*direction);
          float stepOver = dist2previous.second;
          //std::cout <<" STEP overshoots bin boundary by:"<< stepOver<<" :w.r.t. bin:" << dist2previous.first<< std::endl;
          double localp[5];
          Trk::RungeKuttaUtils::transformGlobalToLocal(P, localp);
          const Trk::CurvilinearParameters* cPar =  
            new Trk::CurvilinearParameters(Amg::Vector3D(P[0],P[1],P[2]),localp[2],localp[3],localp[4]); 
          if (cache.m_identifiedParameters) {
            if (binIDMat && binIDMat->second>0 && !iMat ) {  // exit from active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),-binIDMat->second));
            } else if (binIDMat && binIDMat->second>0 && (iMat->second==0 || iMat->second==binIDMat->second) ) {  // exit from active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),-binIDMat->second));
            } else if (iMat && iMat->second>0) {       // entry active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),iMat->second));
            }
          } 
          if (cache.m_hitVector) {
            double hitTiming = cache.m_timeIn+cache.m_timeOfFlight+cache.m_timeStep;
            if (binIDMat && binIDMat->second>0 && !iMat ) {  // exit from active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming,-binIDMat->second,0.));
            } else if (binIDMat && binIDMat->second>0 && (iMat->second==0 || iMat->second==binIDMat->second) ) {  // exit from active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming,-binIDMat->second,0.));
            } else if (iMat && iMat->second>0) {       // entry active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming, iMat->second,0.));
            }
          } 
          delete cPar;

          cache.m_currentLayerBin = layerBin;
          binIDMat = iMat;
          if (binIDMat) {                
            // change of material triggers update of the cache 
            // @TODO Coverity complains about a possible NULL pointer dereferencing here 
            //  because the code above does not explicitly forbid m_material to be NULL and m_material is used
            //  unchecked inside updateMaterialEffects.
            //  Can m_material be NULL at this point ?
            if (cache.m_material) {
              updateMaterialEffects(cache,mom, sin(direction.theta()), sumPath+path-stepOver);
            }
            cache.m_material = binIDMat->first;                
          }
          // recalculate distance to next bin
          if (distanceToNextBin<h) {
            Amg::Vector3D probe = position + (distanceToNextBin+h)*propDir*direction;
            std::pair<size_t,float> d2n = lbu->distanceToNext(probe,propDir*direction);
            distanceToNextBin += d2n.second+h;
          }
        } else if ( dist2next.first < lbu->bins() && fabs(distanceToNextBin) < 0.01 && h>0.01 ) {     // tolerance 10 microns ?
          double localp[5];
          Trk::RungeKuttaUtils::transformGlobalToLocal(P, localp);
          const Trk::CurvilinearParameters* cPar =  
            new Trk::CurvilinearParameters(Amg::Vector3D(P[0],P[1],P[2]),localp[2],localp[3],localp[4]); 

          const Trk::IdentifiedMaterial* nextMat = binIDMat;
          // need to know what comes next
          Amg::Vector3D probe = position + (distanceToNextBin+0.01)*propDir*direction.normalized();
          nextMat = cache.m_binMat->material(probe); 

          //if (m_identifiedParameters || m_hitVector) {
          //  std::cout <<"dump of identified parameters? step before:"<<distanceToNextBin<<": current:"<< 
          //  m_currentLayerBin<<","<<dist2next.first<< ":"<<position.perp()<<","<<position.z()<<std::endl;
          //  if (binIDMat) std::cout <<"layer identification current:"<<binIDMat->second <<std::endl;
          //  if (nextMat) std::cout <<"layer identification next:"<<nextMat->second <<std::endl;
          //}
          if (cache.m_identifiedParameters ) {
            if (binIDMat && binIDMat->second>0 && !nextMat ) {  // exit from active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),-binIDMat->second));
            } else if (binIDMat && binIDMat->second>0 && (nextMat->second==0 || nextMat->second==binIDMat->second) ) {  
              // exit from active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),-binIDMat->second));
            } else if (nextMat && nextMat->second>0) {       // entry active layer
              cache.m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int> (cPar->clone(),nextMat->second));
            }
          }
          if (cache.m_hitVector) {
            double hitTiming = cache.m_timeIn+cache.m_timeOfFlight+cache.m_timeStep;
            if (binIDMat && binIDMat->second>0 && !nextMat ) {  // exit from active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming, -binIDMat->second,0.));
            } else if (binIDMat && binIDMat->second>0 && (nextMat->second==0 || nextMat->second==binIDMat->second) ) {  // exit from active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming, -binIDMat->second,0.));
            } else if (nextMat && nextMat->second>0) {       // entry active layer
              cache.m_hitVector->push_back(Trk::HitInfo(cPar->clone(), hitTiming, nextMat->second,0.));
            }
          }
          delete cPar;

          cache.m_currentLayerBin = dist2next.first;
          if (binIDMat!=nextMat) {               // change of material triggers update of the cache 
            binIDMat = nextMat;
            if (binIDMat) {
              assert( cache.m_material);
              updateMaterialEffects(cache,mom, sin(direction.theta()), sumPath+path);
              cache.m_material = binIDMat->first;            
            }
          }
          // recalculate distance to next bin
          std::pair<size_t,float> d2n = lbu->distanceToNext(probe,propDir*direction.normalized());
          distanceToNextBin += d2n.second+0.01;
        }
        // TODO: trigger the update of material properties and recalculation of distance to the target sliding surface
      }
    }

    //Calculate new distance to targets
    bool flipDirection = false;
    numSf = 0 ;
    nextSfCand = nextSf;
    double dev = direction0.dot(direction);
    std::vector<DestSurf >::iterator sIter = sBeg;
    std::vector< std::pair<int,std::pair<double,double> > >::iterator vsIter  = vsBeg;
    int ic = 0;
    int numRestart = 0;

    if (cache.m_brem) {
      if ( mom < cache.m_bremEmitThreshold && m_simMatUpdator) {
        // ST : strictly speaking, the emission point should be shifted backwards a bit (mom-m_bremEmitThreshold)
        // this seems to be a minor point
        m_simMatUpdator->recordBremPhoton(cache.m_timeIn+cache.m_timeOfFlight+cache.m_timeStep, 
                                          mom, cache.m_bremMom, position, direction, cache.m_particle);
        cache.m_bremEmitThreshold = 0.;
      }
      if ( mom < cache.m_bremSampleThreshold ) sampleBrem(cache,cache.m_bremSampleThreshold);
    }

    for ( ; vsIter!= vsEnd; vsIter++) {
      if ( restart ) {
        numRestart++;
        if (numRestart>restartLimit) return false;

        vsIter = vsBeg; ic=0; sIter=sBeg; distanceToTarget = propDir*maxPath;
        nextSf = -1; nextSfCand = -1;
        restart = false;
        helpSoft = 1.;
      }
      if ( (*vsIter).first != -1 && ( ic==nextSf || (*vsIter).first==1 || nextSf<0 ||
                                      fabs((*vsIter).second.first) < 500. ||  fabs(path)>0.5*fabs((*vsIter).second.second) )  ) {
        previousDistance = (*vsIter).second.first;
        Trk::DistanceSolution distSol = (*sIter).first->straightLineDistanceEstimate(position,propDir*direction);
        double distanceEst=-propDir*maxPath;
        if (distSol.numberOfSolutions()>0 ) {
          distanceEst = distSol.first();
          if (distSol.numberOfSolutions()>1 &&
              fabs(distSol.first()*propDir+distanceStepped-previousDistance) >
              fabs(distSol.second()*propDir+distanceStepped-previousDistance) ){
            distanceEst = distSol.second();
          }
          // Peter Kluit: avoid jumping into other (distSol.first->second) distance solution for start surface with negative distance solution
          //              negative distanceEst will trigger flipDirection = true and will iterate to the start surface 
          //              this will lead to very similar positions for multiple propagator calls and many tiny X0 scatterers   
          if(ic==startSf&&distanceEst<0&&distSol.first()>0) distanceEst = distSol.first();
        }
        // eliminate close surface if path too small
        if (ic==nextSf && fabs(distanceEst)<tol && fabs(path)<tol) {
          (*vsIter).first=-1; vsIter=vsBeg; restart=true; distanceToTarget=maxPath; nextSf=-1;
          continue;  
        }

        //If h and distance are in opposite directions, target is passed. Flip propagation direction
        //Verify if true intersection
        // if (  h * propDir * distanceEst < 0. &&  fabs(distanceEst)>distanceTolerance ) {
        if (  (*vsIter).second.first *propDir* distanceEst < 0. &&  fabs(distanceEst)>distanceTolerance ) {
          // verify change of sign in signedDistance ( after eliminating situations where this is meaningless )
          if ( !distSol.signedDistance() || fabs(distSol.currentDistance(true))<tol || fabs((*vsIter).second.second)<tol
               || (*vsIter).second.second*distSol.currentDistance(true)<0) {   // true intersection
            if (ic==nextSf) {
              ((*vsIter).first)++;
              // eliminate surface if diverging
              if ((*vsIter).first>3) {
                helpSoft = fmax(0.05,1.-0.05*(*vsIter).first);
                if ((*vsIter).first>20) helpSoft=1./(*vsIter).first;
              }
              // take care of eliminating when number of flips even - otherwise it may end up at the start !  
              if ((*vsIter).first>50 && h*propDir>0 ) {
                // fabs(distanceEst) >= fabs(previousDistance) )  {
                (*vsIter).first = -1; vsIter = vsBeg; restart = true;
                continue; 
              }   
              if ((*vsIter).first!=-1) flipDirection = true;
            } else if ( fabs((*vsIter).second.second)>tol && fabs(distSol.currentDistance(true))>tol ) {
              // here we need to compare with distance from current closest
              if ( ic>nextSf ) {   // easy case, already calculated
                if (propDir*distanceEst<(*(vsBeg+nextSf)).second.first-tol)  {
                  if ((*vsIter).first!=-1) {
                    ((*vsIter).first)++;
                    flipDirection = true;
                    nextSf=ic;
                  }
                }
              } else if (distanceToTarget>0.) {             // set as nearest (if not iterating already), will be rewritten later
                if ((*vsIter).first!=-1) {
                  ((*vsIter).first)++;
                  flipDirection = true;
                  nextSf = ic;
                }
              }
            }
          } else if (ic==nextSf) {
            vsIter = vsBeg; restart = true;
            continue; 
          }
        }

        // save current distance to surface
        (*vsIter).second.first = propDir*distanceEst;
        (*vsIter).second.second = distSol.currentDistance(true);

        //std::cout <<"iterating over surfaces:"<<vsIter-vsBeg<<","<<(*vsIter).second.first<<","<<(*vsIter).second.second<< std::endl;
        // find closest surface: the step may have been beyond several surfaces
        // from all surfaces with 'negative' distance, consider only the one currently designed as 'closest'  
        // mw	if ((*vsIter).first!=-1 && ( distanceEst>-tol || ic==nextSf ) ) {
        if ((*vsIter).first!=-1 && ( distanceEst > 0. || ic==nextSf ) ) {
          numSf++;
          if ( distanceEst < fabs(distanceToTarget) ) {
            distanceToTarget = propDir*distanceEst;
            nextSfCand = ic;
          }
        }
      } else if ( fabs(path) > fabs((*vsIter).second.second) || dev<0.985 || nextSf<0 ) {  // keep an eye on surfaces with negative distance; tracks are curved !
        Trk::DistanceSolution distSol = (*sIter).first->straightLineDistanceEstimate(position,propDir*direction);
        double distanceEst=-propDir*maxPath;
        if (distSol.numberOfSolutions()>0 ) {
          distanceEst = distSol.first();
        }
        // save current distance to surface
        (*vsIter).second.first = propDir*distanceEst;
        (*vsIter).second.second = distSol.currentDistance(true);
        // reactivate surface
        if ( distanceEst > tol && distanceEst < maxPath ) {
          (*vsIter).first = 0;
        } else {
          (*vsIter).second.first = distSol.currentDistance()+fabs(path);
        }
        if ((*vsIter).first!=-1 && distanceEst > 0.) {
          numSf++;
          if ( distanceEst < fabs(distanceToTarget) ) {
            distanceToTarget = propDir*distanceEst;
            nextSfCand = ic;
          }
        }
      }
      // additional protection - return to the same surface
      // eliminate the surface and restart the search
      // 04/10/10 ST:infinite loop due to distanceTolerance>tol fixed; 
      if ( fabs(distanceToTarget)<=distanceTolerance && path*propDir<distanceTolerance ) {
        (*vsIter).first = -1; vsIter = vsBeg; restart = true;
        continue; 
      }
      sIter++; ic++;
    }
    // if next closest not found, propagation failed
    if (nextSf<0 && nextSfCand<0) return false;

    // flip direction
    if (flipDirection) {
      distanceToTarget = (*(vsBeg+nextSf)).second.first;
      h = -h;
    } else if (nextSfCand!=nextSf) {
      nextSf = nextSfCand;
      if (cache.m_currentDist[nextSf].first<3) helpSoft = 1.;
    }

    //don't step beyond surfaces - adjust step
    if (fabs( h) > fabs( distanceToTarget)) h = distanceToTarget;

    //don't step beyond bin boundary - adjust step
    if (cache.m_binMat && fabs( h) > fabs(distanceToNextBin)+0.001 ) {
      if ( distanceToNextBin>0 ) {     // TODO : investigate source of negative distance in BinningData  
        //std::cout <<"adjusting step because of bin boundary:"<< h<<"->"<< distanceToNextBin*propDir<< std::endl;
        h = distanceToNextBin*propDir;
      }
    }

    if ( helpSoft<1.) h*=helpSoft;

    // don't step much beyond path limit
    if (cache.m_propagateWithPathLimit>0 && h > cache.m_pathLimit ) h = cache.m_pathLimit+tol; 

    //std::cout <<"current closest estimate: distanceToTarget: step size :"<< nextSf<<":"<< distanceToTarget <<":"<<h<< std::endl;

    //Abort if maxPath is reached 
    if (fabs( path) > maxPath) return false;

    if (steps++ > m_maxSteps) return false; //Too many steps, something is wrong

  }

  if (!numSf) return false;

  //Use Taylor expansions to step the remaining distance (typically microns).
  path = path + distanceToTarget;

  // timing
  mom = fabs(1./P[6]); beta = mom/std::sqrt(mom*mom+cache.m_particleMass*cache.m_particleMass);
  cache.m_timeStep += distanceToTarget/beta/Gaudi::Units::c_light;

  //pos = pos + h*dir + 1/2*h*h*dDir. Second order Taylor expansion.
  pos[0] = pos[0] + distanceToTarget*(dir[0] + 0.5*distanceToTarget*dDir[0]);
  pos[1] = pos[1] + distanceToTarget*(dir[1] + 0.5*distanceToTarget*dDir[1]);
  pos[2] = pos[2] + distanceToTarget*(dir[2] + 0.5*distanceToTarget*dDir[2]);

  //dir = dir + h*dDir. First order Taylor expansion (Euler).
  dir[0] = dir[0] + distanceToTarget*dDir[0];
  dir[1] = dir[1] + distanceToTarget*dDir[1];
  dir[2] = dir[2] + distanceToTarget*dDir[2];

  //Normalize dir
  double norm  = 1./std::sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
  dir[0] = norm*dir[0];
  dir[1] = norm*dir[1];
  dir[2] = norm*dir[2];
  P[42]  = dDir[0];
  P[43]  = dDir[1];
  P[44]  = dDir[2];

  // collect all surfaces with distance below tolerance
  std::vector< std::pair<int,std::pair<double,double> > >::iterator vsIter  = vsBeg;

  int index = 0; 
  for ( ; vsIter!= vsEnd; vsIter++) { 
    if ( (*vsIter).first != -1 && propDir*(*vsIter).second.first>=propDir*distanceToTarget-tol  &&   
         propDir*(*vsIter).second.first < 0.01 && index!= nextSf) {
      solutions.push_back(index);
    }        
    if (index==nextSf) solutions.push_back(index);
    index++;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta trajectory model (units->mm,MeV,kGauss)
//
// Runge Kutta method is adaptive Runge-Kutta-Nystrom.
// This is the default STEP method
/////////////////////////////////////////////////////////////////////////////////

// Force this function to be optimized, even in unoptimized builds:
// this makes heavy use of Eigen, and Eigen is extremely slow if optimization
// is disabled.  If you do need to debug this function, you can comment
// this out.
#if defined(__GNUC__) && !defined(__OPTIMIZE__)
__attribute__ ((optimize(2)))
#endif
bool
Trk::STEP_Propagator::rungeKuttaStep( Cache& cache,
                                      bool    errorPropagation,
                                      double& h,
                                      double* P,
                                      double* dDir,
                                      float*  BG1,
                                      bool&   firstStep,
                                      double& distanceStepped) const
{
  double sol = 0.0299792458;			// Speed of light
  double charge;
  P[6] >= 0. ? charge = 1. : charge = -1.;      // Set charge
  double     lambda1 = P[6];
  double     lambda2 = P[6];	// Store inverse momentum for Jacobian transport
  double     lambda3 = P[6];
  double     lambda4 = P[6];
  double     dP1=0.;
  double     dP2=0.;
  double     dP3=0.;
  double     dP4=0.;    // dp/ds = -g*E/p for positive g=dE/ds
  double     dL1=0.;
  double     dL2=0.;
  double     dL3=0.;
  double     dL4=0.;    // factor used for calculating dCM/dCM, P[41], in the Jacobian.
  double     initialMomentum = fabs( 1./P[6]);  // Set initial momentum
  Amg::Vector3D initialPos( P[0], P[1], P[2]);	// Set initial values for position
  Amg::Vector3D initialDir( P[3], P[4], P[5]);	// Set initial values for direction.
  Amg::Vector3D dir1;
  Amg::Vector3D dir2;
  Amg::Vector3D dir3;
  Amg::Vector3D dir4;            // Directions at the different points. Used by the error propagation
  float      BG23[12] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; // Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz, dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz
  float      BG4[12] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};  // The gradients are used by the error propagation
  double     g = 0.;                            // Energyloss in Mev/mm.
  double     dgdl = 0.;                         // dg/dlambda in Mev/mm.
  double     particleMass = s_particleMasses.mass[cache.m_particle]; //Get particle mass from ParticleHypothesis
  int        steps = 0;

  //POINT 1. Only calculate this once per step, even if step is rejected. This point is independant of the step length, h
  double     momentum = initialMomentum;        // Current momentum
  if (m_energyLoss && cache.m_matPropOK) {
    g = dEds( cache,momentum); //Use the same energy loss throughout the step.
    double E = std::sqrt( momentum*momentum + particleMass*particleMass);
    dP1 = g*E/momentum;
    if (errorPropagation) {
      if (m_includeGgradient) {
        dgdl = dgdlambda(cache, lambda1); //Use this value throughout the step.
      }
      dL1 = -lambda1*lambda1*g*E*(3.-(momentum*momentum)/(E*E)) - lambda1*lambda1*lambda1*E*dgdl;
    }
  }

  Amg::Vector3D pos = initialPos; // Coordinate solution
  Amg::Vector3D dir = initialDir; // Direction solution
  dir1 = dir;
  if (firstStep) {	       // Poll BG1 if this is the first step, else use recycled BG4
    firstStep = false;
    // MT Field cache is stored in cache
    getMagneticField( cache, pos, errorPropagation, BG1); //Get the gradients needed for the error propagation if errorPropagation=true
  }
  // Lorentz force, d2r/ds2 = lambda * (dir x B)
  Amg::Vector3D k1( dir.y()*BG1[2] - dir.z()*BG1[1], dir.z()*BG1[0] - dir.x()*BG1[2], dir.x()*BG1[1] - dir.y()*BG1[0]);
  k1 = sol * lambda1 * k1;

  while (true) { //Repeat step until error estimate is within the requested tolerance
    if (steps++ > m_maxSteps) return false; //Abort propagation
    //POINT 2
    if (m_energyLoss && cache.m_matPropOK) {
      momentum = initialMomentum + (h/2.)*dP1;
      if (momentum <= m_momentumCutOff) return false; //Abort propagation
      double E = std::sqrt( momentum*momentum + particleMass*particleMass);
      dP2 = g*E/momentum;
      lambda2 = charge/momentum;
      if (errorPropagation) {
        dL2 = -lambda2*lambda2*g*E*(3.-(momentum*momentum)/(E*E)) - lambda2*lambda2*lambda2*E*dgdl;
      }
    }
    pos = initialPos + (h/2.)*initialDir + (h*h/8.)*k1;
    dir = initialDir + (h/2.)*k1;
    dir2 = dir;

    // MT Field cache is stored in cache
    getMagneticField( cache, pos, errorPropagation, BG23);
    Amg::Vector3D k2( dir.y()*BG23[2] - dir.z()*BG23[1], dir.z()*BG23[0] - dir.x()*BG23[2],
                      dir.x()*BG23[1] - dir.y()*BG23[0]);
    k2 = sol * lambda2 * k2;

    //POINT 3. Same position and B-field as point 2.
    if (m_energyLoss && cache.m_matPropOK) {
      momentum = initialMomentum + (h/2.)*dP2;
      if (momentum <= m_momentumCutOff) return false; //Abort propagation
      double E = std::sqrt( momentum*momentum + particleMass*particleMass);
      dP3 = g*E/momentum;
      lambda3 = charge/momentum;
      if (errorPropagation) {
        dL3 = -lambda3*lambda3*g*E*(3.-(momentum*momentum)/(E*E)) - lambda3*lambda3*lambda3*E*dgdl;
      }
    }
    dir = initialDir + (h/2.)*k2;
    dir3 = dir;
    Amg::Vector3D k3( dir.y()*BG23[2] - dir.z()*BG23[1], dir.z()*BG23[0] - dir.x()*BG23[2],
                      dir.x()*BG23[1] - dir.y()*BG23[0]);
    k3 = sol * lambda3 * k3;

    //POINT 4
    if (m_energyLoss && cache.m_matPropOK) {
      momentum = initialMomentum + h*dP3;
      if (momentum <= m_momentumCutOff) return false; //Abort propagation
      double E = std::sqrt( momentum*momentum + particleMass*particleMass);
      dP4 = g*E/momentum;
      lambda4 = charge/momentum;
      if (errorPropagation) {
        dL4 = -lambda4*lambda4*g*E*(3.-(momentum*momentum)/(E*E)) - lambda4*lambda4*lambda4*E*dgdl;
      }
    }
    pos = initialPos + h*initialDir + (h*h/2.)*k3;
    dir = initialDir + h*k3;
    dir4 = dir;

    // MT Field cache is stored in cache
    getMagneticField( cache, pos, errorPropagation, BG4);
    Amg::Vector3D k4( dir.y()*BG4[2] - dir.z()*BG4[1], dir.z()*BG4[0] - dir.x()*BG4[2], dir.x()*BG4[1] - dir.y()*BG4[0]);
    k4 = sol * lambda4 * k4;

    //Estimate local error and avoid division by zero
    Amg::Vector3D errorPos((h*h) * (k1 - k2 - k3 + k4));
    double errorEstimate = std::max( errorPos.mag(), 1e-20);

    //Use the error estimate to calculate new step length. h is returned by reference.
    distanceStepped = h; //Store old step length.
    h = h*std::min(std::max( 0.25, std::sqrt(std::sqrt(m_tolerance / errorEstimate))), 4.);

    //Repeat step with the new step size if error is too big.
    if (errorEstimate > 4.*m_tolerance) continue;

    //Step was ok. Store solutions.
    //Update positions.
    pos = initialPos + distanceStepped*initialDir + (distanceStepped*distanceStepped/6.)*(k1 + k2 + k3);
    P[0] = pos.x();
    P[1] = pos.y();
    P[2] = pos.z();

    //update directions
    dir = initialDir + (distanceStepped/6.)*(k1 + 2.*k2 + 2.*k3 + k4);
    P[3] = dir.x();
    P[4] = dir.y();
    P[5] = dir.z();

    //Normalize direction
    double norm = 1./std::sqrt( P[3]*P[3] + P[4]*P[4] + P[5]*P[5]);
    P[3] = norm*P[3];
    P[4] = norm*P[4];
    P[5] = norm*P[5];

    //Update inverse momentum if energyloss is switched on
    if (m_energyLoss &&cache.m_matPropOK) {
      momentum = initialMomentum + (distanceStepped/6.)*(dP1 + 2.*dP2 + 2.*dP3 + dP4);
      if (momentum <= m_momentumCutOff) return false; //Abort propagation
      P[6] = charge/momentum;
    }

    //dDir provides a small correction to the final tiny step in PropagateWithJacobian
    dDir[0] = k4.x();
    dDir[1] = k4.y();
    dDir[2] = k4.z();

    //Transport Jacobian using the same step length, points and magnetic fields as for the track parameters
    //BG contains Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz, dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz
    //               0   1   2   3       4       5       6       7       8       9       10      11
    if (errorPropagation) {
      double     initialjLambda = P[41]; //dLambda/dLambda
      double     jLambda = initialjLambda;
      double     jdL1=0.;
      double     jdL2=0.;
      double     jdL3=0.;
      double     jdL4=0.;

      for (int i=7; i<42; i+=7) {

        //POINT 1
        Amg::Vector3D initialjPos( P[i], P[i+1], P[i+2]);
        Amg::Vector3D jPos = initialjPos;
        Amg::Vector3D initialjDir( P[i+3], P[i+4], P[i+5]);
        Amg::Vector3D jDir = initialjDir;

        //B-field terms
        Amg::Vector3D jk1( jDir.y()*BG1[2] - jDir.z()*BG1[1], jDir.z()*BG1[0] - jDir.x()*BG1[2],
                           jDir.x()*BG1[1] - jDir.y()*BG1[0]);

        //B-field gradient terms
        if (m_includeBgradients) {
          Amg::Vector3D C((dir1.y()*BG1[9] - dir1.z()*BG1[6])*jPos.x() + (dir1.y()*BG1[10] - dir1.z()*BG1[7])*jPos.y() +
                          (dir1.y()*BG1[11] - dir1.z()*BG1[8])*jPos.z(),
                          (dir1.z()*BG1[3] - dir1.x()*BG1[9])*jPos.x() + (dir1.z()*BG1[4] - dir1.x()*BG1[10])*jPos.y() +
                          (dir1.z()*BG1[5] - dir1.x()*BG1[11])*jPos.z(),
                          (dir1.x()*BG1[6] - dir1.y()*BG1[3])*jPos.x() + (dir1.x()*BG1[7] - dir1.y()*BG1[4])*jPos.y() +
                          (dir1.x()*BG1[8] - dir1.y()*BG1[5])*jPos.z());
          jk1 = jk1 + C;
        }
        jk1 = sol * lambda1 * jk1;

        //Last column of the A-matrix
        if (i==35) {                  //Only dLambda/dLambda, in the last row of the jacobian, is different from zero
          jdL1 = dL1*jLambda;         //Energy loss term. dL1 = 0 if no material effects -> jLambda = P[41] is unchanged
          jk1 = jk1 + k1*jLambda/lambda1; //B-field terms
        }

        //POINT 2
        jPos = initialjPos + (distanceStepped/2.)*initialjDir + (distanceStepped*distanceStepped/8.)*jk1;
        jDir = initialjDir + (distanceStepped/2.)*jk1;

        Amg::Vector3D jk2( jDir.y()*BG23[2] - jDir.z()*BG23[1], jDir.z()*BG23[0] - jDir.x()*BG23[2],
                           jDir.x()*BG23[1] - jDir.y()*BG23[0]);

        if (m_includeBgradients) {
          Amg::Vector3D C((dir2.y()*BG23[9] - dir2.z()*BG23[6])*jPos.x() + (dir2.y()*BG23[10] - dir2.z()*BG23[7])*jPos.y() +
                          (dir2.y()*BG23[11] - dir2.z()*BG23[8])*jPos.z(),
                          (dir2.z()*BG23[3] - dir2.x()*BG23[9])*jPos.x() + (dir2.z()*BG23[4] - dir2.x()*BG23[10])*jPos.y() +
                          (dir2.z()*BG23[5] - dir2.x()*BG23[11])*jPos.z(),
                          (dir2.x()*BG23[6] - dir2.y()*BG23[3])*jPos.x() + (dir2.x()*BG23[7] - dir2.y()*BG23[4])*jPos.y() +
                          (dir2.x()*BG23[8] - dir2.y()*BG23[5])*jPos.z());
          jk2 = jk2 + C;
        }
        jk2 = sol * lambda2 * jk2;

        if (i==35) {
          jLambda = initialjLambda + (distanceStepped/2.)*jdL1;
          jdL2 = dL2*jLambda;
          jk2 = jk2 + k2*jLambda/lambda2;
        }

        //POINT 3
        jDir = initialjDir + (distanceStepped/2.)*jk2;

        Amg::Vector3D jk3( jDir.y()*BG23[2] - jDir.z()*BG23[1], jDir.z()*BG23[0] - jDir.x()*BG23[2],
                           jDir.x()*BG23[1] - jDir.y()*BG23[0]);

        if (m_includeBgradients) {
          Amg::Vector3D C((dir3.y()*BG23[9] - dir3.z()*BG23[6])*jPos.x() + (dir3.y()*BG23[10] - dir3.z()*BG23[7])*jPos.y() +
                          (dir3.y()*BG23[11] - dir3.z()*BG23[8])*jPos.z(),
                          (dir3.z()*BG23[3] - dir3.x()*BG23[9])*jPos.x() + (dir3.z()*BG23[4] - dir3.x()*BG23[10])*jPos.y() +
                          (dir3.z()*BG23[5] - dir3.x()*BG23[11])*jPos.z(),
                          (dir3.x()*BG23[6] - dir3.y()*BG23[3])*jPos.x() + (dir3.x()*BG23[7] - dir3.y()*BG23[4])*jPos.y() +
                          (dir3.x()*BG23[8] - dir3.y()*BG23[5])*jPos.z());
          jk3 = jk3 + C;
        }
        jk3 = sol * lambda3 * jk3;

        if (i==35) {
          jLambda = initialjLambda + (distanceStepped/2.)*jdL2;
          jdL3 = dL3*jLambda;
          jk3 = jk3 + k3*jLambda/lambda3;
        }

        //POINT 4
        jPos = initialjPos + distanceStepped*initialjDir + (distanceStepped*distanceStepped/2.)*jk3;
        jDir = initialjDir + distanceStepped*jk3;

        Amg::Vector3D jk4( jDir.y()*BG4[2] - jDir.z()*BG4[1], jDir.z()*BG4[0] - jDir.x()*BG4[2],
                           jDir.x()*BG4[1] - jDir.y()*BG4[0]);

        if (m_includeBgradients) {
          Amg::Vector3D C((dir4.y()*BG4[9] - dir4.z()*BG4[6])*jPos.x() + (dir4.y()*BG4[10] - dir4.z()*BG4[7])*jPos.y() +
                          (dir4.y()*BG4[11] - dir4.z()*BG4[8])*jPos.z(),
                          (dir4.z()*BG4[3] - dir4.x()*BG4[9])*jPos.x() + (dir4.z()*BG4[4] - dir4.x()*BG4[10])*jPos.y() +
                          (dir4.z()*BG4[5] - dir4.x()*BG4[11])*jPos.z(),
                          (dir4.x()*BG4[6] - dir4.y()*BG4[3])*jPos.x() + (dir4.x()*BG4[7] - dir4.y()*BG4[4])*jPos.y() +
                          (dir4.x()*BG4[8] - dir4.y()*BG4[5])*jPos.z());
          jk4 = jk4 + C;
        }
        jk4 = sol * lambda4 * jk4;

        if (i==35) {
          jLambda = initialjLambda + distanceStepped*jdL3;
          jdL4 = dL4*jLambda;
          jk4 = jk4 + k4*jLambda/lambda4;
        }

        //solution
        jPos = initialjPos + distanceStepped*initialjDir + (distanceStepped*distanceStepped/6.)*(jk1 + jk2 + jk3);
        jDir = initialjDir + (distanceStepped/6.)*(jk1 + 2.*jk2 + 2.*jk3 + jk4);
        if (i==35) {
          jLambda = initialjLambda + (distanceStepped/6.)*(jdL1 + 2.*jdL2 + 2.*jdL3 + jdL4);
        }

        //Update positions
        P[i]   = jPos.x();
        P[i+1] = jPos.y();
        P[i+2] = jPos.z();

        //update directions
        P[i+3] = jDir.x();
        P[i+4] = jDir.y();
        P[i+5] = jDir.z();
      }
      P[41] = jLambda; //update dLambda/dLambda
    }

    //Store BG4 for use as BG1 in the next step
    for (int i=0; i<12; i++) {
      BG1[i] = BG4[i];
    }
    return true;
  }
}


/////////////////////////////////////////////////////////////////////////////////
// Get the magnetic field and gradients
// Input: Globalposition
// Output: BG, which contains Bx, By, Bz, dBx/dx, dBx/dy, dBx/dz, dBy/dx, dBy/dy, dBy/dz, dBz/dx, dBz/dy, dBz/dz
/////////////////////////////////////////////////////////////////////////////////

void
Trk::STEP_Propagator::getMagneticField( Cache& cache,
                                        const Amg::Vector3D&  position,
                                        bool            getGradients,
                                        float*          BG) const
{
  double      pos[3] = {0.}; //Auxiliary variable, needed for fieldGradient_XYZ_in_mm.
  pos[0] = position.x();
  pos[1] = position.y();
  pos[2] = position.z();
  const double magScale = 10000.;
  double* R=&pos[0];
  double H[3];
  double dH[9];

  if (getGradients && m_includeBgradients) {   // field gradients needed and available

    // MT Field cache is stored in cache
    getFieldGradient(cache, R,H,dH);

    BG[0]=H[0]*magScale;    
    BG[1]=H[1]*magScale;    
    BG[2]=H[2]*magScale;    
    BG[3] =dH[0]*magScale;    
    BG[4] =dH[1]*magScale;    
    BG[5] =dH[2]*magScale;    
    BG[6] =dH[3]*magScale;    
    BG[7] =dH[4]*magScale;    
    BG[8] =dH[5]*magScale;    
    BG[9] =dH[6]*magScale;    
    BG[10]=dH[7]*magScale;    
    BG[11]=dH[8]*magScale;    

  }
  else {  //Homogenous field or no gradients needed, only retrieve the field strength.

    // MT Field cache is stored in cache
    getField(cache, R,H); 

    BG[0]=H[0]*magScale;
    BG[1]=H[1]*magScale;    
    BG[2]=H[2]*magScale;    

    for (int i=3; i<12; i++) { //Set gradients to zero
      BG[i] = 0.;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////
// Main program for step estimation to surfaces
/////////////////////////////////////////////////////////////////////////////////

double
Trk::STEP_Propagator::distance (Surface::SurfaceType surfaceType,
                                double*     targetSurface,
                                const double*     P,
                                bool&       distanceEstimationSuccessful) const
{
  if (surfaceType == Trk::Surface::Plane || surfaceType == Trk::Surface::Disc)
    return Trk::RungeKuttaUtils::stepEstimatorToPlane(targetSurface, P,
                                                distanceEstimationSuccessful);
  if (surfaceType == Trk::Surface::Cylinder)
    return Trk::RungeKuttaUtils::stepEstimatorToCylinder(
        targetSurface, P, distanceEstimationSuccessful);
  
  if (surfaceType == Trk::Surface::Line || surfaceType == Trk::Surface::Perigee)
    return Trk::RungeKuttaUtils::stepEstimatorToStraightLine(
        targetSurface, P, distanceEstimationSuccessful);
  
  if (surfaceType == Trk::Surface::Cone)
    return Trk::RungeKuttaUtils::stepEstimatorToCone(targetSurface, P,
                                               distanceEstimationSuccessful);

  // presumably curvilinear?
  return Trk::RungeKuttaUtils::stepEstimatorToPlane(targetSurface, P, distanceEstimationSuccessful);
}


/////////////////////////////////////////////////////////////////////////////////
// dg/dlambda for non-electrons (g=dEdX and lambda=q/p).
/////////////////////////////////////////////////////////////////////////////////

double Trk::STEP_Propagator::dgdlambda( Cache& cache,double l) const
{
  if (cache.m_particle == Trk::geantino || cache.m_particle == Trk::nonInteractingMuon) return 0.;
  if (cache.m_material->x0()==0 || cache.m_material->averageZ()==0) return 0.; 
  if (cache.m_material->zOverAtimesRho()==0) return 0.; 

  double p     = fabs( 1./l);
  double m     = s_particleMasses.mass[cache.m_particle];
  double me    = s_particleMasses.mass[Trk::electron];
  double E     = std::sqrt(p*p+m*m);
  double beta  = p/E;
  double gamma = E/m;
  double I     = 16.e-6*std::pow(cache.m_material->averageZ(),0.9);
  double kaz   = 0.5*30.7075*cache.m_material->zOverAtimesRho();

  //Bethe-Bloch
  double lnCore = 4.*me*me/(m*m*m*m*I*I*l*l*l*l)/(1.+2.*gamma*me/m+me*me/m*m);
  double lnCore_deriv = -4.*me*me/(m*m*m*m*I*I) * std::pow( l*l*l*l+2.*gamma*l*l*l*l*me/m+l*l*l*l*me*me/(m*m) ,-2) *
    (4.*l*l*l+8.*me*l*l*l*gamma/m-2.*me*l/(m*m*m*gamma)+4.*l*l*l*me*me/(m*m));
  double ln_deriv = 2.*l*m*m*log(lnCore) + lnCore_deriv/(lnCore*beta*beta);
  double Bethe_Bloch_deriv = -kaz*ln_deriv;

  //density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
  if (gamma > 10.) {
    double delta = 2.*log(28.816e-6 * std::sqrt(1000.*cache.m_material->zOverAtimesRho())/I) + 2.*log(beta*gamma) - 1.;
    double delta_deriv = -2./(l*beta*beta)+2.*delta*l*m*m;
    Bethe_Bloch_deriv += kaz*delta_deriv;
  }

  //Bethe-Heitler
  double Bethe_Heitler_deriv = me*me/(m*m*cache.m_material->x0()*l*l*l*E);

  //Radiative corrections (e+e- pair production + photonuclear) for muons at energies above 8 GeV and below 1 TeV
  double radiative_deriv = 0.;
  if ((cache.m_particle == Trk::muon) && (E > 8000.)) {
    if (E < 1.e6) {
      radiative_deriv = 6.803e-5/(cache.m_material->x0()*l*l*l*E) + 2.*2.278e-11/(cache.m_material->x0()*l*l*l) -
        3.*9.899e-18*E/(cache.m_material->x0()*l*l*l);
    } else {
      radiative_deriv = 9.253e-5/(cache.m_material->x0()*l*l*l*E);
    }
  }

  //return the total derivative
  if (m_MPV) {
    return 0.9*Bethe_Bloch_deriv + 0.15*Bethe_Heitler_deriv + 0.15*radiative_deriv; //Most probable value
  }
  else {
    return Bethe_Bloch_deriv + Bethe_Heitler_deriv + radiative_deriv; //Mean value
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Multiple scattering and straggling contribution to the covariance matrix
/////////////////////////////////////////////////////////////////////////////////

void Trk::STEP_Propagator::covarianceContribution( Cache& cache, 
                                                  const Trk::TrackParameters*  trackParameters,
                                                  double                       path,
                                                  const Trk::TrackParameters*  targetParms,
                                                  AmgSymMatrix(5)*       measurementCovariance) const
{
  // kinematics
  double finalMomentum = targetParms->momentum().mag();

  // first update to make sure all material counted
  updateMaterialEffects(cache,finalMomentum, sin(trackParameters->momentum().theta()), path );

  double Jac[21];
  Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal(*targetParms,Jac);

  //Transform multiple scattering and straggling covariance from curvilinear to local system
  AmgSymMatrix(5)* localMSCov = Trk::RungeKuttaUtils::newCovarianceMatrix(
    Jac, cache.m_combinedCovariance+cache.m_covariance );

  *measurementCovariance += *localMSCov;

  delete localMSCov; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multiple scattering and straggling contribution to the covariance matrix in curvilinear representation
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Trk::STEP_Propagator::covarianceContribution( Cache& cache,
                                                   const Trk::TrackParameters*  trackParameters,
                                                   double                 path,
                                                   double                 finalMomentum,
                                                   AmgSymMatrix(5)*       measurementCovariance) const
{
  // first update to make sure all material counted
  updateMaterialEffects( cache,finalMomentum, sin(trackParameters->momentum().theta()), path );

  //Add measurement errors and multiple scattering + straggling covariance
  *measurementCovariance += cache.m_combinedCovariance + cache.m_covariance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multiple scattering and straggling contribution to the covariance matrix in curvilinear representation
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Trk::STEP_Propagator::dumpMaterialEffects( Cache& cache,
                                                const Trk::CurvilinearParameters*  parms,
                                                double                 path ) const
{

  // kinematics
  double mom = parms->momentum().mag();

  // first update to make sure all material counted
  updateMaterialEffects(cache, mom, sin(parms->momentum().theta()), path );

  if(cache.m_extrapolationCache) {
    cache.m_extrapolationCache->updateX0(cache.m_combinedThickness);
    cache.m_extrapolationCache->updateEloss(cache.m_combinedEloss.meanIoni(),cache.m_combinedEloss.sigmaIoni(),
                                      cache.m_combinedEloss.meanRad(),cache.m_combinedEloss.sigmaRad());
  }
  // output
  if(cache.m_matstates) {
    Trk::EnergyLoss* eloss = !m_detailedEloss ? new Trk::EnergyLoss(cache.m_combinedEloss.deltaE(),
                                                                    cache.m_combinedEloss.sigmaDeltaE() ) :
      new Trk::EnergyLoss(cache.m_combinedEloss.deltaE(), cache.m_combinedEloss.sigmaDeltaE(),
                          cache.m_combinedEloss.sigmaDeltaE(),cache.m_combinedEloss.sigmaDeltaE(),
                          cache.m_combinedEloss.meanIoni(), cache.m_combinedEloss.sigmaIoni(),
                          cache.m_combinedEloss.meanRad(), cache.m_combinedEloss.sigmaRad(), path ) ;

    Trk::ScatteringAngles* sa = new Trk::ScatteringAngles(0.,0.,std::sqrt(cache.m_covariance(2,2)), 
                                                          std::sqrt(cache.m_covariance(3,3)));    

    Trk::CurvilinearParameters* cvlTP = parms->clone();
    Trk::MaterialEffectsOnTrack* mefot = new Trk::MaterialEffectsOnTrack(cache.m_combinedThickness,sa,eloss,
                                                                         cvlTP->associatedSurface());   

    cache.m_matstates->push_back(new TrackStateOnSurface(nullptr,cvlTP,nullptr,mefot));
  }

  cache.m_matdump_lastpath = path; 

  // clean-up
  cache.m_combinedCovariance += cache.m_covariance;
  cache.m_covariance.setZero();
  cache.m_combinedThickness = 0.;
  cache.m_combinedEloss.set(0.,0.,0.,0.,0.,0.);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multiple scattering and straggling contribution to the covariance matrix in global coordinates
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Trk::STEP_Propagator::updateMaterialEffects( Cache& cache, 
                                                  double mom, 
                                                  double sinTheta,
                                                  double path ) const
{
  // collects material effects in between material dumps ( m_covariance )

  // collect straggling 
  if (m_straggling) {
    cache.m_covariance(4,4) += cache.m_stragglingVariance;
    cache.m_stragglingVariance = 0.;
  }

  if (!m_multipleScattering) return;

  double totalMomentumLoss = mom - cache.m_matupd_lastmom;
  double pathSinceLastUpdate = path - cache.m_matupd_lastpath; 

  double pathAbs = fabs(pathSinceLastUpdate);

  if (pathAbs<1.e-03) return;

  double matX0 = cache.m_material->x0();

  // calculate number of layers : TODO : thickness to be adjusted 
  int msLayers = int(pathAbs/m_layXmax/matX0)+1 ;  

  double massSquared = s_particleMasses.mass[cache.m_particle]*s_particleMasses.mass[cache.m_particle];
  double layerThickness = pathAbs/msLayers;
  double radiationLengths = layerThickness/matX0;
  sinTheta = std::max( sinTheta, 1e-20); //avoid division by zero
  double remainingPath = pathAbs;
  double momentum = 0.;
  double mom2 = 0.;
  double E = 0.;
  double beta = 0.;
  double dLambdads = 0.;
  double thetaVariance = 0.;

  double average_dEds = totalMomentumLoss/pathAbs;

  double cumulatedVariance = cache.m_inputThetaVariance + cache.m_combinedCovariance(3,3) + cache.m_covariance(3,3);

  double cumulatedX0 = 0.;

  bool useCache =cache.m_extrapolationCache != nullptr;
  if(useCache)  {
    double dX0 =  fabs(cache.m_combinedThickness) - pathAbs/matX0;
    if(dX0<0) dX0 = 0.;
    if(cache.m_extrapolationCache->x0tot()>0) cumulatedX0 = cache.m_extrapolationCache->x0tot() + dX0;
  }

  // calculate multiple scattering by summing the contributions from the layers
  for (int layer=1; layer <= msLayers; layer++) {

    //calculate momentum in the middle of the layer by assuming a linear momentum loss
    momentum = cache.m_matupd_lastmom + totalMomentumLoss*(layer - 0.5)/msLayers;

    mom2 = momentum*momentum;
    E = std::sqrt( mom2 + massSquared);
    beta = momentum/E;

    double C0 = 13.6*13.6/mom2/beta/beta;
    double C1 = 2*0.038*C0;
    double C2 = 0.038*0.038*C0;

    double MS2 = radiationLengths;

    dLambdads = -cache.m_charge*average_dEds*E/(momentum*momentum*momentum);
    remainingPath -= layerThickness;

    // simple - step dependent formula
    // thetaVariance = C0*MS2 + C1*MS2*log(MS2) + C2*MS2*log(MS2)*log(MS2);

    // replaced by the step-size independent code below
    if(!useCache) {
      cumulatedX0 = cumulatedVariance/C0;
      if (cumulatedX0>0.001) {
        double lX0 = log(cumulatedX0);        
        cumulatedX0 = cumulatedX0/(1+2*0.038*lX0+0.038*0.038*lX0*lX0);
      }
    }

    double MS2s = MS2+cumulatedX0;
    thetaVariance = C0*MS2 + C1*MS2s*log(MS2s) + C2*MS2s*log(MS2s)*log(MS2s);

    if (cumulatedX0>0.001) {
      double lX0 = log(cumulatedX0);        
      thetaVariance += -C1*cumulatedX0*lX0 - C2*cumulatedX0*lX0*lX0;  
    }

    //Calculate ms covariance contributions and scale
    double varScale = m_scatteringScale*m_scatteringScale;
    double positionVariance = thetaVariance* (layerThickness*layerThickness/3. + remainingPath*layerThickness + remainingPath*remainingPath);
    cache.m_covariance(0,0) += varScale* positionVariance;
    cache.m_covariance(1,1) += varScale* positionVariance;
    cache.m_covariance.fillSymmetric(2,0,cache.m_covariance(2,0) + varScale* thetaVariance/(sinTheta)* (layerThickness/2. + remainingPath));
    cache.m_covariance(2,2) += varScale* thetaVariance/(sinTheta*sinTheta);
    cache.m_covariance.fillSymmetric(3,1,cache.m_covariance(3,1) + varScale* thetaVariance* (-layerThickness/2. - remainingPath));
    cache.m_covariance(3,3) += varScale* thetaVariance;
    cache.m_covariance(4,4) += varScale* 3.*thetaVariance*thetaVariance*layerThickness*layerThickness*dLambdads*dLambdads;
    cumulatedVariance +=  varScale* thetaVariance;
    cumulatedX0  += MS2;
  }

  cache.m_matupd_lastmom = mom;
  cache.m_matupd_lastpath = path;
}

/////////////////////////////////////////////////////////////////////////////////
// Create straight line in case of q/p = 0
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::STEP_Propagator::createStraightLine( const Trk::TrackParameters*  inputTrackParameters) const
{
  AmgVector(5) lp = inputTrackParameters->parameters();
  lp[Trk::qOverP] = 1./1e10;

  //  ATH_MSG_VERBOSE("STEP propagator detects invalid input parameters (q/p=0 ), resetting momentum to 1.e10");

  if (inputTrackParameters->type()==Trk::Curvilinear) {
    return new Trk::CurvilinearParameters(inputTrackParameters->position(), lp[2], lp[3], lp[4], 
                                          (inputTrackParameters->covariance() ? 
                                           new AmgSymMatrix(5)(*inputTrackParameters->covariance()) : nullptr ) );
  }  
  return inputTrackParameters->associatedSurface().createTrackParameters(lp[0], lp[1], lp[2], lp[3], lp[4],
                                                                         (inputTrackParameters->covariance() ? 
                                                                          new AmgSymMatrix(5)(*inputTrackParameters->covariance()) : nullptr ) );

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculate energy loss in MeV/mm.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Trk::STEP_Propagator::dEds( Cache& cache, double p) const
{
  cache.m_delIoni = 0.; cache.m_delRad = 0.; cache.m_kazL = 0.;

  if (cache.m_particle == Trk::geantino || cache.m_particle == Trk::nonInteractingMuon) return 0.;
  if (cache.m_material->x0()==0 || cache.m_material->averageZ()==0) return 0.; 

  cache.m_delIoni = cache.m_matInt.dEdl_ionization(p, cache.m_material, cache.m_particle, 
                                             cache.m_sigmaIoni, cache.m_kazL);    

  cache.m_delRad  = cache.m_matInt.dEdl_radiation(p, cache.m_material, cache.m_particle, cache.m_sigmaRad);

  double eLoss = m_MPV ? 0.9*cache.m_delIoni + 0.15*cache.m_delRad : cache.m_delIoni + cache.m_delRad;

  return eLoss;

}

// Smear momentum ( simulation mode )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Trk::STEP_Propagator::smear(Cache& cache, 
                                 double& phi, 
                                 double& theta, 
                                 const Trk::TrackParameters* parms, 
                                 double radDist) const
{
  if (cache.m_particle == Trk::geantino) return ;
  if (!parms) return;

  //Calculate polar angle
  double particleMass = s_particleMasses.mass[cache.m_particle]; //Get particle mass from ParticleHypothesis
  double momentum = parms->momentum().mag();
  double energy = std::sqrt( momentum*momentum + particleMass*particleMass);
  double beta = momentum/energy;
  double th = std::sqrt(2.)*15.*std::sqrt(radDist)/(beta*momentum) * CLHEP::RandGauss::shoot(m_randomEngine); //Moliere
  //double th = (sqrt(2.)*13.6*std::sqrt(radDist)/(beta*momentum)) * (1.+0.038*log(radDist/(beta*beta))) * m_gaussian->shoot(); //Highland

  //Calculate azimuthal angle
  double ph = 2.*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);

  Amg::Transform3D rot(Amg::AngleAxis3D(-theta, Amg::Vector3D(0.,1.,0.))*
                       Amg::AngleAxis3D(-phi, Amg::Vector3D(0.,0.,1.)));
  Amg::Vector3D dir0(0.,0.,1.);
  Amg::Vector3D rotated = rot.inverse()*
    Amg::AngleAxis3D(ph, Amg::Vector3D(0.,0.,1.))*
    Amg::AngleAxis3D(th, Amg::Vector3D(0.,1.,0.))*
    dir0;

  theta = rotated.theta();
  phi   = rotated.phi();

}

// Sample momentum of brem photon ( simulation mode )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Trk::STEP_Propagator::sampleBrem(Cache& cache, double mom) const
{
  double rndx = CLHEP::RandFlat::shoot(m_randomEngine);
  double rnde = CLHEP::RandFlat::shoot(m_randomEngine);

  // sample visible fraction of the mother momentum taken according to 1/f  
  double eps = m_momentumCutOff/mom;
  cache.m_bremMom = pow(eps,pow(rndx,exp(1.)))*mom;          // adjustment here ? 
  cache.m_bremSampleThreshold = mom - cache.m_bremMom;
  cache.m_bremEmitThreshold = mom - rnde*cache.m_bremMom;      
}

Trk::TrackParameters*
Trk::STEP_Propagator::propagateNeutral(const Trk::TrackParameters&   parm,
                                       std::vector<DestSurf>&        targetSurfaces,
                                       Trk::PropDirection            propDir,
                                       std::vector<unsigned int>&    solutions,
                                       double&                  path,
                                       bool                     usePathLimit,
                                       bool                     returnCurv) const {

  // find nearest valid intersection
  double tol = 0.001; 
  solutions.clear();
  std::vector<std::pair<unsigned int,double> > currentDist;
  currentDist.clear(); 
  std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >::iterator sIter = targetSurfaces.begin();
  std::vector<std::pair<unsigned int,double> >::iterator oIter = currentDist.begin();
  std::vector<DestSurf >::iterator sBeg  = targetSurfaces.begin();

  Amg::Vector3D position(parm.position()); 
  Amg::Vector3D direction(parm.momentum().normalized()); 

  for (; sIter!=targetSurfaces.end(); sIter++) {
    Trk::DistanceSolution distSol = (*sIter).first->straightLineDistanceEstimate(position,direction);
    if (distSol.numberOfSolutions()>0 ) {
      unsigned int numSf = sIter-sBeg;
      if (distSol.first()>tol) {
        if (!currentDist.empty()) {
          oIter= currentDist.end();
          while (oIter!=currentDist.begin() && distSol.first()<(*(oIter-1)).second ) oIter--;
          oIter = currentDist.insert(oIter,std::pair<unsigned int,double>(numSf,distSol.first()));
        } else {
          currentDist.emplace_back(numSf,distSol.first());
        }
      }
      if ( distSol.numberOfSolutions()>1 && distSol.second()>tol) {
        if (!currentDist.empty()) {
          oIter= currentDist.end();
          while (oIter!=currentDist.begin() && distSol.second()<(*(oIter-1)).second ) oIter--;
          oIter = currentDist.insert(oIter,std::pair<unsigned int,double>(numSf,distSol.second()));
        } else {
          currentDist.emplace_back(numSf,distSol.second());
        }
      }
    }
  }

  // loop over surfaces, find valid intersections
  for (oIter=currentDist.begin();oIter!=currentDist.end(); oIter++) {
    Amg::Vector3D xsct= position+propDir*direction*((*oIter).second);
    if (targetSurfaces[(*oIter).first].first->isOnSurface(xsct, (*oIter).second, 0.001, 0.001) ) {
      if ( usePathLimit && path>0. && path<((*oIter).second) ) {
        Amg::Vector3D endpoint = position+propDir*direction*path;
        return new Trk::CurvilinearParameters(endpoint,parm.momentum(),parm.charge());         
      }
      path = (*oIter).second;
      solutions.push_back((*oIter).first);
      const Trk::Surface* sf = targetSurfaces[(*oIter).first].first;

      if( returnCurv || sf->type()==Trk::Surface::Cone) return new Trk::CurvilinearParameters(xsct,parm.momentum(),parm.charge());  
      return sf->createTrackParameters(xsct,parm.momentum(),parm.charge(),nullptr);

    }   
  }

  return nullptr;
}

void Trk::STEP_Propagator::clearCache(Cache& cache) const
{
  cache.m_delIoni=0.;
  cache.m_delRad=0.;
  cache.m_sigmaIoni=0.;
  cache.m_sigmaRad=0.;
}

void Trk::STEP_Propagator::getFieldCacheObject( Cache& cache,const EventContext& ctx) const
{
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("extrapolate: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
      return;
  }
  fieldCondObj->getInitializedCache (cache.m_fieldCache);
}

