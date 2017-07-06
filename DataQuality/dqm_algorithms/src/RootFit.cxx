/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file RootFit.cxx does one of the basic fits implemented by ROOT and returns dqm_core::Result
 * \author Haleh Hadavand
 * \author Cristobal Cuenca Almenar, Novermber 29th 2009
 * \author of Fermi function Matt King and Akimasa Ishikawa, 8th Mar 2010
 */

#include <dqm_algorithms/RootFit.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TClass.h>
#include <ers/ers.h>
#include <TROOT.h>
#include <memory>
#include <TMath.h>
#include <dqm_core/AlgorithmManager.h>


namespace
{
  dqm_algorithms::RootFit gaussian_fit( "gaus" );
  dqm_algorithms::RootFit linear_fit( "pol1" );
  dqm_algorithms::RootFit landau_fit( "landau" );
  dqm_algorithms::RootFit sinusoid_fit( "sinusoid" );
  dqm_algorithms::RootFit gauspol1_fit( "gauspluspol1" );
  dqm_algorithms::RootFit doublegaussian_fit( "doublegaus" );
  dqm_algorithms::RootFit gausplusexpo_fit( "gausplusexpo" );
  dqm_algorithms::RootFit fermi_fit( "fermi" );
  dqm_algorithms::RootFit flat_fit( "flat" ); 
}

dqm_algorithms::RootFit::RootFit( const std::string & name )
  : m_name( name )
{ 

  if (m_name == "gaus"){
    m_func.reset( new TF1("gaus1","gaus") );
  }
  if (m_name == "pol1"){
    m_func.reset( new TF1("pol11","pol1") );
  }
  if (m_name == "landau"){
    m_func.reset( new TF1("landau1","landau") );
  }
  if (m_name == "sinusoid"){
    m_func.reset( new TF1("sinusoid","[0]*sin(x) + [1]*cos(x)") );
  }
  if (m_name == "gauspluspol1"){
    m_func.reset( new TF1("gauspluspol1","gaus(0)+pol1(3)") );
  }
  if (m_name == "doublegaus"){
    m_func.reset( new TF1("doublegaus","gaus(0)+gaus(3)",0,1) );
  }
  if (m_name == "gausplusexpo"){
    m_func.reset( new TF1("gausplusexpo","gaus(0)+expo(3)",0,1) );
  }
  if (m_name == "fermi"){
    m_func.reset( new TF1("fermi","[0]/(1+exp(([1]-x)/[2]))") );
  }
  if (m_name == "flat"){
    m_func.reset( new TF1("flat","[0]") );
  }
  dqm_core::AlgorithmManager::instance().registerAlgorithm( "Simple_"+name +"_Fit", this );
}

dqm_algorithms::RootFit::~RootFit()
{
  // totally defeats the purpose of auto_ptr, but fixes a segfault in 5.34 ...
  m_func.release();
}

dqm_algorithms::RootFit * 
dqm_algorithms::RootFit::clone()
{
  return new RootFit( m_name );
}


