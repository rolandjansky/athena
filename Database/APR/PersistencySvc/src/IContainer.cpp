/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistencySvc/IContainer.h"

pool::IContainer::IContainer( const std::string& name ):
  m_name( name )
{}

const std::string&
pool::IContainer::name() const
{
  return m_name;
}
