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
#include <Python.h>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <execinfo.h>
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;

namespace
{
  class ProcessProxy
  {
  public:
    ProcessProxy(hltinterface::HLTInterface& interface,
                 const eformat::read::FullEventFragment& e,
                 HLTTestApps::TimeoutGuard& watchdog,
                 uint32_t max_result_size);
                 
    /**
     * Call operator. Processes the event
     */
    eformat::helper::u32list operator()();
    
  private:
    hltinterface::HLTInterface& m_interface;
    const eformat::read::FullEventFragment& m_event;
    hltinterface::HLTResult m_hltr;
    HLTTestApps::TimeoutGuard& m_watchdog;
    uint32_t m_max_result_size;
  };

  /**
   * Programatically obtain and print a stack trace.
   */
  void print_stack_trace ()
  {
    static const int TRACE_DEPTH=128;
    void * array[TRACE_DEPTH];
    int nSize = backtrace(array, TRACE_DEPTH);
    char ** symbols = backtrace_symbols(array, nSize);
    std::cerr << "Found " << nSize << " backtrace symbols." << std::endl;
    for (int i = 0; i < nSize; i++) std::cerr << symbols[i] << std::endl;
    free(symbols);
  }

  /*
   * A PEB specification type consisting of a pair with a set of robs and a set
   * of sub-detectors
   */
  using PebSpec = std::pair<std::set<uint32_t>, std::set<eformat::SubDetector>>;

  /*
   * If all tags specify PEB lists, get a pair with a list of ROB IDs and a list
   * of SubDetector IDs, corresponding to the union of the PEB lists specified
   * in all tags. Otherwise (at least one tag specifies FEB), get a pair with
   * two empty sets.
   */
  PebSpec get_peb_list(const std::vector<eformat::helper::StreamTag>& tags)
  {
    std::pair<std::set<uint32_t>, std::set<eformat::SubDetector>> ret;
    for(const auto& t : tags)
    {
      if(!t.robs.empty() || !t.dets.empty())
      {
        copy(begin(t.robs), end(t.robs), inserter(ret.first,
                                                  begin(ret.first)));
        copy(begin(t.dets), end(t.dets), inserter(ret.second,
                                                  begin(ret.second)));
      }
      else // at least one tag requires full event building
      {
        // we clear everything and we're done (empty return signals no PEB)
        ret.first.clear();
        ret.second.clear();
        break;
      }
    }

    return ret;
  }

  /*
   * select robs from src_robs following peb_spec, and put them into selection,
   * filtering result robs or not according to filter_hltrs
   */
  void choose_robs(std::vector<const uint32_t*>& selection,
                   const std::vector<eformat::read::ROBFragment>& src_robs,
                   const PebSpec& peb_spec,
                   bool filter_hltrs)
  {
    // for each rob...
    for(const auto& rob : src_robs)
    {
      auto sid = rob.source_id();
      auto sdid = eformat::helper::SourceIdentifier{sid}.subdetector_id();
      if(!filter_hltrs || // ... if we don't need to filter...
         (sdid != eformat::TDAQ_HLT &&  // ... or if we don't have to filter
          sdid != eformat::TDAQ_LVL2))  // this particular rob...
      {
        auto end_peb_robs = end(peb_spec.first);
        auto end_peb_dets = end(peb_spec.second);
        bool peb_empty = peb_spec.first.empty() && peb_spec.second.empty();
        if(peb_empty || // ... if we need FEB, or if the rob is covered by PEB
           find(begin(peb_spec.first), end_peb_robs, sid) != end_peb_robs ||
           find(begin(peb_spec.second), end_peb_dets, sdid) != end_peb_dets)
        {
          selection.push_back(rob.start()); // then we select it
        }
      }
    }
  }

  /*
   * Choose which ROBs, from both src and hltr_robs, to put into the output
   * event. If FEB is required, all robs from hltr_robs are included in the
   * output, as well as all robs from src, with the exception of HLT results.
   * If PEB is required, only the robs covered by it are included. HLTR robs
   * from src are still not included in this case.
   */
  std::vector<const uint32_t*>
  choose_event_payload(const eformat::read::FullEventFragment& src,
                       const std::vector<eformat::read::ROBFragment>& hltr_robs,
                       const std::vector<eformat::helper::StreamTag>& tags)
  {
    std::vector<const uint32_t*> ret;
    std::vector<eformat::read::ROBFragment> src_robs;
    src.robs(src_robs);
    ret.reserve(src_robs.size() + hltr_robs.size());

    auto peb_spec = get_peb_list(tags);
    choose_robs(ret, src_robs, peb_spec, true);
    choose_robs(ret, hltr_robs, peb_spec, false);

    return ret;
  }
}

ProcessProxy::ProcessProxy(hltinterface::HLTInterface& interface,
                           const eformat::read::FullEventFragment& e,
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
  retval.lvl2_trigger_info(0, nullptr); // drop l2 trigger bits
  
  //set the trigger info if needed
  uint32_t* hlt_info = 0;
  if(m_hltr.trigger_info.size()) 
  {
    hlt_info = new uint32_t[m_hltr.trigger_info.size()];
    for(unsigned i = 0; i < m_hltr.trigger_info.size(); ++i) 
      hlt_info[i] = m_hltr.trigger_info[i];
    retval.hlt_info(m_hltr.trigger_info.size(), hlt_info);
  }
  
  //set the stream tags
  uint32_t* stream_tag = 0;
  if (m_hltr.stream_tag.size()) {
    uint32_t size = eformat::helper::size_word(m_hltr.stream_tag);
    stream_tag = new uint32_t[size];
    eformat::helper::encode(m_hltr.stream_tag, size, stream_tag);
    retval.stream_tag(size, stream_tag);
  }
  
  //choose the stuff to put on the final event output
  auto to_pack = choose_event_payload(m_event,
                                      m_hltr.hltResult_robs,
                                      m_hltr.stream_tag);

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
    

  // this signals the event is finished (no more robs will be added)
  // If activated, compression kicks in now
  const eformat::write::node_t* top = retval.bind();
  
    //finally, we serialize the new event and return that
  uint32_t final_size = retval.size_word();
  uint32_t* final_event = new uint32_t[final_size];
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
   const eformat::read::FullEventFragment& e,
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

using namespace boost::property_tree;
namespace
{
  auto wsettings = xml_parser::xml_writer_make_settings(' ', 2);
}

string HLTTestApps::to_string(const ptree& p)
{
  std::ostringstream oss;
  xml_parser::write_xml(oss, p, wsettings);
  return oss.str();
}

void HLTTestApps::print_ptree(const ptree& p)
{
  xml_parser::write_xml(std::cout, p, wsettings);
}
