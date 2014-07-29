/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InfoService_wrap.cxx
 * @author Ricardo Abreu
 *
 * @brief Python bindings for InfoService
 */

#include "InfoService_wrap.h"
#include "issue.h"

#include "HLTMPPU/InfoService.h"
#include "dynlibs/Module.h"

#include <boost/python.hpp>

#include <memory>
#include <string>
#include <vector>


using namespace std;
using namespace boost::python;
using namespace boost::property_tree;
using HLTMP::InfoService;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  const string factory_name = "create_hltmp_infoservice";

  //////////////////////////////////////////////////////////////////////////////
  shared_ptr<InfoService> create_InfoService(const string& lib)
  {
    typedef InfoService* (*factory)();

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
      boost::format msg{"Failed to create InfoService with function %s "
                        "from library %s"};
      throw(HLTTESTAPPS_UNCLASSIFIED((msg % factory_name % lib).str()));
    }

    return shared_ptr<InfoService>(raw);
  }
}

////////////////////////////////////////////////////////////////////////////////
void HLTTestApps::wrap_InfoService()
{
  class_<InfoService, boost::noncopyable>("InfoService", no_init)
      .def("__init__", make_constructor(&create_InfoService))
      .def("configure", &InfoService::configure)
      .def("prepareForRun", &InfoService::prepareForRun)
      .def("preRunHook", &InfoService::preRunHook)
      .def("postRunHook", &InfoService::postRunHook)
      .def("finalize", &InfoService::finalize);
}


