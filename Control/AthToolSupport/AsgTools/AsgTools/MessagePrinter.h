/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__MESSAGE_PRINTER_H
#define ASG_TOOLS__MESSAGE_PRINTER_H

#ifdef XAOD_STANDALONE

#include <AsgTools/IMessagePrinter.h>

namespace asg
{
  /// \brief the standard implementation for \ref IMessagePrinter

  class MessagePrinter final : public IMessagePrinter
  {
    /// \brief standard constructor
    /// \par Guarantee
    ///   no-fail
  public:
    MessagePrinter (unsigned val_sourceWidth = 25);


    /// \brief print the given message
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    virtual void
    print (MSG::Level reglvl, const std::string& name,
           const std::string& text) override;


    /// \brief the width reserved for the source name
  private:
    std::size_t m_sourceWidth = 25;
  };
}

#endif

#endif
