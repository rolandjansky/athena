/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYTOOL_H
#define PYANALYSISEXAMPLES_MYTOOL_H

// Work around a compilation warning.
#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"
#include "AthenaBaseComps/AthAlgTool.h"

class MyObj;

namespace AthPyEx {

class MyTool : public AthAlgTool
{
public:

  /** constructor
   */
  MyTool ( const std::string& type, const std::string& name,
	   const IInterface* parent )
    : AthAlgTool(type,name,parent),
      m_A(0),
      m_self(0)
    {}

  /** destructor
   */
  virtual ~MyTool () {}

  /** InterfaceID
   */
  static const InterfaceID& interfaceID( ) ;

  /** set Python binding 
   */
  void setPython(PyObject *self) 
  {
    m_self=self;
  }

  StatusCode execute(MyObj *obj);

  /** accessor 
   */
  int getA () const ;
  void setA (int a);

private:
  int m_A;

  PyObject * m_self;

  MyTool& operator= (const MyTool&);
};

} //> end namespace AthPyEx

#endif

  
