/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYS_SELECTION_HELPERS_SELECTION_WRITE_HANDLE_H
#define SYS_SELECTION_HELPERS_SELECTION_WRITE_HANDLE_H

#include <AsgDataHandles/VarHandleKey.h>
#include <AthContainers/AuxElement.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <SelectionHelpers/ISelectionWriteAccessor.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <memory>

class StatusCode;

namespace CP
{
  class ISelectionWriteAccessor;
  class SysListHandle;
  class SystematicSet;


  /// \brief a data handle for writing systematically varied selection
  /// properties from objects

  class SysWriteSelectionHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysWriteSelectionHandle (T2 *owner, const std::string& propertyName,
                            const std::string& propertyValue,
                            const std::string& propertyDescription);


    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief initialize the accessor
  public:
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle);
    StatusCode initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle, SG::AllowEmptyEnum);


    /// \brief set the selection decoration
  public:
    void setBits (const SG::AuxElement& element,
                  SelectionType selection,
                  const CP::SystematicSet& sys) const;

    /// \brief set the selection decoration
  public:
    void setBool (const SG::AuxElement& element,
                  bool selection,
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
    std::unique_ptr<ISelectionWriteAccessor> m_accessor;
  };
}

#include "SysWriteSelectionHandle.icc"

#endif
