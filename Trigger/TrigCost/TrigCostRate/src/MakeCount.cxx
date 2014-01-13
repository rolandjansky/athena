/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <string.h>

// Boost                                                                                                                                 
#include <boost/foreach.hpp>

// ROOT
#include "TH1.h"
#include "TCanvas.h"
//#include "TFile.h"
//#include "TTree.h"
//#include "TBranch.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/MakeCount.h"

REGISTER_ANP_OBJECT(AlgEvent,MakeCount)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::MakeCount::MakeCount()
  :fDir(0),
   fDebug(false),
   fFillHistBCID(false),
   fHistKey("MakeCount"),
   fIsConfig(false),
   fMakeRatesNtup(false),
   fNEvents(0),
   fNSwitch(0),
   fCountCoolL1(0),
   fCountCoolL2(0),
   fCountCoolEF(0),
   fCountThisL1(0),
   fCountThisL2(0),
   fCountThisEF(0),
   fTotalDuration(0),
   fRatesNtup(0),
   fBL1(0),
   fBL2(0),
   fBEF(0)
{
}

//-----------------------------------------------------------------------------
Anp::MakeCount::~MakeCount()
{
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Config(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("MakeCount", "Debug",     fDebug);
  reg.Get("MakeCount::CoolLumi",    fCoolLumiName);
  reg.Get("MakeCount::FillBCID",    fFillHistBCID);
  reg.Get("MakeCount::HistKey",     fHistKey);
  reg.Get("MakeCount::RateNtuple",  fMakeRatesNtup);

  //
  // Initiliaze input data pointers
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  
  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "MakeCount::ConfAlg", fConf, this);
  
  //
  // Register self for html writing callback and clients
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
  Factory<LumiABC>::Instance().Hold(AlgEvent::GetName(), Handle<LumiABC>(this));

  //
  // If Writing rates ntuple, then intialize
  //
  if(fMakeRatesNtup) InitNtuple();
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback
  //
  if(signal == "TRIG_CONFIG" || signal == "TRIG_CONFIG_DB") {    
    //
    // Read trigger configuration
    //
    if(fConf.valid()) { 
      Set(fConf->GetConfig()); 
    }
    else {
      log() << "Signal - invalid configuration algorithm" << endl;
    }
  }
}


//-----------------------------------------------------------------------------
void Anp::MakeCount::Save(TDirectory *dir)
{ 
  fDir = Anp::GetDir(dir, AlgEvent::GetName());
}

