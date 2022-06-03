/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__I_SELECTION_READ_ACCESSOR_H
#define SELECTION_HELPERS__I_SELECTION_READ_ACCESSOR_H

#include <AthContainers/AuxElement.h>
#include <SelectionHelpers/SelectionHelpers.h>
#include <memory>

class StatusCode;

namespace CP
{
  class ISystematicsSvc;
  class SystematicSet;


  /// \brief a specialized accessor to read a selection
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

  class ISelectionReadAccessor
  {
    // no slicing
    ISelectionReadAccessor& operator = (const ISelectionReadAccessor&) = delete;

    /// \brief standard (virtual) destructor
  public:
    virtual ~ISelectionReadAccessor () noexcept = default;

    /// \brief get the selection decoration
  public:
    virtual SelectionType
    getBits (const SG::AuxElement& element,
             const CP::SystematicSet *sys = nullptr) const = 0;

    /// \brief get the selection decoration
  public:
    virtual bool getBool (const SG::AuxElement& element,
                          const CP::SystematicSet *sys = nullptr) const = 0;

    /// \brief get the label of the accessor
  public:
    virtual std::string label () const = 0;

    /// \brief get the systematics when reading from the decoration
  public:
    virtual CP::SystematicSet
    getInputAffecting (const ISystematicsSvc& svc,
                       const std::string& objectName) const = 0;

    /// \brief fill the systematic variations
  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) = 0;
  };


  /// \brief make the \ref ISelectionReadAccessor for the given name
  ///
  /// This will invoke @c SelectionExprParser to build a @c ISelectionReadAccessor
  /// that corresponds to a given expression.
  /// @note Passing in a simple selection name will createa plain selection accessor that
  ///       only reads that single name.
  /// @note Passing in an expression which only contains names and && will produce an
  ///       instance of @c SelectionReadAccessorList, which has special semantics and allows
  ///       retrievel of AND'ed accessors via a bit sequence.
  /// @note Allowed operators are &&, || and !, grouping with () is possible.
  ///
  /// @param [in] expr The expression to parse.
  /// @param [out] accessor The created accessor will be owned by this unique pointer.
  /// @param [in] defaultToChar Whether to treat decorations as char by default
  StatusCode
  makeSelectionReadAccessor (const std::string& expr,
                             std::unique_ptr<ISelectionReadAccessor>& accessor,
                             bool defaultToChar = false);

  /// @brief Produces a simple @c ISelectionReadAccessor accessing the given decoration
  /// @note Through annotations like ",as_char" or ",as_bits" at the end of a name,
  ///       different decoding strategies can be selected. These need to match the
  ///       decoration type.
  /// @param [in] name The name of the decoration to create an accessor for.
  /// @param [out] accessor The created accessor will be owned by this unique pointer.
  /// @param [in] defaultToChar Whether to treat decorations as char by default
  StatusCode
  makeSelectionReadAccessorVar (const std::string& name,
                                std::unique_ptr<ISelectionReadAccessor>& accessor,
                                bool defaultToChar = false);
}

#endif
