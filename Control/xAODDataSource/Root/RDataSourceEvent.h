// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODDATASOURCE_RDATASOURCEEVENT_H
#define XAODDATASOURCE_RDATASOURCEEVENT_H

// xAOD include(s).
#include "xAODRootAccess/TEvent.h"

// ROOT include(s).
#include <ROOT/RStringView.hxx>

// System include(s).
#include <string>
#include <vector>
#include <utility>
#include <typeinfo>
#include <unordered_map>

namespace xAOD {

   /// Extension to @c xAOD::TEvent, used by @c xAOD::RDataSource
   ///
   /// In order to get access to the internals of @c xAOD::TEvent, and provide
   /// pointers for @c ROOT::RDataFrame in a convenient way, this class provides
   /// a layer between the two codebases.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class RDataSourceEvent final : public TEvent {

   public:
      /// Default constructor
      RDataSourceEvent();

      /// Get the available columm and type names from the input
      std::vector< std::pair< std::string, std::string > > columnAndTypeNames();
      /// Get a "column reader", a pointer to a pointer to the object
      void* columnReader( std::string_view columnName,
                          const std::type_info& typeInfo );
      /// Update all objects in memory for a new event
      void updateObjectsForEntry( Long64_t entry );

   private:
      /// Type of the key used in the internal map of object pointers
      typedef std::pair< std::string, const std::type_info* > Key_t;

      /// Custom hash functor for use with @c Key_t
      class key_hash {
      public:
         /// Function calculating the hash for this type
         std::size_t operator()( const Key_t& key ) const;
      };

      /// Objects served to RDataFrame
      std::unordered_map< Key_t, const void*, key_hash > m_objects;

   }; // class RDataSourceEvent

} // namespace xAOD

#endif // XAODDATASOURCE_RDATASOURCEEVENT_H
