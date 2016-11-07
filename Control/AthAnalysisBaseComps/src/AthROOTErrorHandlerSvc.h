/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ATHROOTERRORHANDLERSVC__
#define __ATHROOTERRORHANDLERSVC__

#include "AthenaBaseComps/AthService.h"


/// AthROOTErrorHandlerSvc: 
///   Replaces ROOT's standard error handler in order to detect
///   Problematic ROOT issues such as stale file handles and read failures
///
/// Author: Will Buttinger
///

/*Here's an example of how to include in an athena job options:

theApp.CreateSvc += ["AthROOTErrorHandlerSvc"]

*/

/*Here's an example test for the ROOT prompt:

AAH::initGaudi();
ServiceHandle<IService> s("AthROOTErrorHandlerSvc","");
std::map<std::string, int> sources; sources["test1"] = kError;
AAH::setProperty( s , "ThrowSources", sources )
s->initialize();
Error("aaa","bbb"); //should be ok
Error("test1","hello"); //should throw exception

*/


#include "TError.h"

class AthROOTErrorHandlerSvc : public AthService {

public:
  
  // Constructors, destructor
  AthROOTErrorHandlerSvc(const std::string& name, ISvcLocator *svcLoc);
  virtual ~AthROOTErrorHandlerSvc();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

  static void ErrorHandler( Int_t level, Bool_t abort, const char* location, const char* message );

private:  

  static ErrorHandlerFunc_t s_oldHandler;
  static std::map<std::string,int> s_throwSources;
  static int s_catchLevel;

};


#endif

