/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			MultiComponentState.cxx  -  description
			---------------------------------------
begin                : Sunday 8th May 2005
author               : atkinson, amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for MultiComponentState class			
*********************************************************************************/

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/Surface.h"

#include "GaudiKernel/MsgStream.h"

Trk::MultiComponentState::MultiComponentState()
  :
  std::list< Trk::ComponentParameters >()
{}

Trk::MultiComponentState::MultiComponentState( const Trk::ComponentParameters& componentParameters )
  :
  std::list< Trk::ComponentParameters >()
{
  this->push_back( componentParameters );
}

Trk::MultiComponentState::~MultiComponentState()
{
  Trk::MultiComponentState::const_iterator component = this->begin();
  for ( ; component != this->end(); ++component ) delete component->first;
  this->clear(); 
}

const Trk::MultiComponentState* Trk::MultiComponentState::clone() const
{
  Trk::MultiComponentState* clonedState = new Trk::MultiComponentState();
  Trk::MultiComponentState::const_iterator component = this->begin();
  for ( ; component != this->end(); ++component ){
    const Trk::TrackParameters* clonedParameters = component->first->clone();
    Trk::ComponentParameters componentParameters( clonedParameters, component->second );
    clonedState->push_back( componentParameters );
  }
  return clonedState;
}


const Trk::MultiComponentState* Trk::MultiComponentState::cloneWithWeightScaling( double scalingFactor ) const
{
  Trk::MultiComponentState* clonedState = new Trk::MultiComponentState();
  Trk::MultiComponentState::const_iterator component = this->begin();
  for ( ; component != this->end(); ++component ){
    const Trk::TrackParameters* clonedParameters = (component->first)->clone();
    Trk::ComponentParameters componentParameters( clonedParameters, component->second * scalingFactor );
    clonedState->push_back( componentParameters );
  }
  return clonedState;

}


const Trk::MultiComponentState* Trk::MultiComponentState::cloneWithScaledError( double errorScaleLocX,double errorScaleLocY, double errorScalePhi,
                                                                                double errorScaleTheta,double errorScaleQoverP ) const
{
  Trk::MultiComponentState* stateWithScaledErrors = new Trk::MultiComponentState();
  Trk::MultiComponentState::const_iterator component = this->begin();
  for ( ; component != this->end(); ++component ){
    const Trk::TrackParameters* trackParameters = component->first;
    const AmgSymMatrix(5)* originalMatrix = trackParameters->covariance();
    if (!originalMatrix) {
      delete stateWithScaledErrors;
      return this->clone();
    }

     
    AmgSymMatrix(5)* covarianceMatrix = new AmgSymMatrix(5);
        
    int size = covarianceMatrix->rows();
    if ( size == 5 ){
      (*covarianceMatrix)( 0, 0 ) = (*originalMatrix)(0,0) * (errorScaleLocX * errorScaleLocX) ;
      (*covarianceMatrix)( 1, 1 ) = (*originalMatrix)(1,1) * (errorScaleLocY * errorScaleLocY) ;
      (*covarianceMatrix)( 2, 2 ) = (*originalMatrix)(2,2) * (errorScalePhi * errorScalePhi) ;
      (*covarianceMatrix)( 3, 3 ) = (*originalMatrix)(3,3) * (errorScaleTheta * errorScaleTheta) ;
      (*covarianceMatrix)( 4, 4 ) = (*originalMatrix)(4,4) * (errorScaleQoverP * errorScaleQoverP) ;

      covarianceMatrix->fillSymmetric( 0, 1 , (*originalMatrix)(0,1) * (errorScaleLocX * errorScaleLocY) );
      covarianceMatrix->fillSymmetric( 0, 2 , (*originalMatrix)(0,2) * (errorScaleLocX * errorScalePhi)  );
      covarianceMatrix->fillSymmetric( 0, 3 , (*originalMatrix)(0,3) * (errorScaleLocX * errorScaleTheta) );
      covarianceMatrix->fillSymmetric( 0, 4 , (*originalMatrix)(0,4) * (errorScaleLocX * errorScaleQoverP) );
      
      covarianceMatrix->fillSymmetric( 1, 2 , (*originalMatrix)(1,2) * (errorScaleLocY * errorScalePhi) );
      covarianceMatrix->fillSymmetric( 1, 3 , (*originalMatrix)(1,3) * (errorScaleLocY * errorScaleTheta) );
      covarianceMatrix->fillSymmetric( 1, 4 , (*originalMatrix)(1,4) * (errorScaleLocY * errorScaleQoverP) );
      
      covarianceMatrix->fillSymmetric( 2, 3 , (*originalMatrix)(2,3) * (errorScalePhi * errorScaleTheta) );
      covarianceMatrix->fillSymmetric( 2, 4 , (*originalMatrix)(2,4) * (errorScalePhi * errorScaleQoverP) );
      
      covarianceMatrix->fillSymmetric( 3, 4 , (*originalMatrix)(3,4) * (errorScaleTheta * errorScaleQoverP) );
      
    } else {
      delete covarianceMatrix;
      delete stateWithScaledErrors;
      return this->clone();
    }
      

    const Amg::VectorX& par = trackParameters->parameters(); 
 	  const TrackParameters* newTrackParameters 
 	          = trackParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2], 
 	                                                                       par[Trk::phi],par[Trk::theta],
 	                                                                       par[Trk::qOverP],covarianceMatrix); 
 
 
    // Recalling of error does not change weighting
    const Trk::ComponentParameters componentParameters( newTrackParameters, component->second );
    
    // Push back new component
    stateWithScaledErrors->push_back( componentParameters );

  }

  return stateWithScaledErrors;

}



