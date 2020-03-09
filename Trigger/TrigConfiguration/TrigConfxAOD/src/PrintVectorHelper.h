/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_PRINTVECTORHELPER_H
#define TRIGCONFXAOD_PRINTVECTORHELPER_H

namespace {
   /// Helper operator for printing the contents of vectors
   template< typename T >
   MsgStream& operator<< ( MsgStream& out,
                           const std::vector< T >& vec ) {

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
}

#endif
