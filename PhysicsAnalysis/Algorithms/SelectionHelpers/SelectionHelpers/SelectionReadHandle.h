/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS_SELECTION_READ_HANDLE_H
#define SELECTION_HELPERS_SELECTION_READ_HANDLE_H

#include <AthContainers/AuxElement.h>
#include <AsgTools/MsgStream.h>
#include <memory>

class StatusCode;

namespace CP
{
  class ISelectionAccessor;

  /// \brief a data handle for reading selection properties from
  /// objects
  ///
  /// Essentially this is just a wrapper around \ref
  /// ISelectionAccessor to make it easier to use in an algorithm.
  /// Since we are now using this a lot, it seems like a good idea to
  /// streamline this as much as possible.

  class SelectionReadHandle final
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SelectionReadHandle (T2 *owner, const std::string& propertyName,
                         const std::string& propertyValue,
                         const std::string& propertyDescription);


    /// \brief whether we have a name configured
  public:
    bool empty () const noexcept;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief initialize the accessor
  public:
    StatusCode initialize ();


    /// \brief get the selection as a bool
  public:
    bool getBool (const SG::AuxElement& element) const;



    //
    // private interface
    //

    /// \brief the selection we use
  private:
    std::string m_selection;

    /// \brief the accessor we use
  private:
    std::unique_ptr<ISelectionAccessor> m_accessor;


    /// \brief the message stream we use
  private:
    MsgStream *m_msg {nullptr};

    /// \brief helper for message macros
  private:
    MsgStream& msg( ) const;

    /// \brief helper for message macros
  private:
    MsgStream& msg( const MSG::Level lvl ) const;
  };
}

#include "SelectionReadHandle.icc"

#endif
