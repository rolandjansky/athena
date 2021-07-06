/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ANA_REENTRANT_ALGORITHM_CONFIG_H
#define ANA_ALGORITHM__ANA_REENTRANT_ALGORITHM_CONFIG_H

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

#include <AnaAlgorithm/Global.h>

#include <AsgTools/AsgComponentConfig.h>

namespace EL
{
  struct AlgorithmWorkerData;

  /// \brief an object that can create a \ref AnaReentrantAlgorithm

  class AnaReentrantAlgorithmConfig : public asg::AsgComponentConfig
  {
    //
    // public interface
    //


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AnaReentrantAlgorithmConfig ();


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AnaReentrantAlgorithmConfig (const std::string& val_typeAndName);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AnaReentrantAlgorithmConfig (const AsgComponentConfig& val_config);


    /// \brief Virtual destructor, to make PyROOT happy
    ///
    /// Without it ROOT 6.22+ does not allow Python classes to inherit from this
    /// type.
    ///
  public:
    virtual ~AnaReentrantAlgorithmConfig() = default;


    /// \brief make an algorithm from this configuration
    ///
    /// Note that generally users won't call this function.  The
    /// typical workflow is to fully configure this object and then
    /// hand it over to EventLoop which will take care of creating the
    /// actual algorithm.
    ///
    /// An exception may be unit tests.  If this turns out to be too
    /// cumbersome for unit testing we may need to revisit it at that
    /// point.  For now it is unclear whether we'll even be able to do
    /// algorithm unit testing either way.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration errors\n
    ///   algorithm creation/initialization errors
  public:
    ::StatusCode
    makeAlgorithm (std::unique_ptr<AnaReentrantAlgorithm>& algorithm,
                   const AlgorithmWorkerData& workerData) const;



    //
    // private interface
    //
  };
}

#endif
