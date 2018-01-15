#ifndef ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H
#define ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AnaAlgorithm/Global.h>

#include <map>
#include <memory>
#include <string>

class StatusCode;

#ifndef ROOTCORE
#error only include this header in AnalysisBase
#endif

namespace EL
{
  /// \brief an object that can create a \ref AnaAlgorithm

  class AnaAlgorithmConfig
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


    /// \brief the type of the algorithm
    /// \par Guarantee
    ///   no-fail
  public:
    const std::string& type () const noexcept;

    /// \brief set the value of \ref type
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setType (const std::string val_type) noexcept;


    /// \brief the name of the algorithm
    /// \par Guarantee
    ///   no-fail
  public:
    const std::string& name () const noexcept;

    /// \brief set the value of \ref name
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setName (const std::string val_name) noexcept;


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


    /// \brief set the given property
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   could not convert to string\n
    ///   out of memory II
  public:
    template<typename T> ::StatusCode
    setProperty (const std::string& name, const T& value);


    /// \brief set a given property from a string value
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setPropertyFromString (const std::string& name,
                                const std::string& value);


    /// \brief make an algorithm with the given configuration
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration errors\n
    ///   algorithm creation/initialization errors
  public:
    ::StatusCode makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm) const;



    //
    // private interface
    //

    /// \brief the value of \ref type
  private:
    std::string m_type;

    /// \brief the value of \ref name
  private:
    std::string m_name;

    /// \brief the value of \ref useXAODs
  private:
    bool m_useXAODs = true;

    /// \brief the map of property values
  private:
    std::map<std::string,std::string> m_propertyValues;
  };
}

#include "AnaAlgorithmConfig.icc"

#endif
