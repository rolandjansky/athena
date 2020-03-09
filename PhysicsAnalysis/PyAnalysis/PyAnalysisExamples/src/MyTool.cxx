/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Python.h"

#include "PyAnalysisExamples/MyTool.h"
#include "PyAnalysisExamples/MyObj.h"
#include "PyKernel/PyReverseProxy.h"
#include "RootUtils/PyAthenaGILStateEnsure.h"

using namespace AthPyEx;

// interface ID
static const InterfaceID IID_MyTool("AthEx::MyTool", 1, 0);

const InterfaceID& MyTool::interfaceID( )
{ return IID_MyTool; }

StatusCode MyTool::execute(MyObj *obj)
{
  RootUtils::PyGILStateEnsure gil;

  // get RevProxy
  PyReverseProxy * proxy = PyReverseProxy::getProxy("MyObj_Proxy");
  // convert C++ obj to Python obj
  PyObject *pyObj = proxy->toPyObj(obj);
  // get Python code fragment
#if PY_MAJOR_VERSION < 3
  PyObject *pyFunc = PyObject_GetAttr(m_self,PyString_FromString("py_execute"));
#else
  PyObject *pyFunc = PyObject_GetAttr(m_self,PyUnicode_FromString("py_execute"));
#endif
  // execute Python code fragment
  PyObject *tup = Py_BuildValue((char*)"(O)",pyObj);
  PyObject *ret = PyObject_CallObject(pyFunc, tup); 
  // return value
  StatusCode sc = StatusCode::FAILURE;
  if (ret != NULL) {
    if (PyObject_IsTrue(ret))
      sc = StatusCode::SUCCESS;
    Py_DECREF(ret); 
  }
  // decrement reference counters
  Py_DECREF(pyObj); 
  Py_DECREF(pyFunc); 
  Py_DECREF(tup); 
  // return
  return sc;
}

int MyTool::getA () const 
{
  return m_A;
}

void MyTool::setA (const int a)
{
  m_A = a;
}
