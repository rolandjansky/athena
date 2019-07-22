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

  /// \brief get the label of the accessor
  public:
    virtual std::string label () const = 0;
  };


  /// \brief make the \ref ISelectionAccessor for the given name
  ///
  /// This will invoke @c SelectionExprParser to build a @c ISelectionAccessor
  /// that corresponds to a given expression.
  /// @note Passing in a simple selection name will createa plain selection accessor that
  ///       only reads that single name.
  /// @note Passing in an expression which only contains names and && will produce an
  ///       instance of @c SelectionAccessorList, which has special semantics and allows
  ///       retrievel of AND'ed accessors via a bit sequence.
  /// @note Allowed operators are &&, || and !, grouping with () is possible.
  ///
  /// @param [in] expr The expression to parse.
  /// @param [out] accessor The created accessor will be owned by this unique pointer.
  /// @param [in] defaultToChar Whether to treat decorations as char by default
  StatusCode
  makeSelectionAccessor (const std::string& expr,
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar = false);

  /// @brief Produces a simple @c ISelectionAccessor accessing the given decoration
  /// @note Through annotations like ",as_char" or ",as_bits" at the end of a name,
  ///       different decoding strategies can be selected. These need to match the
  ///       decoration type.
  /// @param [in] name The name of the decoration to create an accessor for.
  /// @param [out] accessor The created accessor will be owned by this unique pointer.
  /// @param [in] defaultToChar Whether to treat decorations as char by default
  StatusCode
  makeSelectionAccessorVar (const std::string& name,
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar = false);
}

#endif
