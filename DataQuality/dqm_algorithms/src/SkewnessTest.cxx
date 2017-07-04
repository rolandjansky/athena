/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest.cxx checks skewness wrt to a threshold value and returns dqm_core::Result
 * \author andrea.dotti@cern.ch
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/SkewnessTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>
#include <TMath.h>


#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::SkewnessTest skweness_GreaterThan( "GreaterThan" );
  dqm_algorithms::SkewnessTest skewness_LessThan( "LessThan" );
  dqm_algorithms::SkewnessTest skewness_GreaterThanAbs( "GreaterThanAbs" );
  dqm_algorithms::SkewnessTest skewness_LessThanAbs( "LessThanAbs" ); 
}


dqm_algorithms::SkewnessTest::SkewnessTest( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("SkewnessTest_"+name,this);
}

dqm_algorithms::SkewnessTest * 
dqm_algorithms::SkewnessTest::clone()
{
    return new SkewnessTest( m_name );
}


dqm_core::Result *
dqm_algorithms::SkewnessTest::execute(	const std::string &  name, 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
{  
  TH1 * histogram;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  
  if (histogram->GetEffectiveEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEffectiveEntries"] = histogram->GetEffectiveEntries();
    return result;
  }
  
  double gthreshold;
  double rthreshold;
  
  const int axis = (int) dqm_algorithms::tools::GetFirstFromMap( "Axis", config.getParameters(), 1);

  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "S", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "S", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  ERS_DEBUG(1,"Axis for skewness calculation:"<<axis);
  
  Double_t skewness = histogram->GetSkewness( axis );
  Double_t skewness_e = histogram->GetSkewness( axis + 10 );

  dqm_core::Result* result = new dqm_core::Result();
  ERS_DEBUG(1,"Skewness = "<<skewness<<" +- "<<skewness_e);
  result->tags_["Skewness"] = skewness;
  result->tags_["ApproxStandardError"]=skewness_e;

  if ( CompareSkewnessTest( m_name , skewness ,gthreshold)  ) {
    result->status_ = dqm_core::Result::Green;
  } else if ( CompareSkewnessTest( m_name, skewness, rthreshold) ) {
    result->status_ = dqm_core::Result::Yellow;
  } else {
    result->status_ = dqm_core::Result::Red;
  }
  
  return result;
  
}

bool
dqm_algorithms::SkewnessTest::CompareSkewnessTest(const std::string & type, double value, double threshold) {
  
  if (type == "GreaterThan") return (value > threshold);
  if (type == "LessThan") return (value < threshold);
  if (type == "GreaterThanAbs") return ( TMath::Abs(value) > threshold );
  if (type == "LessThanAbs") return ( TMath::Abs(value) < threshold);
  return 0;
}


void
dqm_algorithms::SkewnessTest::printDescription(std::ostream& out)
{
  out<<"SkewnessTest_"+m_name+" : Checks if the Skewness of the distribution is "+m_name+" threshold value\n"<<std::endl;
  
  out<<"Mandatory Green/Red Threshold: S: Value of Skewness"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: Axis: Axis along which compute Skewness: 1=X, 2=Y, 3=Z"<<std::endl;
  
}

