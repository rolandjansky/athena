/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H
#define ANA_ALGORITHM__ANA_ALGORITHM_CONFIG_H

#include <AnaAlgorithm/Global.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

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


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AnaAlgorithmConfig (const std::string& val_typeAndName);


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
    void setType (const std::string& val_type) noexcept;


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
    void setName (const std::string& val_name) noexcept;


    /// \brief set \ref type and \ref name at the same time
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    void setTypeAndName (const std::string& val_typeAndName);


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


    /// \brief create a private tool of the given name and type
    ///
    /// This is the only way you can configure tool properties through
    /// \ref AnaAlgorithmConfig: You need to add it the tool to the
    /// algorithm configuration and then you can set individual
    /// properties on the tool, using the property name
    /// "tool.property".
    ///
    /// If you want to add a subtool for a tool, you can do this by
    /// calling it "tool.subtool".  Note that you will have to create
    /// both the tool and the subtool this way, i.e. you can't
    /// configure a subtool without also configuring the tool that
    /// owns it.
    ///
    /// You can apply this to tools held by either \ref ToolHandle or
    /// \ref asg::AnaToolHandle, but if you configure a tool held by
    /// an AnaToolHandle this way it will completely replace teh tool
    /// configuration done in the algorithm itself.
    ///
    /// The calling convention is somewhat inverted compared to
    /// setTypeAndName() calls, but otherwise they would be inverted
    /// compared to the setProperty() calls. :(
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    ::StatusCode createPrivateTool (const std::string& name,
                                    const std::string& toolType);


    /// \brief make an algorithm with the given configuration
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
    makeAlgorithm (std::unique_ptr<AnaAlgorithm>& algorithm) const;



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

    /// \brief the value of \ref isPublicTool
  private:
    bool m_isPublicTool {false};

    /// \brief the map of private tools to create
  private:
    std::map<std::string,std::string> m_privateTools;

    /// \brief the map of property values
  private:
    std::map<std::string,std::string> m_propertyValues;
  };
}

#include "AnaAlgorithmConfig.icc"

#endif
