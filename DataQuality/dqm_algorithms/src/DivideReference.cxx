/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference.cxx checks an histogram with a specified algorithm, after summing to input histograms the reference histogram
 *  \author andrea.dotti@cern.ch
 */

#include <dqm_algorithms/DivideReference.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_algorithms/tools/SimpleAlgorithmConfig.h>
#include <TH1.h>
#include <TClass.h>
#include <TObjArray.h>
#include <dqm_core/AlgorithmManager.h>

#include <iostream>
using namespace std;

namespace {
  static dqm_algorithms::DivideReference d1("All_Bins_Filled");
  static dqm_algorithms::DivideReference d2("Bins_Diff_FromAvg");
  static dqm_algorithms::DivideReference d3("Bins_GreaterThan_Threshold");
  static dqm_algorithms::DivideReference d4("Bins_GreaterThanEqual_Threshold");
  static dqm_algorithms::DivideReference d5("Bins_LessThan_Threshold");
  static dqm_algorithms::DivideReference d6("Bins_LessThanEqual_Threshold");
  static dqm_algorithms::DivideReference d7("Bins_Equal_Threshold");
  static dqm_algorithms::DivideReference d8("Bins_NotEqual_Threshold");
  static dqm_algorithms::DivideReference d9("BinContentComp");
}

dqm_algorithms::DivideReference::DivideReference(const std::string& name ) : name_(name)
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("DivideReference_"+name,this);
}

dqm_algorithms::DivideReference* 
dqm_algorithms::DivideReference::clone()
{
  return new DivideReference(name_);
}

dqm_core::Result* 
dqm_algorithms::DivideReference::execute(const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
{
  TH1 * histogram;

  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)(object.Clone());
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);

  if (histogram->GetEffectiveEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEffectiveEntries"] = histogram->GetEffectiveEntries();
    delete histogram;
    return result;
  }
  

  TObject* ro = config.getReference();
  TObject* firstReference=0;
  TObject* secondReference=0;
  try { 
    dqm_algorithms::tools::handleReference( *ro , firstReference , secondReference );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  //Check the reference
  TH1* refhist = dynamic_cast<TH1*>(firstReference);
  if ( refhist==0 )
    {
      throw dqm_core::BadRefHist( ERS_HERE, "Dimension", name );
    }
  if ((histogram->GetDimension() != refhist->GetDimension()) ||
      (histogram->GetNbinsX() != refhist->GetNbinsX()) || 
      (histogram->GetNbinsY() != refhist->GetNbinsY()) || 
      refhist->GetNbinsZ() != histogram->GetNbinsZ() ) 
    {
      throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
    }
  //Configuration done, prepare input histogram
  histogram->Divide(refhist);
  //Now prepare to run the real algorithm...
  ERS_DEBUG(2,"Running algorithm: "<<name_);
  dqm_core::Algorithm* subalgorithm;
  try {
    subalgorithm = dqm_core::AlgorithmManager::instance().getAlgorithm( name_ );
  }
  catch ( dqm_core::AlgorithmNotFound& ex )
    {
      ERS_DEBUG(2,"Cannot find algorithm:"+name_);
      throw dqm_core::BadConfig(ERS_HERE,name,"Cannot Find sub-algorithm:"+name_);
    }
  //Copy configuration and update reference
  dqm_algorithms::tools::SimpleAlgorithmConfig newConf( config );
  newConf.setReference( secondReference );
  dqm_core::Result* result = subalgorithm->execute( name , *histogram ,  newConf);
  ERS_DEBUG(2,"Sub algorithm returns:"<<*result);
  //Add modified histogram to result
  TObject* robject = result->getObject();
  if ( !robject ) //No object defined, add the modified histogram
    {
      ERS_DEBUG(2,"Adding modified histogram in result");
      result->object_.reset(histogram);
    }
  else //Transform the object_ in TObjArray (if needed) and add this result
    {
      ERS_DEBUG(2,"Result already have an associated TObject, appending modified histogram");
      if ( robject->IsA()->InheritsFrom("TCollection") ) //It is already an array add it...
	{	  
	  static_cast<TCollection*>(robject)->Add( histogram );
	}
      else
	{
	  TObjArray* array = new TObjArray( 2 );
	  array->AddAt( robject->Clone() , 0 );
	  array->AddAt( histogram , 1 );
	  //Check in again
	  result->object_.reset( array );
	}
      ERS_DEBUG(2,"Result now have a TObjArray of size:"<<static_cast<TObjArray*>(result->getObject())->GetEntries());
    }
  return result;
}


void dqm_algorithms::DivideReference::printDescription(std::ostream& out) {
  out<<"DivideReference_"+name_+" : Performst the "+name_+" algorithm after dividing the input histogram by the reference. I.e. performing: histogram /= Reference (see TH1::Divide). Adds to the output TObject list the modified input histogram."<<std::endl;
  out<<"Optional Parameter : MinStat : Minimum histogram statistics needed to perform Algorithm"<<std::endl;


}
