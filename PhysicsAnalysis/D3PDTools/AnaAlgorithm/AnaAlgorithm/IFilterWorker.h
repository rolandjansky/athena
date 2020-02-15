/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__I_FILTER_WORKER_H
#define ANA_ALGORITHM__I_FILTER_WORKER_H

#include <AnaAlgorithm/Global.h>

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

namespace EL
{
  /// \brief the interface to the filter functions in the algorithm
  /// sequence
  ///
  /// This interface summarizes the subset of the \ref Worker
  /// interface that relates to setting the filter status.

  class IFilterWorker
  {
    /// \brief standard (virtual) destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~IFilterWorker () noexcept = default;


    /// \brief whether the current algorithm passed its filter
    /// criterion for the current event
    /// \par Guarantee
    ///   no-fail
  public:
    virtual bool filterPassed () const noexcept = 0;

    /// \brief set the value of \ref filterPassed
    /// \par Guarantee
    ///   no-fail
  public:
    virtual void setFilterPassed (bool val_filterPassed) noexcept = 0;
  };
}

#endif
