/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: CheckMean.cxx,v 1.1 2007-04-06 11:02:53 mgwilson Exp $
// **********************************************************************

#include "dqm_algorithms/CheckMean.h"

#include <cmath>
#include <iostream>

#include <TH1.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"


static dqm_algorithms::CheckMean staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

CheckMean::
CheckMean()
  : m_name("AlgCheckMean")
  , m_limitName("chi2")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


CheckMean::
~CheckMean()
{
}


dqm_core::Algorithm*
CheckMean::
clone()
{
  return new CheckMean(*this);
}


dqm_core::Result*
CheckMean::
execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& config )
{
  dqm_core::Result::Status status(dqm_core::Result::Undefined);
  
  // Cast to the type of TObject to assess
  const TH1* h = dynamic_cast<const TH1*>( &data );
  if( h == 0 ) {
    throw dqm_core::BadConfig( ERS_HERE, name, "Cannot cast data to type TH1" );
  }
  
  const TH1* ref = dynamic_cast<const TH1*>( config.getReference() );
  if( ref == 0 ) {
    throw dqm_core::BadConfig( ERS_HERE, name, "Cannot obtain reference of type TH1" );
  }
  
  // Perform the check
  const double mean_data = h->GetMean();
  const double mean_ref  = ref->GetMean();
  
  const double mean_data_err = h->GetMeanError();
  const double mean_ref_err  = ref->GetMeanError();
  
  const double chi2_warn = getWarningLimit( config, m_limitName );
  const double chi2_err = getErrorLimit( config, m_limitName );
  
  const double s  = ( mean_data - mean_ref );
  const double s2 = s*s;
  const double e2 =  mean_data_err*mean_data_err + mean_ref_err*mean_ref_err;
  
  const double chi2 = s2/e2;
  
  //out << "mean_data = " << mean_data << "\n";
  //out << "mean_ref = " << mean_ref << "\n";
  //out << "mean_data_err = " << mean_data_err << "\n";
  //out << "mean_ref_err = " << mean_ref_err << "\n";
  //out << "chi2 = " << chi2 << "\n";
  
  if( chi2 < chi2_warn ) {
    status = dqm_core::Result::Green;
  }
  else if( chi2 < chi2_err ) {
    status = dqm_core::Result::Yellow;
  }
  else {
    status = dqm_core::Result::Red;
  }
  
  // Return the result
  return new dqm_core::Result( status );
}


void
CheckMean::
printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Checks the mean of a TH1 against the mean of a reference TH1\n";
  message += "             by assessing a chi2 defined by the two means and their errors\n";
  message += "Parameters: none\n";
  message += "Limits:\n";
  message += " \"" + m_limitName + "\" : generate warnings on the chi2 value\n";
  message += "\n";
  
  out << message;
}


// *********************************************************************
// Protected Methods
// *********************************************************************


double
CheckMean::
getWarningLimit( const dqm_core::AlgorithmConfig& config, std::string limitName )
{
  std::map<std::string,double> grmap = config.getGreenThresholds();
  std::map<std::string,double>::const_iterator i = grmap.find( limitName );
  if( i == grmap.end() ) {
    throw dqm_core::BadConfig( ERS_HERE, limitName, "Cannot find green threshold" );
  }
  
  return i->second;
}


double
CheckMean::
getErrorLimit( const dqm_core::AlgorithmConfig& config, std::string limitName )
{
  std::map<std::string,double> rdmap = config.getRedThresholds();
  std::map<std::string,double>::const_iterator i = rdmap.find( limitName );
  if( i == rdmap.end() ) {
    throw dqm_core::BadConfig( ERS_HERE, limitName, "Cannot find red threshold" );
  }
  
  return i->second;
}


} // namespace dqm_algorithms

