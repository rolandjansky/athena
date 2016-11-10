/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define METCONFIGINST
#include "MissingETGoodness/EtmissGoodnessConfig.h"
#include "CxxUtils/unused.h"


/*-------------------------------------------------------------------------
 * this function sets up the Etmiss Goodness configuration map
 *-------------------------------------------------------------------------*/
bool
MET::InitConfigMap()
{
  Bool_t status(kTRUE);

  MET::ConfigMap.clear();
  std::vector<TString> parList;

  // select on "all" parameters
  parList.clear();
  MET::ConfigMap["All"] = parList;

  // add your list of favorite parameters here
  // ...

/*
  // Cleaning set for December'09 data
  parList.clear();
  parList.push_back("N90Cells_Jet0");
  parList.push_back("N90Cells_Jet1");
  parList.push_back("fcor_Jet0");
  parList.push_back("fcor_Jet1");
  parList.push_back("SamplingMaxID_Jet0");
  parList.push_back("SamplingMaxID_Jet1");
  MET::ConfigMap["JetCleaning"] = parList;
*/

  // April'10 jet cleaning selections
  parList.clear();
  parList.push_back("N90Cells_Jet0");  
  parList.push_back("N90Cells_Jet1");
  parList.push_back("EMFraction_Jet0");
  parList.push_back("EMFraction_Jet1");
  parList.push_back("MET_Goodness_QualityFrac_Jet0");
  parList.push_back("MET_Goodness_QualityFrac_Jet1");
  parList.push_back("SamplingMaxID_Jet0");
  parList.push_back("SamplingMaxID_Jet1");
  parList.push_back("MET_Goodness_Timing_Jet0");
  parList.push_back("MET_Goodness_Timing_Jet1");
  MET::ConfigMap["JetCleaning"] = parList;

/*
  // example 1: selection "Muon", with list of specific parameters
  parList.clear();
  parList.push_back("parname1");
  parList.push_back("parname2");
  // etc ...
  // and store the parameter list under name "Muon"
  MET::ConfigMap["Muon"] = parList;

  // example 2: selection "Top", with composite list of specific parameters
  parList.clear();
  MET::AddExistingConfigTo(parList,"Muon");
  MET::AddExistingConfigTo(parList,"All"); // note: empty set!
  parList.push_back("parname3");
  // etc ...
  // and store the parameter list under name "Top"
  MET::ConfigMap["Top"] = parList;
*/

  return status;    
}


/*-------------------------------------------------------------------------
 * this fills the selection map when the library is loaded.
 *-------------------------------------------------------------------------*/
namespace {
  const bool UNUSED(dummy) = MET::InitConfigMap();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------
 * Helper functions
 *-------------------------------------------------------------------------*/

// get list of observables corresponding to qualifier
std::vector<TString>
MET::GetEtmissGoodnessConfig(const TString& qualifier)
{
  std::vector<TString> parList; // equivalent to qualifier="all"
  std::map< TString, std::vector<TString> >::iterator itr = ConfigMap.find(qualifier);
  if ( itr != ConfigMap.end() ) { parList=ConfigMap[qualifier]; }
  else {
    MET::TMsgLogger logger("EtmissGoodnessConfig");
    logger << kWARNING << "Configuration list <" << qualifier << "> not found. Return empty list." << GEndl;
  }
  return parList;
}


// helper function, add existing list of observables to new set of observables
void
MET::AddExistingConfigTo(std::vector<TString>& parList, const TString& qualifier)
{
  std::vector<TString> existingList = MET::GetEtmissGoodnessConfig(qualifier);
  std::vector<TString>::iterator itr = existingList.begin();
  for (; itr!=existingList.end(); ++itr ) { parList.push_back(*itr); }
}


// selection summary
void
MET::SummaryEtmissGoodnessConfig(const TString& qualifier)
{
  MET::TMsgLogger logger("EtmissGoodnessConfig");

  if (qualifier.IsNull()) {
    logger << kINFO << "Number of configured parameter lists : " << MET::ConfigMap.size() << GEndl;
    std::map< TString, std::vector<TString> >::iterator itr = MET::ConfigMap.begin();
    for (; itr!=MET::ConfigMap.end(); ++itr)
      logger << kINFO << "o) \"" << itr->first << "\" with " << itr->second.size() << " parameters to cut on." << GEndl;
  } else {
    std::vector<TString> parList = MET::GetEtmissGoodnessConfig(qualifier);
    logger << kINFO << "List <" << qualifier << "> contains " << parList.size() << " parameters." << GEndl;
    std::vector<TString>::iterator itr = parList.begin();
    logger << kINFO << "o) ";
    for (; itr!=parList.end(); ++itr ) { logger << *itr << " "; }
    logger << GEndl;
  }
}


Bool_t 
MET::ContainsEtmissGoodnessQualifier(const TString& qualifier)
{
  return ( MET::ConfigMap.find(qualifier)!=MET::ConfigMap.end() );
}


std::vector<TString> 
MET::GetEtmissGoodnessQualifierList()
{
  std::vector<TString> qualList;
  std::map< TString, std::vector<TString> >::iterator itr = MET::ConfigMap.begin();
  for (; itr!=MET::ConfigMap.end(); ++itr) qualList.push_back(itr->first);
  return qualList;
}


