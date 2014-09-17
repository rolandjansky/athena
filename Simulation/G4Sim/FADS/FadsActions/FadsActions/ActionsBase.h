/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FADS_ACTIONS_BASE
#define FADS_ACTIONS_BASE

#include "GaudiKernel/SmartIF.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Needed because SmartIF has pointer access in the header
#include "GaudiKernel/IMessageSvc.h"

#include <string>

namespace FADS {

class ActionsBase : public ::AthMessaging
{
 public:
   ActionsBase(std::string name) : ::AthMessaging( msgSvc(), name ) {}
   ~ActionsBase() {}
   SmartIF<IMessageSvc>& msgSvc() const;

   using AthMessaging::msg;

  protected:
   mutable SmartIF<IMessageSvc> m_MS;          ///< Message service
   std::string name;
};

} // End of namespace
#endif