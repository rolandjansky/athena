/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlgorithmHelper.cpp does basic functions to get dqm_core::Results from algorithms 
 * \author Haleh Hadavand
 */
#ifndef DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_CXX
#define DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_CXX

#include <TF1.h>
#include <TH1.h>
#include <TH3F.h>
#include <TH2F.h>
#include <TH1F.h>

#include <Math/ProbFuncMathCore.h>
#include <Math/DistFunc.h>
#include <TClass.h>
#include <iostream>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <ers/ers.h>

#include <dqm_core/Result.h> 

#include <dqm_core/AlgorithmManager.h>
#include <dqm_core/LibraryManager.h>

// MN: ROOT6 is more const-correct
#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
#  define ROOT6_CONST const
#else
#  define ROOT6_CONST
#endif


std::map<std::string, double > 
dqm_algorithms::tools::GetFitParams(const TF1 * func )
{
  std::map<std::string,double> fitparams;

  const int nPar =(int) func->GetNpar();
  
  for ( int i = 0; i< nPar; ++i ) {
    std::string name = func->GetParName( i );
    fitparams[name] = func->GetParameter( i );
  }
  return fitparams;
}

std::map<std::string, double > 
dqm_algorithms::tools::GetFitParamErrors(const TF1 * func )
{
  std::map<std::string,double> fitParamErrors;

  const int nPar =(int) func->GetNpar();
  
  for ( int i = 0; i< nPar; ++i ) {
    std::string name = func->GetParName( i );
    fitParamErrors[name] = func->GetParError( i );
  }
  return fitParamErrors;
}



dqm_core::Result * 
dqm_algorithms::tools::MakeComparisons(	const std::map<std::string,double> & algparams, 
					const std::map<std::string,double> & gthreshold,
					const std::map<std::string,double> & rthreshold )
{
  
  int red=0;
  int yellow=0;
  int green=0;
  
  
  if (gthreshold.size() != rthreshold.size()) {
    throw dqm_core::BadConfig( ERS_HERE, "MakeComparisons", "Need same number of Red and Green Threshold" );
  }
 
  if (gthreshold.size() == 0 ) {
    throw dqm_core::BadConfig( ERS_HERE, "MakeComparisons", "No Threshold specified" );
  } 
  
  std::map<std::string,double>::const_iterator g_iter;
  
  for (g_iter=gthreshold.begin();g_iter!=gthreshold.end();g_iter++){
        
    std::string name=(std::string) g_iter->first;
    std::string findname=name;
    double gtvalue=g_iter->second;
    
    if (name == "AbsMean" )
      findname="Mean";
    if (name == "AbsXMean" )
      findname="XMean";
    if (name == "AbsYMean" )
      findname="YMean";

    
    std::map<std::string,double>::const_iterator ait = algparams.find(findname);
    std::map<std::string,double>::const_iterator rit = rthreshold.find(name);
    double rtvalue=-999;
    double avalue=-999;

    if ( ait != algparams.end()  ) {
      avalue=ait->second;
    } 
    else {
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }
    
    if ( rit != rthreshold.end() ) {
      rtvalue=rit->second;
    } 
    else {
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }
    

    if (name == "AbsMean" || name == "AbsXMean" || name== "AbsYMean") {
      avalue = fabs(avalue);
      if (gtvalue < 0 || rtvalue <0 ) {
	throw dqm_core::BadConfig( ERS_HERE, "None", name );
      }
    }

    ERS_DEBUG(1, name<<" :red value "<<rtvalue<<" :green value "<<gtvalue<<" :param value  "<<avalue); 
	
    if (gtvalue < rtvalue) {  

      if (avalue <= gtvalue ){
	++green;
      }else if(avalue < rtvalue){
	++yellow;
      }else {
	++red;
      }
    }
    else if (gtvalue > rtvalue) {
      if (avalue >= gtvalue ){
	++green;
      }else if(avalue > rtvalue){
	++yellow;
      }else {
	++red;
      }
    } 
    else {
      ERS_DEBUG(0, "red threshold (" << rtvalue << ") same as green threshold "<< gtvalue <<") for parameter " << name << ": can't evaluate result"); 
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }	    
    
  }
  ERS_DEBUG(1, "red: "<<red<<"   yellow: "<<yellow<<"   green: "<<green);

  
  dqm_core::Result* result = new dqm_core::Result();

  result->tags_ = algparams;


  if (red>0) {
    result->status_= dqm_core::Result::Red;
  }else if (yellow>0) {
    result->status_= dqm_core::Result::Yellow;
  }else if (green>0) {
    result->status_= dqm_core::Result::Green;
  }
    
  return result;
}

dqm_core::Result * 
dqm_algorithms::tools::CompareWithErrors( const std::map<std::string,double> & algparams, 
					  const std::map<std::string,double> & paramErrors,
					  const std::map<std::string,double> & gthreshold,
					  const std::map<std::string,double> & rthreshold, double minSig )
{
  
  int red       = 0;
  int yellow    = 0;
  int green     = 0;
  int undefined = 0;
  
  
  if (gthreshold.size() != rthreshold.size()) {
    throw dqm_core::BadConfig( ERS_HERE, "MakeComparisons", "Need same number of Red and Green Threshold" );
  }
 
  if (gthreshold.size() == 0 ) {
    throw dqm_core::BadConfig( ERS_HERE, "MakeComparisons", "No Threshold specified" );
  } 
  
  std::map<std::string,double>::const_iterator g_iter;
  
  for (g_iter=gthreshold.begin();g_iter!=gthreshold.end();g_iter++){
        
    std::string name=(std::string) g_iter->first;
    std::string findname=name;
    double gtvalue=g_iter->second;
    
    if (name == "AbsMean" )
      findname="Mean";
    if (name == "AbsXMean" )
      findname="XMean";
    if (name == "AbsYMean" )
      findname="YMean";

    
    std::map<std::string,double>::const_iterator ait = algparams.find(findname);
    std::map<std::string,double>::const_iterator errItr = paramErrors.find(findname);
    std::map<std::string,double>::const_iterator rit = rthreshold.find(name);

    double rtvalue = -999;
    double avalue  = -999;
    double error   = 0;
    
    if ( ait != algparams.end()  ) {
      avalue=ait->second;
    } 
    else {
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }
    
    if ( errItr != paramErrors.end()  ) {
      error = errItr->second;
    } 
    else {
      throw dqm_core::BadConfig( ERS_HERE, "Problem retrieving fit param error", name );
    }
    if ( error < 0 ) error = 0;
    
    if ( rit != rthreshold.end() ) {
      rtvalue=rit->second;
    } 
    else {
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }
    

    if (name == "AbsMean" || name == "AbsXMean" || name== "AbsYMean") {
      avalue = fabs(avalue);
      if (gtvalue < 0 || rtvalue <0 ) {
	throw dqm_core::BadConfig( ERS_HERE, "None", name );
      }
    }
    
    ERS_DEBUG(1, name<<" :red value "<<rtvalue<<" :green value "<<gtvalue<<" :param value  "<<avalue <<" :param error  "<<error); 
    
    double significanceBound = error * minSig;
    
  
    if (gtvalue < rtvalue) {  
      
      if ( avalue <= (gtvalue - significanceBound) ){
	++green;
      }
      else if( avalue > (rtvalue + significanceBound) ){
	++red;
      }
      else if( avalue > (gtvalue + significanceBound) ) {
	++yellow;
      }
      else {
	++undefined;
      }
    }
    else if (gtvalue > rtvalue) {
      if ( avalue >= (gtvalue + significanceBound) ){
	++green;
      }
      else if( avalue < (rtvalue - significanceBound) ){
	++red;
      }
      else if( avalue < (gtvalue - significanceBound) ){
	++yellow;
      }
      else {
	++undefined;
      }
    } 
    else {
      ERS_DEBUG(0, "red threshold same as green threshold: can't evaluate result"); 
      throw dqm_core::BadConfig( ERS_HERE, "None", name );
    }
     
  }
  ERS_DEBUG(1, "red: "<<red<<"   yellow: "<<yellow<<"   green: "<<green << "   undefined: " <<undefined);

  
  dqm_core::Result* result = new dqm_core::Result();

  result->tags_ = algparams;

  // We use a strung undefined here: if one of the test parameters was in the undefined region about gth,
  // then the overall result will be undefined. 
  if (red>0) {
    result->status_= dqm_core::Result::Red;
  }
  else if (yellow>0) {
    result->status_= dqm_core::Result::Yellow;
  }
  else if (undefined>0) {
    result->status_= dqm_core::Result::Undefined;
  }
  else if (green>0) {
    result->status_= dqm_core::Result::Green;
  }
    
  return result;
}

