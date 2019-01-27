/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/MDTTDCOfflineSpectrum.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::MDTTDCOfflineSpectrum MDTTDCOfflineSpectrum( "AlgMDTTDCOfflineSpectrum" );
}


dqm_algorithms::MDTTDCOfflineSpectrum::MDTTDCOfflineSpectrum( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::MDTTDCOfflineSpectrum * 
dqm_algorithms::MDTTDCOfflineSpectrum::clone()
{
  
  return new MDTTDCOfflineSpectrum( m_name );
}


dqm_core::Result *
dqm_algorithms::MDTTDCOfflineSpectrum::execute(	const std::string &  name, 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
{  
  TH1 * histogram;

  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  /*
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  */
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
 
  double t0_low_warning;
  double t0_low_error;
  double t0_high_warning;
  double t0_high_error;
  double tmax_low_warning;
  double tmax_low_error;
  double tmax_high_warning;
  double tmax_high_error;
  try {
    t0_low_warning = dqm_algorithms::tools::GetFirstFromMap( "t0_low_Warning", config.getParameters() );
    t0_low_error = dqm_algorithms::tools::GetFirstFromMap( "t0_low_Error", config.getParameters() );
    t0_high_warning = dqm_algorithms::tools::GetFirstFromMap( "t0_high_Warning", config.getParameters() );
    t0_high_error = dqm_algorithms::tools::GetFirstFromMap( "t0_high_Error", config.getParameters() );
    tmax_low_warning = dqm_algorithms::tools::GetFirstFromMap( "tMax_low_Warning", config.getParameters() );
    tmax_low_error = dqm_algorithms::tools::GetFirstFromMap( "tMax_low_Error", config.getParameters() );
    tmax_high_warning = dqm_algorithms::tools::GetFirstFromMap( "tMax_high_Warning", config.getParameters() );
    tmax_high_error = dqm_algorithms::tools::GetFirstFromMap( "tMax_high_Error", config.getParameters() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  dqm_core::Result* result = new dqm_core::Result();
  
  TF1* t0Fit = histogram->GetFunction("func1");
  TF1* tmaxFit = histogram->GetFunction("func2");
  if(!t0Fit || !tmaxFit)  throw dqm_core::BadConfig( ERS_HERE, name, "TH1 has no TF1" );
  double t0 = t0Fit->GetParameter(1);
  double tmax = tmaxFit->GetParameter(1);
  double tdrift = tmax - t0;
  double t0Err = t0Fit->GetParameter(2);
  double tmaxErr = tmaxFit->GetParameter(2);
  
  ERS_DEBUG(1, m_name << " TDrift " << " is " << tdrift );
  ERS_DEBUG(1,"Green threshold: "<< t0_low_warning << " < t0 < "<< t0_high_warning << " &&  " << tmax_low_warning <<" < tmax < " << tmax_high_warning <<   
           " ;  Red threshold : t0 < " << t0_low_error      << "\n" << 
                               "t0 > " << t0_high_error     << "\n" << 
                               "tmax > " << tmax_high_error << "\n" <<
                               "tmax < " << tmax_low_error
            );    
  
  std::map<std::string,double> tags;

  if (t0 > t0_low_warning && t0 < t0_high_warning && tmax < tmax_high_warning && tmax > tmax_low_warning && std::abs(tdrift) < 1000) {
    result->status_ = dqm_core::Result::Green;
  }
  else if (t0 > t0_low_error && t0 < t0_high_error && tmax < tmax_high_error && tmax > tmax_low_error && std::abs(tdrift) < 1200) {
    if(t0 < t0_low_warning) tags["t0_low_Warning"] = t0;
    else if(t0 > t0_high_warning) tags["t0_high_Warning"] = t0;
    else tags["t0"] = t0;
    if(tmax > tmax_high_warning) tags["tMax_high_Warning"] = tmax;
    else if(tmax < tmax_low_warning) tags["tMax_low_Warning"] = tmax;
    else tags["tMax"] = tmax;
    if( std::abs(tdrift) > 1200 ) tags["tDrift_Warning"] = tdrift;
    else tags["tdrift"] = tdrift;
    result->status_ = dqm_core::Result::Yellow;
  }
  else {
    result->status_ = dqm_core::Result::Red;
    if(t0 < t0_low_error) tags["t0_low_Error"] = t0;
    else if(t0 > t0_high_error) tags["t0_high_Error"] = t0;
    else tags["t0"] = t0;
    if(tmax > tmax_high_error) tags["tMax_high_Error"] = tmax;
    else if(tmax < tmax_low_error) tags["tMax_low_Error"] = tmax;
    else tags["tMax"] = tmax;
    if( std::abs(tdrift) > 1400 ) tags["tDrift_Error"] = tdrift;
    else tags["tdrift"] = tdrift;
  }

  if(tags.size()==0) {
    tags["t0"] = t0;
    tags["tmax"] = tmax;
    tags["tdrift"] = tdrift;
  }
  if(t0Err > 2.*t0){
    tags["t0_FitError"] = t0Err;
    result->status_ = dqm_core::Result::Red;
  }
  if(tmaxErr > 2.*tmax) {
    tags["tmax_FitError"] = tmaxErr;
    result->status_ = dqm_core::Result::Red;
  }

  result->tags_ = tags;
 
  return result;
  
}

void
dqm_algorithms::MDTTDCOfflineSpectrum::printDescription(std::ostream& out)
{
  
  out << "Analyze pre-fitted MDTTDC spectra histograms" << std::endl;
  out << "Required Parameters: name " << m_name << ", t0Warning threshold" <<  ", tMaxWarning threshold" << std::endl;
  out << "Required parameters: t0Error" << ", tMaxError " << std::endl;
  out << "Returns yellow if t0 < t0Warning or t0 > 800 or if tMax > tMaxWarning or tMax < 800" << std::endl;
  out << "Returns red if t0 < t0Error or t0 > 1000 or if tMax > tMaxError or tMax < 700" << std::endl;
  out << "Returns red if t0Err > 2.*t0 or tMaxErr > 2.*tMax" << std::endl;
  out << "Requires that a histogram has a function named \"func1\" for the t0 fit and \"func2\" for the tMax fit" << std::endl;

}

