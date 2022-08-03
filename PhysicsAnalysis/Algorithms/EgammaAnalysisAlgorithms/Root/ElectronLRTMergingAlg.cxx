/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   ElectronLRTMergingAlg
//
//   Electron merger algorithm merges the standard and LRT electron containers.
//   It uses the ElectronLRTOverlapRemovalTool to remove overlaps.
//   The output merged collection is decorated with isLRT=0/1 to denote
//   if the electron was from the standard or LRT container.  Merging can
//   be output into a transient view container or copied container written out.
///////////////////////////////////////////////////////////////////

#include "EgammaAnalysisAlgorithms/ElectronLRTMergingAlg.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "AsgTools/AsgToolConfig.h"

namespace CP
{
    ElectronLRTMergingAlg::ElectronLRTMergingAlg(const std::string &name, ISvcLocator *svcLoc)
        : EL::AnaAlgorithm(name, svcLoc)
    {
    }

    StatusCode ElectronLRTMergingAlg::initialize()
    {
        // Greet the user:
        ATH_MSG_INFO("Initialising");

        ATH_CHECK(m_promptElectronLocation.initialize());
        ATH_CHECK(m_lrtElectronLocation.initialize());
        ATH_CHECK(m_outElectronLocation.initialize());

        /// if the tool is not user-set, configure the automatic instance via our overlap flag
        if (m_overlapRemovalTool.empty())
        {
            asg::AsgToolConfig config("CP::ElectronLRTOverlapRemovalTool/ElectronLRTOverlapRemovalTool");
            ATH_CHECK(config.setProperty("overlapStrategy", m_ORstrategy.value()));
            ATH_CHECK(config.makePrivateTool(m_overlapRemovalTool));
        }

        // Retrieve the tools
        ATH_CHECK(m_overlapRemovalTool.retrieve());

        // Return gracefully:
        return StatusCode::SUCCESS;
    }


    StatusCode ElectronLRTMergingAlg::execute()
    {

        const EventContext &ctx = Gaudi::Hive::currentContext();

        // Setup containers for output, to avoid const conversions setup two different kind of containers
        std::unique_ptr<ConstDataVector<xAOD::ElectronContainer>> transientContainer = std::make_unique<ConstDataVector<xAOD::ElectronContainer>>(SG::VIEW_ELEMENTS);
        std::unique_ptr<xAOD::ElectronContainer> outputCol = std::make_unique<xAOD::ElectronContainer>();

        // Aux container, if needed
        std::unique_ptr<xAOD::ElectronAuxContainer> outputAuxCol;

        // Assign the aux in the copy case
        if (!m_createViewCollection)
        {
            outputAuxCol = std::make_unique<xAOD::ElectronAuxContainer>();
            outputCol->setStore(outputAuxCol.get());
        }

        // Retrieve electrons from StoreGate
        SG::ReadHandle<xAOD::ElectronContainer> promptCol(m_promptElectronLocation, ctx);
        SG::ReadHandle<xAOD::ElectronContainer> lrtCol(m_lrtElectronLocation, ctx);
        if (!promptCol.isValid())
        {
            ATH_MSG_FATAL("Unable to retrieve xAOD::ElectronContainer, \"" << m_promptElectronLocation << "\", cannot run the LRT electron merger!");
            return StatusCode::FAILURE;
        }
        if (!lrtCol.isValid())
        {
            ATH_MSG_FATAL("Unable to retrieve xAOD::ElectronContainer, \"" << m_lrtElectronLocation << "\", cannot run the LRT electron merger!");
            return StatusCode::FAILURE;
        }


        std::set<const xAOD::Electron *> ElectronsToRemove;
        m_overlapRemovalTool->checkOverlap(*promptCol, *lrtCol, ElectronsToRemove);

        ATH_MSG_DEBUG("Size of overlapping electrons to remove: " << ElectronsToRemove.size());

        // Decorate the electrons with their track type
        static const SG::AuxElement::Decorator<char> isLRT("isLRT"); // false if prompt, true if LRT
        for (const xAOD::Electron *el : *promptCol)
            isLRT(*el) = 0;
        for (const xAOD::Electron *el : *lrtCol)
            isLRT(*el) = 1;

        // merging loop over containers
        if (m_createViewCollection)
        {
            transientContainer->reserve(promptCol->size() + lrtCol->size());

            mergeElectron(*promptCol, transientContainer.get(), ElectronsToRemove);
            mergeElectron(*lrtCol, transientContainer.get(), ElectronsToRemove);
        }
        else
        {
            outputCol->reserve(promptCol->size() + lrtCol->size());

            mergeElectron(*promptCol, outputCol.get(), ElectronsToRemove);
            mergeElectron(*lrtCol, outputCol.get(), ElectronsToRemove);
        }

        SG::WriteHandle<xAOD::ElectronContainer> h_write(m_outElectronLocation, ctx);
        if (m_createViewCollection)
        {
            ATH_CHECK(evtStore()->record(transientContainer.release(), m_outElectronLocation.key()));
        }
        else
        {
            ATH_CHECK(h_write.record(std::move(outputCol), std::move(outputAuxCol)));
        }

        ATH_MSG_DEBUG("Done !");

        return StatusCode::SUCCESS;
    }

    ///////////////////////////////////////////////////////////////////
    // Merge electron collections and remove duplicates, for copy
    ///////////////////////////////////////////////////////////////////
    void ElectronLRTMergingAlg::mergeElectron(const xAOD::ElectronContainer &electronCol,
                                              xAOD::ElectronContainer *outputCol,
                                              const std::set<const xAOD::Electron *> &ElectronsToRemove) const
    {
        // loop over electrons, accept them and add them into association tool
        if (!electronCol.empty())
        {
            ATH_MSG_DEBUG("Size of output electron collection " << electronCol.size());

            static const SG::AuxElement::Decorator<ElementLink<xAOD::ElectronContainer>> originalElectronLink("originalElectronLink");

            // loop over electrons
            for (const auto *const electron : electronCol)
            {
                // add electron into output and check if LRT electron failed overlap check
                if (m_doRemoval && ElectronsToRemove.find(electron) != ElectronsToRemove.end())
                    continue;
                else
                {
                    std::unique_ptr<xAOD::Electron> newElectron = std::make_unique<xAOD::Electron>(*electron);
                    ElementLink<xAOD::ElectronContainer> eLink;
                    eLink.toIndexedElement(electronCol, electron->index());
                    originalElectronLink(*newElectron) = eLink;

                    outputCol->push_back(std::move(newElectron));
                }
            }
            ATH_MSG_DEBUG("Size of merged output electron collection " << outputCol->size());
        }
    }

    ///////////////////////////////////////////////////////////////////
    // Merge electron collections and remove duplicates, for transient
    ///////////////////////////////////////////////////////////////////
    void ElectronLRTMergingAlg::mergeElectron(const xAOD::ElectronContainer &electronCol,
                                              ConstDataVector<xAOD::ElectronContainer> *outputCol,
                                              const std::set<const xAOD::Electron *> &ElectronsToRemove) const
    {
        // loop over electrons, accept them and add them into association tool
        if (!electronCol.empty())
        {
            ATH_MSG_DEBUG("Size of transient electron collection " << electronCol.size());
            // loop over electrons
            for (const auto *const electron : electronCol)
            {
                // add electron into output and check if LRT electron failed overlap check
                if (m_doRemoval && ElectronsToRemove.find(electron) != ElectronsToRemove.end())
                    continue;
                else
                {
                    outputCol->push_back(electron);
                }
            }
            ATH_MSG_DEBUG("Size of transient merged electron collection " << outputCol->size());
        }
    }

}