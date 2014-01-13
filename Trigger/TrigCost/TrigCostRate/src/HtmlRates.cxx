/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/HtmlRates.h"

REGISTER_ANP_OBJECT(AlgEvent,HtmlRates)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
namespace Anp
{
  namespace Html
  {
    // for backward compatibility
    inline Handle<HtmlNode> AddRateNode(HtmlNode &index,
					const std::string &path, 
					const std::string &text) {
      return index.AddNode(path,text);
    }
  }

  bool isPhysicsStream(const std::string &str)
  {
    return (str=="EF_MinBias") || 
      (str=="EF_JetTauEtmiss") || 
      (str=="EF_Muons") || 
      (str=="EF_Egamma");
  }
}

//-----------------------------------------------------------------------------
Anp::HtmlRates::HtmlRates()
  :fDebug(false),
   fDoOverlapXML(false),
   fDoCumultvXML(true),
   fWriteLBCount(false)
{
}

//-----------------------------------------------------------------------------
Anp::HtmlRates::~HtmlRates()
{
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::Config(const Registry &reg)
{
  //
  // Configure properties and register self
  //
  reg.Get("HtmlRates", "Debug",      fDebug);
  reg.Get("HtmlRates::DoOverlapXML", fDoOverlapXML);
  reg.Get("HtmlRates::DoCumultvXML", fDoCumultvXML);
  reg.Get("HtmlRates::XMLPath",      fXmlPath)
;
  reg.Get("HtmlRates::RatesName",    fRatesName);
  reg.Get("HtmlRates::ConfigKeys",   fConfigKeys);

  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::Done()
{
  //
  // Prepare data for CostMaker algorithm
  //
	
  fRates = Factory<RateABC>::Instance().Get(fRatesName);
  if(!fRates.valid()) {    
    log() << "Done - missing rates algorithm: " << fRatesName << endl;
    return;
  }
  else {
    log() << "Done - retrieved rates algorithm: " << fRatesName << endl;
  }

  fRegIndex.Merge(fRates->GetIndexRegistry());

  //
  // Write XML file
  //
  if(!fXmlPath.empty()) {
    std::ofstream outf(fXmlPath.c_str());
    if(outf.is_open()) { 
      WriteXML(outf);
    }
  }
}



//-----------------------------------------------------------------------------
void Anp::HtmlRates::Write(HtmlNode &inode, HtmlMenu &hmenu)
{
  //
  // Create html page for each rate element
  //
  if(!fRates.valid()) {
    log() << "Write - invalid rates handle" << endl;
    return;
  }
  if(!(fRates->GetScale() > 0.0) || !(fRates->GetTotal() > 0.0)) {
    log() << "Write - rates algorithm " << fRatesName << " is missing normalization data: " << endl
	  << "   Scale = "  << fRates->GetScale() << endl
	  << "   Total = "  << fRates->GetTotal() << endl;
    return;
  }
  
  log() << "Write - writing rates to node: " << inode.GetPath() << endl;
  
  //
  // Write total level rates
  //
  const vector<Handle<RateGroup> > &levels = fRates->GetRateLevels();
  map<unsigned, Handle<RateGroup> > level_map;

  //  for(unsigned int i = 0; i < levels.size(); ++i) {
  //    level_map[levels.at(i)->GetLevel()] = levels.at(i);
  //  }
  
  //  for(map<unsigned, Handle<RateGroup> >::iterator it = level_map.begin(); it != level_map.end(); ++it) {
  map<unsigned, double> total_rates; map<unsigned, double> total_rates_err;
  
  for(unsigned int ilevel=1; ilevel<=3; ilevel++){
    string name;
    stringstream lcont, lfile;
    if (ilevel == 1){
      lfile << "rates_chain_L1.html";
      name = "L1";
    } else if(ilevel == 2) {
      lfile << "rates_chain_L2.html";
      name = "L2";
    } else if(ilevel == 3) {
      lfile << "rates_chain_EF.html";
      name = "EF";
    } 

    double total_rates = 0.0; double total_rates_err = 0.0; 
    std::vector<Handle<RateGroup> > groups;
    for(unsigned int i = 0; i < levels.size(); ++i) {
      Handle<RateGroup> group = levels.at(i);

      // calculate the total rates for each level
      if(group->GetLevel() != ilevel) continue;

      const std::pair<double, double> prateW = group->GetPassRate();
      const std::pair<string, string> rrateW_i = Anp::Round2Pair(prateW.first, prateW.second);
      
      total_rates += prateW.first;
      total_rates_err += prateW.second*prateW.second;

      // load the vector of the groups
      groups.push_back(group);

      // create a page for this group
      stringstream lcont_indiv, lfile_indiv;
      
      lcont_indiv << group->GetName() << " rate: " << rrateW_i.first << " +/- " << rrateW_i.second << " Hz";
      lfile_indiv << "rates_indiv_chain_" << group->GetName() << ".html";

      std::vector<Handle<RateGroup> > groups_indiv;
      groups_indiv.push_back(group);

      Handle<HtmlNode> rateLevel_indiv = Handle<HtmlNode>(new HtmlNode(lfile_indiv.str(), lcont_indiv.str(), HtmlNode::kHTML));
      inode.AddChild(rateLevel_indiv);
      WriteGroupRates(rateLevel_indiv, hmenu, "TOP SORT", groups_indiv);
    }

    total_rates_err = sqrt(total_rates_err);
    const std::pair<string, string> rrateW = Anp::Round2Pair(total_rates, total_rates_err);
    
    lcont << name << " rate: " << rrateW.first << " +/- " << rrateW.second << " Hz";

    Handle<HtmlNode> rateLevel = Html::AddRateNode(inode, lfile.str(), lcont.str());
    // check that there are items for this level
    if(groups.size() >= 1) {
      WriteGroupRates(rateLevel, hmenu, "CHAIN_RATE TOP SORT", groups);
    }
  } 

  //
  // Write groups and streams
  //
  inode.GetCont() << endl;
  Handle<HtmlNode> grpL1 = Html::AddRateNode(inode, "rates_groups_L1.html", "L1 group  rates");

  inode.GetCont() << "L2 group rates (";
  Handle<HtmlNode> grpL2_RATE = inode.AddNode("rates_groups_L2.html", "rate groups"      ,true); 
  inode.GetCont() << ",";
  Handle<HtmlNode> grpL2_BW   = inode.AddNode("bw_groups_L2.html"   , "bandwidth"        ,true);
  inode.GetCont() << ",";
  Handle<HtmlNode> grpL2_CPS  = inode.AddNode("cps_groups_L2.html"  , "coherent prescale",true);
  inode.GetCont() << ")" << endl;  
  Handle<HtmlNode> strL2 = Html::AddRateNode(inode, "rates_stream_L2.html", "L2 stream rates");
  
  inode.GetCont() << endl;
  inode.GetCont() << "EF group rates (";
  Handle<HtmlNode> grpEF_RATE = inode.AddNode("rates_groups_EF.html", "rate groups"      ,true); 
  inode.GetCont() << ",";
  Handle<HtmlNode> grpEF_BW   = inode.AddNode("bw_groups_EF.html"   , "bandwidth"        ,true);
  inode.GetCont() << ",";
  Handle<HtmlNode> grpEF_CPS  = inode.AddNode("cps_groups_EF.html"  , "coherent prescale",true);
  inode.GetCont() << ")" << endl;
  Handle<HtmlNode> strEF      = inode.AddNode("rates_stream_EF.html", "EF stream rates"    );
  Handle<HtmlNode> inpEF      = inode.AddNode("rates_inputs_EF.html", "EF input  rates"    );
  
  WriteGroupTable(grpL1, hmenu, 1, "SORT_RATE PARENT", fRates->GetRateGroups());
  
  WriteGroupTable(grpL2_RATE, hmenu, 2, "SORT_RATE PARENT FILTERRATE ", fRates->GetRateGroups());
  WriteGroupTable(grpL2_BW  , hmenu, 2, "SORT_RATE PARENT FILTERBW "  , fRates->GetRateGroups());
  WriteGroupTable(grpL2_CPS , hmenu, 2, "SORT_RATE PARENT FILTERCPS"  , fRates->GetRateGroups());
  WriteGroupTable(strL2, hmenu, 2, "SORT_RATE STREAM FILTERCPS"  , fRates->GetRateStream());
  
  WriteGroupTable(grpEF_RATE, hmenu, 3, "SORT_RATE PARENT FILTERRATE", fRates->GetRateGroups());
  WriteGroupTable(grpEF_BW,   hmenu, 3, "SORT_RATE PARENT FILTERBW",   fRates->GetRateGroups());
  WriteGroupTable(grpEF_CPS,  hmenu, 3, "SORT_RATE PARENT FILTERCPS",  fRates->GetRateGroups());
  WriteGroupTable(strEF, hmenu, 3, "SORT_RATE STREAM", fRates->GetRateStream());
  WriteTotalRates(inpEF.ref(), hmenu);
  
  inode.GetCont() << endl;

  // Write multi-seeded triggers  
  Handle<HtmlNode> nodeMS = Html::AddRateNode(inode, "multi_seeds.html", "Multi-seeded chain rates");
  WriteMultiSeeds(nodeMS, hmenu, "");

  // Write counts by LB
  inode.GetCont() << endl;
  if(fWriteLBCount) {
    Handle<HtmlNode> rateLB = Html::AddRateNode(inode, "rates_counts_by_lb.html", "Counts by Luminosity Block");
    WriteCountsByLB(rateLB->GetCont(), inode, hmenu);
  }

  // Write bandwidth table
  Handle<HtmlNode> nodeBW = Html::AddRateNode(inode, "BandwidthRates.html", "Bandwidth rates"); 
  WriteBandwidths(nodeBW->GetCont()); 
  
  // Write rates in XML file
  Handle<HtmlNode> rateXM = Html::AddRateNode(inode, "TriggerCosts.xml", "XML rates");
  WriteXML(rateXM->GetCont());
  inode.GetCont() << endl;
}

//-----------------------------------------------------------------------------
namespace Anp
{
  namespace Trig
  {
    Handle<RateChain> FindLowerChain(Handle<RateABC> rates,
				     Handle<RateChain> chain)
    {
      //
      // Find lower chain for this chain
      //
      if(!rates.valid() || !chain.valid()) { 
	return Handle<Anp::RateChain>(0);
      }
      
      const vector<Handle<RateChain> > &chains = rates -> GetRateChains();
      const string &lname = chain->GetChain().getLowerName();
      
      if(lname.empty()) {
	return Handle<Anp::RateChain>(0);      
      }
      
      for(unsigned i = 0; i < chains.size(); ++i) {
	Handle<RateChain> curr = chains.at(i);
	if(curr -> GetChain().getName() == lname) {
	  return curr;
	}
      }
      
      return Handle<Anp::RateChain>(0);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteTotalRates(HtmlNode &node, HtmlMenu &hmenu)
{
  //
  // Write trigger levels across all three levels
  //
  if(!fRates.valid()) return;

  ostream &os = node.GetCont();
  os << "<pre>";
  
  node.AddLink("top ", "../", false);
  node.AddLink("index ", "index.html", false);

  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Chain </th>"
     << "<th> Total PS </th>"
     << "<th> Total PT </th>"
     << "<th> EF rate (Hz) </th>"
     << "<th> L2 rate (Hz) </th>"
     << "<th> L1 rate (Hz) </th>"
     << "<th> L2/EF rates</th>"
     << "<th> L2/L1 rates</th>"
     << "<th> L1/EF rates</th>"
     << "</tr>" << endl;

  const vector<Handle<RateChain> > &chains = fRates -> GetRateChains();

  for(unsigned i = 0; i < chains.size(); ++i) {
    const Handle<RateChain> &chainEF = chains.at(i);
    if(chainEF -> GetLevel() != 3) {
      continue;
    }
     
    Handle<RateChain> chainL2 = Trig::FindLowerChain(fRates, chainEF);
    Handle<RateChain> chainL1 = Trig::FindLowerChain(fRates, chainL2);

    string nameL1, nameL2;
    pair<double, double> rateL1(0.0, 0.0), rateL2(0.0, 0.0);

    if(chainL2.valid()) {
      nameL2 = hmenu.GetChainLink(chainL2->GetName());
      rateL2 = chainL2->GetPassRate();
    }    
    if(chainL1.valid()) {
      nameL1 = hmenu.GetChainLink(chainL1->GetName());
      rateL1 = chainL1->GetPassRate();
    }
    
    const string nameEF = hmenu.GetChainLink(chainEF->GetName());
    const pair<double, double> rateEF = chainEF->GetPassRate();

    const std::pair<string, string> strL1 = Anp::Round2Pair(rateL1.first, rateL1.second);
    const std::pair<string, string> strL2 = Anp::Round2Pair(rateL2.first, rateL2.second);
    const std::pair<string, string> strEF = Anp::Round2Pair(rateEF.first, rateEF.second);


    double L1overEF = 0.0, L2overEF = 0.0, L1overL2 = 0.0;
    
    if(rateEF.first > 0.0) L1overEF = rateL1.first/rateEF.first;
    if(rateEF.first > 0.0) L2overEF = rateL2.first/rateEF.first;
    if(rateL2.first > 0.0) L1overL2 = rateL1.first/rateL2.first;
    
    const std::pair<string, string> L1overEF_str = Anp::Round2Pair(L1overEF, 1.0);
    const std::pair<string, string> L2overEF_str = Anp::Round2Pair(L2overEF, 1.0);
    const std::pair<string, string> L1overL2_str = Anp::Round2Pair(L1overL2, 1.0);

    double tot_pt = 0; 
    double l2_pt = chainEF->GetInputL2().getPassThrough();
    double ef_pt = chainEF->GetChain().getPassThrough();
    if(l2_pt > 0 && ef_pt > 0) tot_pt = l2_pt*ef_pt;
    else if(l2_pt > 0) tot_pt = l2_pt;
    else if(ef_pt > 0) tot_pt = ef_pt;

    os << "<tr>" 
       << "<td align=\"left\"> "   << nameEF       << " </td>"
       << "<td align=\"center\"> " << chainEF->GetTotalPS()  << " </td>"
       << "<td align=\"center\"> " << tot_pt  << " </td>"
       << "<td align=\"center\"> " << strEF.first  << " </td>"
       << "<td align=\"center\"> " << strL2.first  << " </td>"
       << "<td align=\"center\"> " << strL1.first  << " </td>"
       << "<td align=\"center\"> " << L2overEF_str.first << " </td>"
       << "<td align=\"center\"> " << L1overL2_str.first << " </td>"
       << "<td align=\"center\"> " << L1overEF_str.first << " </td>"
       << "</tr>" << endl;
  }

  os << "</table></pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteMultiSeeds(Handle<HtmlNode> node,
				     HtmlMenu &hmenu,
				     const std::string &)
  
{
  //
  // Write table for multi-seeded triggers
  //
  node -> GetCont() << "<pre>";
  node -> AddLink("top ", "../", false);
  node -> AddLink("index ", "index.html", false);

  node -> GetCont() << "\n\nThese multi-seeded chains are excluded from rate calculations for groups"
		    << ", streams and levels." << endl;

  WriteChainTable(node, hmenu, 2, "CHAIN_RATE SORT", "MS", fRates->GetMultiSeeds());
  WriteChainTable(node, hmenu, 3, "CHAIN_RATE SORT", "MS", fRates->GetMultiSeeds());

  node -> GetCont() << "</pre>";  
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteCountsByLB(ostream &os, 
				     HtmlNode &inode,
				     HtmlMenu &hmenu)
{
  //
  // Write chain counts by Lumi Block(s)
  //
  if(!fRates.valid()) return;

  const vector<Handle<RateChain> > &chains = fRates -> GetRateChains();
  
  //
  // Find all valid lumi blocks
  //
  set<unsigned> runs;

  for(unsigned i = 0; i < chains.size(); ++i) {    
    const Handle<RateChain> &chain = chains.at(i);
    if(!chain.valid()) continue;

    const std::map<unsigned, std::vector<LumiCount> *>& rmap = chain->GetCounts();
      
    for(std::map<unsigned, std::vector<LumiCount> *>::const_iterator rit = rmap.begin(); 
	rit != rmap.end(); rit++) {
      runs.insert(rit->first);
    }
  }

  for(set<unsigned>::const_iterator rit = runs.begin(); rit != runs.end(); ++rit) {
    const unsigned run = *rit;

    std::stringstream rstr, hstr;
    rstr << "rates_counts_by_lb_run_" << setw(8) << setfill('0') << run << ".html";
    hstr << "Trigger counts for run " << run;

    Handle<HtmlNode> node(new HtmlNode(rstr.str(), HtmlNode::kHTML));
    inode.AddChild(node);
    os << node->GetLink(hstr.str()) << endl;

    WriteCountsByLB(node->GetCont(), inode, hmenu, run);
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteCountsByLB(ostream &os,
				     HtmlNode &inode,
				     HtmlMenu &hmenu,
				     unsigned run)
{
  //
  // Write chain counts by Lumi Block(s)
  //
  if(!fRates.valid()) return;

  const vector<Handle<RateChain> > &chains = fRates -> GetRateChains();
  
  //
  // Find all valid lumi blocks
  //
  set<int> lumi_set;

  for(unsigned i = 0; i < chains.size(); ++i) {    
    const Handle<RateChain> &chain = chains.at(i);
    if(!chain.valid()) continue;

    const std::map<unsigned, std::vector<LumiCount> *>& lmap = chain->GetCounts();
    const std::map<unsigned, std::vector<LumiCount> *>::const_iterator rit = lmap.find(run);
    if(rit == lmap.end() || !(rit->second)) {
      continue;
    }
    
    const std::vector<LumiCount> &lvec = *(rit->second);
      
    for(std::vector<LumiCount>::const_iterator lit = lvec.begin(); lit != lvec.end(); lit++) {
      if(lit->lumi_block >= 0) {
	lumi_set.insert(lit->lumi_block);
      }
    }
  }

  //
  // Start html table for all chains and all lumi blocks
  //
  os << "<pre>"
     << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th>chain</th>"
     << "<th>rate (Hz)</th>"
     << "<th>count</th>";
  
  for(set<int>::const_iterator sit = lumi_set.begin(); sit != lumi_set.end(); ++sit) {
    os << "<th>" << *sit << "</th>" << endl;
  }

  os << "</tr>" << endl;

  //
  // Iterate over chains and fill single table for all chains 
  // and individual table for each chain
  //
  for(unsigned i = 0; i < chains.size(); ++i) {
    const Handle<RateChain> &chain = chains.at(i);
    if(!chain.valid()) continue;
    
    const std::map<unsigned, std::vector<LumiCount> *>& lmap = chain->GetCounts();
    const std::map<unsigned, std::vector<LumiCount> *>::const_iterator rit = lmap.find(run);
    if(rit == lmap.end() || !(rit->second)) {
      continue;
    }
    
    const std::vector<LumiCount> &lvec = *(rit->second);
    string chain_name = chain->GetName();

    Handle<HtmlNode> nodeChain = hmenu.FindChnNode(chain->GetName());
    if(nodeChain.valid()) { 
      chain_name = nodeChain->GetLink(chain_name);
    }
    
    const std::pair<double, double> chain_rate = chain->GetPassRate();
    const double chain_count = chain->GetEventsPassed();
    
    const std::pair<string, string> chain_rateS = Anp::Round2Pair(chain_rate.first,
								  chain_rate.second);

    os << "<tr>"
       << "<td align=\"left\">" << chain_name << "</td>"
       << "<td align=\"right\">" << chain_rateS.first << "</td>"
       << "<td align=\"right\">" << static_cast<unsigned>(chain_count) << "</td>";

    for(set<int>::const_iterator sit = lumi_set.begin(); sit != lumi_set.end(); ++sit) {
      const int lumi = *sit;
      const std::vector<LumiCount>::const_iterator ilumi = std::find(lvec.begin(), lvec.end(), lumi);

      if(ilumi != lvec.end() && ilumi->GetLumi() == lumi) {
	os << "<td align=\"right\"> " << ilumi->GetLumiEventsPassed() << " </td> ";
      }
      else {
	os << "<td align=\"right\"> 0 </td> ";
      }
    }
    os << "</tr>" << endl;

    //
    // Fill individual chain table
    //
    stringstream page;
    if(lvec.empty()) {
      page << "<style type=\"text/css\">"<< endl
	   << "table,  th {border:1px solid black;}"<< endl
	   << "th { padding:10px; background-color:white; color:black; }"<< endl
	   << "td {padding-left:15px;padding-right:15px;}"<< endl
	   << "</style>"<< endl
	   << "<script src=\"sorttable.js\"></script>" << endl
	   << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	   << "<tr>"
	   << "<th> Chain: " << chain->GetName() << "  </th>"
	   << "<th> No counts </th>"
	   << "</tr>" << endl;
      
    }
    else {
      page << "<style type=\"text/css\">"<< endl
	   << "table,  th {border:1px solid black;}"<< endl
	   << "th { padding:10px; background-color:white; color:black; }"<< endl
	   << "td {padding-left:15px;padding-right:15px;}"<< endl
	   << "</style>"<< endl
	   << "<script src=\"sorttable.js\"></script>" << endl
	   << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	   << "<tr>"
	   << "<th> Chain: " << chain->GetName() << "  </th>"
	   << "<th> Lumi Block </th>"
	   << "<th> Count </th>"
	   << "</tr>" << endl;
      
      for(std::vector<LumiCount>::const_iterator lit = lvec.begin(); lit != lvec.end(); lit++) {
	if(lit->lumi_block >= 0) {
	  page << "<tr>" 
	       << "<td align=\"right\"> " << ""                         << " </td>"
	       << "<td align=\"right\"> " << lit->GetLumi()             << " </td>"
	       << "<td align=\"right\"> " << lit->GetLumiEventsPassed() << " </td>"
	       << "</tr>" << endl; 
	}
      }
    }
    
    Handle<HtmlNode> nodeCh = hmenu.FindChnNode(chain->GetName());
    if(nodeCh.valid()) {
      Handle<HtmlNode> nodeLB(new HtmlNode(nodeCh->GetBase()+"_countByLB.html", HtmlNode::kHTML));
      inode.AddChild(nodeLB);

      nodeCh -> GetCont() << "<pre>" << nodeLB->GetLink("Counts by luminosity blocks") << "</pre>" << endl;
      nodeLB -> GetCont() << page.str() << endl;
    }
  }

  os << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
namespace Anp
{
  namespace Html
  {
    struct Overlap {
      string over_name;
      string cell_rate;
      string cell_frac;
    };
    
    void WriteOverlap(std::ostream &os,
		      const Handle<RateGroup> &group,
		      const vector<Handle<RateGroup> > &unions)
    {
      //
      // Write overlap table sorted by rate
      //
      multimap<double, Overlap> over_map;
    
      const double group_rate = group->GetPassRate().first;

      for(unsigned int i = 0; i < unions.size(); ++i) {
	const Handle<RateGroup> &urate = unions.at(i);
	
	if(urate->GetGroup1() != group && urate->GetGroup2() != group) {
	  continue;
	}
	
	Overlap op;
	const pair<double, double> orate = urate->GetRateOverlap();
	
	if(group->GetName() != urate->GetGroup1()->GetName()) {
	  op.over_name = urate->GetGroup1()->GetName();
	}
	else {
	  op.over_name = urate->GetGroup2()->GetName();
	}
	
	double orate_frac = 0.0;
	if(group_rate > 0.0) {
	  orate_frac = 100.0*orate.first/group_rate;
	}
	
	op.cell_rate = Anp::Round2Pair(orate.first, 1.0).first;
	op.cell_frac = Anp::Round2Pair(orate_frac,  1.0).first;
	
	over_map.insert(make_pair(orate.first, op));
      }
      
      os << "<table border=\"1\">" << endl
	 << "<tr>" 
	 << "<th>&nbsp</th>"
	 << "<th>Overlap rate (Hz)</th>"
	 << "<th>Overlap fraction (%)</th>"
	 << "</tr>" << endl;
      
      for(multimap<double, Overlap>::reverse_iterator it = over_map.rbegin(); it != over_map.rend(); ++it) {
	const Overlap &op = it->second;
	os << "<tr>"
	   << "<td>" << op.over_name << "</td>"
	   << "<td>" << op.cell_rate << "</td>"
	   << "<td>" << op.cell_frac << "</td>"
	   << "</tr>" << endl;
      }   
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteChainTable(Handle<HtmlNode> node,
				     HtmlMenu &hmenu,
				     unsigned level,
				     const std::string &option,
				     const std::string &callee,
				     vector<Handle<RateChain> > chains)
{
  //
  // Write table for chain rates
  //  
  if(fDebug) {
    log() << "WriteChainTable - callee: " << callee << endl;
  }

  bool do_unique = false;
  for(unsigned i = 0; i < chains.size(); ++i) {
    if(chains[i]->HasUnique()) {
      do_unique = true;
    }
  }

  ostream &os = node->GetCont();
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr><th>Chain</th>"
     << "<th>PS</th>";
  
  if(level == 2) {
    os << "<th>Total PS</th>"
       << "<th>PT</th>";
  }
  else if(level == 3) { 
    os << "<th>Total PS</th>"
       << "<th>PT_L2</th>"
       << "<th>PT_EF</th>";
  }
  

  if (option.find("EXPRESS") != string::npos) {
    os << "<th>Express PS</th>";    
  }

  os << "<th>Pass events (raw counts) </th>"    
     << "<th>Pass fraction</th>"
     << "<th>Accept rate (Hz)</th>"
     << "<th>Accept error (Hz)</th>"
     << "<th>Accept frac. error (%)</th>";
  
  if(do_unique) {
    os << "<th>Unique rate (Hz)</th>";
  }
 
  if( level == 1 ) {
    os << "<th>EF Output Rate (Hz) </th>";
  }
  os << "</tr>" << endl;

  bool write_chain_rate = false;
  if(option.find("CHAIN_RATE") != string::npos) {
    write_chain_rate = true;
  }
  
  if(option.find("SORT") != string::npos) {
    std::sort(chains.begin(), chains.end(), Anp::RateChainSort("rate"));
  }
  
  for(unsigned int i = 0; i < chains.size(); ++i) {
    const Handle<RateChain> &chain = chains.at(i);
    
    if(level != 0 && chain->GetLevel() != level) continue;
    
    const std::pair<double, double> pfracW = chain->GetPassFrac();
    const std::pair<double, double> prateW = chain->GetPassRate();
    const std::pair<double, double> prateU = chain->GetRateUnique();
    const std::pair<double, double> prateR = chain->GetPassRateRaw();
    
    const double events_passedR = chain->GetEventsPassedRaw();

    const string chain_name = hmenu.GetChainLink(chain->GetName());
    
    os << "<tr>"
       << "<td align=\"left\"> "   << chain_name                         << " </td>"
       << "<td align=\"center\"> " << chain->GetChain().getPrescale()    << " </td>";
    
    if(level == 2) {
      os << "<td align=\"center\"> " << chain->GetTotalPS()                << " </td>"
	 << "<td align=\"center\"> " << chain->GetChain().getPassThrough() << " </td>";
    }    
    else if(level == 3) {
      os << "<td align=\"center\"> " << chain->GetTotalPS()                  << " </td>"
	 << "<td align=\"center\"> " << chain->GetInputL2().getPassThrough() << " </td>"
	 << "<td align=\"center\"> " << chain->GetChain().getPassThrough()   << " </td>";
    }

    if (option.find("EXPRESS") != string::npos) {
      os << "<td align=\"center\"> " << chain->GetExpressPS()                << " </td>";
    }
    
    const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
    const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);
    const std::pair<string, string> rrateU = Anp::Round2Pair(prateU.first, prateU.second);
    const std::pair<string, string> rrateR = Anp::Round2Pair(prateR.first, prateR.second);
    double prateW_frac = 0.0;
    if(prateW.first > 0.0) prateW_frac = prateW.second/prateW.first*100.;
    const std::pair<string, string> rrateW_frac = Anp::Round2Pair(prateW_frac, 0.1);
    
    
    os << "<td align=\"center\"> " << events_passedR << " </td>"
       << "<td align=\"center\"> " << rfracW.first   << " </td>"
       << "<td align=\"center\"> " << rrateW.first   << " </td>"
       << "<td align=\"center\"> " << rrateW.second  << " </td>"
       << "<td align=\"center\"> " << rrateW_frac.first  << " </td>";

    if(level == 1) {
      const RateGroup * ogroup = chain->GetOutputGroup();
      if(ogroup) {
        const std::pair<double, double> prateOG = ogroup->GetPassRate();
        const std::pair<string, string> rrateOG = Anp::Round2Pair(prateOG.first, prateOG.second);
        os << "<td align=\"center\"> " << rrateOG.first << " </td>";
      }
      else {
        if(fDebug) {
          log() << "Output group for " << chain_name << " is not valid" << endl;
        }
        os << "<td align=\"center\"> </td>";

      }
    }

    if(do_unique) {
      
      if(chain->HasUnique()) {
        os << "<td align=\"center\">" << rrateU.first << "</td>" << endl;
      }
      else {
        os << "<td align=\"center\"> </td>" << endl;
      }
    }
    os  << "</tr>" << endl;
    
    //
    // Write rates to chain web page
    //
    if(write_chain_rate && fRates.valid()) {
      Handle<HtmlNode> hnode = hmenu.FindChnNode(chain->GetName());
      
      const vector<Handle<RateChain> > &all_chains = fRates -> GetRateChains();
      const vector<Handle<RateChain> > &ptr_chains = fRates -> GetPTOnlyRate();

      if(hnode.valid()) {

	string rate_key;
	if     (chain->GetRateKey() == "WEIGHTED") rate_key = "re-weight events using PS/PT";
	else if(chain->GetRateKey() == "COUNT")    rate_key = "simple event count - PS/PT are ignored";

	ostream &hs = hnode -> GetCont();

	hs << "<pre>" << endl
	   << "Rate scheme:   " << rate_key << endl
	   << "Prescale:      " << std::left << chain->GetChain().getPrescale()    << endl
	   << "Passthrough:   " << std::left << chain->GetChain().getPassThrough() << endl
	   << "Total PS:      " << chain->GetTotalPS()   << endl
	   << "Express PS:    " << chain->GetExpressPS() << endl
	   << "Pass fraction: " << rfracW.first << endl
	   << "Pass events (raw counts):   " << events_passedR << endl
	   << "Accept rate:   " << rrateW.first << " +/- " << rrateW.second << " Hz" << endl;
	
	if(chain->GetLevel() != 1) {
	  hs << "Unique rate:   " << rrateU.first << " +/- " << rrateU.second << " Hz"
	     << " (within stream) " << endl;
	  
	  hs << endl 
	     << "Rate from L2 PT only: " << endl;

	  const vector<Handle<RateChain> >::const_iterator ipt = std::find(ptr_chains.begin(), 
									   ptr_chains.end(), 
									   chain->GetName());

	  if(ipt != ptr_chains.end() && ipt->valid()) {
	    const double events_passedPT = (*ipt)->GetEventsPassed();
	    const std::pair<double, double> ratePT = (*ipt)->GetPassRate();
	    const std::pair<string, string> ratePT_str = Anp::Round2Pair(ratePT.first, ratePT.second);

	    hs << "  pass events: " << events_passedPT << endl
	       << "  accept rate: " << ratePT_str.first << " +/- " << ratePT_str.second << " Hz" << endl;
	  }
	  else {
	    hs << "  pass events: 0"    << endl
	       << "  accept rate: 0 Hz" << endl;
	  }
	}
	
	hs << endl
	   << "Using raw event counts: " << endl
	   << "  pass events: " << events_passedR << endl
	   << "  accept rate: " << rrateR.first << " +/- " << rrateR.second << " Hz" << endl;
	

	//
        // collect chains from next level and write rates table
	//
        if(level == 1) {
          vector<Handle<RateChain> > L2_chains, EF_chains;
          for(unsigned j = 0; j < all_chains.size(); ++j) {
            const Handle<RateChain> &nchain = all_chains.at(j);

            if(nchain->GetLevel() != 1 && nchain->GetInputL1() == chain) {
              if(nchain->GetLevel() == 2) L2_chains.push_back(nchain);
              if(nchain->GetLevel() == 3) EF_chains.push_back(nchain);
            }
          }

          WriteChainTable(hnode, hmenu, 2, "SORT", chain->GetName(), L2_chains);
          WriteChainTable(hnode, hmenu, 3, "SORT", chain->GetName(), EF_chains);

          if(chain->GetOutputGroup()) {
            std::pair<double, double> grate = chain->GetOutputGroup()->GetPassRate();
            hnode->GetCont() << "<pre>Total EF rate = " << Anp::Round2Pair(grate.first,grate.second).first << " Hz </pre>" << endl;
          }
        }

        //
        // For EF chains write rates for chains that seed it
        //
        if(level == 3) {
          std::vector<Handle<RateChain> > seed_chains;
          seed_chains.push_back(chain);
          
          const TrigConfChain &inputL1 = chain->GetInputL1();
          const TrigConfChain &inputL2 = chain->GetInputL2();
          
          std::vector<Handle<RateChain> >::const_iterator L2match =
                std::find(all_chains.begin(), all_chains.end(), inputL2);
          if(L2match != all_chains.end()) {
            seed_chains.push_back(*L2match);
          }
          
          std::vector<Handle<RateChain> >::const_iterator L1match =
                std::find(all_chains.begin(), all_chains.end(), inputL1);
          if(L1match != all_chains.end()) {
            seed_chains.push_back(*L1match);
          }
          
          WriteChainTable(hnode, hmenu, 0,"", callee, seed_chains); 
        }

	node->GetCont() << "</pre>" << endl;
      }
      else {
	log() << "WriteChainTable - failed to find node for chain: " << chain->GetName() << endl;
      }
    }
  }
  
  os << "</table>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteGroupTable(Handle<HtmlNode> node,
				     HtmlMenu &hmenu, 
				     unsigned level,
				     const std::string &option,
				     vector<Handle<RateGroup> > groups)
{
  //
  // Write HTML table of group rates
  //
  node -> GetCont() << "<pre>";
  node -> AddLink("top ", "../", false);
  node -> AddLink("index ", "index.html", false);

  //
  // Write total level rate
  //
  const vector<Handle<RateGroup> > &levels = fRates->GetRateLevels();
  for(unsigned int i = 0; i < levels.size(); ++i) {
    const Handle<RateGroup> &group = levels.at(i);
    if(group->GetLevel() != level) continue;
    
    const std::pair<double, double> prateW = group->GetPassRate();
    const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);
    
    stringstream lcont;
    lcont << "   " << group->GetName() 
	  << " rate: " << rrateW.first << " +/- " << rrateW.second << " Hz";
    
    node->GetCont() << lcont.str();
  }

  //
  // Make HtmlNode for overlap tables
  // 
  stringstream fileO, titleO;
  bool is_group = true;

  if(option.find("STREAM") != string::npos) {
    fileO << "stream_overlaps_L" << level << ".html";
    titleO << "streams";
    is_group = false;
  }
  else {
    titleO << "groups";
    if (option.find("FILTERBW") != string::npos) {
      fileO << "bw_";
    } else if (option.find("FILTERRATE") != string::npos) {
      fileO << "rate_";
    } else if (option.find("FILTERCPS") != string::npos) {
      fileO << "cps_";
    }
    fileO << "groups_overlaps_L" << level << ".html";
  }

  Handle<HtmlNode> nodeO(new HtmlNode(fileO.str(), HtmlNode::kHTML));
  node -> AddChild(nodeO);
  
  node->GetCont() << nodeO->GetLink("   Overlap table") << endl;

  nodeO -> GetCont() << "<pre>";
  nodeO -> AddLink("top ", "../", false);
  nodeO -> AddLink("index ", "index.html", false);
  nodeO -> AddLink(titleO.str(), node->GetPath());

  //
  // Select groups which match level
  //
  vector<Handle<RateGroup> > selected_groups;

  for(unsigned int i = 0; i < groups.size(); ++i) {
    const Handle<RateGroup> &group = groups.at(i);
    
    if(group->GetLevel() == level ) { 
      if (option.find("FILTERBW") != string::npos) {
	if (group->GetName().find("_BW")!= string::npos) {
	  cout << "BW: " << group->GetName();
	  selected_groups.push_back(group);
	}
      } else if (option.find("FILTERRATE") != string::npos) {
	if (group->GetName().find("_RATE")!= string::npos) {
	  cout << "RATE: " << group->GetName();
	  selected_groups.push_back(group);
	}
      } else if (option.find("FILTERCPS") != string::npos) {
	if (group->GetName().find("_CPS")!= string::npos) {
	  cout << "CPS: " << group->GetName();
	  selected_groups.push_back(group);
	}
      } else {
	selected_groups.push_back(group);
      } 
    } 
  }

  //
  // Sorted groups by rate
  //
  if(option.find("SORT_RATE") != string::npos) {
    std::sort(selected_groups.begin(), selected_groups.end(), Anp::RateGroupSort("rate"));
  }
  else if(option.find("SORT_NAME") != string::npos) {
    std::sort(selected_groups.begin(), selected_groups.end(), Anp::RateGroupSort(""));
  }
  
  ostream &os = node->GetCont();

  os  << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Name </th>"
     << "<th> Effective PS </th>"
     << "<th> Pass events (raw counts) </th>"    
     << "<th> Pass fraction </th>"
     << "<th> Accept rate (Hz) </th>"
     << "<th> Accept error (Hz) </th>"
    //<< "<th> Unique rate (Hz) </th>"
     << "</tr>" << endl; 

  //
  // Sum global rates to sum for streams
  //
  double total_EF_physics_rate      = 0.0;
  double total_EF_physics_rate_err2 = 0.0; // square of error

  //
  // Write group table
  //
  for(unsigned int i = 0; i < selected_groups.size(); ++i) {
    const Handle<RateGroup> &group = selected_groups.at(i);
    cout << group->GetName() << endl;
    //
    // Make group html node
    //
    string gname = group->GetName()+".html";    
    for(unsigned iname = 0; iname < gname.size(); ++iname) {
      if(gname.at(iname) == ':' || gname.at(iname) == ' ') {
	gname.at(iname) = '_';
      }
    }

    Handle<HtmlNode> nodeGR, nodeGO;
    if(is_group) {
      nodeGR = Handle<HtmlNode>(new HtmlNode("GRP_"+gname, "GROUP_"+group->GetName(), HtmlNode::kHTML));
      nodeGO = Handle<HtmlNode>(new HtmlNode("GRO_"+gname, "GOVER_"+group->GetName(), HtmlNode::kHTML));
    }
    else {
      nodeGR = Handle<HtmlNode>(new HtmlNode("STR_"+gname, "STREAM_"+group->GetName(), HtmlNode::kHTML));
      nodeGO = Handle<HtmlNode>(new HtmlNode("STO_"+gname, "STOVER_"+group->GetName(), HtmlNode::kHTML));
    }

    fGroup[nodeGR->GetName()] = nodeGR;

    node -> AddChild(nodeGR);
    node -> AddChild(nodeGO);
    
    nodeGR -> GetCont() << "<pre>" << endl;
    nodeGR -> AddLink("top ", "../", false);
    nodeGR -> AddLink("index ", "index.html", false);    
    nodeGR -> GetCont() << node->GetLink(titleO.str()) << "   ";
    nodeGR -> GetCont() << nodeGO->GetLink("overlaps with other "+titleO.str()) << endl << endl;

    nodeGO -> GetCont() << "<pre>" << endl;
    nodeGO -> AddLink("top ", "../", false);
    nodeGO -> AddLink("index ", "index.html", false);    
    nodeGO -> GetCont() << node->GetLink(titleO.str()) << endl << endl;

    const double nevtW = group->GetEventsPassed();
    const double nevtR = group->GetEventsPassedRaw();
    
    double effps = 0.0;
    if(nevtW > 0.0) effps = nevtR/nevtW;
    
    os << "<tr>"
       << "<td align=\"left\"> "   << nodeGR->GetLink(group->GetName()) << " </td>"
       << "<td align=\"center\"> " << effps << " </td>"
       << "<td align=\"center\"> " << nevtR << " </td>";
    
    const std::pair<double, double> pfracW = group->GetPassFrac();
    const std::pair<double, double> prateW = group->GetPassRate();
    const std::pair<double, double> prateR = group->GetPassRateRaw();
    const std::pair<double, double> prateU = group->GetRateUnique();
    
    const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
    const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);
    const std::pair<string, string> rrateR = Anp::Round2Pair(prateR.first, prateR.second);
    const std::pair<string, string> rrateU = Anp::Round2Pair(prateU.first, prateU.second);
    
    if (!is_group && isPhysicsStream(group->GetName())) {
      total_EF_physics_rate      += prateW.first;
      total_EF_physics_rate_err2 += prateW.second*prateW.second;
    }

    stringstream uniqStr;
    uniqStr << rrateU.first;
    
    os << "<td align=\"center\"> " << rfracW.first  << " </td>"
       << "<td align=\"center\"> " << rrateW.first  << " </td>"
       << "<td align=\"center\"> " << rrateW.second << " </td>"
      //<< "<td align=\"center\"> " << uniqStr.str() << " </td>"      	
       << "</tr>" << endl;

    nodeGR->GetCont() 
      << "Group name:       " << group->GetName() << endl
      << "Pass events:      " << nevtW << endl
      << "Effective PS:     " << effps << endl
      << "Pass fraction:    " << rfracW.first << endl
      << "Accept rate:      " << rrateW.first << " +/- " << rrateW.second << " Hz" << endl
      //      << "Unique rate:      " << uniqStr.str() << " Hz" << endl
      << "Number of chains: " << group->GetChains().size() << endl
      << endl                                                                               
      << "Using raw event counts: " << endl                                                 
      << "Pass events (raw counts):      " << nevtR << endl                                              
      << "Accept rate:      " << rrateR.first << " +/- " << rrateR.second << " Hz" << endl;

      
    //
    // Write chain rates
    //
    string opts = "SORT";
    if (group->GetName()=="EF_express") {
      opts += " EXPRESS";
    }

    WriteChainTable(nodeGR, hmenu, level, opts, group->GetName(), group->GetChains());
    
    //
    // Write overlap table
    //
    nodeGO->GetCont() << nodeGR->GetLink(group->GetName()) 
		      << " overlap rates with other " << titleO.str() << ":" << endl << endl;
    Html::WriteOverlap(nodeGO->GetCont(), group, fRates->GetRateUnions());
    
    nodeGR -> GetCont() << "</pre>" << endl;
    nodeGO -> GetCont() << "</pre>" << endl;
  } 
 
   
  WriteOverlRates(nodeO, option, selected_groups, fRates->GetRateUnions());
  nodeO -> GetCont() << "<pre>";

  os << "</table>" << endl;
  if (!is_group)
    os << "Total EF Physics Rate:       " 
       << total_EF_physics_rate << " +/- " 
       << sqrt(total_EF_physics_rate_err2) << " Hz" << endl;

  os << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteGroupRates(Handle<HtmlNode> node,
				     HtmlMenu &hmenu, 
				     const std::string &option,
				     std::vector<Handle<RateGroup> > group_vec)
{
  //
  // Write HTML table of chain rates within a group
  //
  if(!node.valid()) {
    return;
  }

  node->GetCont() << "<pre>";

  if(option.find("TOP") != string::npos) {
    node -> AddLink("top ", "../", false);
    node -> AddLink("index ", "index.html", false); 
    node -> GetCont() << endl << endl;
  }


  // create columns for the summary pages
  int cwidth = 80;
  if(group_vec.size() > 0) {
    cwidth /= group_vec.size();
  }

  node->GetCont() 
    << "<table width=\"80%\" cellpadding=\"0\" cellspacing=\"10\" border=\"0\">" << endl
    << "   <tr>" << endl;

  std::vector<Handle<RateChain> > chains_merged;
  for(unsigned int i = 0; i < group_vec.size(); ++i) {
    const Handle<RateGroup> &group = group_vec.at(i);
    chains_merged.insert(chains_merged.end(), group->GetChains().begin(), group->GetChains().end());

    const double nevtW = group->GetEventsPassed();
    const double nevtR = group->GetEventsPassedRaw();
  
    double effps = 0.0;
    if(nevtW > 0.0) effps = nevtR/nevtW;
  
    const std::pair<double, double> pfracW = group->GetPassFrac();
    const std::pair<double, double> prateW = group->GetPassRate();
    const std::pair<double, double> prateR = group->GetPassRateRaw();
    const std::pair<double, double> prateU = group->GetRateUnique();
    
    const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
    const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);
    const std::pair<string, string> rrateR = Anp::Round2Pair(prateR.first, prateR.second);
    const std::pair<string, string> rrateU = Anp::Round2Pair(prateU.first, prateU.second);

    stringstream lfile_indiv, lcont_indiv;
    lfile_indiv << "rates_indiv_chain_" << group->GetName() << ".html";
    lcont_indiv << "Group name: " << group->GetName();

    node->GetCont() 
      << "      <td width=\"" << cwidth << "%\" valign=\"top\">" << endl
      << "         <p>" << endl; 
    node->AddLink(lcont_indiv.str(), lfile_indiv.str(), false); 
    node->GetCont() 
      << "<br>" << endl
      << "            Effective PS:     " << effps << "<br>" << endl
      << "            Pass fraction:    " << rfracW.first << "<br>" << endl
      << "            Pass events:      " << nevtW << "<br>" << endl
      << "            Accept rate:      " << rrateW.first << " +/- " << rrateW.second << " Hz" << "<br>" << endl
      << "            Number of chains: " << group->GetChains().size() << "<br>" << endl
      << "            Using raw event counts: " << "<br>" << endl                                                 
      << "            Pass events (raw counts): " << nevtR << "<br>" << endl                                              
      << "            Accept rate:      " << rrateR.first << " +/- " << rrateR.second << " Hz" << endl
      << "         </p>" << endl
      << "      </td>" << endl
      << endl;
  } 

  // close the columns
  node->GetCont() 
    << "   </tr>" << endl
    << "</table>" << endl;

  //
  // Write chain table
  //
  const Handle<RateGroup> &group = group_vec.at(0);
  WriteChainTable(node, hmenu, group->GetLevel(), option, group->GetName(), chains_merged);
  
  node -> GetCont() << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteOverlRates(Handle<HtmlNode> node,
				     const std::string &option,
				     vector<Handle<RateGroup> > groups,
				     vector<Handle<RateGroup> > unions)
{
  //
  // Write overlap group rates
  //
  if(groups.empty() || unions.empty()) {
    return;
  }
  
  //
  // Write out sorted chains
  //
  if(option.find("SORT_RATE") != string::npos) {
    std::sort(groups.begin(), groups.end(), Anp::RateGroupSort("rate"));
  }
  else if(option.find("SORT_NAME") != string::npos) {
    std::sort(groups.begin(), groups.end(), Anp::RateGroupSort(""));
  }

  unsigned group_size = 0;

  for(unsigned i = 0; i < groups.size(); ++i) {
    const Handle<RateGroup> &group = groups.at(i);
    if(group->GetPassRate().first > 0.0) {
      group_size = i+1;
    }
  }
  
  if(group_size < 2) {
    cerr << "HtmlRates::WriteOverlRates - number of groups is too small " << endl;
    return;
  }
  
  ostream &os = node->GetCont();  
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr><th align=\"left\">&nbsp</th>";
  
  //
  // Write group names as headers
  //
  for(unsigned i = 0; i < group_size; ++i) {
    const string &group_name = groups.at(i)->GetName();
   
    if(group_name.find("express") != string::npos) {
      continue;
    }
    Handle<HtmlNode> node;

    GroupMap::iterator igroup = fGroup.find("GROUP_"+group_name);
    if(igroup != fGroup.end()) {
      node = igroup->second;
    }
      
    if(node.valid()) {
      os << "<th>" << node->GetLink(group_name) << "</th>";
    }
    else {
      os << "<th>" << group_name << "</th>";
    }
  }
  os << "<th align=\"left\">&nbsp</th></tr>" << endl;
  
  //
  // Write overlap rates
  //
  for(unsigned i = 0; i < group_size; ++i) {
    const Handle<RateGroup> &group1 = groups.at(i);
    Handle<HtmlNode> node1;
    if(group1->GetName().find("express") != string::npos) {
      continue;
    }

    GroupMap::iterator igroup1 = fGroup.find("GROUP_"+group1->GetName());
    if(igroup1 != fGroup.end()) {
      node1 = igroup1->second;
    }

    std::string group1_name;
    if(node1.valid()) {
      group1_name = node1->GetLink(group1->GetName());
    }
    else {
      group1_name = group1->GetName();
    }

    os << "<tr><th align=\"left\">" << group1_name << "</th>";

    for(unsigned j = 0; j < group_size; ++j) {
      const Handle<RateGroup> &group2 = groups.at(j);
      Handle<HtmlNode> node2;
      if(group2->GetName().find("express") != string::npos) {
        continue;
      }

      GroupMap::iterator igroup2 = fGroup.find("GROUP_"+group2->GetName());
      if(igroup2 != fGroup.end()) {
	node2 = igroup2->second;
      }
     
      Handle<RateGroup> groupU;
      for(unsigned int u = 0; u < unions.size(); ++u) {
	groupU = unions.at(u);
	
	if(groupU->GetGroup1() == group1 && groupU->GetGroup2() == group2) {
	  break;
	}
	else if(groupU->GetGroup1() == group2 && groupU->GetGroup2() == group1) {
	  break;
	}
	else {
	  groupU.release();
	}
      }
      
      if(groupU.valid() && i < j) {
	const pair<double, double> orateD = groupU->GetRateOverlap();
	const pair<string, string> orateS = Anp::Round2Pair(orateD.first, 1.0);

	os << "<td>" << orateS.first << " Hz</td>";
	
	if(fDebug) {
	  cout << group1->GetName() << ": " << group2->GetName()
	       << orateS.first << endl;
	}
      }
      else if(i == j) {
	const pair<double, double> grateD = group1->GetPassRate();
	const pair<string, string> grateS = Anp::Round2Pair(grateD.first, 1.0);

	os << "<td>" << grateS.first << " Hz</td>";
      }
      else {
	os << "<td>&nbsp</td>";
      }
    }

    os << "<th align=\"left\">" << group1_name << "</th></tr>";
  }

  os << "</table>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteBandwidths(std::ostream &os)
{  
  log() << "Writing Bandwidth HTML" << endl;

  //
  // Write HTML table of rates at given level
  // 
  if(!fRates.valid()) { 
    log() << "No valid rates" << endl;
    return;
  }

  Handle<RateBand> rateBand = fRates->GetRateBand();
  if(!rateBand.valid()) { 
    log() << "No bandwidth rates" << endl;
    return;
  }

  vector<pair<string,string> > ratesCorr = rateBand -> GetBwCorr();
  vector<pair<vector<string>, string> > bwOlap = rateBand -> GetBwOlaps();

  if(fDebug) {
    log() << "WriteBandwidths - size of overlaps = " << ratesCorr.size() << "-" << bwOlap.size() << endl;
  }

  os << "<html>\n"
     << "<head>\n"
     << "<TITLE>Rate Tables</TITLE>\n"
     << "<body bgcolor=#FFFFFF text=#000000 link=#0000FF vlink=#000080 alink=#800000>\n"
     << "<div align='center'>\n" 
     << "<a href=#L1BWCalc>EF Bandwitdh Calculation</a><br/>\n"
     << "<a href=#L1BWOlap>EF Bandwidth overlaps</a><br/>" << endl;
     
  os << "<a name=EFBWCalc><p><b>EF Bandwidth rates</b></p></a>\n"
     << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<table border=1 frame=box rules=cols>\n" 
     << "\t<tr>\n"
     << "\t\t<td align=CENTER><b>Group</b></td>\n"
     << "\t\t<td align=CENTER><b>Original Rate</b></td>\n"
     << "\t\t<td align=CENTER><b>Corrected Rate</b></td>\n"
     << "\t</tr>" << endl;
     
  for(unsigned i = 0; i < ratesCorr.size(); ++i) {
    if(ratesCorr[i].first.find("_Corr") != string::npos) {
      continue;
    }
    const string &name = ratesCorr[i].first;
    string corrected = "";
    for(unsigned j = 0; j < ratesCorr.size(); ++j) {
      if(ratesCorr[j].first == name+"_Corr") {
        corrected = ratesCorr[j].second;
        break;
      }
    }
    os << "\t<tr>\n"
       << "\t\t<td align=LEFT><b>" << name << "</b></td>" << endl 
       << "\t\t<td align=LEFT><b>" << ratesCorr[i].second << "</b></td>" << endl
       << "\t\t<td align=LEFT><b>" << corrected << "</b></td>" << endl
       << "\t</tr>" << endl;
  }

  os << "</table>" << endl;
  
  //find largest overlap order   
  unsigned int order = 0;
  
  for(unsigned int i = 0; i < bwOlap.size(); ++i) {
    if(bwOlap[i].first.size() > order) {
      order = bwOlap[i].first.size();
    }
  }
  
  for(unsigned i = 2; i <= order; ++i) {
    os << "<a name=EFBWCalc><p><b>EF Bandwidth overlaps of order "<< i << " </b></p></a>\n"
       << "<table border=1 frame=box rules=cols>\n" 
       << "\t<tr>\n"
       << "\t\t<td align=CENTER><b>Overlapping triggers</b></td>\n"
       << "\t\t<td align=CENTER><b>Overlap Rate</b></td>" << endl;
    
    for(unsigned j = 0; j < bwOlap.size(); ++j) {
      if(bwOlap[j].first.size() != i) continue;
      os << "\t<tr>" << endl;
      os << "\t\t<td align=LEFT><b>";
      for(unsigned int tr = 0; tr < bwOlap[j].first.size(); ++tr) {
	os << bwOlap[j].first[tr] << ",";
      }

      os << "</b></td>" << endl;
      os << "\t\t<td align=LEFT><b>" 
	 << bwOlap[j].second << "</b></td>" << endl;
      os << "\t</tr>" << endl;
    }
    os << "</table>" << endl;
  }
}

//-----------------------------------------------------------------------------
namespace Anp
{
  namespace Html
  {
    template<class T>
    std::ostream& xml_line(std::ostream &os,
			   unsigned int nspace,
			   const std::string &tag,
			   const T &val) {    
      for(unsigned i = 0; i < nspace; ++i) os << " ";
      os << "<" << tag << ">" << val << "</" << tag << ">\n";
      return os;
    }
    std::ostream& xml_open(std::ostream &os,
			   unsigned int nspace,
			   const std::string &tag) {
      for(unsigned i = 0; i < nspace; ++i) os << " ";
      os << "<" << tag << ">\n";
      return os;
    }
    std::ostream& xml_stop(std::ostream &os,
			   unsigned int nspace,
			   const std::string &tag) {
      for(unsigned i = 0; i < nspace; ++i) os << " ";
      os << "</" << tag << ">\n";
      return os;
    }
    std::ostream& xml_level(std::ostream &os,
			    unsigned nspace, string level, 
			    const std::vector<Anp::Handle<Anp::RateGroup> > &groups) {
      //
      // Write level specific information
      //
      vector<Anp::Handle<Anp::RateGroup> >::const_iterator it = 
	std::find(groups.begin(), groups.end(), level);
     
      // level not found?
      if(it == groups.end()) { 
	xml_line(os, nspace, "lvl_name",              level);
	xml_line(os, nspace, "lvl_eff",               0.0);
	xml_line(os, nspace, "lvl_eff_err",           0.0);
	xml_line(os, nspace, "lvl_prescaled_eff",     0.0);
	xml_line(os, nspace, "lvl_prescaled_eff_err", 0.0);
	xml_line(os, nspace, "lvl_rate",              0.0);
	xml_line(os, nspace, "lvl_rate_err",          0.0);
	return os;
      }

      const Handle<Anp::RateGroup> &group = *it;
      
      const std::pair<double, double> pfracW = group->GetPassFrac();
      const std::pair<double, double> prateW = group->GetPassRate();
      
      const std::pair<double, double> pfracC = group->GetPassFracRaw();
      const std::pair<double, double> prateC = group->GetPassRateRaw();
      
      const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
      
      const std::pair<string, string> rfracC = Anp::Round2Pair(pfracC.first, pfracC.second);
      const std::pair<string, string> rrateC = Anp::Round2Pair(prateC.first, prateC.second);

      // for the rate, see if group _rd0 and _rd1 exist, if so add them to total rate
      double level_rate = prateW.first;
      double level_rate_err = prateW.second*prateW.second;

      for(int ird=0; ird<2; ird++){
	string level_rd = level;
	if(ird==0) 
	  level_rd += "_rd0";
	else 
	  level_rd += "_rd1";

	vector<Anp::Handle<Anp::RateGroup> >::const_iterator it_rd = std::find(groups.begin(), groups.end(), level_rd);
	if(it_rd != groups.end()) {
	  const Handle<Anp::RateGroup> &group_rd = *it_rd;
	  const std::pair<double, double> prateW_rd = group_rd->GetPassRate();
	  level_rate += prateW_rd.first;
	  level_rate_err += prateW_rd.second*prateW_rd.second;
	}
      } // end of addition of rd0 and rd1 rates
      level_rate_err = sqrt(level_rate_err);
      const std::pair<string, string> rrateW = Anp::Round2Pair(level_rate, level_rate_err);
      
      xml_line(os, nspace, "lvl_name",              level);
      xml_line(os, nspace, "lvl_eff",               rfracC.first);
      xml_line(os, nspace, "lvl_eff_err",           rfracC.second);
      xml_line(os, nspace, "lvl_prescaled_eff",     rfracW.first);
      xml_line(os, nspace, "lvl_prescaled_eff_err", rfracW.second);
      xml_line(os, nspace, "lvl_rate",              rrateW.first);
      xml_line(os, nspace, "lvl_rate_err",          rrateW.second);

      return os;
    }
    std::ostream& xml_group(std::ostream &os,
	                    const unsigned nspace, 
			    const Anp::Handle<Anp::RateGroup> &group,
	                    const std::string &group_type,
	                    const bool write_chains) {
      //
      // Write rates for one group
      //
      if(!group.valid()) {
	return os;
      }
      
      const std::pair<double, double> pfracW = group->GetPassFrac();
      const std::pair<double, double> prateW = group->GetPassRate();
      const std::pair<double, double> prateU = group->GetRateUnique();
      
      const std::pair<double, double> pfracC = group->GetPassFracRaw();
      const std::pair<double, double> prateC = group->GetPassRateRaw();
      
      const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
      const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);   
      const std::pair<string, string> rrateU = Anp::Round2Pair(prateU.first, prateU.second);
      
      const std::pair<string, string> rfracC = Anp::Round2Pair(pfracC.first, pfracC.second);
      const std::pair<string, string> rrateC = Anp::Round2Pair(prateC.first, prateC.second);
      
      double effps = 0.0;
      if(group->GetEventsPassed() > 0.0) {
	effps = group->GetEventsPassedRaw()/group->GetEventsPassed();
      }
      
      Html::xml_line(os, nspace, "level",                    group->GetLevel());
      Html::xml_line(os, nspace, "sig_name",                 group->GetName());
      Html::xml_line(os, nspace, "type",                     group_type);
      
      Html::xml_line(os, nspace, "prescale",                 effps);
      Html::xml_line(os, nspace, "efficiency",               rfracC.first);
      Html::xml_line(os, nspace, "efficiency_err",           rfracC.second);
      Html::xml_line(os, nspace, "prescaled_efficiency",     rfracW.first);
      Html::xml_line(os, nspace, "prescaled_efficiency_err", rfracW.second);
      Html::xml_line(os, nspace, "rate",                     rrateW.first);
      Html::xml_line(os, nspace, "rate_err",                 rrateW.second);
      Html::xml_line(os, nspace, "unique_rate",              rrateU.first);
      
      if(write_chains) {
	const std::vector<Handle<RateChain> > &chains = group->GetChains();
	Html::xml_open(os, nspace, "components");
	
	for(unsigned j = 0; j < chains.size(); ++j) {
	  const Handle<RateChain> &chain = chains.at(j);
	  Html::xml_open(os, nspace, "sig");
	  Html::xml_line(os, nspace, "sig_name", chain->GetName());
	  Html::xml_stop(os, nspace, "sig");
	}

	Html::xml_stop(os, nspace, "components");
      }
      
      return os;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlRates::WriteXML(std::ostream &os)
{
  //
  // Write XML summary of trigger rates
  //
  if(!fRates.valid()) return;

  const vector<Handle<RateChain> > &chains = fRates -> GetRateChains();
  const vector<Handle<RateGroup> > &groups = fRates -> GetRateGroups();

  string geff, lumi, xsec, ntot;
  Registry mReg = fRates->GetIndexRegistry(); 
  
  mReg.Get("Generator efficiency",  geff);
  mReg.Get("Luminosity",            lumi);
  mReg.Get("Cross section",         xsec);
  mReg.Get("Number of events",      ntot);   

  string pred_lumi;
  std::vector<string> bg_num(8);
  mReg.Get("Prediction Lumi",       pred_lumi);
    
  os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> \n"
     << "<trigger> \n"
     << "    <Xsection>"   << std::atof(xsec.c_str())*1e+24 << "</Xsection> \n"
     << "    <Luminosity>" << std::atof(lumi.c_str())*1e-24 << "</Luminosity> \n"
     << "    <GenEff>"     << geff                          << "</GenEff> \n"
     << "    <n_evts>"     << ntot                          << "</n_evts> \n"
     << "    <PredictionLumi>" << std::atof(pred_lumi.c_str()) << "</PredictionLumi> \n";

  // print the data sets
  string dataset_name, ndataset;
  mReg.Get("Number of datasets",        ndataset);
  for(int ifile=0; ifile<std::atoi(ndataset.c_str()); ifile++) {
    std::stringstream sval_set;
    sval_set << "Dataset  " << ifile+1;
    mReg.Get(sval_set.str(), dataset_name);
    os << "    <Dataset>" << dataset_name << "</Dataset> \n";
  }
  

  // Read BG keys
  Registry bgReg;
  mReg.Get("BunchGroups", bgReg);
  for(int ibg=0; ibg<8; ibg++){
    std::stringstream sval_bg;
    sval_bg << "Prediction BGRP" << ibg;
    bgReg.Get(sval_bg.str(),           bg_num[ibg]);
  }
  os << "    <bunchgroup>\n";
  for(int ibg=0; ibg<8; ibg++){
    os << "         <bunchgrouptype>\n";
    os << "              <bunchgroup_keynum>" << ibg << "</bunchgroup_keynum> \n";
    os << "              <bunchgroup_key>BGRP" << ibg << "</bunchgroup_key> \n";
    os << "              <bunchgroup_size>" << std::atoi(bg_num.at(ibg).c_str()) << "</bunchgroup_size> \n";
    os << "         </bunchgrouptype>\n";
    // os << "    <PredictionBGRP" << ibg << ">" << std::atoi(bg_num.at(ibg).c_str()) << "</PredictionBGRP" << ibg << "> \n";
  }
  os << "    </bunchgroup>\n";
  
  //
  // Read keys from TrigMonConfig: these are copied by MakeRates into Registry
  //
  Registry cReg;
  mReg.Get("MakeRates", cReg);

  for(unsigned i = 0; i < fConfigKeys.size(); ++i) {
    string value;
    if(cReg.Get(fConfigKeys.at(i), value)) {
      os << "    <" << fConfigKeys.at(i) << ">" << value << "</" << fConfigKeys.at(i) << ">\n";
    }
  }

  //
  // Write individual chains
  //
  for(unsigned int level = 1; level < 4; ++level) {
    os << "    <level>\n";
    
    string levelS;
    if(level == 1) Html::xml_level(os, 8, "L1", fRates->GetRateLevels());
    if(level == 2) Html::xml_level(os, 8, "L2", fRates->GetRateLevels());
    if(level == 3) Html::xml_level(os, 8, "EF", fRates->GetRateLevels());
    
    for(unsigned int i = 0; i < chains.size(); ++i) {
      const Handle<RateChain> &chain = chains.at(i);
      const TrigConfChain &confg = chain->GetChain();
      
      if(chain->GetLevel() != level) continue;
      
      const std::pair<double, double> pfracW = chain->GetPassFrac();
      const std::pair<double, double> prateW = chain->GetPassRate();
      
      const std::pair<double, double> pfracU = chain->GetFracUnique();
      const std::pair<double, double> prateU = chain->GetRateUnique();
      
      const std::pair<double, double> pfracC = chain->GetPassFracRaw();
      const std::pair<double, double> prateC = chain->GetPassRateRaw();
 
      const double passed_event_W = chain->GetEventsPassed();
      const double passed_event_R = chain->GetEventsPassedRaw();

      const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
      const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);

      const std::pair<string, string> rfracU = Anp::Round2Pair(pfracU.first, pfracU.second);
      const std::pair<string, string> rrateU = Anp::Round2Pair(prateU.first, prateU.second);
      
      const std::pair<string, string> rfracC = Anp::Round2Pair(pfracC.first, pfracC.second);
      const std::pair<string, string> rrateC = Anp::Round2Pair(prateC.first, prateC.second);
         
      double unique_frac = 0.0;
      if(prateW.first > 0.0) {
	unique_frac = prateU.first/prateW.first;
      }

      os << "        <signature> \n";
      
      Html::xml_line(os, 12, "sig_name",                 confg.getName());
      Html::xml_line(os, 12, "prescale",                 chain->GetTotalPS());
      Html::xml_line(os, 12, "express_stream",           chain->GetExpressPS());
      Html::xml_line(os, 12, "passthrough",              confg.getPassThrough());
      Html::xml_line(os, 12, "chain_prescale",           confg.getPrescale());
      Html::xml_line(os, 12, "lower_chain_name",         confg.getLowerName());
      Html::xml_line(os, 12, "evts_passed",              passed_event_R);
      Html::xml_line(os, 12, "evts_passed_weighted",     passed_event_W);
      Html::xml_line(os, 12, "efficiency",               rfracC.first);
      Html::xml_line(os, 12, "efficiency_err",           rfracC.second);
      Html::xml_line(os, 12, "prescaled_efficiency",     rfracW.first);
      Html::xml_line(os, 12, "prescaled_efficiency_err", rfracW.second);
      Html::xml_line(os, 12, "rate",                     rrateW.first);
      Html::xml_line(os, 12, "rate_err",                 rrateW.second);
      Html::xml_line(os, 12, "indep_fraction",           rfracU.first);
      Html::xml_line(os, 12, "unique_fraction",          unique_frac);
      Html::xml_line(os, 12, "unique_rate",              rrateU.first);

      os << "        </signature> \n";
    }
    
    os << "    </level>" << endl;
  }

  //
  // Write groups
  //
  for(unsigned int i = 0; i < groups.size(); ++i) {
    const Handle<RateGroup> &group = groups.at(i);

    std::string gtype = "Group";
    if(group->GetName().find("cumu_rev") != string::npos) {
      gtype = "CumulativeReverse";
    }
    else if( group->GetName().find("cumu") != string::npos) {
     gtype = "Cumulative";
    }
    
    if(!fDoCumultvXML && gtype != "Group") {
      continue;
    }

    Html::xml_open (os, 4, "cplx_signature");
    Html::xml_group(os, 8, group, gtype, true);
    Html::xml_stop (os, 4, "cplx_signature");
  }

  //
  // Write groups
  //
  const vector<Handle<RateGroup> > &stream = fRates -> GetRateStream();

  for(unsigned int i = 0; i < stream.size(); ++i) {
    const Handle<RateGroup> &sgroup = stream.at(i);
    
    Html::xml_open (os, 4, "cplx_signature");
    Html::xml_group(os, 8, sgroup, "Stream", true);
    Html::xml_stop (os, 4, "cplx_signature");
  }
  
  //
  //  Write overlap groups
  //  
  if(fDoOverlapXML) {
    const vector<Handle<RateGroup> > &unions = fRates -> GetRateUnions();
    
    for(unsigned int i = 0; i < unions.size(); ++i) {
      const Handle<RateGroup> &ugroup = unions.at(i);
      
      const std::pair<double, double> pfracW = ugroup->GetFracOverlap();
      const std::pair<double, double> prateW = ugroup->GetRateOverlap();
      
      const std::pair<string, string> rfracW = Anp::Round2Pair(pfracW.first, pfracW.second);
      const std::pair<string, string> rrateW = Anp::Round2Pair(prateW.first, prateW.second);   
      
      double effps = 0.0;
      if(ugroup->GetEventsPassed() > 0.0) {
	effps = ugroup->GetEventsPassedRaw()/ugroup->GetEventsPassed();
      }
      
      Html::xml_open(os, 4, "cplx_signature");
      Html::xml_line(os, 8, "level",                    ugroup->GetLevel());
      Html::xml_line(os, 8, "sig_name",                 ugroup->GetName());
      Html::xml_line(os, 8, "type", "                   Overlap");
      Html::xml_line(os, 8, "prescale",                 effps);
      Html::xml_line(os, 8, "prescaled_efficiency",     rfracW.first);
      Html::xml_line(os, 8, "prescaled_efficiency_err", rfracW.second);
      Html::xml_line(os, 8, "rate",                     rrateW.first);
      Html::xml_line(os, 8, "rate_err",                 rrateW.second);
      
      Html::xml_open(os, 8, "components");
      
      Html::xml_open(os, 8, "sig");
      Html::xml_line(os, 8, "sig_name", ugroup->GetGroup1()->GetName());
      Html::xml_stop(os, 8, "sig");
      Html::xml_open(os, 8, "sig");
      Html::xml_line(os, 8, "sig_name", ugroup->GetGroup2()->GetName());
      Html::xml_stop(os, 8, "sig");
      
      Html::xml_stop(os, 8, "components");
      Html::xml_stop(os, 4, "cplx_signature");
    }
  }
  
  os << "</trigger>" << endl;
}
