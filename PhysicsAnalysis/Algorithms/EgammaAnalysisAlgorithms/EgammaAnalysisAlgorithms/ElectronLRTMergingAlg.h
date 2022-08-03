/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ElectronLRTMergingAlg
//   Author: Jonathan Long, jonathan.long@cern.ch
//   Algorithm invoking std+lrt electron merger, based on the
//   MuonLRTMergingAlg, author: Max Goblirsch-Kolb
///////////////////////////////////////////////////////////////////

#ifndef ASG_ANALYSIS_ALGORITHMS__ELECTRON_LRT_MERGING_ALGORITHM__H
#define ASG_ANALYSIS_ALGORITHMS__ELECTRON_LRT_MERGING_ALGORITHM__H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <xAODEgamma/ElectronContainer.h>
#include <EgammaAnalysisInterfaces/IElectronLRTOverlapRemovalTool.h>
#include <AsgTools/CurrentContext.h>
#include <AsgTools/ToolHandle.h>

#include "AthContainers/ConstDataVector.h"

#include <AsgTools/PropertyWrapper.h>
#include <AsgDataHandles/WriteHandleKey.h>
#include <AsgDataHandles/ReadHandleKey.h>
#include <AsgDataHandles/WriteHandle.h>
#include <AsgDataHandles/ReadHandle.h>

namespace CP
{
    /// \brief this wraps the Electron LRT collection merger in a CP algorithm
    class ElectronLRTMergingAlg final : public EL::AnaAlgorithm
    {
        /// \brief the standard constructor
    public:
        ElectronLRTMergingAlg(const std::string &name,
                              ISvcLocator *pSvcLocator);

    public:
        StatusCode initialize() override;

    public:
        StatusCode execute() override;

    private:
        ///////////////////////////////////////////////////////////////////
        /** @brief Protected data:                                       */
        ///////////////////////////////////////////////////////////////////

        Gaudi::Property<bool> m_createViewCollection{this, "CreateViewCollection", true, "Create a view to avoid deep copy"}; /** Create a view to avoid deep copy. */

        Gaudi::Property<bool> m_doRemoval{this, "doRemoval", true, "Remove overlapping electrons from output collection."}; /** Remove overlapping electrons from output collection. */

        Gaudi::Property<float> m_ORThreshold{this, "ORThreshold", 0.001, "Delta R threshold for matching in overlap removal."}; /** Delta R threshold for matching in overlap removal. */

        /// allows to pass an overlap removal strategy to the underlying removal tool, without manually configuring said tool.
        /// Advantageous in certain analysis frameworks.
        Gaudi::Property<int> m_ORstrategy{this, "overlapStrategy", CP::IElectronLRTOverlapRemovalTool::defaultStrategy, "Overlap removal strategy to use (0 = default)"};

        SG::ReadHandleKey<xAOD::ElectronContainer> m_promptElectronLocation{this, "PromptElectronLocation", "Electrons", "Prompt electrons to merge"}; /** Standard electron collection to be merged. */

        SG::ReadHandleKey<xAOD::ElectronContainer> m_lrtElectronLocation{this, "LRTElectronLocation", "ElectronsLRT", "LRT electrons to merge"}; /** LRT electron collection to be merged. */

        SG::WriteHandleKey<xAOD::ElectronContainer> m_outElectronLocation{this, "OutputCollectionName", "StdWithLRTElectrons", "Name for combined output collection"}; /** Combined electron collection.   */

        ToolHandle<CP::IElectronLRTOverlapRemovalTool> m_overlapRemovalTool{this, "overlapRemovalTool", "", "Tool used to check overlaps between standard and LRT electrons"}; /** The lrt electron overlap removal tool **/

        ///////////////////////////////////////////////////////////////////
        /** @brief Protected methods:                                    */
        ///////////////////////////////////////////////////////////////////

        /** @brief A routine that merges the electron collections in the copy case. */
        void mergeElectron(const xAOD::ElectronContainer &electronCol,
                           xAOD::ElectronContainer *outputCol,
                           const std::set<const xAOD::Electron *> &LRTElectronsToRemove) const;

        /** @brief A routine that merges the electron collections in the transient case. */
        void mergeElectron(const xAOD::ElectronContainer &electronCol,
                           ConstDataVector<xAOD::ElectronContainer> *outputCol,
                           const std::set<const xAOD::Electron *> &LRTElectronsToRemove) const;
    };
}
#endif