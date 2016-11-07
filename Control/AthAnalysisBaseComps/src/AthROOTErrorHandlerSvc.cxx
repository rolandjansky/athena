/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthROOTErrorHandlerSvc.h"

#include "TROOT.h"

ErrorHandlerFunc_t AthROOTErrorHandlerSvc::s_oldHandler = 0;
std::map<std::string, int> AthROOTErrorHandlerSvc::s_throwSources;
int AthROOTErrorHandlerSvc::s_catchLevel = kFatal+1;

AthROOTErrorHandlerSvc::AthROOTErrorHandlerSvc(const std::string& name, ISvcLocator *svcLoc) :
  AthService(name, svcLoc)
{
  //default map:
  //levels are: kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal
  //set in joboptions using: import ROOT ... ROOT.kError
  s_throwSources["TBranch::GetBasket"] = kError;
  s_throwSources["TFile::ReadBuffer"] = kError;
  declareProperty("ThrowSources",s_throwSources,"Map from source to error level. Any message at level or HIGHER will trigger runtime error");
  //POSSIBLE EXTENSION: declareProperty("IgnoreSources", s_ignoreSources, "Map from source to error level. Any message at level or LOWER will be ignored");
  declareProperty("CatchLevel",s_catchLevel=kFatal+1,"Throw runtime error for all messages at this level or HIGHER");
}

// Destructor
AthROOTErrorHandlerSvc::~AthROOTErrorHandlerSvc() {}


StatusCode AthROOTErrorHandlerSvc::initialize() {
  ATH_MSG_DEBUG("Replacing ROOT ErrorHandler");
  ErrorHandlerFunc_t h = ::SetErrorHandler( ErrorHandler );
  if(h != ErrorHandler) s_oldHandler = h; //if statement protects against double-initialize causing infinite loop in ErrorHandler
  return StatusCode::SUCCESS;
}

StatusCode AthROOTErrorHandlerSvc::finalize() {
  ATH_MSG_DEBUG("Restoring old ROOT ErrorHandler");
  if(s_oldHandler) ::SetErrorHandler( s_oldHandler );
  s_oldHandler = 0;
  return StatusCode::SUCCESS;
}

void AthROOTErrorHandlerSvc::ErrorHandler( Int_t level, Bool_t abort, const char* location, const char* message ) {
  if(s_catchLevel <= level) {
    std::ostringstream oss; oss << "AthROOTErrorHandlerSvc detected ROOT message >= CatchLevel " << s_catchLevel << " (" << level << "): " << location << " : " << message;
    throw std::runtime_error(oss.str());
  }
  auto itr = s_throwSources.find(location);
  if(itr!=s_throwSources.end() && itr->second <= level) {
    std::ostringstream oss; oss << "AthROOTErrorHandlerSvc detected ROOT message from ThrowSources : " << location << " (level=" << level << ") : " << message;
    throw std::runtime_error(oss.str());
  }
  //now pass on to old handler
  if(s_oldHandler) s_oldHandler( level, abort, location, message );
}
