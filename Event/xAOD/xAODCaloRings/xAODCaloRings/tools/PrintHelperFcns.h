/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PrintHelperFcns.h 667903 2015-05-18 18:59:15Z wsfreund $
#ifndef XAODCALORINGS_TOOLS_PRINTHELPERFCNS_H
#define XAODCALORINGS_TOOLS_PRINTHELPERFCNS_H

// STL includes:
#include <vector>

// Asg includes:
#include "AsgTools/MsgStream.h"
#include "AsgTools/MsgStreamMacros.h"

#define ATH_MSG_LVL_DISP_VAR( lvl, var)                                        \
  do {                                                                         \
    if ( msgLvl(lvl) ) {                                                       \
      msg(lvl) << #var << " : " << var << endreq;                              \
    }                                                                          \
  } while (0)

// =============================================================================
/// Helper operator for printing the contents of vectors (MsgStream)
template <typename T>
MsgStream& operator<< ( MsgStream& out, const std::vector< T >& vec ) 
{

  // A little prefix:
  out << "[";
  // Print the contents:
  for( size_t i = 0; i < vec.size(); ++i ) {
     out << vec[ i ];
     if( i < vec.size() - 1 ) {
        out << ", ";
     }
  }
  // A little postfix:
  out << "]";
  // Return the stream:
  return out;
}

// =============================================================================
/// Helper operator for printing the contents of vector pointers (MsgStream)
template <typename T>
MsgStream& operator<< ( MsgStream& out, const std::vector< T >* vec)
{
  return out.operator<<(*vec);
}

// =============================================================================
/// Helper operator for printing the contents of vectors (ostream)
template <typename T>
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec )
{
  // A little prefix:
  out << "[";
  // Print the contents:
  for( size_t i = 0; i < vec.size(); ++i ) {
     out << vec[ i ];
     if( i < vec.size() - 1 ) {
        out << ", ";
     }
  }
  // A little postfix:
  out << "]";
  // Return the stream:
  return out;
}

// =============================================================================
/// Helper operator for printing the contents of vector pointers (ostream)
template <typename T>
std::ostream& operator<< ( std::ostream& out, const std::vector< T >* vec)
{
  return out.operator<<(*vec);
}


#endif // XAODCALORINGS_TOOLS_PRINTHELPERFCNS_H
