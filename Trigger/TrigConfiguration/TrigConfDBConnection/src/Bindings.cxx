/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/python.hpp>

#include "frontier_client/frontier-cpp.h"
#include <iostream>

namespace {
   std::string escape_list="\\\'";
   std::string
   str_escape_quota(const std::string& str_orig) {
      std::string str(str_orig);
      std::string::size_type pos = 0;
      for(;;) {
         pos=str.find_first_of(escape_list,pos);
         if(pos==str.npos) break;
         str.insert(pos,1,'\\');    
         pos+=2;
      }
      return str;
   }
}


void getData(frontier::Session& s, const boost::python::list& l) {
   typedef std::vector<const frontier::Request*> RequestList;
   typedef std::vector<const frontier::Request*> RequestValueTypoe;

   RequestList cl;

   PyObject *obj_ptr = l.ptr();
   boost::python::handle<> obj_iter(PyObject_GetIter(obj_ptr));
   for(std::size_t i=0;;i++) {
      boost::python::handle<> 
         py_elem_hdl(boost::python::allow_null(PyIter_Next(obj_iter.get())));
      
      if (PyErr_Occurred()) boost::python::throw_error_already_set();
      if (!py_elem_hdl.get()) break; // end of iteration

      boost::python::object py_elem_obj(py_elem_hdl);
      boost::python::extract<const frontier::Request*> elem_proxy(py_elem_obj);

      bool success = elem_proxy.check();
      if (success) {
         cl.push_back(elem_proxy());
      } else if (i == 0) {
         break;
      } else {
         PyErr_SetString(PyExc_TypeError, "All items must be of same type.");
         boost::python::throw_error_already_set();
      }
   }
   s.getData(cl);
}

// // Determine if obj_ptr can be converted in a QString
//   static void* convertible(PyObject* obj_ptr)
//     {
//       if (!PyString_Check(obj_ptr)) return 0;
//       return obj_ptr;
//     }


PyObject*
assignString(frontier::Session& s) {
   std::string fieldvalue;
   s.assignString(&fieldvalue);
   return PyString_FromString(fieldvalue.c_str());
}


frontier::AnyData anydata;

boost::python::object class_anydata;

// frontier::AnyData
// getAnyData1(frontier::Session& s, int not_eor) {
//    s.getAnyData(&anydata, not_eor);
//    return anydata; 
// }

// frontier::AnyData
// getAnyData11(frontier::Session& s) { return getAnyData1(s,1); }

int
getAnyData2(frontier::Session & s, frontier::AnyData & anydata, int not_eor) {
   return s.getAnyData(&anydata, not_eor);
}

int
getAnyData21(frontier::Session & s, frontier::AnyData & anydata) { return getAnyData2(s, anydata, 1); }


PyObject*
getString(frontier::AnyData& data) {
   std::string* str = data.getString();
   if(str==0) return 0;
   return PyString_FromString(data.getString()->c_str());
}


enum BLOB_TYPE {
   BLOB_TYPE_BYTE       = 0,
   BLOB_TYPE_INT4       = 1,
   BLOB_TYPE_INT8       = 2,
   BLOB_TYPE_FLOAT      = 3,
   BLOB_TYPE_DOUBLE     = 4,
   BLOB_TYPE_TIME       = 5,
   BLOB_TYPE_ARRAY_BYTE = 6,
   BLOB_TYPE_EOR        = 7,
   BLOB_TYPE_NONE       = (1<<7),
   BLOB_BIT_NULL        = (1<<7)
};