dqm_core::Result *
dqm_algorithms::tools::GetFitResult (const TF1 * func, const dqm_core::AlgorithmConfig & config, double minSig )
{  
  std::map<std::string, double > params = GetFitParams(func);
  std::map<std::string, double > greenthresh=config.getGreenThresholds();
  std::map<std::string,double>::const_iterator ait = greenthresh.find("Chi2_per_NDF");
  if ( ait != greenthresh.end()  ) {
    params["Chi2_per_NDF"]=func->GetChisquare()/ func->GetNDF();
  } 
  double subtractfrommean = GetFirstFromMap( "SubtractFromMean", config.getParameters(), 0 );
    
  params["Mean"] = params["Mean"] - subtractfrommean;
  dqm_core::Result * result;
  std::map<std::string, double > paramErrors = GetFitParamErrors(func);
  if( minSig == 0) {
    result = MakeComparisons( params, config.getGreenThresholds(), config.getRedThresholds() );
    //result->tags_ = params;
  }
  else {
    result = CompareWithErrors( params, paramErrors, config.getGreenThresholds(), config.getRedThresholds(), minSig );
  }
  
  result->tags_ = params;
  for ( std::map<std::string,double>::const_iterator peItr = paramErrors.begin(); peItr != paramErrors.end(); ++peItr ) {
    std::string errorName =  peItr->first;
    errorName += " Error";
    result->tags_[errorName] = peItr->second;
  }

  return result;
}

double dqm_algorithms::tools::GetFirstFromMap(const std::string &paramName, const std::map<std::string, double > &params)
{
    std::map<std::string, double >::const_iterator it = params.find(paramName);
    if (it == params.end()) {
        throw dqm_core::BadConfig(ERS_HERE, "None", paramName);	// this is the only difference between the two overloaded versions
    } else {
        return it->second;
    }
}

double dqm_algorithms::tools::GetFirstFromMap(const std::string &paramName, const std::map<std::string, double > &params, double defaultValue)
{
    std::map<std::string, double >::const_iterator it = params.find(paramName);
    if (it == params.end()) {
        return defaultValue; // this is the only difference between the two overloaded versions
    } else {
        return it->second;
    }
}        	

std::vector<int> dqm_algorithms::tools::GetBinRange(const TH1 *h, const std::map<std::string, double > &params)
{
  std::vector<int> range;
  ROOT6_CONST TAxis *xAxis = h->GetXaxis();
  ROOT6_CONST TAxis *yAxis = h->GetYaxis();

  if (h->GetDimension() > 2) {
    throw dqm_core::BadConfig(ERS_HERE, h->GetName(), "histogram has more than 2 dimensions");
  }

  const double notFound = -99999;
  const double xmin = dqm_algorithms::tools::GetFirstFromMap("xmin", params, notFound);
  const double xmax = dqm_algorithms::tools::GetFirstFromMap("xmax", params, notFound);
  const double ymin = dqm_algorithms::tools::GetFirstFromMap("ymin", params, notFound);
  const double ymax = dqm_algorithms::tools::GetFirstFromMap("ymax", params, notFound);

  /**The nested ternaries do the following. 
   * Suppose xmax (or ymax) happen to be a value that corresponds to the boundary of a bin. 
   * TAxis::FindBin then returns the bin for which the xmax is the lower edge of the bin.
   * The nested ternaries prevent this happening by decrementing the bin number returned by TAxis::FindBin by 1.
   */
  const int xlow    = (xmin == notFound) ? 1                 : xAxis->FindBin(xmin);
  const int xhigh   = (xmax == notFound) ? xAxis->GetNbins() : (xAxis->GetBinLowEdge(xAxis->FindBin(xmax))== xmax) ? (xAxis->FindBin(xmax)-1) : xAxis->FindBin(xmax);
  const int ylow    = (ymin == notFound) ? 1                 : yAxis->FindBin(ymin);
  const int yhigh   = (ymax == notFound) ? yAxis->GetNbins() : (yAxis->GetBinLowEdge(yAxis->FindBin(ymax))== ymax) ? (yAxis->FindBin(ymax)-1) : yAxis->FindBin(ymax);

                                                                                                                                                             
  if (xlow>xhigh) {
    char temp[128];
    sprintf(temp,"Bin Range Error: xmin = %.3g, xmax = %.3g",xmin,xmax);
    throw dqm_core::BadConfig( ERS_HERE, h->GetName(), temp ); 
  }
  if (ylow>yhigh) {
    char temp[128];
    sprintf(temp,"Bin Range Error: xmin = %.3g, xmax = %.3g",xmin,xmax);
    throw dqm_core::BadConfig( ERS_HERE, h->GetName(), temp ); 
  }
       
  range.push_back(xlow);
  range.push_back(xhigh);
  range.push_back(ylow);
  range.push_back(yhigh);
  return range;
}

void 
dqm_algorithms::tools::PublishBin(const TH1 * histogram, int x, int y, double inputcont, dqm_core::Result *result) {
	
  std::string name = histogram->GetName();
  double xbin = histogram->GetXaxis()->GetBinCenter(x);
  std::string xbinlabel = (std::string)(histogram->GetXaxis()->GetBinLabel(x));
  if (histogram->GetDimension() == 2){
    double ybin = histogram->GetYaxis()->GetBinCenter(y);
    std::string ybinlabel = (std::string)(histogram->GetYaxis()->GetBinLabel(y));
    std::string badbin = Form("x-axis %s(%f)  y-axis %s(%f))",xbinlabel.c_str(),xbin,ybinlabel.c_str(),ybin);
    result->tags_[badbin.c_str()] = inputcont;
    ERS_DEBUG(1,"x bin "<<xbin<<" y bin "<<ybin <<" value "<<inputcont);
  } else { 
    std::string badbin = Form("%s(%f)",xbinlabel.c_str(),xbin);
    result->tags_[badbin.c_str()] = inputcont;
    ERS_DEBUG(1,"x bin "<<xbin<<" value "<<inputcont);
  }
}


