/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      PosteriorWeightsCalculator.cxx  -  description
      -----------------------------------------------
created              : Wednesday 22nd December 2004
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
description          : Implementation code for PosteriorWeightsCalculator class
*********************************************************************************/

#include "TrkGaussianSumFilter/PosteriorWeightsCalculator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"

Trk::PosteriorWeightsCalculator::PosteriorWeightsCalculator(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent)
{

  declareInterface<IPosteriorWeightsCalculator>(this);

}

StatusCode Trk::PosteriorWeightsCalculator::initialize() 
{

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg; 

  return StatusCode::SUCCESS;

}

StatusCode Trk::PosteriorWeightsCalculator::finalize() 
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState*
Trk::PosteriorWeightsCalculator::weights ( const MultiComponentState& predictedState, const MeasurementBase& measurement ){
  
  if (msgLvl(MSG::VERBOSE)) msg() << "Calculating Posterior Weights" << endmsg;
  
  if ( predictedState.empty() ) {
    msg(MSG::WARNING) << "Predicted state is empty... Exiting!" << endmsg;
    return 0;
  }
  
  if (msgLvl(MSG::VERBOSE)) msg() << "State for update is valid!" << endmsg;
    
  Trk::MultiComponentState* returnMultiComponentState = new Trk::MultiComponentState();
  
  std::vector<double> componentDeterminantR;
  std::vector<double> componentChi2;
  
  // Calculate chi2 and determinant of each component.
  double minimumChi2(10.e10); // Initalise high
  
  // Loop over all components in the prediction state
  Trk::MultiComponentState::const_iterator component = predictedState.begin();
  
  for ( ; component != predictedState.end() ; ++component ){

    const Trk::TrackParameters* componentTrackParameters = (*component).first;

    if ( !componentTrackParameters ) {
      if (msgLvl(MSG::DEBUG)) msg() << "Component in the state prepared for update is invalid... Ignoring!" << endmsg;
      continue;
    }
    
    const AmgSymMatrix(5)* predictedCov = componentTrackParameters->covariance();
    
    if (!predictedCov){
      msg(MSG::WARNING) << "No measurement associated with track parameters... Ignoring!" << endmsg;
      continue;
    }

    if (msgLvl(MSG::VERBOSE)) msg() << "Component for update is valid!" << endmsg;
    
    // Extract the LocalParameters from the MeasurementBase
    const Trk::LocalParameters& measurementLocalParameters = measurement.localParameters();
    
    // Extract predicted state track parameters
    Amg::VectorX trackParameters = componentTrackParameters->parameters();

    // Define the expansion matrix
    const Amg::MatrixX& H = measurementLocalParameters.expansionMatrix();

    // Calculate the residual
    Amg::VectorX r = measurementLocalParameters - H * trackParameters;

    // Extract the CovarianceMatrix of the MeasurementBase
    const Amg::MatrixX& measurementCovariance   = measurement.localCovariance();

    // Residual covariance. Posterior weights is calculated used predicted state and measurement. Therefore add covariances
    Amg::MatrixX R ( measurementCovariance + H * (*predictedCov) * H.transpose() );

    //compute determinant of residual
    double determinantR = R.determinant();

    if (determinantR==0){
      msg(MSG::WARNING) << "Determinant is 0, cannot invert matrix... Ignoring component" << endmsg;
      continue;
    }
    // Compute Chi2
    double size = measurementCovariance.innerSize();

    double chi2 = (1./size)*((r.transpose() * R.inverse() * r)(0,0));
 
    if (msgLvl(MSG::VERBOSE)) msg() << "determinant R / chiSquared: " << determinantR << '\t' << chi2 << endmsg;

    componentDeterminantR.push_back(determinantR);
    componentChi2.push_back(chi2);
    
    if ( chi2 < minimumChi2 )
      minimumChi2 = chi2;
    
  } // end loop over components
  
  if ( componentDeterminantR.size() != predictedState.size() || componentChi2.size() != predictedState.size() ){
    msg(MSG::WARNING) << "Inconsistent number of components in chi2 and detR vectors... Exiting!" << endmsg;
    return 0;
  }
  
  // Calculate posterior weights.
  
  unsigned int index(0);
  double sumWeights(0.);

  component = predictedState.begin();
  
  for ( ; component != predictedState.end(); ++component, ++index){

    double priorWeight = (*component).second;
      
    // Extract common factor to avoid numerical problems during exponentiation
    double chi2 = componentChi2[index] - minimumChi2;
    

  
    double updatedWeight(0.);
    // Determinant can not be belowe 1e-19 in CLHEP .... rather ugly but protect against 0 determinants
    // Normally occur when the component is a poor fit 
    if (componentDeterminantR[index] > 1e-20) 
      updatedWeight = priorWeight * sqrt (1. / componentDeterminantR[index]) * exp( -0.5 * chi2 );
    else 
      updatedWeight  = 1e-10;
    
    ATH_MSG_VERBOSE(" Prior Weight: " << priorWeight << "  Updated Weight:  "<< updatedWeight );      
    Trk::ComponentParameters componentWithNewWeight( component->first->clone(), updatedWeight );
    returnMultiComponentState->push_back(componentWithNewWeight);
    sumWeights += updatedWeight;
  }
  
  if ( returnMultiComponentState->size() != predictedState.size() ){
    msg(MSG::WARNING) << "Inconsistent number of components between initial and final states... Exiting!" << endmsg;
    return 0;
  }
  
  // Renormalise the state to total weight = 1
  Trk::MultiComponentState::iterator returnComponent = returnMultiComponentState->begin();
  component = predictedState.begin();

  for ( ; returnComponent != returnMultiComponentState->end() ; ++returnComponent, ++component ){ 
    if (sumWeights > 0. ){
      (*returnComponent).second /= sumWeights;
    } else {
      (*returnComponent).second  = component->second;
    }
  }  

  return returnMultiComponentState;

}
