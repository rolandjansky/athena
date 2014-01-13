/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/ProcessDQ.h"

REGISTER_ANP_OBJECT(AlgEvent,ProcessDQ)

using namespace std;

//-----------------------------------------------------------------------------
// Functions for helper classes
//
bool Anp::LumiRange::MatchLB(int lb) const
{
  return (lb_beg <= lb && lb <= lb_end);
}

//-----------------------------------------------------------------------------
void Anp::RunDQ::Print() const
{
  cout << "   run=" << run 
       << " #bxid=" << bxid.size() << " #lumi=" << lumi.size() << endl;
  
  for(set<unsigned>::const_iterator it = bxid.begin(); it != bxid.end(); ++it) {
    cout << "      bxid=" << *it << endl;    
  }
  
  for(unsigned i = 0; i < lumi.size(); ++i) {
    cout << "      lumi=" << lumi.at(i).lb_beg << "-" << lumi.at(i).lb_end << endl;
  }  
}

//-----------------------------------------------------------------------------
// Member functions for ProcessDQ
//
Anp::ProcessDQ::ProcessDQ()
  :fDebug(false),
   fIsActive(false),
   fIsData(false),
   fSelectBX(true),
   fSelectLB(true),
   fCountPass(0),
   fCountFail(0),
   fCountFailBX(0),
   fCountFailLB(0),
   fCountFailRun(0),
   fCountTotal(0)
{
}

//-----------------------------------------------------------------------------
Anp::ProcessDQ::~ProcessDQ()
{
}

