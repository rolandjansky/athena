/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYS_SELECTION_HELPERS_SELECTION_READ_HANDLE_H
#define SYS_SELECTION_HELPERS_SELECTION_READ_HANDLE_H

#include <AsgDataHandles/VarHandleKey.h>
#include <AthContainers/AuxElement.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <memory>

class StatusCode;

namespace CP
{
  class ISelectionAccessor;
  class SysListHandle;
  class SystematicSet;


  /// \brief a data handle for reading systematically varied selection
  /// properties from objects

  class SysReadSelectionHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysReadSelectionHandle (T2 *owner, const std::string& propertyName,
                            const std::string& propertyValue,
                            const std::string& propertyDescription);


    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief initialize the accessor
  public:
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle);
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle, SG::AllowEmptyEnum);


    /// \brief get the selection as a bool
  public:
    bool getBool (const SG::AuxElement& element,
                  const CP::SystematicSet& sys) const;



    /// Inherited Members
    /// =================

  public:
    virtual bool empty () const noexcept override;
    virtual std::string getNamePattern () const override;
    virtual CP::SystematicSet
    getInputAffecting (const ISystematicsSvc& svc) const override;
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList) override;



    //
    // private interface
    //

    /// \brief the selection we use
  private:
    std::string m_selection;

    /// \brief the object handle we use
  private:
    const ISysHandleBase *m_objectHandle {nullptr};

    /// \brief the accessor we use
  private:
    std::unique_ptr<ISelectionAccessor> m_accessor;
  };
}

#include "SysReadSelectionHandle.icc"

#endif
