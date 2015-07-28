/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// std includes
#include <sstream>
#include <iomanip>
#include <math.h>

// TrigT1 configuration includes
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/PrescaleSet.h"

// local includes
#include "TrigT1CTP/ItemMap.h"
#include "TrigT1CTP/CTPTriggerItem.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerItemNode.h"

// random number generator
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"


LVL1CTP::ItemMap::ItemMap( const TrigConf::ItemContainer& item_vector, 
                           const TrigConf::PrescaleSet& prescales, 
                           CLHEP::HepRandomEngine* randEngine )
   : m_logger( "ItemMap" )
{
   
   m_logger.send( MSG::VERBOSE, "Created" );
   m_logger.send( MSG::DEBUG, "Creating CTPTriggerItem objects for each TrigConf::TriggerItem object..." );

   for( TrigConf::ItemContainer::const_iterator it = item_vector.begin(); it != item_vector.end(); ++it ) {

      // get postion from ctp id
      unsigned int position = (*it)->ctpId();

      m_map[ *it ] = new CTPTriggerItem( *it, prescales.prescales_float()[ position ] );
      m_map[ *it ]->setItemPos( position );

      if (randEngine != 0) { // change prescale offset randomly if requested
         long random = CLHEP::RandFlat::shootInt(randEngine, m_map[ *it ]->prescale());
         m_map[ *it ]->setPrescaleCounter( random );
	 int32_t cut = TrigConf::PrescaleSet::getCutFromPrescale(  m_map[*it]->prescale() );
         std::ostringstream message;
         message << "REGTEST - random prescale offset for "  << std::setw( 30 ) << ( *it )->name() << ": " 
                 << std::setw( 8 ) << m_map[*it]->prescaleCounter() << "/" << std::setw( 8 ) << m_map[*it]->prescale() <<" (cut="<< cut<<")";
         m_logger.send( MSG::DEBUG, message.str() );
      }

      std::ostringstream message;
      message << "  --> Created object for item with name: " << std::setw( 30 ) << ( *it )->name() << " in the position: "
              << std::setw( 3 ) << m_map[*it]->itemPos() << " with prescale: " << std::setw( 8 ) << m_map[*it]->prescale();
      m_logger.send( MSG::VERBOSE, message.str() );

   }
   m_logger.send( MSG::DEBUG, "Creating objects done" );

}

LVL1CTP::ItemMap::~ItemMap() {

   for( std::map< const TrigConf::TriggerItem*, CTPTriggerItem* >::const_iterator it = m_map.begin(); it != m_map.end(); ++it ) {

      delete it->second;

   }

   m_logger.send( MSG::VERBOSE, "Deleted" );

}

LVL1CTP::CTPTriggerItem*
LVL1CTP::ItemMap::getItem( const TrigConf::TriggerItem* item ) const {
   return m_map.find( item )->second;
}



void
LVL1CTP::ItemMap::updatePrescaleCounters( const ThresholdMap* decisionMap, const InternalTriggerMap* internalTrigger, CLHEP::HepRandomEngine* randEngine ) {

   m_logger.send( MSG::VERBOSE, "Updating prescale counters" );

   for ( std::map< const TrigConf::TriggerItem*, CTPTriggerItem* >::const_iterator it = m_map.begin(); it != m_map.end(); ++it ) {

      const bool decision = CTPTriggerItemNode::evaluate( it->first->topNode(), decisionMap, internalTrigger );

      if ( decision ) {
         if (randEngine != 0) {      // random prescale update
           //long random = CLHEP::RandFlat::shootInt(randEngine, it->second->prescale());
            //randommly create a cut value
            long random = CLHEP::RandFlat::shootInt(randEngine, pow(2,24) );
            it->second->setPrescaleCounter( random );

            std::ostringstream message;
            message << "REGTEST - random prescale for "  << std::setw( 30 ) << it->first->name() 
                    << " (" << std::setw(3) << it->second->itemPos() << "): " 
           << std::setw( 8 ) << /*TrigConf::PrescaleSet::getPrescaleFromCut*/( it->second->prescaleCounter()) << "/" << std::setw( 8 ) << it->second->prescale();
            m_logger.send( MSG::DEBUG, message.str() );
         }else{
           it->second->setPrescaleCounter( it->second->prescaleCounter() + 1 );
         }
      }

   }

   m_logger.send( MSG::VERBOSE, "Updating done" );

   return;

}
