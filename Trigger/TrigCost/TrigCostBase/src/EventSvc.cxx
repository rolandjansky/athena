/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cstring>

// Local
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/EventSvc.h"

//------------------------------------------------------------------------------------------
// return static instance
//------------------------------------------------------------------------------------------
Anp::EventSvc& Anp::EventSvc::Instance()
{
  static Anp::EventSvc svc;
  return svc;
}

using namespace std;

//-----------------------------------------------------------------------------
// Code for helper class
// 
bool Anp::EventSvc::DSInfo::Match(const std::string &key) const
{
  //
  // Match ket to dataset using name
  //
  if(!name.empty() && (key.find(name) != string::npos || name == key)) {
    return true;
  }

  return false;
}

namespace Anp
{
  inline bool operator==(EventSvc::DSInfo *info, const std::string &key) {
    return (info && info->Match(key));
  }
  inline bool operator==(const std::string &key, EventSvc::DSInfo *info) {
    return (info && info->Match(key));
  }
}

//-----------------------------------------------------------------------------
// Code for EventSvc class
//
Anp::EventSvc::EventSvc() 
  :fCurrWeight(1.0),
   fTotalWeight(0.0),
   fNEvents(0.0),
   fTotalXsec(-1.0),
   fTotalLumi(-1.0),
   fDebug(false),
   fWarning(false),
   fValid(false),
   fCurrDS(0),
   fNormDS(0)
{
}

//-----------------------------------------------------------------------------
Anp::EventSvc::~EventSvc()
{
  for(unsigned i = 0; i < fInfoVec.size(); ++i) {
    DSInfo *ds = fInfoVec.at(i);
    delete ds;
  }
}

//-----------------------------------------------------------------------------
void Anp::EventSvc::SetDataset(const std::string &ds)
{   
  //
  // Set current dataset
  //
  if(!fValid || !(fTotalXsec > 0.0)) {
    if(fDebug) { 
      cout << "EventSvc::SetDataset - no dataset data available" << endl;
    }
    return;
  }
  
  //
  // Check if current dataset has changed
  // 
  if(fCurrDS && fCurrDS->Match(ds)) {
    return;
  }

  //
  // Match dataset
  //  
  fCurrWeight = 1.0;

  const std::vector<DSInfo *>::const_iterator dit = std::find(fActvVec.begin(),
							      fActvVec.end(), ds);
  if(dit == fActvVec.end()) {
    if(fSkipSet.insert(ds).second) {
      cout << "EventSvc::SetDataset - unknown active dataset: " << ds << endl;
    }
    return;
  }
  
  fCurrDS = *dit;
  
  if(fCurrDS->nevent > 0 && fTotalWeight > 0.0) {
    fCurrWeight = (fTotalWeight*fCurrDS->xsec)/(fTotalXsec*fCurrDS->nevent);
    fNEvents += fCurrWeight*fCurrDS->nevent;

    if(fDebug) {       
      cout << "EventSvc::SetDataset - computed weight = " << fCurrWeight << endl;
    }
  }

  cout << "EventSvc::SetDataset - new weight: " << fCurrWeight << " for " << ds << endl;

}

