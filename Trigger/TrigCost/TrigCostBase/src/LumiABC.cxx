/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <ostream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/LumiABC.h"

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::Lumi::LV1Count::LV1Count()
  :ctp_id(0),
   ctp_ps(0),
   count_tbp(0),
   count_tap(0),
   count_tav(0)
{
}

//-----------------------------------------------------------------------------------------
Anp::Lumi::HLTCount::HLTCount()
  :chain_counter(0),
   chain_level(0),
   chain_ps(0),
   chain_pt(0),
   count_after_ps(0),
   count_input(0),
   count_total(0),   
   count_raw(0)
{
}

//-----------------------------------------------------------------------------------------
Anp::Lumi::BlockInfo::BlockInfo()
  :run(0),
   lb(0),
   bcid(0),
   lumiBegTime(0),
   lumiEndTime(0),
   lumiValid(0),
   lumiInstan(0),
   lifetimeFrac(0),
   scaleCorrL1(0),
   scaleCorrL2(0),
   scaleCorrEF(0),
   L1accept(0),
   L2accept(0),
   EFaccept(0)
{
}

//-----------------------------------------------------------------------------------------
double Anp::Lumi::BlockInfo::GetLBDuration() const
{
  //
  // Get lumi block duration in seconds
  //
  if(lumiBegTime < lumiEndTime) {
    return double(lumiEndTime - lumiBegTime)*1.0e-9;
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
std::string Anp::Lumi::BlockInfo::GetBegTimeAsString() const
{
  //
  // LB start time as string
  //
  time_t timeval = static_cast<uint64_t>(static_cast<double>(lumiBegTime)*1.0e-9);
  return ctime(&timeval);
}

//-----------------------------------------------------------------------------------------
std::string Anp::Lumi::BlockInfo::GetEndTimeAsString() const
{
  //
  // LB end time as string
  //
  time_t timeval = static_cast<uint64_t>(static_cast<double>(lumiEndTime)*1.0e-9);
  return ctime(&timeval);
}

//-----------------------------------------------------------------------------------------
const Anp::Lumi::LV1Count* Anp::Lumi::BlockInfo::GetL1Count(unsigned ctp_id) const
{
  for(unsigned i = 0; i < countsL1.size(); ++i) {
    const Ptr<LV1Count> &c = countsL1.at(i);
    if(c->ctp_id == ctp_id) {
      return c.get();
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------------------
const Anp::Lumi::HLTCount* Anp::Lumi::BlockInfo::GetL2Count(unsigned counter) const
{
  for(unsigned i = 0; i < countsL2.size(); ++i) {
    const Ptr<HLTCount> &c = countsL2.at(i);
    if(c->chain_counter == counter) {
      return c.get();
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------------------
const Anp::Lumi::HLTCount* Anp::Lumi::BlockInfo::GetEFCount(unsigned counter) const
{
  for(unsigned i = 0; i < countsEF.size(); ++i) {
    const Ptr<HLTCount> &c = countsEF.at(i);
    if(c->chain_counter == counter) {
      return c.get();
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------------------
void Anp::Lumi::BlockInfo::RemoveNullCountPointers()
{
  //
  // Remove null pointers from count vectors
  //
  std::vector<Ptr<LV1Count> >::iterator itL1 = countsL1.begin();
  while(itL1 != countsL1.end()) {
    if(itL1->valid()) {
      ++itL1;
    }
    else {
      itL1 = countsL1.erase(itL1);
    }
  }

  std::vector<Ptr<HLTCount> >::iterator itL2 = countsL2.begin();
  while(itL2 != countsL2.end()) {
    if(itL2->valid()) {
      ++itL2;
    }
    else {
      itL2 = countsL2.erase(itL2);
    }
  }

  std::vector<Ptr<HLTCount> >::iterator itEF = countsEF.begin();
  while(itEF != countsEF.end()) {
    if(itEF->valid()) {
      ++itEF;
    }
    else {
      itEF = countsEF.erase(itEF);
    }
  }
}

//-----------------------------------------------------------------------------------------
bool Anp::LumiABC::FillBlockInfo(unsigned run, unsigned lb, Lumi::BlockInfo &info) const
{
  //
  // Copy into info matching run and lumi block
  //
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);
    if(b.run == run && b.lb == lb) {
      info = b;
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------------------
void Anp::LumiABC::WriteInfoTable(HtmlNode &inode) const
{
  //
  // Write full run and lb table
  //
  ostream &os = inode.GetCont();

  os << "<pre><style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Run </th>"
     << "<th> LB </th>"
     << "<th> Start </th>"
     << "<th> End </th>"
     << "<th> L status </th>"
     << "<th> Inst L </th>"
     << "<th> Scale L1 </th>"
     << "<th> Scale L2 </th>"
     << "<th> Scale EF </th>"
     << "<th> L1 </th>"
     << "<th> L2 </th>"
     << "<th> EF </th>"
     << "</tr>" 
     << endl;
  
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);

    os << "<tr>" 
       << "<td align=\"right\">" << b.run                  << "</td>"
       << "<td align=\"right\">" << b.lb                   << "</td>"
       << "<td align=\"right\">" << b.GetBegTimeAsString() << "</td>"
       << "<td align=\"right\">" << b.GetEndTimeAsString() << "</td>"
       << "<td align=\"right\">" << b.lumiValid            << "</td>"
       << "<td align=\"right\">" << b.lumiInstan           << "</td>"
       << "<td align=\"right\">" << b.scaleCorrL1          << "</td>"
       << "<td align=\"right\">" << b.scaleCorrL2          << "</td>"
       << "<td align=\"right\">" << b.scaleCorrEF          << "</td>";
    
    stringstream base_str;
    base_str << inode.GetBase() 
	     << "run_" << b.run << "_lb" << setw(4) << setfill('0') << b.lb << "_table";

    //
    // Write L1 counts
    //
    stringstream nameL1;
    nameL1 << b.L1accept;
    
    Handle<HtmlNode> nodeL1(new HtmlNode(base_str.str()+"L1.html", HtmlNode::kHTML));
    inode.AddChild(nodeL1);
    WriteLV1Counts(nodeL1.ref(), b);    

    //
    // Write L2 counts
    //
    stringstream nameL2;
    nameL2 << b.L2accept;

    Handle<HtmlNode> nodeL2(new HtmlNode(base_str.str()+"L2.html", HtmlNode::kHTML));
    inode.AddChild(nodeL2);
    WriteHLTCounts(nodeL2.ref(), b, 2);

    //
    // Write EF counts
    //
    stringstream nameEF;
    nameEF << b.EFaccept;

    Handle<HtmlNode> nodeEF(new HtmlNode(base_str.str()+"EF.html", HtmlNode::kHTML));
    inode.AddChild(nodeEF);
    WriteHLTCounts(nodeEF.ref(), b, 3);
    
    //
    // Insert links
    //
    os << "<td align=\"right\">" << nodeL1->GetLink(nameL1.str()) << "</td>"
       << "<td align=\"right\">" << nodeL2->GetLink(nameL2.str()) << "</td>"
       << "<td align=\"right\">" << nodeEF->GetLink(nameEF.str()) << "</td>"
       << "</tr>"
       << endl;    
  }

  os << "</table></pre>" << endl;
}

//-----------------------------------------------------------------------------------------
void Anp::LumiABC::WriteHLTCounts(HtmlNode &cnode, const Lumi::BlockInfo &b, unsigned level) const
{
  //
  // Write html table with trigger counts 
  //
  ostream &os = cnode.GetCont(); 
  
  os << "<pre>"
     << "run: " << b.run << endl
     << "lb:  " << b.lb  << endl;

  os << "<style type=\"text/css\">"             << endl
     << "table,  th {border:1px solid black;}"  << endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Name </th>"
     << "<th> Level </th>"
     << "<th> Counter </th>"
     << "<th> Input </th>"
     << "<th> After PS </th>"
     << "<th> Raw </th>"
     << "<th> Total </th>"
     << "<th> Total rate (Hz) </th>"
     << "</tr>" 
     << endl;

  for(unsigned i = 0; i < b.countsL2.size(); ++i) {
    const Lumi::HLTCount *c = b.countsL2.at(i).get();
    if(!c) continue;

    if(level != 0 && level != c->chain_level) {
      continue;
    }

    std::string chain_name;
    FindTrigName(c->chain_level, c->chain_counter, chain_name);
    
    os << "<tr>" 
       << "<td align=\"left\">"  <<    chain_name                << "</td>"
       << "<td align=\"right\">" << c->chain_level               << "</td>"
       << "<td align=\"right\">" << c->chain_counter             << "</td>"
       << "<td align=\"right\">" << c->count_input               << "</td>"
       << "<td align=\"right\">" << c->count_after_ps            << "</td>"
       << "<td align=\"right\">" << c->count_raw                 << "</td>"
       << "<td align=\"right\">" << c->count_total               << "</td>"
       << "<td align=\"right\">" << c->count_total*b.scaleCorrL2 << "</td>"
       << "</tr>"
       << endl;
  }

  for(unsigned i = 0; i < b.countsEF.size(); ++i) {
    const Lumi::HLTCount *c = b.countsEF.at(i).get();
    if(!c) continue;

    if(level != 0 && level != c->chain_level) {
      continue;
    }

    std::string chain_name;
    FindTrigName(c->chain_level, c->chain_counter, chain_name);

    os << "<tr>" 
       << "<td align=\"left\">"  <<    chain_name                << "</td>"
       << "<td align=\"right\">" << c->chain_level               << "</td>"
       << "<td align=\"right\">" << c->chain_counter             << "</td>"
       << "<td align=\"right\">" << c->count_input               << "</td>"
       << "<td align=\"right\">" << c->count_after_ps            << "</td>"
       << "<td align=\"right\">" << c->count_raw                 << "</td>"
       << "<td align=\"right\">" << c->count_total               << "</td>"
       << "<td align=\"right\">" << c->count_total*b.scaleCorrEF << "</td>"
       << "</tr>"
       << endl;
  }

  os << "</table></pre>" << endl;
}

//-----------------------------------------------------------------------------------------
void Anp::LumiABC::WriteLV1Counts(HtmlNode &cnode, const Lumi::BlockInfo &b) const
{
  //
  // Write html table with trigger counts 
  //
  ostream &os = cnode.GetCont(); 
  
  os << "<pre>"
     << "run: " << b.run << endl
     << "lb:  " << b.lb  << endl;

  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Name </th>"
     << "<th> CTP id </th>"
     << "<th> TBP </th>"
     << "<th> TAP </th>"
     << "<th> TAV </th>"
     << "<th> TAV rate (Hz) </th>"
     << "</tr>" 
     << endl;
  
  for(unsigned i = 0; i < b.countsL1.size(); ++i) {
    const Lumi::LV1Count *c = b.countsL1.at(i).get();
    if(!c) continue;
    
    std::string ctp_name;
    FindTrigName(1, c->ctp_id, ctp_name);

    os << "<tr>" 
       << "<td align=\"left\">"  <<    ctp_name                << "</td>"
       << "<td align=\"right\">" << c->ctp_id                  << "</td>"
       << "<td align=\"right\">" << c->count_tbp               << "</td>"
       << "<td align=\"right\">" << c->count_tap               << "</td>"
       << "<td align=\"right\">" << c->count_tav               << "</td>"
       << "<td align=\"right\">" << c->count_tav*b.scaleCorrL1 << "</td>"
       << "</tr>"
       << endl;
  }
  
  os << "</table></pre>" << endl;
}

//-----------------------------------------------------------------------------------------
void Anp::LumiABC::FillTrigNames(const TrigMonConfig &config)
{
  //
  // Fill id -> name map
  //
  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);
    fNameMap[chain.getEncodedId()] = chain.getName();
  }
}

//-----------------------------------------------------------------------------------------
bool Anp::LumiABC::FindTrigName(unsigned level, unsigned counter, std::string &name) const
{
  //
  // Find trigger name
  //
  const uint16_t id = Trig::getEncodedId(level, counter);
  
  const std::map<uint16_t, std::string>::const_iterator fit = fNameMap.find(id);
  if(fit != fNameMap.end()) {    
    name = fit->second;
    return true;
  }

  return false;
}
