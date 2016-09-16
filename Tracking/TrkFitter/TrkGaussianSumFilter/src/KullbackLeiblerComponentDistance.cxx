/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
		        KullbackLeiblerComponentDistance.cxx  -  description
			----------------------------------------------------
begin                : Thursday 24th February 2005
author               : atkinson  &  amorley
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for KullbackLeiblerComponentDistance
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/KullbackLeiblerComponentDistance.h"
#include "TrkParameters/TrackParameters.h"

Trk::KullbackLeiblerComponentDistance::KullbackLeiblerComponentDistance(const std::string type, const std::string name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0),
  m_do1D(true)
{

  declareInterface<IComponentSeparationDistance>(this);
  declareProperty("DoOneDimensionalDistance", m_do1D);

}

StatusCode Trk::KullbackLeiblerComponentDistance::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member
  
  msg(MSG::INFO) << "Initialisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::KullbackLeiblerComponentDistance::finalize()
{
  
  msg(MSG::INFO) << "Finalisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

double Trk::KullbackLeiblerComponentDistance::operator() (const Trk::ComponentParameters& firstComponent, const Trk::ComponentParameters& secondComponent) const
{

  // msg(MSG::VERBOSE) << "Calculating the Kullback-Leibler distance between 2 components" << endmsg;

  const AmgSymMatrix(5)* firstMeasuredCov  = firstComponent.first->covariance();
  const AmgSymMatrix(5)* secondMeasuredCov = secondComponent.first->covariance();

  const Amg::VectorX& firstComponentParameters  = firstComponent.first->parameters();
  const Amg::VectorX& secondComponentParameters = secondComponent.first->parameters();

  if (!firstMeasuredCov || !secondMeasuredCov){
    if (m_outputlevel <= 0) 
      msg(MSG::DEBUG) << "Attempting to calculate the separation of components without errors... return 0" << endmsg;
    return 0.; 
  }

  // std::cout << "First covariance: " << firstComponentCovariance << std::endl;
  // std::cout << "Second covariance: " << secondComponentCovariance << std::endl;


  if( firstComponentParameters.rows() == 5  &&  secondComponentParameters.rows() == 5 ){
    
    if(m_do1D){
      double firstPars  = firstComponentParameters[Trk::qOverP];
      double secondPars = secondComponentParameters[Trk::qOverP];
  
      double firstCovTrk  = (*firstMeasuredCov)(Trk::qOverP,Trk::qOverP);
      double secondCovTrk = (*secondMeasuredCov)(Trk::qOverP,Trk::qOverP);
    
      double G1 =  firstCovTrk > 0  ? 1./firstCovTrk : 1e10;
      double G2 =  secondCovTrk > 0 ? 1./secondCovTrk : 1e10;
      
      double parametersDifference = firstPars - secondPars;
      double covarianceDifference = firstCovTrk - secondCovTrk;
      double G_difference = G2 - G1;
      double G_sum        = G1 + G2;      
      
      double distance = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
      
      return distance;    
    }
  
    //int matrixInversionError = 0;
    
    AmgSymMatrix(5) G1 =  firstMeasuredCov->inverse();
    
    //if (matrixInversionError != 0 && m_outputlevel <= 0 )
    //  msg(MSG::DEBUG) << "Inversion of covariance matrix of first component failed" << endmsg;

    AmgSymMatrix(5) G2 =  secondMeasuredCov->inverse();

    //if (matrixInversionError != 0 && m_outputlevel <= 0 )
    //  msg(MSG::DEBUG) << "Inversion of covariance matrix of second component failed" << endmsg;


    Amg::VectorX parametersDifference = firstComponentParameters - secondComponentParameters;

    //Ensure that we don't have any problems with the cyclical nature of phi 
    if( parametersDifference[2] > M_PI ){
      parametersDifference[2] -= 2 * M_PI;
    } else if ( parametersDifference[2] < -M_PI ){
      parametersDifference[2] += 2 * M_PI;
    }

    
    
    AmgSymMatrix(5) covarianceDifference = *firstMeasuredCov - *secondMeasuredCov;
    AmgSymMatrix(5) G_difference = G2 - G1;
    AmgSymMatrix(5) G_sum        = G1 + G2;

    //SGenMatrix5 covDiffbyinvDiff = covarianceDifference * G_difference;
    //double distancePart1 =  covarianceDifference.Trace();
    
    double distancePart1(0);
    //Trace of the two matricies
    for (int i=0; i<5; ++i) {
      for (int j=0; j<5; ++j) {
        distancePart1 += covarianceDifference(j,i) * G_difference(i,j);
      }
    }


    //double distancePart1 =  covarianceDifference.Trace(); //(covarianceDifference * G_difference).Trace(); 
    double distancePart2 =  parametersDifference.transpose() *  G_sum * parametersDifference ; //ROOT::Math::Similarity( G_sum, parametersDifference ); 

    double distance = distancePart1  + distancePart2;
  
    //std::cout << "Distance  "<<  distance << std::endl;
    return distance;
  } else {
    //int matrixInversionError = 0;
  
    AmgSymMatrix(5) G1 = firstMeasuredCov->inverse();
  
    //if (matrixInversionError != 0 && m_outputlevel <= 0 )
    //  msg(MSG::DEBUG) << "Inversion of covariance matrix of first component failed" << endmsg;
  
    //matrixInversionError = 0;
  
    AmgSymMatrix(5) G2 = secondMeasuredCov->inverse();
    
    //if (matrixInversionError != 0 && m_outputlevel <= 0 )
    //  msg(MSG::DEBUG) << "Inversion of covariance matrix of second component failed" << endmsg;
  
    Amg::VectorX parametersDifference = firstComponentParameters - secondComponentParameters;
  
    // std::cout << "First new covariance: " <<  G1 << std::endl;
    // std::cout << "Second new covariance: " <<  G2 << std::endl;
  
    AmgSymMatrix(5) covarianceDifference = *firstMeasuredCov - *secondMeasuredCov;
    AmgSymMatrix(5) G_difference = G2 - G1;
    AmgSymMatrix(5) G_sum        = G1 + G2;
  
    double distance = (covarianceDifference * G_difference).trace() + (parametersDifference.transpose() * G_sum * parametersDifference);
  
  
    //msg(MSG::VERBOSE) << "Kullback-Leibler distance: " << distance << endmsg;
    //std::cout << "Kullback-Leibler distance: " <<  distance << std::endl;
  
    return distance;
  }
}


