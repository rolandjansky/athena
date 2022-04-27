/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthROOTErrorHandlerSvc.h"

#include "TROOT.h"

namespace Handler {
  AthROOTErrorHandlerSvc* svc;
  ErrorHandlerFunc_t oldHandler;

  void ErrorHandler ATLAS_NOT_THREAD_SAFE ( Int_t level, Bool_t abort, const char* location, const char* message ) {
    if (svc) {  // should always be set by now
      if(svc->m_catchLevel <= level) {
        std::ostringstream oss;
        oss << "AthROOTErrorHandlerSvc detected ROOT message >= CatchLevel "
            << svc->m_catchLevel << " (" << level << "): " << location << " : " << message;
        throw std::runtime_error(oss.str());
      }
      const auto itr = svc->m_throwSources.find(location);
      if(itr!=svc->m_throwSources.end() && itr->second <= level) {
        std::ostringstream oss;
        oss << "AthROOTErrorHandlerSvc detected ROOT message from Throw Sources : "
            << location << " (level=" << level << ") : " << message;
        throw std::runtime_error(oss.str());
      }
    }
    //now pass on to old handler
    if(oldHandler) oldHandler( level, abort, location, message );
  }

}


AthROOTErrorHandlerSvc::AthROOTErrorHandlerSvc(const std::string& name, ISvcLocator *svcLoc) :
  AthService(name, svcLoc) {
}

AthROOTErrorHandlerSvc::~AthROOTErrorHandlerSvc() {
}


StatusCode AthROOTErrorHandlerSvc::initialize ATLAS_NOT_THREAD_SAFE() {

  ATH_MSG_DEBUG("Replacing ROOT ErrorHandler");
  ErrorHandlerFunc_t h = ::SetErrorHandler( Handler::ErrorHandler );
  Handler::svc = this;

  // if statement protects against double-initialize causing infinite loop in ErrorHandler
  if (h != Handler::ErrorHandler) {
    Handler::oldHandler = h;
  }
  return StatusCode::SUCCESS;
}

StatusCode AthROOTErrorHandlerSvc::finalize ATLAS_NOT_THREAD_SAFE() {

  ATH_MSG_DEBUG("Restoring old ROOT ErrorHandler");
  Handler::svc = nullptr;
  if (Handler::oldHandler) ::SetErrorHandler( Handler::oldHandler );
  Handler::oldHandler = nullptr;
  return StatusCode::SUCCESS;
}
