/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: CompositeAlgorithm.cxx,v 1.5 2009-05-07 14:45:54 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/CompositeAlgorithm.h"
#include "dqm_core/LibraryManager.h"
#include "dqm_core/AlgorithmManager.h"

#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <TCollection.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TKey.h>


namespace dqi{

CompositeAlgorithm::
CompositeAlgorithm( const CompositeAlgorithm& other )
  : Algorithm(other)
  , m_name(other.m_name)
{
  AlgVec_t::const_iterator subAlgsEnd = other.m_subAlgs.end();
  AlgVec_t::const_iterator subAlgsIter = other.m_subAlgs.begin();
  for( ; subAlgsIter != subAlgsEnd; ++subAlgsIter ) {
    dqm_core::Algorithm* otherSubAlg = subAlgsIter->first;    
    dqm_core::Algorithm* alg = otherSubAlg->clone();
    m_subAlgs.push_back( AlgVec_t::value_type(alg, subAlgsIter->second) );
  }
}


CompositeAlgorithm::
CompositeAlgorithm( HanConfigCompAlg& compAlgConfig )
{
  m_name = compAlgConfig.GetName();
  
  TObjString* libStr;
  TIter libIter( compAlgConfig.GetAllLib() );
  while( (libStr = dynamic_cast<TObjString*>( libIter() )) != 0 ){
    try {
      dqm_core::LibraryManager::instance().loadLibrary( libStr->GetString().Data() );
    } 
    catch ( dqm_core::Exception& ex ) {
      std::cout << "Can't load library " << libStr->GetString()
		<< " for composite algorithm " << m_name 
		<< ". Continuing regardless ..." << std::endl;
    }
  }
  
  TObjString* algStr;
  TIter algIter( compAlgConfig.GetAllAlg() );
  while( (algStr = dynamic_cast<TObjString*>( algIter() )) != 0 ){
    dqm_core::Algorithm* alg(0);
    std::string algName(algStr->GetString().Data());
    try {
    	alg = dqm_core::AlgorithmManager::instance().getAlgorithm( algName );
    }
    catch( dqm_core::Exception& ex ) {
    	throw dqm_core::BadConfig( ERS_HERE, m_name, ex.what(), ex );
    }
    //std::cout << "  --> using component algorithm: \"" << algStr->GetString().Data() << "\"\n";
    m_subAlgs.push_back( AlgVec_t::value_type(alg, algName) );
  }
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


CompositeAlgorithm::
~CompositeAlgorithm()
{
  AlgVec_t::const_iterator subAlgsEnd = m_subAlgs.end();
  AlgVec_t::const_iterator subAlgsIter = m_subAlgs.begin();
  for( ; subAlgsIter != subAlgsEnd; ++subAlgsIter ) {
    dqm_core::Algorithm* alg = subAlgsIter->first;
    delete alg;
  }
  m_subAlgs.clear();
}


dqm_core::Algorithm*
CompositeAlgorithm::
clone()
{
  dqi::CompositeAlgorithm* result = new CompositeAlgorithm(*this);
  return result;
}


void
CompositeAlgorithm::
printDescription()
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Builds a container for subalgorithms.\n";
  message += "Parameters: none\n";
  
  std::cout << message;
}


dqm_core::Result*
CompositeAlgorithm::
execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config )
{
  dqm_core::Result::Status status(dqm_core::Result::Undefined);
  std::map<std::string,double> tags;
  
  AlgVec_t::const_iterator subAlgsEnd = m_subAlgs.end();
  AlgVec_t::const_iterator subAlgsIter = m_subAlgs.begin();
  for( ; subAlgsIter != subAlgsEnd; ++subAlgsIter ) {
    boost::scoped_ptr<HanAlgorithmConfig> subConfig(ConfigureSubAlg(config, subAlgsIter->second));

    dqm_core::Algorithm* alg = subAlgsIter->first;
    dqm_core::Result* subResult = alg->execute( name, data, *subConfig );
    if( subResult->status_ != dqm_core::Result::Undefined ) {
      status = ( status == dqm_core::Result::Undefined ) ? dqm_core::Result::Green : status;
      status = ( subResult->status_ < status ) ? subResult->status_ : status;
    }
    
    std::map<std::string,double>::const_iterator tagsEnd  = subResult->tags_.end();
    std::map<std::string,double>::const_iterator tagsIter = subResult->tags_.begin();
    for( ; tagsIter != tagsEnd; ++tagsIter ) {
      std::map<std::string,double>::value_type tagVal( subAlgsIter->second + std::string("|") + tagsIter->first, tagsIter->second );
      tags.insert( tagVal );
    }
    //delete subConfig;
    delete subResult;
  }
  
  dqm_core::Result* result = new dqm_core::Result( status );
  result->tags_ = tags;
  
  return result;
}

HanAlgorithmConfig*
CompositeAlgorithm::
ConfigureSubAlg(const dqm_core::AlgorithmConfig& config, std::string subalg)
{
// caller owns the returned object

// what we do: copy reference, params, limits to newly created config
// copy all those WITHOUT a / (backwards compatibility) and those of the form
// subalg/xxx.

  TObject* ref(0);
  try {
    ref = config.getReference();
  } catch (dqm_core::BadConfig &) { /* ignore */ }
  std::map< std::string,double >  pars;
  std::map<std::string,double> grthr;
  std::map<std::string,double> rdthr;

  std::map< std::string, double > oldpars(config.getParameters());
  std::map<std::string,double> oldgrthr(config.getGreenThresholds());
  std::map<std::string,double> oldrdthr(config.getRedThresholds());

  for (std::map< std::string, double >::const_iterator parVal = oldpars.begin();
    parVal != oldpars.end(); ++parVal) {
      std::string parname(parVal->first);
      std::string::size_type pos = parname.find("|");
      if (pos == std::string::npos) {
	pars.insert(*parVal);
      } else if (parname.substr(0, pos) == subalg) {
	pars.insert(std::map< std::string, double >::value_type(parname.substr(pos+1), parVal->second));
      }
  }
  for (std::map< std::string, double >::const_iterator thrVal = oldgrthr.begin();
       thrVal != oldgrthr.end(); ++thrVal) {
    std::string thrname(thrVal->first);
    std::string::size_type pos = thrname.find("|");
    if (pos == std::string::npos) {
      grthr.insert(*thrVal);
    } else if (thrname.substr(0, pos) == subalg) {
      grthr.insert(std::map< std::string, double >::value_type(thrname.substr(pos+1), thrVal->second));
    }
  }
  for (std::map< std::string, double >::const_iterator thrVal = oldrdthr.begin();
       thrVal != oldrdthr.end(); ++thrVal) {
    std::string thrname(thrVal->first);
    std::string::size_type pos = thrname.find("|");
    if (pos == std::string::npos) {
      rdthr.insert(*thrVal);
    } else if (thrname.substr(0, pos) == subalg) {
	rdthr.insert(std::map< std::string, double >::value_type(thrname.substr(pos+1), thrVal->second));
      }
  }
  return new HanAlgorithmConfig(ref, pars, grthr, rdthr, 0);
}
	    


}//namespace dqi
