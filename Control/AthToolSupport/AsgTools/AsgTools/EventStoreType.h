/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__EVENT_STORE_TYPE_H
#define ASG_TOOLS__EVENT_STORE_TYPE_H

#ifndef XAOD_STANDALONE
class StoreGateSvc;
#endif

namespace asg
{
#ifdef XAOD_STANDALONE
  class SgTEvent;
#endif

  /// \brief the type returned by \ref AsgTool::evtStore
  ///
  /// This is provided in case you need to pass the event store into a
  /// stand-alone helper function.
#ifndef XAOD_STANDALONE
   typedef StoreGateSvc EventStoreType;
#else
   typedef SgTEvent EventStoreType;
#endif
}

#endif
