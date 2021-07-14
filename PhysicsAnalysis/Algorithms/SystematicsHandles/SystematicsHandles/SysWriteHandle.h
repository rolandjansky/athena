/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_WRITE_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_WRITE_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgDataHandles/VarHandleKey.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <SystematicsHandles/SysListHandle.h>
#include <memory>
#include <string>
#include <unordered_map>

class StatusCode;

namespace CP
{
  class SystematicSet;

  /// \brief a data handle for writing systematics varied input data

  template<typename T,typename Aux = void> class SysWriteHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysWriteHandle (T2 *owner, const std::string& propertyName,
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


    /// \brief get the name we record to the event store
  public:
    const std::string& getName (const CP::SystematicSet& sys) const;


    /// \brief record the object for the given systematic
  public:
    template<typename X = Aux,typename = std::enable_if_t<std::is_same<X,void>::value>>
    ::StatusCode record (std::unique_ptr<T> object,
                         const CP::SystematicSet& sys) const;


    /// \brief retrieve the object for the given name
  public:
    template<typename X = Aux,typename = std::enable_if_t<!std::is_same<X,void>::value>>
    ::StatusCode record (std::unique_ptr<T> object,
                         std::unique_ptr<Aux> aux,
                         const CP::SystematicSet& sys) const;



    //
    // inherited interface
    //

  private:
    virtual CP::SystematicSet
    getInputAffecting (const ISystematicsSvc& svc) const override;
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList) override;



    //
    // private interface
    //

    /// \brief the output name we use
  private:
    std::string m_outputName;

    /// \brief the cache of names we use
  private:
    std::unordered_map<CP::SystematicSet,std::string> m_outputNameCache;


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

#include "SysWriteHandle.icc"

#endif
