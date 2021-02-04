/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PropagationEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/PropagationEngine.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventPrimitives/PropDirection.h"
//https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkEventPrimitives/trunk/TrkEventPrimitives/PropDirection.h

// constructor
Trk::PropagationEngine::PropagationEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_propagator(""),
  m_pathLimitTolerance(0.01)
{
    declareInterface<Trk::IPropagationEngine>(this);
    // configure the Propagator
    declareProperty("Propagator"                            , m_propagator);
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
    // the path limit tolerance
    declareProperty("PathLimitTolerance"                    , m_pathLimitTolerance);
}

// destructor
Trk::PropagationEngine::~PropagationEngine()
{}


// the interface method initialize
StatusCode Trk::PropagationEngine::initialize()
{
    if (m_propagator.retrieve().isFailure()){
        EX_MSG_FATAL( "", "initialize", "", "failed to retrieve propagator '"<< m_propagator << "'. Aborting." );
        return StatusCode::FAILURE;
    } else 
        EX_MSG_DEBUG( "", "initialize", "", "successfully retrieved '" << m_propagator << "'." );

    EX_MSG_DEBUG( "", "initialize", "", "successful" );
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::PropagationEngine::finalize()
{    
    EX_MSG_DEBUG( "", "finalize", "", "successful" );
    return StatusCode::SUCCESS;
}


/** resolve the boundary situation - for charged particles */
Trk::ExtrapolationCode Trk::PropagationEngine::propagate(Trk::ExCellCharged& eCell,
                                                         const Trk::Surface& sf,
                                                         Trk::PropDirection pDir,
                                                         Trk::BoundaryCheck bcheck,
                                                         bool returnCurvilinear) const
{
    EX_MSG_DEBUG(++eCell.navigationStep, "propagate", "char", "propagation engine called with charged parameters with propagation direction " << pDir ); 

    double propLength = -1.;
    if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit)){
        // the path limit
        propLength = eCell.pathLimit > 0 ? (eCell.pathLimit-eCell.pathLength) : eCell.pathLimit;
        EX_MSG_VERBOSE(eCell.navigationStep, "propagate", "char", "available step length for this propagation " << propLength ); 
    }
    // it is the final propagation if it is the endSurface
    bool finalPropagation = (eCell.endSurface == (&sf));

    Trk::TransportJacobian* tjac = 0;
    // we need to first fill the propagation parameters in order to be able to updates & fallbacks
    //release, otherwise need to change the Trk::ExCell code
    auto pParameters = m_propagator->propagate(*eCell.leadParameters, 
                                              sf,
                                              pDir,
                                              bcheck,
                                              eCell.mFieldMode,
                                              tjac,
                                              propLength,
                                              eCell.pHypothesis,
                                              returnCurvilinear).release();
   // set the return type according to how the propagation went
   if (pParameters){
       // cache the last lead parameters, useful in case a navigation error occured
       eCell.lastLeadParameters = eCell.leadParameters;
       // assign the lead and end parameters
       eCell.leadParameters = pParameters;
       // check what to do with the path Length
       if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit) || propLength > 0){
           // add the new propagation length to the path length
           eCell.pathLength += propLength;
           // check if Limit reached
           if (eCell.pathLimitReached(m_pathLimitTolerance)){
               EX_MSG_VERBOSE(eCell.navigationStep, "propagate", "char", "path limit of " << eCell.pathLimit << " successfully reached -> stopping." ); 
               return Trk::ExtrapolationCode::SuccessPathLimit;
           }
       }

       // check if the propagation was called with directly, then lead parameters become end parameters
       if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::Direct)) 
	 eCell.endParameters = eCell.leadParameters;
	 
       // return Success only if it is the final propagation - the extrapolation engine knows that 
       return (finalPropagation ? Trk::ExtrapolationCode::SuccessDestination : Trk::ExtrapolationCode::InProgress);
   }                                                                      
   // return - recovered means that the leadParameters are the input ones 
   return (finalPropagation ? Trk::ExtrapolationCode::FailureDestination : Trk::ExtrapolationCode::Recovered) ;
}

