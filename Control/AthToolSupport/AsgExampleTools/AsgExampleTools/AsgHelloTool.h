// AsgHelloTool.h

#ifndef AsgHelloTool_H
#define AsgHelloTool_H

// David Adams.
// January 2014
//
// This is a simple ASG dual-use tool intended as an
// example for tool developers.

#include "AsgExampleTools/IAsgHelloTool.h"
#include "AsgTools/AsgTool.h"

class AsgHelloTool
: public asg::AsgTool,
  virtual public IAsgHelloTool {
ASG_TOOL_CLASS(AsgHelloTool, IAsgHelloTool)

public:

  AsgHelloTool( const std::string& myname );

  virtual StatusCode initialize();

  virtual void print() const;

  // Display the message recorded as property "Message"
  virtual int talk() const;

private:

  std::string m_msg;

};

#endif
