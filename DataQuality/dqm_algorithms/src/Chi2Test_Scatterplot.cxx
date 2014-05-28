/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_Scatterplot.cxx computes the chisq/ndf for a scatterplot; returns dqm_core::Result
 * \author Ian Moult
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/Chi2Test_Scatterplot.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TObjArray.h>
#include <TMath.h>
#include <math.h>
#include <string>

#include <TClass.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <ers/ers.h>

using namespace std;


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::Chi2Test_Scatterplot myInstance;

dqm_algorithms::Chi2Test_Scatterplot::Chi2Test_Scatterplot()
 {
  dqm_core::AlgorithmManager::instance().registerAlgorithm("Chi2Test_Scatterplot", this );
}


dqm_algorithms::Chi2Test_Scatterplot * 
dqm_algorithms::Chi2Test_Scatterplot::clone()
{
  return new Chi2Test_Scatterplot();
}


dqm_core::Result *
dqm_algorithms::Chi2Test_Scatterplot::execute(	const std::string & name , 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
  const TH1 * inputgraph2;
  
  if(object.IsA()->InheritsFrom( "TH1" )) {
    inputgraph2 = static_cast<const TH1*>( &object );
    
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  //now, cast inputgraph2 to input graph and remove its constness
  TH1* inputgraph=const_cast<TH1*>(inputgraph2);
  
  
  
  double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), 1 );
  
  if (inputgraph->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = inputgraph->GetEntries();
    return result;
  }
  
 
 
  
  TH1 * refhist;
  double gthresho;
  double rthresho;
  std::string option;
  
  std::string thresholdname="Chi2_per_NDF";
 
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );

  }

  try {
    refhist = static_cast<TH1 *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
   throw dqm_core::BadRefHist(ERS_HERE,name," Could not retreive reference");
  }

  if (inputgraph->GetDimension() != refhist->GetDimension() ) {
    throw dqm_core::BadRefHist( ERS_HERE, "Dimension", name );
  }
  
   
  
  if ((inputgraph->GetNbinsX() != refhist->GetNbinsX()) || (inputgraph->GetNbinsY() != refhist->GetNbinsY())) {
    throw dqm_core::BadRefHist( ERS_HERE, "number of bins", name );
  } 
  
  int n=0;
  double chisq=0;
  double errsquared;
  double inputerr;
  double referr;
  double val;
  double refval;
  
 
 //read in the range of bin values to use
 vector<int> range;
 try{
 range=dqm_algorithms::tools::GetBinRange(inputgraph,config.getParameters());
 }
 catch( dqm_core::Exception & ex ) {
     throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
   }

//now compute the chisq/ndf
  
  int count_ndf=0;
  
  for(n=range[0];n<(range[1]+1);n++)
  { 	inputerr=inputgraph->GetBinError(n);
	referr=refhist->GetBinError(n);
	errsquared= referr*referr+inputerr*inputerr;
	val=inputgraph->GetBinContent(n);
	refval=refhist->GetBinContent(n);
	if (referr >0.00001 && inputerr >0.00001)
	{chisq=chisq+((val-refval)*(val-refval))/errsquared;
	 count_ndf++;
	 }
	else
	{//out<<"One of the errors is too small???? "<<n<<std::endl;
	}
	
	}
  
  
  const double value = (count_ndf > 1) ? (chisq / (count_ndf - 1)) : 0; // avoid to divide by zero (should never happen)
  
  dqm_core::Result* result = new dqm_core::Result();
  result->tags_[thresholdname] = value;
  

//check the thresholds

    if ( value <= gthresho ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  value < rthresho ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
    
  ERS_DEBUG(2,"Result: "<<*result);
 
  return result; 

}
void
dqm_algorithms::Chi2Test_Scatterplot::printDescription(std::ostream& out)
{
  
  out<<"Chi2Test_Scatterplot performs chisq/ndf test on a scatterplot and returns a dqm_result"<<std::endl;

  
  out<<"Mandatory Green/Red Threshold: Chi2_per_NDF to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
  
}

