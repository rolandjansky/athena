/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file src/util.cxx
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implements a few of the utilities in util.h  
 */

#include "util.h"
#include "hltinterface/EventId.h"
#include "eformat/index.h"
#include "eformat/eformat.h"
#include "eformat/write/eformat.h"
#include "owl/time.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Event.h" // datacollector implementation
#include <boost/foreach.hpp>
#include <Python.h>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <execinfo.h>

namespace
{
  class ProcessProxy
  {
  public:
    ProcessProxy(hltinterface::HLTInterface& interface,
                 eformat::read::FullEventFragment& e,
                 HLTTestApps::TimeoutGuard& watchdog,
                 uint32_t max_result_size);
                 
    eformat::helper::u32list operator()();
    
  private:
    hltinterface::HLTInterface& m_interface;
    eformat::read::FullEventFragment& m_event;
    hltinterface::HLTResult m_hltr;
    HLTTestApps::TimeoutGuard& m_watchdog;
    uint32_t m_max_result_size;
  };

}

/**
 * Programatically obtain and print a stack trace.
 */
static void print_stack_trace ()
{
  static const int TRACE_DEPTH=128;
  void * array[TRACE_DEPTH];
  int nSize = backtrace(array, TRACE_DEPTH);
  char ** symbols = backtrace_symbols(array, nSize);
  std::cerr << "Found " << nSize << " backtrace symbols." << std::endl;
  for (int i = 0; i < nSize; i++) std::cerr << symbols[i] << std::endl;
  free(symbols);
}
 
/**
 * Decides wether we need partial event building or not.
 */
static bool do_partial_build(const std::vector<eformat::helper::StreamTag>& st) {
  if (!st.size()) return false;
  
  std::set<uint32_t>::size_type n_robs = 0;
  std::set<eformat::SubDetector>::size_type n_sds = 0;
  bool only_calibration = true;
  for (size_t i=0; i<st.size(); ++i) {
    eformat::TagType t = eformat::helper::string_to_tagtype(st[i].type);
    if (t != eformat::CALIBRATION_TAG) {
      only_calibration = false;
      break;
    }
    else
    {
      n_robs += st[i].robs.size();
      n_sds += st[i].dets.size();
    }
  }
  return only_calibration && (n_robs || n_sds); 
}

/**
 * This non-member method will choose the contents of the "src" event,
 * respecting the following directives, in this order of preference:
 *
 * 1. If a "SubDetector" is to be excluded, no entries for that particular
 *   SubDetector will be found in the finally returned event. Use this to
 *   remove existing L2 or EF results from the data.
 * 2. If nothing is mentioned in "include_sd" or "include_source", then, we copy
 *   everything that was not excluded.
 * 3. If anything is mentioned in either the "include_" parameters, those
 *   parts will be included additively, iff do_partial_build() is true
 */
static void 
choose_event_payload(const eformat::read::FullEventFragment& src,
                     const std::vector<eformat::helper::StreamTag>& tags,
                     eformat::SubDetector exclude_sd,
                     std::vector<const uint32_t*>& choosen)
{
  if(do_partial_build(tags)) 
  {
    std::set<const uint32_t*> ret; // using a set to avoid duplicates 
    std::map<eformat::SubDetector, std::vector<const uint32_t*> > toc1;
    eformat::helper::build_toc(src, toc1);
    std::map<uint32_t, const uint32_t*> toc2;
    eformat::helper::build_toc(src, toc2);
    
    BOOST_FOREACH(const eformat::helper::StreamTag& tag, tags)
    {
      BOOST_FOREACH(const eformat::SubDetector& sd, tag.dets)
      {
        std::map<eformat::SubDetector, std::vector<const uint32_t*> >::iterator
                                                        toc_pos = toc1.find(sd);
        if(toc_pos != toc1.end() && toc_pos->first != exclude_sd)
          ret.insert(toc_pos->second.begin(), 
                     toc_pos->second.end());
      }
      BOOST_FOREACH(uint32_t rob, tag.robs)
      {
        std::map<uint32_t, const uint32_t*>::iterator toc_pos = toc2.find(rob);
        if(toc_pos != toc2.end() &&
           eformat::helper::SourceIdentifier(toc_pos->first).subdetector_id() 
                                                                != exclude_sd)
          ret.insert(toc_pos->second);
      }
    }
    // filling in choosen
    choosen.insert(choosen.begin(), ret.begin(), ret.end());
  }
  else {
    std::map<uint32_t, const uint32_t*> toc;
    eformat::helper::build_toc(src, toc);
    for (std::map<uint32_t, const uint32_t*>::iterator 
         i = toc.begin(); i != toc.end(); ++i) 
      if(eformat::helper::SourceIdentifier(i->first).subdetector_id() 
                                                        != exclude_sd)
        choosen.push_back(i->second);
  }
}

