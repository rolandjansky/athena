/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_PT_ETA_SELECTION_TOOL_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_PT_ETA_SELECTION_TOOL_H

#include <AsgTools/AsgTool.h>
#include <PATCore/IAsgSelectionTool.h>
#include <atomic>

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

    virtual const asg::AcceptInfo& getAcceptInfo( ) const override;

    virtual asg::AcceptData accept( const xAOD::IParticle *particle ) const override;



    //
    // private interface
    //

    /// tool properties
    /// \{

  private:
    float m_minPt {0};
    float m_maxPt {0};
    float m_maxEta {0};
    float m_etaGapLow {0};
    float m_etaGapHigh {0};
    bool m_useClusterEta {false};
    bool m_printCastWarning {true};
    bool m_printClusterWarning {true};

    /// \}

    /// Index for the minimum pT selection
    int m_minPtCutIndex{ -1 };
    /// Index for the maximum pT selection
    int m_maxPtCutIndex{ -1 };
    /// Index for the maximum eta selection
    int m_maxEtaCutIndex{ -1 };
    /// Index for the eta gap selection
    int m_etaGapCutIndex{ -1 };
    /// Index for the e/gamma casting
    int m_egammaCastCutIndex{ -1 };
    /// Index for the e/gamma calo-cluster
    int m_egammaClusterCutIndex{ -1 };

    /// \brief a version of \ref m_printCastWarning that we modify
    /// once we printed the warning
    ///
    /// I don't like modifying property values in the tool itself, so
    /// I copy it over here and then modify once I print out.
    ///
    /// Technically this tool isn't thread-safe due to the use of
    /// TAccept, but once we move to master this will be fixed, so
    /// this member is already made thread-safe so that we don't trip
    /// up on that later.
  private:
    mutable std::atomic<bool> m_shouldPrintCastWarning {true};

    /// \brief a version of \ref m_printClusterWarning that we modify
    /// once we printed the warning
    ///
    /// I don't like modifying property values in the tool itself, so
    /// I copy it over here and then modify once I print out.
    ///
    /// Technically this tool isn't thread-safe due to the use of
    /// TAccept, but once we move to master this will be fixed, so
    /// this member is already made thread-safe so that we don't trip
    /// up on that later.
  private:
    mutable std::atomic<bool> m_shouldPrintClusterWarning {true};

    /// \brief the \ref asg::AcceptInfo we are using
  private:
    mutable asg::AcceptInfo m_accept;
  };
}

#endif
