// $Id: SystematicsTool.cxx 676502 2015-06-18 20:14:13Z sfarrell $
//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.

// System include(s):
#include <iostream>

// Infrastructure include(s):
#include "RootCoreUtils/Assert.h"

// Local include(s):
#include "PATInterfaces/SystematicsTool.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"

// Other include(s):
//#include "boost/unordered_map.hpp"

namespace CP {

   SystematicsTool::SystematicsTool()
       : m_appliedSystematics(0) {

      RCU_NEW_INVARIANT( this );
   }

   bool SystematicsTool::
   isAffectedBySystematic( const SystematicVariation& systematic ) const {

      RCU_READ_INVARIANT( this );
      return ( m_affectingSystematics.find( systematic ) !=
               m_affectingSystematics.end() );
   }

   SystematicSet
   SystematicsTool::affectingSystematics() const {

      RCU_READ_INVARIANT( this );
      return m_affectingSystematics;
   }

   SystematicSet
   SystematicsTool::recommendedSystematics() const {

      RCU_READ_INVARIANT( this );
      return m_recommendedSystematics;
   }

   const SystematicSet&
   SystematicsTool::appliedSystematics() const {

      RCU_READ_INVARIANT( this );
      if(m_appliedSystematics == 0){
         std::cerr << "SystematicsTool::appliedSystematics WARNING: "
                   << "no systematic configuration has been applied yet. "
                   << "Returning NULL" << std::endl;
      }
      return *m_appliedSystematics;
   }

   std::string SystematicsTool::appliedSystematicsString () const {

      RCU_READ_INVARIANT( this );
      return m_appliedSystematics->name();
   }

   SystematicCode SystematicsTool::
   applySystematicVariation( const SystematicSet& systConfig ) {

      RCU_CHANGE_INVARIANT( this );

      // First, try to find this input set in the map
      auto itr = m_systFilterMap.find(systConfig);

      // This is a new input set, so we need to filter it
      if(itr == m_systFilterMap.end()){

         // Filter input systematics with the affecting systematics
         SystematicSet myAppliedSystematics;
         if( SystematicSet::
             filterForAffectingSystematics( systConfig,
                                            m_affectingSystematics,
                                            myAppliedSystematics ) !=
             SystematicCode::Ok ) {
            return SystematicCode::Unsupported;
         }

         // Insert the new filtered set onto our filter map
         itr = m_systFilterMap.insert
           (std::make_pair(systConfig, myAppliedSystematics)).first;

      }

      // Apply the filtered systematics
      if( sysApplySystematicVariation( itr->second ) !=
          SystematicCode::Ok ) {
         return SystematicCode::Unsupported;
      }

      // Store the filtered systematics
      m_appliedSystematics = &itr->second;
      return SystematicCode::Ok;
   }

   SystematicCode SystematicsTool::
   addAffectingSystematic( const SystematicVariation& systematic, bool recommended ) {

      RCU_READ_INVARIANT( this );

      SystematicRegistry& reg = SystematicRegistry::getInstance();
      reg.registerSystematic( systematic );
      m_affectingSystematics.insert( systematic );
      if( recommended ) {
         if( addRecommendedSystematic(systematic) != SystematicCode::Ok ) {
            return SystematicCode::Unsupported;
         }
      }
      return SystematicCode::Ok;
   }

   SystematicCode SystematicsTool::
   addRecommendedSystematic( const SystematicVariation& systematic ) {

      RCU_READ_INVARIANT( this );

      SystematicRegistry& reg = SystematicRegistry::getInstance();
      m_recommendedSystematics.insert( systematic );
      if( reg.addSystematicToRecommended( systematic ) !=
          SystematicCode::Ok ) {
         return SystematicCode::Unsupported;
      }
      return SystematicCode::Ok;
   }

   void SystematicsTool::
   addAffectingSystematics( const SystematicSet& systematics ) {

      RCU_READ_INVARIANT( this );

      SystematicRegistry& reg = SystematicRegistry::getInstance();
      m_affectingSystematics.insert( systematics );
      reg.registerSystematics( systematics );
   }

   SystematicCode SystematicsTool::
   addRecommendedSystematics( const SystematicSet& systematics ) {

      RCU_READ_INVARIANT( this );

      SystematicRegistry& reg = SystematicRegistry::getInstance();
      m_recommendedSystematics.insert( systematics );
      if( reg.addSystematicsToRecommended( systematics ) !=
          SystematicCode::Ok ) {
         return SystematicCode::Unsupported;
      }
      return SystematicCode::Ok;
   }

   void SystematicsTool::testInvariant() const {

      RCU_INVARIANT( this != 0 );
   }

} // namespace CP
