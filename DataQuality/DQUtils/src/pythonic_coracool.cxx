/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Efficient pythonic CoraCool bindings
// Author: <peter.waller@cern.ch> Jan 2010

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#include <Python.h>

#include <CoolKernel/ChannelSelection.h>
#include <CoolKernel/IFolder.h>
#include <CoolKernel/IDatabase.h>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include <CoraCool/CoraCoolDatabaseSvcFactory.h>
#include <CoraCool/CoraCoolDatabaseSvc.h>
#include <CoraCool/CoraCoolDatabase.h>
#include <CoraCool/CoraCoolFolder.h>
#include <CoraCool/CoraCoolObjectIter.h>
#include <CoraCool/CoraCoolObject.h>

#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;
using std::vector;

using boost::bind;
using boost::placeholders::_1;
using cool::IFolderPtr;
using cool::IDatabasePtr;
using cool::ChannelSelection;
using cool::ValidityKey;
using coral::Attribute;
using coral::AttributeSpecification;
using coral::AttributeList;

typedef boost::function<PyObject* (const AttributeList&)> 
    coral_attribute_fetcher_t;

// A function to signal that a conversion object could not be found
PyObject *no_coral_conversion_available(const AttributeList&) {return NULL;}

// Re-use these two functions from quick_retrieve.cxx
PyObject *qr_PyString_FromBlob(const coral::Blob& blob);
PyObject *qr_PyString_FromStdString(const string& str);

template<typename T>
const T& fetch_attribute_data(const coral::Attribute& A)
{
    return A.data<T>();
}

