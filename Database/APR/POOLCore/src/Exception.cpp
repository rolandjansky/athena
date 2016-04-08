/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/Exception.h"


pool::Exception::Exception( const std::string& message,
                            const std::string& methodName,
                            const std::string& moduleName ):
  m_message( message + " ( POOL : \"" + methodName + "\" from \"" + moduleName + "\" )" )
{}
