/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultiComponentStateCombiner.cxx  -  description
      -----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Implementation code for MultiComponentStateCombiner class 
*********************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"


Trk::MultiComponentStateCombiner::MultiComponentStateCombiner (const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_modeCalculator("Trk::MultiComponentStateModeCalculator/MultiComponentStateModeCalculator"),
  m_useMode(false),
  m_useModeD0(true),
  m_useModeZ0(true),
  m_useModePhi(true),
  m_useModeTheta(true),
  m_useModeqOverP(true),
  m_fractionPDFused(1.0)
{

  declareInterface<IMultiComponentStateCombiner>(this);
  declareProperty("UseMode",            m_useMode );
  declareProperty("ModeCalculator",     m_modeCalculator);
  declareProperty("UseModeqOverP",      m_useModeqOverP );
  declareProperty("UseModeD0",          m_useModeD0 );
  declareProperty("UseModeZ0",          m_useModeZ0 );
  declareProperty("UseModePhi",         m_useModePhi );
  declareProperty("UseModeTheta",       m_useModeTheta );
  declareProperty("FractionPDFused" ,   m_fractionPDFused);
  
}

StatusCode Trk::MultiComponentStateCombiner::initialize()
{

   // Request the mode calculator
  if ( m_modeCalculator.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Unable to retrieve the mode calculator... Exiting!" );
    return StatusCode::FAILURE;
  }
  
  m_NumberOfCalls = 0;
  
  if (m_fractionPDFused < 0.1 ){
    ATH_MSG_INFO("Fraction of PDF is set too low begin reset to 1"); 
    m_fractionPDFused = 1;   
  }

  if (m_fractionPDFused > 1 ){
    ATH_MSG_INFO("Fraction of PDF is set high low begin reset to 1"); 
    m_fractionPDFused = 1;   
  }
  
  if (msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE( "Initialisation of " << name() << " was successful" );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MultiComponentStateCombiner::finalize()
{

  ATH_MSG_INFO("-----------------------------------------------");
  ATH_MSG_INFO("         GSF MCS Combiner  Statistics          ");
  ATH_MSG_INFO("-----------------------------------------------");
  ATH_MSG_INFO("Number of Calls    " << m_NumberOfCalls          );
  ATH_MSG_INFO("Finalisation of " << name() << " was successful" );
  
  return StatusCode::SUCCESS;

}

const Trk::TrackParameters* Trk::MultiComponentStateCombiner::combine( const Trk::MultiComponentState& uncombinedState, bool useModeTemp ) const
{  
  const Trk::ComponentParameters* combinedComponent = compute(&uncombinedState, useModeTemp);
  const Trk::TrackParameters* trackParameters = combinedComponent->first;
  delete combinedComponent;
  return trackParameters;
}

const Trk::ComponentParameters* Trk::MultiComponentStateCombiner::combineWithWeight( const Trk::MultiComponentState& uncombinedState, bool useModeTemp ) const
{

  const Trk::ComponentParameters* combinedComponent = compute(&uncombinedState, useModeTemp);

  return combinedComponent;

}

const Trk::ComponentParameters* Trk::MultiComponentStateCombiner::compute( const Trk::MultiComponentState* uncombinedState, bool useModeTemp  ) const
{
  ++m_NumberOfCalls;
  if ( uncombinedState->empty() ){
    ATH_MSG_WARNING( "Trying to collapse state with zero components" );
    return 0;
  }

  const Trk::TrackParameters* firstParameters = uncombinedState->front().first;

  // Check to see if first track parameters are measured or not
  const AmgSymMatrix(5)* firstMeasuredCov = firstParameters->covariance();
  
  if ( uncombinedState->size() == 1 )
    return new Trk::ComponentParameters(uncombinedState->front().first->clone(), uncombinedState->front().second);
  
  double sumW(0.);
  const int dimension = (uncombinedState->front()).first->parameters().rows();
  if (dimension!=5){
    ATH_MSG_FATAL("More than 5 track parameters");
  }
  // generalized in 'dimension' causing headaches - temperarily removed
  //Amg::VectorX        mean(dimension);
  //Amg::MatrixX* covariance = new Amg::MatrixX(dimension,dimension);
  //Amg::MatrixX covariancePart1(dimension,dimension); covariancePart1.setZero();
  //Amg::MatrixX covariancePart2(dimension,dimension); covariancePart2.setZero();

  AmgVector(5) mean; mean.setZero();
  AmgSymMatrix(5)* covariance = new AmgSymMatrix(5);
  AmgSymMatrix(5) covariancePart1; covariancePart1.setZero();
  AmgSymMatrix(5) covariancePart2; covariancePart2.setZero();


  /* =============================
     Loop over all components
     ============================= */

  Trk::MultiComponentState::const_iterator component = uncombinedState->begin();
  double totalWeight(0.);
  for ( ; component != uncombinedState->end()   ; ++component){ 
    double weight                          = (*component).second;
    totalWeight += weight;
  }


  component = uncombinedState->begin();

  for ( ; component != uncombinedState->end()   ; ++component){ 

    const TrackParameters* trackParameters = (*component).first;
    double weight                          = (*component).second;
    
    AmgVector(5) parameters = trackParameters->parameters();

   
   
    //Ensure that we don't have any problems with the cyclical nature of phi
    //Use first state as reference poin
    double deltaPhi = (*uncombinedState->begin()).first->parameters()[2] - parameters[2];
   
    if( deltaPhi > M_PI ){
      parameters[2] += 2 * M_PI;
    } else if ( deltaPhi < -M_PI ){
      parameters[2] -= 2 * M_PI;
    }


    sumW += weight;
    mean += weight * parameters;
    
    // Extract local error matrix: Must make sure track parameters are measured, ie have an associated error matrix.
    const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();
    
    if (measuredCov){
      // Changed from errorMatrixInMeasurementFrame
      
      covariancePart1 += weight * (*measuredCov);
  
      /* ============================================================================
         Loop over all remaining components to find the second part of the covariance
         ============================================================================ */

      Trk::MultiComponentState::const_iterator remainingComponentIterator = component;

      for ( ; remainingComponentIterator != uncombinedState->end(); ++remainingComponentIterator) {
  
        if ( remainingComponentIterator == component ) continue;
        
        AmgVector(5) parameterDifference = parameters - ((*remainingComponentIterator).first)->parameters();
      
        double remainingComponentIteratorWeight = (*remainingComponentIterator).second;
        
        AmgSymMatrix(5) unity;
        for(int i(0); i<5;++i){
          for(int j(0); j<5;++j){
            unity(i,j) = parameterDifference(i) * parameterDifference(j) ; 
          }
        }
        covariancePart2 += weight * remainingComponentIteratorWeight * unity ;

      } // end loop over remaining components

    } // end clause if errors are involved
    if( weight/totalWeight > m_fractionPDFused) break; 
  } // end loop over all components
 
  mean /= sumW;
  
  //Ensure that phi is between -pi and pi 
  //
  if(  mean[2] > M_PI ){
    mean[2] -= 2 * M_PI;
  } else if (  mean[2] < -M_PI ){
    mean[2] += 2 * M_PI;
  } 


  (*covariance) = covariancePart1 / sumW + covariancePart2 / (sumW * sumW);
  
  if ( m_useMode || useModeTemp ){

    if ( dimension == 5 ){
    
      // Calculate the mode of the q/p distribution
      Amg::VectorX modes(10); modes.setZero();
      modes = m_modeCalculator->calculateMode( *uncombinedState );

      if (  msgLvl(MSG::VERBOSE) && modes[4] )
        ATH_MSG_VERBOSE( "Calculated mode q/p is: " << modes[4] );
  
      //  Replace mean with mode if qOverP mode is not 0
      if (modes[4] != 0){
        if (m_useModeD0){
          mean[0]=modes[0];
          if(modes[5+0] > 0){
            double currentErr = sqrt( (*covariance)(0,0) ) ;
            currentErr = modes[5+0] / currentErr;
            (*covariance)(0,0) = modes[5+0] * modes[5+0] ;
            covariance->fillSymmetric(1,0, (*covariance)(1,0) * currentErr);
            covariance->fillSymmetric(2,0, (*covariance)(2,0) * currentErr);
            covariance->fillSymmetric(3,0, (*covariance)(3,0) * currentErr);
            covariance->fillSymmetric(4,0, (*covariance)(4,0) * currentErr);
          }
        }
        if (m_useModeZ0) {
          mean[1]=modes[1];
          if(modes[5+1] > 0){
            double currentErr = sqrt( (*covariance)(1,1) ) ;
            currentErr = modes[5+1] / currentErr;
            covariance->fillSymmetric(1,0, (*covariance)(1,0) * currentErr);
            (*covariance)(1,1) = modes[5+1] * modes[5+1] ;
            covariance->fillSymmetric(2,1, (*covariance)(2,1) * currentErr);
            covariance->fillSymmetric(3,1, (*covariance)(3,1) * currentErr);
            covariance->fillSymmetric(4,1, (*covariance)(4,1) * currentErr);
          }
        }
        if (m_useModePhi){
          mean[2]=modes[2];
          if(modes[5+2] > 0){
            double currentErr = sqrt( (*covariance)(2,2) ) ;
            currentErr = modes[5+2] / currentErr;
            covariance->fillSymmetric(2,0, (*covariance)(2,0) * currentErr);
            covariance->fillSymmetric(2,1, (*covariance)(2,1) * currentErr);
            (*covariance)(2,2) = modes[5+2] * modes[5+2] ;
            covariance->fillSymmetric(3,2, (*covariance)(3,2) * currentErr);
            covariance->fillSymmetric(4,2, (*covariance)(4,2) * currentErr);
          }
        }
        if (m_useModeTheta){
          mean[3]=modes[3];
          if(modes[5+3] > 0){
            double currentErr = sqrt( (*covariance)(3,3) ) ;
            currentErr = modes[5+3] / currentErr;
            covariance->fillSymmetric(3,0, (*covariance)(3,0) * currentErr);
            covariance->fillSymmetric(3,1, (*covariance)(3,1) * currentErr);
            covariance->fillSymmetric(3,2, (*covariance)(3,2) * currentErr);
            (*covariance)(3,3) = modes[5+3] * modes[5+3] ;
            covariance->fillSymmetric(4,3, (*covariance)(4,3) * currentErr);
          }
        }
        if (m_useModeqOverP){
          mean[4]=modes[4];
          if(modes[5+4] > 0){
            double currentErr = sqrt( (*covariance)(4,4) ) ;
            currentErr = modes[5+4] / currentErr;
            covariance->fillSymmetric(4,0, (*covariance)(4,0) * currentErr);
            covariance->fillSymmetric(4,1, (*covariance)(4,1) * currentErr);
            covariance->fillSymmetric(4,2, (*covariance)(4,2) * currentErr);
            covariance->fillSymmetric(4,3, (*covariance)(4,3) * currentErr);
            (*covariance)(4,4) = modes[5+4] * modes[5+4] ;
          }
        }
      }
    } else {

      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG( " Dimension != 5 not updating q/p to mode q/p");

    }

  }

  const Trk::TrackParameters* combinedTrackParameters = 0;

  double loc1 = mean[Trk::loc1];
  double loc2 = mean[Trk::loc2];
  double phi = mean[Trk::phi];
  double theta = mean[Trk::theta];
  double qoverp = mean[Trk::qOverP];

  if ( firstMeasuredCov )
    //combinedTrackParameters = firstParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP], covariance );
    combinedTrackParameters = firstParameters->associatedSurface().createTrackParameters(loc1,loc2,phi,theta,qoverp,covariance);
  else{
    //combinedTrackParameters = firstParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP], 0 );
    combinedTrackParameters = firstParameters->associatedSurface().createTrackParameters(loc1,loc2,phi,theta,qoverp,0);
    delete covariance;
  }

  Trk::ComponentParameters* combinedComponentParameters = new Trk::ComponentParameters(combinedTrackParameters, totalWeight);

  return combinedComponentParameters;

}

  
void Trk::MultiComponentStateCombiner::useMode( bool useModeTemp )
{
  m_useMode = useModeTemp;
}