TH1*
dqm_algorithms::tools::DivideByHistogram(const TH1* hNumerator, const TH1* hDenominator) 
{
  
  TH1 * hResult = 0;

  int hNdimension = hNumerator->GetDimension();
  int hDdimension = hDenominator->GetDimension();

  if( hDdimension == hNdimension ) {
    if( (hNumerator->GetNbinsX() != hDenominator->GetNbinsX()) || (hNumerator->GetNbinsY() != hDenominator->GetNbinsY()) || 
	(hNumerator->GetNbinsZ() != hNumerator->GetNbinsZ()) ) {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "number of bins do not match");
    }
    //since the dimensions match, use the root TH1 division method
    //as recommended in root documentation, we call Sumw2 first to keep the errors well behaved:
    
    if( hNdimension == 1) {
      hResult = BookHistogramByExample(hNumerator,"hQuotient","Result of Division of One Histogram By Another",XAxis);
    }
    else if ( hNdimension == 2 ) {
      hResult = BookHistogramByExample(hNumerator,"hQuotient","Result of Division of One Histogram By Another",XYAxes);
    }
    else if ( hNdimension == 3 ) {
      hResult = BookHistogramByExample(hNumerator,"hQuotient","Result of Division of One Histogram By Another",XYZAxes);
    }
    else {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "Histograms with dimension greater than 3 are not supported");
    }
    hResult->Sumw2();

    hResult->Divide(hNumerator,hDenominator);
  }
  else if ( hNdimension > hDdimension ) {
    //Handle division for case where denominator has lower dimension than numerator:
    //There are only a few possibilities, hNdimension = 2 or 3, hDdimension = 1 or 2.
      
    ROOT6_CONST TAxis* xax = hNumerator->GetXaxis();
    int nbinsx = xax->GetNbins();
    ROOT6_CONST TAxis* yax = hNumerator->GetYaxis();
    int nbinsy = yax->GetNbins();
    ROOT6_CONST TAxis* zax = hNumerator->GetZaxis();
    int nbinsz = zax->GetNbins();

    if( nbinsx != hDenominator->GetNbinsX() ) {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "number of bins in x-axis do not match");
    }
    if( (hDdimension == 2) && (nbinsy != hDenominator->GetNbinsY()) ) {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "number of bins in y-axis do not match");
    }
    
    double numeratorCont = 0;
    double denominatorCont = 0;
    double numeratorError = 0;
    double denominatorError = 0;
    
    if(hNdimension == 2) {
      hResult = BookHistogramByExample(hNumerator,"hQuotient","Result of Division of One Histogram By Another",XYAxes);
    }
    else if(hNdimension == 3) {
      hResult = BookHistogramByExample(hNumerator,"hQuotient","Result of Division of One Histogram By Another",XYZAxes);
    }
    else {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "Histograms with dimension greater than 3 are not supported");
    }
    
    for (int i=1; i <= nbinsx; i++) {
      if( hDdimension == 1){
	denominatorCont = hDenominator->GetBinContent(i);
	denominatorError = hDenominator->GetBinError(i);
      }
      for (int j=1; j <= nbinsy; j++) {
	if( hDdimension == 2){
	  denominatorCont = hDenominator->GetBinContent(i,j);
	  denominatorError = hDenominator->GetBinError(i,j);
	}
	if( hNdimension == 2){ 
	  numeratorCont = hNumerator->GetBinContent(i,j);
	  numeratorError = hNumerator->GetBinError(i,j);
	  if(denominatorCont != 0){
	    hResult->SetBinContent(i,j,numeratorCont/denominatorCont);
	    hResult->SetBinError(i,j,sqrt( ( pow(numeratorError,2)/pow(denominatorCont,2) )
					   + ( pow(denominatorError,2) * pow(numeratorCont,2) / pow(denominatorCont,4) ) ) );
	  }
	}
	else {
	  for (int k=1; k <= nbinsz; k++) {
	    numeratorCont = hNumerator->GetBinContent(i,j,k);
	    numeratorError = hNumerator->GetBinError(i,j,k);
	    if(denominatorCont != 0){
	      hResult->SetBinContent(i,j,k,numeratorCont/denominatorCont);
	      hResult->SetBinError(i,j,k,sqrt( ( pow(numeratorError,2)/pow(denominatorCont,2) )
					       + ( pow(denominatorError,2) * pow(numeratorCont,2) / pow(denominatorCont,4) ) ) );
	    }
	  }
	}
      }
    }
  }
  else {
    //Handle division for case where the numerator has lower dimension than the denominator
    //There are only a few possibilities, hNdimension = 1 or 2, hDdimension = 2 or 3.
    
    ROOT6_CONST TAxis* xax = hDenominator->GetXaxis();
    int nbinsx = xax->GetNbins();
    ROOT6_CONST TAxis* yax = hDenominator->GetYaxis();
    int nbinsy = yax->GetNbins();
    ROOT6_CONST TAxis* zax = hDenominator->GetZaxis();
    int nbinsz = zax->GetNbins();
    
    if( nbinsx != hNumerator->GetNbinsX() ) {
      throw dqm_core::BadConfig( ERS_HERE, "number of bins in x-axis", "do not match");
    }
    if( (hNdimension == 2) && (nbinsy != hNumerator->GetNbinsY()) ) {
      throw dqm_core::BadConfig( ERS_HERE, "number of bins in y-axis", "do not match");
    }
    
    double denominatorCont = 0;
    double numeratorCont = 0;
    double denominatorError = 0;
    double numeratorError = 0;
    
    if(hDdimension == 2) {
      hResult = BookHistogramByExample(hDenominator,"hQuotient","Result of Division of One Histogram By Another",XYAxes);
    }
    else if(hDdimension == 3) {
      hResult = BookHistogramByExample(hDenominator,"hQuotient","Result of Division of One Histogram By Another",XYZAxes);
    }
    else {
      throw dqm_core::BadConfig( ERS_HERE, "DivideByHistogram", "Histograms with dimension greater than 3 are not supported");
    }
    
    // Always loop so at to handle over/underflows too
    for (int i=1; i <= nbinsx; i++) {
      if( hNdimension == 1){
	numeratorCont = hNumerator->GetBinContent(i);
	numeratorError = hNumerator->GetBinError(i);
      }
      for (int j=1; j <= nbinsy; j++) {
	if( hNdimension == 2){
	  numeratorCont = hNumerator->GetBinContent(i,j);
	  numeratorError = hNumerator->GetBinError(i,j);
	}
	if( hDdimension == 2){ 
	  denominatorCont = hDenominator->GetBinContent(i,j);
	  denominatorError = hDenominator->GetBinError(i,j);
	  if(denominatorCont != 0){
	    hResult->SetBinContent(i,j,numeratorCont/denominatorCont);
	    hResult->SetBinError(i,j,sqrt( ( pow(numeratorError,2)/pow(denominatorCont,2) )
					   + ( pow(denominatorError,2) * pow(numeratorCont,2) / pow(denominatorCont,4) ) ) );
	  }
	}
	else {
	  for (int k=1; k <= nbinsz; k++) {
	    denominatorCont = hDenominator->GetBinContent(i,j,k);
	    denominatorError = hDenominator->GetBinError(i,j,k);
	    if(denominatorCont != 0){
	      hResult->SetBinContent(i,j,k,numeratorCont/denominatorCont);
	      hResult->SetBinError(i,j,sqrt( ( pow(numeratorError,2)/pow(denominatorCont,2) )
					     + ( pow(denominatorError,2) * pow(numeratorCont,2) / pow(denominatorCont,4) ) ) );
	    }
	  }
	}
      }
    }
  }
  return hResult;
}

