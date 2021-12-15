/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// \author Nils Krumnack

#ifndef ASGTOOLS_ASGCOMPONENT_H
#define ASGTOOLS_ASGCOMPONENT_H

#if !defined(XAOD_STANDALONE) && !defined(__CPPCHECK__)
#error "This header should only be used in XAOD_STANDALONE"
#else

#include <AsgMessaging/AsgMessaging.h>
#include <AsgMessaging/INamedInterface.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/MsgLevel.h>
#include <memory>
#include <vector>

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

  class AsgComponent : public virtual INamedInterface,
                       public AsgMessaging
  {
    //
    // public interface
    //

    AsgComponent (const AsgComponent&) = delete;
    AsgComponent& operator = (const AsgComponent&) = delete;

    /// standard constructor
  public:
    AsgComponent (const std::string& val_name);


    /// standard destructor
  public:
    ~AsgComponent() noexcept;


    /// \brief add an object to release when this component gets
    /// destructed
    ///
    /// This is mostly used to attach private tools to the component.
  public:
    void addCleanup (const std::shared_ptr<void>& cleanup);



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
    setProperty (const std::string& name, const T& value);
    ::StatusCode setProperty (const std::string& name, const char *value);
    /// \}


    /// Get a pointer to the property manager
    /// \{
    PropertyMgr *getPropertyMgr () noexcept;
    const PropertyMgr* getPropertyMgr() const noexcept;
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
    ///
    /// This needs to be a simple pointer, or something funny happens
    /// in the dictionaries in some of the CP tools.
  private:
    PropertyMgr *m_properties {nullptr};

    /// \brief a list of objects to clean up when releasing the
    /// component
  private:
    std::vector<std::shared_ptr<void> > m_cleanup;
  };
}

#include "AsgTools/AsgComponent.icc"

#endif
#endif
