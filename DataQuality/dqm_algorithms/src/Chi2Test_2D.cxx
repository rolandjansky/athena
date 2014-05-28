/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_2D.cxx does Chi2Test on a 2D histogram, and also returns the positions of the highest outlying bins; returns dqm_core::Result
 //result is based on the number of outlying bins above NSigma, not on the chisq/ndf value
 \Written by Ian Moult
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/Chi2Test_2D.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TF1.h>
#include <TH2.h>
#include <TF2.h>
#include <TClass.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <ers/ers.h>
#include <vector>
#include <cctype>
#include <map>
using namespace std;


#include <dqm_core/AlgorithmManager.h>
static dqm_algorithms::Chi2Test_2D myInstance;

dqm_algorithms::Chi2Test_2D::Chi2Test_2D()
 {
  dqm_core::AlgorithmManager::instance().registerAlgorithm("Chi2Test_2D", this );
}

dqm_algorithms::Chi2Test_2D * 
dqm_algorithms::Chi2Test_2D::clone()
{
  return new Chi2Test_2D();
}


dqm_core::Result *
dqm_algorithms::Chi2Test_2D::execute(	const std::string & name , 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{
const TH2 * inputgraph_original;
  
  if(object.IsA()->InheritsFrom( "TH1" )) {
    inputgraph_original = static_cast<const TH2*>( &object );
    
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }

//now, cast inputgraph_original to a non const object
 
 TH2* inputgraph = const_cast<TH2*>(inputgraph_original); 
 
 
 //Make sure the input histogram has enough statistics 
  double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), 1 );
  
  if (inputgraph->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = inputgraph->GetEntries();
    return result;
  }
  
  
  TH2 * refhist;
  double gthresho;
  double rthresho;
  std::string option;
  
  
  //read in the threshold values
  std::string thresholdname="NBins";
 
  try {
    gthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getGreenThresholds() );
    rthresho = dqm_algorithms::tools::GetFromMap( thresholdname, config.getRedThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );

  }