dqm_core::Result *
dqm_algorithms::RootFit::execute(	const std::string & name, 
					const TObject & object, 
					const dqm_core::AlgorithmConfig & config )
{  
  //std::cout<<"ROOTFIT = calling rootfit with name "<<name<<std::endl;
  TH1 * histogram;
  if(object.IsA()->InheritsFrom( "TH1" ))
    {  
      histogram = (TH1*)&object;
      if (histogram->GetDimension() > 1 ){ 
        throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 1 for Fit" );
      }
      //if (histogram->GetEffectiveEntries()==0 ){
	//throw dqm_core::BadConfig( ERS_HERE, name, "Histogram is empty: No Fit performed" );
      //}
      
    }
  else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  //std::cout<<"ROOTFIT = Trying to get parameters"<<std::endl;
  double xmin = dqm_algorithms::tools::GetFirstFromMap( "xmin", config.getParameters(), histogram->GetXaxis()->GetXmin());
  double xmax = dqm_algorithms::tools::GetFirstFromMap( "xmax", config.getParameters(), histogram->GetXaxis()->GetXmax());
  const bool ignoreFirstLastBin = dqm_algorithms::tools::GetFirstFromMap( "ignoreFirstLastBin", config.getParameters(), 0 );

  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  const bool verbose = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "Verbose", config.getParameters(), 0));

  const double minSig = dqm_algorithms::tools::GetFirstFromMap( "MinSignificance", config.getParameters(), 0);

  // const bool draw = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap( "DrawFitCurve", config.getParameters(), 0));
  const double lf   = dqm_algorithms::tools::GetFirstFromMap( "LikelihoodFit", config.getParameters(), 0);

  //std::cout << "verbose " << verbose 
  //<< " draw " << draw  
  //<< " lf " << lf << std::endl;

  if (histogram->GetEffectiveEntries() < minstat || histogram->GetEffectiveEntries()==0) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEffectiveEntries"] = histogram->GetEffectiveEntries();
    return result;
  }
  
  TAxis *x = histogram->GetXaxis();
  int nbins = x->GetNbins();
  double high = x->GetBinUpEdge(nbins);
  double low = x->GetBinUpEdge(0);

  if ( xmin>high || xmin<low || xmax>high || xmax<low) {
    throw dqm_core::BadConfig( ERS_HERE, name, "xmin and/or xmax value not in histogram bin range" );
  }
	 
  std::string option;
  //Always set option end to avoid making graphics object and drawing it.
  //draw fit curve if DrawFitCurve == 1.0 
  if (verbose){
    //if( draw ) option = "";
    //else option="N";
    option="N";
  } else {
    //if( draw ) option = "Q";
    //else option = "QN";
    option = "QN";
  }

  // Likelihood fit
  if( lf == 1.0 ) option += "L";
  else if ( lf == 2.0 ) option += "LL";

  //Use Minos technique as recommended for better error calculation, if errors are important:
  if ( minSig != 0 ) {
    option += "E";
  }
  //show histo name and fit option for verbose mode
  if( verbose ){
    std::cout <<" histo name " << histogram->GetName() << std::endl;
    std::cout <<" fit option " << option << std::endl;
  }
  //std::cout<<"ROOTFIT Trying to do fit"<<std::endl;
  if (m_name == "gauspluspol1"){
    m_func->SetParameters(histogram->GetBinContent(histogram->GetMaximumBin()),histogram->GetMean(),histogram->GetRMS());
    m_func->SetParNames ("Constant","Mean","Sigma","pol1[0]","pol1[1]");
  }
  else if (m_name == "pol1"){
    m_func->SetParNames ("pol1[0]","pol1[1]");
  }
  else if (m_name == "sinusoid") {
    m_func->SetParameters(4.0,-1.0);
    m_func->SetParNames("s1","s2");
  }
  else if (m_name == "doublegaus"){
    TF1 f1("f1","gaus",xmin,xmax);
    histogram->Fit(&f1,"q");
    double par[6] ;

    f1.GetParameters(par);
    m_func->SetParameters(par);
    m_func->SetParameter(3,histogram->GetBinContent(histogram->GetMaximumBin()));
    m_func->SetParameter(4,histogram->GetMean());
    m_func->SetParameter(5,par[2]);
    m_func->SetParNames("Constant","Mean","Sigma","Constant1","Mean1","Sigma1");
    /*
    const int numsig1 = m_func->GetParNumber("Sigma1");
    const double sigmaup = dqm_algorithms::tools::GetFirstFromMap( "Sigma_upperLimit", config.getParameters(), 1000000);
    m_func->SetParLimits(numsig1, 0., sigmaup);
    */
  }
  else if (m_name == "gausplusexpo") {
    m_func->SetParameters(histogram->GetBinContent(histogram->GetMaximumBin()),histogram->GetMean(),histogram->GetRMS());
    m_func->SetParNames("Constant","Mean","Sigma","ConstantExpo","Slope");
  }
  else if (m_name == "fermi") {
    m_func->SetParameter(0,0.99);
    m_func->SetParameter(1,5);
    m_func->SetParameter(2,1);
    m_func->SetParNames("Plateau","Threshold","Resolution");
  }
  else if(m_name == "flat") {
    if(verbose)std::cout << "set "<<name<< " parameters" << std::endl;
    m_func->SetParNames("Height");
  }
