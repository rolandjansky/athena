/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__I_SELECTION_ACCESSOR_H
#define SELECTION_HELPERS__I_SELECTION_ACCESSOR_H

#include <AthContainers/AuxElement.h>
#include <SelectionHelpers/SelectionHelpers.h>
#include <memory>

class StatusCode;

namespace CP
{
  /// \brief a specialized accessor to read/write a selection
  /// decoration from/to an xAOD object
  ///
  /// We have (at least) two conventions on how selection decorations
  /// can look like, the one from the CP algorithms/TAccept and the
  /// other from the derivation framework/overlap removal tool.  This
  /// accessor is a base class with specific implementations for
  /// different conventions.
  ///
  /// The actual interface has separate accessor functions for
  /// accessing as bitsets or as booleans.  In part that is to avoid a
  /// lot of unnecessary back and forth conversions, but mostly it is
  /// to give us some more flexibility if we need to extend this in
  /// the future.

  class ISelectionAccessor
  {
    // no slicing
    ISelectionAccessor& operator = (const ISelectionAccessor&) = delete;

    /// \brief standard (virtual) destructor
  public:
    virtual ~ISelectionAccessor () noexcept = default;

    /// \brief get the selection decoration
  public:
    virtual SelectionType
    getBits (const SG::AuxElement& element) const = 0;

    /// \brief set the selection decoration
  public:
    virtual void setBits (SG::AuxElement& element,
                          SelectionType selection) const = 0;

    /// \brief get the selection decoration
  public:
    virtual bool getBool (const SG::AuxElement& element) const = 0;

    /// \brief set the selection decoration
  public:
    virtual void setBool (SG::AuxElement& element,
                          bool selection) const = 0;
  };


  /// \brief make the \ref ISelectionAccessor for the given name
  ///
  /// This will normally make an accessor for regular CP algorithm
  /// style decorations, but through special names can be configured
  /// to yield other kinds of decorations as well.
  StatusCode
  makeSelectionAccessor (const std::string& name,
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar = false);
}

#endif
