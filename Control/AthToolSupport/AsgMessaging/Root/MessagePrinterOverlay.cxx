/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgMessaging/MessagePrinterOverlay.h>

#include <AsgMessaging/MessageCheckLocal.h>
#include <AsgMessaging/MessagePrinter.h>
#include <cstdlib>

//
// method implementations
//

namespace asg
{
  MessagePrinterOverlay ::
  MessagePrinterOverlay (IMessagePrinter *val_printer) noexcept
    : m_saved (getCurrent()), m_current (val_printer)
  {
    using namespace msgAsgMessaging;

    if (val_printer == nullptr)
    {
      ANA_MSG_FATAL ("nullptr passed illegally");
      std::abort ();
    }
    getCurrent() = val_printer;
  }



  MessagePrinterOverlay ::
  ~MessagePrinterOverlay () noexcept
  {
    using namespace msgAsgMessaging;

    if (getCurrent() != m_current)
    {
      ANA_MSG_FATAL ("the overlay message printer changed underhand this should never happen");
      std::abort ();
    }
    getCurrent() = m_saved;
  }



  IMessagePrinter& MessagePrinterOverlay ::
  current () noexcept
  {
    return *getCurrent();
  }



  IMessagePrinter*& MessagePrinterOverlay ::
  getCurrent () noexcept
  {
    static MessagePrinter defaultPrinter;
    static IMessagePrinter *pointer = &defaultPrinter;
    return pointer;
  }
}
