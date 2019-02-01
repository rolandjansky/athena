/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__MESSAGE_PRINTER_ERROR_COLLECT_H
#define ASG_TOOLS__MESSAGE_PRINTER_ERROR_COLLECT_H

#ifdef ROOTCORE

#include <AsgTools/IMessagePrinter.h>
#include <vector>

namespace asg
{
  /// \brief an implementation for \ref IMessagePrinter that collects
  /// the reported errors as strings
  ///
  /// This is mostly meant for unit tests to allow them to collect all
  /// error messages and then check whether they match a given regular
  /// expression.

  class MessagePrinterErrorCollect final : public IMessagePrinter
  {
    /// \brief print the given message
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    virtual void
    print (MSG::Level reglvl, const std::string& name,
           const std::string& text) override;


    /// \brief whether we have no error messages
    /// \par Guarantee
    ///   no-fail
  public:
    bool empty () const noexcept;


    /// \brief whether we have an error message matching the given
    /// expression
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    bool matchesRegex (const std::string& pattern) const;


    /// \brief return this as a string when reporting a failure
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    std::string asString (const std::string& prefix) const;


    /// \brief the error strings encountered
  private:
    std::vector<std::string> m_errorMessages;
  };
}

#endif

#endif
