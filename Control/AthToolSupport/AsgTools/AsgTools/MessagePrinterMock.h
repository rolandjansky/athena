/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__MESSAGE_PRINTER_MOCK_H
#define ASG_TOOLS__MESSAGE_PRINTER_MOCK_H

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
