// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SetPrint.h 618129 2014-09-23 11:37:00Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_SETPRINT_H
#define TRIGBUNCHCROSSINGTOOL_SETPRINT_H

// System include(s):
#include <set>
#include <iostream>

// Gaudi/Athena include(s):
#include "AsgMessaging/MsgStream.h"

/// Helper operator for printing std::set variables easily
template< typename T >
std::ostream& operator<< ( std::ostream& out, const std::set< T >& var ) {

   out << "[";
   typename std::set< T >::const_iterator itr = var.begin();
   typename std::set< T >::const_iterator end = var.end();
   for( ; itr != end; ++itr ) {
      out << *itr;
      if( ++itr != end ) {
         out << ", ";
      }
      --itr;
   }
   out << "]";

   return out;
}

/// Helper operator for printing std::set variables easily
template< typename T >
MsgStream& operator<< ( MsgStream& out, const std::set< T >& var ) {

   out << "[";
   typename std::set< T >::const_iterator itr = var.begin();
   typename std::set< T >::const_iterator end = var.end();
   for( ; itr != end; ++itr ) {
      out << *itr;
      if( ++itr != end ) {
         out << ", ";
      }
      --itr;
   }
   out << "]";

   return out;
}

#endif // TRIGBUNCHCROSSINGTOOL_SETPRINT_H
