#ifndef ASG_TOOLS__ANA_TOOL_HANDLE_H
#define ASG_TOOLS__ANA_TOOL_HANDLE_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// Author: Will Buttinger


#include <AsgTools/AsgTool.h>
#include <AsgTools/Deprecated.h>
#include <AsgTools/ToolHandle.h>
#include <map>

namespace asg
{
#ifdef ROOTCORE
   typedef asg::AsgTool parentType_t;
#else
   typedef INamedInterface parentType_t;
   typedef IAlgTool interfaceType_t;
#endif


  template<class T> class AnaToolHandle;


  /// \brief standard output operator
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   streaming errors
  template <typename T>
  std::ostream& operator << (std::ostream& str, const AnaToolHandle<T>& obj);


  /// \brief a modified tool handle that allows its owner to configure
  /// new tools from the C++ side
  ///
  /// One of the main differences between RootCore and Athena is that
  /// RootCore lacks a configuration manager.  This means that in
  /// RootCore we have to create and configure all tools on the spot,
  /// which is not (easily) possible in Athena.  This particular tool
  /// handle provides a dual-use way of doing so, while introducing a
  /// number of important safety pre-cautions that are lacking on the
  /// RootCore side.
  ///
  /// The primary (or at least original) purpose of this tool handle
  /// was to support dual-use analysis frameworks.  For these the
  /// actual configuration of the CP tools is a critical component,
  /// and having to configure them separately in RootCore and Athena
  /// is a potential source of rather difficult to track down bugs.
  /// An unfortunate side-effect of this is that we can't use the
  /// Athena configuration mechanism.  Once we have a dual-use
  /// configuration manager it will hopefully provide a more
  /// attractive option.
  ///
  /// A secondary benefit is that this allows to create and configure
  /// tools on the Athena side without having to write configuration
  /// files.  This is particularly beneficial when writing unit tests
  /// where you have to create a lot of rather short-lived tools and
  /// where separate configuration files would be impractical.  It may
  /// also be beneficial for new users who can (at first) be turned
  /// off by the complexity of Athena configuration.
  ///
  ///
  /// Conceptually this tools tries to model an std::unique_ptr or
  /// std::shared_ptr, for which calls to new, setProperty and
  /// initialize have been wrapped to make them dual-use as well as
  /// safer to use.  If a parent tool is specified, the tool is
  /// private (and this tool handle is modeled after unique_ptr).  If
  /// no parent tool is specified, the tool is public/shared (and this
  /// tool handle is modeled after shared_ptr).
  ///
  /// In public/shared mode, if you create and initialize one tool it
  /// is then available for use by other tool handles registered with
  /// the same name and type.  The subsequent tool handles with that
  /// name will then automatically pick up the tool in the
  /// configuration set by the first tool handle.  The user still has
  /// to call initialize() on the tool handle before using the tool.
  /// While it is allowed to call setProperty* method on those tool
  /// handles these calls will be effectively ignored.  The net effect
  /// of that is that as a user of a handle for a shared tool you
  /// don't have to worry about whether you are the one creating the
  /// tool or just using it, you just initialize the tool handle in
  /// the same way.
  ///
  /// The tool handle can also be declared as a property on the parent
  /// tool by using the ToolHandle provided via its handle member
  /// function.  This allows the user to override the configuration
  /// done in the parent tool via the job options file in Athena or
  /// via setProperty in RootCore.  Like for shared tools, initialize
  /// still has to be called and the calls to setProperty get ignored.
  /// This is so that inside the parent tool you don't have to write
  /// any special detection code to see whether the user configured
  /// the AnaToolHandle or if you need to do so yourself.  Instead,
  /// you can just write your code as if the user didn't configure
  /// your tool handle and if the user does, the tool handle will
  /// automatically switch over.

  template<class T>
  class AnaToolHandle final
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief create a tool handle with the given name and parent
    ///
    /// The name can either be just a regular name or of the form
    /// type/name, in which case type indicates the type of the tool
    /// to be created.  If you don't explicitly call make (or pass no
    /// type into it), you have to rely on this form.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AnaToolHandle (const std::string& val_name = "",
			    parentType_t *val_parent = nullptr);


    /// \brief move constructor
    /// \par Guarantee
    ///   no-fail
  public:
    AnaToolHandle (AnaToolHandle<T>&& that) noexcept;


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~AnaToolHandle ();


    /// \brief standard swap
    /// \par Guarantee
    ///   no-fail
  public:
    void swap (AnaToolHandle<T>& that);


    /// \brief standard move assignment operator
    /// \par Guarantee
    ///   no-fail
  public:
    AnaToolHandle<T>& operator = (AnaToolHandle<T>&& that);


    /// \brief make a new tool of the type specified in the
    /// constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   tool creation failures\n
    ///   no tool type specified
    /// \pre tool has not yet been created
  public:
    StatusCode make ();

    /// \brief make a new tool of the given type
    ///
    /// The type name can either be just the type itself or be of the
    /// form type/name, in which case name indicates the name of the
    /// tool to be created.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   tool creation failures
    /// \pre tool has not yet been created
  public:
    StatusCode make (const std::string& typeName);

    /// \brief make a new tool of the given type
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   tool creation failures
    /// \pre tool has not yet been created
    /// \warn this is only for legacy support and even then should
    ///   only be called via the \ref ASG_MAKE_ANA_TOOL macro
  public:
