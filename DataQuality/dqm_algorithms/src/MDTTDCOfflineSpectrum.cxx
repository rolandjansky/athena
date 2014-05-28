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
  : name_( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::MDTTDCOfflineSpectrum * 
dqm_algorithms::MDTTDCOfflineSpectrum::clone()
{
  
  return new MDTTDCOfflineSpectrum( name_ );
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
  // const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 0); 
  // const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }

  double t0_warning;
  double t0_error;
  double tmax_warning;
  double tmax_error;
  try {
    t0_warning = dqm_algorithms::tools::GetFirstFromMap( "t0Warning", config.getParameters() );
    t0_error = dqm_algorithms::tools::GetFirstFromMap( "t0Error", config.getParameters() );
    tmax_warning = dqm_algorithms::tools::GetFirstFromMap( "tMaxWarning", config.getParameters() );
    tmax_error = dqm_algorithms::tools::GetFirstFromMap( "tMaxError", config.getParameters() );
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
  
  ERS_DEBUG(1, name_ << " TDrift " << " is " << tdrift );
  ERS_DEBUG(1,"Green threshold: t0 > "<< t0_warning << " tmax < " << tmax_warning << " ; Red threshold : t0 < " << t0_error << " tmax > " << tmax_error  );    
  
  std::map<std::string,double> tags;

  if ( t0 > t0_warning && tmax < tmax_warning && std::abs(tdrift) < 1000 && t0 < 750 && tmax > 850) {
    result->status_ = dqm_core::Result::Green;
  }
  else if ( t0 > t0_error && tmax < tmax_error && std::abs(tdrift) < 1200 && t0 < 800 && tmax > 800) {
    if(t0 < t0_warning || t0 > 800) tags["t0_Warning"] = t0;
    else tags["t0"] = t0;
    if(tmax > tmax_warning || tmax < 800) tags["tMax_Warning"] = tmax;
    else tags["tMax"] = tmax;
    if( std::abs(tdrift) > 1200 ) tags["tDrift_Warning"] = tdrift;
    else tags["tdrift"] = tdrift;
    result->status_ = dqm_core::Result::Yellow;
  }
  else {
    result->status_ = dqm_core::Result::Red;
    if(t0 < t0_error || t0 > 1000) tags["t0_Error"] = t0;
    else tags["t0"] = t0;
    if(tmax > tmax_error || tmax < 700) tags["tMax_Error"] = tmax;
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
  out << "Required Parameters: name " << name_ << ", t0Warning threshold" <<  ", tMaxWarning threshold" << std::endl;
  out << "Required parameters: t0Error" << ", tMaxError " << std::endl;
  out << "Returns yellow if t0 < t0Warning or t0 > 800 or if tMax > tMaxWarning or tMax < 800" << std::endl;
  out << "Returns red if t0 < t0Error or t0 > 1000 or if tMax > tMaxError or tMax < 700" << std::endl;
  out << "Returns red if t0Err > 2.*t0 or tMaxErr > 2.*tMax" << std::endl;
  out << "Requires that a histogram has a function named \"func1\" for the t0 fit and \"func2\" for the tMax fit" << std::endl;

}

