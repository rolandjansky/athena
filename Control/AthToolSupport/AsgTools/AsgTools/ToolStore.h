// ToolStore.h

#ifndef asg_ToolStore_H
#define asg_ToolStore_H

#include "AsgTools/IAsgTool.h"

namespace asg {

class ToolStore {

public:

  // Store a named tool by its name.
  // The name must be non-blank and not already exist in the store.
  // Returns 0 for success.
  static int put(IAsgTool* ptool);

  // Store a tool by name and assign its name accordingly.
  // The name must be non-blank and not already exist in the store.
  // Returns 0 for success.
  static int put(IAsgTool* ptool, std::string name);

  // Retrieve a tool by name.
  static IAsgTool* get(std::string name);

  // Retrieve a tool by name and interface.
  template<typename T>
  static T* get(std::string name);

};

}  // end namespace asg

template<typename T>
T* asg::ToolStore::get(std::string name) {
  return dynamic_cast<T*>(ToolStore::get(name));
}

#endif
