/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/MessagePrinterOverlay.h>

#include <AsgTools/MessagePrinter.h>
#include <cassert>

//
// method implementations
//

namespace asg
{
  MessagePrinterOverlay ::
  MessagePrinterOverlay (IMessagePrinter *val_printer) noexcept
    : m_saved (getCurrent()), m_current (val_printer)
  {
    assert (val_printer != nullptr);
    getCurrent() = val_printer;
  }



  MessagePrinterOverlay ::
  ~MessagePrinterOverlay () noexcept
  {
    assert (getCurrent() == m_current);
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
