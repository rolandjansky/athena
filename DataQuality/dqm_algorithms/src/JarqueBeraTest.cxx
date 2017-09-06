/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file JarqueBeraTest.cxx calculates JB and compares either the value or the probability; returns dqm_core::Result
 * \author andrea.dotti@cern.ch
 */

#include <dqm_core/AlgorithmManager.h>
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/JarqueBeraTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TClass.h>
#include <TMath.h>

static dqm_algorithms::JarqueBeraTest jb("JB");
static dqm_algorithms::JarqueBeraTest jb_prob("Prob");

dqm_algorithms::JarqueBeraTest::JarqueBeraTest( const std::string & name ) : m_name(name)
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("JarqueBeraTest_"+ name, this );
}


dqm_algorithms::JarqueBeraTest*
dqm_algorithms::JarqueBeraTest::clone()
{
  return new JarqueBeraTest(m_name);
}

dqm_core::Result *
dqm_algorithms::JarqueBeraTest::execute(	const std::string & name , 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
  const TH1 * histogram;  
  if(object.IsA()->InheritsFrom( "TH1" )) {
    histogram = static_cast<const TH1*>( &object );
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  ERS_DEBUG(2,"Minimum statistics required:"<<minstat);
  if (histogram->GetEffectiveEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEffectiveEntries"] = histogram->GetEffectiveEntries();
    return result;
  }
  double gthresho,rthresho;
  std::string thresholdname="JB";
  if ( m_name == "Prob") thresholdname = "P";
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  ERS_DEBUG(2,"Green Threshold: "<<gthresho<<" Red: "<<rthresho);
  
  // N.B. Root's return the excess kurtosis, i.e. gamma_2-3
  // The excess kurtosis is defined scuh as K(N(0,1))=0
  double k = histogram->GetKurtosis();
  double s = histogram->GetSkewness();
  double n = histogram->GetEffectiveEntries();
  double jb = n/6*( s*s + (k*k)/4 ); //Calculate Jarque Bera
  //The statistic JB has an asymptotic chi-square distribution with two degrees of freedom 
  // and can be used to test the null hypothesis that the data are from a normal distribution. 
  double prob = TMath::Prob( jb , 2 );
  ERS_DEBUG(2," K="<<k<<" S="<<s<<" n="<<n<<" jb="<<jb<<" prob="<<prob);

  //Check against thresholds
  double testValue;
  if ( m_name == "Prob" ) testValue = prob; //Use probability
  else testValue = jb; // Use the JB value for comparison

  dqm_core::Result* result = new dqm_core::Result();
  result->tags_["JB"] = jb;
  result->tags_["Prob"]=prob;
  if ( TMath::IsNaN( testValue ) ) //This may happen if RMS==0 Kurtosis and Skewness are inf
    {
      result->status_=dqm_core::Result::Undefined;
      return result;
    }
  if ( m_name=="Prob" )
    {
      if ( testValue >= gthresho ) {
	result->status_ = dqm_core::Result::Green;
      } else if (  testValue > rthresho ) {
	result->status_ = dqm_core::Result::Yellow;
      } else {
	result->status_ = dqm_core::Result::Red;
      }
    }
  else
    {
      if ( testValue <= gthresho ) {
	result->status_ = dqm_core::Result::Green;
      } else if (  testValue < rthresho ) {
	result->status_ = dqm_core::Result::Yellow;
      } else {
	result->status_ = dqm_core::Result::Red;
      }
    }
  return result;
}

void
dqm_algorithms::JarqueBeraTest::printDescription(std::ostream& out)
{
  out<<"JarqueBeraTest"+m_name+" gives back the ";
  if ( m_name == "Prob") out<<" probability that the input histogram follows the normal distribution\n"<<std::endl;
  else out<<" Jarque-Bera value of the input histogram\n"<<std::endl;
  out<<"Mandatory Green/Red Threshold: ";
  if ( m_name == "Prob") out<<" P : Probability";
  else out<<" JB : Jarque-Bera value";
  out<<" to give Green/Red result.\n"<<std::endl;
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
}
