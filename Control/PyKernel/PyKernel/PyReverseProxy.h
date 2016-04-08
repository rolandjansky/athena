/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYKERNEL_PYREVERSEPROXY_H
#define PYKERNEL_PYREVERSEPROXY_H

/**
   A utility class to convert a C++ object to a PyObject

   @author Tadashi Maeno
*/

#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"
#include <map>
#include <string>


struct PyReverseProxy
{
  /// destructor
  ~PyReverseProxy()
  {
    m_proxyMap.erase(m_key);
  }

  /// setter
  void setFunc (PyObject * func)
  {
    Py_DECREF(m_func); 
    m_func = func;
    Py_INCREF(m_func); 
  }

  // getter
  void getObj (void *& obj)
  {
    obj = m_obj;
  }

  PyObject * toPyObj (void * obj)
  {
    m_obj = obj;
    return PyObject_CallObject(m_func,NULL);
  }

  /// factory method
  static PyReverseProxy * getProxy(const std::string & key)
  {
    if (m_proxyMap.count(key) == 0)
      m_proxyMap[key] = new PyReverseProxy(key);
    return m_proxyMap[key];
  }

private:
  /// default constructor : never used
  PyReverseProxy() 
  {}

  /// constructor
  PyReverseProxy(const std::string &key)
    : m_key(key), m_obj(0), m_func(0)
  {}

  /// key
  std::string m_key;

  /// C++ obj
  void * m_obj;

  /// python code fragment to convert C++ obj to PyObj
  PyObject * m_func;

  /// proxy map
  static std::map<std::string, PyReverseProxy *> m_proxyMap;
};

#endif

  
