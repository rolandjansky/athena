/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicHistoCheck.cxx checks basic histogram quanitites like over/underflow; if filled and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BasicHistoCheckModuleStatus.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::BasicHistoCheckModuleStatus BinsFilled( "ModuleStatus_All_Bins_Filled" );

}


dqm_algorithms::BasicHistoCheckModuleStatus::BasicHistoCheckModuleStatus( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::BasicHistoCheckModuleStatus * 
dqm_algorithms::BasicHistoCheckModuleStatus::clone()
{
  
  return new BasicHistoCheckModuleStatus( m_name );
}


dqm_core::Result *
dqm_algorithms::BasicHistoCheckModuleStatus::execute(	const std::string & name , 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config)
{  
  TH1 * histogram;
  
  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 3 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 3 " );
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
  
  
 
  if ( m_name == "ModuleStatus_All_Bins_Filled" ) {
    unsigned int nbinsX_filled = 0;
    const unsigned int binsx = histogram->GetNbinsX();
    //const unsigned int binsy = histogram->GetNbinsY();
    //const unsigned int binsz = histogram->GetNbinsZ();
    for ( unsigned int i(1); i <= binsx; ++i ) {
      //for ( unsigned int j(1); j <= binsy; ++j ) {
      //for ( unsigned int k(1); k<= binsz; ++k ) {
      double content= histogram -> GetBinContent(i);
      if ( content == 0) {
	// ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has empty bins");
// 	return new dqm_core::Result(dqm_core::Result::Red);
	
	nbinsX_filled++;
      }
	  
    }   

    if(histogram->GetEffectiveEntries() == 0){
      
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" does not have all filled bins");
      return new dqm_core::Result(dqm_core::Result::Disabled);
      
    }
    

    if(nbinsX_filled >0 && nbinsX_filled < binsx){
       ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has empty bins");
       return new dqm_core::Result(dqm_core::Result::Red);
    }
    
    
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has all filled bins");
    return new dqm_core::Result(dqm_core::Result::Green);

  } 

  else {
    return new dqm_core::Result();
  }
  
  
}

void
dqm_algorithms::BasicHistoCheckModuleStatus::printDescription(std::ostream& out)
{
  if ( m_name == "All_Bins_Filled"){
    out<<"All_Bins_Filled: Checks that all bins of histogram are filled\n"<<std::endl;
  } 
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
}

