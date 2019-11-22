// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODDATASOURCE_TEST_HELPERS_H
#define XAODDATASOURCE_TEST_HELPERS_H

// System include(s).
#include <iostream>
#include <vector>

/// Helper macro for checking return values
#define CHECK( EXP )                                                    \
   do {                                                                 \
      auto ret = EXP;                                                   \
      if( ! ret.isSuccess() ) {                                         \
         Error( "dataSource_test",                                      \
                XAOD_MESSAGE( "Failed to execute: %s" ), #EXP );        \
         return 1;                                                      \
      }                                                                 \
   } while( false )

namespace std {

   /// Helper print operator
   template< typename FIRST, typename SECOND >
   std::ostream& operator<< ( std::ostream& out,
                              const std::pair< FIRST, SECOND >& pair ) {

      out << "[" << pair.first << ", " << pair.second << "]";
      return out;
   }

   /// Helper print operator
   template< typename T >
   std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i + 1 < vec.size() ) {
            out << ", ";
         }
      }
      out << "]";
      return out;
   }

} // namespace std

#endif // XAODDATASOURCE_TEST_HELPERS_H
