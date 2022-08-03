/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ElectronLRTOverlapRemovalTool.h
//   Author: Jonathan Long, jonathan.long AT cern.ch
//
//   Implements overlap removal between standard and LRT electrons.
///////////////////////////////////////////////////////////////////

#ifndef LRTELECTRONANALYSISTOOLS_ELECTRONLRTOVERLAPREMOVALTOOL_H
#define LRTELECTRONANALYSISTOOLS_ELECTRONLRTOVERLAPREMOVALTOOL_H

#include "EgammaAnalysisInterfaces/IElectronLRTOverlapRemovalTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include <string>
#include <map>

#include "xAODTracking/TrackParticle.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/PropertyWrapper.h>

namespace CP
{

    /** @brief Class-algorithm for electron particle collection merging*/
    class ElectronLRTOverlapRemovalTool : virtual public CP::IElectronLRTOverlapRemovalTool, public asg::AsgTool
    {

    public:
        ///////////////////////////////////////////////////////////////////
        /** @brief Standard algorithm methods:                           */
        ///////////////////////////////////////////////////////////////////

        ElectronLRTOverlapRemovalTool(const std::string &name);
        virtual ~ElectronLRTOverlapRemovalTool() = default;
        ASG_TOOL_CLASS(ElectronLRTOverlapRemovalTool, CP::IElectronLRTOverlapRemovalTool)
        virtual StatusCode initialize();

        /// Check the overlap between the prompt and LRT electron collections.
        /// Saves a set of points to electrons to be removed.
        virtual void checkOverlap(const xAOD::ElectronContainer &promptCollection,
                                  const xAOD::ElectronContainer &lrtCollection,
                                  std::set<const xAOD::Electron *> &ElectronsToRemove) const;

    private:
        Gaudi::Property<int> m_strategy{this, "overlapStrategy", CP::IElectronLRTOverlapRemovalTool::defaultStrategy, "Overlap removal strategy to use (0 = default)"}; /** Allows for setting the overlap removal strategy in case of future variations **/
        Gaudi::Property<float> m_ORThreshold{this, "ORThreshold", 0.001, "Delta R threshold for matching in overlap removal."};                                         /** Delta R threshold for matching in overlap removal. */

        ToolHandle<IAsgElectronLikelihoodTool> m_electronLLHTool{this, "ElectronLLHTool", "", "Electron LLH tool to use for the overlap removal"}; 

    }; // end namespace CP

}
#endif // LRTELECTRONANALYSISTOOLS_ELECTRONLRTOVERLAPREMOVALTOOL_H