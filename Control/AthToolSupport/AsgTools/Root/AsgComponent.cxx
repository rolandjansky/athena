/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

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
  {
  }



  AsgComponent ::
  ~AsgComponent ()
  {
  }



  const std::string& AsgComponent ::
  name () const
  {
    return m_name;
  }
}
