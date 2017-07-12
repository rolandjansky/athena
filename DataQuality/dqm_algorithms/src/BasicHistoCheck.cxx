/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicHistoCheck.cxx checks basic histogram quanitites like over/underflow; if filled and returns dqm_core::Result
 * \author Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BasicHistoCheck.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <ers/ers.h>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::BasicHistoCheck Filling( "Histogram_Not_Empty" );
  dqm_algorithms::BasicHistoCheck OverFlow( "No_OverFlows" );
  dqm_algorithms::BasicHistoCheck UnderFlow( "No_UnderFlows" );
  dqm_algorithms::BasicHistoCheck BinsFilled( "All_Bins_Filled" );

  dqm_algorithms::BasicHistoCheck Empty( "Histogram_Empty" );
  dqm_algorithms::BasicHistoCheck EffectiveEmpty( "Histogram_Effective_Empty" );
}


dqm_algorithms::BasicHistoCheck::BasicHistoCheck( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::BasicHistoCheck * 
dqm_algorithms::BasicHistoCheck::clone()
{
  
  return new BasicHistoCheck( m_name );
}


dqm_core::Result *
dqm_algorithms::BasicHistoCheck::execute(	const std::string & name , 
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
  
  if (m_name == "Histogram_Not_Empty") {
    if (histogram->GetEntries() != 0) {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Not Empty");
      return new dqm_core::Result(dqm_core::Result::Green);
    }else {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Empty");
      return new dqm_core::Result(dqm_core::Result::Red);
    }
  } else if (m_name == "No_OverFlows" ) {
    const unsigned int binsx = histogram->GetNbinsX()+1;
    const unsigned int binsy = histogram->GetNbinsY()+1;
    const unsigned int binsz = histogram->GetNbinsZ()+1;
    switch (histogram->GetDimension()){
    case 1:
      if (histogram->GetBinContent(binsx) != 0) {
	ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows X");
	return new dqm_core::Result(dqm_core::Result::Red);
      }
      break;
    case 2:
      for (unsigned int i(0); i <= binsx; ++i)
	if (histogram->GetBinContent(i,binsy) != 0) {
	  ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows Y");
	  return new dqm_core::Result(dqm_core::Result::Red);
	}
      for (unsigned int i(0); i <= binsy; ++i)
	if (histogram->GetBinContent(binsx,i) != 0) {
	  ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows X");
	  return new dqm_core::Result(dqm_core::Result::Red);
	}
      break;
    case 3:
      for (unsigned int i(0); i <= binsx; ++i)
	for (unsigned int j(0); j <= binsy; ++j)
	  if (histogram->GetBinContent(i,j,binsz) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows Z");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      for (unsigned int i(0); i <= binsx; ++i)
	for (unsigned int j(0); j <= binsz; ++j)
	  if (histogram->GetBinContent(i,binsy,j) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows Y");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      for (unsigned int i(0); i <= binsz; ++i)
	for (unsigned int j(0); j <= binsy; ++j)
	  if (histogram->GetBinContent(binsx,j,i) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has OverFlows X");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      break;
    default:
      throw dqm_core::BadConfig( ERS_HERE, name, "Something is wrong with the Dimension of the Histogram");
    }
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" does NOT have OverFlows");
    return new dqm_core::Result(dqm_core::Result::Green);
  }else if (m_name == "No_UnderFlows") {
    const unsigned int binsx = histogram->GetNbinsX()+1;
    const unsigned int binsy = histogram->GetNbinsY()+1;
    const unsigned int binsz = histogram->GetNbinsZ()+1;
    switch (histogram->GetDimension()){
    case 1:
      if (histogram->GetBinContent(0) != 0) {
	ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows X");
	return new dqm_core::Result(dqm_core::Result::Red);
      }
      break;
    case 2:
      for (unsigned int i(0); i <= binsx; ++i)
	if (histogram->GetBinContent(i,0) != 0) {
	  ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows Y");
	  return new dqm_core::Result(dqm_core::Result::Red);
	}
      for (unsigned int i(0); i <= binsy; ++i)
	if (histogram->GetBinContent(0,i) != 0) {
	  ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows X");
	  return new dqm_core::Result(dqm_core::Result::Red);
	}
      break;
    case 3:
      for (unsigned int i(0); i <= binsx; ++i)
	for (unsigned int j(0); j <= binsy; ++j)
	  if (histogram->GetBinContent(i,j,0) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows Z");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      for (unsigned int i(0); i <= binsx; ++i)
	for (unsigned int j(0); j <= binsz; ++j)
	  if (histogram->GetBinContent(i,0,j) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows Y");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      for (unsigned int i(0); i <= binsz; ++i)
	for (unsigned int j(0); j <= binsy; ++j)
	  if (histogram->GetBinContent(0,j,i) != 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has UnderFlows X");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
      break;
    default:
      throw dqm_core::BadConfig( ERS_HERE, name, "Something is wrong with the Dimension of the Histogram");
    }
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" does NOT have UnderFlows");
    return new dqm_core::Result(dqm_core::Result::Green);
  } else if ( m_name == "All_Bins_Filled" ) {
    const unsigned int binsx = histogram->GetNbinsX();
    const unsigned int binsy = histogram->GetNbinsY();
    const unsigned int binsz = histogram->GetNbinsZ();
    for ( unsigned int i(1); i <= binsx; ++i ) {
      for ( unsigned int j(1); j <= binsy; ++j ) {
	for ( unsigned int k(1); k<= binsz; ++k ) {
	  double content= histogram -> GetBinContent(i,j);
	  if ( content == 0) {
	    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has empty bins");
	    return new dqm_core::Result(dqm_core::Result::Red);
	  }
	}
      }
    }
    
    ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" has all filled bins");
    return new dqm_core::Result(dqm_core::Result::Green);


  } else if (m_name == "Histogram_Empty") {
    if (histogram->GetEntries() == 0) {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Empty");
      return new dqm_core::Result(dqm_core::Result::Green);
    }else {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Not Empty");
      return new dqm_core::Result(dqm_core::Result::Red);
    }
  } else if (m_name == "Histogram_Effective_Empty") {
    if (histogram->GetEffectiveEntries() == 0) {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Empty");
      return new dqm_core::Result(dqm_core::Result::Green);
    }else {
      ERS_DEBUG(1, "Histogram " <<histogram->GetName()<<" is Not Empty");
      return new dqm_core::Result(dqm_core::Result::Red);
    }

    
  } else {
    return new dqm_core::Result();
  }
  
  
}

void
dqm_algorithms::BasicHistoCheck::printDescription(std::ostream& out)
{
  if ( m_name == "All_Bins_Filled"){
    out<<"All_Bins_Filled: Checks that all bins of histogram are filled\n"<<std::endl;
  } else if ( m_name == "Histogram_Not_Empty"){
    out<<"Histogram_Not_Empty: Checks that histogram is not empty\n"<<std::endl;
  }else if ( m_name == "No_UnderFlows"){
    out<<"No_UnderFlows: Checks that histogram has no Underflows"<<std::endl;
  }else if ( m_name == "No_OverFlows"){
    out<<"No_OverFlows: Checks that histogram has no Overflows"<<std::endl;


  } else if ( m_name == "Histogram_Empty"){
    out<<"Histogram_Empty: Checks that histogram is empty\n"<<std::endl;
  } else if ( m_name == "Histogram_Effective_Empty"){
    out<<"Histogram_Not_Empty:\tChecks that histogram has no effective entries\n\t\t\t(see ROOT doc -> TH1 -> GetEffectiveEntries())\n"<<std::endl;
  }
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
}

