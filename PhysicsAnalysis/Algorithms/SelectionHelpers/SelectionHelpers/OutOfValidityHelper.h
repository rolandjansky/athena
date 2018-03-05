/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SYSTEMATICS_HANDLES__OUT_OF_VALIDITY_HELPER_H
#define SYSTEMATICS_HANDLES__OUT_OF_VALIDITY_HELPER_H

#include <AsgTools/MsgStream.h>
#include <AthContainers/AuxElement.h>
#include <SelectionHelpers/SelectionHelpers.h>
#include <xAODBase/IParticle.h>
#include <memory>

class StatusCode;

namespace CP
{
  class CorrectionCode;

  /// \brief a helper to translate a \ref CP::CorrectionCode into a
  /// \ref ::StatusCode
  ///
  /// The prolem is OutOfValidityRange which does not have an
  /// equivalent in StatusCode and which does not have a unique,
  /// correct handling in all situations.  This helper allows to
  /// configure a variety of behaviors via properties.

  class OutOfValidityHelper final
  {
    /// \brief standard constructor
  public:
    template<typename T>
    OutOfValidityHelper (T *owner, const std::string& propertyName = "outOfValidity",
                         const std::string& propertyDescription = "how to handle out of validity results",
                         const std::string& defaultValue = "FAILURE");


    /// \brief standard initialize
  public:
    ::StatusCode initialize ();

    /// \brief check the correction code and do the proper thing
  public:
    ::StatusCode check (xAOD::IParticle& particle,
                        const CP::CorrectionCode& code,
                        const char *context) const;


    /// \brief either the name of the decoration we use, or a special
    /// keyword for the action we apply when encountering
    /// OutOfValidity
  private:
    std::string m_decoName;

    /// \brief the action to perform on encountering an
    /// OutOfValidityRange
  private:
    enum class Action
    {
      UNDEFINED,
      SUCCESS,
      FAILURE,
      WARNING
    };

    /// \brief the action to take
  private:
    Action m_action {Action::UNDEFINED};

    /// \brief the message stream we use
  private:
    MsgStream *m_msg {nullptr};

    /// \brief the accessor if we apply one
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<SelectionType> > m_accessor;

    /// \brief helper for message macros
  private:
    MsgStream& msg( const MSG::Level lvl ) const;
  };



  template<typename T> OutOfValidityHelper ::
  OutOfValidityHelper (T *owner, const std::string& propertyName,
                       const std::string& propertyDescription,
                       const std::string& defaultValue)
    : m_msg (&owner->msg())
  {
    owner->declareProperty (propertyName, m_decoName = defaultValue,
                            propertyDescription);
  }
}

/// \brief a helper check macro to work with \ref OutOfValidityHelper
#define ANA_CHECK_CORRECTION(helper,object,expr)                \
  { if ((helper).check ((object), (expr), #expr).isFailure())   \
      return StatusCode::FAILURE; }

#endif
