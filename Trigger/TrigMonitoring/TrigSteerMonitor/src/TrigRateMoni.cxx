/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <set>

#include <TH2F.h>
#include <TString.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "Gaudi/Property.h"

#include "TrigInterfaces/AlgoConfig.h"

#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/StreamTag.h"
#include "TrigSteering/TrigSteer.h"

#include "TrigRateMoni.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"


 
TrigRateMoni::TrigRateMoni(const std::string & type, const std::string & name,
			   const IInterface* parent) 
  : TrigMonitorToolBase(type, name, parent),
    m_duration(10),
    m_intervals(6),
    m_timeDivider(0),
    m_in_running(false)
{  
  declareProperty("IntervalDuration",  m_duration,  "The time interval over which the rate information is collected. If <60 then it is in seconds, if >60 then in minutes (must be multiple of 60)");
  declareProperty("NumberOfIntervals", m_intervals, "Number of intrvals.");
  declareProperty("doGroups", m_doGroups=true, "enable/disable groups monitoring");
  declareProperty("doStreams", m_doStreams=true, "enable/disable streams monitoring");
  declareProperty("doChains", m_doChains=true, "enable/disable chains monitoring");
  declareProperty("StreamSets", m_specialStreamSetProperties, 
		  "declare sets of streams whose sum is monitored. Format: setName:sreamName1,streamNam2...");

}

TrigRateMoni::~TrigRateMoni() {
  delete m_timeDivider;
  delete m_buffer;
}


StatusCode TrigRateMoni::initialize() {

  if (m_duration >= 60 ) {
    if ( (m_duration % 60) != 0 ){
      ATH_MSG_WARNING("Time interval >= 60, but not multiple of it " << m_duration);
      m_duration = m_duration - (m_duration % 60);
      ATH_MSG_WARNING("Rounding it to : " << m_duration);
    }
    m_timeDivider = new TimeDivider(m_intervals, m_duration/60, TimeDivider::minutes);    
  } else {
    if ( (m_intervals*m_duration) % 60 != 0 ) {
      ATH_MSG_INFO("Time interval * Number of Interval  does not round to minute");
    }
    m_timeDivider = new TimeDivider(m_intervals, m_duration, TimeDivider::seconds);
  }

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if ( !m_parentAlg ) {
    ATH_MSG_ERROR(" Unable to cast the parent algorithm to HLT::TrigSteer !");
    return StatusCode::FAILURE;
  }

  // decode stream sets and set up
  SetUpStreamSets();

  return StatusCode::SUCCESS;

      
}

void TrigRateMoni::SetUpStreamSets() {

  // Note that these are limited at the moment to EF physics streams only 
  // Adding the other typess would only require work!

  /* 
     Be careful when defining the strings in python, the decoding is
     very basic!  Expected syntax:
     'set_name:stream_name,stream_name...'  embedded blanks are okay

     Note as well that if a set is defined with no associated streams,
     it is interpreted as a set which contains all strings except
     those in the other sets. Only one such set is allowed (but this
     not checked).

  */
  bool isL2 = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? true : false;
  if(isL2)
    return;

  m_specialStreamSets.clear();
  m_specialStream_OtherName = "";
  
  for (const std::string& is : m_specialStreamSetProperties) {
    
    std::vector<std::string> streamNames;
    
    // Decode the property string
    
    unsigned int start = 0;
    for(start = 0; start < is.length(); start++) {
      if(is[start] != ' ') break;
    }
    if(start == is.length()) {
      break;
    }
    unsigned int colon = start;
    for(colon = start; colon < is.length(); colon++) {
      if(is[colon] == ':') break;
    }
    if(colon == is.length()) {
      break;
    }
    
    std::string setName = is.substr(start, colon-start);
    
    start = colon+1;
    unsigned int end = start;
    while(start < is.length()) {
      for(end = start; end < is.length(); end++) {
        if(is[end] == ' ') continue; // assumes no embedded blanks in stream names
        if(is[end] == ',') break;
      }
      if(end == start) {
        start++;
        continue;
      }
      streamNames.push_back(is.substr(start, end-start));
      start = end+1;
    }
    if(end > start) {
      streamNames.push_back(is.substr(start, end-start));
    }

    m_specialStreamSets.insert(std::pair<std::string, std::vector<std::string> >
                               (setName, streamNames));

    if(streamNames.size() == 0)
      m_specialStream_OtherName = setName;    
  }

  // Check
  ATH_MSG_INFO("Adding rate monitoring stream sets from python properties");
  
  for (const auto& sss : m_specialStreamSets) { // std::map<std::string, std::vector<std::string>>
    std::string outString = "Adding set name: " + sss.first + " with streams: ";
    
    for (const std::string& isn : sss.second) {
      outString += isn + " ";
    }
    ATH_MSG_INFO(outString);
  }
  if(m_specialStream_OtherName.size() != 0) {
    ATH_MSG_INFO("The set containing all streams but those above is: " 
                 << m_specialStream_OtherName);
  } else {
    ATH_MSG_INFO("No catch-all set was defined ");
  }
}

