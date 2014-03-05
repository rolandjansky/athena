// ToolHandle.h

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/ToolHandle.h"
#else

#ifndef asg_ToolHandle_H
#define asg_ToolHandle_H

// David Adams
// January 2014
//
// Partial reimplementation of Gaudi ToolHandle so that tools
// outside Athena can hold other tools with the same syntax as Athena.

#include <iostream>
#include <typeinfo>
#include "AsgTools/StatusCode.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/ToolStore.h"

namespace asg {

template <class T>
class ToolHandle {

public:

  typedef T value_type;

  // Ctor from a tool pointer.
  ToolHandle(IAsgTool* ptool =0);

  // Ctor from a tool name.
  ToolHandle(const char* toolname);
  ToolHandle(std::string toolname);

  // Dereferencing.
  T* operator*() const;
  T* operator->() const;

  // Retrieve tool.
  // For compatibility with Gaudi.
  // Returns success if pointer is non-null and of the correct type.
  StatusCode retrieve() const;

private:  // data

  mutable IAsgTool* m_ptool;
  std::string m_toolname;

};

}  // end namespace asg

#endif  // asg_ToolHandle_H

#ifndef asg_ToolHandle_ICC
#define asg_ToolHandle_ICC

template <class T>
asg::ToolHandle<T>::ToolHandle(IAsgTool* ptool) : m_ptool(ptool) {
  //std::cout << "Tool handle ctor: " << ptool << " --> " << m_ptool << " (" << dynamic_cast<T*>(m_ptool) << ")" << std::endl;
}

template <class T>
asg::ToolHandle<T>::ToolHandle(const char* toolname) : m_ptool(0), m_toolname(toolname) { }

template <class T>
asg::ToolHandle<T>::ToolHandle(std::string toolname) : m_ptool(0), m_toolname(toolname) { }

template <class T>
T* asg::ToolHandle<T>::operator*() const {
  if ( m_ptool == 0 ) {
    m_ptool = ToolStore::get<T>(m_toolname);
  }
  return dynamic_cast<T*>(m_ptool);
}

template <class T>
T* asg::ToolHandle<T>::operator->() const {
  //std::cout << "Tool handle dereference: " << m_ptool << " --> " << dynamic_cast<T*>(m_ptool) << std::endl;
  if ( m_ptool == 0 ) {
    m_ptool = ToolStore::get<T>(m_toolname);
  }
  return dynamic_cast<T*>(m_ptool);
}

template <class T>
StatusCode asg::ToolHandle<T>::retrieve() const {
  return StatusCode(operator->() != 0);
}

// Ostream operator for Gaudi compatibility.
template <class T>
std::ostream& operator<<(std::ostream& lhs, asg::ToolHandle<T> rhs) {
  T* ptool = rhs.operator->();
  if ( ptool == 0 ) {
    lhs << "Invalid ASG ToolHandle";
  } else {
    lhs << "ASG ToolHandle (" << typeid(*rhs.operator->()).name() << ")";
  }
  return lhs;
}

#endif  // asg_ToolHandle_ICC

#endif  // ! ASGTOOL_ATHENA
