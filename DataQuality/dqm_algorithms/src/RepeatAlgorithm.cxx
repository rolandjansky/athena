/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: RepeatAlgorithm.cxx,v 1.5 2009-05-07 14:45:54 ponyisi Exp $
// **********************************************************************

#include "dqm_algorithms/RepeatAlgorithm.h"
#include "dqm_core/LibraryManager.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/test/DummyAlgorithmConfig.h"

#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <TCollection.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TKey.h>

namespace {
  dqm_algorithms::RepeatAlgorithm instance;
}

namespace dqm_algorithms{

RepeatAlgorithm::
RepeatAlgorithm( const RepeatAlgorithm& ) :
  m_subalg() {
}  

RepeatAlgorithm& 
RepeatAlgorithm::
operator=(const RepeatAlgorithm& other) {
  if (this != &other) {
    m_subalg = other.m_subalg;
  }
  return *this;
}

RepeatAlgorithm::
RepeatAlgorithm()
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("RepeatAlgorithm", this);
}

RepeatAlgorithm::
~RepeatAlgorithm()
{
}

dqm_core::Algorithm*
RepeatAlgorithm::
clone()
{
  return new RepeatAlgorithm(*this);
}

void
RepeatAlgorithm::
printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: RepeatAlgorithm\n";
  message += "Description: Repeats the specified algorithm for each input reference.\n";
  message += "Parameters: AuxAlgName--xxx: run algorithm xxx\n";
  message += "            RepeatAlgorithm--ResultsNEntries: return # of entries of reference histogram as a result\n";
  
  out << message;
}


dqm_core::Result*
RepeatAlgorithm::
execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config )
{
  dqm_core::Result::Status status(dqm_core::Result::Undefined);
  std::map<std::string,double> tags;
  std::unique_ptr<TObjArray> returnObjs(new TObjArray);

  if (!m_subalg.get()) {
    // rely on requested subalg not changing over time
    std::string subalgname(dqm_algorithms::tools::ExtractAlgorithmName(config));
    try {
      m_subalg.reset(dqm_core::AlgorithmManager::instance().getAlgorithm(subalgname));
    }
    catch (dqm_core::Exception& ex) {
      throw dqm_core::BadConfig( ERS_HERE, "RepeatAlgorithm", ex.what(), ex );
    }
  }
  TObject* ref(0);
  try {
    ref = config.getReference();
  } catch (dqm_core::BadConfig &) {
    throw dqm_core::BadConfig( ERS_HERE, "RepeatAlgorithm", "No references defined for RepeatAlgorithm - this makes no sense" );
  }
  const TCollection* listptr(dynamic_cast<const TCollection*>(ref));
  if (!listptr) {
    throw dqm_core::BadConfig( ERS_HERE, "RepeatAlgorithm", "Reference needs to be a TCollection" );
  }
  TIter itr(listptr);
  while ( TObject* ireference = itr.Next() ) {
    boost::scoped_ptr<dqm_core::AlgorithmConfig> subConfig(ConfigureSubAlg(config, ireference));
    dqm_core::Result* subResult = m_subalg->execute( name, data, *subConfig );
    if( subResult->status_ != dqm_core::Result::Undefined ) {
      status = ( status == dqm_core::Result::Undefined ) ? dqm_core::Result::Green : status;
      status = ( subResult->status_ < status ) ? subResult->status_ : status;
    }
    
    std::map<std::string,double>::const_iterator tagsEnd  = subResult->tags_.end();
    std::map<std::string,double>::const_iterator tagsIter = subResult->tags_.begin();
    for( ; tagsIter != tagsEnd; ++tagsIter ) {
      tags[ireference->GetName() + std::string("|") + tagsIter->first] = tagsIter->second;
    }
    tags[ireference->GetName() + std::string("|Status")] = subResult->status_; 
    if ( dqm_algorithms::tools::GetFirstFromMap("RepeatAlgorithm--ResultsNEntries", config.getParameters(), 0) > 0 ) {
      TH1* hireference = dynamic_cast<TH1*>(ireference);
      if (hireference) {
	tags[ireference->GetName() + std::string("|NEntries")] = hireference->GetEntries();
      } else {
	throw dqm_core::BadConfig( ERS_HERE, "RepeatAlgorithm", std::string("Reference ") + ireference->GetName() + " is not TH1, yet we want to get # entries" );
      }
    }

    if (subResult->getObject()) {
      // do nothing here; ROOT handling is terrible
      //      returnObjs->Add(subResult->getObject()->Clone());
    }
    //delete subConfig;
    delete subResult;
  }
  
  dqm_core::Result* result = new dqm_core::Result( status );
  result->tags_ = tags;
  if (!returnObjs->IsEmpty()) {
    result->object_.reset(returnObjs.release());
  }
  
  return result;
}

dqm_core::AlgorithmConfig*
RepeatAlgorithm::
ConfigureSubAlg(const dqm_core::AlgorithmConfig& config, TObject* reference)
{
// caller owns the returned object

// what we do: copy reference, params, limits to newly created config
// copy all params except AuxAlgName--blah and RepeatAlgorithm--blah
  
  auto rv(new dqm_core::test::DummyAlgorithmConfig(reference));

  for (const auto parVal : config.getParameters()) {
    if (parVal.first.find("AuxAlgName--") == std::string::npos
	&& parVal.first.find("RepeatAlgorithm--") == std::string::npos) {
      rv->addParameter(parVal.first, parVal.second);
    }
  }
  for (const auto grthr : config.getGreenThresholds()) {
    rv->addGreenThreshold(grthr.first, grthr.second);
  }
  for (const auto rdthr : config.getRedThresholds()) {
    rv->addRedThreshold(rdthr.first, rdthr.second);
  }
  return rv;
}
	    


}//namespace dqi
