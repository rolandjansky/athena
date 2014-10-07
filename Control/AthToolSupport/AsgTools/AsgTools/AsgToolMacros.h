// AsgToolMacros.h

#ifndef AsgToolMacros_H
#define AsgToolMacros_H

// David Adams
// January 2014
//
// CPP macros that turn ASG tools into Athena tools.

#ifdef ASGTOOL_ATHENA

// This is used in the body of the tool interface class declaration, e.g.
//   class MyToolInterface:
//   virtual public IAsgTool {
//   ASG_TOOL_INTERFACE(MyToolInterface)
//   .
//   .
//   .
//   };
  
#define ASG_TOOL_INTERFACE(CLASSNAME) \
public: \
  static const InterfaceID& interfaceID() { \
    static const InterfaceID myid(#CLASSNAME, 1, 0 ); \
    return myid; \
  } 

// This is used in the body of the tool class declaration, e.g.
//   class MyTool
//   : public MyToolInterface,
//   : public asg::AsgTool {
//   ASG_TOOL_CLASS(MyTool, MyToolInterface)
//   .
//   .
//   .
//   };
// Macros are provided for 0, 1 or 2 interfaces.
// Need work here to make this work for an arbitrary number of interfaces.

#define ASG_TOOL_CLASS0(CLASSNAME) \
public: \
  CLASSNAME(const std::string&, const std::string& myname, const IInterface*) \
  : CLASSNAME(myname) { \
  }

#define ASG_TOOL_CLASS(CLASSNAME, INT1) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface*) \
  : CLASSNAME(myname) { \
    declareInterface<INT1>(this); \
    setAlgToolType(type); \
  }

#define ASG_TOOL_CLASS1 ASG_TOOL_CLASS

#define ASG_TOOL_CLASS2(CLASSNAME, INT1, INT2) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface*) \
  : CLASSNAME(myname) { \
    declareInterface<INT1>(this); \
    declareInterface<INT2>(this); \
    setAlgToolType(type); \
  }

#define ASG_TOOL_CLASS3(CLASSNAME, INT1, INT2, INT3) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface*) \
  : CLASSNAME(myname) { \
    declareInterface<INT1>(this); \
    declareInterface<INT2>(this); \
    declareInterface<INT3>(this); \
    setAlgToolType(type); \
  }

#else

#define ASG_TOOL_INTERFACE(CLASSNAME)
#define ASG_TOOL_CLASS(CLASSNAME, INT1)
#define ASG_TOOL_CLASS0(CLASSNAME)
#define ASG_TOOL_CLASS1(CLASSNAME, INT1)
#define ASG_TOOL_CLASS2(CLASSNAME, INT1, INT2)
#define ASG_TOOL_CLASS3(CLASSNAME, INT1, INT2, INT3)

#endif

#endif
