/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/// @author Tadej Novak



#ifndef PMG_TOOLS__PMG_DECAY_PRODUCTS_SELECTION_TOOL_H
#define PMG_TOOLS__PMG_DECAY_PRODUCTS_SELECTION_TOOL_H

#include <AsgTools/AsgTool.h>
#include <PATCore/IAsgSelectionTool.h>
#include <xAODTruth/TruthParticle.h>
#include <xAODTruth/TruthParticleContainer.h>


namespace PMGTools
{
  /// \brief an \ref IAsgSelectionTool that select particles
  /// based on the allowed decay chain

  class PMGDecayProductsSelectionTool final
    : public asg::AsgTool, virtual public IAsgSelectionTool
  {
    // Create a proper constructor for Athena
    ASG_TOOL_CLASS (PMGDecayProductsSelectionTool, IAsgSelectionTool)


    /// \brief standard constructor
  public:
    PMGDecayProductsSelectionTool (const std::string& name);




    //
    // inherited interface
    //

    virtual StatusCode initialize () override;

    virtual const asg::AcceptInfo& getAcceptInfo () const override;

    virtual asg::AcceptData accept (const xAOD::IParticle* /*part*/) const override;



    //
    // private interface
    //

    /// \brief Helper function to check for required parent particles
  private:
    asg::AcceptData hasRequiredInitialParent (const xAOD::TruthParticle *truthParticle, asg::AcceptData& acceptData) const;

    /// \brief Helper function to get the number of parent particles
  private:
    size_t getNParents (const xAOD::TruthParticle *truthParticle) const;

    /// \brief Helper function get a parent by index
  private:
    const xAOD::TruthParticle* getParent (const xAOD::TruthParticle *truthParticle,
                                          size_t index) const;

    /// tool properties
    /// \{

  private:
    std::vector<int> m_requiredParentPDGIDs;
    std::vector<int> m_allowedIntermediatePDGIDs;

    /// \}

  private:
    /// \brief Index for the truth particle link
    int m_truthParticleIndex{ -1 };
    /// \brief Index for the required parent particles
    int m_requiredParentIndex{ -1 };

    /// \brief the \ref AcceptInfo we are using
  private:
    asg::AcceptInfo m_accept;

    /// \brief common parents accessor
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::TruthParticleContainer>>>> m_parentsAccessor{};
  };
}

#endif
