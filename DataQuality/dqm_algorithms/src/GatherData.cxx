/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: GatherData.cxx 585724 2014-02-28 18:07:18Z ponyisi $
// **********************************************************************

#include "dqm_algorithms/GatherData.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TH1.h>
#include <TGraph.h>
#include <TEfficiency.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"


static dqm_algorithms::GatherData staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

GatherData::
GatherData()
  : m_name("GatherData")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


GatherData::
~GatherData()
{
}


dqm_core::Algorithm*
GatherData::
clone()
{
  return new GatherData(*this);
}


dqm_core::Result*
GatherData::
execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& )
{
  dqm_core::Result::Status status(dqm_core::Result::Undefined);
  
  // Cast to the type of TObject to assess
  const TH1* h = dynamic_cast<const TH1*>( &data );
  const TGraph* g = dynamic_cast<const TGraph*>( &data );
  const TEfficiency* e = dynamic_cast<const TEfficiency*>( &data );
  if( h == 0 && g == 0 && e==0 ) {
    throw dqm_core::BadConfig( ERS_HERE, name, "Cannot cast data to type TH1, TGraph, or TEfficiency" );
  }
  
  std::map<std::string,double> tags;
  if( h != 0 ) {
    tags["Mean"] = h->GetMean();
    tags["MeanError"] = h->GetMeanError();
    tags["RMS"]  = h->GetRMS();
    tags["RMSError"]  = h->GetRMSError();
  }

  if( g != 0 ) {
    tags["XMean"] = g->GetMean(1);
    tags["YMean"] = g->GetMean(2);
    tags["XRMS"]  = g->GetRMS(1);
    tags["YRMS"]  = g->GetRMS(2);
  }

  if ( e != 0 ) {
    tags["Mean"] = e->GetCopyPassedHisto()->GetMean();
    tags["MeanError"] = e->GetCopyPassedHisto()->GetMeanError();
    tags["RMS"] = e->GetCopyPassedHisto()->GetRMS();
    tags["RMSError"] = e->GetCopyPassedHisto()->GetRMSError();
  }
  
  dqm_core::Result* result = new dqm_core::Result( status );
  result->tags_ = tags;
  
  return result;
}


void
GatherData::
printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Extracts data (ex., mean, RMS) from an object without performing\n";
  message += "             any assessment; the status is always \"Undefined\"\n";
  message += "Parameters: none\n";
  message += "Limits: none\n";
  message += "\n";
  
  out << message;
}


// *********************************************************************
// Protected Methods
// *********************************************************************


} // namespace dqm_algorithms

