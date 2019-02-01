/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__I_HISTOGRAM_WORKER_H
#define ANA_ALGORITHM__I_HISTOGRAM_WORKER_H

#include <AnaAlgorithm/Global.h>

#include <string>
class TH1;
class TObject;

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

namespace EL
{
  /// \brief the interface to histogram storage on the worker
  ///
  /// This interface summarizes the subset of \ref Worker that is
  /// needed for generating outputs.

  class IHistogramWorker
  {
    /// \brief standard (virtual) destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~IHistogramWorker () noexcept = default;


    /// effects: add an object to the output.  the worker takes over
    ///   ownership of the object, but the caller may maintain a
    ///   reference to it
    /// guarantee: basic, argument is always swallowed
    /// failures: out of memory I
    /// requires: output_swallow != 0
    /// warning: so far I placed no requirements on the output
    ///   objects.  I may do that at a later stage though, possibly
    ///   breaking existing code.
  public:
    virtual void addOutput (TObject *output_swallow) = 0;


    /// \brief get the output histogram with the given name
    ///
    /// This is mostly meant, so that I can emulate the Athena
    /// histogram mechanism.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   object not found
    /// \post result != 0
  public:
    virtual TH1 *getOutputHist (const std::string& name) const = 0;
  };
}

#endif
