/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// Local
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/RobService.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/AlgCorr.h"

using namespace std;

//--------------------------------------------------------------------------------------      
// Start AlgCorr class
//
Anp::AlgCorr::AlgCorr()
  :fDir(0),
   fDebug(true),
   fUseType(false),
   fRunCorr(false),
   fLevel(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::AlgCorr::~AlgCorr()
{
}

//--------------------------------------------------------------------------------------      
void Anp::AlgCorr::Config(const Registry &reg)
{

  if(!fIsConfig) {
    fIsConfig = true;
  }
  else {
    cout << "AlgCorr::Config - error: executing second time" << endl;    
  }

  reg.Get("AlgCorr", "Debug",      fDebug);
  reg.Get("AlgCorr::RunCorr",      fRunCorr);

  reg.Get("TrigTimeAlg::UseType",      fUseType);
}

//--------------------------------------------------------------------------------------      
void Anp::AlgCorr::Config(const TrigConfAlg &alg)
{
  //
  // Configure self
  //
  if(!fAlgVec.empty() && GetTypeId() != alg.getTypeId()) {
    cerr << "AlgCorr::Config - error! Wrong algorithm type: " << alg.getType() << endl;
    return;
  }

  fAlgVec.push_back(alg);
  fIds.insert(alg.getNameId());
}

//--------------------------------------------------------------------------------------      
void Anp::AlgCorr::AddAlgsRunOnRoi(const std::vector<uint32_t> &algIds, 
				       const TrigMonAlg &alg, 
				       const std::vector<std::string> &algnames)
{
  for(unsigned id = 0;id<algIds.size(); ++id){
    CpuAlgMap::iterator algmap = fCpuAlgMap.find(algIds.at(id));
    if(algmap != fCpuAlgMap.end()){
      //
      // add in the alg time
      //
      ++algmap->second.nCalls;
      if(alg.isCalled()) algmap->second.avgTimeCalls += (alg.getTimer()-algmap->second.avgTimeCalls)/static_cast<float>(algmap->second.nCalls);
      algmap->second.avgTimeAll += (alg.getTimer()-algmap->second.avgTimeAll)/static_cast<float>(algmap->second.nCalls);
    //
    // add in the rob request time
    //
      
    }
    else{
      //
      // add algorithms on demand
      //
      AlgCorrelation algcorr;
      if(id < algnames.size()) algcorr.name = algnames.at(id);
      ++algcorr.nCalls;
      algcorr.avgTimeAll = alg.getTimer();
      if(alg.isCalled()) algcorr.avgTimeCalls = alg.getTimer();
      fCpuAlgMap.insert(CpuAlgMap::value_type(algIds.at(id), algcorr));
    }
  }
}

//--------------------------------------------------------------------------------------      
unsigned Anp::AlgCorr::GetNameId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getNameId();
  return 0;
}

//--------------------------------------------------------------------------------------      
unsigned Anp::AlgCorr::GetTypeId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getTypeId();
  return 0;
}

//--------------------------------------------------------------------------------------      
const std::string Anp::AlgCorr::GetName() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) { 
    if(fUseType) return fAlgVec.front().getType();
    else         return fAlgVec.front().getName();
  }

  return "UNKNOWN";
}

//---------------------------------------------------------------------------------------
void Anp::AlgCorr::WriteHtml(Handle<HtmlNode> node, TCanvas */*canvas*/) 
{
  //
  // Write html summary
  //
  if(!node.valid()) { 
    return;
  }
  
  ostream &os = node -> GetCont();
  if(fRunCorr){
    os << "<script src=\"sorttable.js\"></script>" << endl
       << "<p style=\"font-family:times; font-size:11px;\">" << endl
       << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
       << "<tr>"
       << "<th> Algorithm </th>"
       << "<th> Avg Time (ms) </th>"
       << "<th> Avg Time for Calls (ms) </th>"
       << "<th> Avg Rob Req Time (ms) </th>"
       << "<th> Percent of Cached Requests (%) </th>"
       << "</tr>" << endl;
    
    for(CpuAlgMap::iterator algmap = fCpuAlgMap.begin(); algmap != fCpuAlgMap.end(); ++algmap){
      float cached_frac = 0.0;
      if(algmap->second.robCalledReq+algmap->second.robCachedReq != 0) cached_frac = static_cast<float>(algmap->second.robCachedReq)/static_cast<float>(algmap->second.robCalledReq+algmap->second.robCachedReq);
      const string cached_fracS   = Anp::Round2Pair(cached_frac, 0.1).first;
      os <<  "<tr>" 
	 << "<td align=\"left\"> "   << algmap->second.name            << " </td>"
	 << "<td align=\"center\"> " << algmap->second.avgTimeAll      << " </td>"
	 << "<td align=\"center\"> " << algmap->second.avgTimeCalls    << " </td>"
	 << "<td align=\"center\"> " << algmap->second.avgRobTime      << " </td>"
	 << "<td align=\"center\"> " << cached_fracS                   << " </td>"
	 << "</tr>" << endl;
    }
    os << "</table>" << endl;
  }
}