/** resolve the boundary situation - for neutral particles */
Trk::ExtrapolationCode Trk::PropagationEngine::propagate(Trk::ExCellNeutral& eCell,
                                                         const Trk::Surface& sf,
                                                         Trk::PropDirection pDir,
                                                         Trk::BoundaryCheck bcheck,
                                                         bool returnCurvilinear) const
{ 
    EX_MSG_DEBUG(++eCell.navigationStep, "propagate", "neut", "propagation engine called with neutral parameters with propagation direction " << pDir );     
    // leave this for the moment, can re replaced by an appropriate propagator call later
    if (eCell.leadParameters->covariance()){
        EX_MSG_VERBOSE(eCell.navigationStep,"propagate", "neut", "propagation of neutral parameters with covariances requested. This is not supported for the moment."); 
    }
    // the pathLimit cache so far
    double cPath = eCell.pathLength;
    // it is the final propagation if it is the endSurface
    bool finalPropagation = (eCell.endSurface == (&sf));
    // intersect the surface
    Trk::Intersection sfIntersection = (pDir!=Trk::anyDirection) ? sf.straightLineIntersection(eCell.leadParameters->position(),
                                                                          pDir*eCell.leadParameters->momentum().unit(),
                                                                          true, bcheck) :
                                              sf.straightLineIntersection(eCell.leadParameters->position(),
                                              eCell.leadParameters->momentum().unit(),
                                              false, bcheck); 
    // we have a valid intersection                                 
    if (sfIntersection.valid){
        // fill the transport information - only if the propation direction is not 0 ('anyDirection')
        if (pDir!=Trk::anyDirection){
           double pLength = (sfIntersection.position-eCell.leadParameters->position()).mag(); 
           EX_MSG_VERBOSE(eCell.navigationStep,"propagate", "neut", "path length of " << pLength << " added to the extrapolation cell (limit = " << eCell.pathLimit << ")" );    
           eCell.stepTransport(sf,pLength);
        }
        // now check if it is valud it's further away than the pathLimit
        if (eCell.pathLimitReached(m_pathLimitTolerance)){
						// cache the last lead parameters
						eCell.lastLeadParameters = eCell.leadParameters;
						// create new neutral curvilinear parameters at the path limit reached 
            double pDiff = eCell.pathLimit - cPath;
            eCell.leadParameters = new Trk::NeutralCurvilinearParameters(eCell.leadParameters->position()+pDiff*eCell.leadParameters->momentum().unit(), 
                                                                         eCell.leadParameters->momentum(),
                                                                         0.);
            EX_MSG_VERBOSE(eCell.navigationStep,"propagate", "neut", "path limit of " << eCell.pathLimit << " reached. Stopping extrapolation."); 
            return Trk::ExtrapolationCode::SuccessPathLimit;
        }  
				// cache the last lead parameters
				eCell.lastLeadParameters = eCell.leadParameters;
        // now exchange the lead parameters 
        // create the new curvilinear paramters at the surface intersection -> if so, trigger the success
        eCell.leadParameters = returnCurvilinear ? new Trk::NeutralCurvilinearParameters(sfIntersection.position, eCell.leadParameters->momentum(), 0.) :
                                                       sf.createNeutralParameters(sfIntersection.position, eCell.leadParameters->momentum(), 0.);
                                                       
        // check if the propagation was called with directly, then lead parameters become end parameters
        if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::Direct)) 
	          eCell.endParameters = eCell.leadParameters;
	
	      // return success for the final destination or in progress                                                                   
        return (finalPropagation ? Trk::ExtrapolationCode::SuccessDestination : Trk::ExtrapolationCode::InProgress);

    } else {
        // give some screen output
        EX_MSG_VERBOSE(eCell.navigationStep,"propagate", "neut", "intersection with the surface did not succeed.");
    }                                                                     
   // return - recovered means that the leadParameters are the input ones 
   return (finalPropagation ? Trk::ExtrapolationCode::FailureDestination : Trk::ExtrapolationCode::Recovered) ;
}
