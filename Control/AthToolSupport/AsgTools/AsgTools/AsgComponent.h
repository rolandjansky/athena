/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// \author Nils Krumnack

#ifndef ASGTOOLS_ASGCOMPONENT_H
#define ASGTOOLS_ASGCOMPONENT_H

#ifndef XAOD_STANDALONE
#error "This header should only be used in XAOD_STANDALONE"
#else

#include <AsgTools/IAsgComponent.h>
#include <AsgMessaging/AsgMessaging.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/MsgLevel.h>

class Property;
class PropertyMgr;

namespace asg
{
  /// \brief the base class for all configurable components in
  /// stand-alone mode
  ///
  /// This class is meant to unify the creation, configuration and
  /// management of algorithms, tools and services in stand-alone
  /// mode by implementing their common features.
  ///
  /// For the most part this is about messaging (via the base class
  /// \ref AsgMessaging) and configuration.  However given that the
  /// number of component types keeps increasing this still seems like
  /// a worthwhile improvement.

  class AsgComponent : public virtual IAsgComponent,
                       public AsgMessaging
  {
    //
    // public interface
    //

    /// standard constructor
  public:
    AsgComponent (const std::string& val_name);


    /// standard destructor
  public:
    ~AsgComponent();



    //
    // properties interface
    //

    /// \brief declare an algorithm property
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    template<typename T> Property *
    declareProperty (const std::string& name, T& loc,
                     const std::string& doc = "");


    /// \brief set a property with a given value
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   invalid property type
    /// \{
  public:
    template<typename T> ::StatusCode
    setProperty (const std::string& name, T&& value);
    ::StatusCode setProperty (const std::string& name, const char *value);
    /// \}



    //
    // inherited interface
    //

    /// the name of the component
  public:
    virtual const std::string& name() const override;



    //
    // private interface
    //

    // for some reason AsgTool allows to set/override the tool name
    // and be able to get properties/the property manager.  this
    // friend declaration allows that.
    friend class AsgTool;

    /// \brief the name of the component
  private:
    std::string m_name;

    /// \brief the property manager
  private:
    std::unique_ptr<PropertyMgr> m_properties;
  };
}

#include "AsgTools/AsgComponent.icc"

#endif
#endif
