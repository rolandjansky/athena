/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ATHROOTERRORHANDLERSVC__
#define __ATHROOTERRORHANDLERSVC__

#include "AthenaBaseComps/AthService.h"
#include "CxxUtils/checker_macros.h"

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

namespace Handler {
  void ErrorHandler ATLAS_NOT_THREAD_SAFE ( Int_t level, Bool_t abort, const char* location, const char* message );
}

class AthROOTErrorHandlerSvc : public AthService {

public:
  friend void Handler::ErrorHandler( Int_t level, Bool_t abort, const char* location, const char* message );

  AthROOTErrorHandlerSvc(const std::string& name, ISvcLocator *svcLoc);
  virtual ~AthROOTErrorHandlerSvc();

  virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE() override;
  virtual StatusCode finalize ATLAS_NOT_THREAD_SAFE() override;

private:  

  Gaudi::Property<int> m_catchLevel{
    this, "CatchLevel", kFatal+1,
    "Throw runtime error for all messages at this level or HIGHER"};

  Gaudi::Property<std::map<std::string,int>> m_throwSources{
    this, "ThrowSources", {{"TBranch::GetBasket", kError},
                           {"TFile::ReadBuffer", kError}},
    "Map from source to error level. Any message at level or HIGHER will trigger runtime error"};
};


#endif