#ifdef ROOTCORE
    template<class T2>
    StatusCode makeNew (std::string val_type);
#endif


    /// \brief set the given property of the tool.
    ///
    /// this will make the tool is not already created.
    ///
    /// if the tool is shared and has already been created, this call
    /// will be ignored.
    ///
    /// this can also be used to set tool handle properties by passing
    /// in either a regular \ref ToolHandle or an \ref AnaToolHandle
    /// object.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   property setting failures
    /// \pre tool has not yet been initialized
  public:
    template <class T2> StatusCode
    setProperty (const std::string& property, const T2& value);

    /// \copydoc setProperty
  public:
    template <class T2> StatusCode
    setProperty (const std::string& property, const ToolHandle<T2>& tool);

    /// \copydoc setProperty
  public:
    template <class T2> StatusCode
    setProperty (const std::string& property, const AnaToolHandle<T2>& tool);


#ifndef ROOTCORE //implementation for cmt releases only so far, I think rootcore releases will already work ~ok
    /// \copydoc setProperty
  public:
    template <class T2> StatusCode
    setProperty (const std::string& property, const ToolHandleArray<T2>& tool);
#endif


    /// \brief initialize the tool
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool initialization failures
    /// \pre tool has not yet initialized
  public:
    StatusCode initialize ();

    /// \brief retrieve (i.e. initialize) the tool
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool initialization failures
    /// \pre tool has not yet initialized
  public:
    StatusCode retrieve ();


    /// \brief access the tool
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
    /// \post result != nullptr
  public:
    T *operator -> () const noexcept;

    /// \brief access the tool
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
  public:
    T& operator * () const noexcept;

    /// \brief access the tool
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
    /// \post result != nullptr
  public:
    T *get () const noexcept;


    /// \brief declare as property on the given tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   property declaration failures\n
    ///   tool already made
  public:
    template<typename T2> void
    declarePropertyFor (T2 *tool, const std::string& name,
			const std::string& description = "");


    /// \brief the type configured for this AnaToolHandle
    /// \par Guarantee
    ///   no-fail
  public:
#ifdef ROOTCORE
    const std::string& type () const noexcept;
#else
    std::string type () const;
#endif

    /// \brief the name configured for this AnaToolHandle
    /// \par Guarantee
    ///   no-fail
  public:
#ifdef ROOTCORE
    const std::string& name () const noexcept;
#else
    std::string name () const;
