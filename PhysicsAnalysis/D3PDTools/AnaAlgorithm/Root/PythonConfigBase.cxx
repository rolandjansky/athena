/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/PythonConfigBase.h>

//
// method implementations
//

namespace EL
{
  PythonConfigBase ::
  PythonConfigBase ()
  {}



  PythonConfigBase ::
  PythonConfigBase (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {}



  const std::string& PythonConfigBase ::
  componentType () const noexcept
  {
    return m_componentType;
  }



  void PythonConfigBase ::
  setComponentType (const std::string& val_componentType)
  {
    m_componentType = val_componentType;
  }
}
