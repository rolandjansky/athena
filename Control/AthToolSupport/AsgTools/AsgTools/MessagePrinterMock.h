#ifndef ASG_TOOLS__MESSAGE_PRINTER_MOCK_H
#define ASG_TOOLS__MESSAGE_PRINTER_MOCK_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/IMessagePrinter.h>
#include <gmock/gmock.h>

#ifdef XAOD_STANDALONE

namespace asg
{
  struct MessagePrinterMock : public IMessagePrinter
  {
    MOCK_METHOD3 (print, void (MSG::Level, const std::string&, const std::string&));
  };
}

#endif

#endif