ProcessProxy::ProcessProxy(hltinterface::HLTInterface& interface,
                           eformat::read::FullEventFragment& e,
                           HLTTestApps::TimeoutGuard& watchdog,
                           uint32_t max_result_size)
  : m_interface(interface)
  , m_event(e)
  , m_watchdog(watchdog)
  , m_max_result_size(max_result_size)
{
  m_hltr.max_result_size = m_max_result_size;
  m_hltr.fragment_pointer = new uint32_t[m_max_result_size];
  m_hltr.fragment_pointer[0] = 0; //makes WW happier
}

eformat::helper::u32list ProcessProxy::operator()()
{
  try
  {
    HLTTestApps::Event proxy(m_event); //our DataCollector interface
    proxy.register_datacollector();
    auto eid = hltinterface::EventId{m_event.global_id(),
                                     m_event.lvl1_id(),
                                     m_event.lumi_block()};

    m_watchdog.start();
    if(!m_interface.process(proxy.l1r(), m_hltr, eid))
      ers::warning(HLTTESTAPPS_UNCLASSIFIED("HLT framework has not returned OK "
                                            "for process() call"));
    m_watchdog.reset();
  }
  catch (ers::Issue& e) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Uncaught ers::Issue left HLT framework: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  catch (std::exception& e) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Uncaught std::exception left HLT framework: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  catch (...) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Unknown left HLT framework");
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
      
  //do we need to do continue?
  if (m_hltr.stream_tag.size() == 0) 
  {
    delete[] m_hltr.fragment_pointer;
    return eformat::helper::u32list(0, 0, 0);
  }
      
  //check the quality of data we get back
  try 
  {
    eformat::ROBFragment<const uint32_t*>(m_hltr.fragment_pointer).check();
  }
  catch (ers::Issue& e) 
  {
    boost::format msg("HLT result ROB does not validate: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  
  eformat::write::FullEventFragment retval;
  retval.copy_header(m_event.start());
  
  //reset the trigger info if needed
  uint32_t* hlt_info = 0;
  if(m_hltr.trigger_info.size()) 
  {
    hlt_info = new uint32_t[m_hltr.trigger_info.size()];
    for(unsigned i = 0; i < m_hltr.trigger_info.size(); ++i) 
      hlt_info[i] = m_hltr.trigger_info[i];
    retval.hlt_info(m_hltr.trigger_info.size(), hlt_info);
  }
  
  //reset the stream tags
  uint32_t* stream_tag = 0;
  if (m_hltr.stream_tag.size()) {
    uint32_t size = eformat::helper::size_word(m_hltr.stream_tag);
    stream_tag = new uint32_t[size];
    eformat::helper::encode(m_hltr.stream_tag, size, stream_tag);
    retval.stream_tag(size, stream_tag);
  }
  
  //choose the stuff to put on the final event output
  std::vector<const uint32_t*> to_pack;
  choose_event_payload(m_event, m_hltr.stream_tag, eformat::TDAQ_HLT, to_pack);
  // add hltResult_robs' pointers to the list of stuff "to_pack" into the event
  std::transform(begin(m_hltr.hltResult_robs), end(m_hltr.hltResult_robs),
                 std::back_inserter(to_pack),
                 [](const eformat::read::ROBFragment& rob){
    return rob.start();
  });
    
  //copy the selected ROB fragments to the output event
  std::vector<eformat::write::ROBFragment> rob;
  for(std::vector<const uint32_t*>::const_iterator it = to_pack.begin(); it != to_pack.end(); ++it)
    try
    {
      rob.push_back(*it);
    }
    catch(const eformat::Issue&)
    {
      ers::warning(HLTTESTAPPS_UNCLASSIFIED("Got an invalid ROBFragment that will be skipped"));
    }

  for(unsigned i = 0; i < rob.size(); ++i) 
    retval.append(&rob[i]);
    
  //finally, we serialize the new event and return that
  uint32_t final_size = retval.size_word();
  uint32_t* final_event = new uint32_t[final_size];
  const eformat::write::node_t* top = retval.bind();
  uint32_t result = eformat::write::copy(*top, final_event, final_size);
  if (final_size != result) {
    boost::format msg("Event serialization failed. Serialized %lu words out of %lu");
    msg % result % final_size;
    delete[] m_hltr.fragment_pointer;
    delete[] hlt_info;
    delete[] stream_tag;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  
  //cleanup
  delete[] m_hltr.fragment_pointer;
  delete[] hlt_info;
  delete[] stream_tag;
  
  return eformat::helper::u32list(final_event, 0, final_size);
}

eformat::helper::u32list HLTTestApps::process
  (boost::shared_ptr<hltinterface::HLTInterface> interface,
   eformat::read::FullEventFragment& e,
   HLTTestApps::TimeoutGuard& watchdog,
   uint32_t max_result_size)
{
  ProcessProxy pp(*interface, e, watchdog, max_result_size);
  return pp();
}

void HLTTestApps::python_prompt(void) {
  PyRun_InteractiveLoop(stdin, const_cast< char* >( "\0" ));
}

void HLTTestApps::python_exec(const std::string& command) {
  int result = PyRun_SimpleString(command.c_str());
  if (result != 0) {
    boost::format msg("Error detected while executing '%s'.");
    msg % command;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
}

void HLTTestApps::python_execfile(const std::string& filename) {
  FILE *fp;
  if((fp = fopen(filename.c_str(), "rb")) == NULL) {
    boost::format msg("Cannot open file '%s'.");
    msg % filename;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  int result = PyRun_SimpleFileEx(fp, filename.c_str(), 1); //close "fp"
  if (result != 0) {
    boost::format msg("Error detected while executing file '%s'.");
    msg % filename;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
}

std::string HLTTestApps::tdaq_time_str_from_microsec(time_t sec, time_t microsec)
{
  auto * cstr = OWLTime{sec, microsec}.c_str();
  auto ret = std::string{cstr};
  delete[] cstr;
  return ret;
}

bool HLTTestApps::finalize_hist_svc()
{
  ServiceHandle<ITHistSvc> thsvc("THistSvc", "psc::Psc");
  return thsvc->sysFinalize().isSuccess();
}

bool HLTTestApps::initialize_hist_svc()
{
  ServiceHandle<ITHistSvc> thsvc("THistSvc", "psc::Psc");
  return thsvc->sysInitialize().isSuccess();
}

void HLTTestApps::ers_debug_level(int level) {
  ers::Configuration::instance().debug_level(level);
}

int HLTTestApps::get_ers_debug_level() {
  return ers::Configuration::instance().debug_level();
}

void HLTTestApps::ers_debug(const std::string& msg)
{
    ers::debug(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_info(const std::string& msg)
{
    ers::info(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_warning(const std::string& msg)
{
    ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_error(const std::string& msg)
{
    ers::error(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_fatal(const std::string& msg)
{
    ers::fatal(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ipc_init()
{
  try
  {
    IPCCore::init({});
  }
  catch(const daq::ipc::AlreadyInitialized& e)
  {
    ers::log(e);
  }
}