std::string 
dqm_algorithms::tools::ExtractAlgorithmName(const dqm_core::AlgorithmConfig& config) 
{
  // Function to extract the name of an auxiliary algorithm specified through an algorithm configuration parameter
  // Uses syntax AuxAlgName::[Algorithm Name] = [value], where value is ignored.
  
  std::string algNameTag = "AuxAlgName--";
  std::string algName = "None";
  
  if (config.getParameters().empty()){
    throw dqm_core::BadConfig( ERS_HERE, "ExtractAlgorithmName","called with an empty config file");
  }
  
  std::map<std::string,double > confParams = config.getParameters();//Copy is necessary as AlgorithmConfig does not return map by reference (bug?)
  for ( std::map<std::string,double >::const_iterator itr = confParams.begin();itr != confParams.end();++itr ) {
    size_t stringPos;
    if ( (stringPos = itr->first.find(algNameTag)) != std::string::npos ){
      stringPos += algNameTag.length();
      algName = itr->first.substr(stringPos);
    }
  }

  return algName;
}

dqm_core::Result *
dqm_algorithms::tools::ExecuteNamedAlgorithm(const std::string & name, const TObject & object, const dqm_core::AlgorithmConfig & config)
{
  // Function to find and execute the named algorithm
  dqm_core::Algorithm* algorithm = 0;

  if (name.compare("None") == 0 ) {
    throw dqm_core::BadConfig( ERS_HERE,"no auxiliary algorithm specified", name);
  }

  try {
    algorithm = dqm_core::AlgorithmManager::instance().getAlgorithm(name);
  }
  catch( dqm_core::Exception& ex ) {
    throw dqm_core::BadConfig( ERS_HERE,"unable to get algorithm of this name", name);
  }

  return algorithm->execute(name,object,config);
}


void
dqm_algorithms::tools::ModifyHistogram(TH1 * histogram, const dqm_core::AlgorithmConfig & config)
{
  if (config.getParameters().empty()){
    //Nothing to do
    return;
  }
  std::map<std::string,double > confParams = config.getParameters();//Copy is necessary as AlgorithmConfig does not return map by reference (bug?)
  for ( std::map<std::string,double >::const_iterator itr = confParams.begin();itr != confParams.end();++itr ) {
    if ( itr->first.find("MultiplyHistogramByValue") != std::string::npos ){
      histogram->Scale(itr->second);
      continue;
    }
    if ( itr->first.find("DivideHistogramByValue") != std::string::npos ){
      histogram->Scale( (1./itr->second) );
      continue;
    }
    size_t stringPos;
    std::string configTag = "SetHistogramTitle";
    if  ( (stringPos = itr->first.find(configTag)) != std::string::npos ){
      stringPos += configTag.length();
      histogram->SetTitle( (itr->first.substr(stringPos)).c_str() );
      continue;
    }
  }
}

TH1*
dqm_algorithms::tools::BookHistogramByExample(const TH1* histogram, const std::string& name, const std::string& title, AxisType axisType){
  
  switch (axisType) {
  case XYAxes:

    if (histogram->GetDimension() == 1){
      throw dqm_core::BadConfig( ERS_HERE, name,"can't make a 2D histogram from a 1D histogram");
    }
    TH2F * resultXY;
    if(histogram->GetXaxis()->IsVariableBinSize()) {
      resultXY = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsX(),histogram->GetXaxis()->GetXbins()->GetArray(),
			  histogram->GetNbinsY(),histogram->GetYaxis()->GetXbins()->GetArray());
    }
    else {
      resultXY = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsX(),histogram->GetXaxis()->GetXmin(),histogram->GetXaxis()->GetXmax(),
			  histogram->GetNbinsY(),histogram->GetYaxis()->GetXmin(),histogram->GetYaxis()->GetXmax());
    }

    resultXY->SetXTitle(histogram->GetXaxis()->GetTitle());
    resultXY->SetYTitle(histogram->GetYaxis()->GetTitle());
    return (TH1*) resultXY;

  case YAxis:

    if (histogram->GetDimension() == 1){
      throw dqm_core::BadConfig( ERS_HERE, name, "can't extract a Y-Axis from a 1D histogram");
    }
    TH1F *resultY;
    if(histogram->GetYaxis()->IsVariableBinSize()) {
      resultY = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsY(),histogram->GetYaxis()->GetXbins()->GetArray());
    }
    else {
      resultY = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsY(),histogram->GetYaxis()->GetXmin(),histogram->GetYaxis()->GetXmax());
    }
    resultY->SetXTitle(histogram->GetYaxis()->GetTitle());
    return (TH1*) resultY;
   
  case XYZAxes:

    if (histogram->GetDimension() < 3){
      throw dqm_core::BadConfig( ERS_HERE, name, "can't make a 3D histrogram without at least a 3D histogram");
    }
    TH3F *resultXYZ;
    if(histogram->GetXaxis()->IsVariableBinSize()) {
      resultXYZ = new TH3F(name.c_str(),title.c_str(),
			   histogram->GetNbinsX(),histogram->GetXaxis()->GetXbins()->GetArray(),
			   histogram->GetNbinsY(),histogram->GetYaxis()->GetXbins()->GetArray(),
			   histogram->GetNbinsZ(),histogram->GetZaxis()->GetXbins()->GetArray());
    }
    else {
      resultXYZ = new TH3F(name.c_str(),title.c_str(),
			   histogram->GetNbinsX(),histogram->GetXaxis()->GetXmin(),histogram->GetXaxis()->GetXmax(),
			   histogram->GetNbinsY(),histogram->GetYaxis()->GetXmin(),histogram->GetYaxis()->GetXmax(),
			   histogram->GetNbinsZ(),histogram->GetZaxis()->GetXmin(),histogram->GetZaxis()->GetXmax());
    }
    resultXYZ->SetXTitle(histogram->GetXaxis()->GetTitle());
    resultXYZ->SetYTitle(histogram->GetYaxis()->GetTitle());
    resultXYZ->SetZTitle(histogram->GetZaxis()->GetTitle());
    return resultXYZ;

  case XZAxes:

    if (histogram->GetDimension() < 3){
      throw dqm_core::BadConfig( ERS_HERE, name,"can't extract a Z-Axis from a dimension < 3 histogram");
    }
    TH2F * resultXZ;
    if(histogram->GetXaxis()->IsVariableBinSize()) {
      resultXZ = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsX(),histogram->GetXaxis()->GetXbins()->GetArray(),
			  histogram->GetNbinsZ(),histogram->GetZaxis()->GetXbins()->GetArray());
    }
    else {
      resultXZ = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsX(),histogram->GetXaxis()->GetXmin(),histogram->GetXaxis()->GetXmax(),
			  histogram->GetNbinsZ(),histogram->GetZaxis()->GetXmin(),histogram->GetZaxis()->GetXmax());
    }
    resultXZ->SetXTitle(histogram->GetXaxis()->GetTitle());
    resultXZ->SetYTitle(histogram->GetZaxis()->GetTitle());
    return (TH1*) resultXZ;

  case YZAxes:

    if (histogram->GetDimension() < 3){
      throw dqm_core::BadConfig( ERS_HERE, name,"can't extract a Z-Axis from a dimension < 3 histogram");
    }
    TH2F * resultYZ;
    if(histogram->GetYaxis()->IsVariableBinSize()) {
      resultYZ = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsY(),histogram->GetYaxis()->GetXbins()->GetArray(),
			  histogram->GetNbinsZ(),histogram->GetZaxis()->GetXbins()->GetArray());
    }
    else {
      resultYZ = new TH2F(name.c_str(),title.c_str(),
			  histogram->GetNbinsY(),histogram->GetYaxis()->GetXmin(),histogram->GetYaxis()->GetXmax(),
			  histogram->GetNbinsZ(),histogram->GetZaxis()->GetXmin(),histogram->GetZaxis()->GetXmax());
    }
    resultYZ->SetXTitle(histogram->GetYaxis()->GetTitle());
    resultYZ->SetYTitle(histogram->GetZaxis()->GetTitle());
    return (TH1*) resultYZ;

  case ZAxis:

    if (histogram->GetDimension() < 3){
      throw dqm_core::BadConfig( ERS_HERE, name, "can't extract a Z-Axis from a dimension < 3 histogram");
    }
    TH1F *resultZ;
    if(histogram->GetZaxis()->IsVariableBinSize()) {
      resultZ = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsZ(),histogram->GetZaxis()->GetXbins()->GetArray());
    }
    else {
      resultZ = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsZ(),histogram->GetZaxis()->GetXmin(),histogram->GetZaxis()->GetXmax());
    }
    resultZ->SetXTitle(histogram->GetZaxis()->GetTitle());
    return (TH1*) resultZ;

  case XAxis:
  default:

    TH1F *resultX;
    if(histogram->GetXaxis()->IsVariableBinSize()) {
      resultX = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsX(),histogram->GetXaxis()->GetXbins()->GetArray());
    }
    else {
      resultX = new TH1F(name.c_str(),title.c_str(),
			 histogram->GetNbinsX(),histogram->GetXaxis()->GetXmin(),histogram->GetXaxis()->GetXmax());
    }
    resultX->SetXTitle(histogram->GetXaxis()->GetTitle());
    return (TH1*) resultX ;
  }

}

