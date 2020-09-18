/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#include <Python.h>

#include <CoolKernel/ChannelSelection.h>
#include <CoolKernel/ITime.h>
#include <CoolKernel/IField.h>
#include <CoolKernel/FieldSelection.h>
#include <CoolKernel/CompositeSelection.h>
#include <CoolKernel/IObject.h>
#include <CoolKernel/IObjectIterator.h>
#include <CoolKernel/IFolder.h>
#include <CoolKernel/IDatabase.h>
#include <CoolKernel/IDatabaseSvc.h>

#include <CoolApplication/DatabaseSvcFactory.h>

#include <boost/typeof/typeof.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <vector>
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;
using std::vector;

using boost::bind;
using boost::placeholders::_1;

using cool::DatabaseSvcFactory;
using cool::IDatabasePtr;
using cool::IField;
using cool::IObject;
using cool::IFolder;
using cool::IFolderPtr;
using cool::ChannelSelection;
using cool::IObjectIteratorPtr;
using cool::IObjectVectorPtr;
using cool::ValidityKey;
using cool::ITime;

cool::IRecordSelection* make_fieldselection(
    const std::string& name, 
    const cool::StorageType::TypeId typeId, 
    cool::FieldSelection::Relation relation, 
    PyObject* refValue)
{    
    #define MAKE_FS(type, converter) \
        if (typeId == cool::StorageType::type) \
            return new cool::FieldSelection(name, typeId, relation, \
                static_cast<cool::type>(converter(refValue)));
    
    MAKE_FS(Bool,      PyLong_AsLong)
    MAKE_FS(Float,     PyFloat_AsDouble)
    MAKE_FS(Double,    PyFloat_AsDouble)
    
    MAKE_FS(UChar,     PyLong_AsUnsignedLong)
    MAKE_FS(Int16,     PyLong_AsLong)
    MAKE_FS(UInt16,    PyLong_AsUnsignedLong)
    MAKE_FS(Int32,     PyLong_AsLong)
    MAKE_FS(UInt32,    PyLong_AsUnsignedLong)
    MAKE_FS(Int64,     PyLong_AsLongLong)
    MAKE_FS(UInt63,    PyLong_AsUnsignedLongLong)
    
#if PY_VERSION_HEX < 0x03000000
    MAKE_FS(String255, PyString_AsString)
    MAKE_FS(String4k,  PyString_AsString)
    MAKE_FS(String64k, PyString_AsString)
    MAKE_FS(String16M, PyString_AsString)
#else
    MAKE_FS(String255, _PyUnicode_AsString)
    MAKE_FS(String4k,  _PyUnicode_AsString)
    MAKE_FS(String64k, _PyUnicode_AsString)
    MAKE_FS(String16M, _PyUnicode_AsString)
#endif
        
    //MAKE_FS(Blob16M,   PyString_AsString)
    //MAKE_FS(Blob64k,   PyString_AsString)
    throw;
}

vector<const cool::IRecordSelection*> make_selection_vector()
{
    return vector<const cool::IRecordSelection*>();

}

// A function taking an IObject and returning a PyObject*
typedef boost::function<PyObject* (const IObject&)> payload_fetcher_t;

// A function to signal that a conversion object could not be found
PyObject *no_conversion_available(const IObject&) {return NULL;}

// Helpers to create python strings from Blob and std::string
PyObject *qr_PyString_FromBlob(const coral::Blob& blob)
{
    const char* data = reinterpret_cast<const char*>(blob.startingAddress());
#if PY_VERSION_HEX < 0x03000000
    return PyString_FromStringAndSize(data, blob.size());
#else
    return PyUnicode_FromStringAndSize(data, blob.size());
#endif
}

PyObject *qr_PyString_FromStdString(const string& str)
{
#if PY_VERSION_HEX < 0x03000000
    return PyString_FromStringAndSize(str.c_str(), str.size());
#else
    return PyUnicode_FromStringAndSize(str.c_str(), str.size());
#endif
}

PyObject *qr_PyUnicode_FromStdString(const string& str)
{
    return PyUnicode_FromStringAndSize(str.c_str(), str.size());
}