// normalizes groups name
std::string grp_bin_name(const std::string& grp ) {
  return "grp_"+grp.substr(grp.find(':')+1);
}


StatusCode TrigRateMoni::bookHists() {

  // find about all possible chains
  std::vector<const HLT::SteeringChain*> configuredChains = m_parentAlg->getConfiguredChains();
  
  std::set<std::string> groups;
  std::set<std::string> streams;


  for ( const HLT::SteeringChain* chain : configuredChains ) {
    // get groups it belongs to
    const std::set<std::string>& chainGroupList = chain->getConfigChain()->groups();

    if (chainGroupList.size() == 0 ) {
      ATH_MSG_WARNING("Chain " << chain->getChainName() << " has not group configured, it's rate wil sneak out from groups rate monitoring ");
    }

    for (const std::string& gr : chainGroupList) {
      if ( gr.compare(0,4,"RATE") == 0 ) // name starts with string RATE
        groups.insert(gr);
    }

    for (const HLT::StreamTag& stream : chain->getStreamTags()) {
      streams.insert("str_" + stream.getStream() + "_" + stream.getType());
    }
  }

  // get debug stream names
  for (const auto& s : m_parentAlg->getErrorStreamTags()) {
    streams.insert("str_" + s.name()+"_"+s.type());
    ATH_MSG_INFO("adding error stream name: " << s.name()+"_"+s.type());
  }

  // initialize m_genericStreamSets and m_specialStreamSets

  m_genericStreamSets.clear();
  m_genericStreamSets.push_back("physics");
  m_genericStreamSets.push_back("express");
  m_genericStreamSets.push_back("calibration");
  m_genericStreamSets.push_back("debug");


  // now figure out histogram size needed

  // !!!
  unsigned int nbins 
    = 1 // for total rate 
    + 1 // for recording rate
    + m_genericStreamSets.size()
    + m_specialStreamSets.size() // number of special stream sets
    + (m_doStreams ? streams.size(): 0) // streams
    + (m_doGroups  ? groups.size() : 0)  // goups
    + (m_doChains  ? configuredChains.size() : 0) // and fianlly chains;
    //+ m_intervals  // and one to mark the source
    + 1; //for absolute time

  bool isL2 = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? true : false;
  if(isL2) nbins++; // for total_physics

    // second pass to find mapping between chains and bins
  
  m_buffer    = new TH2F("Buffer", "Buffer", nbins, 0., nbins, 7, 0., 7);
  m_buffer->SetDirectory(0);
  
  TString name("Rate");
  if ( m_duration < 60 )
    name = Form("Rate%ds", m_duration);
  else
    name = Form("Rate%dm", m_duration/60);
  
  m_published = new TH2F(name.Data(), "Rates", nbins, 0., nbins, 7, 0., 7);

  m_published->GetYaxis()->SetBinLabel(1, "input");
  m_published->GetYaxis()->SetBinLabel(2, "prescaled");
  m_published->GetYaxis()->SetBinLabel(3, "raw");
  m_published->GetYaxis()->SetBinLabel(4, "output");
  m_published->GetYaxis()->SetBinLabel(5, "rerun");
  m_published->GetYaxis()->SetBinLabel(6, "algoIn");
  m_published->GetYaxis()->SetBinLabel(7, "passedrerun");
  

  unsigned int bin = 1;
  m_stream_map["total"] = bin;
  m_published->GetXaxis()->SetBinLabel(bin, "total");
  bin++;

  if(isL2) {
    m_stream_map["total_physics"] = bin;
    m_published->GetXaxis()->SetBinLabel(bin, "total_physics");
    bin++;
  }

  m_stream_map["recording"] = bin;
  m_published->GetXaxis()->SetBinLabel(bin, "recording");
  bin++;

  // the generic stream sets
  for (const std::string& gs : m_genericStreamSets) {
    m_published->GetXaxis()->SetBinLabel(bin, ("recording_" + gs).c_str());
    m_stream_map[gs] = bin;
    bin++;
  }

  if ( m_doStreams ) {

    if(!isL2) {
      for (const auto& sss : m_specialStreamSets) { // std::map<std::string, std::vector<std::string>>
        m_published->GetXaxis()->SetBinLabel(bin, sss.first.c_str());
        if(sss.first != m_specialStream_OtherName) {
          m_stream_map[sss.first] = bin;
          bin++;
        }
      }
      // put the "other" stream-set last
      if(m_specialStream_OtherName != "") {
        m_stream_map[m_specialStream_OtherName] = bin;
        m_published->GetXaxis()->SetBinLabel(bin, m_specialStream_OtherName.c_str() );
        bin++;
      }
    }
    
    // a bin for each stream
    for ( const std::string& s : streams ) {
      m_published->GetXaxis()->SetBinLabel(bin, s.c_str());
      m_stream_map[s] = bin;
      bin++;
    }    
  }
  
  // a bin for each group
  if ( m_doGroups ) {
    for ( const std::string& g : groups ) {
      m_published->GetXaxis()->SetBinLabel(bin, grp_bin_name(g).c_str());
      bin++;
    }
  }

  // a bin for each chain
  if ( m_doChains || m_doGroups ) {
     for( const HLT::SteeringChain* chain : configuredChains ) {
      if ( m_doChains ) {
        m_published->GetXaxis()->SetBinLabel(bin, chain->getChainName().c_str());
        m_chains_map[chain] = bin;
        bin++;
      }
      if ( m_doGroups ) {
        // preserve chain -> group bin map
        const std::set<std::string> chainGroupList = chain->getConfigChain()->groups();
        for ( std::string grp : chainGroupList ) {
          if (groups.find(grp)!=groups.end()) { // only consider RATE groups previously defined
            m_groups_map[chain].insert(m_published->GetXaxis()->FindBin( grp_bin_name(grp).c_str() ));
          }
        }
      }
    }
  }

  // name time marker bins
  /*
  for ( unsigned int inter = 0 ; inter < m_intervals; inter++ ) {
    TString l = Form("interval_%d", inter);
    m_published->GetXaxis()->SetBinLabel(bin, l.Data());
    bin++;
  }
  */
  m_published->GetXaxis()->SetBinLabel(bin, "time");
  bin++;

  TrigMonGroup expressHistograms( this, m_parentAlg->name(), shift);
   
  if ( expressHistograms.regHist(m_published).isFailure()){
    ATH_MSG_WARNING("Can't book " << m_published->GetName());
  }
  //  time_t t = time(0);
  //  fillAbsoluteTime(t);
  //  m_timer.start(m_duration*50);

  // m_in_running = true;  


  // If m_in_running is true, the output histo is sometimes reset, so for debugging, set it false
  //std::cout << "MMMrate: if you forget to get rid of the following line, you might be shot!" << std::endl;
  //m_in_running = false;  

  return StatusCode::SUCCESS;
}


