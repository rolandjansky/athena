/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
		        MahalanobisComponentDistance.cxx  -  description
			------------------------------------------------
begin                : Thursday 24th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for MahalanobisComponentDistance class
*********************************************************************************/

#include "TrkGaussianSumFilter/MahalanobisComponentDistance.h"

Trk::MahalanobisComponentDistance::MahalanobisComponentDistance(const std::string type, const std::string name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_outputlevel(0)
{

  declareInterface<IComponentSeparationDistance>(this);

}

StatusCode Trk::MahalanobisComponentDistance::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member

  msg(MSG::INFO) << "Initialisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::MahalanobisComponentDistance::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << type() << " under instance " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

double Trk::MahalanobisComponentDistance::operator() (const Trk::ComponentParameters& firstComponent, const Trk::ComponentParameters& secondComponent) const
{

  const AmgSymMatrix(5)* firstMeasuredCov  = firstComponent.first->covariance();
  const AmgSymMatrix(5)* secondMeasuredCov = secondComponent.first->covariance();

  if (!firstMeasuredCov || !secondMeasuredCov){
    msg(MSG::ERROR) << "Attempting to calculate the separation of components without errors" << endmsg;
    return -1.;
  }

  const Amg::VectorX& firstComponentParameters  = firstComponent.first->parameters();
  const Amg::VectorX& secondComponentParameters = secondComponent.first->parameters();

  const AmgSymMatrix(5) covarianceSum = *firstMeasuredCov + *secondMeasuredCov;

  //int matrixInversionError = 0;

  const AmgSymMatrix(5) inverseCovarianceSum = covarianceSum.inverse();

  const Amg::VectorX parametersDifference = firstComponentParameters - secondComponentParameters;

  double distance = parametersDifference.transpose() * inverseCovarianceSum * parametersDifference; //u^T * M * u

  return distance;

}
