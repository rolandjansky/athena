/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test.cxx does Chi2Test and compares either chi2/ndf or the probability; returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/Chi2Test.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TEfficiency.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

using namespace std;


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::Chi2Test chi2_ndf( "Chi2_per_NDF" );
static dqm_algorithms::Chi2Test chi2_p( "Prob" );
static dqm_algorithms::Chi2Test chi2_uw("ProbUW" );
static dqm_algorithms::Chi2Test chi2_ww("ProbWW" );
static dqm_algorithms::Chi2Test chi2_chi2("Chi2");

dqm_algorithms::Chi2Test::Chi2Test( const std::string & name )
  : m_name ( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("Chi2Test_"+ name, this );
}

dqm_algorithms::Chi2Test * 
dqm_algorithms::Chi2Test::clone()
{
  return new Chi2Test( m_name );
}


dqm_core::Result *
dqm_algorithms::Chi2Test::execute(	const std::string & name , 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
  const TH1* histogram = 0;
  const TEfficiency* efficiency = 0;
  TH1* passed_histogram = 0;
  TH1* total_histogram = 0; 

  if(object.IsA()->InheritsFrom( "TH1" )) {
    histogram = static_cast<const TH1*>( &object );
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else if(object.IsA()->InheritsFrom( "TEfficiency" )) { 
    // get the histograms from TEfficiency object to perform Chi2Test
    efficiency = static_cast<const TEfficiency*>( &object);
     if (efficiency->GetDimension() > 2 ){
        throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }

    passed_histogram = efficiency->GetCopyPassedHisto();
    total_histogram = efficiency->GetCopyTotalHisto();
    passed_histogram->Divide(total_histogram);

  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1 or TEfficiency");
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  double current_stat = 0; 

  if(object.IsA()->InheritsFrom( "TH1" )) { 
    current_stat = histogram->GetEntries();
  } else if(object.IsA()->InheritsFrom( "TEfficiency" )){
    current_stat = total_histogram->GetEntries();
  }  

  if(current_stat < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = current_stat;
    return result;
  }
     
  TH1 * refhist = 0;
  TH1 * ref_total_hist;
  TEfficiency * refeff;
  double gthresho;
  double rthresho;
  std::string option;
  std::string thresholdname="P";
  if (m_name == "Chi2_per_NDF") {
    option="Chi2/ndfUU";
    thresholdname="Chi2_per_NDF";
  }else if (m_name == "Prob" ) {
    option="UU";
  }else if (m_name == "ProbUW") {
    option="UW";
  }else if (m_name == "ProbWW") {
    option="WW";
  } else if (m_name == "Chi2") {
    option="CHI2UU";
    thresholdname="Chi2";
  } else {
    throw dqm_core::BadConfig( ERS_HERE, "None", m_name );
  }
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );

  }

  if(object.IsA()->InheritsFrom( "TH1" )) {
     try {
        refhist = dynamic_cast<TH1 *>( config.getReference() );
     }
     catch ( dqm_core::Exception & ex ) {
        throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
     }
  } else if(object.IsA()->InheritsFrom( "TEfficiency" )){
     try {
        refeff = dynamic_cast<TEfficiency *>( config.getReference() );
     }
     catch ( dqm_core::Exception & ex ) {
        throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
     }

     refhist = refeff->GetCopyPassedHisto();
     ref_total_hist = refeff->GetCopyTotalHisto();
     refhist->Divide(ref_total_hist);  
  }  

  if (!refhist) { throw dqm_core::BadRefHist(ERS_HERE,name,"Bad reference type"); }
 
  double value = 0;
  if(object.IsA()->InheritsFrom( "TH1" )) {

    if (histogram->GetDimension() != refhist->GetDimension() ) {
       throw dqm_core::BadRefHist( ERS_HERE, "Dimension", name );
    }
  
    if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
       throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
    }
  
    value = histogram->Chi2Test( refhist, option.c_str() );

  } else if(object.IsA()->InheritsFrom( "TEfficiency" )){
    
    if (passed_histogram->GetDimension() != refhist->GetDimension() ) {
       throw dqm_core::BadRefHist( ERS_HERE, "Dimension", name );
    } 

    if ((passed_histogram->GetNbinsX() != refhist->GetNbinsX()) || (passed_histogram->GetNbinsY() != refhist->GetNbinsY())) {
       throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
    }
 
    value = passed_histogram->Chi2Test( refhist, option.c_str() );
  }

  ERS_DEBUG(1,"Green threshold: "<< gthresho << ";  Red threshold: " << rthresho );    
  ERS_DEBUG(1,"Chi2 Test with Option " << option <<  " is " << value );

  dqm_core::Result* result = new dqm_core::Result();
  result->tags_[thresholdname] = value;
  
  if (thresholdname == "P") {//Checking against a probability
    if ( value >= gthresho ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  value > rthresho ) {
      result->status_ = dqm_core::Result::Yellow;
    } else { 
      result->status_ = dqm_core::Result::Red;
    }
  } else {//checking against Chi2 value
    if ( value <= gthresho ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  value < rthresho ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
    
  }
 
  ERS_DEBUG(2,"Result: "<<*result);
  return result; 
  
}
void
dqm_algorithms::Chi2Test::printDescription(std::ostream& out)
{
  m_option="UU";
  std::string thresholdname = "P";
  if (m_name == "Chi2_per_NDF") {
    m_option="Chi2/ndf";
    thresholdname="Chi2_per_NDF";
  }else if (m_name == "Prob") {
    m_option="UU";
  }else if (m_name == "ProbUW") {
    m_option="UW";
  }else if (m_name == "ProbWW") {
    m_option="WW";
  }else if (m_name == "Chi2") {
    m_option="CHI2";
    thresholdname="Chi2";
  }

  out<<"Chi2Test_"+ m_name+": Gives back "+thresholdname+" after performing Chi2 test on histogram against referece histogram with option: "<<m_option<<" (see TH1::GetChi2Test)"<<std::endl;

  
  out<<"Mandatory Green/Red Threshold: "+ thresholdname+" : "+thresholdname+" to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
  
}

