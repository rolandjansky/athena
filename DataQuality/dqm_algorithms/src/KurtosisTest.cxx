/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest.cxx checks excess kurtosis wrt to a threshold value and returns dqm_core::Result
 * \author andrea.dotti@cern.ch
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/KurtosisTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>
#include <TMath.h>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::KurtosisTest kurtosis_GreaterThan( "GreaterThan" );
  dqm_algorithms::KurtosisTest kurtosis_LessThan( "LessThan" );
  dqm_algorithms::KurtosisTest kurtosis_GreaterThanAbs( "GreaterThanAbs" );// Ok, for Kurtosis this is not so much sense (K<0 and K>0 indicate really different things), but just for completeness
  dqm_algorithms::KurtosisTest kurtosis_LessThanAbs( "LessThanAbs" ); // Kurtosis( normal(0,1) ) =0
}

//NB. Following ROOT definition This is the excess kurtosis, i.e. fourth standarized momentum -3. K(N(0,1))=0
dqm_algorithms::KurtosisTest::KurtosisTest( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("KurtosisTest_"+name,this);
}

dqm_algorithms::KurtosisTest * 
dqm_algorithms::KurtosisTest::clone()
{
    return new KurtosisTest( m_name );
}


dqm_core::Result *
dqm_algorithms::KurtosisTest::execute(	const std::string &  name, 
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
  
  int axis = (int) dqm_algorithms::tools::GetFirstFromMap( "Axis", config.getParameters(), 1);

  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "K", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "K", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  ERS_DEBUG(1,"Axis for kurtosis calculation:"<<axis);
  
  Double_t kurtosis = histogram->GetKurtosis( axis );
  Double_t kurtosis_e = histogram->GetKurtosis( axis + 10 );

  dqm_core::Result* result = new dqm_core::Result();
  ERS_DEBUG(1,"Kurtosis = "<<kurtosis<<" +- "<<kurtosis_e);
  result->tags_["Kurtosis"] = kurtosis;
  result->tags_["ApproxStandardError"]=kurtosis_e;
  
  if ( CompareKurtosisTest( m_name , kurtosis ,gthreshold)  ) {
    result->status_ = dqm_core::Result::Green;
  } else if ( CompareKurtosisTest( m_name, kurtosis, rthreshold) ) {
    result->status_ = dqm_core::Result::Yellow;
  } else {
    result->status_ = dqm_core::Result::Red;
  }
  
  return result;
  
}

bool
dqm_algorithms::KurtosisTest::CompareKurtosisTest(const std::string & type, double value, double threshold) {
  
  if (type == "GreaterThan") return (value > threshold);
  if (type == "LessThan") return (value < threshold);
  if (type == "GreaterThanAbs") return ( TMath::Abs(value) > threshold );
  if (type == "LessThanAbs") return ( TMath::Abs(value) < threshold);
  return 0;
}


void
dqm_algorithms::KurtosisTest::printDescription(std::ostream& out)
{
  out<<"KurtosisTest_"+m_name+" : Checks if the Excess Kurtosis of the distribution is "+m_name+" threshold value\n"<<std::endl;
  
  out<<"Mandatory Green/Red Threshold: K: Value of Kurtosis"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: Axis: Axis along which compute Kurtosis: 1=X, 2=Y, 3=Z"<<std::endl;
  
}