//-----------------------------------------------------------------------------
void Anp::EventSvc::AddDataset(const std::string &ds, long nevent)
{ 
  //
  // Count number of events in dataset
  //
  std::vector<DSInfo *>::iterator dit = std::find(fInfoVec.begin(), fInfoVec.end(), ds);
  if(dit == fInfoVec.end()) {
    if(fDebug) {
      cerr << "EventSvc::AddDataset - uknown dataset: " << ds << endl;
    }
    return;
  }

  DSInfo *info = *dit;
  info->nevent += nevent;
  
  vector<DSInfo *>::iterator sit = std::find(fActvVec.begin(), fActvVec.end(), ds);
  if(sit == fActvVec.end()) {
    if(fDebug) {
      cout << "EventSvc::AddDataset - adding active dataset: " << ds << endl;
    }

    fActvVec.push_back(info);
  }

  if(fDebug) {
    cout << "EventSvc::AddDataset - added/total " << nevent << "/" << info->nevent
	 << " event(s) from: " << ds << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::EventSvc::Config(const Registry &reg)
{
  //
  // Initialize service
  //
  string xmlFile, normDS;

  reg.Get("EventSvc", "Debug",   fDebug); 
  reg.Get("EventSvc::NormXML" ,  xmlFile);
  reg.Get("EventSvc::NormDS" ,   normDS);
  reg.Get("EventSvc::TotalXsec", fTotalXsec);
  reg.Get("EventSvc::TotalLumi", fTotalLumi);
  reg.Get("EventSvc::Datasets",  fDatasets);

  ParseXML(xmlFile, normDS);

  cout << "EventSvc::Config - using " << fDatasets.size() << " input dataset(s)" << endl;
  for(unsigned i = 0; i < fDatasets.size(); ++i) {
    cout << "   " << i+1 << ": " << fDatasets.at(i) << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::EventSvc::Init()
{
  //
  // Determine total cross section for normalization
  //
  if(fNormDS) {
    fTotalXsec = fNormDS->xsec;
  }
  
  if(fTotalLumi > 0.0) {
    fTotalXsec = 1.0;
    fTotalWeight = fTotalLumi;
  }
    
  if(!(fTotalXsec > 0.0)) {
    //
    // Total cross section is sum of dataset cross-sections
    //
    fTotalXsec = 0.0;
    for(unsigned i = 0; i < fActvVec.size(); ++i) {
      DSInfo *ds = fActvVec.at(i);
      fTotalXsec += ds->xsec;
    }
  }
  
  if(!(fTotalXsec > 0.0)) {
    cerr << "EventSvc::Init - invalid total cross-section: " << fTotalXsec << endl;
    return false;
  }

  long nevent = 0;
  if(!(fTotalWeight > 0.0) ) {
    for(unsigned i = 0; i < fActvVec.size(); ++i) {
      DSInfo *ds = fActvVec.at(i);
      nevent += ds->nevent;
      if(ds->xsec > 0.0 && ds->geff > 0.0) {
        const double weight = (ds->nevent*ds->xsec)/(fTotalXsec*ds->geff);
        ds->total_weight += weight;
        fTotalWeight     += weight;
      }
    }
  }

  if(!(fTotalWeight > 0.0)) {
    cerr << "EventSvc::Init - failed to initialize datasets" << endl;
    return false;
  }

  cout << "EventSvc::Init - using " << fActvVec.size() << " active dataset(s)" << endl
       << "   nevent       = " << nevent       << endl
       << "   total weight = " << fTotalWeight << endl
       << "   total xsec   = " << fTotalXsec   << endl
       << "   total lumi   = " << fTotalLumi   << endl;
  
  fValid = true;
  return true;
}

//-----------------------------------------------------------------------------
unsigned Anp::EventSvc::GetDirectoryIndex(const std::string &path) const
{
  //
  // Get dataset index
  //
  for(unsigned i = 0; i < fDatasets.size(); ++i) {
    const string &name = fDatasets.at(i);
    if(name.find(path) != string::npos ||
       path.find(name) != string::npos) {
      return i+1;
    }
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
void Anp::EventSvc::ParseXML(const string &path, const std::string &norm)
{
  //
  // Read XML file with data descriptions
  //  - input is path to XML file
  //
  if(path.empty()) {
    if(fDebug) {
      cout << "EventSvc::ParseXML - empty input file name" << endl;
    }
    return;
  }
  
  if(fDebug) {
    cout << "EventSvc::ParseXML - will now parse XML file... " << endl;
  }

  Anp::Registry xml_reg = Anp::ParseXml(path, fDebug);
  
  if(fDebug) {
    cout << "EventSvc::ParseXML - will now parse Registry... " << endl;
  }

  string root_element;
  xml_reg.Get("RootElement", root_element);
  
  if(root_element != "ntuples") {
    cerr << "EventSvc::ParseXML - incorrect root element=\"" << root_element << "\"" << endl;
    return;
  }
  
  while(xml_reg.KeyExists("dataset")) {
    //
    // Read and then erase new key
    //
    Anp::Registry set_reg;
    xml_reg.Get("dataset", set_reg);
    xml_reg.RemoveKey("dataset");
    
    DSInfo *info = new DSInfo();
    
    if(set_reg.Get("name",           info->name)    && 
       set_reg.Get("path",           info->path)    && 
       set_reg.Get("parent",         info->parent)  && 
       set_reg.Get("cross_section",  info->xsec)    && 
       set_reg.Get("gen_eff",        info->geff)) {
      
      if(fDebug) {
	cout << "  read and parsed new \"dataset\"" << endl;
      }

      const std::vector<DSInfo *>::const_iterator dit = std::find(fInfoVec.begin(),
								  fInfoVec.end(), info->name);
      if(dit == fInfoVec.end()) {
	set_reg.Get("version", info->version);
	set_reg.Get("project", info->project);
	set_reg.Get("menu",    info->menu);      

	fInfoVec.push_back(info);
	fInfoVec.back()->index = fInfoVec.size();

	if(info->Match(norm)) fNormDS = info;
      }
      else {
	cout << "EventSvc::ParseXML - duplicate dataset: " << info->name << endl;
	delete info;
      }
    }
    else {
      if(fDebug) {
	cout << "  failed to parse new \"dataset\"" << endl;
      }
      
      delete info;
    }
  }

  cout << "EventSvc - read " << fInfoVec.size() << " dataset description(s): " << endl;
  for(unsigned i = 0; i < fInfoVec.size(); ++i) {
    cout << "   dataset: " << fInfoVec.at(i)->name << endl;
  }
}
