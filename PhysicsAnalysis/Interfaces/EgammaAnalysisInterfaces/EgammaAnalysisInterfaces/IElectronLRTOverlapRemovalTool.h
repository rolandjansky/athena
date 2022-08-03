/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IELECTRONLRTOVERLAPREMOVALTOOL__H
#define IELECTRONLRTOVERLAPREMOVALTOOL__H

// C++ include(s):
#include <set>

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

// Local include(s):

/**
 * @mainpage ElectronLRTOverlapRemovalTool
 *
 */
namespace CP
{

    class IElectronLRTOverlapRemovalTool : public virtual asg::IAsgTool
    {
        ///
        /// @class IElectronLRTOverlapRemovalTool
        /// @brief Interface class.
        /// Abstract interface class. Provides the user interface for the
        /// ElectronLRTOverlapRemovalTool class.

        ASG_TOOL_INTERFACE(CP::IElectronLRTOverlapRemovalTool)

    public:
        /// Allow to specify a number of supported overlap removal strategies.
        /// So far, there is only a single one available.
        typedef enum
        {
            defaultStrategy = 0
        } overlapStrategy;

        /// Check the overlap between the prompt and LRT electron collections.
        /// Saves a set of points to electrons to be removed.
        virtual void checkOverlap(const xAOD::ElectronContainer &promptCollection,
                                  const xAOD::ElectronContainer &lrtCollection,
                                  std::set<const xAOD::Electron *> &ElectronsToRemove) const = 0;
    };
    // class IElectronLRTOverlapRemovalTool

} // namespace CP

#endif /* IELECTRONLRTOVERLAPREMOVALTOOL__H */