/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/// @author Miha Muskinja

#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_INT_VALUE_SELECTION_TOOL_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_INT_VALUE_SELECTION_TOOL_H

#include <AsgTools/AsgTool.h>
#include <PATCore/IAsgSelectionTool.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <xAODBase/IParticle.h>
#include <string>
#include <vector>

namespace CP
{
    /// \brief an \ref IAsgSelectionTool that cuts on char decorations
    ///
    /// Selects all particles based on an int property (e.g. PDG ID, jet truth label).
    /// Two properties must be set: selectionFlag and acceptedValues. 'selectionFlag'
    /// corresponds to an integer property of the particle accessed through an int
    /// accessor. 'acceptedValues' is a list of integers that pass the selection.
    /// All particles with the selectionFlag inside the acceptedValues are accepted.

    class AsgIntValueSelectionTool final
        : public asg::AsgTool,
          virtual public IAsgSelectionTool
    {
        //
        // public interface
        //

        // Create a proper constructor for Athena
        ASG_TOOL_CLASS(AsgIntValueSelectionTool, IAsgSelectionTool)

        /// \brief standard constructor
        /// \par Guarantee
        ///   strong
        /// \par Failures
        ///   out of memory II
    public:
        AsgIntValueSelectionTool(const std::string &name);

        //
        // inherited interface
        //

        virtual StatusCode initialize() override;

        virtual const Root::TAccept &getTAccept() const override;

        virtual const Root::TAccept &accept(const xAOD::IParticle * /*part*/) const override;

        //
        // private interface
        //

        /// tool properties
        /// \{

        std::string m_selFlag;
        std::vector<int> m_acceptedValues;
        std::vector<std::unique_ptr<ISelectionAccessor>> m_acc_selFlag;
        std::unique_ptr<const SG::AuxElement::Accessor<int>> m_selFlagAccessor{};

        /// \}

        /// \brief the \ref TAccept we are using
    private:
        mutable Root::TAccept m_accept;
    };
} // namespace CP

#endif