void
dqm_algorithms::tools::handleReference( const TObject& ro ,
					TObject*& firstReference ,
					TObject*& secondReference )
{
  if ( ro.InheritsFrom("TH1") )
    {
      //Simple reference
      firstReference = const_cast<TObject*>(&ro);
      secondReference = 0;
      return;
    }
  else if ( ro.InheritsFrom("TCollection") )
    {
      //The reference is a collection of TObjects 
      const TCollection* coll = static_cast<const TCollection*>(&ro);
      TIterator* it = coll->MakeIterator();
      TObject* content = it->Next();
      if ( content == 0 || content->InheritsFrom("TH1") == kFALSE )  
	{ 
	  throw dqm_core::BadRefHist(ERS_HERE,"handleReference"," Could not retreive reference"); 
	}
      firstReference = static_cast<TH1 *>(content); 
      Int_t csize = coll->GetEntries();
      if ( csize == 1 )
	{
	  //Only one element, no secondReference is available.
	  secondReference = 0;
	  return;
	}
      else if ( coll->GetEntries() == 2 ) 
	{ 
	  //The collection is a pair, the second reference will be used directly
	  secondReference = it->Next(); 
	  return;
	}
      else 
	{
	  //This case is more complex, we basically want to create a new collection without the first element
	  secondReference = static_cast<TObject*>( (coll->IsA())->New() );//Create a new container of the same class as the original one
	  if ( secondReference == 0 || secondReference->InheritsFrom("TCollection") == kFALSE )
	    {
	      throw dqm_core::BadRefHist(ERS_HERE,"handleReference"," Could not retreive reference"); 
	    }
	  while ( (content = it->Next()) )//start from second element, since we already called Next()
	    {
	      static_cast<TCollection*>(secondReference)->Add(content);
	    }
	  return;
	}
    }
  else
    {
      //This kind of reference is not supported
      throw dqm_core::BadRefHist(ERS_HERE,"handleReference"," Could not retreive reference");
    }
}



void 
dqm_algorithms::tools::findOutliers( std::vector<binContainer>& input, double& mean, double& scale, int& nIn, 
				     int nIterations, double exponent, double threshold, double SBCF, double nStop) {
  
  int size = input.size();
  if(size < 2) {
    return;
  }
  
  if (nStop < 0) nStop = 0;
  if (SBCF < 0) SBCF = 0;

  std::vector<binContainer>::const_iterator cbegin = input.begin();
  std::vector<binContainer>::const_iterator cend = input.end();

  std::vector<binContainer>::iterator begin = input.begin();
  std::vector<binContainer>::iterator end = input.end();
  
  nIn = 0;
  mean = 0;
  scale = 0;
 
  double newMean = 0;
  double newScale = 0;
  for ( int j = 0; j < nIterations ; j++ ) {
    mean = newMean;
    scale = newScale;

    int newNin = 0;
    double sum = 0;
    //Loop to test, counting and summing elements that pass:
    for ( std::vector<binContainer>::iterator it = begin; it != end; ++it ) {
      //The Test:
      it->test = ( (fabs(it->value - mean) < (threshold * scale) ) || (j==0) ); 
      if ( it->test ) {
	sum += it->value;
	newNin++;
      }
    }
    int nOut = size - newNin;
    // Check if the iteration process is complete (or failed):
    if (newNin < (2 + nStop + SBCF * nOut) ) {
      return;
    }
    newMean = newNin ? (sum / newNin) : 0; // avoid to divide by zero (should never happen)
    // Check if the iteration process has stabilized:
    if ( (newNin == nIn) && (newMean == mean) ) {
      return;
    }
    nIn = newNin;

    // Calculate the scale parameter:  
    double scaleSum = 0;
    for ( std::vector<binContainer>::const_iterator it = cbegin; it != cend; ++it ) {
      if(it->test) {
	scaleSum += pow( fabs(it->value - newMean) , exponent );
      }
    }
    newScale = pow( scaleSum / (nIn - 1 - (SBCF * nOut) ), 1/exponent);
  }
  return;
}

void
dqm_algorithms::tools::findOutliersUsingErrors( std::vector<binContainer>& input, double& mean, double& meanError, int& nIn, double minDiff, int minNin ) {

 
  //Perform outlier determination using the bin errors and the relative pull on the mean, 
  // Taken from algorithm by Michele Petteni

  if( minDiff < 0 ) minDiff = 0;
  if( minNin < 2) minNin = 3;

  nIn = input.size();
  if(nIn < minNin) {
    return;
  }
  meanError = 0;

  
  std::vector<binContainer>::iterator begin = input.begin();
  std::vector<binContainer>::iterator end = input.end();
  
  //First pass: calucalate the mean:
  double sum = 0;
  double sumError2 = 0;
  for( std::vector<binContainer>::iterator it = begin; it != end; ++it ) {
    sum += it->value;
    sumError2 += pow(it->error,2);
    it->test = true;
  }
  mean = sum / nIn;
  meanError = sqrt( sumError2/ nIn );
  //Loop to tag and remove outliers:
  while( nIn > 2 ) {
    
    //Use map for ordering by distance from mean:
    std::map<double,binContainer*> absDiffMap;
    for( std::vector<binContainer>::iterator it = begin; it != end; ++it ) {
      if( it->test ) {
	absDiffMap[ fabs( it->value - mean ) ] = &(*it);
      }
    }

    //If max(absDiff) < minDiff, this is not considered an outlier: we are done.
    std::map<double,binContainer*>::iterator outlierCandidate = absDiffMap.end();
    outlierCandidate--;//<- the last element in a map is the largest.
    if( outlierCandidate->first < minDiff ) {
      return;
    }
  
    //Find the mean and its error if we exclude the outlier candidate:
    double newMean = 0;
    double newMeanError = 0;
    sum = 0;
    for( std::map<double,binContainer*>::iterator it = absDiffMap.begin(); it != outlierCandidate; ++it) {
      sum += it->second->value;
      sumError2 += pow(it->second->error,2);
    }
    newMean = sum/(nIn - 1);  
    newMeanError = sqrt( sumError2/(nIn - 2) );

    double meanShift = fabs( mean - newMean );
 
    //If the shift is bigger than the new error the candidate is an outlier: mark it and move on to the next one.
    if( meanShift > newMeanError ) {
      outlierCandidate->second->test = false;
      mean = newMean;
      meanError = newMeanError;
      nIn--;
    }
    else {
      return;
    }
  } 
}

