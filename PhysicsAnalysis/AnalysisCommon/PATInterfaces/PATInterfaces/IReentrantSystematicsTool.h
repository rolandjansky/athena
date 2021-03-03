/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef PAT_INTERFACES__I_RENTRANT_SYSTEMATICS_TOOL_H
#define PAT_INTERFACES__I_RENTRANT_SYSTEMATICS_TOOL_H

#include "AsgTools/IAsgTool.h"

namespace CP
{
  class SystematicSet;
  class SystematicVariation;


  /// \brief interface for all CP tools supporting systematic
  /// variations within a reentrant algorithm
  ///
  /// This is an evolution of the \ref ISystematicsTool interface used
  /// during run 2 with the goal of allowing to use systematics within
  /// an \ref AnaReentrantAlgorithm.  To ease migration (and
  /// interoperating tools with both interfaces) the old interface was
  /// changed to inherit from the new interface.
  ///
  /// The basic change compared to the old interface is that this
  /// interface only allows to query the list of supported
  /// systematics, but does not allow to set the current systematics
  /// as setting the current systematics breaks the reentrancy.
  /// Instead to allow a tool to be used in reentrant code, its
  /// interface needs to be changed so that the functions that vary
  /// with systematics need a `const SystematicSet&` argument that
  /// specifies the current systematic (and the interface base class
  /// should be changed to this class).
  ///
  /// For the implementation, tools should process each
  /// `SystematicSet` value only once, and cache whatever information they
  /// need in a `tbb::concurrent_unordered_map<CP::ISystematicSet,...>`
  /// (or a `std::unordered_map` protected by an `std::mutex`).  The
  /// recommendation/requirement to cache was also true with the old
  /// interface, the only new part is that it needs to be safe for
  /// concurrent access.
  ///
  /// Note that currently (01 Mar 21) there is no requirement for
  /// systematics to be able to run within a reentrant algorithm.
  /// Reentrant algorithms only offer benefits within AthenaMT, and
  /// currently no jobs using systematics use AthenaMT.  In particular
  /// a tool inheriting from ISystematicsTool can still be used in
  /// reentrant algorithms, if systematics are not used.  This
  /// interface is mainly provided in case there is a special need for
  /// a particular systematics tool to run in AthenaMT, or if a CP
  /// group wants to already implement such an interface in case such
  /// a need arises in the future.

  class IReentrantSystematicsTool : virtual public asg::IAsgTool
  {
  public:

    ASG_TOOL_INTERFACE( CP::IReentrantSystematicsTool )

    /// \brief the list of all systematics this tool can be affected by
    virtual SystematicSet affectingSystematics() const = 0;

    /// \brief the list of all systematics this tool recommends to use
    virtual SystematicSet recommendedSystematics() const = 0;
  };
}

#endif
