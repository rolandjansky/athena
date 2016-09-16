/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IInfoRegister_wrap.cxx
 * @author Ricardo Abreu
 *
 * @brief Python bindings for hltinterface::IInfoRegister
 */

#include "IInfoRegister_wrap.h"
#include "issue.h"
#include "util.h"

#include "hltinterface/IInfoRegister.h"
#include "dynlibs/Module.h"

#include <boost/python.hpp>

#include <memory>
#include <string>
#include <vector>


using namespace std;
using namespace boost::python;
using namespace boost::property_tree;
using hltinterface::IInfoRegister;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  const string factory_name = "create_hltmp_infoservice";

  //////////////////////////////////////////////////////////////////////////////
  shared_ptr<IInfoRegister> create_IInfoRegister(const string& lib)
  {
    typedef IInfoRegister* (*factory)();

    Module::add("InfoService", vector<string>{lib}, vector<string>{});
    auto fact = Module::get("InfoService")->function<factory>(factory_name);

    if(!fact)
    {
      boost::format msg{"Failed to locate function %s in library %s"};
      throw(HLTTESTAPPS_UNCLASSIFIED((msg % factory_name % lib).str()));
    }

    auto raw = fact();
    if(!raw)
    {
      boost::format msg{"Failed to create IInfoRegister with function %s "
                        "from library %s"};
      throw(HLTTESTAPPS_UNCLASSIFIED((msg % factory_name % lib).str()));
    }

    return shared_ptr<IInfoRegister>(raw);
  }
}

////////////////////////////////////////////////////////////////////////////////
void HLTTestApps::wrap_IInfoRegister()
{
  class_<IInfoRegister, boost::noncopyable>("IInfoRegister", docstr, no_init)
      .def("__init__", make_constructor(&create_IInfoRegister), docstr)
      .def("configure", &IInfoRegister::configure, docstr)
      .def("prepareForRun", &IInfoRegister::prepareForRun, docstr)
      .def("prepareWorker", &IInfoRegister::prepareWorker, docstr)
      .def("finalizeWorker", &IInfoRegister::finalizeWorker, docstr)
      .def("finalize", &IInfoRegister::finalize, docstr);
}


