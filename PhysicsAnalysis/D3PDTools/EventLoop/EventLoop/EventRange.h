/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__EVENT_RANGE_H
#define EVENT_LOOP__EVENT_RANGE_H

#include <EventLoop/Global.h>

#include <Rtypes.h>
#include <string>

namespace EL
{
  /// \brief a range of events in a given file

  struct EventRange final
  {
    /// \brief the location of the file
    std::string m_url;

    /// \brief the first event to process
    Long64_t m_beginEvent {0};

    /// \brief the event past the last event, or `eof`
    Long64_t m_endEvent {eof};

    /// \brief the special value to indicate that the range includes
    /// all events until the end of the file
    static constexpr Long64_t eof {-1};
  };
}

#endif
