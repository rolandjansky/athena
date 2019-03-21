/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__MESSAGE_PRINTER_OVERLAY_H
#define ASG_TOOLS__MESSAGE_PRINTER_OVERLAY_H

namespace asg
{
  class IMessagePrinter;

  /// \brief an object to change the \ref IMessagePrinter temporarily
  ///
  /// The idea is to use this in unit tests to replace the standard
  /// printer with a mock object.  The idea is to place this on the
  /// stack and it will then replace the standard object for as long
  /// as it is on the stack.
  ///
  /// Note that this is per se not thread-safe, but given the intended
  /// purpose that ought to be fine, i.e. there is only one
  /// MessagePrinter for all threads, so if it is replaced in a
  /// multi-threaded manner it will cause other problems.

  class MessagePrinterOverlay final
  {
    //
    // public interface
    //

    MessagePrinterOverlay (MessagePrinterOverlay&) = delete;
    MessagePrinterOverlay operator = (MessagePrinterOverlay&) = delete;


    /// \brief overlay with the given message printer
    /// \par Guarantee
    ///   no-fail
    /// \pre val_printer != nullptr
  public:
    MessagePrinterOverlay (IMessagePrinter *val_printer) noexcept;

    /// \brief remove the currently overlaid printer
    /// \par Guarantee
    ///   no-fail
  public:
    ~MessagePrinterOverlay () noexcept;

    /// \brief get the current \ref IMessagePrinter
    /// \par Guarantee
    ///   no-fail
  public:
    static IMessagePrinter& current () noexcept;



    //
    // private interface
    //

    /// \brief get a reference to the pointer to the current \ref
    /// IMessagePrinter
    /// \par Guarantee
    ///   no-fail
    /// \post result != nullptr
  private:
    static IMessagePrinter*& getCurrent () noexcept;

    /// \brief the stored message printer we need to restore
  private:
    IMessagePrinter *m_saved = nullptr;

    /// \brief the current message printer we should have when we restore
  private:
    IMessagePrinter *m_current = nullptr;
  };
}

#endif