BOOST_PYTHON_MODULE(libTrigConfDBFrontier)
{
   using namespace boost::python;

   /***
    * function frontier::init
    ***/
   int (*init1)() = &frontier::init;
   int (*init2)(const std::string&, const std::string&) = &frontier::init;

   def("init", init1, "Initializes frontier client.");
   def("init", init2, 
       "Initializes frontier client. \nloglevel can can be 'nolog', 'error', 'info' or 'warning' (which are equivalent), or anything else (treated as 'debug')",
       args("logfilename", "loglevel"));
   def("str_escape_quota", str_escape_quota, "add \\ before any \\\'", args("string"));
   def("getFieldTypeName", frontier::getFieldTypeName, "returns string representation of field type", args("BLOB_TYPE"));


   /***
    * class frontier::Connection
    ***/
   class_<frontier::Connection>("Connection", init<const std::string&, const std::string*>("Connection with server url and proxy url", args("self","serverurl","proxyurl")) )
      .def(init<const std::string&>("Connection with server url.", args("self","serverurl")))
      .def(init<const std::list<std::string>&, const std::list<std::string>&>("Connection with list of server urls and list of proxy urls",args("self","serverurl","proxyurl")))
      .def("setReload", &frontier::Connection::setReload)
      .def("setDefaultParams", &frontier::Connection::setDefaultParams)
      ;


   /***
    * To do the list wrapping we have two choices:
    *   * wrap them with class_<> ourself or
    *   * write to_python_converter() and some wrappers to extract<data> from python. 
    * Our goal is to get to the working prototype as soon as we can. So in simplistic (a) approach 
    * A full flexed wrapper is long and expensive, e.g. http://cci.lbl.gov/cctbx_sources/scitbx/array_family/boost_python/flex_wrapper.h
    ***/


   /***
    * class frontier::Session
    ***/

   //BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getAnyData_overloads, frontier::Session::getAnyData, 1, 2)
   //BOOST_PYTHON_FUNCTION_OVERLOADS(getAnyData2_overloads, getAnyData2, 2, 3)
      //    int (*init1)() = &frontier::init;
      //    int (*init2)(const std::string&, const std::string&) = &frontier::init;


   class_<frontier::Session>("Session", init<frontier::Connection*>() )
      .def("getData", &getData)
      .def("next", &frontier::Session::next)
      .def("isEOR", &frontier::Session::isEOR)
      .def("isEOF", &frontier::Session::isEOF)
      .def("assignString", &assignString)
      .def("getNumberOfRecords", &frontier::Session::getNumberOfRecords)
      .def("getRecNum", &frontier::Session::getRecNum)
      .def("setCurrentLoad", &frontier::Session::setCurrentLoad,
           "Each Request generates a payload. Payload numbers started with 1.\nSo, to get data for the first Request call setCurrentLoad(1)",
           args("self","n"))
      .def("getAnyData", &frontier::Session::getAnyData)
//       .def("getAnyData", &getAnyData1)
//       .def("getAnyData", &getAnyData11)
      .def("getAnyData", &getAnyData2)
      .def("getAnyData", &getAnyData21)
      ;

   /***
    * enum frontier::BLOB_TYPE
    ***/
   enum_<BLOB_TYPE>("BLOB_TYPE")
      .value("BLOB_TYPE_BYTE",       BLOB_TYPE_BYTE      )
      .value("BLOB_TYPE_INT4",       BLOB_TYPE_INT4      )
      .value("BLOB_TYPE_INT8",       BLOB_TYPE_INT8      )
      .value("BLOB_TYPE_FLOAT",      BLOB_TYPE_FLOAT     )
      .value("BLOB_TYPE_DOUBLE",     BLOB_TYPE_DOUBLE    )
      .value("BLOB_TYPE_TIME",       BLOB_TYPE_TIME      )
      .value("BLOB_TYPE_ARRAY_BYTE", BLOB_TYPE_ARRAY_BYTE)
      .value("BLOB_TYPE_EOR",        BLOB_TYPE_EOR       )
      .value("BLOB_TYPE_NONE",       BLOB_TYPE_NONE      )
      .value("BLOB_BIT_NULL",        BLOB_BIT_NULL       )
      .export_values()
      ;

   /***
    * enum frontier::encoding_t
    ***/
   enum_<frontier::encoding_t>("encoding_t")
      .value("BLOB", frontier::BLOB)
      .export_values()
      ;

   /***
    * class frontier::AnyData
    ***/
   class_anydata = class_<frontier::AnyData>("AnyData")
      .def("type",       &frontier::AnyData::type,        "type of data field")
      .def("getRawI8",   &frontier::AnyData::getRawI8,    "type long long")
      .def("getRawD",    &frontier::AnyData::getRawD,     "type double")
      .def("getRawStrP", &frontier::AnyData::getRawStrP,  "type string")
      .def("getRawStrS", &frontier::AnyData::getRawStrS,  "type unsigned int")
      .def("getRawI4",   &frontier::AnyData::getRawI4,    "type int")
      .def("getRawF",    &frontier::AnyData::getRawF,     "type float")
      .def("getRawB",    &frontier::AnyData::getRawB,     "type char")
      .def("getInt",     &frontier::AnyData::getInt,      "cast to type int")
      .def("getLongLong",&frontier::AnyData::getLongLong, "cast to type long long")
      .def("getFloat",   &frontier::AnyData::getFloat,    "cast to type float")
      .def("getDouble",  &frontier::AnyData::getDouble,   "cast to type double")
      .def("getString",  &getString,                      "cast to type string")
      .def("clean",      &frontier::AnyData::clean,       "clean data")
      ;

   /***
    * class frontier::Request
    ***/
   object class_req = class_<frontier::Request>("Request", init<const std::string&, frontier::encoding_t>() )
      .def("addKey", &frontier::Request::addKey, "", args("self", "key", "value"))
      .def("encodeParam", &frontier::Request::encodeParam, "", args("value"))
      .def("setRetrieveZipLevel", &frontier::Request::setRetrieveZipLevel,
           "Set the zip level of retrieved data\nlevel 0 is off, level 1 is fast, level 5 is normal, level 9 is best\ndefault is 5",
           args("level"))
      .staticmethod("encodeParam")
      .staticmethod("setRetrieveZipLevel")
      ;



}
