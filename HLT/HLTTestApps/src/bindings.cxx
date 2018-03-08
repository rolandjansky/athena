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
#include "IInfoRegister_wrap.h"
#include "util.h"
#include "Event.h"
#include "issue.h"
#include "TimeoutGuard.h"

using namespace boost::python;
using namespace boost::property_tree;
using namespace HLTTestApps;
using eformat::python::translate_ers_issue;

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
    process, 3, 4)

BOOST_PYTHON_MODULE(libpyhlttestapps)
{
  //a catch-all for exception types
  register_exception_translator<ers::Issue>(&translate_ers_issue);

  // wrap the boost ptree type
  wrap_ptree();

  // wrap the IInfoRegister type
  wrap_IInfoRegister();
 
  class_<TimeoutGuard, boost::noncopyable>("TimeoutGuard", docstr, no_init)
    .def(init<unsigned int, float, hltinterface::HLTInterface*>(docstr))
    .def("start", &TimeoutGuard::start, docstr)
    .def("reset", &TimeoutGuard::reset, docstr)
    ;

  class_<hltinterface::HLTInterface, boost::noncopyable>("HLTInterface", docstr,
                                                         no_init)
    .def("__init__", make_constructor(&load_impl<hltinterface::HLTInterface>),
                     docstr)
    .def("configure", &hltinterface::HLTInterface::configure, docstr)
    .def("connect", &hltinterface::HLTInterface::connect, docstr)
    .def("prepareForRun", &hltinterface::HLTInterface::prepareForRun, docstr)
    .def("process", process, process_overloads(docstr))
    .def("stopRun", &hltinterface::HLTInterface::stopRun, docstr)
    .def("disconnect", &hltinterface::HLTInterface::disconnect, docstr)
    .def("unconfigure", &hltinterface::HLTInterface::unconfigure, docstr)
    .def("publishStatistics", &hltinterface::HLTInterface::publishStatistics,
                              docstr)
    .def("prepareWorker", &hltinterface::HLTInterface::prepareWorker, docstr)
    .def("finalizeWorker", &hltinterface::HLTInterface::finalizeWorker, docstr)
    .def("hltUserCommand", hltUserCommand, docstr)
    ;

  def("set_ros2rob_map", &Event::set_ros2rob_map, docstr);
  def("set_l1r_robs", &Event::set_l1r_robs, docstr);
  def("set_dcm_strategy", &Event::set_dcm_strategy, docstr);
  def("print_ros_hit_map", &Event::debug_print_ros_hit_map, docstr);
  def("python_prompt", &python_prompt, docstr);
  def("python_exec", &python_exec, docstr);
  def("python_execfile", &python_execfile, docstr);
  def("tdaq_time_str_from_microsec", &tdaq_time_str_from_microsec, docstr);
  def("ers_debug_level", &ers_debug_level, docstr);
  def("get_ers_debug_level", &get_ers_debug_level, docstr);
  def("ers_debug", &ers_debug, docstr);
  def("ers_info", &ers_info, docstr);
  def("ers_warning", &ers_warning, docstr);
  def("ers_error", &ers_error, docstr);
  def("ers_fatal", &ers_fatal, docstr);
  def("ipc_init", &ipc_init, docstr);
}
