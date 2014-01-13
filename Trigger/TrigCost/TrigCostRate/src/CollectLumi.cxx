/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/CollectLumi.h"

REGISTER_ANP_OBJECT(AlgEvent,CollectLumi)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::HostInfo::HostInfo()
  :fName(),
   fId(0)
{
}

//-----------------------------------------------------------------------------
Anp::HostInfo::HostInfo(const std::string &name, unsigned id)
  :fName(name),
   fId(id)
{
}

//-----------------------------------------------------------------------------
Anp::CollectLumi::CollectLumi()
  :fDir(0),
   fDebug(false),
   fPrintLumi(false),
   fWriteHtml(false),
   fLumiEncodedId(0),
   fIsConfig(false),
   fTotalLumi(0),
   fTotalTime(0),
   fCountEvents(0),
   fCountRead(0),
   fCountEstm(0),
   fCountBuffers(0),
   hDiffTime(0),
   hDiffTimeC(0)
{
}

//-----------------------------------------------------------------------------
Anp::CollectLumi::~CollectLumi()
{
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Config(const Registry &reg)
{
  reg.Get("CollectLumi", "Debug",   fDebug);
  reg.Get("CollectLumi::PrintLumi", fPrintLumi);
  reg.Get("CollectLumi::LumiChain", fLumiChain);
  reg.Get("CollectLumi::WriteHtml", fWriteHtml);

  fConfg = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf");
  fEntry = NtupleSvc::Instance("event") .Branch<TrigMonEvent> ("data");

  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Save(TDirectory *dir)
{
  //
  // Create histograms
  //
  fDir = Anp::GetDir(dir, "lumi");

  if(fDir) {
    hDiffTime  = Anp::HistMan::Instance().CreateTH1("time_diff_L1_host",     "lumi");
    hDiffTimeC = Anp::HistMan::Instance().CreateTH1("time_diff_L1_host_cor", "lumi");

    Anp::SetDir(hDiffTime,  fDir);
    Anp::SetDir(hDiffTimeC, fDir);
  }
}

//-----------------------------------------------------------------------------
bool Anp::CollectLumi::Init()
{  
  //
  // Read configuration from init file and release pointer
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) { 
    Set(fConfg.GetRef());
  }
  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Exec()
{
  //
  // Process one event from ntuple
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) { 
    Set(fConfg.GetRef());
    Run(fConfg.GetRef());
  }

  //
  // Make new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Done()
{
  //
  // Fill summary data for luminosity blocks
  //
  log()<< "Done - processing lumi block summary:" << endl;

  long count_total = 0, count_lumis = 0;

  for(RunMap::iterator rit = fRuns.begin(); rit != fRuns.end(); ++rit) {
    std::vector<Anp::LumiBlock> &lvec = rit->second.lumis;

    set<int> lumi_set, hist_set;
    
    //
    // Find time duration of lumi blocks
    // 
    for(unsigned i = 0; i < lvec.size(); ++i) {
      LumiBlock &lb = lvec.at(i);
      if(lb.GetLB() < 0) continue;

      //
      // Check next lumi block
      //
      if(i+1 < lvec.size()) {	
	const LumiBlock &next_lb = lvec.at(i+1);

	if(lb.GetLB()+1 == next_lb.GetLB()) {
	  lb.SetNext(next_lb.GetFirstSec(), next_lb.GetFirstNSec());
	}
      }
      
      count_lumis++;
      count_total += lb.GetNEvents();
      fTotalLumi  += lb.GetLumi();
      fTotalTime  += lb.GetDeltaT();
      
      lumi_set.insert(lb.GetLB());
    }

    const unsigned run = rit->first;
    const HostMap &hmap = rit->second.hosts;

    //
    // Collect lumi blocks from different hosts and add them up
    //
    unsigned count_read_run = 0, count_estm_run = 0;

    for(map<uint32_t, Host>::const_iterator hit = hmap.begin(); hit != hmap.end(); ++hit) {
      const Host &h = hit->second;

      for(unsigned i = 0; i < h.lumi.size(); ++i) {
	const Count &hcount = h.lumi.at(i);
	count_read_run += hcount.count_read;
	count_estm_run += hcount.count_estm;
	
	if(hcount.count_estm == 0) {
	  hist_set.insert(i);
	}
	
	if(i < lvec.size()) {
	  LumiBlock &lb = lvec.at(i);
	  if(lb.GetLB() >= 0 && static_cast<unsigned>(lb.GetLB()) == i) {
	    lb.AddReadEvent(hcount.count_read);
	    lb.AddEstmEvent(hcount.count_estm);
	  }
	  else {
	    if(fDebug) {
	      log() << "Done - failed to match lb: " << i << "?" << lb.GetLB() << endl;
	    }
	  }
	}
      }
    }

    fCountRead += count_read_run;
    fCountEstm += count_estm_run;
    
    cout << "   run " << setw(7) << setfill(' ') << run << endl
	 << "     count read " << std::right << count_read_run  << endl
	 << "     count estm " << std::right << count_estm_run  << endl
	 << "     count host " << std::right << hmap.size()     << endl;
    
    if(fPrintLumi) {
      int prev_lumi = -1;
      for(set<int>::const_iterator sit = lumi_set.begin(); sit != lumi_set.end(); ++sit) {
	const int curr_lumi = *sit;

	if(prev_lumi >= 0 && prev_lumi+1 != curr_lumi) {
	  cout << "     lumi: --gap--" << endl;
	}
	
	cout << "     lumi: " << curr_lumi << endl;
	prev_lumi = curr_lumi;
      }
    
      for(set<int>::iterator lit = lumi_set.begin(); lit != lumi_set.end(); ++lit) {
	if(!hist_set.count(*lit)) {
	cout << "   missing history lumi block: " << *lit << endl;
	}
      }
    }
  }
  
  cout << "   total time   = " << fTotalTime    << endl
       << "   integ lumi   = " << fTotalLumi    << endl
       << "   #lumi blocks = " << count_lumis   << endl
       << "   input events = " << fCountEvents  << endl
       << "   total events = " << count_total   << endl
       << "   nevents T0   = " << fCountBuffers << endl;
}

//-----------------------------------------------------------------------------
long double Anp::CollectLumi::GetScaleCorr() const
{
  //
  // Get overall correction factor for deadtime, cost events not recorded, etc
  //
  if(fCountRead > 0 && fCountEstm >= fCountRead) {
    return double(fCountEstm)/double(fCountRead);
  }
  
  return 1.0;
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Set(const TrigMonConfig &config)
{
  //
  // Need trigger configuration to find chain used for luminosity count
  //
  if(fIsConfig) {
    return;
  }
  else {
    fIsConfig = true;
  }

  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);
    if(chain.getName() == fLumiChain) {
      fLumiEncodedId = chain.getEncodedId();

      log() << "Set - luminosity chain " << chain.getName() 
	    << " with encoded id = " << fLumiEncodedId << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Run(const TrigMonConfig &config)
{
  //
  // Process configuration
  //
  const std::vector<std::string> &var_nm = config.getVarName();
  const std::vector<uint32_t>    &var_id = config.getVarId();

  if(var_nm.size() != var_id.size()) {
    log() << "Run - variable vector size error" << endl;
    return;
  }

  if(fDebug) {
    log()<< "Run - TrigMonConfig: nvar=" << var_nm.size() << endl;

    for(unsigned i = 0; i < var_id.size(); ++i) {
      cout << "  var[" << var_id.at(i) << "]=" << var_nm.at(i) << endl;
    }
  }

  HostMap &hmap = fRuns[config.getRun()].hosts;
  InfoMap &imap = fRuns[config.getRun()].infos;
  
  for(unsigned i = 0; i < var_id.size(); ++i) {
    const std::string &vnm = var_nm.at(i);
    const uint32_t     vid = var_id.at(i);   

    if(vnm.find("xpu") == string::npos) {
      continue;
    }

    if(hmap.insert(HostMap::value_type(vid, Host())).second) {  
      if(fDebug) {
	log() << "Run - new host: name/ = " << vnm << "/" << vid << endl;
      }
    }
    
    if(imap.insert(InfoMap::value_type(vid, HostInfo())).second) {
      if(fDebug) {
	log() << "Run - new info: name/ = " << vnm << "/" << vid << endl;
      }      
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Run(const TrigMonEvent &event)
{
  //
  // Process one event and collect lumi data
  //
  ++fCountEvents;

  vector<LumiBlock> &lumi_vec = fRuns[event.getRun()].lumis;

  const int run = event.getRun();
  const int lb  = event.getLumi();

  if(lb >= int(lumi_vec.size())) {
    lumi_vec.insert(lumi_vec.end(), 1+lb-lumi_vec.size(), Anp::LumiBlock(run, -1));
  }
  
  LumiBlock &block = lumi_vec.at(lb);

  if(block.GetLB() < 0) {
    //
    // Initialize new LB - can happpen at different events 
    //
    block.SetLB(lb);
    block.SetFirst(event.getSec(), event.getNanoSec());
    block.SetLast (event.getSec(), event.getNanoSec());
  }
  
  block.AddEvent();

  const unsigned sec  = event.getSec();
  const unsigned nsec = event.getNanoSec();

  if(sec < block.GetFirstSec() || (sec == block.GetFirstSec() && nsec < block.GetFirstSec())) {
    block.SetFirst(sec, nsec);
  }

  if(sec > block.GetLastSec() || (sec == block.GetLastSec() && nsec > block.GetLastSec())) {
    block.SetLast(sec, nsec);
  }


  if(fLumiEncodedId > 0) {
    //
    // Count events passed by trigger used for luminosty measurement
    //
    const std::vector<uint32_t> &hlt = event.getChain();
    const std::vector<uint16_t> &lv1 = event.getL1Item();

    for(unsigned i = 0; i < hlt.size(); ++i) {
      const TrigMonChain chain(hlt.at(i));
      
      if(chain.getEncodedId() == fLumiEncodedId && 
	 chain.isPassed(TrigMonChain::kPassed)) {
	block.AddLumi(1.0);
	break;
      }
    }
    
    for(unsigned i = 0; i < lv1.size(); ++i) {
      const TrigMonL1Item chain(lv1.at(i));
      
      if(chain.getEncodedId() == fLumiEncodedId && 
	 chain.isPassed(TrigMonL1Item::kPassed)) {
	block.AddLumi(1.0);
	break;
      }
    }
  }
  
  if(fDebug) {
    for(unsigned i = 0; i < event.getWord().size(); ++i) {
      log() << "Run - word[" << i << "]=" << event.getWord().at(i) << endl;
    }
  }

  //
  // Count events from individual farm hosts
  // 
  if(event.getWord().size() == 3) { 
    const unsigned sec  = event.getWord().at(0);
    const unsigned msec = event.getWord().at(1);
    const unsigned tid  = event.getWord().at(2);

    //
    // Compute time difference between L1 accept and HLT host time
    //
    const int diff_sec = int(sec) - int(event.getSec());     
    int diff_ns  = int(event.getNanoSec()) - int(msec*1000);

    if(msec*1000 > event.getNanoSec()) { 
      diff_ns = int(msec*1000) - int(event.getNanoSec());
    }
    
    float tproc_timer = 0.0;
    event.getVar(102, tproc_timer);
    event.getVar(112, tproc_timer);
    
    if(event.getTimer() > 0.0) {
      tproc_timer = event.getTimer();
    }

    if(hDiffTime)  hDiffTime ->Fill(1.0e-6*diff_ns);
    if(hDiffTimeC) hDiffTimeC->Fill(1.0e-6*diff_ns - tproc_timer);

    //
    // Count number of events previously processed by this application id
    //
    CountHost(tid, event);

    if(fDebug) {
      log() << "CountHost - time flow: " << endl
	    << "  run, lumi:      " << event.getRun() << ", " << event.getLumi()    << endl
	    << "  LVl1 sec, nsec: " << event.getSec() << ", " << event.getNanoSec() << endl
	    << "  Host sec, nsec: " << sec            << ", " << msec*1000          << endl
	    << "  Difference:     " << diff_sec*1e3 + diff_ns*1e-6 << " ms" << endl;
    }
  }
  else {
    if(fDebug) {
      log() << "Run - error: bad number of event words" << endl;
    }
  }

  if(fDebug) {
    //
    // Check for non-unique L1 timestamps
    //
    const std::pair<uint32_t, uint32_t> l1id(event.getSec(), event.getNanoSec());
    if(!fTime.insert(l1id).second) {
      log() << "Run - duplicate timestamp: " << event.getSec() << " " << event.getNanoSec() << endl;
    }
  }
    //
    // Count the number of SFO Files
    //
  float event_buffer_size = 0.0;
  if(event.getVar(9,event_buffer_size)) {
    ++fCountBuffers;
  }
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::Write(HtmlNode &inode, HtmlMenu &)
{
  //
  // Write out summary of runs and lumi blocks
  //
  if(!fWriteHtml) {
    return;
  }

  Handle<HtmlNode> rnode(new HtmlNode("run_summary.html", HtmlNode::kHTML));
  inode.AddChild(rnode);

  inode.GetCont() << rnode->GetLink("Run summary") << endl << endl;
  
  stringstream &os = rnode -> GetCont();
  os << "<pre>" << endl
     << "Collected " << fCountEvents << " cost events" << endl << endl;

  for(RunMap::const_iterator rit = fRuns.begin(); rit != fRuns.end(); ++rit) {
    const std::vector<Anp::LumiBlock> &lvec = rit->second.lumis;

    unsigned count_lb = 0, event_wd = 0;

    for(unsigned i = 0; i < lvec.size(); ++i) {
      const LumiBlock &lb = lvec.at(i);
      if(lb.GetLB() < 0) continue;
      
      count_lb++;
      event_wd = std::max(event_wd, Anp::GetWidth(lb.GetCount()));
    }

    os << "Run " << rit -> first << " has " << count_lb 
       << " luminosity block(s) with non-zero event count" << endl;
    
    for(unsigned i = 0; i < lvec.size(); ++i) {
      const LumiBlock &lb = lvec.at(i);
      if(lb.GetLB() < 0) { 
	os << "  lb=" << setfill(' ') << setw(4) << right << i << endl;
	continue;
      }

      // Round seconds to 0.01 and make resulting string 6 characters wide
      const string dts = Anp::Round2Pair(lb.GetDeltaT(), 0.1).first;
      const string dtp = Anp::PadStrFront(dts, 6);

      // Compute rate and round it to 0.1 Hz      
      double rate = 0.0;
      if(lb.GetDeltaT() > 0.1) { 
	rate = lb.GetNEvents()/lb.GetDeltaT();      
      }

      // Compute fraction of lost (not recorded) events
      double lost = 0.0;
      if(lb.GetNEstmEvents() > 0) {
	lost = 100.0*double(lb.GetNReadEvents())/double(lb.GetNEstmEvents());
      }

      if(lb.GetLB() >= 0 && lb.GetNReadEvents() != lb.GetNEvents()) {
	if(fDebug) {
	  cerr << "CollectLumi::Write - event count error!" << endl;
	}
	continue;
      }

      const string rateS = Anp::Round2Pair(rate, 1.0).first;
      const string rateP = Anp::PadStrFront(rateS, 6);

      const string lostS = Anp::Round2Pair(lost, 0.1).first;
      const string lostP = Anp::PadStrFront(lostS, 6);

      time_t sec = lb.GetFirstSec();
      tm *tm_ptr = gmtime(&sec);

      os << "  lb="
	 << setfill(' ') << setw(4) << right << lb.GetLB()
	 << ", events="
	 << setfill(' ') << setw(event_wd) << right << lb.GetNEvents()
	 << ", dt=" << dtp << "s"
	 << ", rate=" << rateP << "Hz"
	 << ", read=" << lostP << "%"
	 << ", napp_read=" << setfill(' ') << setw(4) << lb.GetNAppRead()
	 << ", napp_hist=" << setfill(' ') << setw(4) << lb.GetNAppHist()
	 << ", #read=" << setw(event_wd) << lb.GetNReadEvents()
	 << ", #estm=" << setw(event_wd) << lb.GetNEstmEvents()
	 << ", " << asctime(tm_ptr);
      
      if(fDebug) {
	os << ", first=" 
	   << setfill(' ') << setw(10) << right << lb.GetFirstSec() << ", " 
	   << setfill(' ') << setw(10) << right << lb.GetFirstNSec()
	   << ", last="
	   << setfill(' ') << setw(10) << right << lb.GetLastSec() << ", " 
	   << setfill(' ') << setw(10) << right << lb.GetLastNSec()
	   << endl;
      }
    }
  }

  os << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::CollectLumi::CountHost(unsigned host_id, const TrigMonEvent &event)
{
  //
  // Count events processed by HLT application
  //  
  RunData &run = fRuns[event.getRun()];
  
  if(event.getLumi() >= run.lumis.size()) {
    log() << "CountHost - out of range lumi block: " << event.getLumi() << endl;
    return;
  }
  
  //
  // Record application id for current read cost event
  // 
  run.lumis.at(event.getLumi()).AddAppRead(host_id);

  HostMap::iterator hit = run.hosts.find(host_id);
  if(hit == run.hosts.end()) {
    if(fDebug) {
      log() << "CountHost - unknown host id: " << host_id << endl;
    }
    return;
  }

  InfoMap::iterator sit = run.infos.find(host_id);
  if(sit == run.infos.end()) {
    log() << "CountHost - unknown info id: " << host_id << endl;
    return;
  }

  Host &h = hit->second;

  //
  // Count actual number of processed events
  //
  if(event.getLumi() >= h.lumi.size()) {
    h.lumi.insert(h.lumi.end(), 1 + event.getLumi() - h.lumi.size(), Count());
  }
  
  Count &c = h.lumi.at(event.getLumi());
  c.count_read++;  

  //
  // Read history for number of processed events by host
  //
  const std::vector<uint32_t> &keys = event.getVarKey();
  const std::vector<float>    &vals = event.getVarVal();
  
  if(keys.size() != vals.size() ) {
    log() << "CountHost - key and var size mismatch: " << host_id << endl;
    return;   
  }
  
  for(unsigned i = 0; i < keys.size(); ++i) {
    const unsigned var = keys.at(i);
    
    if(var <= 9999 || var > 15000) {
      continue;
    }

    const unsigned lb = var - 10000;
    if(lb >= event.getLumi()) {
      continue;
    }
    
    //
    // Record application id for lumi block history
    // 
    run.lumis.at(lb).AddAppHist(host_id);

    if(lb >= h.lumi.size()) {
      h.lumi.insert(h.lumi.end(), 1+lb-h.lumi.size(), Count());
    }
    
    Count &c = h.lumi.at(lb);
    const unsigned hcount = static_cast<unsigned>(vals.at(i));
    
    if(c.count_estm == 0) {
      c.count_estm = hcount;
    }
    else if(c.count_estm != hcount) {
      if(fDebug) {
	log() << "CountHost - history error: lb=" << lb << ": " << c.count_estm << "!=" << hcount << endl;
      }
    }
    
    if(fDebug) {
      log() << "CountHost - history: lb=" << lb << " host=" << host_id << " count=" << vals.at(i) << endl;
    }
  }

  if(fDebug) { 
    log() << "CountHost - number of keys: " << keys.size() << endl;
    
    for(unsigned i = 0; i < keys.size(); ++i) {
      cout << "   var[" << keys.at(i) << "]=" << vals.at(i) << endl;
    }
  }
}