dqm_algorithms::tools::binCluster 
dqm_algorithms::tools::buildCluster( binContainer& seed, const std::vector<std::vector<binContainer*> >& binMap, 
				     const std::vector<double>& xValues, const std::vector<double>& yValues, double threshold, int topology ) {

  binCluster cluster = {0.,0.,xValues[seed.ix],yValues[seed.iy],-1.,0,seed.ix,seed.ix,seed.iy,seed.iy};
  std::list<binContainer*> binsToCheck;
  std::vector<binContainer*> binsInCluster;
  binsToCheck.push_back(&seed);
  double pvpX = 0;
  double pvpY = 0;
  double error2 = 0;

  double maxDistanceX = -1;
  double maxDistanceY = -1;
  int ixRangeMax = (int)xValues.size() - 2;
  int iyRangeMax = (int)yValues.size() - 2;

  switch(topology) {
  case CylinderX:
    maxDistanceY = (yValues.back() - yValues.front()) / 2;
    break;
  case CylinderY:
    maxDistanceX = (xValues.back() - xValues.front()) / 2;
    break;
  case Torus:
    maxDistanceX = (xValues.back() - xValues.front()) / 2;
    maxDistanceY = (yValues.back() - yValues.front()) / 2;
    break;
  }
  
  while( binsToCheck.size() != 0) {

    std::list<binContainer*> newNeighbors;
    for( std::list<binContainer*>::const_iterator bin = binsToCheck.begin(); bin != binsToCheck.end(); ++bin ) {
      
      //If bin is a null pointer, continue to the next bin:
      if( *bin == 0) {
	continue;
      }

      //If the bin passes the threshold, add its values to the clusters and its neighbors to the list of bins to look at:
      //(As long as it hasn't already been added to another cluster or otherwise excluded)
      if( ( fabs((*bin)->value) > (threshold + 5*(*bin)->error ) ) && ( (*bin)->test == 0 ) && ((*bin)->error >= 0) )  {
	//Add bin to cluster:
	binsInCluster.push_back(*bin);
	(*bin)->test = 10;

	cluster.value += (*bin)->value;
	error2 += pow( (*bin)->error, 2);
	cluster.n++;

	int ix = (*bin)->ix;
	int iy = (*bin)->iy;
	double xPosition = xValues[ix];
	double yPosition = yValues[iy];

	//Adjust position information if it looks like we may have jumped a discontinuity 
	// and such discontinuities are expected. Also check for max, min ix, iy:
	if( (maxDistanceX > 0) && (fabs(xPosition - cluster.x) > maxDistanceX) ) {
	  //Correct for topological mapping difficulties:
	  if( xPosition < maxDistanceX ) {
	    xPosition += 2 * maxDistanceX;
	    if( (ix + ixRangeMax) > cluster.ixmax ) {
	      cluster.ixmax = ix + ixRangeMax;
	    }
	  }
	  else {
	    xPosition -= 2 * maxDistanceX;
	    if( (ix - ixRangeMax) < cluster.ixmin ) {
	      cluster.ixmin = ix - ixRangeMax;
	    }
	  }
	}
	else {
	  //No topological difficulties:
	  if( ix < cluster.ixmin ) {
	    cluster.ixmin = ix;
	  }
	  if( ix > cluster.ixmax ) {
	    cluster.ixmax = ix;
	  }
	}
	 
	if( (maxDistanceY > 0) && (fabs(yPosition - cluster.y) > maxDistanceY) ) {
	  //Correct for topological mapping difficulties:
	  if( yPosition < maxDistanceY ) {
	    yPosition += 2 * maxDistanceY;
	    if( (iy + iyRangeMax) > cluster.iymax ) {
	      cluster.iymax = iy + iyRangeMax;
	    }
	  }
	  else {
	    yPosition -= 2 * maxDistanceY;
	    if( (iy - iyRangeMax) < cluster.iymin ) {
	      cluster.iymin = iy - iyRangeMax;
	    }
	  }
	}
	else {
	  //No topological difficulties:
	  if( iy < cluster.iymin ) {
	    cluster.iymin = iy;
	  }
	  if( iy > cluster.iymax ) {
	    cluster.iymax = iy;
	  }
	}
	    
	pvpX += xPosition * (*bin)->value;
	pvpY += yPosition * (*bin)->value;

	
	//Find neighbors
	newNeighbors.push_back( binMap[ix+1][iy+1] );
	newNeighbors.push_back( binMap[ix+1][iy] );
	newNeighbors.push_back( binMap[ix+1][iy-1] );

	newNeighbors.push_back( binMap[ix][iy+1] );
	newNeighbors.push_back( binMap[ix][iy-1] );

	newNeighbors.push_back( binMap[ix-1][iy+1] );
	newNeighbors.push_back( binMap[ix-1][iy] );
	newNeighbors.push_back( binMap[ix-1][iy-1] );


      }
    }//End looping over binsToCheck (last set of neighbors)
    
    //Remove repeated references to the same bin:
    newNeighbors.unique();
    //Perpare to check the cluster's new neighbors:
    binsToCheck = newNeighbors;

    cluster.x = pvpX / cluster.value;
    cluster.y = pvpY / cluster.value;
  }//Finished finding bins in cluster

  //Assume that the input errors are uncorrelated (or that the caller will deal with this)
  cluster.error = sqrt( error2 );

  //loop to calculate the radius of the cluster (the value weighted average distance from its center)
  double dvpSum = 0;
  for(std::vector<binContainer*>::const_iterator bin = binsInCluster.begin(); bin != binsInCluster.end(); ++bin) {
    double xDistance = fabs( xValues[(*bin)->ix] - cluster.x );
    double yDistance = fabs( yValues[(*bin)->iy] - cluster.y );
    if (maxDistanceX > 0 && (xDistance > maxDistanceX) ) {
      xDistance = 2 * maxDistanceX - xDistance;
    }
    if (maxDistanceY > 0 && (yDistance > maxDistanceY) ) {
      yDistance = 2 * maxDistanceY - yDistance;
    }

    dvpSum += (*bin)->value * sqrt( pow( xValues[(*bin)->ix] - cluster.x,2) + pow(yValues[(*bin)->iy] - cluster.y,2) );
  }
  cluster.radius = dvpSum / cluster.value;

  //Check that positions are within the bounds of the histogram and correct
  while( cluster.ixmin < 1 ){
    cluster.ixmin += ixRangeMax;
  }
  while( cluster.ixmax > ixRangeMax ){
    cluster.ixmax -= ixRangeMax;
  }
  while( cluster.iymin < 1 ){
    cluster.iymin += iyRangeMax;
  }
  while( cluster.iymax > iyRangeMax ){
    cluster.iymax -= iyRangeMax;
  }

  if( cluster.x <  xValues.front() ) {
    cluster.x += 2 * maxDistanceX;
  }
  if( cluster.x >  xValues.back() ) {
    cluster.x -= 2 * maxDistanceX;
  }
  if( cluster.y <  yValues.front() ) {
    cluster.y += 2 * maxDistanceY;
  }
  if( cluster.y >  yValues.back() ) {
    cluster.y -= 2 * maxDistanceY;
  }


  return cluster;
}


