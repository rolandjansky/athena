/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostExec/SlowEvent.h"
#include "TrigCostBase/UtilTrig.h"

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::SlowEvent::SlowEvent()
  :fEvent(),
   fTimer(0.0)
{
}

//-----------------------------------------------------------------------------
Anp::SlowEvent::SlowEvent(const TrigMonEvent &e, double t)
  :fEvent(e),
   fTimer(t)
{
}

//-----------------------------------------------------------------------------
Anp::SlowEvent::SlowEvent(const EventTime &e, double timer)
 :fEvent(),
  fTimer(timer)
{
  fEvent.setEventID(0, e.lumi, e.bxid, e.run, e.sec, e.sec);
}

//-----------------------------------------------------------------------------
Anp::SlowEvents::SlowEvents()
  :fSlowNEvent(0)
{
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::AddSlowEvent(const TrigMonEvent &event, double timer)
{
  //
  // Save slowest events
  //
  if(fSlowNEvent < 1) return;

  //
  // Save N slowest events with detailed cost informartion
  //
  if(event.size<TrigMonSeq>() > 0) {
    if(fSlowDetail.size() < fSlowNEvent) {
      fSlowDetail.insert(SlowEvent(event, timer));
    }
    else {
      if(fSlowDetail.begin()->GetTimer() < timer) {
	fSlowDetail.erase(fSlowDetail.begin());
	fSlowDetail.insert(SlowEvent(event, timer));
      }
    }
  }
  
  //
  // Save N slowest events
  //
  if(fSlowEvents.size() < fSlowNEvent) {
    fSlowEvents.insert(SlowEvent(event, timer));
  }
  else {
    if(fSlowEvents.begin()->GetTimer() < timer) {
      fSlowEvents.erase(fSlowEvents.begin());
      fSlowEvents.insert(SlowEvent(event, timer));
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::AddSlowEvent(const EventTime &event, double timer)
{
  //
  // Save slowest events
  //
  if(fSlowNEvent < 1) return;
  
  //
  // Save N slowest events
  //
  if(fSlowEvents.size() < fSlowNEvent) {
    fSlowEvents.insert(SlowEvent(event, timer));
  }
  else {
    if(fSlowEvents.begin()->GetTimer() < timer) {
      fSlowEvents.erase(fSlowEvents.begin());
      fSlowEvents.insert(SlowEvent(event, timer));
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::WriteSlow(Handle<HtmlNode> nodeS,
				const HtmlMenu &menu,
				const TrigMonConfig &config,
				const std::multiset<SlowEvent> &events,
				const string &key) const
{
  //
  // Write N slowest events
  //
  if(!nodeS.valid() || events.empty()) return;

  nodeS->GetCont() 
    << "<pre>"
    << "List of " << events.size() << " slowest event(s)" << endl;

  unsigned icount = 0;

  for(multiset<SlowEvent>::const_reverse_iterator it = events.rbegin(); it != events.rend(); ++it) {
    const TrigMonEvent &event = it->GetEvent();
    
    stringstream link, html;
    
    link << "run "       << setw(7)  << setfill(' ') << event.getRun()
	 << ", lumi "    << setw(4)  << setfill(' ') << event.getLumi()
	 << ", bcid "    << setw(4)  << setfill(' ') << event.getBunchId()
	 << ", L1 sec="  << setw(10) << setfill(' ') << event.getSec()
	 << ", L1 nsec=" << setw(10) << setfill(' ') << event.getNanoSec()
	 << ", timer="   << it->GetTimer() << " ms";
    
    html << nodeS->GetBase() << "_slow_event_" 
	 << key << "_" << setw(4) << setfill('0') << ++icount << ".html";
    
    if(event.size<TrigMonSeq>() > 0) {
      Handle<HtmlNode> nodeE(new HtmlNode(html.str(), HtmlNode::kHTML));    
      nodeS->AddChild(nodeE);

      string strLevel( nodeS->GetBase() );
      string::size_type L2level = strLevel.find( "L2", 0 );
      string::size_type EFlevel = strLevel.find( "EF", 0 );
      unsigned int fLevel = 0;
      if( L2level != string::npos )    fLevel = 2;
      else if(EFlevel != string::npos) fLevel = 3;
      else                             fLevel = 0;

      nodeS -> GetCont() << nodeE->GetLink(link.str()) << endl;
      nodeE -> GetCont() << "<pre>" << endl;
      nodeE -> GetCont() << link.str() << endl << endl;

      PrintAlgTimers(nodeE->GetCont(), menu, event, config, fLevel);
      nodeE -> GetCont() << "</pre>" << endl;
    }
    else {
      nodeS -> GetCont() << link.str() << endl;
    }
  }
  
  nodeS->GetCont() << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::WriteSlow(Handle<HtmlNode> nodeS,
				const std::multiset<SlowEvent> &events,
				const string &key) const
{
  //
  // Write N slowest events
  //
  if(!nodeS.valid() || events.empty()) return;

  nodeS->GetCont() 
    << "<pre>"
    << "List of " << events.size() << " slowest event(s)" << endl;

  unsigned icount = 0;

  for(multiset<SlowEvent>::const_reverse_iterator it = events.rbegin(); it != events.rend(); ++it) {
    const TrigMonEvent &event = it->GetEvent();
    
    stringstream link, html;
    
    link << "run "       << setw(7)  << setfill(' ') << event.getRun()
	 << ", lumi "    << setw(4)  << setfill(' ') << event.getLumi()
	 << ", bcid "    << setw(4)  << setfill(' ') << event.getBunchId()
	 << ", L1 sec="  << setw(10) << setfill(' ') << event.getSec()
	 << ", L1 nsec=" << setw(10) << setfill(' ') << event.getNanoSec()
	 << ", timer="   << it->GetTimer() << " ms";
    
    html << nodeS->GetBase() << "_slow_event_" 
	 << key << "_" << setw(4) << setfill('0') << ++icount << ".html";

    nodeS -> GetCont() << link.str() << endl;
  }
  
  nodeS->GetCont() << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::PrintAlgTimers(std::ostream &os, 
				     const HtmlMenu &menu,				     
				     const TrigMonEvent &event,
				     const TrigMonConfig &config) const
{
  //
  //  Print individual algorithm timers sorted by time
  //
  unsigned awid = 0;
  multimap<float, string> algs;

  //
  // First loop - determine algorithm name width, make map
  //
  for(unsigned i = 0; i < event.size<TrigMonSeq>(); ++i) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(i);
    
    vector<TrigConfSeq>::const_iterator iseq = config.findSeq(seq);
    if(iseq == config.end<TrigConfSeq>()) {
      continue;
    }
    
    const vector<TrigMonAlg>  &vec_mon = seq.getAlg();    
    for(unsigned j = 0; j < vec_mon.size(); ++j) {
      const TrigMonAlg  &alg_mon = vec_mon.at(j);
      const TrigConfAlg &alg_con = iseq->getAlg(alg_mon.getPosition());

      if(alg_mon.isCalled()) {
	awid = std::max<unsigned>(awid, alg_con.getName().size());
	algs.insert(pair<float, string>(alg_mon.getTimer(), alg_con.getName()));
      }
    }    
  }
  
  //
  // Second loop - print algorithm name and timer
  //
  for(multimap<float, string>::reverse_iterator it = algs.rbegin(); it != algs.rend(); ++it) {

    Handle<HtmlNode> nodeA = menu.FindAlgName(it->second);

    if(nodeA.valid()) {
      os << nodeA->GetLink(Anp::PadStrBack(it->second, awid)) 
	 << " " << it->first << " ms" << endl;
    }
    else {
      os << Anp::PadStrBack(it->second, awid) << " " << it->first << " ms" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::SlowEvents::PrintAlgTimers(std::ostream &os, 
				     const HtmlMenu &menu,				     
				     const TrigMonEvent &event,
				     const TrigMonConfig &config,
				     const unsigned int &fLevel) const
{
  //
  //  Print individual algorithm timers sorted by time
  //
  unsigned awid = 0;
  multimap<float, string> algs;

  //
  // First loop - determine algorithm name width, make map
  //
  for(unsigned i = 0; i < event.size<TrigMonSeq>(); ++i) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(i);
    const unsigned level = seq.getLevel();

    if(fLevel != 0 && level != fLevel) continue; 
    
    vector<TrigConfSeq>::const_iterator iseq = config.findSeq(seq);
    if(iseq == config.end<TrigConfSeq>()) {
      continue;
    }
    
    const vector<TrigMonAlg>  &vec_mon = seq.getAlg();    
    for(unsigned j = 0; j < vec_mon.size(); ++j) {
      const TrigMonAlg  &alg_mon = vec_mon.at(j);
      const TrigConfAlg &alg_con = iseq->getAlg(alg_mon.getPosition());

      if(alg_mon.isCalled()) {
	awid = std::max<unsigned>(awid, alg_con.getName().size());
	algs.insert(pair<float, string>(alg_mon.getTimer(), alg_con.getName()));
      }
    }    
  }
  
  //
  // Second loop - print algorithm name and timer
  //
  for(multimap<float, string>::reverse_iterator it = algs.rbegin(); it != algs.rend(); ++it) {

    Handle<HtmlNode> nodeA = menu.FindAlgName(it->second);

    if(nodeA.valid()) {
      os << nodeA->GetLink(Anp::PadStrBack(it->second, awid)) 
	 << " " << it->first << " ms" << endl;
    }
    else {
      os << Anp::PadStrBack(it->second, awid) << " " << it->first << " ms" << endl;
    }
  }
}
