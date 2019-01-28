// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASG_TOOLS__MESSAGE_PRINTER_H
#define ASG_TOOLS__MESSAGE_PRINTER_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