// payload_fetcher takes an IObject `o` to be retrieved from, a `name` to 
// retrieve and a converter_function which can take a COOL type and create a 
// python object for it.
template<typename T, typename FUNC_TYPE>
inline PyObject* payload_fetcher(const IObject& o, const string& name, 
                                 FUNC_TYPE* converter_function)
{
    // We have to call this, because if one calls payloadValue on a null record,
    // it throws an exception.
    const IField& field = o.payload()[name];
    if (field.isNull())
        Py_RETURN_NONE;
    return converter_function(field.data<T>());
}

// create_payload_fetcher is called for each field being retrieved from COOL,
// once for a given browseObjects() on the first object.
payload_fetcher_t create_payload_fetcher(const char* name, 
                                         const string& type_name, 
                                         bool string_to_unicode = false)
{
    // A macro which takes a cool `type` and a python api function which returns
    // a PyObject*, `converter`.
    #define MAKE_FETCHER(type, converter)                                      \
        if (type_name == #type)                                                \
            return bind(payload_fetcher<cool::type, BOOST_TYPEOF(converter)>,  \
                        _1, name, &converter);
    
    // See the python c-api reference for python conversion functions
    // Python/C API Reference Manual >> Concrete Objects Layer
    // http://docs.python.org/c-api/concrete.html
    
    MAKE_FETCHER(Bool,      PyBool_FromLong)
    MAKE_FETCHER(Float,     PyFloat_FromDouble)
    MAKE_FETCHER(Double,    PyFloat_FromDouble)
    MAKE_FETCHER(UChar,     PyLong_FromLong)
    MAKE_FETCHER(Int16,     PyLong_FromLong)
    MAKE_FETCHER(UInt16,    PyLong_FromUnsignedLong)
    MAKE_FETCHER(Int32,     PyLong_FromLong)
    MAKE_FETCHER(UInt32,    PyLong_FromUnsignedLong)
    MAKE_FETCHER(Int64,     PyLong_FromLong)
    MAKE_FETCHER(UInt63,    PyLong_FromUnsignedLongLong)
    MAKE_FETCHER(UInt64,    PyLong_FromUnsignedLongLong)
    if (string_to_unicode)
    {
        MAKE_FETCHER(String255, qr_PyUnicode_FromStdString)
        MAKE_FETCHER(String4k,  qr_PyUnicode_FromStdString)
        MAKE_FETCHER(String64k, qr_PyUnicode_FromStdString)
        MAKE_FETCHER(String16M, qr_PyUnicode_FromStdString)
    } else
    {
        MAKE_FETCHER(String255, qr_PyString_FromStdString)
        MAKE_FETCHER(String4k,  qr_PyString_FromStdString)
        MAKE_FETCHER(String64k, qr_PyString_FromStdString)
        MAKE_FETCHER(String16M, qr_PyString_FromStdString)
    }
    MAKE_FETCHER(Blob16M,   qr_PyString_FromBlob)
    MAKE_FETCHER(Blob64k,   qr_PyString_FromBlob)

    PyErr_Format(PyExc_RuntimeError, 
                 "Type '%s' is not in type conversion table. "
                 "Please add it to quick_retrieve.cxx. "
                 "Can't convert field '%s'.",
                 type_name.c_str(),
                 name);
    return no_conversion_available;
}

// Scope guard IObjectIterator so that resource is freed even on exception
class IObjectIterator_Guard
{
    const IObjectIteratorPtr& m_iterator;
public:
    IObjectIterator_Guard(const IObjectIteratorPtr& iterator) 
        : m_iterator(iterator) {}
    ~IObjectIterator_Guard() { m_iterator->close(); }
};

inline PyObject* make_iov_key(PyObject *iovkey_wrapper, 
                              unsigned long long value)
{
    static char *argtypes = const_cast<char *>("K");
    if (iovkey_wrapper && iovkey_wrapper != Py_None)
        return PyObject_CallFunction(iovkey_wrapper, argtypes, value);
    return PyLong_FromUnsignedLongLong(value);
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

// Quickly retrieve IObjects from an IObjectIterator
// to_fetch is a list of strings specifying which variables to look at
// This function closes the iterator when it completes.
// The return result is a list of tuples
// [(since, until, channel, ...) ] where ... are variables specified by to_fetch
PyObject* quick_retrieve(const IObjectIteratorPtr& objects, 
                         PyObject *object_converter,
                         PyObject *to_fetch = NULL,
                         const long max_records = -1,
                         const bool with_channel = true,
                         const bool loud = false,
                         PyObject *iovkey_wrapper = NULL,
                         PyObject *channel_name_mapping = NULL,
                         const bool with_time = false,
                         const bool as_unicode = false)
{
    IObjectIterator_Guard closeiterator_guard(objects);
    
    PyObject* result = PyList_New(0);
    
    const long long records = objects->size();
    const Py_ssize_t count = to_fetch ? PySequence_Size(to_fetch) : 0;
    const unsigned int fetch_start = 2 + with_channel + with_time;
    bool first = true;
    int j = 0;
    PyObject *py_datetime_class = NULL;
    
    vector<payload_fetcher_t> payload_fetchers;
    
    while (objects->goToNext())
    {
        const IObject& object = objects->currentRef();
        
        if (unlikely(first))
        {
            // On the first iteration, compute the types in to_fetch, and bind
            // them into the payload_fetchers.
            first = false;
            for (Py_ssize_t i = 0; i < count; i++)
            {
                PyObject *py_name = PySequence_GetItem(to_fetch, i);
#if PY_VERSION_HEX < 0x03000000
                const char *name = PyString_AsString(py_name);
#else
                const char *name = _PyUnicode_AsString(py_name);
#endif
                const string type = (object.payload()
                                           .specification()[name]
                                           .storageType()
                                           .name());
                
                payload_fetcher_t pf = create_payload_fetcher(name, type, 
                                                              as_unicode);
                if (pf == no_conversion_available)
                    return NULL;
                payload_fetchers.push_back(pf);
                Py_DECREF(py_name);
            }
            
            if (with_time)
            {
                PyObject *py_datetime_module = PyImport_ImportModule("datetime");
                if (!py_datetime_module)
                    throw;
                
                py_datetime_class = PyObject_GetAttrString(py_datetime_module,
                                                           "datetime"); 
                
                Py_DECREF(py_datetime_module);
            }
        }
        
        PyObject *one = PyTuple_New(fetch_start + count);
        
        // Fetch (since, until, channel[optional])
        
        PyTuple_SET_ITEM(one, 0, make_iov_key(iovkey_wrapper, object.since()));
        PyTuple_SET_ITEM(one, 1, make_iov_key(iovkey_wrapper, object.until()));

        if (with_time)
        {
            const ITime& t = object.insertionTime();
            static char *argtypes = const_cast<char *>("iiiiiil");
            
            PyObject *py_record_time = 
                PyObject_CallFunction(py_datetime_class, argtypes, 
                    t.year(), t.month(), t.day(), 
                    t.hour(), t.minute(), t.second(), t.nanosecond() / 1000);
                    
            PyTuple_SetItem(one, 2, py_record_time);
        }
        
        if (with_channel)
        {
            if (channel_name_mapping)
            {
                // Use a channel name if it is available, otherwise fall back
                // to standard channelid.
                PyObject *channelId = PyLong_FromUnsignedLong(object.channelId());
                PyObject *channelName = PyDict_GetItem(channel_name_mapping,    
                                                       channelId);
                if (channelName)
                {
                    Py_INCREF(channelName);
                    Py_DECREF(channelId);
                    PyTuple_SET_ITEM(one, 2+with_time, channelName);
                } else
                    PyTuple_SET_ITEM(one, 2+with_time, channelId);
                
            } else
                PyTuple_SET_ITEM(one, 2+with_time, 
                                 PyLong_FromLong(object.channelId()));
        }
        
        // Fetch the objects specified in to_fetch
        for (Py_ssize_t i = 0; i < count; i++)
            PyTuple_SET_ITEM(one, fetch_start + i, payload_fetchers[i](object));
        
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
        
        // Print status if loud is active
        if (unlikely(loud && ((++j) % 1000 == 0)))
        {
            cout << "Progress: " << j << " / " << records << "\r";
            cout.flush();
        }
        
        if (unlikely(max_records > 0 && j >= max_records))
            break;
    }
    
    if (loud)
        cout << "Done fetching " << j << " records.\x1B[K" << endl;
    
    if (py_datetime_class)
    {
        Py_DECREF(py_datetime_class);
    }
    
    return result;
}

