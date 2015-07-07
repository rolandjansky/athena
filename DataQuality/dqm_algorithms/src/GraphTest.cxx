/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file RootFit.h file declares the dqm_algorithms::RootFit  class.
 * \author Haleh Hadavand
*/
#include <dqm_algorithms/GraphTest.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TClass.h>
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/Result.h>
#include <ers/ers.h>
#include <string>
#include <sstream>
#include <iostream>
#include <dqm_core/AlgorithmManager.h>

#define NPARS 6

static dqm_algorithms::GraphTest myInstance;

dqm_algorithms::GraphTest::GraphTest() {
  dqm_core::AlgorithmManager::instance().registerAlgorithm("GraphTest",this);
}

dqm_algorithms::GraphTest::~GraphTest() {}

dqm_algorithms::GraphTest* dqm_algorithms::GraphTest::clone(){
  return new GraphTest();
}

dqm_core::Result* dqm_algorithms::GraphTest::execute(const std::string & name,
						     const TObject & obj, 
						     const dqm_core::AlgorithmConfig & config )
{
  TGraph* graph;
  TGraph* refhist = 0;
  bool ownObject=false;
  if ( obj.IsA()->InheritsFrom("TGraph") )
    {
      ERS_DEBUG(2,"Got TGraph called: "<<obj.GetName()<<" of type:"<<obj.IsA()->GetName());
      graph = (TGraph*)&obj;
    } 
  else if ( obj.IsA()->InheritsFrom("TH1") )
    {
      ERS_DEBUG(2,"Got TH1: converting to TGraphErrors");
      graph = new TGraphErrors((TH1*)&obj);
      graph->SetNameTitle("TempG","TempG");
      ownObject = true; // In case we are here we have to delete the graph 
    }
  else
    {
      throw dqm_core::BadConfig(ERS_HERE,name,"Object is not a TGraph or a TH1");
    }
  //Now get reference object and check compatibility
  ERS_DEBUG(2,"Retreiving reference graph");

  try {
    refhist = static_cast<TGraph *>( config.getReference() );
  }
  catch ( dqm_core::Exception & ex ) {
    ERS_DEBUG(2,"No reference specified, will skip point-by-point comparison");
  }
  Int_t npoints = graph->GetN();
  ERS_DEBUG(2,"Number of points to be checked: "<<npoints);
  if ( refhist ) {
    if ( ! refhist->IsA()->InheritsFrom("TGraph") )
      {
	if ( ownObject ) delete graph;
	throw dqm_core::BadRefHist(ERS_HERE,name,"Reference is not a TGraph");
      }
    if ( refhist->GetN() != npoints )
      {
	if ( ownObject ) delete graph;
	throw dqm_core::BadConfig(ERS_HERE,name,"Reference and object with different number of points");
      }
  }
  else {
    ERS_DEBUG(2,"No reference specified, will skip point-by-point comparison");
  }
  // Configure the DQ algorithm
  const std::string param[NPARS] = {"XErrHigh","XErrLow","YErrHigh","YErrLow","DistFactor","NBins"};
  Double_t bigNumber = 1.7E+308; //some  big number...
  Double_t grValue[NPARS] = { bigNumber , bigNumber , bigNumber , bigNumber , 1, 0 };
  Double_t reValue[NPARS] = { bigNumber , bigNumber , bigNumber , bigNumber , 1, 0 };
  for ( unsigned int i = 0; i<NPARS;++i) {
    // get configuration for Green Threshold
    try 
      {
	grValue[i] = dqm_algorithms::tools::GetFromMap(param[i] , config.getGreenThresholds() );
      } 
    catch ( dqm_core::Exception & ex ) 
      {
	ERS_DEBUG(1,"Parameter: i"<<param[i]<<"' for Green not found in configuration, using default: "<<grValue[i]);
      }
    // get configuration for Red Threshold
    try 
      {
	reValue[i] = dqm_algorithms::tools::GetFromMap(param[i] , config.getRedThresholds() );
      } 
    catch ( dqm_core::Exception & ex ) 
      {
	ERS_DEBUG(1,"Parameter: i"<<param[i]<<"' for Red not found in configuration, using default: "<<reValue[i]);
      }
    if ( reValue[i] < grValue[i] ) // Non sense values
      {
	std::stringstream msg;
	msg<<"Configuration Error (Red<Green):"<<param[i]<<" G="<<grValue[i]<<" R="<<reValue[i];
	if ( ownObject ) delete graph;
	throw dqm_core::BadConfig(ERS_HERE,name,msg.str());
      }
  } // End of configuration retreival
#if DEBUG_LEVEL > 1
  if ( grValue[4]==0 ) { ERS_DEBUG(2,"Exact Match request for Green"); }
  if ( reValue[4]==0 ) { ERS_DEBUG(2,"Exact Match request for Red"); }
  std::stringstream configuration;
  for ( int i = 0;i<NPARS;++i)
    {
      configuration<<" - "<<param[i]<<" Green:"<<grValue[i]<<" Red:"<<reValue[i];
    }
  ERS_DEBUG(2,"Configuration: "<<configuration.str());
#endif
  // Just a translation in something more readable...
  Double_t grXEH =grValue[0]; // Error (High) on X
  Double_t grXEL =grValue[1]; // Error (Low) on X
  Double_t grYEH =grValue[2]; // Error (High) on Y
  Double_t grYEL =grValue[3]; // Error (Low) on Y
  Double_t grExpF=grValue[4]; // Moltiplicative Factor for DX,DY
  Double_t Ngreen=grValue[5]; // Number of points to fail
  Double_t reXEH =reValue[0]; // Error (High) on X
  Double_t reXEL =reValue[1]; // Error (Low) on X
  Double_t reYEH =reValue[2]; // Error (High) on Y
  Double_t reYEL =reValue[3]; // Error (Low) on Y
  Double_t reExpF=reValue[4]; // Multiplicative Factor for DX,DY
  Double_t Nred = reValue[5]; // Number of points to fail
  // Everything is now configured correctly, we can start
  Int_t yellowCounterErrorBars=0 , yellowCounterPoints=0;
  Int_t redCounterErrorBars=0 , redCounterPoints=0;
  std::stringstream errorsList;
  dqm_core::Result* result = new dqm_core::Result;
  for ( Int_t bin = 0 ; bin < npoints ; ++bin) //Loop on all points
    {
      // Check point error bars
      ERS_DEBUG(3,"====> Starting check on point "<<bin);
      ERS_DEBUG(3,"Checking error bars");
      Double_t xEH=graph->GetErrorXhigh(bin),xEL=graph->GetErrorXlow(bin);
      Double_t yEH=graph->GetErrorYhigh(bin),yEL=graph->GetErrorYlow(bin);
      if ( xEH>grXEH || xEL>grXEL || yEH>grYEH || yEL>grYEL ) 
	{
	  ERS_DEBUG(2,"[YELLOW] (point "<<bin<<") error bars:xH="<<xEH<<",xL="<<xEL<<",yH="<<yEH<<",yL="<<yEL);
	  ERS_DEBUG(3,"Limits are: xH="<<grValue[0]<<" xL="<<grValue[1]<<" yH="<<grValue[2]<<" yL="<<grValue[3]<<" nGreen="<<grValue[5]);
	  errorsList<<"; point "<<bin<<" Err. bars:xH="<<xEH<<",xL="<<xEL<<",yH="<<yEH<<",yL="<<yEL;
	  ++yellowCounterErrorBars;
	}
      if ( xEH>reXEH || xEL>reXEL || yEH>reYEH || yEL>reYEL ) 
	{
	  ERS_DEBUG(2,"[RED] (point "<<bin<<") error bars:xH="<<xEH<<",xL="<<xEL<<",yH="<<yEH<<",yL="<<yEL);
	  ERS_DEBUG(3,"Limits are: xH="<<reValue[0]<<" xL="<<reValue[1]<<" yH="<<reValue[2]<<" yL="<<reValue[3]<<" nRed="<<reValue[5]);
	  ERS_DEBUG(1,"[RED] Result");// game over, RED level reached
	  ++redCounterErrorBars;
	  result->tags_.insert(std::make_pair("RedErrorBarsForPoint",bin));
	}
      // Now Check if point is compatible with reference point (x_r,y_r)
      // reference point (asymmetric) errors bars 
      // (xEH,xEL,yEH,yEL) define the max. allowed distance for
      // The ProximityFactor can be used to set two different thresholds for
      // yellow and red.
      // If ref. point errors bars are zeros do not check that coordinate
      // Ex. Point is Green if: x_r - f*xEL =< x =< x_r + f*xEH AND
      //                        y_r - f*yEL =< y =< y_r + f*yEH
      ERS_DEBUG(3,"Checking compatibility with reference point");
      Double_t x,y;
      graph->GetPoint(bin,x,y);
      Double_t x_r = x ,y_r = y;
      if (refhist) refhist->GetPoint(bin,x_r,y_r);
      Double_t DX = x-x_r , DY=y-y_r;
      Double_t xEH_r = 0 , xEL_r = 0 , yEH_r = 0 , yEL_r = 0;  
      if ( refhist ) {
	xEH_r = refhist->GetErrorXhigh(bin);
	xEL_r = refhist->GetErrorXlow(bin);
	yEH_r = refhist->GetErrorYhigh(bin);
	yEL_r = refhist->GetErrorYlow(bin);
      }
      if ( xEH_r==0 && xEL_r==0 ) 
	{
	  ERS_DEBUG(3,"Turn off check on x (point "<<bin<<")");
	  DX=0; // Do not check X 
	}
      if ( yEH_r==0 && yEL_r==0 ) 
        {
	  ERS_DEBUG(3,"Turn off check on y (point "<<bin<<")");
	  DY=0; // Do not check Y
	} 
      if ( DX > grExpF*xEH_r || DX < -grExpF*xEL_r ||
	   DY > grExpF*yEH_r || DY < -grExpF*yEL_r ) 
	{
	  ERS_DEBUG(2,"[YELLOW] Point "<<bin<<" does not match with reference value (DX,DY)=("<<DX<<","<<DY<<")");
	  ERS_DEBUG(3,"Limits are: "<<-grExpF*xEL_r<<"<DX<"<<grExpF*xEH_r<<" ; "<<-grExpF*yEL_r<<"<DY<"<<grExpF*yEH_r);
	  errorsList<<"; point "<<bin<<" Does not match with reference (DX,DY)=("<<DX<<","<<DY<<")";
	  ++yellowCounterPoints;
	  std::stringstream out;
	  out << yellowCounterPoints;
	  std::string yellowtag = out.str();
	  if(yellowCounterPoints<10) yellowtag= "0"+yellowtag;
	  yellowtag ="YellowPointNumber"+yellowtag;
	  result->tags_.insert(std::make_pair(yellowtag,bin));
	}
      if ( DX > reExpF*xEH_r || DX < -reExpF*xEL_r ||
	   DY > reExpF*yEH_r || DY < -reExpF*yEL_r )
	{
	  ERS_DEBUG(2,"[RED] Point "<<bin<<" does not match with reference value (DX,DY)=("<<DX<<","<<DY<<")");
	  ERS_DEBUG(3,"Limits are: "<<-grExpF*xEL_r<<"<DX<"<<grExpF*xEH_r<<" ; "<<-grExpF*yEL_r<<"<DY<"<<grExpF*yEH_r);
	  ERS_DEBUG(1,"[RED] Result");
	  ++redCounterPoints;
	  std::stringstream out;
	  out << redCounterPoints;
	  std::string redtag = out.str();
	  if(redCounterPoints<10) redtag= "0"+redtag;
	  redtag ="RedPointNumber"+redtag;
	  result->tags_.insert(std::make_pair(redtag,bin));
	  //result->tags_.insert(std::make_pair("RedPointNumber"+redCounterPoints,bin));
	}
      ERS_DEBUG(3,"End of checks for Point "<<bin);
    } // End loop on points
  if ( redCounterErrorBars > Nred || redCounterPoints > Nred )
    {
      ERS_DEBUG(1,"[Error] Result with "<<redCounterErrorBars+redCounterPoints<<" errors");
      ERS_DEBUG(2,"List of errors"<<errorsList.str());
      if ( ownObject ) delete graph;
	  result->status_=dqm_core::Result::Red;
      if ( redCounterErrorBars > 0 )
	result->tags_.insert(std::make_pair("NumRedErrorBars",redCounterErrorBars));
      if ( redCounterPoints > 0 )
	result->tags_.insert(std::make_pair("NumRedComparison",redCounterPoints));
      return result;
    }
  if ( yellowCounterErrorBars > Ngreen || yellowCounterPoints > Ngreen )
    {
      ERS_DEBUG(1,"[YELLOW] Result with "<<yellowCounterErrorBars+yellowCounterPoints<<" errors");
      ERS_DEBUG(2,"List of errors"<<errorsList.str());
      if ( ownObject ) delete graph;
      if ( yellowCounterErrorBars > 0 )
	result->tags_.insert(std::make_pair("NumYellowsErrorBars",yellowCounterErrorBars));
      if ( yellowCounterPoints > 0 )
	result->tags_.insert(std::make_pair("NumYellowsComparison",yellowCounterPoints));
      result->status_=dqm_core::Result::Yellow;
      return result;
    }
  ERS_DEBUG(1,"[GREEN] Result");
  if ( ownObject ) delete graph;
  delete result;
  return new dqm_core::Result(dqm_core::Result::Green);
}

