/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_TOOLS__STANDALONE_TOOL_HANDLE_H
#define ASG_TOOLS__STANDALONE_TOOL_HANDLE_H

#include <AsgTools/AsgToolConfig.h>

namespace asg
{
  /// \brief an "initializing" ToolHandle for stand-alone applications
  ///
  /// The purpose of this class is to make it easy to configure and
  /// instantiate a tool using the proper factory methods, so that
  /// they can readily be used within standalone applications.  It is
  /// not meant to be used within any tools, algorithms or inside a
  /// regular Athena or EventLoop job in general.  Those should use a
  /// regular `ToolHandle` (possibly combined with `AsgToolConfig` if
  /// a default configuration is desired).
  ///
  /// This is meant to replace the legacy `AnaToolHandle` class as a
  /// drop-in replacement, that avoids a lot of the extra
  /// functionality that can be problematic, in particular it can not
  /// be used as a property and it does not try to manage public tools
  /// behind the scenes.
  ///
  /// Note that the template argument can be `const` qualified, to
  /// force that it can only be accessed via a `const` pointer after
  /// it has been created.
  ///
  /// \warn The cleanup behavior of this class is essentially
  /// undefined, and code should not try to access the tool after this
  /// object goes out of scope, nor should it rely on the tool being
  /// destroyed when this object is.  In AnalysisBase the tool will be
  /// destroyed and removed when the object goes out of scope.  In
  /// Athena the tool will still be owned by the `ToolSvc` and be
  /// released at a later point.

  template<typename T> class StandaloneToolHandle final
  {
    /// Public Members
    /// ==============

  public:
    StandaloneToolHandle () = default;
    StandaloneToolHandle (const std::string& typeAndName)
      : m_config (typeAndName) {}


    /// initialize the tool, will fail if the tool was already
    /// initialized
    StatusCode initialize ();

    /// initialize the tool, will succeed if the tool was already
    /// initialized
    StatusCode retrieve ();

    /// access the wrapped tool
    /// \{
    T *get () {
      checkIsInitialized (); return m_pointer;}
    const T *get () const {
      checkIsInitialized (); return m_pointer;}
    T *operator -> () {
      checkIsInitialized (); return m_pointer;}
    const T *operator -> () const {
      checkIsInitialized (); return m_pointer;}
    T& operator * () {
      checkIsInitialized (); return *m_pointer;}
    const T& operator * () const {
      checkIsInitialized (); return *m_pointer;}
    /// \}

    /// get the contained handle
    ///
    /// Note that this is probably not ideal, but it is kept around
    /// for legacy code that is using it.
    const ToolHandle<std::remove_const_t<T>>& getHandle ();



    /// Forwarding Members From AsgToolConfig
    /// =====================================
    ///
    /// These are mostly direct forwards of the members of \ref
    /// AsgToolConfig to avoid directly exposing the member object to
    /// the user.  It is generally forbidden to try to change the
    /// configuration after the tool has been initialized, but read
    /// accessor may still be called.

  public:

    auto typeAndName () const {
      return m_config.typeAndName();}
    auto name () const {
      return m_config.name();}
    void setTypeAndName (const std::string& typeAndName) {
      checkNotInitialized ();
      m_config.setTypeAndName (typeAndName);}
    template<typename T2>
    StatusCode setProperty (const std::string& name, T2&& value) {
      checkNotInitialized ();
      return m_config.setProperty (name, value);}
    StatusCode createPrivateTool (const std::string& name,
                                  const std::string& toolType) {
      checkNotInitialized ();
      return m_config.createPrivateTool (name, toolType);}



    /// Private Members
    /// ===============

  private:

    /// the cached pointer to the tool
    T *m_pointer {nullptr};

    /// the `ToolHandle` holding the tool after initialization
    ToolHandle<std::remove_const_t<T>> m_toolHandle;

    /// the tool-cleanup pointer
    std::shared_ptr<void> m_cleanup;

    /// the actual configuration of the tool
    AsgToolConfig m_config;


    /// \brief check that we are not initialized yet
    void checkNotInitialized () const;

    /// \brief check that we are initialized, forcing in-place
    /// initialization if necessary
    ///
    /// This is not truly const, but this convention matches the gaudi
    /// ToolHandle behavior.
    void checkIsInitialized () const;
  };



  template<typename T> StatusCode StandaloneToolHandle<T> ::
  retrieve ()
  {
    if (m_pointer == nullptr)
      return initialize ();
    else
      return StatusCode::SUCCESS;
  }



  template<typename T> StatusCode StandaloneToolHandle<T> ::
  initialize ()
  {
    checkNotInitialized ();
    if (m_config.makeTool (m_toolHandle, m_cleanup, true).isFailure())
      return StatusCode::FAILURE;
    m_pointer = &*m_toolHandle;
    return StatusCode::SUCCESS;
  }



  template<typename T> const ToolHandle<std::remove_const_t<T>>& StandaloneToolHandle<T> ::
  getHandle ()
  {
    checkIsInitialized ();
    return m_toolHandle;
  }



  template<typename T> void StandaloneToolHandle<T> ::
  checkNotInitialized () const
  {
    if (m_pointer)
      throw std::logic_error ("trying to configure StandaloneToolHandle after it was initialized/retrieved");
  }



  template<typename T> void StandaloneToolHandle<T> ::
  checkIsInitialized () const
  {
    if (m_pointer == nullptr)
      throw std::logic_error ("trying to use StandaloneToolHandle that was never initialized/retrieved");
  }



  template <typename T>
  std::ostream& operator << (std::ostream& str, const StandaloneToolHandle<T>& obj)
  {
    return str << "StandaloneToolHandle:" << obj.typeAndName();
  }
}

#endif