// Returns a functor which returns a Python object for the attribute with
// a given name and type. Test type_name against a series of fetchers.
// If none are found, no_coral_conversion_available is returned.
coral_attribute_fetcher_t create_attribute_fetcher(const char*   name, 
                                                   const string& type_name)
{
    // Force the type of fetch_attribute, bind cannot infer it unfortunately.
    const Attribute& (AttributeList::*fetch_attribute)(const string) const = 
        &AttributeList::operator[];
    
    // below, _1 falls through to leave you with a function that "looks like"
    // [converter(attr[name].data<type>())] (Attribute& attr)
    
    // Test type against type_name. If true, return a functor for this type.
    #define MAKE_FETCHER(type, converter)                    \
        if (type_name == #type)                              \
            return bind(converter,                           \
                        bind(fetch_attribute_data<type>,     \
                             bind(fetch_attribute, _1, name) \
                        )                                    \
                    );
    
    // See the python c-api reference for python conversion functions
    // Python/C API Reference Manual >> Concrete Objects Layer
    // http://docs.python.org/c-api/concrete.html
    
    MAKE_FETCHER(short,     PyLong_FromLong)
    MAKE_FETCHER(int,       PyLong_FromLong)
    MAKE_FETCHER(long long, PyLong_FromLongLong)
    
    /*
    
    Types from COOL:
        We need to build up a similar list from CORAL, it would be nice to know
        where they came from.
    
    MAKE_FETCHER(Bool,      PyBool_FromLong)
    MAKE_FETCHER(Float,     PyFloat_FromDouble)
    MAKE_FETCHER(Double,    PyFloat_FromDouble)
    MAKE_FETCHER(UChar,     PyLong_FromLong)
    MAKE_FETCHER(UInt16,    PyLong_FromUnsignedLong)
    MAKE_FETCHER(UInt32,    PyLong_FromUnsignedLong)
    MAKE_FETCHER(UInt63,    PyLong_FromUnsignedLongLong)
    MAKE_FETCHER(UInt64,    PyLong_FromUnsignedLongLong)
    MAKE_FETCHER(String255, qr_PyString_FromStdString)
    MAKE_FETCHER(String4k,  qr_PyString_FromStdString)
    MAKE_FETCHER(String64k, qr_PyString_FromStdString)
    MAKE_FETCHER(String16M, qr_PyString_FromStdString)
    MAKE_FETCHER(Blob16M,   qr_PyString_FromBlob)
    MAKE_FETCHER(Blob64k,   qr_PyString_FromBlob)
    */

    PyErr_Format(PyExc_RuntimeError, 
                 "Type '%s' is not in type conversion table. "
                 "Please add it to pythonic_coracool.cxx. "
                 "Can't convert field '%s'.",
                 type_name.c_str(),
                 name);
    return no_coral_conversion_available;
}

// From a list of python strings (to_fetch) (which are attribute names)
// Create a fetcher for each attribute, and insert them into the 
// payload_fetchers vector
bool make_fetchers(
    PyObject *to_fetch,
    const AttributeList& attribute_list,
    vector<coral_attribute_fetcher_t>& payload_fetchers)
{
    const Py_ssize_t count = to_fetch ? PySequence_Size(to_fetch) : 0;

    for (Py_ssize_t i = 0; i < count; i++)
    {
        PyObject *py_name = PySequence_GetItem(to_fetch, i);
#if PY_VERSION_HEX < 0x03000000
        const char *name = PyString_AsString(py_name);
#else
        const char *name = _PyUnicode_AsString(py_name);
#endif
        const string type = attribute_list[name].specification().typeName();
                       
        coral_attribute_fetcher_t pf = create_attribute_fetcher(name, type);
        
        if (pf == no_coral_conversion_available)
            return false; // Failure: A python exception was thrown above
            
        payload_fetchers.push_back(pf);
        Py_DECREF(py_name);
    }
    
    return true; // Success
}

inline PyObject *apply_function(PyObject *function, PyObject *object)
{
    // Convert object according to function, taking care of references
    // If function is null, return unmodified object
    if (!function || function == Py_None) return object;
    PyObject *old_object = object;
    PyObject *new_object = PyObject_CallObject(function, object);
    Py_DECREF(old_object);
    return new_object;
}

CoraCoolFolderPtr fetch_coracool_folder(IDatabasePtr cooldb, string folder)
{
    CoraCoolDatabaseSvc&  corasvc     = CoraCoolDatabaseSvcFactory::
                                        databaseService();
                                        
    CoraCoolDatabasePtr   coradb      = corasvc.openDatabase(
                                            cooldb->databaseId(), cooldb, true);
                                            
    return coradb->getFolder(folder.c_str());
}

const cool::RecordSpecification 
    get_coracool_payload_spec(IDatabasePtr cooldb, string folder)
{
    return fetch_coracool_folder(cooldb, folder)->payloadSpecification();
}

inline PyObject* make_iov_key(PyObject *iovkey_wrapper, 
                              unsigned long long value)
{
    static char *argtypes = const_cast<char *>("K");
    if (iovkey_wrapper && iovkey_wrapper != Py_None)
        return PyObject_CallFunction(iovkey_wrapper, argtypes, value);
    return PyLong_FromUnsignedLongLong(value);
}

PyObject *browse_coracool(IDatabasePtr cooldb, string folder, 
                          ValidityKey since, ValidityKey until,
                          const ChannelSelection &cs = ChannelSelection::all(), 
                          const char *tag="",
                          PyObject *to_fetch = NULL,
                          PyObject *object_converter = NULL,
                          PyObject *inner_object_converter = NULL,
                          PyObject *iovkey_wrapper = NULL)
{
    // Browse CoraCool objects
    CoraCoolFolderPtr     coralFolder = fetch_coracool_folder(cooldb, folder);
    CoraCoolObjectIterPtr objects     = coralFolder->browseObjects(since, until, 
                                                                   cs, tag);
    
    // Number of attributes to be fetched
    const Py_ssize_t count = to_fetch ? PySequence_Size(to_fetch) : 0;
    
    PyObject* result = PyList_New(0); // List which is returned by this function
    bool first = true;                // Is this the first iteration?
    CoraCoolObject::const_iterator payload; // Current payload
    
    // List of functors which convert an Attribute into a python value
    vector<coral_attribute_fetcher_t> payload_fetchers; 
    
    // Loop over IoVs
    while (objects->hasNext())
    {
        const CoraCoolObjectPtr& object = objects->next();
        
        PyObject *py_payload_tuple = PyTuple_New(object->size());
        unsigned int payload_index = 0;
        
        // Loop over multiple payloads per record
        for (payload = object->begin(); 
             payload != object->end();
             ++payload, ++payload_index)
        {
            if (unlikely(first))
            {
                // On the first iteration, figure out the types and create
                // specialised functions for retrieving them
                first = false;
                if (!make_fetchers(to_fetch, *payload, payload_fetchers))
                    return NULL; // Failure, throw python exception from above
            }
        
            // Create a tuple for the payload and fill it from payload fetchers
            PyObject *py_payload = PyTuple_New(count);
            for (Py_ssize_t i = 0; i < count; i++)
                PyTuple_SET_ITEM(py_payload, i, payload_fetchers[i](*payload));
                
            py_payload = apply_function(inner_object_converter, py_payload);
            
            if (!py_payload)
            {
                // apply_function returned an error
                // We won't return the list to python so we need to tell python it
                // can be deleted
                Py_DECREF(py_payload_tuple);
                Py_DECREF(result);
                return NULL;
            }
                
            PyTuple_SET_ITEM(py_payload_tuple, payload_index, py_payload);
        }
        
        PyObject *one = PyTuple_New(4);
        
        PyTuple_SET_ITEM(one, 0, make_iov_key(iovkey_wrapper, object->since()));
        PyTuple_SET_ITEM(one, 1, make_iov_key(iovkey_wrapper, object->until()));
        
        PyTuple_SET_ITEM(one, 2, PyLong_FromLong(object->channelId()));
        
        // SET_ITEM steals reference, no decref needed
        PyTuple_SET_ITEM(one, 3, py_payload_tuple);
        
        one = apply_function(object_converter, one);
        if (!one)
        {
            // apply_function returned an error
            // We won't return the list to python so we need to tell python it
            // can be deleted
            Py_DECREF(result);
            return NULL;
        }
        
        PyList_Append(result, one);
        Py_DECREF(one);
    }
    
    return result;
}
