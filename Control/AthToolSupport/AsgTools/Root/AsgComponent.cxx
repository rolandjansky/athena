/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// \author Nils Krumnack

//
// includes
//

#include <AsgTools/AsgComponent.h>

//
// method implementations
//

namespace asg
{
  AsgComponent ::
  AsgComponent (const std::string& val_name)
    : AsgMessaging (this)
    , m_name (val_name)
    , m_properties (new PropertyMgr)
  {
    msg().declarePropertyFor (*this);
  }



  AsgComponent ::
  ~AsgComponent () noexcept
  {
    delete m_properties;
  }



  void AsgComponent ::
  addCleanup (const std::shared_ptr<void>& cleanup)
  {
    m_cleanup.push_back (cleanup);
  }



  /// See the comments for PropertyMgr::setProperty to see why this
  /// function specialisation is needed, and why it has this exact form.
  ///
  /// @param name The name of the string property to set
  /// @param value The value of the string property to set
  /// @returns <code>StatusCode::SUCCESS</code> if the call was successful,
  ///          <code>StatusCode::FAILURE</code> otherwise
  ///
  StatusCode AsgComponent ::
  setProperty (const std::string& name, const char *value)
  {
    return m_properties->setProperty (name, value);
  }



  PropertyMgr *AsgComponent ::
  getPropertyMgr () noexcept
  {
     return m_properties;
  }



  const PropertyMgr *AsgComponent ::
  getPropertyMgr() const noexcept
  {
    return m_properties;
  }



  const std::string& AsgComponent ::
  name () const
  {
    return m_name;
  }
}