void TrigRateMoni::callback() {
  // if we are after finalHists the update was already done
  // we shoudl not touch anymore histograms 
  // and we shoudl not reschedule timeout anymore  
  boost::lock_guard<boost::mutex> guard(m_update_mutex);
  if ( ! m_in_running )
    return;

  // first ask time divider if we need to switch to new interval
  time_t t = time(0);
  unsigned int newinterval;
  unsigned int oldinterval;
  bool switchNow = m_timeDivider->isPassed(t, newinterval, oldinterval);
  if ( switchNow ) {
    updatePublished(oldinterval, m_duration, t);
  }  
  // schedule itself in another 1/20 of the integration period
  m_timer->start(m_duration*50); 
}

StatusCode TrigRateMoni::fillHists() { 
  boost::lock_guard<boost::mutex> guard(m_update_mutex);
  /*  
      time_t t = time(0);
      unsigned int newinterval;
      unsigned int oldinterval;
      bool switchNow = m_timeDivider->isPassed(t, newinterval, oldinterval);
      if ( switchNow ) {
      updatePublished(oldinterval, m_duration, t);
      }  
  */
  if ( ! m_in_running ) {
    m_timer=new Athena::AlgorithmTimer(0, boost::bind(&TrigRateMoni::callback, this), Athena::AlgorithmTimer::DELIVERYBYTHREAD);
    m_timer->start(m_duration*50);
    m_in_running = true;  
  }

  fillTotalAndStreamTagBins();

  if ( m_doChains || m_doGroups ) {
    fillChainAndGroupBins();
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigRateMoni::finalHists() {  
  boost::lock_guard<boost::mutex> guard(m_update_mutex);
  m_in_running = false;
  if (m_timer) m_timer->stop();
  
  time_t t = time(0);
  unsigned int duration;
  unsigned int interval;
  
  // procedure is slightly different at the end of the run when we want to close the interval anyway 
  // but do not want to divide by the interval duration but rather by time which realy passed
  duration = m_timeDivider->forcePassed(t, interval);
  updatePublished(interval, duration, t);


  delete m_buffer; m_buffer = 0;
  delete m_timer; m_timer = 0;  

  return StatusCode::SUCCESS;
}


void TrigRateMoni::updatePublished(unsigned int /*oldinterval*/, unsigned duration, const time_t& t) {
  // scale down buffer
  oh_lock_histogram<TH2F>(m_published)->Reset();
  if ( duration == 0 )
    oh_lock_histogram<TH2F>(m_published)->Add(m_buffer, 1./(double)m_duration);
  else 
    oh_lock_histogram<TH2F>(m_published)->Add(m_buffer, 1./duration);
  m_buffer->Reset();
  fillAbsoluteTime(t);
}

void TrigRateMoni::fillInterval(unsigned int iv) {
  int bin = m_published->GetNbinsX() - m_intervals + iv + 1;
  m_published->SetBinContent(bin, input+1, 1.);
}

void TrigRateMoni::fillAbsoluteTime(const time_t& t) {
  // break the time into day of year, hour, minute, second
  tm tmstr;
  localtime_r(&t, &tmstr);
  oh_scoped_lock_histogram lock;
  int bin = m_published->GetNbinsX();
  // use +1 as this enums were defined to be used in Fill
  m_published->SetBinContent(bin, input+1,    tmstr.tm_yday);  
  m_published->SetBinContent(bin, prescale+1, tmstr.tm_hour);
  m_published->SetBinContent(bin, raw+1,      tmstr.tm_min);
  m_published->SetBinContent(bin, output+1,   tmstr.tm_sec);  
}

void TrigRateMoni::fillTotalAndStreamTagBins() {
  const EventInfo* constEventInfo(0);
  
  if (evtStore()->retrieve(constEventInfo).isFailure()) {
    ATH_MSG_FATAL("Can't get EventInfo to fill Stream tags information");
    return;
  }

  std::vector<TriggerInfo::StreamTag> streamTags= constEventInfo->trigger_info()->streamTags();

  bool isL2 = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? true : false;
  bool L2PhysicsStreamHit = false;
  
  if ( m_doStreams ) {
    
    for ( const TriggerInfo::StreamTag& st : streamTags ) {
      m_buffer->Fill( m_stream_map["recording"]-0.5, output); // recording
      
      // Fill the special set bins
      if(!isL2 && st.type() == "physics") {
        bool inSpecialGroup = false;
        for (const auto& sss : m_specialStreamSets) {  // std::map<std::string, std::vector<std::string>>
          for (const std::string& ssi : sss.second) {             
            if( st.name() == ssi ) {
              int bin = m_stream_map[sss.first];
              m_buffer->Fill(bin - 0.5, output);
              inSpecialGroup = true;
            }
          }
        }
        if(!inSpecialGroup) {
          m_buffer->Fill(m_stream_map[m_specialStream_OtherName]-0.5, output);
        }
        
      } else if(isL2 && st.type() == "physics") {
        L2PhysicsStreamHit = true;
      }

      // fill generic types (this will give recording i.e. duplication rate) 
      float x = m_stream_map[st.type()]-0.5;

      m_buffer->Fill( x, output); // recording per type

      // fill particular strings
      // construct the name
      std::string n = "str_"+st.name()+"_"+st.type();
      
      if ( m_stream_map[n] != 0 ) { 
        // it might happen that the ST unknown in the configuration will apear. i.e. error tag
        float x = m_stream_map[n]-0.5;
        m_buffer->Fill( x, output);
      }
    }
  }
  // total input
  m_buffer->Fill(m_stream_map["total"]-0.5, input);

  if(isL2 && L2PhysicsStreamHit)
    m_buffer->Fill(m_stream_map["total_physics"]-0.5, output);

  // total ouptut
  if ( !streamTags.empty() ) 
    m_buffer->Fill(m_stream_map["total"]-0.5, output);

}

void TrigRateMoni::fillChainAndGroupBins() {
  // clear helpers for groups
  
  if ( m_doGroups ) {
    for(GroupHelper::value_type& gh : m_group_helper) 
      gh.second.clear();
  }

  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();
  
  bool eventPassed = false;  
  bool isPhysicsAccept = false;
  for (const HLT::SteeringChain* chain : activeChains) {
    // check whether the event is accepted
    eventPassed = chain->chainPassed() || eventPassed;
    if ( chain->chainPassed() ) {
      for (auto chain_stream : chain->getStreamTags()){
        if ( chain_stream.getType() == "physics" ){
          isPhysicsAccept=true;
          break;
        }
      }
    }
    if (isPhysicsAccept) break;
  }  


  for (const HLT::SteeringChain* ch : activeChains) {
    if ( m_doChains ) {
      if ( m_chains_map[ch] != 0 ) {	
        float x = m_chains_map[ch] - 0.5;  
        
        if (!ch->isResurrected() ){ // not prescaled chains
          
          m_buffer->Fill(x, input);
          
          if (! ch->isPrescaled() )
            m_buffer->Fill(x, prescale );
          
          if ( !ch->isPrescaled() || ch->isPassedThrough() )
            m_buffer->Fill(x, algoIn );
          
          if ( ch->chainPassedRaw())
            m_buffer->Fill(x, raw );
          
          if ( ch->chainPassedRaw() || ch->isPassedThrough() ) 
            m_buffer->Fill(x, output );
          
        } else if ( ch->runInSecondPass() && isPhysicsAccept) {// prescaled chains in rerun
          
          m_buffer->Fill(x, rerun);
          
          if ( ch->chainPassedRaw()) 
            m_buffer->Fill(x, passedrerun );
        }
      } else {
        ATH_MSG_ERROR("the chain " << ch->getChainName() << " is unknown ");
      }
    }
    if ( m_doGroups ) {
      if ( m_groups_map.count(ch) != 0  ) {
        // keep the state for the group
        for ( int bin : m_groups_map[ch] ) {
          if ( !ch->isResurrected() ) {
            m_group_helper[bin].input = true;
            
            if ( !ch->isPrescaled()) 
              m_group_helper[bin].not_ps = true;
            
            if ( ch->chainPassedRaw())
              m_group_helper[bin].raw = true;
            
            if ( ch->chainPassedRaw() || ch->isPassedThrough() ) 
              m_group_helper[bin].output = true;
            
          } else if ( ch->runInSecondPass() ) {
            m_group_helper[bin].rerun = true;
          }
        }
      } 
    }
  } // OF loop over chains

  if ( m_doGroups ) {
    for(GroupHelper::value_type& gh : m_group_helper) {
      if (gh.second.input) {
        float x = gh.first - 0.5;  
        m_buffer->Fill(x,input);
      }
      
      if (gh.second.not_ps) {
        float x = gh.first - 0.5;  
        m_buffer->Fill(x,prescale);
      }
      
      if (gh.second.raw) {
        float x = gh.first - 0.5;  
        m_buffer->Fill(x,raw);
      }
      
      if (gh.second.output) {
        float x = gh.first - 0.5;  
        m_buffer->Fill(x,output);
      }
      
      if (gh.second.rerun) {
        float x = gh.first - 0.5;  
        m_buffer->Fill(x,rerun);
      }      
    } // EOF loop
  }
}
