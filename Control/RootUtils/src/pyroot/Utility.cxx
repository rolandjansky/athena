// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/src/pyroot/Utility.cxx
 * @author scott snyder, Wim Lavrijsen
 * @date Jul, 2015
 * @brief Utility code originally from pyroot.
 */


// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#include "Utility.h"
#include "TInterpreter.h"
#include <sstream>


namespace RootUtils {


int GetBuffer( PyObject* pyobject, char tc, int size, void*& buf, Bool_t check )
{
// Retrieve a linear buffer pointer from the given pyobject.

// special case: don't handle character strings here (yes, they're buffers, but not quite)
   if ( PyBytes_Check( pyobject ) )
      return 0;

// attempt to retrieve pointer to buffer interface
   PyBufferProcs* bufprocs = Py_TYPE(pyobject)->tp_as_buffer;

   PySequenceMethods* seqmeths = Py_TYPE(pyobject)->tp_as_sequence;
   if ( seqmeths != 0 && bufprocs != 0
#if  PY_VERSION_HEX < 0x03000000
        && bufprocs->bf_getwritebuffer != 0
        && (*(bufprocs->bf_getsegcount))( pyobject, 0 ) == 1
#else
        && bufprocs->bf_getbuffer != 0
#endif
      ) {

   // get the buffer
#if PY_VERSION_HEX < 0x03000000
      Py_ssize_t buflen = (*(bufprocs->bf_getwritebuffer))( pyobject, 0, &buf );
#else
      Py_buffer bufinfo;
      (*(bufprocs->bf_getbuffer))( pyobject, &bufinfo, PyBUF_WRITABLE );
      buf = (char*)bufinfo.buf;
      Py_ssize_t buflen = bufinfo.len;
#endif

      if ( buf && check == kTRUE ) {
      // determine buffer compatibility (use "buf" as a status flag)
         PyObject* pytc = PyObject_GetAttrString( pyobject, "typecode");
         if ( pytc != 0 ) {     // for array objects
            if ( PyROOT_PyUnicode_AsString( pytc )[0] != tc )
               buf = 0;         // no match
            Py_DECREF( pytc );
         } else if ( seqmeths->sq_length &&
                     (int)(buflen / (*(seqmeths->sq_length))( pyobject )) == size ) {
         // this is a gamble ... may or may not be ok, but that's for the user
            PyErr_Clear();
         } else if ( buflen == size ) {
         // also a gamble, but at least 1 item will fit into the buffer, so very likely ok ...
            PyErr_Clear();
         } else {
            buf = 0;                      // not compatible

         // clarify error message
            PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
            PyErr_Fetch( &pytype, &pyvalue, &pytrace );
            PyObject* pyvalue2 = PyROOT_PyUnicode_FromFormat(
               (char*)"%s and given element size (%ld) do not match needed (%d)",
               PyROOT_PyUnicode_AsString( pyvalue ),
               seqmeths->sq_length ? (Long_t)(buflen / (*(seqmeths->sq_length))( pyobject )) : (Long_t)buflen,
               size );
            Py_DECREF( pyvalue );
            PyErr_Restore( pytype, pyvalue2, pytrace );
         }
      }

      return buflen;
   }

   return 0;
}


//- public functions ---------------------------------------------------------
PyObject* getRootModule()
{
  return PyImport_ImportModule ("ROOT");
}


PyObject* rootModule()
{
  static PyObject* const rootModule = getRootModule();
  Py_INCREF (rootModule);
  return rootModule;
}


TClass* objectIsA (PyObject* obj)
{
  PyObject* repr = PyObject_Repr (obj);
  if (!repr) return nullptr;
  const char* s = PyROOT_PyUnicode_AsString (repr);
  if (*s == '<') ++s;
  if (strncmp (s, "ROOT.", 5) == 0)
    s += 5;
  const char* p = strstr (s, " object ");
  if (!p) return nullptr;
  std::string name (s, p-s);
  TClass* cls = TClass::GetClass (name.c_str());
  Py_DECREF (repr);
  return cls;
}


PyObject* getSetOwnershipFunc()
{
  PyObject* root = rootModule();
  PyObject* ret = PyObject_GetAttrString (root, "SetOwnership");
  Py_DECREF (root);
  return ret;
}

PyObject* setOwnershipFunc()
{
  static PyObject* const func = getSetOwnershipFunc();
  Py_INCREF (func);
  return func;
}

bool setOwnership (PyObject* obj, bool flag)
{
  PyObject* func = setOwnershipFunc();
  PyObject* ret = PyObject_CallFunction (func, const_cast<char*>("OI"),
                                         obj,
                                         static_cast<unsigned int>(flag));
  Py_DECREF (func);
  if (ret) {
    Py_DECREF (ret);
    return true;
  }
  return false;
}


} // namespace RootUtils
