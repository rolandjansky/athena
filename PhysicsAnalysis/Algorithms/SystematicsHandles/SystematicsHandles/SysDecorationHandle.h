/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_DECORATION_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_DECORATION_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
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

  template<typename T> class SysDecorationHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysDecorationHandle (T2 *owner, const std::string& propertyName,
                             const std::string& propertyValue,
                             const std::string& propertyDescription);


    /// \brief register a set of used systematics for in the current
    /// execut call (usually obtained from \ref CP::SystListHandle)
  public:
    StatusCode preExecute (SysListHandle &systematics);


    /// \brief whether \ref preExecute has been called successfully at least once
  public:
    bool isPrepared () const noexcept;


    /// \brief whether we have a name configured
  public:
    bool empty () const noexcept;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief get the name we retrieve from the event store
  public:
    const std::string& getName (const CP::SystematicSet& sys) const;


    /// \brief retrieve the object decoration for the given systematic
  public:
    const T& get (const SG::AuxElement& object,
                  const CP::SystematicSet& sys) const;

    /// \brief check if the object decoration is available
  public:
    bool isAvailable (const SG::AuxElement& object,
                      const CP::SystematicSet& sys) const;


    /// \brief set the object decoration for the given systematic
  public:
    void set (SG::AuxElement& object, const T& value,
              const CP::SystematicSet& sys) const;



    //
    // inherited interface
    //

  public:
    virtual std::string getInputAffecting () const override;



    //
    // private interface
    //

    /// \brief the value of \ref isPrepared
  private:
    bool m_isPrepared{false};

    /// \brief the input name we use
  private:
    std::string m_inputName;

    /// \brief the regular expression for affecting systematics
  private:
    std::string m_affectingRegex {".*"};

    /// \brief the cache of names we use
  private:
    mutable std::unordered_map<CP::SystematicSet,std::tuple<std::string,SG::AuxElement::ConstAccessor<T>,SG::AuxElement::Accessor<T> > > m_dataCache;

    /// \brief get the data for the given systematics
  private:
    const std::tuple<std::string,SG::AuxElement::ConstAccessor<T>,SG::AuxElement::Accessor<T> >&
    getData (const CP::SystematicSet& sys) const;
  };
}

#include "SysDecorationHandle.icc"

#endif
