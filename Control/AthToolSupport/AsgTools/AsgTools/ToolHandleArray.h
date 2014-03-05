// ToolHandleArray.h

#ifndef ToolHandleArray_H
#define ToolHandleArray_H

// David Adams
// January 2014
//
// Partial reimplementation of Gaudi ToolHandleArray so that tools
// outside Athena can hold other tools with the same syntax as Athena.

#include <vector>
#include "AsgTools/ToolHandle.h"

namespace asg {

template <class T>
class ToolHandleArray : public std::vector<ToolHandle<T> > {

public:

  // Retrieve all tools.
  StatusCode retrieve();

};

}  // end namespace asg

#endif

#ifndef ToolHandleArray_ICC
#define ToolHandleArray_ICC

template <class T>
StatusCode asg::ToolHandleArray<T>::retrieve() {
  StatusCode scout = StatusCode::SUCCESS;
  for ( typename asg::ToolHandleArray<T>::iterator itoo=this->begin();
        itoo!=this->end(); ++itoo ) {
    StatusCode sc = itoo->retrieve();
    if ( sc.isFailure() ) scout = StatusCode::FAILURE;
  }
  return scout;
}

#endif
