/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__PYTHON_CONFIG_BASE_H
#define ANA_ALGORITHM__PYTHON_CONFIG_BASE_H

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

#include <AnaAlgorithm/Global.h>

#include <AsgTools/AsgComponentConfig.h>

namespace EL
{
  struct AlgorithmWorkerData;

  /// \brief the base class for the python configuration of any \ref
  /// asg::AsgComponent
  ///
  /// Most of the functionality is provided by \ref
  /// asg::AsgComponentConfig, with this class mostly adding one extra
  /// member that allows to the the component type.  Another benefit
  /// of this class is that this is its own type, meaning overload
  /// resolution can tell that an object got set from python.

  class PythonConfigBase : public asg::AsgComponentConfig
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
    PythonConfigBase ();


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit PythonConfigBase (const std::string& val_typeAndName);


    /// \brief Virtual destructor, to make PyROOT happy
    ///
    /// Without it ROOT 6.22+ does not allow Python classes to inherit from this
    /// type.
    ///
  public:
    virtual ~PythonConfigBase () = default;


    /// \brief the actual component type to create
    /// \par Guarantee
    ///   no-fail / strong
    /// \par Failures
    ///   out of memory II [2]
    /// \{
  public:
    const std::string& componentType () const noexcept;
    void setComponentType (const std::string& val_componentType);
    /// \}



    //
    // private interface
    //

    /// \brief the value of \ref componentType
  private:
    std::string m_componentType;
  };
}

#endif