#endif


    /// \brief the tool handle we wrap
    /// \par Guarantee
    ///   no-fail
  public:
    const ToolHandle<T>& getHandle () const noexcept;


    /// \brief the tool handle we can expose to the configuration
    /// mechanism
    ///
    /// This allows to register this AnaToolHandle like a regular
    /// ToolHandle via declareProperty().  If it is set during
    /// configuration it will then replace anything provided by
    /// calling \ref make or \ref setProperty on this AnaToolHandle.
    /// \par Guarantee
    ///   no-fail
  public:
    ToolHandle<T>& handle ();


    /// \brief whether this tool handle has been configured by the
    /// user
    ///
    /// i.e. somebody called declareProperty with our handle member,
    /// and then that got configured either from the job options file
    /// in Athena or via setProperty in RootCore
    /// \par Guarantee
    ///   no-fail
  public:
    bool isUserConfigured () const noexcept;


    /// \brief whether the tool can be configured by us
    ///
    /// if this refers to a shared tool or has been configured via the
    /// ToolHandle, then we can't configure the tool ourselves.  we
    /// can still call the setProperty methods, they just won't do
    /// anything.
    /// \par Guarantee
    ///   no-fail
  public:
    bool isConfigurable () const;


    /// \brief whether initialize has been called successfully,
    ///   i.e. whether the tool is ready to be used
    /// \par Guarantee
    ///   no-fail
  public:
    bool isInitialized () const noexcept;


    /// \brief whether we are in the state before calling make
    ///
    /// Normally users don't need to check this, but sometimes this
    /// can be worthwhile to check for debugging purposes.
    /// \par Guarantee
    ///   no-fail
  public:
    bool inPremakeState () const noexcept;


    /// \brief whether we are in a broken state
    ///
    /// Normally users don't need to check this, but sometimes this
    /// can be worthwhile to check for debugging purposes.  Normally
    /// tools get into a broken state either because one of the
    /// configuration functions failed, or because the content of this
    /// object was moved to another tool handle.
    /// \par Guarantee
    ///   no-fail
  public:
    bool inBrokenState () const noexcept;



    //
    // private interface
    //

    /// \brief the different states of the tool handle
  private:
    enum class State
    {
      /// \brief the beginning state
      BEGIN,

      /// \brief we are using a pre-initialized tool, i.e. we ignore
      /// all setProperty() requests and initialize() essentially just
      /// unlocks it
      PREINITIALIZED,

      /// \brief the tool has been made, but not initialized
      CREATED,

      /// \brief the tool has been initialized and can be used
      INITIALIZED,

      /// \brief we encountered an error in setProperty
      ///
      /// This is to emulate the Athena behavior in RootCore,
      /// i.e. that setProperty failures only show up when actually
      /// initializing the tool, not when setting properties on it.
      BAD_PROPERTY,

      /// \brief an error has occurred in the tool creation process
      BROKEN
    };

    /// \brief the current \ref State of the tool handle
  private:
    State m_state = State::BEGIN;


    /// \brief the \ref ToolHandle we wrap our tool handle around
    ///
    /// This serves as a backend for a lot of our functions, and we
    /// rely on its properties as much as possible instead of trying
    /// to implement our own.  As such we purposely limit write access
    /// by the user to this handle.  It can be declared as a property
    /// on the parent tool and set during the configuration stage, but
    /// otherwise it is off-limits to the user.
    ///
    /// This is done as a shared pointer, so that the include
    /// dependency is only pulled in for the source file that actually
    /// uses the AnaToolHandle.  That in turn reduces the number of
    /// public dependencies an Athena package has to expose.
  private:
    std::shared_ptr<ToolHandle<T>> m_handle;


    /// \brief the parent of the tool
    ///
    /// in RootCore we can not ask the ToolHandle for the pointer to
    /// the parent, so instead we cache it here and just check in
    /// Athena that it is the same as in the ToolHandle
  private:
    parentType_t *m_parent = nullptr;

    /// \brief the typeAndName at time of creation
    ///
    /// This is used to determine whether the user has been overriding
    /// the ToolHandle during configuration time.
  private:
    std::string m_originalTypeAndName;

    /// \brief the pointer to the tool we use
    ///
    /// This is used for actually accessing the tool, independent of
    /// how we created it.
  private:
    T *m_toolPtr = nullptr;

    /// \brief the value of \ref isUserConfigured cached when we make
    /// the tool
  private:
    bool m_isUserConfigured = false;


    /// \brief do all the checks that need to be done before running
    /// any version of make
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool creation failures\n
    ///   pre-condition failures
  private:
    StatusCode
    preMake (const std::string& val_type);

    /// \brief actually make the tool, if we make our own
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool creation failures\n
    ///   pre-condition failures
  private:
    StatusCode
    doMake ();


    /// \brief do all the checks that need to be done before running
    /// any version of setProperty
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool creation failures\n
    ///   pre-condition failures
  private:
    StatusCode preSetProperty (const std::string& property);



    //
    // private interface for RootCore
    //

#ifdef ROOTCORE
    /// \brief the pointer to the tool used for configuration
  private:
    asg::AsgTool *m_toolConfig = nullptr;

    /// \brief the smart pointer to the tool, if we created it
    /// ourselves
    ///
    /// If we create the tool ourselves, we are responsible for
    /// cleaning it up.  Originally this was a unique_ptr, but I
    /// changed it to a shared pointer, so that it users don't have to
    /// include all the possible tool classes in their code.  Also,
    /// this allows to share a tool between multiple \ref
    /// AnaToolHandle objects.
  private:
    std::shared_ptr<T> m_toolOwn;

    /// \brief the weak pointer referencing shared tools
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \brief !m_name.empty()
  private:
    std::weak_ptr<T>& sharedToolPointer () const;
#endif



    //
    // private interface for Athena
    //

#ifndef ROOTCORE
  private:
    std::vector<std::string> m_addedProperties;

    // \brief this contains a release function we use, so that we can
    // break down on include dependencies
  private:
    std::function<void ()> m_releaseFunction;

  private:
    std::string fullName () const;
#endif



    //
    // legacy interface
    //

    // prevent copying
    AnaToolHandle (const AnaToolHandle<T>&) = delete;
    AnaToolHandle<T>& operator = (const AnaToolHandle<T>&) = delete;


    /// \brief whether this ToolHandle is completely empty
    /// \par Guarantee
    ///   no-fail
  public:
    bool empty() const noexcept;


  private:
    void setType (std::string val_type) {
      setTypeAndName (std::move (val_type), name());}

  private:
    void setName (std::string val_name) {
      setTypeAndName (type(), std::move (val_name));}

  private:
    void setTypeAndName (std::string val_type,
			 std::string val_name) {
      if (val_type.empty())
	m_handle->setTypeAndName (std::move (val_type));
      else if (val_name.empty())
	m_handle->setTypeAndName (std::move (val_name));
      else
	m_handle->setTypeAndName (val_type + "/" + val_name);}

  private:
    void setTypeAndName (std::string val_typeAndName) {
      m_handle->setTypeAndName (std::move (val_typeAndName));}
  };
}

/// \brief create the tool in the given tool handle
#ifdef ROOTCORE
#define ASG_MAKE_ANA_TOOL(handle,type)	\
  (handle).template makeNew<type> (#type)
#else
#define ASG_MAKE_ANA_TOOL(handle,type)	\
  (handle).make (#type)
#endif

#include <AsgTools/AnaToolHandle.icc>


#endif
