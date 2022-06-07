/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__I_SELECTION_WRITE_ACCESSOR_H
#define SELECTION_HELPERS__I_SELECTION_WRITE_ACCESSOR_H

#include <AthContainers/AuxElement.h>
#include <SelectionHelpers/SelectionHelpers.h>
#include <memory>

class StatusCode;

namespace CP
{
  class ISystematicsSvc;
  class SystematicSet;


  /// \brief a specialized accessor to write a selection
  /// decoration to an xAOD object
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

  class ISelectionWriteAccessor
  {
    // no slicing
    ISelectionWriteAccessor& operator = (const ISelectionWriteAccessor&) = delete;

    /// \brief standard (virtual) destructor
  public:
    virtual ~ISelectionWriteAccessor () noexcept = default;

    /// \brief set the selection decoration
  public:
    virtual void setBits (const SG::AuxElement& element,
                          SelectionType selection,
                          const CP::SystematicSet *sys = nullptr) const = 0;

    /// \brief set the selection decoration
  public:
    virtual void setBool (const SG::AuxElement& element,
                          bool selection,
                          const CP::SystematicSet *sys = nullptr) const = 0;

    /// \brief get the label of the accessor
  public:
    virtual std::string label () const = 0;


    /// \brief fill the systematic variations
  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) = 0;
  };


  /// @brief Produces a simple @c ISelectionWriteAccessor accessing the given decoration
  /// @note Through annotations like ",as_char" or ",as_bits" at the end of a name,
  ///       different decoding strategies can be selected. These need to match the
  ///       decoration type.
  /// @param [in] name The name of the decoration to create an accessor for.
  /// @param [out] accessor The created accessor will be owned by this unique pointer.
  /// @param [in] defaultToChar Whether to treat decorations as char by default
  StatusCode
  makeSelectionWriteAccessor (const std::string& name,
                              std::unique_ptr<ISelectionWriteAccessor>& accessor,
                              bool defaultToChar = false);
}

#endif
