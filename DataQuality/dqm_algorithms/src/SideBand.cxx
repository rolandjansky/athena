/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SideBand.h file implements the dqm_algorithms::SideBand base class.
 * \author Andrea Dotti
*/

#include <dqm_algorithms/SideBand.h>
#include <TH1.h>
#include <TClass.h>
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/Result.h>
#include <ers/ers.h>
#include <string>
#include <sstream>
#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::SideBand Absolute( "SideBand_Absolute" );
  dqm_algorithms::SideBand Relative( "SideBand_Relative" );
}

dqm_algorithms::SideBand::SideBand(const std::string & name) : m_name(name) {
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name,this);
}

dqm_algorithms::SideBand* dqm_algorithms::SideBand::clone() {
  return new SideBand( m_name );
}

dqm_core::Result* dqm_algorithms::SideBand::execute(const std::string & name ,
						    const TObject & obj, 
						    const dqm_core::AlgorithmConfig & config )
{
  TH1* histo;
  if ( obj.IsA()->InheritsFrom("TH2") || obj.IsA()->InheritsFrom("TH3") )
    {
      throw dqm_core::BadConfig( ERS_HERE , name , " dimension > 1 ");
    }
  if ( obj.IsA()->InheritsFrom("TH1") )
    {
      ERS_DEBUG(2,"Got TH1 called: "<<obj.GetName()<<" of type:"<<obj.IsA()->GetName());
      histo=(TH1*)&obj;
    }
  else
    {
      throw dqm_core::BadConfig( ERS_HERE ,name , " does not inherit from TH1");
    }
  // Configure the DQ algorithm
  const bool UseUnderFlow = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("UseUnderFlow", config.getParameters(), 0));
  const bool UseOverFlow  = static_cast<bool>(dqm_algorithms::tools::GetFirstFromMap("UseOverFlow",  config.getParameters(), 0));
  // the following is a duplicate of dqm_algorithms::tools::GetBinRange, but with different parameter names
  const double notFound = -99999;
  const double xmin = dqm_algorithms::tools::GetFirstFromMap("Min", config.getParameters(), notFound);
  const double xmax = dqm_algorithms::tools::GetFirstFromMap("Max", config.getParameters(), notFound);
  const int minbin = (xmin == notFound) ? 1                  : histo->GetXaxis()->FindBin(xmin);
  const int maxbin = (xmax == notFound) ? histo->GetNbinsX() : histo->GetXaxis()->FindBin(xmax);

  double grThr, reThr; // Green and Red thresholds
  try
    {
      grThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getGreenThresholds() );
      reThr = dqm_algorithms::tools::GetFromMap("Threshold",config.getRedThresholds() );
    }
  catch ( dqm_core::Exception & ex ) 
    {
      throw dqm_core::BadConfig(ERS_HERE,name,"Paramter: 'Threshold' is mandatory, cannot continue");
    }

  if ( m_name== "SideBand_Relative" && (grThr>1.0 || reThr>1.0) ) 
    //non sense case: compare fraction and threshold >100%
    {
      throw dqm_core::BadConfig(ERS_HERE,m_name,"Configuration Error: Threshold>100%");
    }
#if DEBUG_LEVEL > 1
  std::stringstream configuration;
  configuration
      << " - UseUnderFlow = " << UseUnderFlow << " - UseOverFlow  = " << UseOverFlow
      << " - Min = " << xmin << " - Max = " << xmax
      << " - Green Threshold = " << grThr << " - Red Threshold = " << reThr;
  ERS_DEBUG(2,"Configuration:"<<configuration.str());
#endif
  // Just a translation in something more readable...
  const bool AbsoluteValue = ( m_name == "SideBand_Absolute" );
  Double_t tot = histo->Integral( 1, histo->GetNbinsX() );
  Double_t inside = histo->Integral( minbin , maxbin );
  if ( UseUnderFlow ) tot+=histo->GetBinContent(0);
  if ( UseOverFlow ) tot+=histo->GetBinContent(histo->GetNbinsX()+1);
  Double_t sideband = tot-inside;
  if ( !AbsoluteValue ) { // Compare fraction of events in sideband
    //    if (tot != 0) {
    if (tot > 0. || tot < 0.) {// should be safer than != 
      sideband /= tot; 
    }
  }
  ERS_DEBUG(2,"Total:"<<tot<<" SideBand:"<<sideband);
  dqm_core::Result * result = new dqm_core::Result;
  result->tags_.insert(std::make_pair("TotalIntegral",tot));
  result->tags_.insert(std::make_pair("SideBands",sideband));
  
  
  if (reThr> grThr) {
  if ( sideband>reThr ){
      ERS_DEBUG(1,"[RED] Result : "<<sideband);
      result->status_=dqm_core::Result::Red;
      return result;
    } else if ( sideband > grThr ){
      ERS_DEBUG(1,"[YELLOW] Result : "<<sideband);
      result->status_=dqm_core::Result::Yellow;
      return result;
    }
  }else {
  if ( sideband < reThr ){
      ERS_DEBUG(1,"[RED] Result : "<<sideband);
      result->status_=dqm_core::Result::Red;
      return result;
    } else if ( sideband < grThr ){
      ERS_DEBUG(1,"[YELLOW] Result : "<<sideband);
      result->status_=dqm_core::Result::Yellow;
      return result;
    }
 }

  ERS_DEBUG(1,"[GREEN] Result");
  result->status_=dqm_core::Result::Green;
  return result;
}

void dqm_algorithms::SideBand::printDescription(std::ostream& out){
  std::stringstream msg;
  msg<<m_name<<": Checks the integral of a histogram outside a specified range ";
  if ( m_name=="SideBand_Absolute" )
    msg<<" using an absolute threshold.";
  else
    msg<<" using a relative threshold.";
  msg<<"\n\n";
  out << msg.str() <<
      "Parameter: UseUnderFlow: if != 0 include also underflow bin\n"
      "Parameter: UseOverFlow: if != 0 include also overflow bin\n"
      "Parameter: Min: Specify Lower limit (unset means full range)\n"
      "Parameter: Max: Specify Upper limit (unset means full range)\n"
      "Threshold: Threshold: How large or small the sidebands should be.\n"
      "    The comparison type (greater-than or less-than) depends on\n"
      "    the relative order of the green and red threshold value." << std::endl;
}
