/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      MultiStateMaterialEffects.cxx  -  description
      ----------------------------------------------
begin                : Friday 18th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for the non-abstract methods for the 
                       material effects base class
************************************************************************************/

#include "TrkGaussianSumFilter/MultiStateMaterialEffects.h"

Trk::MultiStateMaterialEffects::MultiStateMaterialEffects( const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_isReset(true)
{
  declareInterface<IMultiStateMaterialEffects>(this);
}


const std::vector<double> 
Trk::MultiStateMaterialEffects::weights ( const Trk::ComponentParameters& componentParameters,
             const Trk::MaterialProperties& materialProperties,
             double pathLength,
             Trk::PropDirection direction,
             Trk::ParticleHypothesis particleHypothesis ) const
{
  // Check for new arguments to avoid computation if necessary
  if ( this->isReset() ){
    if (msgLvl(MSG::VERBOSE)) msg() << "Calculating weights" << endmsg;
    compute(componentParameters, materialProperties, pathLength, direction, particleHypothesis);
    m_isReset = false;
  }

  return m_weights;

}
  
const std::vector<double> 
Trk::MultiStateMaterialEffects::deltaPs ( const Trk::ComponentParameters& componentParameters,
             const Trk::MaterialProperties& materialProperties,
             double pathLength,
             Trk::PropDirection direction,
             Trk::ParticleHypothesis particleHypothesis ) const
{
  // Check for new arguments to avoid computation if necessary
  if ( this->isReset() ){
    if (msgLvl(MSG::VERBOSE)) msg() << "Calculating deltaP" << endmsg;
    compute(componentParameters, materialProperties, pathLength, direction, particleHypothesis);
    m_isReset = false;
  }

  return m_deltaPs;

}
  
const std::vector<const AmgSymMatrix(5)*>
Trk::MultiStateMaterialEffects::deltaCovariances ( const Trk::ComponentParameters& componentParameters,
                const Trk::MaterialProperties& materialProperties,
                double pathLength,
                Trk::PropDirection direction,
                Trk::ParticleHypothesis particleHypothesis ) const
{
  
  // Check for new arguments to avoid computation if necessary
  if ( this->isReset() ){
    if (msgLvl(MSG::VERBOSE)) msg() << "Calculating delta Covariance" << endmsg;
    compute(componentParameters, materialProperties, pathLength, direction, particleHypothesis);
    m_isReset = false;
  }
  
  return m_deltaCovariances;

}

void Trk::MultiStateMaterialEffects::reset() const
{
  
  if (msgLvl(MSG::VERBOSE)) msg() << "Resetting the material effects " << name() << endmsg;

  m_weights.clear();
  m_deltaPs.clear();

  if ( !m_deltaCovariances.empty() ){

    std::vector<const AmgSymMatrix(5)*>::const_iterator componentDeltaCovariance = m_deltaCovariances.begin();
    
    for ( ; componentDeltaCovariance != m_deltaCovariances.end(); ++componentDeltaCovariance )
      delete *componentDeltaCovariance;
    
    m_deltaCovariances.clear();
    
  }

  m_isReset = true;

  return;

}



