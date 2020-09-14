/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AsgToolMacros_H
#define AsgToolMacros_H

// David Adams
// January 2014
//
// CPP macros that turn ASG tools into Athena tools.

#ifndef XAOD_STANDALONE

// This is used in the body of the tool interface class declaration, e.g.
//   class MyToolInterface:
//   virtual public IAsgTool {
//   ASG_TOOL_INTERFACE(MyToolInterface)
//   .
//   .
//   .
//   };

// We need to pass both the tool type and the parent pointer down to
// AthAlgTool, but the intermediate class only takes a single string parameter.
// Sort of like passing a camel through the eye of a needle, but as MAD
// magazine observed, nowhere is the size of the needle specified.
// So, we pack the type, name, name parent pointer into a string:
//   PTR#TYPE/NAME
// where PTR is a hex string representation of the pointer.
// We then unpack this in the AsgTools constructor.

namespace asg {
  std::string ptrToString (const void* p);
}
  
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

// Extra std::string around the ptrToString calls below needed to work around
// problem with cppcheck 2.0,
// which gets confused about the return type of ptrToString.
#define ASG_TOOL_CLASS0(CLASSNAME) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface* parent) \
: CLASSNAME(std::string(asg::ptrToString(parent))+"#"+type+"/"+myname) { \
  }

#define ASG_TOOL_CLASS(CLASSNAME, INT1) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface* parent) \
: CLASSNAME(std::string(asg::ptrToString(parent))+"#"+type+"/"+myname) { \
    declareInterface<INT1>(this); \
  }

#define ASG_TOOL_CLASS1 ASG_TOOL_CLASS

#define ASG_TOOL_CLASS2(CLASSNAME, INT1, INT2) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface* parent) \
: CLASSNAME(std::string(asg::ptrToString(parent))+"#"+type+"/"+myname) { \
    declareInterface<INT1>(this); \
    declareInterface<INT2>(this); \
  }

#define ASG_TOOL_CLASS3(CLASSNAME, INT1, INT2, INT3) \
public: \
  CLASSNAME(const std::string& type, const std::string& myname, const IInterface* parent) \
: CLASSNAME(std::string(asg::ptrToString(parent))+"#"+type+"/"+myname) { \
    declareInterface<INT1>(this); \
    declareInterface<INT2>(this); \
    declareInterface<INT3>(this); \
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
