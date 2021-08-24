/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgDataHandles/VarHandleKey.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <SystematicsHandles/ISystematicsSvc.h>
#include <SystematicsHandles/SysListHandle.h>
#include <string>
#include <tuple>
#include <unordered_map>

class StatusCode;

namespace CP
{
  /// \brief a data handle for copying systematics varied input data

  template<typename T> class SysCopyHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysCopyHandle (T2 *owner, const std::string& propertyName,
                   const std::string& propertyValue,
                   const std::string& propertyDescription);


    /// \brief whether we have a name configured
  public:
    virtual bool empty () const noexcept override;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;

    /// \brief get the name pattern before substitution
  public:
    virtual std::string getNamePattern () const override;


    /// \brief initialize this handle
    /// \{
  public:
    StatusCode initialize (SysListHandle& sysListHandle);
    StatusCode initialize (SysListHandle& sysListHandle, SG::AllowEmptyEnum);
    /// \}


    /// \brief retrieve the object for the given name
  public:
    ::StatusCode getCopy (T*& object,
                          const CP::SystematicSet& sys) const;



    //
    // inherited interface
    //

  private:
    virtual CP::SystematicSet getInputAffecting (const ISystematicsSvc& svc) const override;
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList) override;



    //
    // private interface
    //

    /// \brief the input name we use
  private:
    std::string m_inputName;

    /// \brief the (optional) name of the copy we create
  private:
    std::string m_outputName;

    /// \brief the cache of names we use
  private:
    std::unordered_map<CP::SystematicSet,std::tuple<std::string,std::string,std::string> > m_nameCache;


    /// \brief the type of the event store we use
  private:
    typedef std::decay<decltype(
      *(std::declval<EL::AnaAlgorithm>().evtStore()))>::type StoreType;

    /// \brief the event store we use
  private:
    StoreType *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
    ///
    /// This is an std::function to allow the parent to be either a
    /// tool or an algorithm.  Though we are not really supporting
    /// tools as parents when using \ref SysListHandle, so in
    /// principle this could be replaced with a pointer to the
    /// algorithm instead.
  private:
    std::function<StoreType*()> m_evtStoreGetter;
  };
}

#include "SysCopyHandle.icc"

#endif