//try and get the reference, and make sure that it is good
  try {
    refhist = static_cast<TH2 *>( config.getReference() );
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
  
  


//check if the value "normalize" is 1. If it is, must scale the histogram. Also, make sure that there is an
//error associated with the bin, because you should only be calling normalize on a histogram that has a #of jets in a bin
//that error should be sqrt(n).
// can call sumw2() before scaling to make sure that this is the case, and then can use the rest of the algorithm as normal,
//because the correct errors should all be set
int normalize = dqm_algorithms::tools::GetFirstFromMap( "normalize", config.getParameters(), 1 );


if(normalize==1)
{double ref_entries=refhist->GetEntries();
 double input_entries=inputgraph->GetEntries();
 //call sumw2() to make sure that you have the errors as sqrt(n)
 inputgraph->Sumw2();
 refhist->Sumw2();
//now, rescale the reference histogram to the inputgraph histogram. The errors should scale properly, now that I have called Sumw2()
 refhist->Scale(input_entries/ref_entries);
}
 

//read in the values for NSigma, and the number of bins to print out at the end, and the value for MaxSigma
double NSigma = dqm_algorithms::tools::GetFirstFromMap( "NSigma", config.getParameters(), 1 ); 
int Num_to_print = dqm_algorithms::tools::GetFirstFromMap( "Num_to_print", config.getParameters(), 1 ); 
double MaxSigma = dqm_algorithms::tools::GetFirstFromMap( "MaxSigma", config.getParameters(), 1 ); 

//read in the values for the maximum and minimum x bin
vector<int> range;
try{
range=dqm_algorithms::tools::GetBinRange(inputgraph,config.getParameters());
}
catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
 
  
  int i=0;
  int j=0;
  double chisq=0;
  double errsquared;
  double inputerr;
  double referr;
  double val;
  double refval;
  double partsum;
  int count_ndf=0;
  
  //define a vector to store the chisq value at the highest ranking bins,which can then be sorted
  // so that you can find out what the location of the high chisq values are after you have sorted them
  //define a map, with the chisq partsum value as the key, and the global bin number as the element.
  vector<double> ChisqValues;
  map<double,int> mymap;
  
  for(i=range[0];i<(range[1]+1);i++)
  { 	
  	for(j=range[2];j<(range[3]+1);j++)
	{
		val=inputgraph->GetBinContent(i,j);
		refval=refhist->GetBinContent(i,j);
		inputerr=inputgraph->GetBinError(i,j);
		referr=refhist->GetBinError(i,j);
		errsquared=referr*referr+inputerr*inputerr;
		if(errsquared > 0.000001)
		{
			partsum=((val-refval)*(val-refval))/errsquared;
		}
		else
		{partsum=-1;
		}
		
		if (partsum != -1)
		{	chisq=chisq+partsum;
	 		count_ndf++;
	 
	 		if(partsum>=NSigma*NSigma)
	 		{	ChisqValues.push_back(sqrt(partsum));
	  		 	mymap.insert(pair<double,int>(sqrt(partsum),inputgraph->GetBin(i,j)));
	 		}
	 
	        }
	
	
	}
	
  }
  
  double ndf=count_ndf-1;
  double value=chisq/ndf;
  dqm_core::Result* result = new dqm_core::Result();
  //write out this chisq/ndf value to the website
  result->tags_["Chisq_per_NDF"]=value;
  
 
 
//sort the chisq values for the points that had more than N sigma
 std::sort(ChisqValues.begin(),ChisqValues.end());
  
//Output the top Num_to_print values for chisquares at individual points 
//define an iterator
 vector<double>::iterator p;
 p=ChisqValues.end();

//since the iterator points to one past the end, must decrement before we use it.
 p--;

//define a second iterator for the map, do not decrement it, as it will be assigned to a point in the map before use
 map<double,int>::iterator p2;


char ctag[256];
int k=0;
int global_bin=0;
int xbinnumber;
int ybinnumber;
int zbinnumber;
int& xBin = xbinnumber;
int& yBin = ybinnumber;
int& zBin = zbinnumber;
int globalbinint=0;
double eta;
double phi;

//declare a variable that will be set to true if the value MaxSigma is exceeded for any bin. This will automatically flag the histogram red
int veto=0;


//if mymap has no elements, then output that there were no elements above, and skip the next section
if(mymap.size()==0)
{result->tags_["No flagged bins were found"]=1.0;
}


while(mymap.size()>0)
{
  
//find the value of eta/phi that goes along with the previous chisq value
//first find the global bin number from the map

  p2=mymap.find(*p);
  if(p2 !=mymap.end())
  {	global_bin=p2->second;
   	globalbinint= (int) global_bin;
   	inputgraph->GetBinXYZ(globalbinint,xBin,yBin,zBin);
   	eta=inputgraph->GetXaxis()->GetBinCenter(xBin);
   	phi=inputgraph->GetYaxis()->GetBinCenter(yBin);
   
   	//write the coordinates and Sigma value to the website
	sprintf(ctag," (eta,phi)=(%f,%f) ",eta,phi);
   	result->tags_[ctag] = *p;
        //set the veto variable
	if(*p>=MaxSigma)
	{veto = 1;}
	   
  } 

  else
  { 
   	sprintf(ctag,"eta_%i_error",k);
   	result->tags_[ctag]=0.0;
   	sprintf(ctag,"phi_%i_error",k);
   	result->tags_[ctag]=0.0;
   }

  
  if(p==ChisqValues.begin()||k>Num_to_print)
  {break;}
  
  p--;
  k++;
  }

//write out what value of NSigma you were using
//and the maximum number of bins to publish
result->tags_["MaxSigma"]=MaxSigma;
result->tags_["NSigma"]=NSigma;
result->tags_["Max Bins to Publish"]=Num_to_print;

//write out the number of bins that were above the threshold to the screen
  
  int BinsOver=ChisqValues.size();
  result->tags_["NBinsOver"]=BinsOver;

//check the thresholds

    if ( (BinsOver <= gthresho) && veto==0 ) {
      result->status_ = dqm_core::Result::Green;
    } else if (  (BinsOver < rthresho) && veto==0 ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
    
  ERS_DEBUG(2,"Result: "<<*result);
 
  return result; 
  
}
void
dqm_algorithms::Chi2Test_2D::printDescription(std::ostream& out)
{
 
  out<<"Chi2Test_2D: Gives back the position of the highest NBins relative to the reference, and also computes the chisq/ndf "<<std::endl;

  out<<"Mandatory Green/Red Threshold: NBins to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm\n"<<std::endl;
  
}

