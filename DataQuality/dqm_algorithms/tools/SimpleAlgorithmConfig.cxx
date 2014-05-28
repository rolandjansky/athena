/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SimpleAlgorithmConfig  Defines the class SimpleAlgorithmConfig a concrete simple implementation of dqm_core::AlgorithmConfig
 *  \author andrea.dotti@cern.ch
 */
#include <dqm_algorithms/tools/SimpleAlgorithmConfig.h>

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig() :
  dqm_core::AlgorithmConfig() , ref_(0)
{
  //Empty
}

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig( TObject * ref ) :
  dqm_core::AlgorithmConfig() , ref_(ref)
{
  //Empty
}

#ifndef __MAKECINT__

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig(const AlgorithmConfig& conf):
  dqm_core::AlgorithmConfig()
{
  //Copy configuration in this
  ref_ = conf.getReference();
  param_ = conf.getParameters();
  green_ = conf.getGreenThresholds();
  red_   = conf.getRedThresholds();
}
#endif

TObject* 
dqm_algorithms::tools::SimpleAlgorithmConfig::getReference() const
{
  return ref_;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getParameters() const
{
  return param_;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getGreenThresholds() const
{
  return green_;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getRedThresholds() const
{
  return red_;
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::setReference(TObject* o)
{
  ref_ = o;
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addParameter(std::string key, double value)
{
  param_.insert( std::make_pair(key,value) );
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addGreenThreshold(std::string key, double value)
{
  green_.insert( std::make_pair(key,value) );
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addRedThreshold(std::string key, double value)
{
  red_.insert( std::make_pair(key,value) );
}

