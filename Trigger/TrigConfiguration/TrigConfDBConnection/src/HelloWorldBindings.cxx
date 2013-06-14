/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/python.hpp>

#include "TrigConfDBConnection/HelloWorld.h"


BOOST_PYTHON_MODULE(hello_world)
{
   using namespace boost::python;
   class_<HelloWorld>("HelloWorld", init<std::string>() )
      .def("greetings", &HelloWorld::greetings)
      .def("setGreetings", &HelloWorld::setGreetings)
      .def("setStrPtr", &HelloWorld::setStrPtr)
      ;
}
