/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file src/bindings.cxx
 * @author Andre Anjos, Ricardo Abreu
 *
 * @brief Python bindings linking to the HLT Interface.
 */

#include <string>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
#include "eformat/python/util.h"
#include "ptree_wrap.h"
#include "InfoService_wrap.h"
#include "util.h"
#include "Event.h"
#include "issue.h"
#include "TimeoutGuard.h"

using namespace boost::python;
using namespace boost::property_tree;

/*
 * Wrapper for hltinterface::HLTInterface::hltUserCommand that can receive the
 * user parameters as a python tuple and puts everything in the required ptree
 */
bool hltUserCommand(hltinterface::HLTInterface& interface,
                    const std::string& usrCmd, 
                    tuple usrParam)
{
    ptree pt(usrCmd);
    int len = extract<int>(usrParam.attr("__len__")());
    for(int i = 0; i < len; i++)
        pt.put("Params." + boost::lexical_cast<std::string>(i),
               extract<std::string>(usrParam[i])());
        
    return interface.hltUserCommand(pt);
}

BOOST_PYTHON_FUNCTION_OVERLOADS(process_overloads,
    HLTTestApps::process, 3, 4)

BOOST_PYTHON_MODULE(libpyhlttestapps)
{
  //a catch-all for exception types
  register_exception_translator<ers::Issue>(&eformat::python::translate_ers_issue);

  // wrap the boost ptree type
  HLTTestApps::wrap_ptree();

  // wrap the InfoService type
  HLTTestApps::wrap_InfoService();
 
  class_<HLTTestApps::TimeoutGuard, boost::noncopyable>("TimeoutGuard")
    .def(init<unsigned int, float, hltinterface::HLTInterface*>())
    .def("start", &HLTTestApps::TimeoutGuard::start)
    .def("reset", &HLTTestApps::TimeoutGuard::reset)
    ;

  class_<hltinterface::HLTInterface, boost::noncopyable>("HLTInterface", no_init)
    .def("__init__", make_constructor(&HLTTestApps::load_impl<hltinterface::HLTInterface>))
    .def("configure", &hltinterface::HLTInterface::configure)
    .def("connect", &hltinterface::HLTInterface::connect)
    .def("prepareForRun", &hltinterface::HLTInterface::prepareForRun)
    .def("process", HLTTestApps::process, process_overloads())
    .def("stopRun", &hltinterface::HLTInterface::stopRun)
    .def("disconnect", &hltinterface::HLTInterface::disconnect)
    .def("unconfigure", &hltinterface::HLTInterface::unconfigure)
    .def("publishStatistics", &hltinterface::HLTInterface::publishStatistics)
    .def("prepareWorker", &hltinterface::HLTInterface::prepareWorker)
    .def("finalizeWorker", &hltinterface::HLTInterface::finalizeWorker)
    .def("hltUserCommand", hltUserCommand)
    ;

  def("set_ros2rob_map", &HLTTestApps::Event::set_ros2rob_map);
  def("python_prompt", &HLTTestApps::python_prompt);
  def("python_exec", &HLTTestApps::python_exec);
  def("python_execfile", &HLTTestApps::python_execfile);
  def("tdaq_time_str_from_microsec", &HLTTestApps::tdaq_time_str_from_microsec);
  def("finalize_hist_svc", &HLTTestApps::finalize_hist_svc);
  def("initialize_hist_svc", &HLTTestApps::initialize_hist_svc);
  def("ers_debug_level", &HLTTestApps::ers_debug_level);
  def("get_ers_debug_level", &HLTTestApps::get_ers_debug_level);
  def("ers_debug", &HLTTestApps::ers_debug);
  def("ers_info", &HLTTestApps::ers_info);
  def("ers_warning", &HLTTestApps::ers_warning);
  def("ers_error", &HLTTestApps::ers_error);
  def("ers_fatal", &HLTTestApps::ers_fatal);
  def("ipc_init", &HLTTestApps::ipc_init);

}