//Method to map bins in binContainer based on binContainer::ix, binContainer::iy, and chosen topology
std::vector<std::vector<dqm_algorithms::tools::binContainer*> >
dqm_algorithms::tools::makeBinMap(std::vector<dqm_algorithms::tools::binContainer>& bins, int ixmax, int iymax, int topology) {
    
  std::vector<binContainer*> emptyVector;
  for(int iy = 0; iy <= iymax + 1; ++iy) {
    binContainer* emptyPointer = 0;
    emptyVector.push_back(emptyPointer);
  }
  std::vector<std::vector<binContainer*> > binMap;
  for(int ix = 0; ix <= ixmax + 1; ++ix) {
    binMap.push_back(emptyVector);
  }
  switch(topology) {
    //The mapping depends on the topology of the histogram.
  case CylinderX:
    //Cylinder About X-Axis (default topology), wraps max Y to min Y.
    //Connect iy == 1 with iy == iymax while leaving hard (null pointer) walls at ix = 0 and ix = imax+1:
    for (std::vector<binContainer>::iterator it = bins.begin(); it != bins.end(); ++it) {
      binMap[it->ix][it->iy] = &(*it);
      if(it->iy == 1){
	binMap[it->ix][iymax+1] = &(*it);
      }
      else if(it->iy == iymax){
	binMap[it->ix][0] = &(*it);
      }
    }
    break;
      
  case CylinderY:
    //Cylinder About Y-Axis 
    //Connect ix == 1 with ix == ixmax while leaving hard (null pointer) walls at iy = 0 and iy = imax+1:
    for (std::vector<binContainer>::iterator it = bins.begin(); it != bins.end(); ++it) {
      binMap[it->ix][it->iy] = &(*it);
      if(it->ix == 1){
	binMap[ixmax+1][it->iy] = &(*it);
      }
      else if(it->ix == ixmax){
	binMap[0][it->iy] = &(*it);
      }
    }
    break;  

  case Torus:
    //Torus:
    //Connect iy == 1 with iy == iymax and ix == 1 with ix == ixmax
    for (std::vector<binContainer>::iterator it = bins.begin(); it != bins.end(); ++it) {
      binMap[it->ix][it->iy] = &(*it);
      if(it->ix == 1){
	binMap[ixmax+1][it->iy] = &(*it);
	if(it->iy == 1){
	  binMap[ixmax+1][iymax+1] = &(*it);
	}
	if(it->iy == iymax){
	  binMap[ixmax+1][0] = &(*it);
	}
      }
      else if(it->ix == ixmax){
	binMap[0][it->iy] = &(*it);
	if(it->iy == 1){
	  binMap[0][iymax+1] = &(*it);
	}
	if(it->iy == iymax){
	  binMap[0][0] = &(*it);
	}
      }
      if(it->iy == 1){
	binMap[it->ix][iymax+1] = &(*it);
      }
      else if(it->iy == iymax){
	binMap[it->ix][0] = &(*it);
      }
    }
    break;  
      
  default:
    //Rectangle:
    //Treat the histogram as a simple rectangle, leaving null pointers at its boundaries:
    for (std::vector<binContainer>::iterator it = bins.begin(); it != bins.end(); ++it) {
      binMap[it->ix][it->iy] = &(*it);
    }
  }
  return binMap;
}

//An attempt to generalize the status combination process with the use of weights:
//Worst Case: 
dqm_core::Result::Status
dqm_algorithms::tools::WorstCaseAddStatus(dqm_core::Result::Status baseStatus, dqm_core::Result::Status addedStatus, float weight) { 
  //-- Acts analogously to a logical OR -- (Red <-> True, etc.)

  if(baseStatus == dqm_core::Result::Red) {
    return dqm_core::Result::Red;
  }
  //apply the "weight" to addedstatus only:
  if( weight <= 0 ) {
    return baseStatus;
  }

  if( weight <= 0.25 ) {
    if( addedStatus == dqm_core::Result::Yellow ) {
      addedStatus = dqm_core::Result::Green;
    }
  }
  if( weight <= 0.5 ) {
    if( addedStatus == dqm_core::Result::Red) {
      addedStatus = dqm_core::Result::Yellow;
    }
  }

  if(addedStatus == dqm_core::Result::Red) {
    return dqm_core::Result::Red;
  }

  if(baseStatus == dqm_core::Result::Disabled) {
    return addedStatus;
  }
  if(addedStatus == dqm_core::Result::Disabled) {
    return baseStatus;
  }
  if(baseStatus == dqm_core::Result::Undefined) {
    return addedStatus;
  }
  if(addedStatus == dqm_core::Result::Undefined) {
    return baseStatus;
  }

  if( (baseStatus == dqm_core::Result::Green) && (addedStatus == dqm_core::Result::Green) ) {
    return dqm_core::Result::Green;
  }

  return dqm_core::Result::Yellow;
}
//Best Case:
dqm_core::Result::Status
dqm_algorithms::tools::BestCaseAddStatus(dqm_core::Result::Status baseStatus, dqm_core::Result::Status addedStatus, float weight) { 
  //-- Acts analogously to a logical AND -- (Red <-> True, etc.)

  //apply the "weight" to addedstatus only: (same scheme as in WorstCaseAddStatus)
  if( weight <= 0 ) {
    return baseStatus;
  }
  if( weight <= 0.25 ) {
    if( addedStatus == dqm_core::Result::Yellow ) {
      addedStatus = dqm_core::Result::Green;
    }
  }
  if( weight <= 0.5 ) {
    if( addedStatus == dqm_core::Result::Red) {
      addedStatus = dqm_core::Result::Yellow;
    }
  }
  
  if( (baseStatus == dqm_core::Result::Disabled) || (addedStatus == dqm_core::Result::Disabled) ) {
    return dqm_core::Result::Disabled;
  }

  if( (baseStatus == dqm_core::Result::Undefined) || (addedStatus == dqm_core::Result::Undefined) ) {
    return dqm_core::Result::Undefined;
  }

  if( (baseStatus == dqm_core::Result::Green) || (addedStatus == dqm_core::Result::Green) ) {
    return dqm_core::Result::Green;
  }

  if( (baseStatus == dqm_core::Result::Red) && (addedStatus == dqm_core::Result::Red) ) {
    return dqm_core::Result::Red;
  }
  
  return dqm_core::Result::Yellow;
  
}