/*
  const int numsig = m_func->GetParNumber("Sigma");

  if (numsig != -1 ){
  	  double sigmaup = dqm_algorithms::tools::GetFirstFromMap( "Sigma_upperLimit", config.getParameters(), 1000000);
	  m_func->SetParLimits(numsig, 0., sigmaup);
  }
  */ 
  

  if(ignoreFirstLastBin) {
    int firstNonEmptyBin=0;
    int lastNonEmptyBin=0;
    for(int i=1 ; i<=nbins ; i++) {
      if(histogram->GetBinContent(i)!=0.0) {
	firstNonEmptyBin=i;
	break;
      } 
    }
    for(int i=nbins ; i>=0 ; i--) {
      if(histogram->GetBinContent(i)!=0.0) {
	lastNonEmptyBin=i;
	break;
      } 
    }
    
    if( lastNonEmptyBin-firstNonEmptyBin>2) { //ensures at least two good non-empty bins
      if ( x->GetBinLowEdge(firstNonEmptyBin+1) > xmin ) {
	xmin=x->GetBinLowEdge(firstNonEmptyBin+1);
      }
      if ( x->GetBinUpEdge(lastNonEmptyBin-1) < xmax ) {
	xmax=x->GetBinUpEdge(lastNonEmptyBin-1);
      }
    }

  }
  histogram->Fit( m_func.get(), option.c_str(),"",xmin,xmax );
  if (m_name == "doublegaus") {
    double par[6];
    m_func->GetParameters(par);
    if (TMath::Abs(par[2]) > TMath::Abs(par[5])) {
      m_func->SetParNames("Constant1","Mean1","Sigma1","Constant","Mean","Sigma");
      double sigma=m_func->GetParameter(2);
      m_func->SetParameter(2,fabs(sigma));
    }
    else {
      m_func->SetParNames("Constant","Mean","Sigma","Constant1","Mean1","Sigma1");
      double sigma=m_func->GetParameter(5);
      m_func->SetParameter(5,fabs(sigma));
    }
  }

  const int numsig = m_func->GetParNumber("Sigma");
  if (numsig != -1 ){
	  double sigma=m_func->GetParameter(numsig);
	  m_func->SetParameter(numsig,fabs(sigma));
  }

  try {
    dqm_core::Result *result= dqm_algorithms::tools::GetFitResult (m_func.get() , config, minSig );
    return result;
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
}

void
dqm_algorithms::RootFit::printDescription(std::ostream& out)
{
  out<<"Simple_"+m_name+"_Fit: Does simple "+m_name+" fit to histogram and checks fit parameters against thresholds\n"<<std::endl;
  if ( m_name == "gaus"){
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Treshold: Mean : Mean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: AbsMean : AbsMean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Simga : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant : Constant fit value to give Green/Red Result\n"<<std::endl;
  } else if ( m_name == "sinusoid"){
    out<<"The sinusoid fit has the following functional form: s1*sin(x) + s2*cos(x)."<<std::endl
	     <<"Checks can be configured on both parameters, s1 and s2, with green and red thresholds:"<<std::endl;
  } else if ( m_name == "doublegaus"){
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"smaller width will be assigned to Sigma"<<std::endl;
    out<<"Green/Red Treshold: Mean : Mean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: AbsMean : AbsMean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Sigma : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant : Constant fit value to give Green/Red Result\n"<<std::endl;
    out<<"Green/Red Treshold: Mean1 : Mean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: AbsMean1 : AbsMean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Simga1 : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant1 : Constant fit value to give Green/Red Result\n"<<std::endl;
  } else if (m_name == "pol1") {
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Treshold: pol1[0] : Constant linear fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: pol1[1] : Slope linear fit value to give Green/Red Result\n"<<std::endl;
  } else if (m_name == "gauspluspol1"){
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Treshold: Mean : Mean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: AbsMean : AbsMean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Sigma : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant : Constant fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: pol1[0] : Constant linear fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: pol1[1] : Slope linear fit value to give Green/Red Result\n"<<std::endl;
  } else if (m_name == "gausplusexpo"){
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Treshold: Mean : Mean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: AbsMean : AbsMean fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Sigma : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant : Constant fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: ConstantExpo : ConstantExpo fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Slope : Slope fit value to give Green/Red Result\n"<<std::endl;
  } else if (m_name == "landau") {
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Treshold: MPV : MPV fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Sigma : Sigma fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Treshold: Constant : Constant fit value to give Green/Red Result\n"<<std::endl;
  } else if (m_name == "fermi" ) {
    out<<"The following fit Parameters can be checked with Red and Green Thresholds; only one parameter is needed to get back real result"<<std::endl;
    out<<"Green/Red Threshold: Plateau : Plateau fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Threshold: Threshold : Fermi energy fit value to give Green/Red Result"<<std::endl;
    out<<"Green/Red Threshold: Resolution : Templature fit value to give Green/Red Result\n"<<std::endl;
  }
  out<<"Optional Parameter: Verbose: Write out fit results to log file (set to 1)"<<std::endl;
  out<<"Optional Parameter: LikelihoodFit: Fit with L or LL option if 1 or 2"<<std::endl;
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: MinSignificance : Minimum multiple of the error in fit paramenter by which the parameter must exceed the thresholds"<<std::endl;
  out<<"Optional Parameter: xmin: minimum x range"<<std::endl;
  out<<"Optional Parameter: xmax: maximum x range"<<std::endl;
  out<<"Optional Parameter: SubtractFromMean: value subtracted from XMean before test is applied: allows using AbsXMean for non-zero expected mean"<<std::endl;
  out<<"Optional Parameter: ignoreFirstLastBin: ignores the first and last non-empty bin"<<std::endl; 
//  out<<"Optional Parameter: Sigma_upperLimit: Upper limit on Sigma- lower limit set to 0. and default upper value is 1e^6\n"<<std::endl;
                                                                                                                                            
}

