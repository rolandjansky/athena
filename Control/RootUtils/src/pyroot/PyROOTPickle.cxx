/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RootUtils/src/pyroot/PyROOTPickle.cxx
 * @author Wim Lavrijsen
 * @date Apr 2008
 * @brief Port pickling functionality while awaiting newer release.
 */

// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#include "RootUtils/PyROOTPickle.h"
#include "Python.h"
#include "TClass.h"
#include "TBufferFile.h"
#include "TPython.h"


//- data _______________________________________________________________________
static PyObject* gExpand = 0;


namespace RootUtils {

/**
 * @brief PyROOT object proxy pickle support
 * @param self object proxy instance to be pickled
 */
PyObject* ObjectProxyReduce( PyObject* self )
{
  // Turn the object proxy instance into a character stream and return for
  // pickle, together with the callable object that can restore the stream
  // into the object proxy instance.

  void* vself = TPython::ObjectProxy_AsVoidPtr( self );    // checks type
  if ( ! vself ) {
     PyErr_SetString( PyExc_TypeError,
       "__reduce__ requires an object proxy instance as first argument" );
     return 0;
  }

  PyObject* nattr = PyObject_GetAttrString( (PyObject*)self->ob_type, (char*)"__name__" );
  PyObject* pyname = PyObject_Str( nattr );
  Py_DECREF( nattr );

  TClass* klass = TClass::GetClass( PyString_AS_STRING( pyname ) );

  // no cast is needed, but WriteObject taking a TClass argument is protected,
  // so use WriteObjectAny()
  TBufferFile buf( TBuffer::kWrite );
  if ( buf.WriteObjectAny( vself, klass ) != 1 ) {
     PyErr_Format( PyExc_IOError,
        "could not stream object of type %s", PyString_AS_STRING( pyname ) );
     Py_DECREF( pyname );
     return 0;
  }

  // use a string for the serialized result, as a python buffer will not copy
  // the buffer contents; use a string for the class name, used when casting
  // on reading back in
  PyObject* res2 = PyTuple_New( 2 );
  PyTuple_SET_ITEM( res2, 0, PyString_FromStringAndSize( buf.Buffer(), buf.Length() ) );
  PyTuple_SET_ITEM( res2, 1, pyname );

  PyObject* result = PyTuple_New( 2 );
  Py_INCREF( gExpand );
  PyTuple_SET_ITEM( result, 0, gExpand );
  PyTuple_SET_ITEM( result, 1, res2 );

  return result;
}


/**
 * @brief Helper for (un)pickling of ObjectProxy's
 * @param args The Python arguments.
 */
PyObject* ObjectProxyExpand( PyObject*, PyObject* args )
{
  // This method is a helper for (un)pickling of ObjectProxy instances.
  PyObject* pybuf = 0;
  const char* clname = 0;
  if ( ! PyArg_ParseTuple( args, const_cast< char* >( "O!s:__expand__" ),
           &PyString_Type, &pybuf, &clname ) )
    return 0;

  // use the PyString macros to by-pass error checking; do not adopt the buffer,
  // as the local TBufferFile can go out of scope (there is no copying)
  TBufferFile buf( TBuffer::kRead,
     PyString_GET_SIZE( pybuf ), PyString_AS_STRING( pybuf ), kFALSE );

  void* result = buf.ReadObjectAny( 0 );
  return TPython::ObjectProxy_FromVoidPtr( result, clname );
}


/**
 * @brief Install the pickling of ObjectProxy's functionality.
 * @param libpyroot_pymodule The libPyROOT python module
 * @param objectproxy_pytype The ObjectProxy python type
 */
void PyROOTPickle::Initialize( PyObject* libpyroot_pymodule, PyObject* objectproxy_pytype )
{
  Py_INCREF( libpyroot_pymodule );
  PyTypeObject* pytype = (PyTypeObject*)objectproxy_pytype;

  static PyMethodDef s_pdefExp = { (char*)"_ObjectProxy__expand__",
            (PyCFunction)ObjectProxyExpand, METH_VARARGS, (char*)"internal function" };

  PyObject* pymname = PyString_FromString( PyModule_GetName( libpyroot_pymodule ) );
  gExpand = PyCFunction_NewEx( &s_pdefExp, NULL, pymname );
  Py_DECREF( pymname );
  Bool_t isOk = PyObject_SetAttrString( libpyroot_pymodule, s_pdefExp.ml_name, gExpand ) == 0;
  Py_DECREF( gExpand );      // is moderately risky, but Weakref not allowed (?)

  if ( ! isOk ) {
    Py_DECREF( libpyroot_pymodule );
    PyErr_SetString( PyExc_TypeError, "could not add expand function to libPyROOT" );
    return;
  }

  static PyMethodDef s_pdefRed = { (char*)"__reduce__",
            (PyCFunction)ObjectProxyReduce, METH_NOARGS, (char*)"internal function" };

  PyObject* descr = PyDescr_NewMethod( pytype, &s_pdefRed );
  isOk = PyDict_SetItemString( pytype->tp_dict, s_pdefRed.ml_name, descr) == 0;
  Py_DECREF( descr );
  if ( ! isOk ) {
    Py_DECREF( libpyroot_pymodule );
    PyErr_SetString( PyExc_TypeError, "could not add __reduce__ function to ObjectProxy" );
    return;
  }

  Py_DECREF( libpyroot_pymodule );
}


} // namespace RootUtils
