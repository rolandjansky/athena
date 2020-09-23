/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H
#define ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

#include <AnaAlgorithm/Global.h>

#include <AsgTools/AsgComponentConfig.h>

namespace EL
{
  struct AlgorithmWorkerData;

  /// \brief an object that can create a \ref AnaAlgorithm

  class AnaAlgorithmConfig : public asg::AsgComponentConfig
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AnaAlgorithmConfig ();


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AnaAlgorithmConfig (const std::string& val_typeAndName);


    /// \brief Virtual destructor, to make PyROOT happy
    ///
    /// Without it ROOT 6.22+ does not allow Python classes to inherit from this
    /// type.
    ///
  public:
    virtual ~AnaAlgorithmConfig() = default;


    /// \brief whether we use XAODs
    /// \par Guarantee
    ///   no-fail
  public:
    bool useXAODs () const noexcept;

    /// \brief set the value of \ref useXAODs
    /// \par Guarantee
    ///   no-fail
  public:
    void setUseXAODs (bool val_useXAODs) noexcept;


    /// \brief whether we are really a public tool
    /// \par Guarantee
    ///   no-fail
  public:
    bool isPublicTool () const noexcept;

    /// \brief set the value of \ref isPublicTool
    /// \par Guarantee
    ///   no-fail
  public:
    void setIsPublicTool (bool val_isPublicTool) noexcept;


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
    makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm,
                   const AlgorithmWorkerData& workerData) const;



    //
    // private interface
    //

    /// \brief the value of \ref useXAODs
  private:
    bool m_useXAODs = true;

    /// \brief the value of \ref isPublicTool
  private:
    bool m_isPublicTool {false};
  };
}

#endif
