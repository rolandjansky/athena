//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
