/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_CUT_BOOKKEEPER_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_CUT_BOOKKEEPER_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>

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

    /// \brief run number we are processing
  private:
    uint32_t m_runNumber {};

    /// \brief the pattern for histogram names
  private:
    std::string m_histPattern {"CutBookkeeper_%DSID%_%RUN%"};

    /// \brief the created histogram
  private:
    TH1 *m_hist{};
  };
}

#endif
