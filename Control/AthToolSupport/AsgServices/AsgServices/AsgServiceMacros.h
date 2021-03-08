/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)

#ifndef AsgServiceMacros_H
#define AsgServiceMacros_H

/// \file CPP macros that turn ASG services into Athena services.

#ifndef XAOD_STANDALONE

// This is used in the body of the service interface class definition, e.g.
//   class IMyServiceInterface:
//   virtual public IAsgService {
//   ASG_SERVICE_INTERFACE(MyServiceInterface)
//   .
//   .
//   .
//   };
  
#define ASG_SERVICE_INTERFACE(CLASSNAME) \
public: \
  static const InterfaceID& interfaceID() { \
    static const InterfaceID myid(#CLASSNAME, 1, 0 ); \
    return myid; \
  } 

// This is used in the body of the service implementation definition, e.g.
//   class MyServiceInterface:
//   virtual public IMyServiceInterface, public AsgService {
//   ASG_SERVICE_INTERFACE(MyServiceInterface,IMyServiceInterface)
//   .
//   .
//   .
//   };
  
#define ASG_SERVICE_CLASS1(CLASSNAME,INT1)      \
  public:                                       \
  StatusCode queryInterface( const InterfaceID& riid, void** ppvi ) override { \
  if ( ppvi == nullptr ) { return StatusCode::FAILURE ; }               \
    if ( INT1::interfaceID() == riid ) {                                \
    *ppvi = static_cast<INT1*>(this);                                   \
    addRef();                                                           \
    return StatusCode::SUCCESS;}                                        \
  return AthService::queryInterface( riid, ppvi );}

#else

#define ASG_SERVICE_INTERFACE(CLASSNAME)
#define ASG_SERVICE_CLASS1(CLASSNAME,INT1)

#endif

#endif