void
dqm_algorithms::GraphTest::printDescription(std::ostream& out)
{
  out<<"GraphTest DQ algorithm: check validity of a TGraph (or TH1)"<<std::endl
	   <<"This test can be used to perform a test on a TGraph with (or without) error bars, in case asymmmetric."<<std::endl
	   <<"It performs two tests:"<<std::endl
	   <<"1- It checks that each point in the TGraph has \"small\" error bars"<<std::endl
	   <<"2- It checks that each point is \"close\" to a reference value"<<std::endl
	   <<"For test 2 a reference TGraph has to be provided (in this way each point may have a different reference value). "<<std::endl
	   <<"The (x,y) coordinates of the point are compared with the reference point coordinates (x_r,y_r) and REFERENCE "<<std::endl
	   <<" point error bars defining the maximum allowed distance. i.e."<<std::endl 
	   <<"the point is good if: x_r - f*x_err < x < x_r + f*x_err AND y_r - f*y_err < y < y_r + f*y_err"<<std::endl
	   <<"\"f\" is a factor that can be used to specify different limits for Green and Red thresholds."<<std::endl
	   <<"If, for a specific reference point, the error bars along one coordinate are set to zero the test 2 is NOT performed for that point (this can be used to exclude, for example, a dead channel through the reference graph). "<<std::endl
	   <<"The algorithms can work also with TGraphAsymmErrors object, so high and low errors can always be specified separately."<<std::endl
	   <<"The optional parameters that can be set, separetely, for Green and Red thresholds are:"<<std::endl
	   <<"XErrHigh   : Maximum (higher) error bar length x coord (default is 1.844E19)."<<std::endl
	   <<"XErrLow    : Maximum (lower) error bar length x coord (default is 1.844E19)."<<std::endl
	   <<"YErrHigh   : Maximum (higher) error bar length y coord (default is 1.844E19)."<<std::endl
	   <<"YErrLow    : Maximum (lower) error bar length y coord (default is 1.844E19)."<<std::endl
	   <<"DistFactor : The factor \"f\" used in second part of the test (default is 1)."<<std::endl
	   <<"NBins      : This is the number of points allowed to fail to change result(default is 0)."<<std::endl;
    
}