//-----------------------------------------------------------------------------
bool Anp::MakeCount::Init()
{  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Exec()
{
  //
  // Process data for one event
  // 
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Done()
{
  cout << "MakeCount::Done " << endl;

  //
  // Clear null pointers
  //
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    fInfos.at(i).RemoveNullCountPointers();
  }

  //
  // Setting the scale factors
  //
  if(!fCoolLumiName.empty()) {

    fCoolLumi = Factory<LumiABC>::Instance().Get(fCoolLumiName);
    
    if(fCoolLumi.valid()) {
      log() << "Done - using COOL info algorithm: " << fCoolLumiName << endl;

      FillRateScaleCorr(fCoolLumi->GetInfos());
    }
    else {
      log() << "Done - missing COOL info algorithm: " << fCoolLumiName << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Write(HtmlNode &inode, HtmlMenu &/*hmenu*/)
{
  //
  // Create node for this algorithm
  //
  Handle<HtmlNode> node(new HtmlNode(AlgEvent::GetName()+"_table.html", HtmlNode::kHTML));
  inode.AddChild(node);
  inode.GetCont() << node->GetLink("Online trigger counts per lumi block (from cost)") << endl;
  node->SetBase(AlgEvent::GetName());

  //
  // Write per BCID histograms
  //

  Handle<HtmlNode> nodeL1(new HtmlNode(AlgEvent::GetName()+"_bcid_tableL1.html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeL2(new HtmlNode(AlgEvent::GetName()+"_bcid_tableL2.html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeEF(new HtmlNode(AlgEvent::GetName()+"_bcid_tableEF.html", HtmlNode::kHTML));

  inode.AddChild(nodeL1);
  inode.AddChild(nodeL2);
  inode.AddChild(nodeEF);

  node->GetCont() 
    << "<pre>"
    << nodeL1->GetLink("L1 counts per bcid") << endl
    << nodeL2->GetLink("L2 counts per bcid") << endl
    << nodeEF->GetLink("EF counts per bcid") << endl
    << "</pre>"
    << endl;
  
  if(fFillHistBCID) {
    FillHistBCID(nodeL1.ref(), fBCIDHistL1);
    FillHistBCID(nodeL2.ref(), fBCIDHistL2);
    FillHistBCID(nodeEF.ref(), fBCIDHistEF);
  }

  //
  // Write out count tables per lb
  //
  LumiABC::WriteInfoTable(node.ref());  

  //
  // Write out flat ntuple with rates vs bcid
  //
  if(fMakeRatesNtup) WriteNtuple();
}

//-----------------------------------------------------------------------------
long double Anp::MakeCount::GetRateCorrection(unsigned level) const
{
  //
  // Return scale correction to convert overall counts to Hz
  //
  if(!(fTotalDuration > 0.0)) {    
    return 1.0;
  }

  if(level == 1 && fCountThisL1 > 0.0) {    
    return fCountCoolL1/(fCountThisL1*fTotalDuration);
  }
  if(level == 2 && fCountThisL2 > 0.0) {    
    return fCountCoolL2/(fCountThisL2*fTotalDuration);
  }
  if(level == 3 && fCountThisEF > 0.0) {    
    return fCountCoolEF/(fCountThisEF*fTotalDuration);
  }

  return 1.0;
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::FillRateScaleCorr(const std::vector<Lumi::BlockInfo> &cool_infos)
{
  //
  // Fill rate scale corrrections
  //

  //
  // Looping over all lumi blocks
  //  
  for(unsigned i = 0; i < fInfos.size(); ++i) {    
    Lumi::BlockInfo &this_info = fInfos.at(i);
    
    //
    // Loop over all cool entries
    //
    unsigned nmatch = 0;

    for(unsigned j = 0; j < cool_infos.size(); ++j) {    
      const Lumi::BlockInfo &cool_info = cool_infos.at(j);

      //
      // Fill corrections for two matching lumi blocks
      //
      if(this_info.run == cool_info.run && this_info.lb == cool_info.lb) {
	FillRateScaleCorr(this_info, cool_info);
	++nmatch;
      }
    }

    if(nmatch != 1) {
      log() << "FillRateScaleCorr - missing COOL info for: run=" 
	    << this_info.run << " lb=" << this_info.lb << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::FillRateScaleCorr(Lumi::BlockInfo &this_info, 
				       const Lumi::BlockInfo &cool_info)
{    
  //
  // Compute scale correction for current lumi block 
  // 
  const Lumi::LV1Count* l1cost = GetHighestRateLV1Count(this_info);
  const Lumi::HLTCount* l2cost = GetHighestRateHLTCount(this_info, 2);
  const Lumi::HLTCount* efcost = GetHighestRateHLTCount(this_info, 3);
    
  //
  // L1 scale
  //
  if(cool_info.L1accept > 0) {
    //
    // L1 and L2 scale using total L1 accept count
    //
    if(this_info.L1accept > 0) {
      this_info.scaleCorrL1 = double(cool_info.L1accept)/double(this_info.L1accept);
      this_info.scaleCorrL2 = double(cool_info.L1accept)/double(this_info.L1accept);

      fCountCoolL1 += cool_info.L1accept;
      fCountCoolL2 += cool_info.L1accept;
      
      fCountThisL1 += this_info.L1accept;
      fCountThisL2 += this_info.L1accept;
    }
  }
  else {
    //
    // L1 and L2 scale using highest rate chain
    //
    if(l1cost){    
      const Lumi::LV1Count *l1cool = cool_info.GetL1Count(l1cost->ctp_id);
      
      if(l1cool && l1cost->count_tav > 0) {
	this_info.scaleCorrL1 = double(l1cool->count_tav)/double(l1cost->count_tav);

	fCountCoolL1 += l1cool->count_tav;
	fCountThisL1 += l1cost->count_tav;
      }
    }
    
    if(l2cost){      
      const Lumi::HLTCount* l2cool = cool_info.GetL2Count(l2cost->chain_counter);
      
      if(l2cool && l2cost->count_total > 0) { 
	this_info.scaleCorrL2 = double(l2cool->count_total)/double(l2cost->count_total);

	fCountCoolL2 += l2cool->count_total;
	fCountThisL2 += l2cost->count_total;
      }
    }
  }
  
  //
  // EF scale
  //
  if(efcost) {
    const Lumi::HLTCount* efcool = cool_info.GetEFCount(efcost->chain_counter);
    
    if(efcool && efcost->count_total > 0) { 
      this_info.scaleCorrEF = double(efcool->count_total)/double(efcost->count_total);

      fCountCoolEF += efcool->count_total;
      fCountThisEF += efcost->count_total;
    }
  }

  //
  // Scale by duartion of LB in seconds
  //
  double lb_scale_factor = 0.0;

  if(cool_info.GetLBDuration() > 1.0 && cool_info.lifetimeFrac > 0.01) {
    const double live_dt = cool_info.GetLBDuration()*cool_info.lifetimeFrac;
    lb_scale_factor = 1.0/live_dt;
    fTotalDuration += live_dt;
  }

  if(fDebug) {
    log() << "step 1: scaleCorrL1=" << this_info.scaleCorrL1 << endl;
  }

  this_info.scaleCorrL1 *= lb_scale_factor;
  this_info.scaleCorrL2 *= lb_scale_factor;
  this_info.scaleCorrEF *= lb_scale_factor;

  //
  // Copy additional information
  //
  this_info.lumiBegTime = cool_info.lumiBegTime;
  this_info.lumiEndTime = cool_info.lumiEndTime;
  this_info.lumiInstan  = cool_info.lumiInstan;

  if(fDebug) {
    log()<< "step 2: scaleCorrL1=" << this_info.scaleCorrL1 << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fConfig = config;
  fIsConfig = true;
}

//-----------------------------------------------------------------------------
void Anp::MakeCount::Run(const TrigMonEvent &event)
{
  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - this algorithm is not configured" << endl;
    return;
  }
  
  ++fNEvents;

  Lumi::BlockInfo &info = GetCurrentLumiBlock(event.getRun(), event.getLumi());

  const std::vector<uint16_t> &event_lv1 = event.getL1Item();
  const std::vector<uint32_t> &event_hlt = event.getChain();

  unsigned L1count = 0, L2count = 0, EFcount = 0;

  // Process LV1
  //
  for(unsigned i = 0; i < event_lv1.size(); ++i) {
    const TrigMonL1Item item(event_lv1.at(i));
    L1count += item.isPassed(TrigMonL1Item::kL1AfterVeto);
    
    TH1 *h = FindHistBCID(1, item.getCtpId());
    if(h) {
      h->Fill(event.getBunchId(), 1.0);
    }

    if(item.getCtpId() >= info.countsL1.size()) {
      continue;
    }

    Lumi::LV1Count *count = info.countsL1.at(item.getCtpId()).get();
    
    if(count) {
      count -> count_tbp += item.isPassed(TrigMonL1Item::kL1BeforePrescale);
      count -> count_tap += item.isPassed(TrigMonL1Item::kL1AfterPrescale);
      count -> count_tav += item.isPassed(TrigMonL1Item::kL1AfterVeto);      
    }
  }

  //
  // Process HLT
  //
  for(unsigned i = 0; i < event_hlt.size(); ++i) {
    const TrigMonChain chain(event_hlt.at(i));
    const unsigned counter = chain.getCounter();
    const unsigned level   = chain.getLevel(); 
    
    TH1 *h = FindHistBCID(level, counter);
    if(h) {
      h->Fill(event.getBunchId(), 1.0);
    }

    L2count += (level == 2 && chain.isPassed(TrigMonChain::kPassed));
    EFcount += (level == 3 && chain.isPassed(TrigMonChain::kPassed));

    Lumi::HLTCount *count = 0;

    if(level == 2) {
      if(counter >= info.countsL2.size()) {
	continue;
      }
      
      count = info.countsL2.at(counter).get();
    }
    else if(level == 3) {
      if(counter >= info.countsEF.size()) {
	continue;
      }
      
      count = info.countsEF.at(counter).get();
    }
    
    if(count) {      
      count -> count_after_ps  = 0; // no data
      count -> count_input    += 1; // no data
      count -> count_total    += chain.isPassed(TrigMonChain::kPassed);
      count -> count_raw      += chain.isPassed(TrigMonChain::kPassedRaw);
    }
  }

  //
  // HACK!!! Count L2 and EF events assuming we deal with online streams:
  //   - L2CostMonitoring: must pass L1
  //   - EFCostMonitoring: must pass L2 and not L1
  //
  // TODO: use only physics stream chains
  // 
  if(L1count > 0) {
    info.L1accept++;
    info.L2accept+= (L2count > 0);
  }
  else {
    if(L2count == 0) {
      log() << "Run - logic error counting L2 accepts for EFCostMonitoring" << endl;
    }

    info.L2accept++;
    info.EFaccept+= (EFcount > 0);
  }
}

//-----------------------------------------------------------------------------
Anp::Lumi::BlockInfo& Anp::MakeCount::GetCurrentLumiBlock(unsigned run, unsigned lb)
{
  //
  // Return info object matching run and lb numbers
  //
  
  //
  // Check if current index matches run and lb
  //
  if(fCurrIndex < fInfos.size()) {
    Lumi::BlockInfo &info = fInfos.at(fCurrIndex);
    
    if(info.run == run && info.lb == lb) {
      return info;
    }
  }

  //
  // Consistency check
  //
  if(!fInfos.empty() && fCurrIndex >= fInfos.size()) {
    log() << "GetCurrentLumiBlock - logic error!" << endl;
  }

  //
  // Look for already existing run and lb
  //
  ++fNSwitch;

  for(unsigned i = 0; i < fInfos.size(); ++i) {
    Lumi::BlockInfo &info = fInfos.at(i);

    if(info.run == run && info.lb == lb) {
      fCurrIndex = i;
      return info;
    }    
  }

  --fNSwitch;

  //
  // Insert new object
  // 
  Lumi::BlockInfo info;
  info.run = run;
  info.lb  = lb;
  
  //
  // Initialize count objects
  //
  unsigned max_counter = 0;
  
  for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
    
    if(chain.getLevelId() != 1) {
      max_counter = std::max<unsigned>(max_counter, chain.getCounter());
    }
  }

  info.countsL1 = std::vector<Ptr<Lumi::LV1Count> >(256,           Ptr<Lumi::LV1Count>());
  info.countsL2 = std::vector<Ptr<Lumi::HLTCount> >(max_counter+1, Ptr<Lumi::HLTCount>());
  info.countsEF = std::vector<Ptr<Lumi::HLTCount> >(max_counter+1, Ptr<Lumi::HLTCount>());

  for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
    
    if(chain.getLevelId() == 1) {
      Ptr<Lumi::LV1Count> ptr = ObjectFactory<Lumi::LV1Count>::Instance().CreateObject();
      ptr->ctp_id   = chain.getCounter();

      info.countsL1.at(chain.getCounter()) = ptr;
    }
    else {
      Ptr<Lumi::HLTCount> ptr = ObjectFactory<Lumi::HLTCount>::Instance().CreateObject();
      ptr->chain_counter = chain.getCounter();
      ptr->chain_level   = chain.getLevelId();
      
      if     (chain.getLevelId() == 2) info.countsL2.at(chain.getCounter()) = ptr;
      else if(chain.getLevelId() == 3) info.countsEF.at(chain.getCounter()) = ptr;
    }
  }

  //
  // Fill id -> name map
  //
  LumiABC::FillTrigNames(fConfig);

  //
  // Save and return new object
  // 
  fInfos.push_back(info);
  fCurrIndex = fInfos.size()-1;
  
  return fInfos.at(fCurrIndex);
}

//-----------------------------------------------------------------------------------------
const Anp::Lumi::LV1Count* Anp::MakeCount::GetHighestRateLV1Count(const Lumi::BlockInfo &info) const
{
  //
  // Get the highest rate L1 chain for lb
  //
  uint64_t maxCount = 0;
  Lumi::LV1Count *count = 0;

  for(unsigned i = 0; i < info.countsL1.size(); ++i) {
    const Ptr<Lumi::LV1Count> &cnt = info.countsL1.at(i);
    
    if(maxCount < cnt->count_tav){
      maxCount = cnt->count_tav;
      count    = cnt.get();
    }
  }

  return count;
}

//-----------------------------------------------------------------------------------------
const Anp::Lumi::HLTCount* Anp::MakeCount::GetHighestRateHLTCount(const Lumi::BlockInfo &info,
								  unsigned level) const
{
  //
  // Get the highest rate HLT chain for lb
  //
  uint64_t maxCount = 0;
  Lumi::HLTCount *count = 0;

  if(level == 2){
    for(unsigned i = 0; i < info.countsL2.size(); ++i) {
      const Ptr<Lumi::HLTCount> &cnt = info.countsL2.at(i);

      if(maxCount < cnt->count_total){
	maxCount = cnt->count_total;
	count    = cnt.get();
      }
    }
  }
  else if(level == 3){
    for(unsigned i = 0; i < info.countsEF.size(); ++i) {
      const Ptr<Lumi::HLTCount> &cnt = info.countsEF.at(i);

      if(maxCount < cnt->count_total){
	maxCount = cnt->count_total;
	count    = cnt.get();
      }
    }
  }

  return count;
}

//---------------------------------------------------------------------------------------
void Anp::MakeCount::FillHistBCID(HtmlNode &bnode, const std::vector<TH1 *> &hists)
{
  //
  // Make histograms for counts per BCID
  //
  log() << "FillHistBCID - processing " << hists.size() << " histogram(s)" << endl;
    
  ostream &os = bnode.GetCont();

  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Trigger counts per bcid </th>"
     << "</tr>" 
     << endl;

  Anp::SetROOT_defaults();  
  TCanvas *canvas = new TCanvas(AlgEvent::GetName().c_str(), "", 650, 400);  

  for(unsigned i = 0; i < hists.size(); ++i) {
    TH1 *h = hists.at(i);
    if(!h) continue;

    const string name = "bcid_count_" + string(h->GetTitle()) + ".png";
    const string link = h->GetName();

    //
    // Print histogram and insert link to histogram
    //
    os << "<tr><td align=\"left\">";
    
    Anp::PrintHist(bnode, h, canvas, name, link, "");
    
    os << "</td></tr>" << endl;
  }

  os << "</table>" << endl;

  delete canvas;
}

//---------------------------------------------------------------------------------------
TH1* Anp::MakeCount::FindHistBCID(unsigned level, unsigned counter)
{
  //
  // Lookup histogram for level and counter
  //
  TH1 *h = 0;

  if(level == 1) {    
    if(counter >= fBCIDHistL1.size() || !fBCIDHistL1.at(counter)) {
      h = MakeHistBCID(level, counter, fBCIDHistL1);
    }
    else {
      h = fBCIDHistL1.at(counter);
    }
  }
  else if(level == 2) {
    if(counter >= fBCIDHistL2.size() || !fBCIDHistL2.at(counter)) {
      h = MakeHistBCID(level, counter, fBCIDHistL2);
    }
    else {
      h = fBCIDHistL2.at(counter);
    }
  }
  else if(level == 3) {
    if(counter >= fBCIDHistEF.size() || !fBCIDHistEF.at(counter)) {
      h = MakeHistBCID(level, counter, fBCIDHistEF);
    }
    else {
      h = fBCIDHistEF.at(counter);
    }
  }

  return h;
}

//---------------------------------------------------------------------------------------
TH1* Anp::MakeCount::MakeHistBCID(unsigned level, unsigned counter, std::vector<TH1 *> &hists)
{  
  //
  // Determine trigger name
  //
  TH1 *h = 0;
  string name;  
  stringstream title;

  title << "level_" << level << "_counter_" << setw(10) << setfill('0') << counter;

  for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);

    if(chain.getLevelId() == level && chain.getCounter() == counter) {
      name = chain.getName();
      break;
    }
  }

  if(name.empty()) {
    name = title.str();
  }
  
  //
  // Create histogram, set name and dir
  //
  h = Anp::HistMan::Instance().CreateTH1("count_per_bcid", fHistKey);
  if(h) {
    h->SetName(name.c_str());
    h->SetTitle(title.str().c_str());
    h->SetStats(false);
    Anp::SetDir(h, fDir);
  }
  else {
    log() << "MakeHistBCID - missing histogram: count_per_bcid" << endl;
  }

  //
  // Expand vector and insert pointer
  //
  if(counter < hists.size()) {
    if(hists.at(counter)) {
      log() << "MakeHistBCID - wrong index for: " << name << endl;
    }
  }
  else {
    TH1 *t = 0;
    hists.insert(hists.end(), counter - hists.size() + 1, t);
  }

  hists.at(counter) = h;

  return h;
}

//--------------------------------------------------------------------------------------      
void Anp::MakeCount::WriteNtuple()
{
  //
  // Write chain counts for 
  //
  float raw_rate = 0.0;
  float tot_rate = 0.0;
  float inp_rate = 0.0;

  for(unsigned i = 0; i < fInfos.size(); ++i) {    
    Lumi::BlockInfo &this_info = fInfos.at(i);
    
    fEventVars[0]  = 0.0;        //fill
    fEventVars[1]  = this_info.run;
    fEventVars[2]  = this_info.lb;
    fEventVars[3]  = this_info.lumiBegTime;
    fEventVars[4]  = this_info.lumiInstan;
    fEventVars[5]  = this_info.GetLBDuration();
    fEventVars[6]  = this_info.lumiInstan;
    fEventVars[7]  = this_info.lifetimeFrac;
    fEventVars[8]  = 0.0; // bunches
    fEventVars[9]  = 0.0; // nbunch
    fEventVars[10] = 0.0; // nbunch2
    fEventVars[11] = 0.0; // bunchLumi
    fEventVars[12] = this_info.bcid; // bcid
    // Write L1
    std::string ctp_name="";
    BOOST_FOREACH(const Ptr<Lumi::LV1Count> &pl1,this_info.countsL1){
      const Lumi::LV1Count *l1 = pl1.get();
      if(!l1) continue;

      FindTrigName(1,l1->ctp_id,ctp_name);
      //fBL1->Branch("tbp__"+ctp_name, &l1->count_tbp                      ,"tbp__"+ctp_name+"/F");    
      //fBL1->Branch("tap__"+ctp_name, &l1->count_tap                      ,"tap__"+ctp_name+"/F");     
      //fBL1->Branch("tav__"+ctp_name, &l1->count_tav                      ,"tav__"+ctp_name+"/F");     
      //fBL1->Branch("rbp__"+ctp_name, &l1->count_tbp*this_info.scaleCorrL1,"rbp__"+ctp_name+"/F");     
      //fBL1->Branch("rap__"+ctp_name, &l1->count_tap*this_info.scaleCorrL1,"rap__"+ctp_name+"/F");     
      //fBL1->Branch("rav__"+ctp_name, &l1->count_tav*this_info.scaleCorrL1,"rav__"+ctp_name+"/F");
    }

    // Write L2
    std::string chn_name="";
    BOOST_FOREACH(const Ptr<Lumi::HLTCount> &phlt,this_info.countsL2){
      const Lumi::HLTCount *hlt = phlt.get();
      if(!hlt) continue;

      FindTrigName(hlt->chain_level,hlt->chain_counter,chn_name);
      //fBL2->Branch(raw_ ,  &hlt->count_raw                        ,"raw__"+chn_name +"/F");     
      //fBL2->Branch(inp_ ,  &hlt->count_input                      ,"inp__"+chn_name +"/F");     
      //fBL2->Branch(tot_ ,  &hlt->count_total                      ,"tot__"+chn_name +"/F");     
      //fBL2->Branch(rraw_, &hlt->count_raw*this_info.scaleCorrL2  ,"rraw__"+chn_name+"/F");     
      //fBL2->Branch(rinp_, &hlt->count_input*this_info.scaleCorrL2,"rinp__"+chn_name+"/F");     
      //fBL2->Branch(rtot_, &hlt->count_total*this_info.scaleCorrL2,"rtot__"+chn_name+"/F");
    }

    // Write EF
    BOOST_FOREACH(Ptr<Lumi::HLTCount> &phlt,this_info.countsEF){
      Lumi::HLTCount *hlt = phlt.get();
      if(!hlt) continue;

      FindTrigName(hlt->chain_level,hlt->chain_counter,chn_name);

      TBranch *b = fBEF->Branch(VarName("tot__" ,chn_name),  &hlt->count_raw    ,VarName("tot__" ,chn_name)); 
      raw_rate = hlt->count_raw  *this_info.scaleCorrEF;
      tot_rate = hlt->count_input*this_info.scaleCorrEF;
      inp_rate = hlt->count_input*this_info.scaleCorrEF;
      fBEF->Branch(VarName("inp__", chn_name), &hlt->count_input                      ,VarName("inp__", chn_name));
      fBEF->Branch(VarName("tot__" ,chn_name), &hlt->count_total                      ,VarName("tot__" ,chn_name));
      fBEF->Branch(VarName("rraw__",chn_name), &raw_rate,VarName("rraw__",chn_name));
      fBEF->Branch(VarName("rinp__",chn_name), &tot_rate,VarName("rinp__",chn_name));
      fBEF->Branch(VarName("rtot__",chn_name), &inp_rate,VarName("rtot__",chn_name));
      fBranches.push_back(b);
    }

    if(fBL1) fBL1->Write();
    if(fBL2) fBL2->Write();
    if(fBEF) fBEF->Write();

  }

  
  if(fRatesNtup)  fRatesNtup->Close();
    
}

//--------------------------------------------------------------------------------------      
char* Anp::MakeCount::VarName(const char *var,const std::string &chn_name)
{
  std::stringstream  s;
  s << var << chn_name;
  std::string str(s.str());

  return (char*)str.c_str();
}

//--------------------------------------------------------------------------------------      
void Anp::MakeCount::InitNtuple()
{
  //
  // Intialize output file
  //
  fRatesNtup = new TFile("rates.root","RECREATE");

  fBL1       = new TTree("L1","L1");
  fBL2       = new TTree("L2","L2");
  fBEF       = new TTree("EF","EF");

  
  //fBL1->Branch("evt__fill",      &fEventVars[0] ,"evt__fill/F"      );
  //fBL1->Branch("evt__run",       &fEventVars[1] ,"evt__run/F"       );
  //fBL1->Branch("evt__lb",        &fEventVars[2] ,"evt__lb/F"        );
  //fBL1->Branch("evt__time",      &fEventVars[3] ,"evt__time/F"      );
  //fBL1->Branch("evt__lumi",      &fEventVars[4] ,"evt__lumi/F"      );
  //fBL1->Branch("evt__lblength",  &fEventVars[5] ,"evt__lblength/F"  );
  //fBL1->Branch("evt__lblumi",    &fEventVars[6] ,"evt__lblumi/F"    );
  //fBL1->Branch("evt__l1livefrac",&fEventVars[7] ,"evt__l1livefrac/F");
  //fBL1->Branch("evt__bunches",   &fEventVars[8] ,"evt__bunches/F"   );
  //fBL1->Branch("evt__nbunch",    &fEventVars[9] ,"evt__nbunch/F"    );
  //fBL1->Branch("evt__nbunch2",   &fEventVars[10],"evt__nbunch2/F"   );
  //fBL1->Branch("evt__bunchlumi", &fEventVars[11],"evt__bunchlumi/F" );
  //fBL1->Branch("evt__bcid",      &fEventVars[12],"evt__bcid/F"      );


}