//-----------------------------------------------------------------------------
void Anp::ProcessDQ::Config(const Registry &reg)
{
  reg.Get("ProcessDQ", "Debug",  fDebug);
  reg.Get("ProcessDQ", "IsData", fIsData);

  reg.Get("ProcessDQ::FileXML",  fFileXML);
  reg.Get("ProcessDQ::SelectLB", fSelectLB);
  reg.Get("ProcessDQ::SelectBX", fSelectBX);

  reg.Get("ProcessDQ::useSubset",fuseSubset);

  //
  // Book tree branches before Init()
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Register self for html creation
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));

  //
  // Read data quality info from XML file
  //
  if(fIsData && !fFileXML.empty()) {

    if(Anp::file_size(fFileXML) > 1.0e-6) {
      log() << "Config - reading DQ from: " << fFileXML << endl;

      ReadXML(fFileXML);
      
      if(!fLumi.empty()) {
	fIsActive = true;
	log() << "   DQ selection is active using " << fLumi.size() << " run(s)" << endl
	      << "   SelectLB = " << fSelectLB << endl
	      << "   SelectBX = " << fSelectBX << endl;
      }
    }
    else {
      log() << "Config - file does not exists: \"" << fFileXML << "\"" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::ProcessDQ::Init()
{
  return true;
}

//-----------------------------------------------------------------------------
void Anp::ProcessDQ::Exec()
{
  //
  // Read new event entry
  //
  if(fIsActive && 
     fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    
    TrigMonEvent &event = fEntry.GetRef();

    if(Run(event)) { 
      ++fCountPass;      
    }
    else {
      ++fCountFail;
      fEntry.SetEvent(Ntuple::kFAIL);
    }
    
    ++fCountTotal;
  }
}

//-----------------------------------------------------------------------------
void Anp::ProcessDQ::Done()
{
  //
  // Fill summary data for luminosity blocks
  //
  if(!fIsActive) return;

  log() << "Done" << endl
	<< "   Count      total events = " << fCountTotal   << endl
	<< "   Count     passed events = " << fCountPass    << endl
	<< "   Count     failed events = " << fCountFail    << endl
	<< "   Count run failed events = " << fCountFailRun << endl
	<< "   Count BX  failed events = " << fCountFailBX  << endl
	<< "   Count LB  failed events = " << fCountFailLB  << endl;
}

//-----------------------------------------------------------------------------
bool Anp::ProcessDQ::Run(const TrigMonEvent &event)
{
  //
  // Process one event and collect lumi data
  //
  if(!fSelectLB && !fSelectBX) return true;
  
  LumiMap::const_iterator lit = fLumi.find(event.getRun());
  if(lit == fLumi.end()) {
    ++fCountFailRun;
    return false;
  }

  const RunDQ &run = lit->second;
  
  bool passLB = false;
  bool passBX = false;

  if(fSelectLB) {
    for(unsigned i = 0; i < run.lumi.size(); ++i) {
      if(run.lumi.at(i).MatchLB(event.getLumi())) {
	passLB = true;
	break;
      }
    }
  }
  else {
    passLB = true;
  }

  if(fSelectBX) {
    passBX = run.bxid.count(event.getBunchId()) > 0;
  }
  else {
    passBX = true;
  }

  if(!passLB) ++fCountFailLB;
  if(!passBX) ++fCountFailBX;

  return (passLB && passBX);
}

//-----------------------------------------------------------------------------
void Anp::ProcessDQ::Write(HtmlNode &, HtmlMenu &)
{
  //
  // Write out summary of runs and lumi blocks
  //
}

//-----------------------------------------------------------------------------
void Anp::ProcessDQ::ReadXML(const std::string &path)
{
  //
  // Read DQ info from XML file
  //
  if(path.empty()) {
    log() << "ReadXML - empty nput file" << endl;
    return;
  }
  
  if(fDebug) {
    log() << "ReadXML - will now parse XML file... " << endl;
  }

  Anp::Registry xml_reg = Anp::ParseXml(path, fDebug);
  
  string root_name;
  if(!xml_reg.Get("RootElement", root_name) || root_name != "LumiRangeCollection") {
    log() << "ReadXML - wrong root element name: " << root_name << endl;
    return;    
  }
  
  if(fDebug) {
    log() << "ReadXML - will now parse Registry... " << endl;
  }

  while(xml_reg.KeyExists("NamedLumiRange")) {
    //
    // Read one lumi range (run)
    //
    Registry range_reg;
    xml_reg.Get("NamedLumiRange", range_reg);
    xml_reg.RemoveKey("NamedLumiRange");
    
    while(range_reg.KeyExists("LumiBlockCollection")) {
      //
      // Read one lumi collection
      //
      Registry lumi_reg;
      range_reg.Get("LumiBlockCollection", lumi_reg);
      range_reg.RemoveKey("LumiBlockCollection");

      //
      // Fill data
      //
      RunDQ runDQ;      
      if(!lumi_reg.Get("Run", runDQ.run)) continue;
      
      while(lumi_reg.KeyExists("BunchId")) {
	//
	// Read one bunch id
	//
	unsigned bunch_id = 0;
	
	if(lumi_reg.Get("BunchId", bunch_id)) {
	  runDQ.bxid.insert(bunch_id);
	}

	lumi_reg.RemoveKey("BunchId");
      }
      
      while(lumi_reg.KeyExists("LBRange")) {
	//
	// Read one lumi range
	//
	Registry lb_reg;
	lumi_reg.Get("LBRange", lb_reg);
	lumi_reg.RemoveKey("LBRange");
	
	LumiRange range;	
	if(lb_reg.Get("Start", range.lb_beg) &&
	   lb_reg.Get("End",   range.lb_end)) {	  
	  runDQ.lumi.push_back(range);
	}
      }

      log() << "Number of lines used in DQ file before useSubset scale:" << runDQ.lumi.size() << endl;

      if (fuseSubset < 0 || fuseSubset > 1) fuseSubset=1;

      int numDelete = floor((1-fuseSubset)*runDQ.lumi.size());

      for (int i=0;i<numDelete;i++) runDQ.lumi.pop_back();

      log() << "Number of lines used in DQ file after useSubset scale:" << runDQ.lumi.size() << endl;

      if(!fLumi.insert(LumiMap::value_type(runDQ.run, runDQ)).second) {
	log() << "ReadXML - duplicate entry for run: " << runDQ.run << endl;
      }
      
      if(fDebug) {
	runDQ.Print();
      }
    }
  }
}
