/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_CUT_BOOKKEEPER_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_CUT_BOOKKEEPER_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <PMGAnalysisInterfaces/IPMGTruthWeightTool.h>

namespace CP
{
  /// \brief an algorithm for dumping the CutBookkeeper metadata
  /// into a histogram

  class AsgCutBookkeeperAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgCutBookkeeperAlg (const std::string& name, 
                         ISvcLocator* pSvcLocator);

  public:
    /// \brief initialize
    StatusCode initialize () override;

  public:
    /// \brief run once on each file
    StatusCode fileExecute () override;

  public:
    /// \brief finalize
    StatusCode finalize () override;

    /// \brief flag to enable systematics
  private:
    bool m_enableSystematics{false};

    /// \brief the truth weight tool
  private:
    ToolHandle<PMGTools::IPMGTruthWeightTool> m_truthWeightTool;

    /// \brief run number we are processing
  private:
    uint32_t m_runNumber {};

    /// \brief MC channel number we are processing
  private:
    uint32_t m_mcChannelNumber {};

    /// \brief weights helper struct
  private:
    struct WeightsGroup
    {
      float nEventsProcessed {};
      float sumOfWeights {};
      float sumOfWeightsSquared {};
    };

    /// \brief weights map
  private:
    std::unordered_map<size_t, WeightsGroup> m_weights;

    /// \brief the pattern for histogram names
  private:
    std::string m_histPattern {"CutBookkeeper_%DSID%_%RUN%_%SYS%"};
  };
}

#endif
