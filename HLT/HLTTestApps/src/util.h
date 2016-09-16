/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HLTTestApps/src/util.h
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Utilties required by the python bindings to work cooperatively in
 * HLT in a nice way.
 */

#ifndef HLTTESTAPPS_UTIL_H 
#define HLTTESTAPPS_UTIL_H

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <ctime>
#include "issue.h"
#include "eformat/FullEventFragment.h"
#include "eformat/blob.h"
#include "hltinterface/HLTInterface.h"
#include "hltinterface/HLTResult.h"
#include "dynlibs/Module.h"
#include "TimeoutGuard.h"
#include "ipc/core.h"

namespace HLTTestApps {

  constexpr char docstr[] = "See doxygen documentation (from C++ code)";

  /**
   * Loads an HLTInterface from libs.
   */
  template <class T> boost::shared_ptr<T> load_impl(boost::python::list libs);

  /**
   * Process an event. Uses the provided HLTInterface to process an event.
   * Filters earlier HLT/L2/EF results out. Checks the validity of the HLTResult
   * fragment provided by the HLT. Resets L2 trigger bits (which would be
   * present in events from Run 1). Monitors the HLT's execution time through
   * a TimeoutGuard.
   *
   * @param interface The interface to the HLT (which processes the event)
   * @param e The input event
   * @param watchdog The timeout guard thread
   * @param max_result_size The maximum size the HLTResult can occupy
   * @return A possibly empty event that is serialized in the form of an u32list.
   * This is an empty list if the event is not selected by the HLTInterface.
   * Otherwise, it is a valid event with the new HLTResult, the new trigger info
   * bits, the new stream tag(s) and without any original L2/EF/HLT result. This
   * event is a partial event @a iff the HLT produced calibration and only
   * calibration stream tags (tags with the type CALIBRATION_TAG, including data
   * scouting).
   */
  eformat::helper::u32list process
    (boost::shared_ptr<hltinterface::HLTInterface> interface,
     const eformat::read::FullEventFragment& e,
     HLTTestApps::TimeoutGuard& watchdog,
     uint32_t max_result_size=hltinterface::HLTResult::DEFAULT_MAX_RESULTSIZE);

  /**
   * Makes the application go immediately into the python prompt. Exiting that
   * prompt resumes.
   */
  void python_prompt(void);

  /**
   * Makes the application execute the given python statement inside the
   * jobOptions context. Execution resumes immediately after.
   *
   * @param command A string containing python statement (e.g. "print 1+2")
   */
  void python_exec(const std::string& command);

  /**
   * Makes the application execute the given python file inside the jobOptions
   * context. Execution resumes when the file ends.
   *
   * @param filename The name of the file that will be executed
   */
  void python_execfile(const std::string& filename);

  /**
   * Get a TDAQ compatible string representation of the time specified by the
   * sec and microsec parameters
   *
   * @param sec The number of seconds
   * @param microsec The number of micro seconds
   */
  std::string tdaq_time_str_from_microsec(time_t sec, time_t microsec);

  /**
   * Changes the ERS debug level, dynamically
   *
   * @param level The debugging level (0 means no messages will be printed),
   * options besides that are 1, 2 or 3.
   */
  void ers_debug_level(int level);
  
  /**
   * Get the current ERS debug level
   */
  int get_ers_debug_level();

  /**
   * A wrapper for ers::debug that takes a string message
   *
   * @param msg The message to log
   */
  void ers_debug(const std::string& msg);
  
  /**
   * A wrapper for ers::info that takes a string message
   *
   * @param msg The message to log
   */
  void ers_info(const std::string& msg);
  
  /**
   * A wrapper for ers::warning that takes a string message
   *
   * @param msg The message to log
   */
  void ers_warning(const std::string& msg);
  
  /**
   * A wrapper for ers::error that takes a string message
   *
   * @param msg The message to log
   */
  void ers_error(const std::string& msg);
  
  /**
   * A wrapper for ers::fatal that takes a string message
   *
   * @param msg The message to log
   */
  void ers_fatal(const std::string& msg);
  
  /**
   * Initialize the IPC
   */
  void ipc_init();

  /**
   * Get an XML format string representing a ptree.
   */
  std::string to_string(const boost::property_tree::ptree& p);

  /**
   * Print a ptree in XML format (Useful to print ptree's within gdb).
   */
  void print_ptree(const boost::property_tree::ptree& p);
}

template <class T> 
boost::shared_ptr<T> HLTTestApps::load_impl(boost::python::list l) {
  // The factory function that we are loading
  typedef T* (*factory_func)();
  // A function that is a const member of Module, that receives a const string 
  //  reference and that returns a factory_func
  typedef factory_func (Module::*my_symbol_func)(const std::string&)const;
  
  ERS_DEBUG(1, "Going to load algorithm libraries...");

  std::vector<std::string> libs;
  for (long i=0; i<PyList_Size(l.ptr()); ++i) { 
    PyObject* item = PyList_GetItem(l.ptr(), i);
    if (PyString_Check(item)) libs.push_back(PyString_AsString(item));
    else {
      boost::format msg("Entry %ld at input list is not string");
      msg % i;
      throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
    }
  }

  if (libs.size() == 0) {
    boost::format msg("Cannot load implementation with zero libraries...");
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }

  std::vector<std::string> other_modules; // no other modules
  std::string module_name = libs[0] + "_framework";

  // Load and configure the libraries for the first time
  Module::add(module_name, libs, other_modules);
  const Module* handle = Module::get(module_name);
  if (handle == 0) {
    std::ostringstream o;
    o << "[ ";
    for (unsigned int i=0; i<(libs.size()-1); ++i) 
      o << "'" << libs[i] << "', ";
    o << "'" << libs[libs.size()-1] << "'";
    o << " ]";
    boost::format msg("Failed to load shared libraries: %s");
    msg % o.str();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
   
  // Loading our factory function and then creating our interface.
  //  We cast the Module::symbol method to something that returns the 
  //  factory function we need, instead of a void*. We go this way because ISO
  //  C++ forbids casts between pointers to objects and pointers to functions.
  //  If we were to obtain our factory as a void* and then cast it to 
  //  factory_func we wouldn't be respecting the standard and we would always
  //  get an annoying g++43 warning about this.
  //T* (*sym)() = (handle->*reinterpret_cast<T* (*(Module::*) (const std::string&) const) ()>(&Module::symbol))("factory");    
  factory_func sym = (handle->*reinterpret_cast<my_symbol_func>(&Module::symbol))("hlt_factory");

  
  if (sym == 0) {
    boost::format msg("Failed to locate the factory() function inside shared object.");
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  T * interface = sym();

  if (interface == 0) {
    boost::format msg("Failed to get the Interface implementation");
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }

  return boost::shared_ptr<T>(interface);
}

#endif /* HLTTESTAPPS_UTIL_H */