std::pair<double,double> 
dqm_algorithms::tools::CalcBinsProbChisq(std::vector<double> inputval,std::vector<double> inputerr,std::vector<double> x0,std::vector<double> x0err){

  double chisq = 0.;
  std::vector<double>::iterator iter_vals = inputval.begin();
  std::vector<double>::iterator iter_err = inputerr.begin();

  std::vector<double>::iterator iter_x0 = x0.begin();
  std::vector<double>::iterator iter_x0err = x0err.begin();

  int ndf = 0;
  for ( ; iter_vals != inputval.end(); ++iter_vals,++iter_err,++iter_x0,++iter_x0err){
    if (fabs(*iter_err) > 1.0e-5 || fabs(*iter_x0err) > 1.0e-5){
      double chisq_cont = pow(((*iter_vals)-(*iter_x0)),2.)/(pow(*iter_err,2.0)+pow(*iter_x0err,2.0));
      ++ndf;
      chisq += chisq_cont;
    }
  }

  double prob = ROOT::Math::chisquared_cdf_c(chisq,ndf);
  double sigma_chisq = ROOT::Math::normal_quantile_c(prob,1.);
  return std::make_pair(prob,sigma_chisq);
}

std::pair<double,double> 
dqm_algorithms::tools::CalcBinsProbChisq(std::vector<double> inputval,std::vector<double> inputerr,double x0,double x0_err){

  double chisq = 0.;
  std::vector<double>::iterator iter_vals = inputval.begin();
  std::vector<double>::iterator iter_err = inputerr.begin();
  int ndf = 0;
  for ( ; iter_vals != inputval.end(); ++iter_vals,++iter_err){
    if (fabs(*iter_err) > 1.0e-5){
      double chisq_cont = pow(((*iter_vals)-x0),2.)/(pow(*iter_err,2.0)+pow(x0_err,2.0));
      ++ndf;
      chisq += chisq_cont;
    }
  }

  double prob = ROOT::Math::chisquared_cdf_c(chisq,ndf);
  double sigma_chisq = ROOT::Math::normal_quantile_c(prob,1.);
  return std::make_pair(prob,sigma_chisq);
}

void 
dqm_algorithms::tools::MergePastMinStat(std::vector<std::vector<tools::binContainer> >& strips, int minStat) {
  //Method to merge neighboring vectors so that all non-empty vectors have a size of at least minStat
  //If two vectors are equally close to the vector with size < minstat, it will be merged with the one
  //with the smallest size.

  if(strips.size() < 2) {
    return;
  }

  std::vector<std::vector<tools::binContainer> >::iterator begining = strips.begin();
  std::vector<std::vector<tools::binContainer> >::iterator ending   = strips.end();
  --ending;

  while ( begining != ending ) {
    std::vector<std::vector<tools::binContainer> >::iterator minBinsItr = begining;   
    for( std::vector<std::vector<tools::binContainer> >::iterator itr = begining; itr != (ending+1); ++itr ) {
      if( (!itr->empty()) && ((itr->size() < minBinsItr->size()) || minBinsItr->empty()) ) {
	minBinsItr = itr;
      }
    }
    
    if( ((int)minBinsItr->size()) < minStat ) {
      if( minBinsItr == begining ) {
	//Merge right
	std::vector<std::vector<tools::binContainer> >::iterator mergeStripItr = minBinsItr;
	++mergeStripItr;
	while( mergeStripItr->empty() ) {
	  ++mergeStripItr;
	  if( mergeStripItr == ending ) break;
	}
	mergeStripItr->insert( mergeStripItr->end(), minBinsItr->begin(), minBinsItr->end() );
	minBinsItr->clear();
	begining = mergeStripItr;
      }
      else if( minBinsItr == ending ) {
	//Merge left
	std::vector<std::vector<tools::binContainer> >::iterator mergeStripItr = minBinsItr;
	--mergeStripItr;
	while( mergeStripItr->empty() ) {
	  --mergeStripItr;
	  if( mergeStripItr == begining ) break;
	}
	mergeStripItr->insert( mergeStripItr->end(), minBinsItr->begin(), minBinsItr->end() );
	minBinsItr->clear();
	ending = mergeStripItr;
      }
      else {
	//Merge with the smallest of nearest neighbors:
	std::vector<std::vector<tools::binContainer> >::iterator mergeLeftItr = minBinsItr;
	std::vector<std::vector<tools::binContainer> >::iterator mergeRightItr = minBinsItr;
	--mergeLeftItr;
	++mergeRightItr;
	while( mergeLeftItr->empty() && mergeRightItr->empty() ) {
	  --mergeLeftItr;
	  ++mergeRightItr;
	}
	if( mergeLeftItr->empty() ) {
	  mergeRightItr->insert( mergeRightItr->end(), minBinsItr->begin(), minBinsItr->end() );
	  minBinsItr->clear();
	}
	else if( mergeRightItr->empty() ) {
	  mergeLeftItr->insert( mergeLeftItr->end(), minBinsItr->begin(), minBinsItr->end() );
	  minBinsItr->clear();
	}
	else {
	  if( mergeLeftItr->size() < mergeRightItr->size() ) {
	    mergeLeftItr->insert( mergeLeftItr->end(), minBinsItr->begin(), minBinsItr->end() );
	    minBinsItr->clear();
	  }
	  else {
	    mergeRightItr->insert( mergeRightItr->end(), minBinsItr->begin(), minBinsItr->end() );
	    minBinsItr->clear();
	  }
	}
      }
    }
    else {
      break;
    }
  }
}

void
dqm_algorithms::tools::MakeBinTag( const binContainer& bin, std::string & tag ) {
  if(bin.error < 0) {
    return;
  }
  tag += "(eta,phi)[err]= (";
  FormatToSize(bin.x,6,tag,true);
  tag += ",";
  FormatToSize(bin.y,6,tag,true);
  tag += ")[";
  FormatToSize(bin.error,5,tag,false);
  tag += "]";
}

void
dqm_algorithms::tools::FormatToSize( double value, int size, std::string & str, bool showSign) {
  //Method to append size characters representing value to string str

  char temp[35];

  if( value == 0 ) {
    int prec = size - 2;
    if(prec < 0) prec = 0;
    sprintf(temp,"%.*f",prec,value);
    str += temp;
    return;
  }

  std::string format;
  if(showSign) {
    format = "%+0*.*";
  }
  else {
    format = "%0*.*";
  }
  
  int order = (int) floor( log10(fabs(value)) );

  if( (size > 34) || (size < 1) || ((size < 5) && (order > size)) ) {
    size = 5;
  }
  
  int prec = 0;
  if( (value > 0) && !showSign ) {
    if( ( (abs(order) > (size-3)) || (order < -2) ) && (size > 4) ) {
      prec = size-6;
      if(prec < 0) prec = 0;
      format += "e";
    }
    else if( order == -1 ) {
      prec = size - 2;
      if(prec < 0) prec = 0;
      format += "f";
    }
    else {
      prec = size - order -2;
      if(prec > (size - 2) ) prec = size - 2; 
      if(prec < 0) prec = 0;
      format += "f";
    }
  }
  else {
    if( ( (abs(order) > (size-3)) || (order < -2) ) && (size > 4) ) {
      prec = size-7;
      if(prec < 0) prec = 0;
      format += "e";
    }
    else if( order == -1 ) {
      prec = size - 3;
      if(prec < 0) prec = 0;
      format += "f";
    }
    else {
      prec = size - order -3;
      if(prec > (size - 3) ) prec = size - 3; 
      if(prec < 0) prec = 0;
      format += "f";
    }
  }

  sprintf(temp,format.c_str(),size,prec,value);

  str += temp;
}

#endif // #ifndef DQM_ALGORITHMS_TOOLS_ALGORITHMHELPER_CXX
