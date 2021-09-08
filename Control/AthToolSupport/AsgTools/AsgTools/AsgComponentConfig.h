/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__ASG_COMPONENT_CONFIG_H
#define ASG_TOOLS__ASG_COMPONENT_CONFIG_H

#include <map>
#include <memory>
#include <string>
#include <vector>

class StatusCode;

namespace asg
{
  class AsgComponent;
  class AsgToolConfig;
}

namespace asg
{
  /// \brief an object that stores the configuration for an \ref
  /// AsgComponent and is able to create one from it

  class AsgComponentConfig
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
    AsgComponentConfig () = default;


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AsgComponentConfig (const std::string& val_typeAndName);


    /// \brief whether all properties are unset
    /// \par Guarantee
    ///   no-fail
  public:
    bool empty () const noexcept;


    /// \brief the type of the component
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
    void setType (const std::string& val_type);


    /// \brief the name of the component
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
    void setName (const std::string& val_name);


    /// \brief get \ref type and \ref name at the same time
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    std::string typeAndName () const;


    /// \brief set \ref type and \ref name at the same time
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    void setTypeAndName (const std::string& val_typeAndName);


    /// \brief set the given property
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   could not convert to string\n
    ///   out of memory II
  public:
    template<typename T> StatusCode
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
    /// \ref AsgComponentConfig: You need to add it the tool to the
    /// component configuration and then you can set individual
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
    /// configuration done in the component itself.
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
    StatusCode createPrivateTool (const std::string& name,
                                  const std::string& toolType);


    /// \brief add a private tool from the given configuration
    ///
    /// This will ignore the name set in `toolConfig` and use whatever
    /// `name` is given instead.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    StatusCode addPrivateTool (const std::string& name,
                               AsgToolConfig toolConfig);


    /// \brief the array version of \ref createPrivateTool
    ///
    /// This returns the actual name of the tool to allow setting
    /// properties on it.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    std::string createPrivateToolInArray (const std::string& name,
                                          const std::string& toolType);


    /// \brief the array version of \ref addPrivateTool
    ///
    /// This will ignore the name set in `toolConfig` and use whatever
    /// `name` is given instead.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    std::string addPrivateToolInArray (const std::string& name,
                                       AsgToolConfig toolConfig);


#ifdef XAOD_STANDALONE
    /// \brief make a component with the given configuration
    ///
    /// Note that generally users won't call this function.  The
    /// typical workflow is to fully configure this object and then
    /// hand it over to EventLoop which will take care of creating the
    /// actual component.
    ///
    /// An exception may be unit tests.  If this turns out to be too
    /// cumbersome for unit testing we may need to revisit it at that
    /// point.  For now it is unclear whether we'll even be able to do
    /// component unit testing either way.
    ///
    /// Also note that this is a function meant for experts, there
    /// ought to be a wrapper around this function for specific
    /// component types (e.g. for tools or services).  As such the
    /// interface of this function may change without notice.
    ///
    /// Also be warned that the returned component will *not* be
    /// initialized, as the caller may need to perform extra
    /// configuration actions before initialization.  Also
    /// `AsgComponent` currently does not implement an `initialize`
    /// member function.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration errors\n
    ///   component creation/initialization errors
  public:
    template<typename T> StatusCode
    makeComponentExpert (std::unique_ptr<T>& component,
                         const std::string& newCommand,
                         bool nestedNames, std::string prefix) const;
#endif


#ifndef XAOD_STANDALONE
    /// \brief add component configuration to configuration service
    /// (expert only)
    ///
    /// In Athena we are not creating components (we leave that to the
    /// proper Athena services), but instead we load the configuration
    /// values up first, then ask for the component to be created the
    /// normal way.  This makes sure that if we dump the configuration
    /// it will include this component, and as a bonus it avoids the
    /// whole issue of having to deal with Athena factory functions.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   configuration errors
  public:
    StatusCode
    configureComponentExpert (const std::string& prefix,
                              bool nestedNames) const;
#endif



    //
    // private interface
    //

  private:

    /// \brief the value of \ref type
    std::string m_type;

    /// \brief the value of \ref name
    std::string m_name;

    /// \brief the map of (private) tools to create
    std::map<std::string,std::tuple<AsgToolConfig,std::string>> m_privateTools;

    /// \brief the map of (private) tool handle arrays to manage, and
    /// the tools they contain
    std::map<std::string,std::vector<std::string>> m_toolArrays;

    /// \brief the map of property values
    std::map<std::string,std::string> m_propertyValues;


    /// \brief check that the type and name members have the correct format
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   type or name doesn't have proper format
    StatusCode checkTypeName (bool nestedNames) const;


    /// \brief access the configuration for the given subtool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   unknown subtool
    /// \{
    struct AccessSubtoolData final
    {
      AsgToolConfig *config {nullptr};
      std::string prefix;
      std::string name;
    };
    AccessSubtoolData accessSubtool (const std::string& name,
                                     std::size_t split);
    /// \}
  };
}

#include "AsgComponentConfig.icc"

#include <AsgTools/AsgToolConfig.h>

#endif
