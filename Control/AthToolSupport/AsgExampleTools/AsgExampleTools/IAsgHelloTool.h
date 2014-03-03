// IAsgHelloTool.h

#ifndef IAsgHelloTool_H
#define IAsgHelloTool_H

#include "AsgTools/IAsgTool.h"

class IAsgHelloTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IAsgHelloTool)

public:

  // Display a message.
  virtual int talk() const =0;

};

#endif
