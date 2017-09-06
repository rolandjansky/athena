/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicStatCheck.cxx checks basic histogram quanitites like over/underflow; if filled and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BasicStatCheck.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <ers/ers.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>


#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::BasicStatCheck Mean ( "Mean" );
  dqm_algorithms::BasicStatCheck RMS ( "RMS" );
}


dqm_algorithms::BasicStatCheck::BasicStatCheck( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("CheckHisto_"+name, this);
}

dqm_algorithms::BasicStatCheck * 
dqm_algorithms::BasicStatCheck::clone()
{
  
  return new BasicStatCheck( m_name );
}


dqm_core::Result *
dqm_algorithms::BasicStatCheck::execute(	const std::string & name , 
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
  const double subtractfromxmean = dqm_algorithms::tools::GetFirstFromMap( "SubtractFromXMean", config.getParameters(), 0);
  const double subtractfromymean = dqm_algorithms::tools::GetFirstFromMap( "SubtractFromYMean", config.getParameters(), 0);
  const unsigned int publishType = static_cast<unsigned int>(dqm_algorithms::tools::GetFirstFromMap( "PublishType", config.getParameters(), 3));
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  histogram->GetXaxis()->SetRange(range[0], range[1]);
  histogram->GetYaxis()->SetRange(range[2], range[3]);
  
  std::map<std::string, double> params;
  if (m_name == "Mean") {
    params["XMean"]=histogram->GetMean(1) - subtractfromxmean;	
    params["YMean"]=histogram->GetMean(2) - subtractfromymean;	
    ERS_DEBUG(1,"XMean Value " <<histogram->GetMean(1)<<" YMean Value " <<histogram->GetMean(2));
  } else if (m_name == "RMS" ) {
    params["XRMS"]=histogram->GetRMS(1);	
    params["YRMS"]=histogram->GetRMS(2);	
    ERS_DEBUG(1,"XRMS Value " <<histogram->GetRMS(1)<<" YRMS Value "<<histogram->GetRMS(2));
  }else {
    return new dqm_core::Result();
  }
  
  dqm_core::Result* result;
 
  try {
    result = dqm_algorithms::tools::MakeComparisons(params, config.getGreenThresholds(), config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  if (m_name == "Mean") {
      if (publishType & 0x01) result->tags_["XMean"] = params["XMean"];
      if (publishType & 0x02) result->tags_["YMean"] = params["YMean"];
  } else if (m_name == "RMS") {
      if (publishType & 0x01) result->tags_["XRMS"] = params["XRMS"];
      if (publishType & 0x02) result->tags_["YRMS"] = params["YRMS"];
  }
  
  return result;
}

void
dqm_algorithms::BasicStatCheck::printDescription(std::ostream& out)
{
  if (m_name == "Mean") {
    out<<"CheckHisto_Mean: Checks that X and Y Mean of histo are within specified thresholds\n"<<std::endl;
    out<<"Green/Red Threshold: XMean or AbsXMean: X Mean value to give Green/Red result; AbsXMean checks the absolute value"<<std::endl;
    out<<"Green/Red Threshold: YMean or AbsYMean: Y Mean value to give Green/Red result; AbsYMean checks the absolute value\n"<<std::endl;
  }else if (m_name == "RMS"){
    out<<"CheckHisto_RMS: Checks that X and Y RMS of histo are within specified thresholds\n"<<std::endl;
    out<<"Green/Red Threshold: XRMS: X RMS value to give Green/Red result"<<std::endl;
    out<<"Green/Red Threshold: YRMS: Y RMS value to give Green/Red result\n"<<std::endl;
  }
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: ymin: minimum y range"<<std::endl;
  out<<"Optional Parameter: ymax: maximum y range"<<std::endl;
  out<<"Optional Parameter: SubtractFromXMean: value subtracted from XMean before test is applied: allows using AbsXMean for non-zero expected mean"<<std::endl;
  out<<"Optional Parameter: SubtractFromYMean: value subtracted from YMean before test is applied: allows using AbsYMean for non-zero expected mean"<<std::endl;
  out<<"Optional Parameter: PublishType: 0 = publish no results, 1 = publish X result, 2 = publish Y result, 3 = publish both results (default)."<<std::endl;
}
