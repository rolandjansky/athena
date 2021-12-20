/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/StepEngine.h"

// constructor
Trk::StepEngine::StepEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_propagator(""),          
  m_materialEffectsEngine(""),
  m_navigationEngine(""),    
  m_tolerance(0.01),
  m_debugAndFix(true),
  m_debugCall(false)  
{
    declareInterface<Trk::IExtrapolationEngine>(this);
    // The Tools needed
    declareProperty("Propagator"	                    , m_propagator);
    declareProperty("MaterialEffectsEngine"                 , m_materialEffectsEngine);
    declareProperty("NavigationEngine"                      , m_navigationEngine);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
    declareProperty("DebugAndFixMode"                       , m_debugAndFix);
}

// destructor
Trk::StepEngine::~StepEngine()
{}


// the interface method initialize
StatusCode Trk::StepEngine::initialize()
{
    if (m_propagator.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve propagator '"<< m_propagator << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_propagator << "'." );
            
    if (m_materialEffectsEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve material effect engine '"<< m_materialEffectsEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_materialEffectsEngine << "'." );
    
    if (m_navigationEngine.retrieve().isFailure()){
        EX_MSG_FATAL("", "initialize", "", "failed to retrieve navigation engine '"<< m_navigationEngine << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG("", "initialize", "", "successfully retrieved '" << m_navigationEngine << "'." );
    
    EX_MSG_DEBUG("", "initialize", "", "successful." );

    m_targetSurfaces.setDebugModeOff();

    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::StepEngine::finalize()
{    
    EX_MSG_DEBUG("", "finalize", "", "successful." );
    return StatusCode::SUCCESS;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::StepEngine::extrapolate(ExCellCharged& ecCharged,
                                           const Surface* sf,
                                           BoundaryCheck bcheck) const
{
  // check the input
  if (!ecCharged.leadParameters) return Trk::ExtrapolationCode::FailureConfiguration;	
  if (!ecCharged.leadVolume) {  // should not happen : try to recover anyway
    Trk::ExtrapolationCode eVol = m_navigationEngine->resolvePosition(ecCharged,ecCharged.propDirection,false);  
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol; 
  }  

  // cache the last lead parameters, useful in case a navigation error occured
  ecCharged.lastLeadParameters = ecCharged.leadParameters;
  // new extrapolation call : find possible target surfaces
  //Trk::TargetSurfaceVector ts;
  //Trk::ExtrapolationCode eCode = targetSurfacesT(ecCharged,ts,false,sf,bcheck);

  // test new class
  Trk::ExtrapolationCode exC = m_targetSurfaces.setOnInput(ecCharged,sf,bcheck);

  // catch an infinite loop in frame navigation
  if (exC==Trk::ExtrapolationCode::FailureLoop) {  // should not happen : try to recover once
     
    EX_MSG_WARNING("", "extrapolate", "", "frame navigation recovery attempt for:"<< ecCharged.leadVolume->volumeName());

    Trk::ExtrapolationCode eVol = m_navigationEngine->resolvePosition(ecCharged,ecCharged.propDirection,true);  
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol; 
    //ts.clear();
    //eVol = targetSurfacesT(ecCharged,ts,false,sf,bcheck);
    exC = m_targetSurfaces.setOnInput(ecCharged,sf,bcheck);
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol;   
  }  

  // build PathLimit from eCell info
  Trk::PathLimit pathLim(ecCharged.materialLimitX0-ecCharged.materialX0, ecCharged.materialProcess);  
  // build TimeLimit from eCell info
  //Trk::TimeLimit timeLim(ecCharged.timeLimit,ecCharged.time,201);      // Fatras decay code
  Trk::TimeLimit timeLim(-1.,ecCharged.time,201);      // temporary till time limit absent in eCell

  // loop over intersections
  // std::vector<unsigned int> solutions;
  Trk::TargetSurfaceVector solutions;
  while (ecCharged.leadParameters ) {
    solutions.clear();
    // TODO pass TargetSurfaces directly to the propagator

    EX_MSG_VERBOSE(ecCharged.navigationStep, "propagate", "loop", "starting propagation at position : " << ecCharged.leadParameters->position()<<","<<ecCharged.leadParameters->momentum() );
    if (m_debugCall) EX_MSG_DEBUG(ecCharged.navigationStep, "propagate", "debug loop", "starting propagation at position : " << ecCharged.leadParameters->position()<<","<<ecCharged.leadParameters->momentum() );
    //const Trk::TrackParameters* nextPar = m_propagator->propagateT(*ecCharged.leadParameters,m_targetSurfaces,ecCharged.propDirection,
    //								 ecCharged.mFieldMode, ecCharged.pHypothesis, solutions,
    //								 pathLim,timeLim,ecCharged.navigationCurvilinear,hitVector);  
    // cache the last lead parameters, useful in case a navigation error occured
    //ecCharged.lastLeadParameters = ecCharged.leadParameters;

    Trk::ExtrapolationCode eCode = m_propagator->propagate(ecCharged,m_targetSurfaces,solutions);

    // enforced debugging
    if (eCode==Trk::ExtrapolationCode::FailureNavigation && m_debugAndFix) {
      if (m_debugCall) {
	EX_MSG_INFO(ecCharged.navigationStep, "extrapolate", "loop:debug mode:"," stopping execution for further debugging");
	exit(0);
      } else {
	// repeat the call with printout && stop the execution 
	EX_MSG_INFO(ecCharged.navigationStep, "extrapolate", "loop:debug mode:","rerun last extrapolation call"<<ecCharged.lastLeadParameters->position());
	ecCharged.leadParameters = ecCharged.lastLeadParameters;
        ecCharged.leadVolume = nullptr;
        m_debugCall = true;
        m_targetSurfaces.setDebugModeOn();
	eCode = extrapolate(ecCharged,sf,bcheck);
	exit(0);
      }
    }

    const Trk::TrackParameters* nextPar = (eCode.isSuccess() || eCode.inProgress()) ? ecCharged.leadParameters : nullptr ;    
    if (!nextPar)    EX_MSG_VERBOSE(ecCharged.navigationStep, "propagate", "loop", "propagation failed ");
    else   EX_MSG_VERBOSE(ecCharged.navigationStep, "propagate", "loop", "propagated to :"<< nextPar->position());

    if (!nextPar) return ( sf ? Trk::ExtrapolationCode::FailureDestination : eCode ) ;

    EX_MSG_VERBOSE(ecCharged.navigationStep, "propagate", "loop", "propagation arrived at position : " << ecCharged.leadParameters->position() );
    if (m_debugCall) EX_MSG_DEBUG(ecCharged.navigationStep, "propagate", "loop", "propagation arrived at position : " << ecCharged.leadParameters->position() );

    // record the parameters as sensitive or passive depending on the surface
    //Trk::ExtrapolationMode::eMode emode =  csf.object->isActive() ? Trk::ExtrapolationMode::CollectSensitive : Trk::ExtrapolationMode::CollectPassive;
    // fill the corresponding parameters, the material effects updator can attach material to them
    // ecCharged.stepParameters(ecCharged.leadParameters, Trk::ExtrapolationMode::CollectPassive);

    // handle extrapolation step
    Trk::ExtrapolationCode eStep = handleIntersection(ecCharged,solutions);

    if (eStep != Trk::ExtrapolationCode::InProgress ) return eStep;
  }

  return Trk::ExtrapolationCode::FailureConfiguration;
 }


/** neutral extrapolation */
Trk::ExtrapolationCode Trk::StepEngine::extrapolate(ExCellNeutral& ecNeutral,
                                           const Surface* sf,
                                           BoundaryCheck bcheck) const
{
  // check the input

  if (!ecNeutral.leadParameters) return Trk::ExtrapolationCode::FailureConfiguration;	
  if (!ecNeutral.leadVolume) {  // should not happen : try to recover anyway
    Trk::ExtrapolationCode eVol = m_navigationEngine->resolvePosition(ecNeutral,ecNeutral.propDirection,false);  
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol; 
  }  

  // new extrapolation call : find valid intersections
  Trk::TargetSurfaceVector ts; ts.clear();
  Trk::ExtrapolationCode eCode = targetSurfacesT(ecNeutral,ts,true,sf,bcheck);

  CHECK_ECODE_SUCCESS(ecNeutral, eCode);

  // catch an infinite loop in frame navigation
  if (eCode==Trk::ExtrapolationCode::FailureLoop) {  // should not happen : try to recover once
    Trk::ExtrapolationCode eVol = m_navigationEngine->resolvePosition(ecNeutral,ecNeutral.propDirection,true);  
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol; 
    ts.clear();
    eVol = targetSurfacesT(ecNeutral,ts,true,sf,bcheck);
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol;   
  }  

  // catch an infinite loop in frame navigation
  if (!ecNeutral.leadVolume) {  // should not happen : try to recover once
    Trk::ExtrapolationCode eVol = m_navigationEngine->resolvePosition(ecNeutral,ecNeutral.propDirection);  
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol; 

    eVol = targetSurfacesT(ecNeutral,ts,true,sf,bcheck);
    if (!eVol.isSuccessOrRecovered() && !eVol.inProgress()) return eVol;   
 
  }  

  // loop over intersections
  for (unsigned int ii=0; ii<ts.size(); ii++) {

  
    ////// STEP ACROSS FRAME BOUNDARY  ///////////////  TO DO:TEMPLATE //////////////////////////
    if ( ts[ii].sfType == Trk::SurfNavigType::BoundaryFrame ) {  
      const std::vector< Trk::SharedObject< const Trk::BoundarySurface< Trk::TrackingVolume> > > &bounds = ecNeutral.leadVolume->boundarySurfaces();
      const Trk::TrackingVolume* nextVolume = bounds[ts[ii].index].get()->attachedVolume(ts[ii].intersection, 
											    ecNeutral.leadParameters->momentum(), 
											    ecNeutral.propDirection);  

      if (!nextVolume) return Trk::ExtrapolationCode::SuccessBoundaryReached;

      ecNeutral.leadParameters = new Trk::NeutralCurvilinearParameters(ts[ii].intersection,ecNeutral.leadParameters->momentum(),0.); 

      // - geometrySignature change and configuration to stop then triggers a Success 
      bool stopAtThisBoundary = ecNeutral.checkConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary) 
                                  && (nextVolume->geometrySignature() != ecNeutral.leadVolume->geometrySignature());        
      // fill the boundary into the cache if successfully hit boundary surface
      // - only cache if those are not the final parameters caused by a StopAtBoundary
      if (!stopAtThisBoundary)
	ecNeutral.stepParameters(ecNeutral.leadParameters, Trk::ExtrapolationMode::CollectBoundary);
      // loop protection - relaxed for the cases where you start from the boundary
      if (ecNeutral.leadVolume == nextVolume ) {
	// the start parameters where on the boundary already give a relaxed return code
        //  if (&bSurface == eCell.lastBoundarySurface) return Trk::ExtrapolationCode::Unset;
        // give some screen output as of why this happens
        EX_MSG_VERBOSE(ecNeutral.navigationStep, "navigation", "", "loop detected while trying to leave TrackingVolume '" << nextVolume->volumeName() << ".");
        // return a loop failure, parameter deletion will be done by cache
        return Trk::ExtrapolationCode::FailureLoop;
      }
      // break if configured to break at volume boundary and signature change
      if (stopAtThisBoundary){
	EX_MSG_VERBOSE(ecNeutral.navigationStep, "navigation", "", "geometry signature change from " <<
		       ecNeutral.leadVolume->geometrySignature()  << " to " << nextVolume->geometrySignature());
	ecNeutral.nextGeometrySignature = nextVolume->geometrySignature();
	// return the boundary reached     
	return Trk::ExtrapolationCode::SuccessBoundaryReached;
      } 
      // remember the last boundary surface for loop protection
      // ecNeutral.lastBoundarySurface             = &bounds[index];
      ecNeutral.lastBoundaryParameters          = ecNeutral.leadParameters;
      // set next volume 
      ecNeutral.leadVolume                      = nextVolume;    
      return  Trk::ExtrapolationCode::InProgress;
    } 
  }

  return Trk::ExtrapolationCode::FailureConfiguration;
 }

  
void Trk::StepEngine::evaluateDistance(Trk::TargetSurface& tt, const Amg::Vector3D& pos, const Amg::Vector3D& mom,
                                       Trk::TargetSurfaceVector&ts, bool trueOrdered) const
{
    Trk::DistanceSolution distSol = tt.surf->straightLineDistanceEstimate(pos,mom);

    double dist = distSol.first();
    Amg::Vector3D posi =  pos + dist*mom;
    // skip trivial solutions
    if (distSol.numberOfSolutions()>1 && dist<m_tolerance && distSol.second()>m_tolerance) {
      dist = distSol.second();
      posi =  pos + dist*mom;
      if (trueOrdered && !tt.surf->isOnSurface(posi,tt.bcheck,m_tolerance,m_tolerance) ) return;
      double dAbs = distSol.currentDistance(true);
      tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
      tt.setPosition(posi);
      ts.push_back(tt);
      return;
    } 
    // save closest solution
    if (!trueOrdered || dist>m_tolerance ) {
      double dAbs = distSol.currentDistance(true);
      tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
      tt.setPosition(posi);
      ts.push_back(tt);
    } 

    // save multiple intersection for neutral transport
    if (distSol.numberOfSolutions()>1 && distSol.second()>m_tolerance && trueOrdered) {
       dist = distSol.second();
       posi =  pos + dist*mom;
       if ( tt.surf->isOnSurface(posi,tt.bcheck,m_tolerance,m_tolerance) ) {
        double dAbs = distSol.currentDistance(true);
        tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
        tt.setPosition(posi);
        ts.push_back(tt);
       }
    }

    }  

// handle extrapolation step
Trk::ExtrapolationCode Trk::StepEngine::handleIntersection(ExCellCharged& ecCharged, 
							   Trk::TargetSurfaceVector& solutions) const
{
  std::vector<Trk::TargetSurface>::iterator is=solutions.begin();
  while ( is!=solutions.end() ) {

    // destination
    if ( (*is).sfType == Trk::SurfNavigType::Target )  return Trk::ExtrapolationCode::SuccessDestination;   
                           
    // frame boundary : update of target surfaces
    if ( (*is).sfType == Trk::SurfNavigType::BoundaryFrame ) {
      return resolveFrameBoundaryT(ecCharged,ecCharged.leadParameters->position(),(*is).index);  
    }

    is++;
  }

  return Trk::ExtrapolationCode::InProgress;   
}
