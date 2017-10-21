/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyROOTTypePatch.cxx,v 1.5 2008-10-06 15:03:03 ssnyder Exp $
/**
 * @file RootUtils/src/pyroot/PyROOTTTypePatch.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief setattr hook for PyROOT classes to apply container fixes.
 */

// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "RootUtils/PyROOTTypePatch.h"
#include "Python.h"
#include "TClass.h"
#include "TMethod.h"
#include "TROOT.h"
#include "RVersion.h"
#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <string.h>


namespace RootUtils {


/// Method to make a generic python iterator.
/// See comments below.
PyObject* generic_iter (PyObject*, PyObject* args)
{
  if (PyTuple_GET_SIZE(args) < 1)
    return 0;
  return PySeqIter_New (PyTuple_GET_ITEM (args, 0));
}


PyMethodDef generic_iter_meth = {
  "__iter__",
  (PyCFunction)generic_iter,
  METH_VARARGS,
  0
};


/**
 * @brief Patched setattr function.
 * @param obj Object being set.
 * @param name Attribute name being set.
 * @param val Value being set.
 * @return -1 on error.
 */
int setattro_hack (PyObject* obj, PyObject* name, PyObject* val)
{
  // Trigger special handling if we set _getitem__unchecked in a class.
  if (PyString_Check (name) && PyType_Check(obj)) {
    const char* sname = PyString_AS_STRING (name);
    PyTypeObject* typ = (PyTypeObject*)obj;

    if (strcmp (sname, "_getitem__unchecked") == 0) {
      // Ok, we're doing special handling.

      // There's another issue that comes about with DV derivation.
      // For a container type C, if it has dictionaries for its
      // iterators, PyROOT will define an __iter__ method for it.
      // Otherwise, it will define __getitem__ (and _getitem__unchecked);
      // the generic python iterator code will be used in that case.
      // The problem happens when a base DV class has dictionaries
      // for its iterators but a derived DV class does not.  In that
      // case, the base class will get an __iter__ and the derived
      // class will not.  But due to the inheritance, the derived
      // class will use the base class's __iter__.  This will then
      // end up trying to use the derived class's iterators.
      // Since we don't have a dictionary for these, this won't
      // work correctly.  The symptom is that trying to construct
      // an iterator for an empty container throws a StopIteration
      // exception.
      //
      // As a workaround, if PyROOT doesn't define __iter__ (which we
      // know is so in this case, since _getitem__unchecked is being
      // defined), then we also define an __iter__ which ends up
      // doing the generic python thing (i.e., operates in terms
      // of __getitem__).
      if (PyObject_HasAttrString (obj, "__iter__")) {
        PyObject* func = PyCFunction_New (&generic_iter_meth, 0);
        PyObject* method = PyMethod_New (func, NULL, obj);
        bool isOK = PyObject_SetAttrString (obj, "__iter__", method);
        if (PyErr_Occurred())
          fprintf (stderr, "Py error");
        else if (!isOK)
          fprintf (stderr, "Could not add method __iter__\n");
        Py_DECREF( method );
        Py_DECREF( func );
      }

      // Apply fix for getitem in both base and derived classes.
      // If _getitem__unchecked already exists in a base, don't
      // set it here.
      PyObject* bases = typ->tp_bases;
      if (PyTuple_Check (bases)) {
        Py_XINCREF (bases);
        for (int i = 0; i < PyTuple_GET_SIZE(bases); ++i) {
          PyObject* base = PyTuple_GET_ITEM (bases, i);
          if (PyObject_HasAttr (base, name)) {
            Py_XDECREF (bases);
            return 0;
          }
        }
        Py_XDECREF (bases);
      }
    }
  }

  // Do the generic thing.
  return PyType_Type.tp_setattro (obj, name, val);
}


/// Saved value of original tp_alloc for PyRootType.
allocfunc old_alloc = 0;


/// Saved value of original tp_new for metaclasses.
newfunc old_new = 0;


/**
 * @brief Patched new function.
 * @param subtype metatype of class.
 * @param args Call arguments.
 * @param kw Call keywords.
 *
 * This should get called when we try to create a new pyroot type.
 * We call the python function PyROOTFixes._pyroot_class_hook
 * with the new type.
 */
PyObject* new_hack (PyTypeObject* subtype, PyObject* args, PyObject* kw)
{
  // Do the old thing.
  PyObject* ret = old_new (subtype, args, kw);

  // Call the hook function.
  PyObject* mod = PyImport_ImportModule ("RootUtils.PyROOTFixes");
  if (mod) {
    PyObject* hook = PyObject_GetAttrString (mod, "_pyroot_class_hook");
    if (hook) {
      PyObject* tmp = PyObject_CallFunctionObjArgs (hook, ret, NULL);
      Py_XDECREF (tmp);
      Py_DECREF (hook);
    }
    Py_DECREF (mod);
  }

  return ret;
}


/**
 * @brief Patched alloc function (see below).
 * @param type Type of the object being allocated.
 * @param n Size of variable part.
 */
PyObject* alloc_hack (PyTypeObject* type, Py_ssize_t n)
{
  // Allocate the object.
  PyObject* ret = old_alloc (type, n);

  // We expect that this will be used to make metaclasses for PyROOT classes.
  // So, if we actually make a type, patch its setattr slot.
  if (PyType_Check (ret)) {
    PyTypeObject* typ = (PyTypeObject*)ret;
    typ->tp_setattro = setattro_hack;

    // We also set up to call a hook in python whenever a new pyroot
    // class is created.
    if (old_new == 0)
      old_new = type->tp_new;
    else
      assert (old_new == type->tp_new);
    typ->tp_new = new_hack;
  }
  return ret;
}


/**
 * @brief Install the fixup.
 * @param pyroottype The Python PyRootType object.
 */
void PyROOTTypePatch::initialize (PyObject* pyroottype)
{
  // Make sure what we got is really a type.
  if (!PyType_Check(pyroottype))
    return;

  PyTypeObject* typ = (PyTypeObject*)pyroottype;

  // In root 5.18, each pyroot class gets its own individual metaclass;
  // PyRootType is now the metaclass of those metaclasses.
  // We want to be able to intercept attempts to set attributes
  // of pyroot classes.  So
  // we need to change setattr for each individual metaclass.
  // So what we need to do is patch tp_alloc for PyRootType, so
  // that when it creates the individual metaclasses, it patches
  // their setattr.
  // Don't do anything if it's already been patched.
  if (typ->tp_alloc == alloc_hack)
    return;

  // Install the patch.
  old_alloc = typ->tp_alloc;
  typ->tp_alloc = alloc_hack;
}


} // namespace RootUtils
