/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TIncident.cxx 676314 2015-06-18 12:07:05Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODRootAccess/tools/TIncident.h"

namespace xAOD {

   TIncident::TIncident( ::Int_t type )
      : m_type( type ) {

   }

   ::Int_t TIncident::type() const {

      return m_type;
   }

} // namespace xAOD

/// Helper macro for printing the incident types as a string
#define PRINT_TYPE( TYPE )                      \
   case TYPE:                                   \
      out << #TYPE;                             \
      break;

std::ostream& operator<< ( std::ostream& out, const xAOD::TIncident& inc ) {

   // Print a prefix:
   out << "xAOD::TIncident(";

   // Print the type of the incident:
   switch( inc.type() ) {

      PRINT_TYPE( xAOD::IncidentType::BeginInputFile );
      PRINT_TYPE( xAOD::IncidentType::EndInputFile );
      PRINT_TYPE( xAOD::IncidentType::BeginEvent );
      PRINT_TYPE( xAOD::IncidentType::MetaDataStop );

   default:
      out << "<unknown>";
      break;
   }

   // Print a postfix:
   out << ")";

   // Return the same output stream:
   return out;
}
