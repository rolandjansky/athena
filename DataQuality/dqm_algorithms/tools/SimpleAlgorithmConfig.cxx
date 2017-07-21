/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SimpleAlgorithmConfig  Defines the class SimpleAlgorithmConfig a concrete simple implementation of dqm_core::AlgorithmConfig
 *  \author andrea.dotti@cern.ch
 */
#include <dqm_algorithms/tools/SimpleAlgorithmConfig.h>

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig() :
  dqm_core::AlgorithmConfig() , m_ref(0)
{
  //Empty
}

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig( TObject * ref ) :
  dqm_core::AlgorithmConfig() , m_ref(ref)
{
  //Empty
}

#ifndef __MAKECINT__

dqm_algorithms::tools::SimpleAlgorithmConfig::SimpleAlgorithmConfig(const AlgorithmConfig& conf):
  dqm_core::AlgorithmConfig()
{
  //Copy configuration in this
  m_ref = conf.getReference();
  m_param = conf.getParameters();
  m_green = conf.getGreenThresholds();
  m_red   = conf.getRedThresholds();
}
#endif

TObject* 
dqm_algorithms::tools::SimpleAlgorithmConfig::getReference() const
{
  return m_ref;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getParameters() const
{
  return m_param;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getGreenThresholds() const
{
  return m_green;
}

const std::map<std::string, double>&
dqm_algorithms::tools::SimpleAlgorithmConfig::getRedThresholds() const
{
  return m_red;
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::setReference(TObject* o)
{
  m_ref = o;
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addParameter(std::string key, double value)
{
  m_param.insert( std::make_pair(key,value) );
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addGreenThreshold(std::string key, double value)
{
  m_green.insert( std::make_pair(key,value) );
}

void
dqm_algorithms::tools::SimpleAlgorithmConfig::addRedThreshold(std::string key, double value)
{
  m_red.insert( std::make_pair(key,value) );
}

