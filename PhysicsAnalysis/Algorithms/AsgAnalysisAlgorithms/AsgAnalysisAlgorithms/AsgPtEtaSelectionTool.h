/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_PT_ETA_SELECTION_TOOL_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_PT_ETA_SELECTION_TOOL_H

#include <AsgTools/AsgTool.h>
#include <PATCore/IAsgSelectionTool.h>

namespace CP
{
  /// \brief an \ref IAsgSelectionTool that performs basic pt and eta
  /// cut (with an optional eta gap)
  ///
  /// This is a very basic selection that needs to happen on all
  /// object types to some degree.  Instead of doing this separately
  /// for each type, it is just one basic tool for all \ref IParticle
  /// implementations.  Also, this is a tool, not an algorithm,
  /// because we already have an algorithm wrapping generic selection
  /// tools and there is no benefit to making it an algorithm.
  ///
  /// There may be some overlap with the individual selectors for the
  /// given object types, but having a tool for this allows to apply
  /// it at any point in the algorithm sequence.

  class AsgPtEtaSelectionTool final
    : public asg::AsgTool, virtual public IAsgSelectionTool
  {
    //
    // public interface
    //

    // Create a proper constructor for Athena
    ASG_TOOL_CLASS( AsgPtEtaSelectionTool, IAsgSelectionTool )


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AsgPtEtaSelectionTool (const std::string& name);




    //
    // inherited interface
    //

    virtual StatusCode initialize () override;

    virtual const Root::TAccept& getTAccept( ) const override;

    virtual const Root::TAccept& accept( const xAOD::IParticle* /*part*/ ) const override;



    //
    // private interface
    //

    /// tool properties
    /// \{

    float m_minPt {0};
    float m_maxEta {0};
    float m_etaGapLow {0};
    float m_etaGapHigh {0};

    /// \}


    /// \brief the \ref TAccept we are using
  private:
    mutable Root::TAccept m_accept;
  };
}

#endif
