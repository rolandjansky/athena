/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file ReferenceMasking.cxx checks an histogram with a specified algorithm, after summing to input histograms the reference histogram
 *  \author andrea.dotti@cern.ch
 */

#include <dqm_algorithms/ReferenceMasking.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TClass.h>
#include <TObjArray.h>
#include <dqm_core/AlgorithmManager.h>

namespace {
  static dqm_algorithms::ReferenceMasking Bins_GreaterThan_Threshold("Bins_GreaterThan_Threshold");
  static dqm_algorithms::ReferenceMasking BinsDiffFromAvg("Bins_Diff_FromAvg");
}

dqm_algorithms::ReferenceMasking::ReferenceMasking(const std::string& name ) : m_name(name)
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm("ReferenceMasking_"+name,this);
}

dqm_algorithms::ReferenceMasking* 
dqm_algorithms::ReferenceMasking::clone()
{
  return new ReferenceMasking(m_name);
}

dqm_core::Result* 
dqm_algorithms::ReferenceMasking::execute(const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
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
  
  double coeff = dqm_algorithms::tools::GetFirstFromMap("C",config.getParameters(), 0);

  TH1* refhist;
  try {
    refhist = static_cast<TH1 *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }
  if (histogram->GetDimension() != refhist->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, "Dimension", name );
  }
  if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY()) || refhist->GetNbinsZ() != histogram->GetNbinsZ() ) {
    throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
  }
  ERS_DEBUG(2,"Masking bins according to reference. Will set content to"<<coeff);
  
  for ( Int_t binx=histogram->GetXaxis()->GetFirst() ; binx<=histogram->GetXaxis()->GetLast();++binx)
    {
      for ( Int_t biny=histogram->GetYaxis()->GetFirst() ; biny<=histogram->GetYaxis()->GetLast();++biny)
	{
	  for ( Int_t binz=histogram->GetZaxis()->GetFirst() ; binz<=histogram->GetZaxis()->GetLast();++binz)
	    {
	      if ( refhist->GetBinContent(binx,biny,binz) != 0 )
		{
		  ERS_DEBUG(3,"Found a bin in reference with entries !=0, set content of bin ("<<binx<<","<<biny<<","<<binz<<") with "<<coeff);
		  histogram->SetBinContent( binx , biny, binz , coeff );
		}
	    }
	}
    }


  
  //Now prepare to run the real algorithm...
  ERS_DEBUG(2,"Running algorithm: "<<m_name);
  dqm_core::Algorithm* subalgorithm;
  try {
    subalgorithm = dqm_core::AlgorithmManager::instance().getAlgorithm( m_name );
  }
  catch ( dqm_core::AlgorithmNotFound& ex )
    {
      ERS_DEBUG(2,"Cannot find algorithm:"+m_name);
      throw dqm_core::BadConfig(ERS_HERE,name,"Cannot Find sub-algorithm:"+m_name);
    }
  dqm_core::Result* result = subalgorithm->execute( name , *histogram ,  config);
  ERS_DEBUG(2,"Sub algorithm returns:"<<*result);
  //Add modified histogram to result
  TObject* robject = result->getObject();
  if ( !robject ) //No object defined, add this
    {
      ERS_DEBUG(2,"Adding modified histogram in result");
      result->object_.reset(histogram);
    }
  else //Transform the object_ in TObjArray (if needed) and add this result
    {
      ERS_DEBUG(2,"Result already have an associated TObject, appending modified histogram");
      if ( robject->IsA()->InheritsFrom("TObjArray") ) //It is already an array add it...
	{	  
	  static_cast<TObjArray*>(robject)->Add( histogram );
	  //Check in again
	  result->object_.reset( robject );
	}
      else
	{
	  TObjArray* array = new TObjArray( 2 );
	  array->AddAt( robject , 0 );
	  array->AddAt( histogram , 1 );
	  //Check in again
	  result->object_.reset( array );
	}
      ERS_DEBUG(2,"Result now have a TObjArray of size:"<<static_cast<TObjArray*>(result->getObject())->GetEntries());
    }
  return result;
}


void dqm_algorithms::ReferenceMasking::printDescription(std::ostream& out) {
  out<<"ReferenceMasking_"+m_name+" : Performst the "+m_name+" algorithm after using the reference histogram to mask bins. I.e. Perform TH1::SetBinContent(bin,c) for all bins of reference with entries!=0. c (default 0) can be set via configuration. Adds to the output TObject list the modified input histogram."<<std::endl;
  out<<"Optional Parameter : MinStat : Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter : C : The value of the content of the masked bin. Default c=0"<<std::endl;

}
