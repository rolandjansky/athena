/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_ASGCOMPONENT_H
#define ASGTOOLS_ASGCOMPONENT_H

#ifndef XAOD_STANDALONE
#error "This header should only be used in XAOD_STANDALONE"
#else

#include <AsgTools/IAsgComponent.h>
#include <AsgMessaging/AsgMessaging.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/MsgLevel.h>

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
    // inherited interface
    //

    /// the name of the component
  public:
    virtual const std::string& name() const override;



    //
    // private interface
    //

    // for some reason AsgTool allows to set/override the tool name.
    // this friend declaration allows that.
    friend class AsgTool;

    /// \brief the name of the component
  private:
    std::string m_name;
  };
}

#include "AsgTools/AsgComponent.icc"

#endif
#endif
