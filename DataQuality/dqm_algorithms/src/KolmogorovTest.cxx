/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest.cxx does KolmogorovTest with different options like Norm and/or MaxDist ; returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/KolmogorovTest.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

using namespace std;


#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::KolmogorovTest kolmo_Prob( "Prob" );
static dqm_algorithms::KolmogorovTest kolmo_MaxDist( "MaxDist" );
static dqm_algorithms::KolmogorovTest kolmo_Norm( "Norm" );
static dqm_algorithms::KolmogorovTest kolmo_NormMaxDist( "MaxDistPlusNorm" );


dqm_algorithms::KolmogorovTest::KolmogorovTest( const std::string & name )
  : m_name ( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "KolmogorovTest_"+ name, this );
}

dqm_algorithms::KolmogorovTest * 
dqm_algorithms::KolmogorovTest::clone()
{
  return new KolmogorovTest( m_name );
}


dqm_core::Result *
dqm_algorithms::KolmogorovTest::execute(	const std::string & name, 
						const TObject & object, 
						const dqm_core::AlgorithmConfig & config )
{
  const TH1 * histogram;
  const TH1 * refhist;
  double gthresho;
  double rthresho;
  
  if(object.IsA()->InheritsFrom( "TH1" )) {
    histogram = static_cast<const TH1*>( &object );
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  
  if (histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  try {
    refhist = dynamic_cast<const TH1*>( config.getReference() );
  }
   catch ( dqm_core::Exception & ex ) {
   throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }

  if (!refhist) {
    throw dqm_core::BadRefHist(ERS_HERE, name, "Reference is not a histogram");
  }
  
  if (histogram->GetDimension() != refhist->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "Dimension" );
  }
  
  if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
    throw dqm_core::BadRefHist( ERS_HERE, name, "number of bins" );
  }
  
  if (histogram->GetDimension()==2 && (m_name=="MaxDist" || m_name=="MaxDistPlustNorm")){
    throw dqm_core::BadConfig( ERS_HERE, name, "MaxDist option cannot be used on 2D histograms" );
  }
  
  
  std::string option;
  std::string thresh;
  if (m_name == "Prob") {
    option="";
    thresh="P";
  }else if (m_name == "MaxDist") {
    option="M";
    thresh="MaxDist";
  }else if (m_name == "Norm") {
    option="N";
    thresh="P";
  }else if (m_name == "MaxDistPlusNorm") {
    option="NM";
    thresh="MaxDist";
  }else {
    throw dqm_core::BadConfig( ERS_HERE, "None", m_name );
  }
  
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresh, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresh, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  
  double value = histogram->KolmogorovTest( refhist, option.c_str());
  
  ERS_DEBUG(1, "Kolmogorov Test with Option " << m_name <<  " is " << value );
  ERS_DEBUG(1, "Green threshold: "<< gthresho << ";  Red threshold: " << rthresho );
  
  dqm_core::Result* result = new dqm_core::Result();
  result->tags_[m_name] = value;
  if (thresh =="P"){ 
    if ( value >= gthresho ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  value > rthresho ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
  }else {
    if ( value <= gthresho ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  value < rthresho ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
  }
  
  return result;
}
void
dqm_algorithms::KolmogorovTest::printDescription(std::ostream& out)
{
  std::string thresh;
  if (m_name == "Prob") {
    m_option="";
    thresh="P";
  }else if (m_name == "MaxDist") {
    m_option="M";
    thresh="MaxDist";
  }else if (m_name == "Norm") {
    m_option="N";
    thresh="P";
  }else if (m_name == "MaxDistPlusNorm") {
    m_option="NM";
    thresh="MaxDist";
  }
  
  if (m_name == "Norm" || m_name == "Prob") { 
    out<<"KolmogorovTest_"+ m_name+": Give back probability after performing KolmogorovTest on histogram against referece histogram with "+m_option+" option\n"<<std::endl;
  } else {
    out<<"KolmogorovTest_"+ m_name+": Give back MaxDist(for 1D histograms only) after performing KolmogorovTest on histogram against referece histogram with "+m_option+" option\n"<<std::endl;
  }
  out<<"Mandatory Green/Red Threshold: "+thresh+" : Probability to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
  
}