const Trk::MultiComponentState* Trk::MultiComponentState::cloneWithScaledError( double errorScale ) const
{

  Trk::MultiComponentState* stateWithScaledErrors = new Trk::MultiComponentState();

  Trk::MultiComponentState::const_iterator component = this->begin();

  for ( ; component != this->end(); ++component ){

    const Trk::TrackParameters* trackParameters = component->first;

   
    const AmgSymMatrix(5)* originalMatrix = trackParameters->covariance();
    if (!originalMatrix) {
      delete stateWithScaledErrors;
      return this->clone();
    }


    AmgSymMatrix(5)* covarianceMatrix = new AmgSymMatrix( 5 );
    
   
    (*covarianceMatrix)( 0, 0 ) = (*originalMatrix)(0,0) * errorScale ;
    (*covarianceMatrix)( 1, 1 ) = (*originalMatrix)(1,1) * errorScale ;
    (*covarianceMatrix)( 2, 2 ) = (*originalMatrix)(2,2) * errorScale ;
    (*covarianceMatrix)( 3, 3 ) = (*originalMatrix)(3,3) * errorScale ;
    (*covarianceMatrix)( 4, 4 ) = (*originalMatrix)(4,4) * errorScale ;
   
    covarianceMatrix->fillSymmetric( 0, 1 , (*originalMatrix)(0,1) * errorScale );
    covarianceMatrix->fillSymmetric( 0, 2 , (*originalMatrix)(0,2) * errorScale );
    covarianceMatrix->fillSymmetric( 0, 3 , (*originalMatrix)(0,3) * errorScale );
    covarianceMatrix->fillSymmetric( 0, 4 , (*originalMatrix)(0,4) * errorScale);
    
    covarianceMatrix->fillSymmetric( 1, 2 , (*originalMatrix)(1,2) * errorScale );
    covarianceMatrix->fillSymmetric( 1, 3 , (*originalMatrix)(1,3) * errorScale );
    covarianceMatrix->fillSymmetric( 1, 4 , (*originalMatrix)(1,4) * errorScale );

    covarianceMatrix->fillSymmetric( 2, 3 , (*originalMatrix)(2,3) * errorScale );
    covarianceMatrix->fillSymmetric( 2, 4 , (*originalMatrix)(2,4) * errorScale );

    covarianceMatrix->fillSymmetric( 3, 4 , (*originalMatrix)(3,4) * errorScale );

   
    const Amg::VectorX& par = trackParameters->parameters(); 
 	  const TrackParameters* newTrackParameters 
 	          = trackParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2], 
 	                                                                       par[Trk::phi],par[Trk::theta],
 	                                                                       par[Trk::qOverP],covarianceMatrix); 
 


    // Recalling of error does not change weighting
    const Trk::ComponentParameters componentParameters( newTrackParameters, component->second );
    
    // Push back new component
    stateWithScaledErrors->push_back( componentParameters );

  }

  return stateWithScaledErrors;

}


bool Trk::MultiComponentState::isMeasured() const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  bool isNotMeasured = false;

  for ( ; component != this->end(); ++component ){

    const AmgSymMatrix(5)* originalMatrix = component->first->covariance();
    if (!originalMatrix)    
      isNotMeasured = true;
    
  }

  return !isNotMeasured;

}

const Trk::MultiComponentState* Trk::MultiComponentState::clonedRenormalisedState() const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  // Determine total weighting of state

  double sumWeights = 0.;

  for ( ; component != this->end(); ++component )
    sumWeights += component->second;

  Trk::MultiComponentState* renormalisedState = new Trk::MultiComponentState();

  component = this->begin();

  for ( ; component != this->end(); ++component ){
    Trk::ComponentParameters componentParameters( component->first->clone(), component->second / sumWeights);
    renormalisedState->push_back( componentParameters );
  }

  return renormalisedState;

}

MsgStream& Trk::MultiComponentState::dump( MsgStream& out ) const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  out << "***************** Multi-Component State print *****************" << std::endl;

  out << "State address: " << this << std::endl;

  int componentCounter(1);
  int stateSize = this->size();

  for ( ; component != this->end(); ++component, ++componentCounter ){
    out << "Component " << componentCounter << " of " << stateSize << std::endl;
    out << "Component address: " << component->first << std::endl;
    out << "Track parameters: " << *(component->first) << std::endl;
    out << "Weight: " << component->second << std::endl;
    out << "------------------------------------------------" << std::endl;
  }

  out << "********************** End of state print *********************" << std::endl;

  return out;

}

std::ostream& Trk::MultiComponentState::dump( std::ostream& out ) const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  out << "***************** Multi-Component State print *****************" << std::endl;

  out << "State address: " << this << std::endl;

  int componentCounter(1);
  int stateSize = this->size();

  for ( ; component != this->end(); ++component, ++componentCounter ){
    out << "Component " << componentCounter << " of " << stateSize << std::endl;
    out << "Component address: " << component->first << std::endl;
    out << "Track parameters: " << *(component->first) << std::endl;
    out << "Weight: " << component->second << std::endl;
    out << "------------------------------------------------" << std::endl;
  }

  out << "********************** End of state print *********************" << std::endl;

  return out;

}

MsgStream& Trk::operator << ( MsgStream& stream, const Trk::MultiComponentState& multiComponentState )
{
  return multiComponentState.dump( stream );
}

std::ostream& Trk::operator << ( std::ostream& stream , const MultiComponentState& multiComponentState )
{
  return multiComponentState.dump( stream );
}
