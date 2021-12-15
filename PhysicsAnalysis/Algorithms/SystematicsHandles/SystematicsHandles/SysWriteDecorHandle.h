/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_WRITE_DECOR_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_WRITE_DECOR_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgDataHandles/VarHandleKey.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <AthContainers/AuxElement.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <SystematicsHandles/SysListHandle.h>
#include <string>
#include <type_traits>
#include <unordered_map>

class StatusCode;

namespace CP
{
  class SystematicSet;

  /// \brief the decoration value to use if there is no valid scale
  /// factor decoration
  constexpr float invalidScaleFactor () {return -1;}

  /// \brief the decoration value to use if there is no valid
  /// efficiency decoration
  constexpr float invalidEfficiency () {return -1;}


  /// \brief a data handle for reading systematics varied input data

  template<typename T> class SysWriteDecorHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysWriteDecorHandle (T2 *owner, const std::string& propertyName,
                             const std::string& propertyValue,
                             const std::string& propertyDescription);


    /// \brief whether we have a name configured
  public:
    virtual bool empty () const noexcept override;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;

    /// \brief get the name pattern before substitution
    ///
    /// This is not currently defined for decoration handles and made
    /// private.
  private:
    virtual std::string getNamePattern () const override;


    /// \brief initialize this handle
    /// \{
  public:
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle);
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle, SG::AllowEmptyEnum);
    /// \}


    /// \brief get the name we retrieve from the event store
  public:
    const std::string& getName (const CP::SystematicSet& sys) const;


    /// \brief set the object decoration for the given systematic
  public:
    void set (const SG::AuxElement& object, const T& value,
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

    /// \brief the input name we use
  private:
    std::string m_decorName;

    /// \brief the object handle we use
  private:
    const ISysHandleBase *m_objectHandle {nullptr};

    /// \brief the cache of names we use
  private:
    std::unordered_map<CP::SystematicSet,std::tuple<std::string,SG::AuxElement::Decorator<T> > > m_dataCache;

    /// \brief get the data for the given systematics
  private:
    const auto&
    getData (const CP::SystematicSet& sys) const;
  };
}

#include "SysWriteDecorHandle.icc"

#endif
